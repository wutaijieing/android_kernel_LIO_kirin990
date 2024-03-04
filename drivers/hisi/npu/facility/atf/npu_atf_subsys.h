/*
 * npu_atf_subsys.c
 *
 * about npu atf subsys
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
#ifndef _NPU_ATF_H_
#define _NPU_ATF_H_
#include "npu_common.h"

/* 1dd must be the same */
#define NPU_SLV_SECMODE                 0xC501dd00
#define NPU_MST_SECMODE                 0xC501dd01
#define NPU_START_SECMODE               0xC501dd02
#define NPU_ENABLE_SECMODE              0xC501dd03
#define GIC_CFG_CHECK_SECMODE           0xC501dd04
#define GIC_ONLINE_READY_SECMODE        0xC501dd05
#define NPU_CPU_POWER_DOWN_SECMODE      0xC501dd06
#define NPU_INFORM_POWER_DOWN_SECMODE   0xC501dd07
#define NPU_POWER_DOWN_TS_SEC_REG       0xC501dd08
#define NPU_SMMU_TCU_INIT_NS            0xC501dd09
#define NPU_SMMU_TCU_CACHE_INIT         0xC501dd0a
#define NPU_SMMU_TCU_DISABLE            0xC501dd0b
#define NPU_POWER_UP_SMMU_TBU           0xC501dd0c
#define NPU_POWER_DOWN_SMMU_TBU         0xC501dd0d
#define NPU_LOWERPOWER_OPS              0xC501dd0e

/* smc command flags[0-31]: x3*/
#define NPU_FLAGS_POWER_ON                  (0x1 << 0) /* bit0: npu subsys poweron/off */
#define NPU_FLAGS_POWER_OFF                 (0x1 << 1) /* bit1: npu subsys poweron/off */
#define NPU_FLAGS_PWONOFF_AIC0              (0x1 << 2) /* bit2: core0 poweron/off */
#define NPU_FLAGS_PWONOFF_AIC1              (0x1 << 3) /* bit3: core1 poweron/off */
#define NPU_FLAGS_PWONOFF_AIC0_1            (0x1 << 4) /* bit4: core0&core1 poweron/off */
#define NPU_FLAGS_ISPNN_ENABLE_SEPARATED    (0x1 << 5) /* bit5: ispnn enable single core */
#define NPU_FLAGS_ISPNN_DISABLE_SEPARATED   (0x1 << 6) /* bit6: return aicore to aicore pool */
#define NPU_FLAGS_ISPNN_ENABLE_SHARED       (0x1 << 7) /* bit7: take aicore from aicore pool */
#define NPU_FLAGS_ISPNN_DISABLE_SHARED      (0x1 << 8) /* bit8: return aicore to aicore pool */
#define NPU_FLAGS_ISPNN_PROFILING           (0x1 << 9) /* bit9: profiling for ispnn */
#define NPU_FLAGS_INIT_SYSDMA               (0x1 << 10) /* bit10: sysdma init */
#define NPU_FLAGS_DISBALE_SYSDMA            (0x1 << 11) /* bit11: sysdma disable */

#define ARRAY_NUM(array_name) \
	(sizeof(array_name)/sizeof(array_name[0]))

typedef struct npu_workmode_action {
	uint32_t work_mode;
	uint32_t action;
	uint32_t smc_bit;
} npu_workmode_action_t;

enum npu_atf_error_code {
	NPU_ATF_SUCC = 0,
	NPU_ATF_MULTICHIP_GIC_ONLINE_FAILED  = 1,
	NPU_ATF_MULTICHIP_GIC_RWP_FAILED = 2,
	NPU_ATF_MULTICHIP_GIC_PUP_FAILED = 4, // Power update in Progress check
	NPU_ATF_MULTICHIP_GIC_RTS_FAILED = 8, // Route Table Status check
};

enum npu_gic_chipid {
	NPU_GIC_1  = 1
};

enum {
	HW_IRQ_LEVEL_TRIGGER_TYPE = 0x0,
	HW_IRQ_EDGE_TRIGGER_TYPE = 0x1
};

enum npuip {
	NPUIP_NONE = 0,
	NPUIP_NPU_SUBSYS,
	NPUIP_NPU_BUS,
	NPUIP_TS_SUBSYS,
	NPUIP_AICORE,
	NPUIP_AICPU,
	NPUIP_NPUCPU_SUBSYS, // kirin990 cs
	NPUIP_SDMA,
	NPUIP_TCU,
	NPUIP_MAX
};

enum npu_gic_cmd {
	NPU_GIC_POWER_OFF_INFORM = 0,
	TSCPU_GIC_WAKER_OPS_READY = 100,
	TSCPU_GIC_WAKER_OPS_DONE = 200,
	TSCPU_GIC_PWRR_OPS_READY = 300,
	TSCPU_GIC_PWRR_OPS_DONE = 400,
	AICPU_GIC_WAKER_OPS_READY = 500,
	AICPU_GIC_WAKER_OPS_DOWN = 600,
	AICPU_GIC_PWRR_OPS_READY = 700,
	AICPU_GIC_PWRR_OPS_DOWN = 800
};
enum npu_lowpower_dev_target {
	NPU_LOWPOWER_PCR = 0,
	NPU_LOWPOWER_MAX
};

enum npu_lowpower_flag {
	NPU_LOWPOWER_FLAG_ENABLE = 0,
	NPU_LOWPOWER_FLAG_DISABLE,
	NPU_LOWPOWER_FLAG_MAX
};

int npuatf_change_slv_secmod(u64 cmd);

int npuatf_change_mst_secmod(u64 cmd);

int npuatf_start_secmod(u64 ts_cpu_addr, u64 canary);

int npuatf_enable_secmod(u64 cmd);

int npuatf_powerup_aicore(u64 secure_mode, u32 aic_flag);

int npuatf_power_down_aicore(u64 is_secure, u32 aic_flag);

int npuatf_power_down_tcu(void);

int npuatf_power_down(u32 mode);

int atf_query_gic0_state(u64 cmd);

int acpu_gic0_online_ready(u64 cmd);

int npuatf_power_down_ts_secreg(u32 is_secure);

int npuatf_inform_power_down(int gic_cmd);

int npuatf_enable_tbu(u64 cmd);

int npuatf_disable_tbu(u64 cmd, u32 aic_flag);

int npuatf_switch_hwts_aicore_pool(u32 work_mode, u32 flag);

int devdrv_init_sdma_tbu(u64 secure_mode, u32 flag);

int npuatf_lowpower_ops(u64 lowpower_dev, u64 lowpower_flag);

#endif
