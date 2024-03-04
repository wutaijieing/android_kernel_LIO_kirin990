/*
 * npu_adapter.c
 *
 * about npu adapter
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
#include "npu_adapter.h"

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/dma-direction.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/hisi/hisi_svm.h>

#include "npu_platform_resource.h"
#include "npu_platform_register.h"
#include "npu_common.h" // for NPU_SEC
#include "npu_pm_framework.h"
#include "npu_svm.h"
#include "dts/npu_reg.h"

tmp_log_buf_header_t *g_ts_buf_header_addr = NULL;
tmp_log_buf_header_t *g_aicpu_buf_header_addr = NULL;
struct task_struct *g_log_fetch_thread = NULL;

u32 g_tmp_log_switch = 0;

int devdrv_plat_res_fw_proc(const struct file *fp, loff_t fsize, loff_t pos, u64 fw_load_addr)
{
	mm_segment_t old_fs;
	ssize_t ret;
	old_fs = get_fs();

	NPU_DRV_INFO("firmware is loading, addr 0x%llx, pos %d\n", fw_load_addr, (u32)pos);
	set_fs((mm_segment_t)KERNEL_DS);
	ret = vfs_read((struct file *)fp, (char*)(uintptr_t)fw_load_addr, fsize, &pos);
	set_fs(old_fs);
	if (ret < 0) {
		NPU_DRV_ERR("vfs_read ret %d\n", (u32)ret);
		return false;
	}
	return true;
}

int npu_plat_powerup_smmu(struct device *dev)
{
	int ret;

	ret = hisi_smmu_poweron(0); /* 0: sysdma smmu */
	if (ret != 0) {
		NPU_DRV_ERR("hisi_smmu_poweron 0 failed.ret=%d\n", ret);
		return ret;
	}

	ret = hisi_smmu_poweron(1); /* 1: aicore smmu */
	if (ret != 0) {
		NPU_DRV_ERR("hisi_smmu_poweron 1 failed.ret=%d,\n", ret);
		goto aicore_failed;
	}
	return 0;
aicore_failed:
	(void)hisi_smmu_poweroff(0);
	return ret;
}
int devdrv_plat_svm_bind(struct devdrv_dev_ctx *dev_ctx, struct task_struct *task, void **svm_dev)
{
	*svm_dev = (void*)hisi_svm_bind_task(dev_ctx->npu_dev, task);
	if (*svm_dev == NULL) {
		NPU_DRV_ERR("hisi_svm_bind_task failed \n");
		/* likely bound by other process */
		return -EBUSY;
	}
	return 0;
}


int devdrv_plat_pm_powerup(struct devdrv_dev_ctx *dev_ctx, u32 work_mode)
{
	int ret = 0;
	u32 *stage = NULL;
	NPU_DRV_BOOT_TIME_TAG("start devdrv_plat_powerup_till_npucpu \n");
	COND_RETURN_ERROR(dev_ctx == NULL, -EINVAL, "invalid para\n");

	stage = &dev_ctx->power_stage;

	if (*stage == DEVDRV_PM_DOWN) {
		ret = devdrv_plat_powerup_till_npucpu(work_mode);
		if (ret != 0) {
			NPU_DRV_WARN("devdrv_plat_powerup_till_npucpu failed ,ret=%d\n", ret);
			goto failed;
		}
		*stage = DEVDRV_PM_NPUCPU;
	}
	NPU_DRV_BOOT_TIME_TAG("start devdrv_plat_powerup_till_ts \n");

	if (*stage == DEVDRV_PM_NPUCPU) {
		ret = devdrv_plat_powerup_till_ts(work_mode, DEVDRV_SC_TESTREG0_OFFSET);
		if (ret != 0) {
			NPU_DRV_WARN("devdrv_plat_powerup_till_ts failed ret=%d\n", ret);
			goto ts_failed;
		}
		*stage = DEVDRV_PM_TS;
	}
	NPU_DRV_BOOT_TIME_TAG("start npu_plat_powerup_smmu \n");

	if (*stage == DEVDRV_PM_TS) {
		// power up smmu in non_secure npu mode
		if (work_mode != NPU_SEC) {
			ret = npu_plat_powerup_smmu(dev_ctx->npu_dev);
			COND_GOTO_ERROR(ret != 0, smmu_failed, ret, ret,
				"npu_plat_powerup_smmu failed ret=%d\n", ret);
		} else {
			NPU_DRV_WARN("secure npu power up ,no need to power up smmu"
			"in linux non_secure world, smmu power up will"
			"be excuted on tee secure world \n");
		}
	}
	*stage = DEVDRV_PM_UP;
	devdrv_plat_set_npu_power_status(DRV_NPU_POWER_ON_FLAG);
	NPU_DRV_WARN("devdrv_plat_pm_powerup success \n");
	return 0;

smmu_failed:
	(void)npu_plat_powerdown_ts(DEVDRV_SC_TESTREG8_OFFSET, work_mode);
	// continue even if gic grace exit failed
	*stage = DEVDRV_PM_NPUCPU;
ts_failed:
	if (npu_plat_powerdown_npucpu(0x1 << 5, DEVDRV_CFG_STAT0_OFFSET) != 0)
		return ret;

	*stage = DEVDRV_PM_DOWN;
failed:
	return ret;
}

