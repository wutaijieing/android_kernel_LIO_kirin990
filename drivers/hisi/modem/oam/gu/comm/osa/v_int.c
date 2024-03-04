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



#include "vos_config.h"
#include "v_typdef.h"
#include "v_io.h"
#include "v_timer.h"
#include "mdrv.h"
#include "pam_tag.h"


#define THIS_MODU mod_pam_osa

#if (VOS_RTOSCK == VOS_OS_VER)

#if (VOS_DEBUG == VOS_CHECK_INTLOCK)

/* VOS 支持的最大中断嵌套数目 */
#define VOS_NESTED_INTLOCK_MAX_NUM (128)

/* VOS 统计的锁中断时间间隔 单位slice 30us */
#define VOS_INTLOCK_MAX_INTERVAL (900)

/* 嵌套信息记录 */
typedef struct {
    VOS_UINT32                suffix; /* 数组下标 */
    VOS_NestedLockInfo nestedInfo[VOS_NESTED_INTLOCK_MAX_NUM];
} VOS_NestedIntlock;

VOS_NestedIntlock g_vosIntLockNestedInfo = {0};

/*
 * Description: Turn off the interrupt
 */
VOS_INT V_IntLock(VOS_UINT32 fileId, VOS_INT32 lineNo)
{
    VOS_INT flag;

    flag = (VOS_INT)SRE_IntLock();

    if (VOS_NESTED_INTLOCK_MAX_NUM <= g_vosIntLockNestedInfo.suffix) {
        g_vosIntLockNestedInfo.suffix++;

        mdrv_debug("<V_IntLock> intlock nested too big.F=%d L=%d.\n", (VOS_INT32)fileId, (VOS_INT32)lineNo);

        return flag;
    }

    g_vosIntLockNestedInfo.nestedInfo[g_vosIntLockNestedInfo.suffix].file  = fileId;
    g_vosIntLockNestedInfo.nestedInfo[g_vosIntLockNestedInfo.suffix].line   = lineNo;
    g_vosIntLockNestedInfo.nestedInfo[g_vosIntLockNestedInfo.suffix].slice = VOS_GetSlice();

    g_vosIntLockNestedInfo.suffix++;

    return flag;
}

/*
 * Description: Turn on the interrupt
 */
VOS_VOID V_IntUnlock(VOS_INT lockKey)
{
    g_vosIntLockNestedInfo.suffix--;

    if (VOS_NESTED_INTLOCK_MAX_NUM <= g_vosIntLockNestedInfo.suffix) {
        SRE_IntRestore((VOS_UINT32)lockKey);

        return;
    }

    if (VOS_INTLOCK_MAX_INTERVAL <
        (VOS_GetSlice() - g_vosIntLockNestedInfo.nestedInfo[g_vosIntLockNestedInfo.suffix].slice)) {
        mdrv_debug("<V_IntUnlock> intlock time too big.F=%d L=%d.\n",
                   (VOS_INT32)g_vosIntLockNestedInfo.nestedInfo[g_vosIntLockNestedInfo.suffix].file,
                   (VOS_INT32)g_vosIntLockNestedInfo.nestedInfo[g_vosIntLockNestedInfo.suffix].line);
    }

    SRE_IntRestore((VOS_UINT32)lockKey);

    return;
}

#endif

#endif

#if (VOS_WIN32 == VOS_OS_VER)

/*
 * Description: Initialize the interrupt
 */
VOS_VOID VOS_SplInit()
{
}

/*
 * Description: Turn off the interrupt
 */
VOS_INT32 VOS_SplIMP()
{
    return VOS_OK;
}

/*
 * Description: Turn on the interrupt
 */
VOS_VOID VOS_Splx(VOS_ULONG s)
{
}

#endif

#if (VOS_LINUX == VOS_OS_VER)

/*
 * Description: Turn off the interrupt
 */
VOS_ULONG VOS_SplIMP(VOS_VOID)
{
    VOS_ULONG flags;

    local_irq_save(flags);

    return flags;
}

/*
 * Description: Turn on the interrupt
 */
VOS_VOID VOS_Splx(VOS_ULONG s)
{
    local_irq_restore(s);
}
#endif

