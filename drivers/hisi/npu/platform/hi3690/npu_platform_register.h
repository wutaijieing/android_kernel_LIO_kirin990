/*
 * npu_platform_register.h
 *
 * about npu platform register
 *
 * Copyright (c) 2012-2019 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#ifndef __DEVDRV_PLATFORM_REGISTER_H
#define __DEVDRV_PLATFORM_REGISTER_H

#define SOC_HARDWARE_VERSION (DEVDRV_PLATFORM_LITE_PHOENIX_CS)

#include "soc_npu_ts_sysctrl_reg_offset.h"
#include "soc_npu_tscpu_cfg_reg_offset.h"
#include "soc_acpu_baseaddr_interface.h"

#define DEVDRV_SC_TESTREG0_OFFSET \
	(SOC_TS_SYSCTRL_SC_TESTREG0_REG - SOC_TS_SYSCTRL_BASE)

#define DEVDRV_SC_TESTREG1_OFFSET \
	(SOC_TS_SYSCTRL_SC_TESTREG1_REG - SOC_TS_SYSCTRL_BASE)

#define TS_BOOT_STATUS_OBSERVATION_REG \
	(SOC_TS_SYSCTRL_SC_TESTREG6_REG - SOC_TS_SYSCTRL_BASE)

#define DEVDRV_SC_TESTREG8_OFFSET \
	(SOC_TS_SYSCTRL_SC_TESTREG8_REG - SOC_TS_SYSCTRL_BASE)

#define DEVDRV_CFG_STAT0_OFFSET \
	(SOC_NPU_TSCPU_CFG_TS_CPU_STAT0_REG - SOC_TS_SYSCTRL_BASE)

#define SOC_CRGPERIPH_NPUCRG_STAT_ADDR(base)   ((base) + (0x658UL))
#define SOC_CRGPERIPH_NPUCRG_IP_RST_BIT_OFFSET_START  12

#define SOC_ACPU_NPU_DPM_BASE_ADDR      SOC_ACPU_npu_dpm_BASE_ADDR
#define SOC_ACPU_NPU_CRG_BASE_ADDR      SOC_ACPU_npu_crg_BASE_ADDR

#endif
