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

#ifndef __DMS_CORE_H__
#define __DMS_CORE_H__

#include "dms.h"
#include "vos.h"
#include "ps_common_def.h"
#include "mdrv.h"
#include "ps_tag.h"

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
#include "AtMntn.h"
#endif

#if (VOS_OS_VER == VOS_LINUX)
#include "linux/module.h"
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/dma-mapping.h>
#include <asm/dma-mapping.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>

#include <linux/module.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/moduleparam.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/version.h>

#include <net/netlink.h>
#include <net/sock.h>
#include <linux/pm_wakeup.h>
#else
#include "Linuxstub.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DMS_APP_DS_TASK_PRIORITY 84

#define DMS_UL_DATA_BUFF_SIZE 1024
#define DMS_UL_DATA_BUFF_NUM 16

/* 物理通道所对应的逻辑属性结构体 */
typedef struct {
    int       portHandle;
    VOS_UINT8 chanStat;
    VOS_UINT8 reserved[3];
} DMS_PHY_BearProperty;

typedef struct {
    DMS_READ_DATA_PFN    pfnRdDataCallback;
    DMS_PHY_BearProperty phyProperty[DMS_PHY_BEAR_LAST];
} DMS_MainInfo;

extern VOS_UINT32   g_ndisCfgFlag;
extern DMS_MainInfo g_dmsMainInfo;
extern pComRecv     g_acmReadData;

extern VOS_VOID DMS_Init(VOS_VOID);

extern DMS_PHY_BearProperty* DMS_GetPhyBearProperty(DMS_PHY_Bear phyBear);
extern DMS_MainInfo*         DMS_GetMainInfo(VOS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DMS_CORE_H__ */
