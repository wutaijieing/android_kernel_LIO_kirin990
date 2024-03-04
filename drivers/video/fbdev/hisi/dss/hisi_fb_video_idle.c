/* Copyright (c) 2017-2020, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"

#include "hisi_fb.h"
#include "hisi_fb_video_idle.h"

#define ONLINE_WB_TIMEOUT_COUNT 16600
#define DSS_CLEAR_TIMEOUT 1000
#define BUFFER_RELEASE_LAYER_CNT 3

static void hisifb_video_idle_wb_err_clear(struct hisi_fb_data_type *hisifd)
{
	char __iomem *pipe_sw_wb_base = NULL;
	char __iomem *mctl_sys_base = NULL;
	uint32_t delay_count = 0;
	uint32_t wch0_status;
	uint32_t wback_status;

	/* pipe sw config */
	pipe_sw_wb_base = hisifd->dss_base + DSS_PIPE_SW_WB_OFFSET;
	set_reg(pipe_sw_wb_base + PIPE_SW_SIG_CTRL, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + SW_POS_CTRL_SIG_EN, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + PIPE_SW_DAT_CTRL, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + SW_POS_CTRL_DAT_EN, 0x0, 32, 0);

	mctl_sys_base = hisifd->dss_base + DSS_MCTRL_SYS_OFFSET;
	set_reg(mctl_sys_base + MCTL_MOD_CLK_SEL, 0x1, 1, 14);

	set_reg(mctl_sys_base + MCTL_MOD14_DBG, 0x3, 2, 22); /* wback_debug[23:22] =2'b11 */
	set_reg(mctl_sys_base + MCTL_MOD8_DBG, 0x3, 2, 22); /* wch0_debug[23:22] =2'b11 */

	while (delay_count++ < DSS_CLEAR_TIMEOUT) {
		wch0_status = inp32(mctl_sys_base + MCTL_MOD8_STATUS);
		wback_status = inp32(mctl_sys_base + MCTL_MOD14_STATUS);
		if ((wch0_status & BIT(5)) && (wback_status & BIT(5))) {
			set_reg(mctl_sys_base + MCTL_MOD8_DBG, 0x0, 2, 22); /* wch0_debug[23:22] =2'b11 */
			set_reg(mctl_sys_base + MCTL_MOD14_DBG, 0x0, 2, 22); /* wback_debug[23:22] =2'b11 */
			HISI_FB_INFO("wb err clear succ.\n");
			break;
		}
		udelay(1);  /* 1us */
	}

	if (delay_count > DSS_CLEAR_TIMEOUT)
		HISI_FB_ERR("wb err clear timeout!\n");

	hisi_fb_frame_refresh(hisifd, "videoidle");
}

static void hisifb_video_idle_feature_init(struct hisifb_video_idle_ctrl *video_idle_ctrl)
{
	video_idle_ctrl->gpu_compose_idle_frame = VIDEO_IDLE_GPU_COMPOSE_ENABLE;
	video_idle_ctrl->policy_id = DISPLAY_LB_POLICY_ID;
	video_idle_ctrl->l3cache_size = L3CACHE_AVAILABLE_SIZE;
	video_idle_ctrl->l3_request_size = L3CACHE_REQUEST_SIZE;
	video_idle_ctrl->lb_size = LB_AVAILABLE_SIZE;
}

static void hisifb_video_idle_init(struct hisi_fb_data_type *hisifd)
{
	uint32_t bpp;
	dss_rect_t rect;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;

	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	video_idle_ctrl->hisifd = hisifd;

	video_idle_ctrl->afbc_enable = false;
	video_idle_ctrl->mmu_enable = false;
	video_idle_ctrl->compress_enable = true;

	video_idle_ctrl->buffer_alloced = false;
	video_idle_ctrl->video_idle_wb_status = false;
	video_idle_ctrl->video_idle_rb_status = false;
	video_idle_ctrl->rb_closed_reg.need_recovery = false;
	video_idle_ctrl->idle_frame_display = false;

	video_idle_ctrl->rch_idx = DSS_RCHN_D2;
	video_idle_ctrl->wch_idx = DSS_WCHN_W0;
	video_idle_ctrl->wdma_format = DMA_PIXEL_FORMAT_ARGB_8888;
	video_idle_ctrl->ovl_idx = DSS_OVL0;

	video_idle_ctrl->buf = NULL;
	video_idle_ctrl->buf_size = 0;

	hisifb_video_idle_feature_init(video_idle_ctrl);

	rect.x = 0;
	rect.y = 0;
	rect.w = hisifd->panel_info.xres;
	rect.h = hisifd->panel_info.yres;

	if (video_idle_ctrl->wdma_format == DMA_PIXEL_FORMAT_RGB_565)
		bpp = 2;  /* BIT PER PIEXL, 2 - LCD_RGB565 */
	else if (video_idle_ctrl->wdma_format == DMA_PIXEL_FORMAT_ARGB_8888)
		bpp = 4;  /* BIT PER PIEXL, 4 - LCD_RGBA8888 */
	else
		bpp = 4;

	video_idle_ctrl->wb_buffer_size = ALIGN_UP((uint32_t)rect.w * bpp, DMA_STRIDE_ALIGN) * rect.h;
	HISI_FB_INFO("video_idle_ctrl->wb_buffer_size = 0x%x\n", video_idle_ctrl->wb_buffer_size);

	mipi_ifbc_get_rect(hisifd, &rect);
	video_idle_ctrl->wb_hsize = ALIGN_UP((uint32_t)rect.w, 8);
	video_idle_ctrl->wb_pad_num = ALIGN_UP((uint32_t)rect.w, 8) - rect.w;

	if (video_idle_ctrl->compress_enable)
		video_idle_ctrl->wb_pack_hsize = ALIGN_UP((uint32_t)rect.w, 8) * 3 / 4;  /* 4byte Align */
	else
		video_idle_ctrl->wb_pack_hsize = ALIGN_UP((uint32_t)rect.w, 8);

	video_idle_ctrl->wdfc_pad_hsize = ALIGN_UP(video_idle_ctrl->wb_pack_hsize, 4);
	video_idle_ctrl->wdfc_pad_num = video_idle_ctrl->wdfc_pad_hsize - video_idle_ctrl->wb_pack_hsize;
	video_idle_ctrl->wb_vsize = rect.h;
}

static void hisifb_video_idle_interrupt_config(struct hisi_fb_data_type *hisifd)
{
	uint32_t isr_s1;
	char __iomem *dss_base = NULL;

	dss_base = hisifd->dss_base;
	isr_s1 = BIT_DSS_WB_ERR_INTS;
	outp32(dss_base + GLB_CPU_PDP_INTS, isr_s1);

	isr_s1 = BIT_OFF_WCH0_INTS;
	outp32(dss_base + GLB_CPU_OFF_INTS, isr_s1);

	isr_s1 = BIT_WB_ONLINE_ERR_INTS;
	outp32(dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS, isr_s1);
}

static int hisifb_video_idle_l3cache_request
	(struct hisifb_video_idle_ctrl *video_idle_ctrl)
{
	int ret = 0;

	if (video_idle_ctrl->l3cache_size == 0)
		return ret;

#ifdef CONFIG_HISI_L3CACHE_SHARE
	video_idle_ctrl->request_params.id = DSS_IDLE;
	video_idle_ctrl->request_params.request_size =
		video_idle_ctrl->l3_request_size;
	video_idle_ctrl->request_params.type = PRIVATE;
	ret = l3_cache_request(&(video_idle_ctrl->request_params));
	if (ret)
		HISI_FB_ERR("l3_cache_request fail, ret = %d\n", ret);
#endif

	return ret;
}

static void hisifb_video_idle_l3cache_release
	(struct hisifb_video_idle_ctrl *video_idle_ctrl)
{
	if (video_idle_ctrl->l3cache_size == 0)
		return;

#ifdef CONFIG_HISI_L3CACHE_SHARE
	video_idle_ctrl->release_params.id = DSS_IDLE;
	l3_cache_release(&(video_idle_ctrl->release_params));
#endif
}

static int hisifb_video_idle_lb_request
	(struct hisifb_video_idle_ctrl *video_idle_ctrl,
	struct sg_table *table)
{
	int ret = 0;

	if (video_idle_ctrl->lb_size == 0)
		return ret;

#ifdef CONFIG_HISI_LB
	ret = lb_sg_attach(video_idle_ctrl->policy_id,
		table->sgl, sg_nents(table->sgl));
	if (ret)
		HISI_FB_ERR("lb_sg_attach fail\n");
#endif

	return ret;
}

static void hisifb_video_idle_lb_release
	(struct hisifb_video_idle_ctrl *video_idle_ctrl,
	struct sg_table *table)

{
	if (video_idle_ctrl->lb_size == 0)
		return;

#ifdef CONFIG_HISI_LB
	if (lb_sg_detach(video_idle_ctrl->policy_id,
		table->sgl, sg_nents(table->sgl)))
		HISI_FB_ERR("lb_sg_detach fail\n");
#endif

}

