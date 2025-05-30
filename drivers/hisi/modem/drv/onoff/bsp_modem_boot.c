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

/*lint --e{528,537,715} */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <osl_spinlock.h>
#include "mdrv_errno.h"
#include <bsp_om_enum.h>
#include <bsp_reset.h>
#include <bsp_dump.h>
#include <gunas_errno.h>
#include <bsp_slice.h>
#include <linux/ctype.h>
#include "bsp_llt.h"
#include <securec.h>
#include "mdrv_vcom.h"
#include <bsp_icc.h>
#include <bsp_print.h>
#define THIS_MODU mod_onoff
struct balong_power_plat_data {
    u32 modem_state;
};

enum modem_state_index {
    MODEM_NOT_READY = 0,
    MODEM_READY,
    MODEM_INVALID,
};

static struct balong_power_plat_data *g_balong_driver_plat_data = NULL;
static const char *g_modem_state_str[] = {
    "MODEM_STATE_OFF\n",
    "MODEM_STATE_READY\n",
    "MODEM_STATE_INVALID\n",
};

/* To make modem poweroff called only once when there are two rilds. */
static int g_modem_power_off_flag = 0;
static int g_modem_power_on_flag = 0;
spinlock_t g_modem_power_spinlock;


int mdrv_set_modem_state(unsigned int state)
{
    if (g_balong_driver_plat_data == NULL) {
        bsp_err("Balong_power %s:%d not init.\n", __FUNCTION__, __LINE__);
        return -EINVAL;
    }
    if (state >= MODEM_INVALID) {
        bsp_err("Balong_power %s:%d invalid state 0x%x.\n", __FUNCTION__, __LINE__, state);
        return -EINVAL;
    }

    g_balong_driver_plat_data->modem_state = state;

    if (g_balong_driver_plat_data->modem_state == MODEM_READY) {
        bsp_err("Balong_power  set state %d ,time slice %d\n", state, bsp_get_elapse_ms());
    } else {
        bsp_err("Balong_power  set state %d\n", state);
    }
    return 0;
}

static ssize_t balong_power_get(struct device *dev, struct device_attribute *attr, char *buf)
{
    ssize_t len;

    if (g_balong_driver_plat_data == NULL) {
        bsp_err("Balong_power %s:%d not init.\n", __FUNCTION__, __LINE__);
        return 0;
    }
    if (g_balong_driver_plat_data->modem_state >= MODEM_INVALID) {
        bsp_err("Balong_power : %s:%d Invalid state 0x%x now is set.\n", __FUNCTION__, __LINE__,
                g_balong_driver_plat_data->modem_state);
        return 0;
    }

    len = snprintf_s(buf, strlen(g_modem_state_str[g_balong_driver_plat_data->modem_state]) + 2,
                     strlen(g_modem_state_str[g_balong_driver_plat_data->modem_state]) + 1, "%s\n",
                     g_modem_state_str[g_balong_driver_plat_data->modem_state]);
    if (len < 0) {
        bsp_err("Balong_power %s:snprintf fail.\n", __FUNCTION__);
    }

    return len;
}
EXPORT_SYMBOL_GPL(balong_power_get);

