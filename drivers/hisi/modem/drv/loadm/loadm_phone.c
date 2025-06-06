/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * File Name       : loadm_phone.c
 * Description     : load modem image(ccore image),run in ccore
 * History         :
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <product_config.h>

#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#include <bsp_version.h>
#include <bsp_nvim.h>
#include <securec.h>
#include "load_image.h"
#include <bsp_print.h>
#include <bsp_cold_patch.h>
#define THIS_MODU mod_loadm
#include <../../adrv/adrv.h>

extern int modem_rfile_init(void);
extern int modem_nv_init(void);
extern int VOS_ModuleInit(void);

DECLARE_COMPLETION(g_modem_sysboot_start_complete);

#ifndef BSP_CONFIG_PHONE_TYPE
int bsp_need_loadmodem(void)
{
    return 1;
}
#endif

int modem_sysboot_start_init(void)
{
    int ret;

    u64 jiffies_old = 0;
    u64 jiffies_new = 0;
    u64 jiffies_time = 0;

    bsp_err("%s in.\n", __func__);

    if (bsp_get_version_info()) {
        if (BSP_BOARD_TYPE_SFT == bsp_get_version_info()->board_type) {
            ssleep(60);
        }
    }

    /* 如果是recovery down模式时不能解复位modem */
    if (!bsp_need_loadmodem()) {
        bsp_err("upload or charge mode, will not start modem.\n");
        return 0;
    }

    ret = modem_dir_init();
    if (ret) {
        bsp_err("modem_dir_init failed, ret %#x\n", ret);
        return ret;
    }

    bsp_err("modem_rfile_init \n");
    ret = modem_rfile_init();
    if (ret) {
        bsp_err("%s rfile init fail ", __func__);
    }

    bsp_err("modem_nv_init \n");
    ret = modem_nv_init();
    if (ret) {
        bsp_err("%s NV init fail ", __func__);
    }

    bsp_err("VOS_ModuleInit start\n");

    jiffies_old = get_jiffies_64();
    ret = VOS_ModuleInit();
    if (ret) {
        bsp_err("%s vos module init fail ", __func__);
    }
    jiffies_new = get_jiffies_64();
    jiffies_time = jiffies_new - jiffies_old;
    bsp_err("VOS_ModuleInit end tick 0x%llx\n", jiffies_time);
    bsp_err("bsp_load_modem_images \n");
    ret = bsp_load_modem_images();
    if (ret) {
        bsp_err("%s his_load_image_start_up fail ", __func__);
    }

    /* 只在MBB平台上需要发送复位请求中断 */
    ret = bsp_load_notify_ccpu_start();
    if (ret) {
        bsp_err("send ipc to unreset ccore failed, ret=0x%x\n", ret);
    } else {
        bsp_err("send ipc to unreset ccore succeed\n");
    }

    return 0;
}

static int modem_sysboot_wait_start(void *data)
{
    bsp_err("%s in.\n", __func__);
    /* 此任务等待被唤醒 */
    wait_for_completion(&g_modem_sysboot_start_complete);

    return modem_sysboot_start_init();
}

static const struct of_device_id his_modem_match_table[] = {
    { .compatible = "hisilicon,his_modem" },
    {},
};

#define MODEM_SYSBOOT_START 1

static ssize_t modem_sysboot_start_store(struct device *dev, struct device_attribute *attr, const char *buf,
                                         size_t count)
{
    int status = count;
    long modem_sysboot_start = 0;

    if ((kstrtol(buf, 10, &modem_sysboot_start) < 0))
        return -EINVAL;

    if (modem_sysboot_start == MODEM_SYSBOOT_START) {
        complete(&g_modem_sysboot_start_complete);
        bsp_info("modem_sysboot_start write ok \n");
    } else {
        bsp_err("modem_sysboot_start write is %x\n", (unsigned int)modem_sysboot_start);
    }
    return status;
}

static DEVICE_ATTR(modem_sysboot_start, S_IWUSR, NULL, modem_sysboot_start_store);

static DEVICE_ATTR(modem_image_patch_status, 0660, modem_imag_patch_status_show, modem_imag_patch_status_store);

static struct attribute *g_his_modem_attributes[] = {
        &dev_attr_modem_sysboot_start.attr,
        &dev_attr_modem_image_patch_status.attr,
        NULL
};

static const struct attribute_group g_his_modem_group = {
    .attrs = g_his_modem_attributes,
};

static int __init his_modem_probe(struct platform_device *pdev)
{
    int ret = 0;
    kthread_run(modem_sysboot_wait_start, NULL, "modem_sysboot_start");
    ret = sysfs_create_group(&pdev->dev.kobj, &g_his_modem_group);
    if (0 != ret) {
        bsp_err("create his modem sys node failed.\n");
        return -ENXIO;
    }
    return ret;
}

/* cov_verified_start */
static int his_modem_resume(struct platform_device *dev)
{
    return 0;
}

static void his_modem_shutdown(struct platform_device *dev)
{
    return;
}

static int his_modem_suspend(struct platform_device *dev, pm_message_t state)
{
    return 0;
}

static int his_modem_remove(struct platform_device *dev)
{
    return 0;
}

/* cov_verified_stop */
static struct platform_driver g_his_modem_drv = {
    .probe      = his_modem_probe,
    .remove     = his_modem_remove,
    .suspend    = his_modem_suspend,
    .resume     = his_modem_resume,
    .shutdown   = his_modem_shutdown,
    .driver     = {
        .name     = "his_modem",
        .owner    = THIS_MODULE,
        .of_match_table = his_modem_match_table,
        .probe_type = PROBE_FORCE_SYNCHRONOUS,
    },
};

int his_modem_init_driver(void)
{
    int ret = 0;

    ret = platform_driver_register(&g_his_modem_drv);
    if (ret) {
        bsp_err("register his_modem driver failed\n");
    }

    return ret;
}

/* cov_verified_start */
static void __exit his_modem_exit_driver(void)
{
    platform_driver_unregister(&g_his_modem_drv);
}
/* cov_verified_stop */
#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(his_modem_init_driver);
module_exit(his_modem_exit_driver);
#endif
MODULE_DESCRIPTION("HIS Balong  Modem load ");
MODULE_LICENSE("GPL");
