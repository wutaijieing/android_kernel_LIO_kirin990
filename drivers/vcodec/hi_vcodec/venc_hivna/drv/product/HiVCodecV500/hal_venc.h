/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description: venc register config
 * Create: 2010-03-31
 */
#ifndef __HAL_VENC_H__
#define __HAL_VENC_H__

#include "hi_type.h"
#include "drv_venc_ioctl.h"
#include "smmu.h"
#include "hal_common.h"

#define VENC_COMPATIBLE       "hisi,HiVCodecV500-venc"  // reused for hivcodecv510 v500
#define VENC_CS_SUPPORT
#define VENC_FPGAFLAG_CS      "venc_fpga"
#define VENC_PCTRL_PERI_CS    0xFEC3E0BC
#define VENC_EXISTBIT_CS      0x08

HI_VOID venc_hal_clr_all_int(S_HEVC_AVC_REGS_TYPE *vedu_reg);
HI_VOID venc_hal_disable_all_int(S_HEVC_AVC_REGS_TYPE *vedu_reg);
HI_VOID venc_hal_start_encode(S_HEVC_AVC_REGS_TYPE *vedu_reg);
HI_VOID venc_hal_get_reg_venc(struct stream_info *stream_info, HI_U32 *reg_base);
HI_VOID venc_hal_get_reg_stream_len(struct stream_info *stream_info, HI_U32 *reg_base);
HI_VOID venc_hal_cfg_curld_osd01(struct encode_info *channel_cfg, HI_U32 *reg_base);
HI_VOID vedu_hal_cfg_smmu(struct encode_info *channel_cfg, HI_U32 core_id);
HI_VOID vedu_hal_set_int(HI_U32 *reg_base);
HI_VOID vedu_hal_cfg(volatile HI_U32 *base, HI_U32 offset, HI_U32 value);
HI_VOID venc_hal_get_slice_reg(struct stream_info *stream_info, HI_U32 *reg_base);

#endif