ssize_t modem_reset_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    long state;
    unsigned long lock_flag;

    if (count > 3 || count <= 1) {
        bsp_err("buf len err: %d\n", (int)count);
        return -EINVAL;
    }
    if (count == 2 && (!isdigit(buf[0]))) {
        bsp_err("count = 2,buf err: %c\n", buf[0]);
        return -EINVAL;
    }
    if (count == 3 && (!isdigit(buf[0]) || (!isdigit(buf[1])))) {
        bsp_err("count = 3,buf err: %c%c\n", buf[0], buf[1]);
        return -EINVAL;
    }
    if (count == 2) {
        bsp_info("Power set to %c\n", buf[0]);
    } else if (count == 3) {
        bsp_info("Power set to %c%c\n", buf[0], buf[1]);
    }

    state = (count == 2) ? (buf[0] - '0') : (((buf[0] - '0') * 10) + (buf[1] - '0'));
    bsp_err("count = %lu\n", (unsigned long)count);

    /* 整机复位对rild为桩,应该整机复位 */
    if (!bsp_reset_is_connect_ril()) {
        bsp_err("<modem_reset_set>: modem reset not to be connected to ril\n");
        if (!is_in_llt()) {
            system_error(DRV_ERRNO_RESET_REBOOT_REQ, 0, 0, NULL, 0);
        }
        return (ssize_t)count;
    }

    spin_lock_irqsave(&g_modem_power_spinlock, lock_flag); /*lint !e550*/
    if ((g_modem_power_off_flag) && (!g_modem_power_on_flag) && (state != BALONG_MODEM_ON)) {
        bsp_err("modem has been power off,please power on,don't reset!\n");
        spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
        return (ssize_t)count;
    }

    /* 切卡 */
    if (state == BALONG_MODEM_RESET) {
        bsp_err("modem reset %d\n", BALONG_MODEM_RESET);
        spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
        if (!is_in_llt()) {
            system_error(DRV_ERRNO_RESET_SIM_SWITCH, 0, 0, NULL, 0);
        }
        return (ssize_t)count;
    } else if (state == BALONG_MODEM_OFF) {
        /* To make modem poweroff called only once when there are two rilds. */
        if (g_modem_power_off_flag) {
            bsp_err("Balong_power: modem power off has been called! \n");
            spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
            return (ssize_t)count;
        }
        bsp_modem_power_off();
        bsp_err("modem power off %d\n", BALONG_MODEM_OFF);
        g_modem_power_off_flag = 1;
        spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
        return (ssize_t)count;
    } else if (state == BALONG_MODEM_ON) { /* TODO: 是否需要上电，根HIFI什么关系 */
        if ((g_modem_power_off_flag) && (!g_modem_power_on_flag)) {
            bsp_modem_power_on();
            g_modem_power_on_flag = 1;
            bsp_err("modem power on %d\n", BALONG_MODEM_ON);
        } else {
            bsp_err("modem now is power on!\n");
        }
    } else if (state == BALONG_MODEM_RILD_SYS_ERR) {
        bsp_err("modem reset using system_error by rild %d\n", BALONG_MODEM_RILD_SYS_ERR);
        spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
        if (!is_in_llt()) {
            system_error(NAS_REBOOT_MOD_ID_RILD, 0, 0, NULL, 0);
        }
        return (ssize_t)count;
    } else if (state == BALONG_MODEM_3RD_SYS_ERR) {
        bsp_err("modem reset using system_error by 3rd modem %d\n", DRV_ERRNO_RESET_3RD_MODEM);
        spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
        if (!is_in_llt()) {
            system_error(DRV_ERRNO_RESET_3RD_MODEM, 0, 0, NULL, 0);
        }
        return (ssize_t)count;
    } else {
        bsp_err("Balong_power : invalid code to balong power !!!!\n");
        spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
        return (ssize_t)count;
    }
    spin_unlock_irqrestore(&g_modem_power_spinlock, lock_flag);
    return (ssize_t)count;
}
EXPORT_SYMBOL_GPL(modem_reset_set);

ssize_t modem_state_write(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return (ssize_t)count;
}
EXPORT_SYMBOL_GPL(modem_state_write);
/*lint -save -e* */
static DEVICE_ATTR(state, 0660, balong_power_get, modem_reset_set);
static DEVICE_ATTR(modem_state, 0660, balong_power_get, modem_state_write);
/*lint -restore */
static int __init bsp_power_probe(struct platform_device *pdev)
{
    u32 ret;

    g_balong_driver_plat_data = pdev->dev.platform_data;

    ret = (u32)device_create_file(&(pdev->dev), &dev_attr_state);
    ret |= (u32)device_create_file(&(pdev->dev), &dev_attr_modem_state);
    if (ret) {
        bsp_err("fail to creat modem boot sysfs\n");
        return ret;
    }

    return (int)ret;
}

static struct balong_power_plat_data g_balong_power_plat_data = {
    .modem_state = MODEM_NOT_READY,
};

static struct platform_device g_balong_power_device = {
    .name = "balong_power",
    .id = -1,
    .dev = {
        .platform_data = &g_balong_power_plat_data,
    }, /* lint !e785 */
}; /* lint !e785 */

static struct platform_driver g_balong_power_drv = {
    .probe      = bsp_power_probe,
    .driver     = {
        .name     = "balong_power",
        .owner    = THIS_MODULE, /* lint !e64 */
        .probe_type = PROBE_FORCE_SYNCHRONOUS,
    }, /* lint !e785 */
}; /* lint !e785 */

int bsp_modem_boot_init(void)
{
    int ret;

    ret = platform_device_register(&g_balong_power_device);
    if (ret) {
        bsp_err("register his_modem boot device failed.\n");
        return ret;
    }
    spin_lock_init(&g_modem_power_spinlock);

    ret = platform_driver_register(&g_balong_power_drv); /*lint !e64*/
    if (ret) {
        bsp_err("register his_modem boot driver failed.\n");
        platform_device_unregister(&g_balong_power_device);
    }

    return ret;
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(bsp_modem_boot_init);
#endif