int devdrv_plat_pm_open(uint32_t devid)
{
	if (devdrv_plat_ioremap(DEVDRV_REG_POWER_STATUS) != 0) {
		NPU_DRV_ERR("devdrv_plat_ioremap failed\n");
		return -1;
	}
	return 0;
}

int devdrv_plat_pm_release(uint32_t devid)
{
	int ret = devdrv_clear_pid_ssid_table(devid, 1);
	devdrv_plat_iounmap(DEVDRV_REG_POWER_STATUS);
	if (ret != 0) {
		NPU_DRV_ERR("devdrv_clear_pid_ssid_table failed\n");
		return ret;
	}
	return 0;
}

int devdrv_plat_res_mailbox_send(void *mailbox, int mailbox_len, const void *message, int message_len)
{
	u8 *message_buf = NULL;

	if (message_len > mailbox_len) {
		NPU_DRV_ERR("message len =%d, too long", message_len);
		return -1;
	}

	message_buf = vmalloc(mailbox_len);
	if (message_buf == NULL) {
		NPU_DRV_ERR("message buf alloc failed");
		return -1;
	}
	memset(message_buf, 0, mailbox_len);
	memcpy(message_buf, message, message_len);
	memcpy(mailbox, message_buf, mailbox_len);
	mb();
	vfree(message_buf);
	return 0;
}

void __iomem *devdrv_plat_sram_remap(struct platform_device *pdev, resource_size_t sram_addr, resource_size_t sram_size)
{
	return devm_ioremap_nocache(&pdev->dev, sram_addr, sram_size);
}

void devdrv_plat_sram_unmap(struct platform_device *pdev, void* sram_addr)
{
	devm_iounmap(&pdev->dev, (void __iomem*)sram_addr);
	return;
}

int npu_plat_poweroff_smmu(uint32_t devid)
{
	devdrv_clear_pid_ssid_table(devid, 0);

	(void)hisi_smmu_poweroff(0); /* 0: sysdma smmu */
	(void)hisi_smmu_poweroff(1); /* 1: aicore smmu */
	return 0;
}

int devdrv_plat_pm_powerdown(uint32_t devid, u32 is_secure, u32 *stage)
{
	int ret = 0;

	devdrv_plat_set_npu_power_status(DRV_NPU_POWER_OFF_FLAG);
	if (*stage == DEVDRV_PM_UP) {
		if (is_secure != NPU_SEC) {
			ret = npu_plat_poweroff_smmu(devid);
			if (ret != 0)
				NPU_DRV_ERR("npu_plat_poweroff_smmu FAILED \n");
		} else {
			NPU_DRV_WARN("secure npu power down ,no need to power down smmu"
				"in linux non_secure world, smmu power down has"
				"been excuted on tee secure world \n");
		}
		*stage = DEVDRV_PM_TS;
	}

	if (*stage == DEVDRV_PM_TS) {
		ret = npu_plat_powerdown_ts(DEVDRV_SC_TESTREG8_OFFSET, is_secure);
		// continue even if gic grace exit failed
		*stage = DEVDRV_PM_NPUCPU;
	}
	if (*stage == DEVDRV_PM_NPUCPU) {
		ret = npu_plat_powerdown_npucpu(0x1 << 5, DEVDRV_CFG_STAT0_OFFSET);
		if (ret != 0)
			NPU_DRV_ERR("devdrv_plat_pm_powerdown FAILED \n");

		ret = npu_plat_powerdown_nputop();
		if (ret != 0)
			NPU_DRV_ERR("npu_plat_powerdown_nputop FAILED \n");
	}
	*stage = DEVDRV_PM_DOWN;
	NPU_DRV_WARN("devdrv_plat_pm_powerdown success \n");
	return ret;
}

int npu_save_tmp_log2fs(const void *buf, u32 len, char* log_path)
{
	struct file *filp;
	loff_t offset = 0;
	int ret;

	filp = filp_open(log_path, O_RDWR | O_APPEND | O_CREAT, 0640);
	if (IS_ERR(filp)) {
		NPU_DRV_ERR("open file %s failed,error=%ld", log_path, PTR_ERR(filp));
		return -1;
	}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
	ret = kernel_write(filp, buf, len, offset);
#else
	ret = kernel_write(filp, buf, len, &offset);
#endif
	if (ret < 0) {
		NPU_DRV_ERR("write log failed,ret=%d", ret);
		filp_close(filp, NULL);
		return -1;
	}

	filp_close(filp, NULL);
	return ret;
}

