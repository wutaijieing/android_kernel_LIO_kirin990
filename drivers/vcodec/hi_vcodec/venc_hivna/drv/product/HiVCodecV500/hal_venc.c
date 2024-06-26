/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description: venc register config
 * Create: 2019-08-30
 */
#include "hal_venc.h"
#include "drv_common.h"

HI_VOID venc_hal_get_reg_stream_len(struct stream_info *stream_info, HI_U32 *reg_base)
{
	return;
}

HI_VOID venc_hal_cfg_curld_osd01(struct encode_info *channel_cfg, HI_U32 *reg_base)
{
	S_HEVC_AVC_REGS_TYPE *all_reg = (S_HEVC_AVC_REGS_TYPE *)reg_base;/*lint !e826 */
	U_VEDU_CURLD_OSD01_ALPHA D32;

	D32.bits.rgb_clip_min = channel_cfg->all_reg.VEDU_CURLD_OSD01_ALPHA.bits.rgb_clip_min;
	D32.bits.rgb_clip_max = channel_cfg->all_reg.VEDU_CURLD_OSD01_ALPHA.bits.rgb_clip_max;
	D32.bits.curld_hfbcd_clk_gt_en = channel_cfg->all_reg.VEDU_CURLD_OSD01_ALPHA.bits.curld_hfbcd_clk_gt_en;
	D32.bits.curld_hfbcd_bypass_en = channel_cfg->all_reg.VEDU_CURLD_OSD01_ALPHA.bits.curld_hfbcd_raw_en;
	D32.bits.curld_hfbcd_raw_en = channel_cfg->all_reg.VEDU_CURLD_OSD01_ALPHA.bits.curld_hfbcd_raw_en;
	all_reg->VEDU_CURLD_OSD01_ALPHA.u32 = D32.u32;
}

HI_VOID vedu_hal_set_int(HI_U32 *reg_base)
{
	S_HEVC_AVC_REGS_TYPE *all_reg = (S_HEVC_AVC_REGS_TYPE *)reg_base; /*lint !e826 */
	all_reg->VEDU_VCPI_INTMASK.u32 = VENC_MASK;
}

HI_VOID vedu_hal_cfg_smmu(struct encode_info *channel_cfg, HI_U32 core_id)
{
	venc_entry_t *venc = platform_get_drvdata(venc_get_device());
	struct venc_context *ctx = &venc->ctx[core_id];

	if (ctx->first_cfg_flag == HI_TRUE) {
		venc_smmu_init(channel_cfg->is_protected, core_id);
		ctx->first_cfg_flag = HI_FALSE;
	}

	venc_smmu_cfg(channel_cfg, ctx->reg_base);
}

HI_VOID vedu_hal_cfg(volatile HI_U32 *base, HI_U32 offset, HI_U32 value)
{
	base[offset / 4] = value;
}