static void hisifb_video_idle_buffer_alloc(struct hisi_fb_data_type *hisifd, uint32_t req_size)
{
	unsigned long buf_addr = 0;
	size_t buf_len;
	struct hisifb_video_idle_ctrl *vic = NULL;

	hisi_check_and_no_retval(!hisifd->pdev, ERR, "fb%d pdev is NULL!\n", hisifd->index);
	vic = &(hisifd->video_idle_ctrl);
	hisi_check_and_no_retval(vic->buffer_alloced, INFO, "fb%d buffer alloced\n", hisifd->index);

	buf_len = roundup(vic->wb_buffer_size, PAGE_SIZE);
	if (hisi_dss_alloc_cma_buffer(buf_len, (dma_addr_t *)&buf_addr, (void **)&vic->wb_buffer_base)) {
		HISI_FB_ERR("fb%d failed to alloc cma buffert!\n", hisifd->index);
		return;
	}
	vic->wb_phys_addr = buf_addr;
	vic->wb_buffer_size = buf_len;

	if (lb_pages_attach(vic->policy_id, phys_to_page(vic->wb_phys_addr),
		vic->wb_buffer_size / PAGE_SIZE)) {
		HISI_FB_ERR("lb_pages_attach fail.\n");
		goto attach_exit;
	}

	if (hisifb_video_idle_l3cache_request(vic)) {
		HISI_FB_ERR("hisifb_video_idle_l3cache_request failed.\n");
		goto l3req_exit;
	}

	vic->wb_vir_addr = hisi_iommu_idle_display_map(__hdss_get_dev(), vic->policy_id, vic->wb_phys_addr,
			vic->wb_buffer_size, vic->l3cache_size, vic->lb_size);
	if (!vic->wb_vir_addr) {
		HISI_FB_ERR("request syscache fail\n");
		goto map_exit;
	}

	vic->buffer_alloced = true;
	vic->mmu_enable = vic->wb_vir_addr ? true : false;

	HISI_FB_DEBUG("fb%d buffer alloc succ, smmu enable = %d.\n", hisifd->index, vic->mmu_enable);
	return;

map_exit:
	hisifb_video_idle_l3cache_release(vic);
l3req_exit:
	(void)lb_pages_detach(vic->policy_id, phys_to_page(vic->wb_phys_addr),
		vic->wb_buffer_size / PAGE_SIZE);
attach_exit:
	hisi_dss_free_cma_buffer(vic->wb_buffer_size, vic->wb_phys_addr, vic->wb_buffer_base);
	vic->wb_buffer_base = 0;
	vic->wb_phys_addr = 0;
	vic->wb_vir_addr = 0;
}

void hisifb_video_idle_buffer_free(struct hisi_fb_data_type *hisifd)
{
	struct hisifb_video_idle_ctrl *vic = NULL;

	hisi_check_and_no_retval(!hisifd, ERR, "hisifd is NULL!\n");

	if (hisifd->index != PRIMARY_PANEL_IDX)
		return;

	if (!is_video_idle_ctrl_mode(hisifd))
		return;

	vic = &(hisifd->video_idle_ctrl);
	hisi_check_and_no_retval(!vic->buffer_alloced, DEBUG, "fb%d buffer already free.\n", hisifd->index);

	(void)hisi_iommu_unmap(__hdss_get_dev(), vic->wb_vir_addr, vic->wb_buffer_size);
	vic->wb_vir_addr = 0;
	hisifb_video_idle_l3cache_release(vic);
	(void)lb_pages_detach(vic->policy_id, phys_to_page(vic->wb_phys_addr), vic->wb_buffer_size / PAGE_SIZE);

	hisi_dss_free_cma_buffer(vic->wb_buffer_size, vic->wb_phys_addr, vic->wb_buffer_base);
	vic->wb_buffer_base = 0;
	vic->wb_phys_addr = 0;
	vic->wb_vir_addr = 0;
	vic->buffer_alloced = false;
	HISI_FB_DEBUG("fb%d buffer free!\n", hisifd->index);
}

static void hisifb_video_idle_wb_clear_by_cpu(struct hisi_fb_data_type *hisifd)
{
	char __iomem *pipe_sw_wb_base = NULL;
	char __iomem *mctl_ov_ien = NULL;
	char __iomem *mctl_mutex_base = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;
	char __iomem *wdma_base = NULL;
	char __iomem *mctl_sys_base = NULL;

	vic = &(hisifd->video_idle_ctrl);
	mctl_sys_base = hisifd->dss_base + DSS_MCTRL_SYS_OFFSET;
	set_reg(mctl_sys_base + MCTL_MOD8_DBG, 0x3, 2, 22);  /* wch0_debug[23:22] =2'b11 */

	wdma_base = hisifd->dss_base + g_dss_module_base[vic->wch_idx][MODULE_DMA];
	set_reg(wdma_base + CH_REG_DEFAULT, 0x1, 32, 0);
	set_reg(wdma_base + CH_REG_DEFAULT, 0x0, 32, 0);

	pipe_sw_wb_base = hisifd->dss_base + DSS_PIPE_SW_WB_OFFSET;
	set_reg(pipe_sw_wb_base + PIPE_SW_SIG_CTRL, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + SW_POS_CTRL_SIG_EN, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + PIPE_SW_DAT_CTRL, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + SW_POS_CTRL_DAT_EN, 0x0, 32, 0);
	set_reg(pipe_sw_wb_base + NXT_SW_NO_PR, 0x0, 32, 0);

	mctl_mutex_base = hisifd->dss_base + g_dss_module_ovl_base[vic->ovl_idx][MODULE_MCTL_BASE];
	set_reg(mctl_mutex_base + MCTL_CTL_MUTEX_WB, 0x0, 32, 0);
	set_reg(mctl_mutex_base + MCTL_CTL_MUTEX_WCH0, 0x0, 32, 0);
	set_reg(mctl_mutex_base + MCTL_CTL_END_SEL, 0xffffbfff, 32, 0);

	mctl_ov_ien = hisifd->dss_base + g_dss_module_base[vic->wch_idx][MODULE_MCTL_CHN_OV_OEN];
	set_reg(mctl_ov_ien, 0x0, 32, 0);
	set_reg(mctl_sys_base + MCTL_MOD8_DBG, 0x0, 2, 22);  /* wch0_debug[23:22] =2'b00 */
}

static void hisifb_video_idle_wb_pipe_set_reg(struct hisi_fb_data_type *hisifd)
{
	char __iomem *wb_base = NULL;
	char __iomem *pipe_sw_base = NULL;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;

	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	/* step1. pipe sw config */
	pipe_sw_base = hisifd->dss_base + DSS_PIPE_SW_WB_OFFSET;
	hisifd->set_reg(hisifd, pipe_sw_base + PIPE_SW_SIG_CTRL, 0x1, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_base + SW_POS_CTRL_SIG_EN, 0x0, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_base + PIPE_SW_DAT_CTRL, 0x1, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_base + SW_POS_CTRL_DAT_EN, 0x1, 32, 0);

	hisifd->set_reg(hisifd, pipe_sw_base + NXT_SW_NO_PR, 0x1, 32, 0);

	/* step2. wb pack config */
	wb_base = hisifd->dss_base + DSS_WB_OFFSET;
	if (video_idle_ctrl->compress_enable)
		hisifd->set_reg(hisifd, wb_base + WB_CTRL, 0x3, 32, 0);
	else
		hisifd->set_reg(hisifd, wb_base + WB_CTRL, 0x2, 32, 0);

	hisifd->set_reg(hisifd, wb_base + WB_WORK_MODE, 0x1, 32, 0);

	hisifd->set_reg(hisifd, wb_base + WB_IMG_HSIZE, DSS_WIDTH(video_idle_ctrl->wb_hsize), 32, 0);
	hisifd->set_reg(hisifd, wb_base + WB_IMG_VSIZE, DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);
	hisifd->set_reg(hisifd, wb_base + WB_PAD_NUM, video_idle_ctrl->wb_pad_num, 32, 0);
}

static void hisifb_video_idle_wch_config(struct hisi_fb_data_type *hisifd)
{
	uint32_t i;
	uint32_t idx;
	uint32_t chn_idx;
	dss_dfc_t *s_dfc = NULL;
	dss_wdma_t *s_wdma = NULL;
	dss_smmu_t *s_smmu = NULL;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;

	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	chn_idx = video_idle_ctrl->wch_idx;

	s_smmu = &(hisifd->dss_module.smmu);
	s_dfc = &(hisifd->dss_module.dfc[chn_idx]);
	s_wdma = &(hisifd->dss_module.wdma[chn_idx]);

	s_dfc->padding_ctl = set_bits32(s_dfc->padding_ctl, BIT(31) | (video_idle_ctrl->wdfc_pad_num << 8), 32, 0);
	s_dfc->disp_size = set_bits32(s_dfc->disp_size,
		(DSS_WIDTH(video_idle_ctrl->wb_pack_hsize) << 16) | DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);

	s_wdma->oft_x0 = set_bits32(s_wdma->oft_x0, 0, 12, 0);
	s_wdma->oft_y0 = set_bits32(s_wdma->oft_y0, 0, 16, 0);
	s_wdma->oft_x1 = set_bits32(s_wdma->oft_x1, DSS_WIDTH(video_idle_ctrl->wdfc_pad_hsize) / 4, 12, 0);
	s_wdma->oft_y1 = set_bits32(s_wdma->oft_y1, DSS_HEIGHT(video_idle_ctrl->wb_vsize), 16, 0);

	s_wdma->ctrl = set_bits32(s_wdma->ctrl, (video_idle_ctrl->mmu_enable ? 0x1 : 0x0), 1, 8);
	s_wdma->ctrl = set_bits32(s_wdma->ctrl, video_idle_ctrl->wdma_format, 5, 3);

	if (video_idle_ctrl->mmu_enable)
		s_wdma->data_addr = set_bits32(s_wdma->data_addr, video_idle_ctrl->wb_vir_addr, 32, 0);
	else
		s_wdma->data_addr = set_bits32(s_wdma->data_addr, video_idle_ctrl->wb_phys_addr, 32, 0);

	s_wdma->stride0 = set_bits32(s_wdma->stride0, video_idle_ctrl->wdfc_pad_hsize / 4, 13, 0);
	s_wdma->ch_ctl = set_bits32(s_wdma->ch_ctl, 0x1, 32, 0);

	s_smmu->smmu_smrx_ns_used[chn_idx] = 1;
	for (i = 0; i < g_dss_chn_sid_num[chn_idx]; i++) {
		idx = g_dss_smmu_smrx_idx[chn_idx] + i;
		hisi_check_and_no_retval((idx >= SMMU_SID_NUM), ERR, "idx is invalid");

		if (video_idle_ctrl->mmu_enable) {
			/* smr_bypass */
			s_smmu->smmu_smrx_ns[idx] = set_bits32(s_smmu->smmu_smrx_ns[idx], 0x0, 1, 0);
			/* smr_invld_en */
			s_smmu->smmu_smrx_ns[idx] = set_bits32(s_smmu->smmu_smrx_ns[idx], 0x1, 1, 4);
		} else {
			s_smmu->smmu_smrx_ns[idx] = set_bits32(s_smmu->smmu_smrx_ns[idx], 0x1, 1, 0);
		}
	}
}

