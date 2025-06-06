/* Copyright (c) 2013-2020, Hisilicon Tech. Co., Ltd. All rights reserved.
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
#ifndef _HISI_OVERLAY_UTILS_PLATFORM_H_
#define _HISI_OVERLAY_UTILS_PLATFORM_H_

#include "hisi_overlay_utils_array_dssv510.h"

#define CONFIG_DSS_LP_USED

/* GPIO */
#define GPIO_LCD_POWER_1V2 54  /* GPIO_6_6 */
#define GPIO_LCD_STANDBY 67  /* GPIO_8_3 */
#define GPIO_LCD_RESETN 65  /* GPIO_8_1 */
#define GPIO_LCD_GATING 60  /* GPIO_7_4 */
#define GPIO_LCD_PCLK_GATING 58  /* GPIO_7_2 */
#define GPIO_LCD_REFCLK_GATING 59 /* GPIO_7_3 */
#define GPIO_LCD_SPICS 168  /* GPIO_21_0 */
#define GPIO_LCD_DRV_EN 73  /* GPIO_9_1 */

#define GPIO_PG_SEL_A 168  /* GPIO_21_0 */
#define GPIO_TX_RX_A 170  /* GPIO_21_2 */
#define GPIO_PG_SEL_B 133  /* GPIO_16_5 */
#define GPIO_TX_RX_B 135  /* GPIO_16_7 */

#define CRGPERI_PLL0_CLK_RATE 1660000000UL
#define CRGPERI_PLL2_CLK_RATE 1920000000UL
#define CRGPERI_PLL3_CLK_RATE 1200000000UL
#define CRGPERI_PLL7_CLK_RATE 1290000000UL

#define DEFAULT_DSS_CORE_CLK_RATE_L4 554000000UL
#define DEFAULT_DSS_CORE_CLK_RATE_L3 415000000UL
#define DEFAULT_DSS_CORE_CLK_RATE_L2 332000000UL
#define DEFAULT_DSS_CORE_CLK_RATE_L1 208000000UL

#define DEFAULT_MDC_CORE_CLK_RATE_L4 554000000UL
#define DEFAULT_MDC_CORE_CLK_RATE_L3 415000000UL
#define DEFAULT_MDC_CORE_CLK_RATE_L2 332000000UL
#define DEFAULT_MDC_CORE_CLK_RATE_L1 238000000UL

/* pix0_clk: 0.65v-300M, 0.7-415M, 0.8-645M */
#define DEFAULT_DSS_PXL0_CLK_RATE_L3 645000000UL
#define DEFAULT_DSS_PXL0_CLK_RATE_L2 415000000UL
#define DEFAULT_DSS_PXL0_CLK_RATE_L1 300000000UL

/* mm_axi_clk, aclk_mmbuf */
#define DEFAULT_DSS_MMBUF_CLK_RATE_L4 480000000UL
#define DEFAULT_DSS_MMBUF_CLK_RATE_L3 332000000UL
#define DEFAULT_DSS_MMBUF_CLK_RATE_L2 277000000UL
#define DEFAULT_DSS_MMBUF_CLK_RATE_L1 208000000UL

/* pix1_clk: clk_ldi1_ini */
#define DEFAULT_DSS_PXL1_CLK_RATE_L4 600000000UL
#define DEFAULT_DSS_PXL1_CLK_RATE_L3 415000000UL
#define DEFAULT_DSS_PXL1_CLK_RATE_L2 332000000UL
#define DEFAULT_DSS_PXL1_CLK_RATE_L1 213000000UL

/* dss clk power off */
#define DEFAULT_DSS_CORE_CLK_RATE_POWER_OFF 104000000UL
#define DEFAULT_DSS_MMBUF_CLK_RATE_POWER_OFF 104000000UL
#define DEFAULT_DSS_PXL1_CLK_RATE_POWER_OFF 104000000UL

#define DEFAULT_PCLK_PCTRL_RATE 83000000UL

/* equal to clk_sysbus, follow peri voltage, 103.75 - 237.14M
 * use max clk rate to calc DPHYTX_STOPCNT
 */
#define DEFAULT_PCLK_DSS_RATE 238000000UL

#define OVL_LAYER_NUM_MAX 9

/* 10BIT: 0x3ff, 8BIT: 0xFF  RATIO = 0x3ff/0xff */
#define OVL_PATTERN_RATIO 4

/* the channel id which support arsr pre scaler */
#define ARSR_PRE_CH (DSS_RCHN_V0)

