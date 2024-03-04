/*
 * npu_pm_framework.c
 *
 * about npu pm
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
#include "npu_pm_framework.h"

#include <linux/hisi/rdr_pub.h>
#include <linux/timer.h>
#include <dsm/dsm_pub.h>

#include "npu_log.h"
#include "npu_platform.h"
#include "npu_firmware.h"
#include "npu_shm.h"
#include "npu_dpm.h"
#include "bbox/npu_dfx_black_box.h"
#include "npu_heart_beat.h"
#include "npu_adapter.h"

static int npu_pm_check_workmode(u32 curr_work_mode, u32 workmode)
{
	if (workmode >= NPU_ISPNN_SEPARATED)
		return -1;
	if (BITMAP_GET(curr_work_mode, NPU_NONSEC) && (workmode == NPU_SEC))
		return -1;
	if (BITMAP_GET(curr_work_mode, NPU_SEC) && (workmode == NPU_NONSEC))
		return -1;

	return 0;
}

int npu_pm_enter_workmode(struct devdrv_proc_ctx *proc_ctx, struct devdrv_dev_ctx *dev_ctx, u32 workmode)
{
	int ret;
	u32 curr_work_mode;

	mutex_lock(&dev_ctx->npu_power_up_off_mutex);
	curr_work_mode = dev_ctx->pm.work_mode;
	NPU_DRV_WARN("curr_work_mode_set = 0x%x secure_mode = 0x%x \n", curr_work_mode, workmode);

	ret = npu_pm_check_workmode(curr_work_mode, workmode);
	if (ret != 0) {
		mutex_unlock(&dev_ctx->npu_power_up_off_mutex);
		NPU_DRV_ERR("not support workmode, curr_work_mode_set = 0x%x work_mode = 0x%x\n", curr_work_mode, workmode);
		return ret;
	}

	dev_ctx->pm.work_mode = (1 << workmode);
	ret = npu_powerup(proc_ctx, dev_ctx, workmode);
	if (ret) {
		dev_ctx->pm.work_mode = curr_work_mode;
		mutex_unlock(&dev_ctx->npu_power_up_off_mutex);
		NPU_DRV_ERR("npu powerup failed\n");

		if (!dsm_client_ocuppy(davinci_dsm_client)) {
			dsm_client_record(davinci_dsm_client, "npu power up failed\n");
			dsm_client_notify(davinci_dsm_client, DSM_AI_KERN_POWER_UP_ERR_NO);
			NPU_DRV_ERR("[I/DSM] %s dmd report\n", davinci_dsm_client->client_name);
		}
		return ret;
	}

	if (workmode == NPU_SEC) {
		mutex_unlock(&dev_ctx->npu_power_up_off_mutex);
		NPU_DRV_WARN("secure or ispnn npu power up,no need send mbx to tscpu,return directly\n");
		return 0;
	}

	ret = devdrv_proc_send_alloc_stream_mailbox(proc_ctx);
	if (ret) {
		mutex_unlock(&dev_ctx->npu_power_up_off_mutex);
		NPU_DRV_ERR("devdrv send stream mailbox failed\n");
		return ret;
	}
	mutex_unlock(&dev_ctx->npu_power_up_off_mutex);

	npu_rdr_exception_init();

	NPU_DRV_WARN("npu dev %u powerup successfully!\n", dev_ctx->devid);
	return ret;
}

int npu_pm_exit_workmode(struct devdrv_proc_ctx *proc_ctx, struct devdrv_dev_ctx *dev_ctx, u32 workmode)
{
	int ret;
	u32 curr_work_mode;

	mutex_lock(&dev_ctx->npu_power_up_off_mutex);
	curr_work_mode = dev_ctx->pm.work_mode;
	NPU_DRV_WARN("cur_secure_mode = 0x%x, workmode = 0x%x\n", curr_work_mode, workmode);
	ret = npu_pm_check_workmode(curr_work_mode, workmode);
	if (ret != 0) {
		mutex_unlock(&dev_ctx->npu_power_up_off_mutex);
		NPU_DRV_ERR("not support workmode, curr_work_mode_set = 0x%x work_mode = 0x%x\n", curr_work_mode, workmode);
		return ret;
	}

	ret = npu_powerdown(proc_ctx, dev_ctx);
	if (ret != 0) {
		mutex_unlock(&dev_ctx->npu_power_up_off_mutex);
		NPU_DRV_ERR("npu powerdown failed\n");
		return ret;
	}

	mutex_unlock(&dev_ctx->npu_power_up_off_mutex);

	return ret;
}

int npu_powerup(struct devdrv_proc_ctx *proc_ctx, struct devdrv_dev_ctx *dev_ctx, u32 work_mode)
{
	int ret;
	unsigned long flags;
	u32 *ts_status = NULL;
	struct devdrv_platform_info *plat_info = NULL;
	NPU_DRV_WARN("enter\n");

	plat_info = devdrv_plat_get_info();
	COND_RETURN_ERROR(plat_info == NULL, -ENODEV, "get plat_ops failed\n");
	COND_RETURN_ERROR(
	    atomic_cmpxchg(&dev_ctx->power_access, 1, 0) == 0, 0, "npu dev %d has already powerup!\n", dev_ctx->devid);

	NPU_DRV_INFO("firmware is loading");
	if (DEVDRV_PLAT_GET_FEAUTRE_SWITCH(plat_info, DEVDRV_FEATURE_KERNEL_LOAD_IMG) == 1) {
		ret = devdrv_load_cpu_fw();
		if (ret != 0) {
			atomic_inc(&dev_ctx->open_access);
			NPU_DRV_ERR("load cpu fw failed ret %d", ret);
			return ret;
		}
	} else {
		NPU_DRV_WARN("no need to load firmware, for npu/aicpu firmware is loaded in fastboot");
	}

	ret = DEVDRV_PLAT_GET_PM_POWER_UP(plat_info)(dev_ctx, work_mode);
	if (ret != 0) {
		atomic_inc(&dev_ctx->power_access);
		NPU_DRV_ERR("plat_ops npu_power_up failed\n");
		/* bbox : npu power up falied */
		if (npu_rdr_exception_is_count_exceeding((u32)RDR_EXC_TYPE_NPU_POWERUP_FAIL) == 0)
			rdr_system_error((u32)RDR_EXC_TYPE_NPU_POWERUP_FAIL, 0, 0);
		return ret;
	}

	npu_proc_clear_sqcq_info(proc_ctx);

	// update inuse
	dev_ctx->inuse.devid = dev_ctx->devid;
	spin_lock_irqsave(&dev_ctx->ts_spinlock, flags);
	dev_ctx->inuse.ai_core_num = DEVDRV_PLAT_GET_AICORE_MAX(plat_info);
	dev_ctx->inuse.ai_core_error_bitmap = 0;
	dev_ctx->inuse.ai_cpu_num = DEVDRV_PLAT_GET_AICPU_MAX(plat_info);
	dev_ctx->inuse.ai_cpu_error_bitmap = 0;
	spin_unlock_irqrestore(&dev_ctx->ts_spinlock, flags);
	dev_ctx->ts_work_status = 1;

	ts_status = devdrv_get_ts_work_status(dev_ctx->devid);
	if (ts_status != NULL)
		*ts_status = (u32)(work_mode != NPU_SEC ? DEVDRV_TS_WORK : DEVDRV_TS_SEC_WORK);
	NPU_DRV_WARN("npu dev %u hardware powerup successfully!\n", dev_ctx->devid);

	if (work_mode != NPU_SEC) {
		 /* bbox heart beat init in non_secure mode */
		ret = devdrv_heart_beat_init(dev_ctx);
		if (ret != 0)
			NPU_DRV_ERR("devdrv_heart_beat_init failed, ret = %d\n", ret);

		ret = npu_sync_ts_time();
		if (ret != 0)
			NPU_DRV_WARN("npu_sync_ts_time fail. ret = %d\n", ret);
#if defined (CONFIG_HISI_DPM_HWMON) && defined (CONFIG_NPU_DPM_ENABLED)
		npu_dpm_init();
#endif
	}

	atomic_dec(&dev_ctx->power_success);
	mutex_lock(&dev_ctx->npu_wake_lock_mutex);
	__pm_stay_awake(&dev_ctx->wakelock);
	mutex_unlock(&dev_ctx->npu_wake_lock_mutex);
	return 0;
}