static void hisifb_video_idle_wch0_config(struct hisi_fb_data_type *hisifd,
	uint32_t chn_idx, struct hisifb_video_idle_ctrl *video_idle_ctrl)
{
	char __iomem *wdma_base = NULL;
	dss_wdma_t *s_wdma = NULL;

	s_wdma = &(hisifd->dss_module.wdma[chn_idx]);
	wdma_base = hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_DMA];
	hisifd->set_reg(hisifd, wdma_base + CH_REG_DEFAULT, 0x1, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + CH_REG_DEFAULT, 0x0, 32, 0);

	hisifd->set_reg(hisifd, wdma_base + DMA_OFT_X0, s_wdma->oft_x0, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_OFT_Y0, s_wdma->oft_y0, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_OFT_X1, s_wdma->oft_x1, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_OFT_Y1, s_wdma->oft_y1, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_CTRL, s_wdma->ctrl, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_DATA_ADDR0, s_wdma->data_addr, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_STRIDE0, s_wdma->stride0, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + CH_CTL, s_wdma->ch_ctl, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + DMA_BUF_SIZE,
		(s_wdma->oft_y1 << 16) | DSS_WIDTH(video_idle_ctrl->wdfc_pad_hsize), 32, 0);
	hisifd->set_reg(hisifd, wdma_base + ROT_SIZE,
		(s_wdma->oft_y1 << 16) | DSS_WIDTH(video_idle_ctrl->wdfc_pad_hsize), 32, 0);
}

static void hisifb_video_idle_wch0_dfc_config(struct hisi_fb_data_type *hisifd,
	uint32_t chn_idx, struct hisifb_video_idle_ctrl *video_idle_ctrl)
{
	char __iomem *dfc_base = NULL;
	dss_dfc_t *s_dfc = NULL;

	s_dfc = &(hisifd->dss_module.dfc[chn_idx]);
	dfc_base = hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_DFC];
	hisifd->set_reg(hisifd, dfc_base + DFC_PIX_IN_NUM, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_PADDING_CTL, s_dfc->padding_ctl, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_DISP_SIZE, s_dfc->disp_size, 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_DISP_FMT, (DFC_PIXEL_FORMAT_ARGB_8888 << 1), 32, 0);
	hisifd->set_reg(hisifd, dfc_base + DFC_ICG_MODULE, 0x1, 32, 0); /* enable clock */

	hisifd->set_reg(hisifd, hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_AIF0_CHN], AXI_CHN0, 1, 0);

	if (video_idle_ctrl->mmu_enable)
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL1, BIT(17), 32, 0);
	else
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL1, BIT(17) | BIT(5), 32, 0);
}

static void hisifb_video_idle_wch_set_reg(struct hisi_fb_data_type *hisifd)
{
	uint32_t ovl_idx;
	uint32_t chn_idx;
	char __iomem *mctl_ov_ien = NULL;
	char __iomem *mctl_chn_flush_en = NULL;
	char __iomem *mctl_mutex_base = NULL;
	dss_smmu_t *s_smmu = NULL;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;

	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	chn_idx = video_idle_ctrl->wch_idx;
	ovl_idx = video_idle_ctrl->ovl_idx;

	/* step1. mctl config */
	mctl_mutex_base = hisifd->dss_base + g_dss_module_ovl_base[ovl_idx][MODULE_MCTL_BASE];
	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_MUTEX_WB, 0x1, 32, 0);
	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_MUTEX_WCH0, 0x1, 32, 0);

	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_END_SEL, 0xffffbeff, 32, 0);

	mctl_ov_ien = hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_MCTL_CHN_OV_OEN];
	hisifd->set_reg(hisifd, mctl_ov_ien, 0x3, 32, 0);  /* mctl_sys 0x1A0 */

	/* step2. wch0 config */
	hisifb_video_idle_wch0_config(hisifd, chn_idx, video_idle_ctrl);

	/* step3. wch0-dfc config */
	hisifb_video_idle_wch0_dfc_config(hisifd, chn_idx, video_idle_ctrl);

	/* step4. smmu config */
	s_smmu = &(hisifd->dss_module.smmu);
	hisi_dss_smmu_ch_set_reg(hisifd, hisifd->dss_module.smmu_base, s_smmu, chn_idx);  /* 0x1d */

	/* step5. mctl sys config */
	mctl_chn_flush_en = hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_MCTL_CHN_FLUSH_EN];
	hisifd->set_reg(hisifd, mctl_chn_flush_en, 0x1, 32, 0);
}

static void get_hisifb_video_idle_rb_regs(struct hisi_fb_data_type *hisifd,
	struct hisifb_video_idle_ctrl *vic,
	struct idle_rb_closed_reg closed_reg_tmp)
{
	vic->rb_closed_reg.dsc_en = closed_reg_tmp.dsc_en;
	vic->rb_closed_reg.dpp_clip_en = closed_reg_tmp.dpp_clip_en;
	vic->rb_closed_reg.dither_ctl0 = closed_reg_tmp.dither_ctl0;
	vic->rb_closed_reg.gama_en = closed_reg_tmp.gama_en;
	vic->rb_closed_reg.xcc_en = closed_reg_tmp.xcc_en;
	vic->rb_closed_reg.degama_en = closed_reg_tmp.degama_en;
	vic->rb_closed_reg.gmp_en = closed_reg_tmp.gmp_en;
	vic->rb_closed_reg.arsr_post_bypass = closed_reg_tmp.arsr_post_bypass;
	vic->rb_closed_reg.ifbc_en = closed_reg_tmp.ifbc_en;
	vic->rb_closed_reg.hiace_bypass = closed_reg_tmp.hiace_bypass;
	vic->rb_closed_reg.nr_bypass = closed_reg_tmp.nr_bypass;

	HISI_FB_DEBUG("fb%d, dsc_en[0x7dc08]=0x%x, dpp_clip_en[0x70190]=0x%x, dither_ctl0[0x70204]=0x%x\n"
		"gama_en[0x70600]=0x%x, xcc_en[0x70934]=0x%x, degama_en[0x70950]=0x%x, gmp_en[0x709A0]=0x%x\n"
		"arsr_post_mode[0x70A24]=0x%x, ifbc_en[0x7d818]=0x%x, hiace_bypass[0x6f008]=0x%x, "
		"hiace_bypass[0x6f400]=0x%x\n",
		hisifd->index, closed_reg_tmp.dsc_en, closed_reg_tmp.dpp_clip_en, closed_reg_tmp.dither_ctl0,
		closed_reg_tmp.gama_en, closed_reg_tmp.xcc_en, closed_reg_tmp.degama_en, closed_reg_tmp.gmp_en,
		closed_reg_tmp.arsr_post_bypass, closed_reg_tmp.ifbc_en, closed_reg_tmp.hiace_bypass,
		closed_reg_tmp.nr_bypass);
}

