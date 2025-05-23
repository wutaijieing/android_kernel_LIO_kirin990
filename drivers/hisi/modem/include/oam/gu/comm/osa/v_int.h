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



#ifndef _VOS_INTERRUPT_H
#define _VOS_INTERRUPT_H

#include "v_typdef.h"
#ifdef VOS_SUPPORT_TSP
#include "mdrv_irq.h"
#include "mdrv_sys.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#if (VOS_TEN == VOS_OS_VER)
/* 自旋锁类型定义 */
typedef VOS_UINT32 VOS_SPINLOCK;
#endif

#if (VOS_RTOSCK == VOS_OS_VER)

/* 锁中断或者所任务嵌套信息的记录 */
typedef struct {
    VOS_UINT32 file;
    VOS_INT32  line;
    VOS_UINT32 slice;
} VOS_NestedLockInfo;

#if (VOS_DEBUG == VOS_CHECK_INTLOCK)

VOS_INT V_IntLock(VOS_UINT32 fileId, VOS_INT32 lineNo);

VOS_VOID V_IntUnlock(VOS_INT lockKey);

#define VOS_SplIMP() V_IntLock(VOS_FILE_ID, __LINE__)

#define VOS_Splx(s) V_IntUnlock((s))

#else

#ifdef VOS_SUPPORT_TSP
/*lint -esym(683,VOS_SplIMP)*/
#define VOS_SplIMP() (VOS_INT) mdrv_int_lock()

/*lint -esym(683,VOS_Splx)*/
#define VOS_Splx(s) mdrv_int_unlock((VOS_UINT32)(s))
#else
/*lint -esym(683,VOS_SplIMP)*/
#define VOS_SplIMP() (VOS_INT) SRE_IntLock()

/*lint -esym(683,VOS_Splx)*/
#define VOS_Splx(s) SRE_IntRestore((VOS_UINT32)(s))
#endif

#endif

#ifdef VOS_SUPPORT_TSP
/* 自旋锁类型定义 */
typedef struct mdrv_spin_lock VOS_SPINLOCK;

/* 自旋锁初始化 */
#define VOS_SpinLockInit(s) (mdrv_spin_lock_zalloc((s)))

/* 自旋锁获取 */
#define VOS_SpinLock(s) (mdrv_spin_lock((s)))

/* 自旋锁解锁 */
#define VOS_SpinUnlock(s) (mdrv_spin_unlock((s)))

/* 先禁止本地中断然后获取自旋锁 */
#define VOS_SpinLockIntLock(s, p) ((p) = mdrv_spin_lock_irqsave((s)))

/* 先使能本地中断然后释放自旋锁 */
#define VOS_SpinUnlockIntUnlock(s, p) (mdrv_spin_unlock_irqrestore((s), (p)))
#else

#ifdef __OS_RTOSCK_SMP__

/* 自旋锁类型定义 */
typedef struct SreSpinLock VOS_SPINLOCK;

/* 自旋锁初始化 */
#define VOS_SpinLockInit(s) (SRE_SplLockInit((s)))

/* 自旋锁获取 */
#define VOS_SpinLock(s) (SRE_SplLock((s)))

/* 自旋锁解锁 */
#define VOS_SpinUnlock(s) (SRE_SplUnlock((s)))

/* 先禁止本地中断然后获取自旋锁 */
#define VOS_SpinLockIntLock(s, p) ((p) = SRE_SplIrqLock((s)))

/* 先使能本地中断然后释放自旋锁 */
#define VOS_SpinUnlockIntUnlock(s, p) (SRE_SplIrqUnlock((s), (p)))

#else

/* 自旋锁类型定义 */
typedef VOS_UINT32 VOS_SPINLOCK;

/* 自旋锁初始化 */
#define VOS_SpinLockInit(s) ((VOS_VOID)0)

/* 自旋锁获取 */
#define VOS_SpinLock(s) ((VOS_VOID)0)

/* 自旋锁解锁 */
#define VOS_SpinUnlock(s) ((VOS_VOID)0)

/* 先禁止本地中断然后获取自旋锁 */
#define VOS_SpinLockIntLock(s, p) ((p) = (VOS_ULONG)VOS_SplIMP())

/* 先使能本地中断然后释放自旋锁 */
#define VOS_SpinUnlockIntUnlock(s, p) VOS_Splx((VOS_INT)(p))
#endif

#endif

#endif

#if (VOS_WIN32 == VOS_OS_VER)

VOS_VOID VOS_SplInit();

VOS_INT32 VOS_SplIMP();

VOS_VOID VOS_Splx(VOS_ULONG s);

/* 自旋锁类型定义 */
typedef VOS_UINT32 VOS_SPINLOCK;

/* 自旋锁初始化 */
#define VOS_SpinLockInit(s) ((VOS_VOID)0)

/* 自旋锁获取 */
#define VOS_SpinLock(s) ((VOS_VOID)0)

/* 自旋锁解锁 */
#define VOS_SpinUnlock(s) ((VOS_VOID)0)

/* 先禁止本地中断然后获取自旋锁 */
#define VOS_SpinLockIntLock(s, p) ((p) = (VOS_ULONG)VOS_SplIMP())

/* 先使能本地中断然后释放自旋锁 */
#define VOS_SpinUnlockIntUnlock(s, p) VOS_Splx((VOS_ULONG)(p))

#endif

#if (VOS_LINUX == VOS_OS_VER)

extern VOS_ULONG VOS_SplIMP(VOS_VOID);

extern VOS_VOID VOS_Splx(VOS_ULONG s);

/* 自旋锁类型定义 */
typedef spinlock_t VOS_SPINLOCK;

/* 自旋锁初始化 */
#define VOS_SpinLockInit(s) spin_lock_init((s))

/* 自旋锁获取 */
#define VOS_SpinLock(s) spin_lock((s))

/* 自旋锁解锁 */
#define VOS_SpinUnlock(s) spin_unlock((s))

/* 先禁止本地中断然后获取自旋锁 */
#define VOS_SpinLockIntLock(s, p) spin_lock_irqsave((s), (p))

/* 先使能本地中断然后释放自旋锁 */
#define VOS_SpinUnlockIntUnlock(s, p) spin_unlock_irqrestore((s), (p))

#endif

VOS_SPINLOCK *VOS_GetStaticMemSpinLock(VOS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_INTERRUPT_H */