int npu_powerdown(struct devdrv_proc_ctx *proc_ctx, struct devdrv_dev_ctx *dev_ctx)
{
	int ret;
	struct devdrv_platform_info *plat_info = NULL;
	u32 *ts_status = NULL;
	u32 is_secure = 0;

	NPU_DRV_WARN("enter\n");
	plat_info = devdrv_plat_get_info();
	if (plat_info == NULL) {
		NPU_DRV_ERR("get plat_ops failed\n");
		return -1;
	}

	if (DEVDRV_PLAT_GET_FEAUTRE_SWITCH(plat_info, DEVDRV_FEATURE_AUTO_POWER_DOWN) == DEVDRV_FEATURE_OFF) {
		NPU_DRV_INFO("npu auto power down switch off\n");
		dev_ctx->pm.work_mode = 0;
		return 0;
	}

	if (atomic_cmpxchg(&dev_ctx->power_success, 0, 1) == 1) {
		NPU_DRV_WARN("npu dev %d has already powerdown!\n", dev_ctx->devid);
		dev_ctx->pm.work_mode = 0;
		return 0;
	}

	if (!BITMAP_GET(dev_ctx->pm.work_mode, NPU_SEC)) {
		/* bbox heart beat exit */
		devdrv_heart_beat_exit(dev_ctx);
#if defined (CONFIG_HISI_DPM_HWMON) && defined (CONFIG_NPU_DPM_ENABLED)
		npu_dpm_exit();
#endif
	}

	if (BITMAP_GET(dev_ctx->pm.work_mode, NPU_SEC))
		is_secure = NPU_SEC;

	ret = DEVDRV_PLAT_GET_PM_POWER_DOWN(plat_info)(dev_ctx->devid, is_secure, &dev_ctx->power_stage);
	if (ret != 0) {
		NPU_DRV_ERR("plat_ops npu_power_down failed\n");
		/* bbox : npu power down falied */
		if (npu_rdr_exception_is_count_exceeding((u32)RDR_EXC_TYPE_NPU_POWERDOWN_FAIL) == 0)
			rdr_system_error((u32)RDR_EXC_TYPE_NPU_POWERDOWN_FAIL, 0, 0);
	} else {
		NPU_DRV_WARN("npu dev %d powerdown success!\n", dev_ctx->devid);
	}
	npu_proc_clear_sqcq_info(proc_ctx);

	dev_ctx->ts_work_status = 0;
	ts_status = devdrv_get_ts_work_status(dev_ctx->devid);
	if (ts_status != NULL)
		*ts_status = DEVDRV_TS_DOWN;
	dev_ctx->pm.work_mode = 0;
	atomic_inc(&dev_ctx->power_access);

	mutex_lock(&dev_ctx->npu_wake_lock_mutex);
	__pm_relax(&dev_ctx->wakelock);
	mutex_unlock(&dev_ctx->npu_wake_lock_mutex);
	return ret;
}