static void hisifb_video_idle_config_dpp_regs(struct hisi_fb_data_type *hisifd)
{
	struct idle_rb_closed_reg closed_reg_tmp = {0};
	char __iomem *dss_base = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;

	vic = &(hisifd->video_idle_ctrl);
	dss_base = hisifd->dss_base;
	/*lint -e529*/
	closed_reg_tmp.dsc_en = inp32(dss_base + DSS_DSC_OFFSET + DSC_EN);
	closed_reg_tmp.dpp_clip_en = inp32(dss_base + DSS_DPP_CLIP_OFFSET + DPP_CLIP_EN);
	closed_reg_tmp.dither_ctl0 = inp32(dss_base + DSS_DPP_DITHER_OFFSET + DITHER_CTL0);
	closed_reg_tmp.gama_en = inp32(dss_base + DSS_DPP_GAMA_OFFSET + GAMA_EN);
	closed_reg_tmp.xcc_en = inp32(dss_base + DSS_DPP_XCC_OFFSET + XCC_EN);
	closed_reg_tmp.degama_en = inp32(dss_base + DSS_DPP_DEGAMMA_OFFSET + DEGAMA_EN);
	closed_reg_tmp.gmp_en = inp32(dss_base + DSS_DPP_GMP_OFFSET + GMP_EN);
	closed_reg_tmp.arsr_post_bypass = inp32(dss_base + DSS_POST_SCF_OFFSET + ARSR_POST_MODE);
	closed_reg_tmp.hiace_bypass = inp32(dss_base + DSS_HI_ACE_OFFSET + DPE_BYPASS_ACE);
	closed_reg_tmp.nr_bypass = inp32(dss_base + DSS_HI_ACE_OFFSET + DPE_BYPASS_NR);
	/*lint +e529*/

	get_hisifb_video_idle_rb_regs(hisifd, vic, closed_reg_tmp);

	/* NOTICE: IFBC & DPP, these configed in initial */
	closed_reg_tmp.dsc_en = set_bits32(closed_reg_tmp.dsc_en, 0x0, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DSC_OFFSET + DSC_EN, closed_reg_tmp.dsc_en, 32, 0);

	closed_reg_tmp.dpp_clip_en = set_bits32(closed_reg_tmp.dpp_clip_en, 0x0, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DPP_CLIP_OFFSET + DPP_CLIP_EN, closed_reg_tmp.dpp_clip_en, 32, 0);

	closed_reg_tmp.dither_ctl0 = set_bits32(closed_reg_tmp.dither_ctl0, 0x0, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DPP_DITHER_OFFSET + DITHER_CTL0, closed_reg_tmp.dither_ctl0, 32, 0);

	closed_reg_tmp.gama_en = set_bits32(closed_reg_tmp.gama_en, 0x0, 2, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DPP_GAMA_OFFSET + GAMA_EN, closed_reg_tmp.gama_en, 32, 0);

	closed_reg_tmp.xcc_en = set_bits32(closed_reg_tmp.xcc_en, 0x0, 2, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DPP_XCC_OFFSET + XCC_EN, closed_reg_tmp.xcc_en, 32, 0);

	closed_reg_tmp.degama_en = set_bits32(closed_reg_tmp.degama_en, 0x0, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DPP_DEGAMMA_OFFSET + DEGAMA_EN, closed_reg_tmp.degama_en, 32, 0);

	closed_reg_tmp.gmp_en = set_bits32(closed_reg_tmp.gmp_en, 0x0, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_DPP_GMP_OFFSET + GMP_EN, closed_reg_tmp.gmp_en, 32, 0);

	closed_reg_tmp.arsr_post_bypass = set_bits32(closed_reg_tmp.arsr_post_bypass, 0x1, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_POST_SCF_OFFSET + ARSR_POST_MODE,
		closed_reg_tmp.arsr_post_bypass, 32, 0);

	closed_reg_tmp.hiace_bypass = set_bits32(closed_reg_tmp.hiace_bypass, 0x1, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_HI_ACE_OFFSET + DPE_BYPASS_ACE, closed_reg_tmp.hiace_bypass, 32, 0);

	closed_reg_tmp.nr_bypass = set_bits32(closed_reg_tmp.nr_bypass, 0x1, 1, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_HI_ACE_OFFSET + DPE_BYPASS_NR, closed_reg_tmp.nr_bypass, 32, 0);

	vic->rb_closed_reg.need_recovery = true;
}

static void hisifb_video_idle_wb_clear(struct hisi_fb_data_type *hisifd)
{
	char __iomem *pipe_sw_wb_base = NULL;
	char __iomem *mctl_ov_ien = NULL;
	char __iomem *mctl_mutex_base = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;
	char __iomem *wdma_base = NULL;
	char __iomem *mctl_sys_base = NULL;

	vic = &(hisifd->video_idle_ctrl);

	mctl_sys_base = hisifd->dss_base + DSS_MCTRL_SYS_OFFSET;
	hisifd->set_reg(hisifd, mctl_sys_base + MCTL_MOD8_DBG, 0x3, 2, 22);  /* wch0_debug[23:22] =2'b11 */

	wdma_base = hisifd->dss_base + g_dss_module_base[vic->wch_idx][MODULE_DMA];
	hisifd->set_reg(hisifd, wdma_base + CH_REG_DEFAULT, 0x1, 32, 0);
	hisifd->set_reg(hisifd, wdma_base + CH_REG_DEFAULT, 0x0, 32, 0);

	pipe_sw_wb_base = hisifd->dss_base + DSS_PIPE_SW_WB_OFFSET;
	hisifd->set_reg(hisifd, pipe_sw_wb_base + PIPE_SW_SIG_CTRL, 0x0, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_wb_base + SW_POS_CTRL_SIG_EN, 0x0, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_wb_base + PIPE_SW_DAT_CTRL, 0x0, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_wb_base + SW_POS_CTRL_DAT_EN, 0x0, 32, 0);
	hisifd->set_reg(hisifd, pipe_sw_wb_base + NXT_SW_NO_PR, 0x0, 32, 0);

	mctl_mutex_base = hisifd->dss_base + g_dss_module_ovl_base[vic->ovl_idx][MODULE_MCTL_BASE];
	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_MUTEX_WB, 0x0, 32, 0);
	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_MUTEX_WCH0, 0x0, 32, 0);
	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_END_SEL, 0xffffbfff, 32, 0);

	mctl_ov_ien = hisifd->dss_base + g_dss_module_base[vic->wch_idx][MODULE_MCTL_CHN_OV_OEN];
	hisifd->set_reg(hisifd, mctl_ov_ien, 0x0, 32, 0);
	hisifd->set_reg(hisifd, mctl_sys_base + MCTL_MOD8_DBG, 0x0, 2, 22);  /* wch0_debug[23:22] =2'b00 */
}

static void set_aif_ar_cache_reg(struct hisi_fb_data_type *hisifd,
	struct hisifb_video_idle_ctrl *video_idle_ctrl,
	uint32_t chn_idx)
{
	hisifd->set_reg(hisifd, hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_AIF0_CHN], 0xf000, 32, 0);
}

static void set_mmu_and_mif_reg(struct hisi_fb_data_type *hisifd,
	struct hisifb_video_idle_ctrl *video_idle_ctrl,
	uint32_t chn_idx)
{
	int idx = 0;
	uint32_t i = 0;
	uint32_t smmu_scr_value = 0;
	char __iomem *smmu_base = NULL;

	if (video_idle_ctrl->mmu_enable) {
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL1, BIT(19), 32, 0);
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL2, 0, 32, 0);
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL3, 0, 32, 0);
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL4, 0, 32, 0);
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL5, 0, 32, 0);

	} else {
		hisifd->set_reg(hisifd, hisifd->dss_base +
			g_dss_module_base[chn_idx][MODULE_MIF_CHN] + MIF_CTRL1, 1, 1, 5);
	}

	smmu_base = hisifd->dss_base + DSS_SMMU_OFFSET;
	for (i = 0; i < g_dss_chn_sid_num[chn_idx]; i++) {
		idx = g_dss_smmu_smrx_idx[chn_idx] + i;
		hisi_check_and_no_retval((idx >= SMMU_SID_NUM), ERR, "idx is invalid\n");
		smmu_scr_value = inp32(smmu_base + SMMU_SMRx_NS + idx * 0x4);  /*lint !e529*/
		if (video_idle_ctrl->mmu_enable) {
			smmu_scr_value = set_bits32(smmu_scr_value, 0x1, 1, 9);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x1, 2, 7);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x1, 2, 5);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x0, 1, 1);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x1, 1, 4);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x3, 2, 2);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x0, 1, 0);
		} else {
			smmu_scr_value = set_bits32(smmu_scr_value, 0x1, 1, 9);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x0, 2, 7);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x0, 2, 5);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x0, 1, 1);
			smmu_scr_value = set_bits32(smmu_scr_value, 0x1, 1, 0);  /* bypass */
		}
		hisifd->set_reg(hisifd, smmu_base + SMMU_SMRx_NS + idx * 0x4, smmu_scr_value, 32, 0);
	}
}

static void set_rch_dma_reg(struct hisi_fb_data_type *hisifd,
	struct hisifb_video_idle_ctrl *video_idle_ctrl,
	uint32_t chn_idx)
{
	char __iomem *rdma_base = NULL;

	rdma_base = hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_DMA];
	hisifd->set_reg(hisifd, rdma_base + CH_REG_DEFAULT, 0x1, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + CH_REG_DEFAULT, 0x0, 32, 0);

	hisifd->set_reg(hisifd, rdma_base + DMA_OFT_X0, 0, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_OFT_Y0, 0, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_OFT_X1, DSS_WIDTH(video_idle_ctrl->wdfc_pad_hsize) / 4, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_OFT_Y1, DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_STRETCH_SIZE_VRT, DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_CTRL, ((video_idle_ctrl->mmu_enable ? 0x1 : 0x0) << 8) |
		(video_idle_ctrl->wdma_format << 3), 32, 0);
	if (video_idle_ctrl->mmu_enable)
		hisifd->set_reg(hisifd, rdma_base + DMA_DATA_ADDR0, video_idle_ctrl->wb_vir_addr, 32, 0);
	else
		hisifd->set_reg(hisifd, rdma_base + DMA_DATA_ADDR0, video_idle_ctrl->wb_phys_addr, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_STRIDE0, video_idle_ctrl->wdfc_pad_hsize / 4, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + CH_CTL, (0xF << 12) | 0x1, 32, 0);
	hisifd->set_reg(hisifd, rdma_base + DMA_BUF_SIZE,
		(DSS_HEIGHT(video_idle_ctrl->wb_vsize) << 16) | DSS_WIDTH(video_idle_ctrl->wdfc_pad_hsize), 32, 0);
}