#ifdef CONFIG_HISI_DPP_CMDLIST
/* reserve the last cmdlist for DPP exclusive use */
#define HISI_DSS_CMDLIST_MAX 15
#else
#define HISI_DSS_CMDLIST_MAX 16
#endif
#define HISI_DSS_CMDLIST_IDXS_MAX 0xFFFF  /* 16 cmdlist, 16bit, 1111,1111,1111,1111=0xFFFF */
#define HISI_DSS_COPYBIT_CMDLIST_IDXS 0xC000  /* bit14, bit15 */
#define HISI_DSS_MEDIACOMMON_CMDLIST_IDXS 0x204  /* bit2, bit9 */
#define HISI_DSS_DPP_MAX_SUPPORT_BIT 0x1fff  /* 13 dpp module, 13bit, contrast to enmu dpp_module_idx */
#define HISIFB_DSS_PLATFORM_TYPE  (FB_ACCEL_DSSV510 | FB_ACCEL_PLATFORM_TYPE_ASIC)

#define DSS_MIF_SMMU_SMRX_IDX_STEP 16

/* PERI REG */
#define CRG_PERI_DIS3_DEFAULT_VAL 0x0002F000

/* scl */
#define SCF_LINE_BUF 2560

/* DSS global */
#define DSS_GLB_MODULE_CLK_SEL_DEFAULT_VAL 0x01800000

/* DISP global */
#define DSS_MODULE_CORE_CLK_SEL_DEFAULT_VAL 0x00030000

/* LDI0 clk sel */
#define DSS_LDI_CLK_SEL_DEFAULT_VAL 0x00000004

/* DBuf mem ctrl */
#define DSS_DBUF_MEM_CTRL_DEFAULT_VAL 0x00000008

/* SMMU */
#define DSS_SMMU_RLD_EN0_DEFAULT_VAL 0xffffffff
#define DSS_SMMU_RLD_EN1_DEFAULT_VAL 0xffffff8f

/* AIF */
#define DSS_AIF0_CH_LS 0x00040061
#define DSS_AIF1_CH_LS 0x00040041
#define DSS_ONLINE_AXI0 (AXI_CHN0)
#define DSS_ONLINE_AXI1 (AXI_CHN1)
#define DSS_OFFLINE_AXI0 (AXI_CHN0)
#define DSS_OFFLINE_AXI1 (AXI_CHN1)

/* MIF */
#define DSS_MIF_CTRL2_INVAL_SEL3_STRIDE_MASK 0xc

/* AFBCE  */
#define DSS_AFBCE_ENC_OS_CFG_DEFAULT_VAL 0x7

#define TUI_SEC_RCH (DSS_RCHN_V0)

typedef struct dss_mctl_ch_base {
	char __iomem *chn_mutex_base;
	char __iomem *chn_flush_en_base;
	char __iomem *chn_ov_en_base;
	char __iomem *chn_starty_base;
	char __iomem *chn_mod_dbg_base;
} dss_mctl_ch_base_t;

typedef struct dss_arsr2p {
	uint32_t arsr_input_width_height;
	uint32_t arsr_output_width_height;
	uint32_t ihleft;
	uint32_t ihright;
	uint32_t ivtop;
	uint32_t ivbottom;
	uint32_t ihinc;
	uint32_t ivinc;
	uint32_t offset;
	uint32_t mode;
	struct arsr2p_info arsr2p_effect;
	struct arsr2p_info arsr2p_effect_scale_up;
	struct arsr2p_info arsr2p_effect_scale_down;
	uint32_t ihleft1;
	uint32_t ihright1;
	uint32_t ivbottom1;
} dss_arsr2p_t;

typedef struct dirty_region_updt {
	uint32_t dbuf_frm_size;
	uint32_t dbuf_frm_hsize;
	uint32_t dpp_img_size_bef_sr;
	uint32_t dpp_img_size_aft_sr;
	uint32_t dpp_img_size_aft_ifbcsw;
	uint32_t hiace_img_size;
	uint32_t ldi_dpi0_hrz_ctrl0;
	uint32_t ldi_dpi0_hrz_ctrl1;
	uint32_t ldi_dpi0_hrz_ctrl2;
	uint32_t ldi_dpi1_hrz_ctrl0;
	uint32_t ldi_dpi1_hrz_ctrl1;
	uint32_t ldi_dpi1_hrz_ctrl2;
	uint32_t ldi_vrt_ctrl0;
	uint32_t ldi_vrt_ctrl1;
	uint32_t ldi_vrt_ctrl2;
	uint32_t ldi_ctrl;
	uint32_t ifbc_size;
	uint32_t edpi_cmd_size;
	dss_arsr1p_t s_arsr1p;
} dirty_region_updt_t;

/*******************************************************************************
 * dss module reg
 */