int devdrv_npu_ctrl_core(u32 dev_id, u32 core_num)
{
	int ret;
	struct devdrv_platform_info *plat_info = NULL;
	struct devdrv_dev_ctx *dev_ctx = NULL;

	plat_info = devdrv_plat_get_info();
	if (plat_info == NULL) {
		NPU_DRV_ERR("get plat_ops failed\n");
		return -EINVAL;
	}

	if (dev_id > NPU_DEV_NUM) {
		NPU_DRV_ERR("invalid device id %d\n", dev_id);
		return -EINVAL;
	}

	if ((core_num == 0) || (core_num > DEVDRV_PLAT_GET_AICORE_MAX(plat_info))) {
		NPU_DRV_ERR("invalid core num %d\n", core_num);
		return -EINVAL;
	}

	if (NULL == DEVDRV_PLAT_GET_RES_CTRL_CORE(plat_info)) {
		NPU_DRV_ERR("do not support ctrl core num %d\n", core_num);
		return -EINVAL;
	}

	dev_ctx = get_dev_ctx_by_id(dev_id);
	if (dev_ctx == NULL) {
		NPU_DRV_ERR("get device %d ctx fail\n", dev_id);
		return -EINVAL;
	}

	mutex_lock(&dev_ctx->npu_power_up_off_mutex);
	ret = DEVDRV_PLAT_GET_RES_CTRL_CORE(plat_info)(dev_ctx, core_num);
	mutex_unlock(&dev_ctx->npu_power_up_off_mutex);

	if (ret != 0) {
		NPU_DRV_ERR("ctrl device %d core num %d fail ret %d\n", dev_id, core_num, ret);
	} else {
		NPU_DRV_WARN("ctrl device %d core num %d success\n", dev_id, core_num);
	}

	return ret;
}
EXPORT_SYMBOL(devdrv_npu_ctrl_core);

int npu_pm_reboot(struct devdrv_proc_ctx *proc_ctx, struct devdrv_dev_ctx *dev_ctx)
{
	return 0;
}

void npu_pm_resource_init(struct devdrv_proc_ctx *proc_ctx, struct npu_power_manage *handle)
{
}

void npu_pm_delete_idle_timer(struct devdrv_dev_ctx *dev_ctx)
{
}

void npu_pm_add_idle_timer(struct devdrv_dev_ctx *dev_ctx)
{
}

int npu_interframe_enable(struct devdrv_proc_ctx *proc_ctx, uint32_t enable)
{
	return 0;
}

void npu_pm_adapt_init(struct devdrv_dev_ctx *dev_ctx)
{
}