static void set_rch_dfc_reg(struct hisi_fb_data_type *hisifd,
	struct hisifb_video_idle_ctrl *video_idle_ctrl,
	uint32_t chn_idx)
{
	char __iomem *rdfc_base = NULL;

	rdfc_base = hisifd->dss_base + g_dss_module_base[chn_idx][MODULE_DFC];
	hisifd->set_reg(hisifd, rdfc_base + DFC_DISP_SIZE,
		(DSS_WIDTH(video_idle_ctrl->wdfc_pad_hsize) << 16) | DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);
	hisifd->set_reg(hisifd, rdfc_base + DFC_CLIP_CTL_HRZ, video_idle_ctrl->wdfc_pad_num, 32, 0);
	hisifd->set_reg(hisifd, rdfc_base + DFC_PIX_IN_NUM, 0, 32, 0);
	hisifd->set_reg(hisifd, rdfc_base + DFC_DISP_FMT, DFC_PIXEL_FORMAT_ARGB_8888 << 1, 32, 0);
	hisifd->set_reg(hisifd, rdfc_base + DFC_CTL_CLIP_EN, 0x1, 32, 0);
	hisifd->set_reg(hisifd, rdfc_base + DFC_ICG_MODULE, 0x1, 32, 0);
}
static void hisifb_video_idle_rch_set_reg(struct hisi_fb_data_type *hisifd)
{
	int ret;
	uint32_t ovl_idx;
	uint32_t chn_idx;
	char __iomem *mctl_mutex_base = NULL;
	char __iomem *mctl_sys_base = NULL;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;
	union cmd_flag flag = {
		.bits.pending = 0,
		.bits.task_end = 0,
		.bits.last = 0,
	};

	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	ovl_idx = video_idle_ctrl->ovl_idx;
	chn_idx = video_idle_ctrl->rch_idx;

	hisifd->cmdlist_idx = hisi_get_cmdlist_idx_by_chnidx(chn_idx);
	ret = hisi_cmdlist_add_new_node(hisifd, 0x1 << (uint32_t)hisifd->cmdlist_idx, flag, 0);
	hisi_check_and_no_retval(ret, ERR, "fb%d, hisi_cmdlist_add_new_node err: %d\n", hisifd->index, ret);

	/* step1. aif ar cache */
	set_aif_ar_cache_reg(hisifd, video_idle_ctrl, chn_idx);

	/* step2. mmu/mif config */
	set_mmu_and_mif_reg(hisifd, video_idle_ctrl, chn_idx);

	/* step3. rch dma config */
	set_rch_dma_reg(hisifd, video_idle_ctrl, chn_idx);

	/* step4. rch dfc config */
	set_rch_dfc_reg(hisifd, video_idle_ctrl, chn_idx);

	/* step5. mctl mutex config */
	mctl_mutex_base = hisifd->dss_base + g_dss_module_ovl_base[ovl_idx][MODULE_MCTL_BASE];
	hisifd->set_reg(hisifd, mctl_mutex_base + MCTL_CTL_MUTEX_RCH0 + chn_idx * 0x4, 0x1, 32, 0);

	/* step6. mctl sys config */
	mctl_sys_base = hisifd->dss_base + DSS_MCTRL_SYS_OFFSET;
	hisifd->set_reg(hisifd, mctl_sys_base + MCTL_RCH0_OV_OEN + chn_idx * 0x4, 0x100, 32, 0);
	hisifd->set_reg(hisifd, mctl_sys_base + MCTL_RCH0_FLUSH_EN + chn_idx * 0x4, 0x1, 32, 0);
}

static void hisifb_video_idle_ovl_config(struct hisi_fb_data_type *hisifd, uint32_t ovl_idx,
	struct hisifb_video_idle_ctrl *video_idle_ctrl)
{
	char __iomem *dss_base = NULL;
	char __iomem *ovl_base = NULL;

	dss_base = hisifd->dss_base;
	ovl_base = dss_base + g_dss_module_ovl_base[ovl_idx][MODULE_OVL_BASE];
	hisifd->set_reg(hisifd, ovl_base + OV8_REG_DEFAULT, 0x1, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV8_REG_DEFAULT, 0x0, 32, 0);

	hisifd->set_reg(hisifd, ovl_base + OV_SIZE,
		(DSS_HEIGHT(video_idle_ctrl->wb_vsize) << 16) | DSS_WIDTH(video_idle_ctrl->wb_pack_hsize), 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_BG_COLOR_RGB, 0x0, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_BG_COLOR_A, 0x0, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_DST_STARTPOS, 0x0, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_DST_ENDPOS,
		(DSS_HEIGHT(video_idle_ctrl->wb_vsize) << 16) | DSS_WIDTH(video_idle_ctrl->wb_pack_hsize), 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_GCFG, 0x10001, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_LAYER0_POS, 0x0, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_LAYER0_SIZE,
		(DSS_HEIGHT(video_idle_ctrl->wb_vsize) << 16) | DSS_WIDTH(video_idle_ctrl->wb_pack_hsize), 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_LAYER0_ALPHA_MODE, 0x4000, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_LAYER0_ALPHA_A, 0x3ff03ff, 32, 0);
	hisifd->set_reg(hisifd, ovl_base + OV_LAYER0_CFG, 0x1, 32, 0);
}

static void hisifb_video_idle_ovl_set_reg(struct hisi_fb_data_type *hisifd)
{
	int ret;
	uint32_t ovl_idx;
	uint32_t chn_idx;
	char __iomem *mctl_sys_base = NULL;
	char __iomem *dbuf_base = NULL;
	char __iomem *dss_base = NULL;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;
	union cmd_flag flag = {
		.bits.pending = 0,
		.bits.task_end = 0,
		.bits.last = 0,
	};


	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	ovl_idx = video_idle_ctrl->ovl_idx;
	chn_idx = video_idle_ctrl->rch_idx;
	dss_base = hisifd->dss_base;

	hisifd->cmdlist_idx = DSS_CMDLIST_OV0;
	ret = hisi_cmdlist_add_new_node(hisifd, 0x1 << DSS_CMDLIST_OV0, flag, 0);
	hisi_check_and_no_retval(ret, ERR, "fb%d, hisi_cmdlist_add_new_node err:%d\n", hisifd->index, ret);

	/* step1. pipe sw disconnect */
	hisifb_video_idle_wb_clear(hisifd);

	/* step2. ov config */
	hisifb_video_idle_ovl_config(hisifd, ovl_idx, video_idle_ctrl);

	/* step3. dbuf unpack & post clip */
	dbuf_base = dss_base + DSS_WB_UNPACK_OFFSET;
	hisifd->set_reg(hisifd, dbuf_base + DBUF_POST_CLIP_DISP_SIZE,
		(DSS_WIDTH(video_idle_ctrl->wb_hsize) << 16) | DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_POST_CLIP_CTL_HRZ, video_idle_ctrl->wb_pad_num, 32, 0);
	if (video_idle_ctrl->compress_enable)
		hisifd->set_reg(hisifd, dbuf_base + DBUF_UNPACK_IMG_FMT, 0x1, 32, 0);
	else
		hisifd->set_reg(hisifd, dbuf_base + DBUF_UNPACK_IMG_FMT, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_WB_UNPACK_EN, 0x1, 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_WB_UNPACK_SIZE,
		(DSS_WIDTH(video_idle_ctrl->wb_pack_hsize) << 16) | DSS_HEIGHT(video_idle_ctrl->wb_vsize), 32, 0);

	/* step4. dsc bypass & dpp bypass */
	hisifb_video_idle_config_dpp_regs(hisifd);

	/* step5. mctl sys flush en */
	mctl_sys_base = dss_base + DSS_MCTRL_SYS_OFFSET;
	hisifd->set_reg(hisifd, mctl_sys_base + MCTL_RCH_OV0_SEL, 0xFFFFFF0E, 32, 0);  /* DSS_RCHN_D2 */
	hisifd->set_reg(hisifd, mctl_sys_base + MCTL_OV0_FLUSH_EN, 0xD, 32, 0);
}

static void hisifb_video_idle_cmdlist_init(struct hisi_fb_data_type *hisifd,
	uint32_t *cmdlist_pre_idxs, uint32_t *cmdlist_idxs)
{
	uint32_t cmdlist_idxs_temp = 0;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;

	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	cmdlist_idxs_temp |= (1 << (video_idle_ctrl->ovl_idx + DSS_CMDLIST_OV0));
	cmdlist_idxs_temp |= (1 << video_idle_ctrl->rch_idx);
	*cmdlist_idxs = cmdlist_idxs_temp;
	*cmdlist_pre_idxs &= (~(*cmdlist_idxs));
}

static int enter_hisifb_video_idle_rb(struct hisi_fb_data_type *hisifd,
	struct hisifb_video_idle_ctrl *vic)
{
	int count = 0;
	int time = 16000;  /* 16ms */
	uint32_t isr_wch;
	uint32_t isr_wb;

	isr_wch = inp32(hisifd->dss_base + GLB_CPU_OFF_INTS);  /*lint !e529*/
	isr_wb = inp32(hisifd->dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS);  /*lint !e529*/
	while (((isr_wch & BIT_OFF_WCH0_INTS) != BIT_OFF_WCH0_INTS) &&
		((isr_wb & BIT_WB_ONLINE_ERR_INTS) != BIT_WB_ONLINE_ERR_INTS) &&
		(count < ONLINE_WB_TIMEOUT_COUNT)) {
		time = time / 2;
		if (time >= 1000) {  /* 1ms */
			udelay(time);
		} else {
			udelay(1);
			time = 1;
		}
		isr_wch = inp32(hisifd->dss_base + GLB_CPU_OFF_INTS);  /*lint !e529*/
		isr_wb = inp32(hisifd->dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS);  /*lint !e529*/
		count += time;
	}
	outp32(hisifd->dss_base + GLB_CPU_OFF_INTS, BIT_OFF_WCH0_INTS);
	outp32(hisifd->dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS, BIT_WB_ONLINE_ERR_INTS);