void npu_tmp_log_write(tmp_log_buf_header_t *log_header, char *log_path)
{
	char *log_buffer_base = NULL;
	char *log_bufer = NULL;
	u32 len;
	u32 buf_write;
	u32 buf_len;

	log_buffer_base = (char*)log_header + TMP_LOG_BUF_HEAD;

	buf_write = atomic_read((atomic_t *)&(log_header->buf_write));
	buf_len = log_header->buf_len - TMP_LOG_BUF_HEAD;
	len = (buf_write + buf_len - log_header->buf_read) % buf_len;

	if (len != 0) {
		log_bufer = log_buffer_base + log_header->buf_read;
		if (log_header->buf_read < buf_write) {
			(void)npu_save_tmp_log2fs(log_bufer, len, log_path);
			log_header->buf_read += len;
		} else {
			(void)npu_save_tmp_log2fs(log_bufer, buf_len - log_header->buf_read, log_path);

			log_bufer = log_buffer_base;
			(void)npu_save_tmp_log2fs(log_bufer, buf_write, log_path);
			log_header->buf_read = buf_write;
		}
	}
	return;
}

static int devdrv_npu_tmp_log_run(void *args)
{
	while (!kthread_should_stop()) {
		npu_tmp_log_write(g_ts_buf_header_addr, "/data/hisi_logs/ts_log.log");
		npu_tmp_log_write(g_aicpu_buf_header_addr, "/data/hisi_logs/aicpu_log.log");
		schedule_timeout_killable(100);
	}
	return 0;
}

int npu_log_stop(void)
{
	int ret;

	iounmap(g_ts_buf_header_addr);
	ret = kthread_stop(g_log_fetch_thread);
	if (ret != 0)
		NPU_DRV_ERR("log thread stop fail, ret = %d", ret);

	return ret;
}

int npu_log_init(void)
{
	char *log_buffer = NULL;

	/* this kernel thread should be created only once */
	if (g_tmp_log_switch == 0) {
		g_tmp_log_switch = 1;
		log_buffer = (char *)ioremap_wc(TMP_LOG_BUFER_BASE, TMP_LOG_BUF_SIZE);
		if (IS_ERR(log_buffer)) {
			NPU_DRV_ERR("remap buffer fail");
			return 0;
		}

		g_ts_buf_header_addr = (tmp_log_buf_header_t*)log_buffer;
		g_ts_buf_header_addr->buf_read = 0;
		g_ts_buf_header_addr->buf_write = 0;
		/* header buf_len with "BUF_HEAD"
		   cmd buf_len without "BUF_HEAD" */
		g_ts_buf_header_addr->buf_len = TMP_LOG_BUF_SIZE / 2; /* 512k */

		/* AICPU uses header buf_len
		   TSCPU uses cmd buf_len */
		g_aicpu_buf_header_addr = (tmp_log_buf_header_t*)(log_buffer + TMP_LOG_BUF_SIZE / 2);
		g_aicpu_buf_header_addr->buf_read = 0;
		g_aicpu_buf_header_addr->buf_write = 0;
		g_aicpu_buf_header_addr->buf_len = TMP_LOG_BUF_SIZE / 2; /* 512k */

		g_log_fetch_thread = kthread_create(devdrv_npu_tmp_log_run, NULL, "ts_aicpu_log");
		if (IS_ERR(g_log_fetch_thread)) {
			NPU_DRV_ERR("create ts log thread fail.(devid=%d)\n", 0);
		} else {
			kthread_bind(g_log_fetch_thread, 0);
			wake_up_process(g_log_fetch_thread);
		}
	}
	return 0;
}

int devdrv_plat_res_ctrl_core(struct devdrv_dev_ctx *dev_ctx, u32 core_num)
{
	// do not support ctrl core
	UNUSED(dev_ctx);
	UNUSED(core_num);
	return 0;
}

int devdrv_testreg_poll_wait(u64 offset)
{
	UNUSED(offset);
	return -1;
}

int devdrv_testreg_clr(u64 offset)
{
	UNUSED(offset);
	return 0;
}

int devdrv_plat_handle_irq_tophalf(u32 irq_index)
{
	UNUSED(irq_index);
	return 0;
}

int devdrv_plat_attach_sc(int fd, u64 offset, u64 size)
{
	UNUSED(fd);
	UNUSED(offset);
	UNUSED(size);
	return 0;
}

int devdrv_hisi_smmu_evt_register_notify(struct notifier_block *n)
{
	return hisi_smmu_evt_register_notify(n);
}

int devdrv_hisi_smmu_evt_unregister_notify(struct notifier_block *n)
{
	return hisi_smmu_evt_unregister_notify(n);
}

int npu_plat_switch_hwts_aicore_pool(struct devdrv_dev_ctx *dev_ctx, hwts_switch_pool_info_t *switch_info)
{
	UNUSED(dev_ctx);
	UNUSED(switch_info);
	return 0;
}

void npu_plat_aicore_pmu_enable(uint32_t subip_set)
{
	UNUSED(subip_set);
	return;
}