typedef struct dss_module_reg {
	char __iomem *mif_ch_base[DSS_CHN_MAX_DEFINE];
	char __iomem *aif_ch_base[DSS_CHN_MAX_DEFINE];
	char __iomem *aif1_ch_base[DSS_CHN_MAX_DEFINE];
	struct dss_mctl_ch_base mctl_ch_base[DSS_CHN_MAX_DEFINE];
	char __iomem *dma_base[DSS_CHN_MAX_DEFINE];
	char __iomem *dfc_base[DSS_CHN_MAX_DEFINE];
	char __iomem *scl_base[DSS_CHN_MAX_DEFINE];
	char __iomem *scl_lut_base[DSS_CHN_MAX_DEFINE];
	char __iomem *arsr2p_base[DSS_CHN_MAX_DEFINE];
	char __iomem *arsr2p_lut_base[DSS_CHN_MAX_DEFINE];
	char __iomem *post_clip_base[DSS_CHN_MAX_DEFINE];
	char __iomem *pcsc_base[DSS_CHN_MAX_DEFINE];
	char __iomem *csc_base[DSS_CHN_MAX_DEFINE];

	char __iomem *ov_base[DSS_OVL_IDX_MAX];
	char __iomem *mctl_base[DSS_MCTL_IDX_MAX];
	char __iomem *mctl_sys_base;
	char __iomem *smmu_base;
	char __iomem *post_scf_base;
	char __iomem *cld_base;

	dss_mif_t mif[DSS_CHN_MAX_DEFINE];
	dss_aif_t aif[DSS_CHN_MAX_DEFINE];
	dss_aif_t aif1[DSS_CHN_MAX_DEFINE];
	dss_aif_bw_t aif_bw[DSS_CHN_MAX_DEFINE];
	dss_aif_bw_t aif1_bw[DSS_CHN_MAX_DEFINE];
	dss_rdma_t rdma[DSS_CHN_MAX_DEFINE];
	dss_wdma_t wdma[DSS_CHN_MAX_DEFINE];
	dss_dfc_t dfc[DSS_CHN_MAX_DEFINE];
	dss_scl_t scl[DSS_CHN_MAX_DEFINE];
	struct dss_arsr2p arsr2p[DSS_CHN_MAX_DEFINE];
	dss_post_clip_t post_clip[DSS_CHN_MAX_DEFINE];
	dss_csc_t pcsc[DSS_CHN_MAX_DEFINE];
	dss_csc_t csc[DSS_CHN_MAX_DEFINE];
	dss_ovl_t ov[DSS_OVL_IDX_MAX];
	dss_mctl_t mctl[DSS_MCTL_IDX_MAX];
	dss_mctl_ch_t mctl_ch[DSS_CHN_MAX_DEFINE];
	dss_mctl_sys_t mctl_sys;
	dss_smmu_t smmu;
	struct dirty_region_updt dirty_region_updt;
	dss_arsr1p_t post_scf;

	uint8_t mif_used[DSS_CHN_MAX_DEFINE];
	uint8_t aif_ch_used[DSS_CHN_MAX_DEFINE];
	uint8_t aif1_ch_used[DSS_CHN_MAX_DEFINE];
	uint8_t dma_used[DSS_CHN_MAX_DEFINE];
	uint8_t dfc_used[DSS_CHN_MAX_DEFINE];
	uint8_t scl_used[DSS_CHN_MAX_DEFINE];
	uint8_t arsr2p_used[DSS_CHN_MAX_DEFINE];
	uint8_t arsr2p_effect_used[DSS_CHN_MAX_DEFINE];
	uint8_t post_clip_used[DSS_CHN_MAX_DEFINE];
	uint8_t pcsc_used[DSS_CHN_MAX_DEFINE];
	uint8_t csc_used[DSS_CHN_MAX_DEFINE];
	uint8_t ov_used[DSS_OVL_IDX_MAX];
	uint8_t ch_reg_default_used[DSS_CHN_MAX_DEFINE];
	uint8_t mctl_used[DSS_MCTL_IDX_MAX];
	uint8_t mctl_ch_used[DSS_CHN_MAX_DEFINE];
	uint8_t mctl_sys_used;
	uint8_t smmu_used;
	uint8_t dirty_region_updt_used;
	uint8_t post_scf_used;
	uint8_t cld_used;
} dss_module_reg_t;

typedef struct dss_mmbuf_info {
	uint32_t mm_base[DSS_CHN_MAX_DEFINE];
	uint32_t mm_size[DSS_CHN_MAX_DEFINE];
	uint32_t mm_base0_y8[DSS_CHN_MAX_DEFINE];
	uint32_t mm_size0_y8[DSS_CHN_MAX_DEFINE];
	uint32_t mm_base1_c8[DSS_CHN_MAX_DEFINE];
	uint32_t mm_size1_c8[DSS_CHN_MAX_DEFINE];
	uint32_t mm_base2_y2[DSS_CHN_MAX_DEFINE];
	uint32_t mm_size2_y2[DSS_CHN_MAX_DEFINE];
	uint32_t mm_base3_c2[DSS_CHN_MAX_DEFINE];
	uint32_t mm_size3_c2[DSS_CHN_MAX_DEFINE];

	uint8_t mm_used[DSS_CHN_MAX_DEFINE];
} dss_mmbuf_info_t;

#define inheritor_dss_wr_reg(addr, value)

#endif