	if ((isr_wb & BIT_WB_ONLINE_ERR_INTS) == BIT_WB_ONLINE_ERR_INTS) {
		enable_ldi(hisifd);
		HISI_FB_INFO("BIT_WB_ONLINE_ERR_INTS return, %d us.\n", count);
		hisifb_video_idle_wb_err_clear(hisifd);
		return -1;
	}

	if (count == ONLINE_WB_TIMEOUT_COUNT) {
		enable_ldi(hisifd);
		HISI_FB_INFO("ONLINE_WB_TIMEOUT return, : %d us.\n", count);
		hisifb_video_idle_wb_err_clear(hisifd);
		return -1;
	}
	__flush_dcache_area(vic->wb_buffer_base, vic->wb_buffer_size);
	HISI_FB_INFO("enter video idle success! time: %d us.\n", count);
	return 0;
}

static int hisifb_video_idle_rb_config(struct hisi_fb_data_type *hisifd)
{
	int ret;
	uint32_t cmdlist_pre_idxs = 0;
	uint32_t cmdlist_idxs = 0;
	dss_overlay_block_t *pov_h_block_infos = NULL;
	dss_overlay_block_t *pov_h_block = NULL;
	dss_overlay_t *pov_req_prev = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;

	vic = &(hisifd->video_idle_ctrl);
	/* step1. wait wb start */
	ret = hisi_vactive0_start_config(hisifd, &(hisifd->ov_req_prev));
	if (ret != 0) {
		HISI_FB_ERR("fb%d, hisi_vactive0_start_config failed! ret = %d\n", hisifd->index, ret);
		hisifb_video_idle_wb_err_clear(hisifd);
		return -1;
	}
	disable_ldi(hisifd);
	outp32(hisifd->dss_base + GLB_CPU_OFF_INTS, BIT_OFF_WCH0_INTS);
	outp32(hisifd->dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS, BIT_WB_ONLINE_ERR_INTS);

	pov_req_prev = &(hisifd->ov_req_prev);
	hisifd->set_reg = hisi_cmdlist_set_reg;

	hisi_cmdlist_data_get_online(hisifd);

	hisi_cmdlist_get_cmdlist_idxs(pov_req_prev, &cmdlist_pre_idxs, NULL);

	hisifb_video_idle_cmdlist_init(hisifd, &cmdlist_pre_idxs, &cmdlist_idxs);

	hisi_cmdlist_add_nop_node(hisifd, cmdlist_pre_idxs, 0, 0);
	hisi_cmdlist_add_nop_node(hisifd, cmdlist_idxs, 0, 0);

	hisi_dss_prev_module_set_regs(hisifd, pov_req_prev, cmdlist_pre_idxs, g_enable_ovl_cmdlist_online, NULL);
	(void)hisi_dss_module_init(hisifd);

	hisifb_video_idle_rch_set_reg(hisifd);
	hisifb_video_idle_ovl_set_reg(hisifd);

	g_online_cmdlist_idxs |= cmdlist_idxs;

	/* add taskend for share channel */
	hisi_cmdlist_add_nop_node(hisifd, cmdlist_idxs, 0, 0);

	/* remove ch cmdlist */
	hisi_cmdlist_config_stop(hisifd, cmdlist_pre_idxs);

	cmdlist_idxs |= cmdlist_pre_idxs;
	hisi_cmdlist_flush_cache(hisifd, cmdlist_idxs);

	if (enter_hisifb_video_idle_rb(hisifd, vic) != 0)
		return -1;

	hisi_cmdlist_config_start(hisifd, vic->ovl_idx, cmdlist_idxs, 0);
	enable_ldi(hisifd);

	if (g_debug_ovl_cmdlist)
		hisi_cmdlist_dump_all_node(hisifd, NULL, cmdlist_idxs);

	hisifd->frame_count++;
	pov_req_prev->ov_block_nums = 1;
	pov_req_prev->ovl_idx = DSS_OVL0;
	pov_h_block_infos = (dss_overlay_block_t *)(uintptr_t)pov_req_prev->ov_block_infos_ptr;
	pov_h_block = &(pov_h_block_infos[0]);
	pov_h_block->layer_nums = 1;
	pov_h_block->layer_infos[0].chn_idx = vic->rch_idx;
	pov_h_block->layer_infos[0].layer_idx = 0;

	return ret;
}

static void hisifb_video_idle_workqueue_handler(struct work_struct *work)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;

	vic = container_of(work, typeof(*vic), video_idle_ctrl_work);
	hisi_check_and_no_retval(!vic, ERR, "video_idle_ctrl is NULL\n");

	hisifd = vic->hisifd;
	hisi_check_and_no_retval(!hisifd, ERR, "hisifd is NULL\n");

	down(&hisifd->blank_sem);

	if (!hisifd->ov_req.video_idle_status) {
		HISI_FB_INFO("-----> not in idle.\n");
		up(&hisifd->blank_sem);
		return;
	}

	if (!vic->video_idle_wb_status) {
		HISI_FB_INFO("-----> not online wb.\n");
		up(&hisifd->blank_sem);
		return;
	}

	if (!hisifd->panel_power_on) {
		HISI_FB_INFO("fb%d, panel is power off!\n", hisifd->index);
		up(&hisifd->blank_sem);
		return;
	}

	if (!vic->video_idle_rb_status) {
		if (!hisifb_video_idle_rb_config(hisifd)) {
			vic->video_idle_wb_status = false;
			vic->video_idle_rb_status = true;
		}
	}
	up(&hisifd->blank_sem);
}

static void hisifb_video_idle_rb_clear(struct hisi_fb_data_type *hisifd)
{
	char __iomem *dbuf_base = NULL;
	char __iomem *dss_base = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;

	vic = &(hisifd->video_idle_ctrl);
	dss_base = hisifd->dss_base;
	hisifd->set_reg(hisifd, dss_base + DSS_VBIF0_AIF + ARCACH0_ADDR_THRH, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_VBIF0_AIF + ARCACH0_ADDR_THRL, 0xFFFFFFFF, 32, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_VBIF0_AIF + ARCACH0_VALUE0, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dss_base + DSS_VBIF0_AIF + ARCACH0_VALUE1, 0x0, 32, 0);

	dbuf_base = dss_base + DSS_WB_UNPACK_OFFSET;
	hisifd->set_reg(hisifd, dbuf_base + DBUF_UNPACK_IMG_FMT, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_WB_UNPACK_EN, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_POST_CLIP_DISP_SIZE, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_POST_CLIP_CTL_HRZ, 0x0, 32, 0);
	hisifd->set_reg(hisifd, dbuf_base + DBUF_WB_UNPACK_SIZE, 0x0, 32, 0);

	/* recovery IFBC & DPP configeration */
	if (vic->rb_closed_reg.need_recovery) {
		hisifd->set_reg(hisifd, dss_base + DSS_DSC_OFFSET + DSC_EN,
			vic->rb_closed_reg.dsc_en, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_DPP_CLIP_OFFSET + DPP_CLIP_EN,
			vic->rb_closed_reg.dpp_clip_en, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_DPP_DITHER_OFFSET + DITHER_CTL0,
			vic->rb_closed_reg.dither_ctl0, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_DPP_GAMA_OFFSET + GAMA_EN,
			vic->rb_closed_reg.gama_en, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_DPP_XCC_OFFSET + XCC_EN,
			vic->rb_closed_reg.xcc_en, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_DPP_DEGAMMA_OFFSET + DEGAMA_EN,
			vic->rb_closed_reg.degama_en, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_DPP_GMP_OFFSET + GMP_EN,
			vic->rb_closed_reg.gmp_en, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_POST_SCF_OFFSET + ARSR_POST_MODE,
			vic->rb_closed_reg.arsr_post_bypass, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_HI_ACE_OFFSET + DPE_BYPASS_ACE,
			vic->rb_closed_reg.hiace_bypass, 32, 0);

		hisifd->set_reg(hisifd, dss_base + DSS_HI_ACE_OFFSET + DPE_BYPASS_NR,
			vic->rb_closed_reg.nr_bypass, 32, 0);

		vic->rb_closed_reg.need_recovery = false;
	}
}
irqreturn_t hisifb_video_idle_dss_wb_isr(int irq, void *ptr)
{
	uint32_t isr_s1;
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisifb_video_idle_ctrl *video_idle_ctrl = NULL;

	hisifd = (struct hisi_fb_data_type *)ptr;
	if (!hisifd) {
		HISI_FB_ERR("hisifd is NULL\n");
		return IRQ_NONE;
	}
	video_idle_ctrl = &(hisifd->video_idle_ctrl);
	isr_s1 = inp32(hisifd->dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS);
	outp32(hisifd->dss_base + DSS_WB_OFFSET + WB_ONLINE_ERR_INTS, isr_s1);
	if ((isr_s1 & BIT_WB_ONLINE_ERR_INTS) == BIT_WB_ONLINE_ERR_INTS)
		schedule_work(&(video_idle_ctrl->wb_err_work));

	return IRQ_HANDLED;
}

void hisifb_video_idle_ctrl_register(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;

	if (!pdev) {
		HISI_FB_ERR("pdev is NULL\n");
		return;
	}
	hisifd = platform_get_drvdata(pdev);
	if (!hisifd) {
		dev_err(&pdev->dev, "hisifd is NULL\n");
		return;
	}

	vic = &(hisifd->video_idle_ctrl);
	if (vic->idle_ctrl_created) {
		HISI_FB_INFO("video_idle_ctrl had registered!\n");
		return;
	}

	if (!is_video_idle_ctrl_mode(hisifd)) {
		HISI_FB_INFO("do not support video idle!\n");
		return;
	}

	if (hisifd->index != PRIMARY_PANEL_IDX) {
		HISI_FB_INFO("hisifd is not primary panel!\n");
		return;
	}

	hisifb_video_idle_init(hisifd);

	INIT_WORK(&vic->video_idle_ctrl_work, hisifb_video_idle_workqueue_handler);
	mutex_init(&(vic->video_idle_ctrl_lock));

	vic->idle_ctrl_created = 1;
}

void hisifb_video_idle_ctrl_unregister(struct platform_device *pdev)
{
	struct hisi_fb_data_type *hisifd = NULL;
	struct hisifb_video_idle_ctrl *vic = NULL;

	if (!pdev) {
		HISI_FB_ERR("pdev is NULL\n");
		return;
	}

	hisifd = platform_get_drvdata(pdev);
	if (!hisifd) {
		dev_err(&pdev->dev, "hisifd is NULL\n");
		return;
	}

	if (!is_video_idle_ctrl_mode(hisifd))
		return;

	vic = &(hisifd->video_idle_ctrl);
	if (!vic->idle_ctrl_created)
		return;

	vic->idle_ctrl_created = 0;
}

static dss_layer_t *get_video_idle_layer_info(struct hisi_fb_data_type *hisifd)
{
	dss_overlay_t *pov_req = NULL;
	dss_overlay_block_t *pov_h_block = NULL;
	dss_overlay_block_t *pov_h_block_infos = NULL;

	pov_req = &(hisifd->ov_req);
	pov_h_block_infos = (dss_overlay_block_t *)(uintptr_t)(pov_req->ov_block_infos_ptr);
	if (pov_req->ov_block_nums > 1) {
		HISI_FB_ERR("only support one block config\n");
		return NULL;
	}

	pov_h_block = &(pov_h_block_infos[0]);
	if (pov_h_block->layer_nums > 1) {
		HISI_FB_ERR("only support one layerconfig\n");
		return NULL;
	}

	return (&(pov_h_block->layer_infos[0]));
}

static void hisifb_video_idle_update_cache_size(
	struct hisifb_video_idle_ctrl *video_idle_ctrl,
	dss_layer_t *layer)
{
	/* update l3 cache size */
	video_idle_ctrl->l3cache_size = layer->img.buf_size > L3CACHE_AVAILABLE_SIZE ?
		L3CACHE_AVAILABLE_SIZE : layer->img.buf_size;

	/* update lb cache size, lb size > 0 means support lb */
	if ((LB_AVAILABLE_SIZE > 0) && (layer->img.buf_size > video_idle_ctrl->l3cache_size))
		video_idle_ctrl->lb_size = layer->img.buf_size - video_idle_ctrl->l3cache_size;
}

struct video_idle_alloc_buf {
	dss_layer_t *layer;
	struct dma_buf *buf;
	struct sg_table *table;
	struct dma_buf_attachment *attach;
};

static int buf_alloc_preprocess(struct hisi_fb_data_type *hisifd,
	struct video_idle_alloc_buf *cache)
{
	int nents;

	if (!hisifd->video_idle_ctrl.gpu_compose_idle_frame)
		return 0;

	hisi_check_and_return(hisifd->video_idle_ctrl.buffer_alloced, 0,
		DEBUG, "fb%d buffer alloced\n", hisifd->index);

	cache->layer = get_video_idle_layer_info(hisifd);
	hisi_check_and_return(!cache->layer, 0, INFO, "get_video_idle_layer_info fail\n");

	cache->buf = hisi_dss_get_dmabuf(cache->layer->img.shared_fd);
	cache->attach = dma_buf_attach(cache->buf, __hdss_get_dev());
	if (IS_ERR_OR_NULL(cache->attach)) {
		hisi_dss_put_dmabuf(cache->buf);
		HISI_FB_ERR("dma_buf_attach fail");
		return -1;
	}

	cache->table = dma_buf_map_attachment(cache->attach, DMA_BIDIRECTIONAL);
	if (IS_ERR_OR_NULL(cache->table)) {
		dma_buf_detach(cache->buf, cache->attach);
		hisi_dss_put_dmabuf(cache->buf);
		HISI_FB_ERR("dma_buf_map_attachment fail");
		return -1;
	}

	nents = sg_nents(cache->table->sgl);

	HISI_FB_DEBUG("nents=%d, length=0x%x, buffer_size=0x%x\n",
		nents, cache->table->sgl->length, cache->layer->img.buf_size);

	return 1;
}

static void save_video_idle_layer_img_info(struct hisi_fb_data_type *hisifd, dss_layer_t *layer)
{
	layer->img.vir_addr = hisifd->video_idle_ctrl.wb_vir_addr;
	layer->img.afbc_header_addr = hisifd->video_idle_ctrl.wb_vir_addr + layer->img.afbc_header_offset;
	layer->img.afbc_payload_addr = hisifd->video_idle_ctrl.wb_vir_addr + layer->img.afbc_payload_offset;

	HISI_FB_DEBUG("vir_addr=0x%lx, header=0x%lx, payload=0x%lx, size-all=0x%x, l3=0x%x, lb=0x%x\n",
		layer->img.vir_addr, layer->img.afbc_header_addr, layer->img.afbc_payload_addr, layer->img.buf_size,
		hisifd->video_idle_ctrl.l3cache_size, hisifd->video_idle_ctrl.lb_size);
}

static int hisifb_video_idle_alloc_cache(struct hisi_fb_data_type *hisifd)
{
	struct video_idle_alloc_buf cache = {NULL};
	int ret;

	ret = buf_alloc_preprocess(hisifd, &cache);
	hisi_check_and_return((ret != 1), ret, DEBUG, "buf_alloc_preprocess return\n");

	hisifb_video_idle_update_cache_size(&(hisifd->video_idle_ctrl), cache.layer);

	if (hisifb_video_idle_lb_request(&(hisifd->video_idle_ctrl), cache.table)) {
		HISI_FB_ERR("lb_dmabuf_attach fail.\n");
		goto attach_fail;
	}

	if (hisifb_video_idle_l3cache_request(&(hisifd->video_idle_ctrl))) {
		HISI_FB_ERR("l3cache request failed\n");
		goto l3cache_request_fail;
	}

	hisifd->video_idle_ctrl.wb_vir_addr = hisi_iommu_idle_display_sg_map(__hdss_get_dev(),
		hisifd->video_idle_ctrl.policy_id, cache.table->sgl, cache.layer->img.buf_size,
		hisifd->video_idle_ctrl.l3cache_size, hisifd->video_idle_ctrl.lb_size);

	if (hisifd->video_idle_ctrl.wb_vir_addr == 0) {
		HISI_FB_ERR("hisi_iommu_idle_display_sg_map fail\n");
		goto map_fail;
	}

	ret = dma_buf_begin_cpu_access(cache.attach->dmabuf, DMA_FROM_DEVICE);
	if (ret) {
		HISI_FB_ERR("dma_buf_begin_cpu_access fail\n");
		goto map_fail;
	}

	dma_buf_unmap_attachment(cache.attach, cache.table, DMA_BIDIRECTIONAL);
	dma_buf_detach(cache.buf, cache.attach);

	/* save buf */
	hisifd->video_idle_ctrl.buf = cache.buf;
	hisifd->video_idle_ctrl.buf_size = cache.layer->img.buf_size;

	save_video_idle_layer_img_info(hisifd, cache.layer);

	hisifd->video_idle_ctrl.buffer_alloced = true;

	HISI_FB_DEBUG("alloc cache success\n");
	return 0;

map_fail:
	hisifb_video_idle_l3cache_release(&(hisifd->video_idle_ctrl));
l3cache_request_fail:
	hisifb_video_idle_lb_release(&(hisifd->video_idle_ctrl), cache.table);
attach_fail:
	dma_buf_unmap_attachment(cache.attach, cache.table, DMA_BIDIRECTIONAL);
	dma_buf_detach(cache.buf, cache.attach);
	hisi_dss_put_dmabuf(cache.buf);
	return -1;
}

int hisifb_video_idle_release_cache(struct hisi_fb_data_type *hisifd)
{
	struct sg_table *table = NULL;
	struct dma_buf_attachment *attach = NULL;

	if (!hisifd->video_idle_ctrl.gpu_compose_idle_frame)
		return 0;

	if (!hisifd->video_idle_ctrl.buffer_alloced) {
		HISI_FB_DEBUG("fb%d buffer freed\n", hisifd->index);
		return 0;
	}

	attach = dma_buf_attach(hisifd->video_idle_ctrl.buf, __hdss_get_dev());
	if (IS_ERR_OR_NULL(attach)) {
		hisi_dss_put_dmabuf(hisifd->video_idle_ctrl.buf);
		HISI_FB_ERR("dma_buf_attach fail\n");
		return -1;
	}
	table = dma_buf_map_attachment(attach, DMA_BIDIRECTIONAL);
	if (IS_ERR_OR_NULL(table)) {
		dma_buf_detach(hisifd->video_idle_ctrl.buf, attach);
		hisi_dss_put_dmabuf(hisifd->video_idle_ctrl.buf);
		HISI_FB_ERR("dma_buf_map_attachment fail\n");
		return -1;
	}

	(void)hisi_iommu_unmap(__hdss_get_dev(),
		hisifd->video_idle_ctrl.wb_vir_addr, hisifd->video_idle_ctrl.buf_size);
	hisifb_video_idle_l3cache_release(&(hisifd->video_idle_ctrl));
	hisifb_video_idle_lb_release(&(hisifd->video_idle_ctrl),
		table);

	dma_buf_unmap_attachment(attach, table, DMA_BIDIRECTIONAL);
	dma_buf_detach(hisifd->video_idle_ctrl.buf, attach);

	hisi_dss_put_dmabuf(hisifd->video_idle_ctrl.buf);
	hisifd->video_idle_ctrl.buf = NULL;
	hisifd->video_idle_ctrl.buf_size = 0;
	hisifd->video_idle_ctrl.wb_buffer_base = 0;
	hisifd->video_idle_ctrl.wb_phys_addr = 0;
	hisifd->video_idle_ctrl.wb_vir_addr = 0;
	hisifd->video_idle_ctrl.buffer_alloced = false;

	HISI_FB_DEBUG("release cache succ\n");
	return 0;
}

static bool is_video_idle_in_secure_mode(struct hisi_fb_data_type *hisifd)
{
	int i;
	int m;
	dss_layer_t *layer = NULL;
	dss_overlay_t *pov_req = NULL;
	dss_overlay_block_t *pov_h_block = NULL;
	dss_overlay_block_t *pov_h_block_infos = NULL;

	if (hisifd->ov_req.sec_enable_status == DSS_SEC_ENABLE)
		return true;

	pov_req = &(hisifd->ov_req);
	pov_h_block_infos = (dss_overlay_block_t *)(uintptr_t)(pov_req->ov_block_infos_ptr);
	for (m = 0; m < (int)pov_req->ov_block_nums; m++) {
		pov_h_block = &(pov_h_block_infos[m]);
		for (i = 0; i < (int)pov_h_block->layer_nums; i++) {
			layer = &(pov_h_block->layer_infos[i]);
			if (layer->img.secure_mode == 1)
				return true;
		}
	}

	return false;
}

static int hisifb_gpu_compose_video_idle_ctrl
	(struct hisi_fb_data_type *hisifd, uint32_t video_idle_status)
{
	int ret = 0;
	static uint32_t last_frm_num;
	static int dss_free_cache_refcount;

	if (is_video_idle_in_secure_mode(hisifd)) {
		HISI_FB_DEBUG("video idle in secure mode!\n");
		goto exit;
	}

	if (video_idle_status) {
		if ((hisifd->ov_req.frame_no - last_frm_num) == 1) {
			HISI_FB_ERR("error idle frame\n");
			return ret;
		}

		if (hisifd->video_idle_ctrl.buffer_alloced) {
			HISI_FB_INFO("free_refcount <= 3, release and remap\n");
			ret = hisifb_video_idle_release_cache(hisifd);
			if (ret < 0) {
				HISI_FB_ERR("release cache fail\n");
				return ret;
			}
		}

		ret = hisifb_video_idle_alloc_cache(hisifd);
		if (ret < 0) {
			HISI_FB_ERR("alloc cache fail\n");
			return ret;
		}
		dss_free_cache_refcount = 0;
		last_frm_num = hisifd->ov_req.frame_no;
		hisifd->video_idle_ctrl.idle_frame_display = true;
		HISI_FB_DEBUG("enter idle display\n");
		return ret;
	}

exit:
	if (dss_free_cache_refcount == 0)
		HISI_FB_DEBUG("exit idle display\n");

	dss_free_cache_refcount++;

	if (dss_free_cache_refcount > BUFFER_RELEASE_LAYER_CNT) {
		dss_free_cache_refcount = 1;  /* avoid overflow */
		hisifb_video_idle_release_cache(hisifd);
	}

	hisifd->video_idle_ctrl.idle_frame_display = false;

	return ret;
}

static void release_display_ctrl(struct hisi_fb_data_type *hisifd,
	int dss_free_cache_refcount,
	struct hisifb_video_idle_ctrl *vic)
{
	if (vic->video_idle_wb_status) {
		HISI_FB_INFO("exit video idle! wb clear.\n");
		hisifb_video_idle_wb_clear(hisifd);
	}

	if (vic->video_idle_rb_status) {
		HISI_FB_INFO("exit video idle! rb clear.\n");
		hisifb_video_idle_wb_clear(hisifd);
		hisifb_video_idle_rb_clear(hisifd);
	}

	/* force clear wch in three times after exiting video idle */
	if ((dss_free_cache_refcount >= 1) && (dss_free_cache_refcount <= 3)) {
		hisifb_video_idle_wb_clear(hisifd);
		hisifb_video_idle_wb_clear_by_cpu(hisifd);
	}

	/* delay two frame */
	if (dss_free_cache_refcount > 3)
		hisifb_video_idle_buffer_free(hisifd);

	vic->video_idle_wb_status = false;
	vic->video_idle_rb_status = false;
}

int hisifb_video_panel_idle_display_ctrl(struct hisi_fb_data_type *hisifd, uint32_t video_idle_status)
{
	struct hisifb_video_idle_ctrl *vic = NULL;
	static int dss_free_cache_refcount;

	hisi_check_and_return(!hisifd, -EINVAL, ERR, "hisifd is NULL!\n");

	HISI_FB_DEBUG("video_idle_status = %d, gpu_compose_idle_frame = %d!\n",
		video_idle_status, hisifd->video_idle_ctrl.gpu_compose_idle_frame);

	if (!is_video_idle_ctrl_mode(hisifd)) {
		HISI_FB_DEBUG("do not support video idle!\n");
		return 0;
	}

	if (hisifd->index != PRIMARY_PANEL_IDX)
		return 0;

	if (is_lb_available() == 0)
		return 0;

	vic = &(hisifd->video_idle_ctrl);

	hisi_check_and_return((!g_enable_video_idle_l3cache && !vic->buffer_alloced), 0,
		INFO, "disable cache idle display");

	if (hisifd->video_idle_ctrl.gpu_compose_idle_frame)
		return hisifb_gpu_compose_video_idle_ctrl(hisifd, video_idle_status);

	if (is_video_idle_in_secure_mode(hisifd))
		goto exit;

	if (video_idle_status && !vic->video_idle_wb_status) {
		/* 0. interrupt config */
		hisifb_video_idle_interrupt_config(hisifd);

		/* 1. alloc buffer and l3cache */
		hisifb_video_idle_buffer_alloc(hisifd, vic->l3cache_size);
		hisi_check_and_return(!vic->buffer_alloced, 0, INFO, "buffer alloc fail, exit video idle.\n");

		/* 2. wb config */
		hisifb_video_idle_wb_pipe_set_reg(hisifd);

		/* 3. wch config */
		hisifb_video_idle_wch_config(hisifd);
		hisifb_video_idle_wch_set_reg(hisifd);

		/* 4. wait for next vsync */
		vic->video_idle_wb_status = true;
		dss_free_cache_refcount = 0;

		schedule_work(&(vic->video_idle_ctrl_work));

		return 0;
	} else {
		dss_free_cache_refcount++;
	}

exit:
	release_display_ctrl(hisifd, dss_free_cache_refcount, vic);
	return 0;
}

void hisifb_hisync_disp_sync_enable(struct hisi_fb_data_type *hisifd)
{
	char __iomem *cmdlist_base = NULL;

	if (!hisifd) {
		HISI_FB_ERR("hisifd is null ptr\n");
		return;
	}

	if (hisifd->index != PRIMARY_PANEL_IDX)
		return;
	HISI_FB_INFO("enable hisync feature!\n");

	cmdlist_base = hisifd->dss_base + DSS_CMDLIST_OFFSET;
	set_reg(cmdlist_base + CMD_SYNC_MODE, 0x1, 1, 0);
}

int hisifb_hisync_disp_sync_config(struct hisi_fb_data_type *hisifd)
{
	uint32_t try_times = 0;
	uint32_t cycle = 0;
	char __iomem *ldi_base = NULL;
	char __iomem *cmdlist_base = NULL;

	if (!hisifd) {
		HISI_FB_ERR("hisifd is null ptr\n");
		return -1;
	}

	if (hisifd->index != PRIMARY_PANEL_IDX)
		return -1;

	cmdlist_base = hisifd->dss_base + DSS_CMDLIST_OFFSET;
	while ((uint32_t)inp32(cmdlist_base + CMD_CFG_FLAG) & BIT(0)) {
		udelay(1);
		if (++try_times > 10000) {  /* 10ms */
			HISI_FB_ERR("Read cmd_cfg_flag timeout!\n");
			return -1;
		}
	}
	HISI_FB_INFO("wait hisync disp sync succss!\n");

	set_reg(cmdlist_base + CMD_CFG_FLAG, 0x1, 1, 0);

	ldi_base = hisifd->mipi_dsi0_base;
	if (hisifd->panel_info.vsync_delay_time > 0) {
		cycle = hisifd->panel_info.vsync_delay_time * hisifd->panel_info.pxl_clk_rate / 1000;  /* ms to s */
		set_reg(ldi_base + MIPI_VSYNC_DELAY_TIME, cycle - 1, 32, 0);
		set_reg(ldi_base + MIPI_VSYNC_DELAY_CTRL, 0x2, 2, 1);
	} else {
		set_reg(ldi_base + MIPI_VSYNC_DELAY_CTRL, 0x1, 2, 1);
	}
	return 0;
}
#pragma GCC diagnostic pop

