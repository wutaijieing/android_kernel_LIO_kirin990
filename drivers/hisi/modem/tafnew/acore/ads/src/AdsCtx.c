/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#include "taf_type_def.h"
#include "AdsCtx.h"
#include "AdsUpLink.h"
#include "AdsDownLink.h"
#include "AdsFilter.h"
#include "AdsDebug.h"
#include "mdrv.h"
#include "AdsMntn.h"
#include "securec.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_ADS_CTX_C

VOS_UINT32 g_adsULTaskId        = 0; /* ADS上行任务ID */
VOS_UINT32 g_adsDlTaskId        = 0; /* ADS下行任务ID */
VOS_UINT32 g_adsULTaskReadyFlag = 0; /* ADS上行任务运行状态 */
VOS_UINT32 g_adsDlTaskReadyFlag = 0; /* ADS下行任务运行状态 */

/* ADS模块的上下文 */
ADS_Ctx g_adsCtx;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
#ifdef CONFIG_ARM64
VOS_UINT64 g_adsDmaMask = 0xffffffffffffffffULL;
#else
VOS_UINT64 g_adsDmaMask = 0xffffffffULL;
#endif
struct device g_adsDmaDev;
#endif

struct device *g_dmaDev = VOS_NULL_PTR;

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)

VOS_UINT32 ADS_UL_InsertPc5Queue(IMM_Zc *immZc)
{
    VOS_UINT32 nonEmptyEvent;
    VOS_UINT32 allUlQueueDataNum;
    VOS_UINT32 slice;
    VOS_UINT   queueLen;

    nonEmptyEvent = VOS_FALSE;

    queueLen = IMM_ZcQueueLen(ADS_UL_GET_PC5_QUEUE_HEAD());
    if (queueLen >= ADS_UL_GET_PC5_MAX_QUEUE_LEN()) {
        ADS_DBG_UL_PC5_PKT_ENQUE_FAIL_NUM(1);
        return VOS_ERR;
    }

    /* 插入队列前将数据打上时间戳 */
    slice = VOS_GetSlice();
    ADS_UL_SAVE_SLICE_TO_IMM(immZc, slice);

    /* 插入队列 */
    IMM_ZcQueueTail(ADS_UL_GET_PC5_QUEUE_HEAD(), immZc);
    ADS_DBG_UL_PC5_PKT_ENQUE_SUCC_NUM(1);

    /* 队列由空变为非空 */
    if (1 == IMM_ZcQueueLen(ADS_UL_GET_PC5_QUEUE_HEAD())) {
        nonEmptyEvent = VOS_TRUE;
    }

    if (VOS_TRUE == nonEmptyEvent) {
        ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);
        ADS_DBG_UL_PC5_QUE_NON_EMPTY_TRIG_EVENT(1);
    } else {
        allUlQueueDataNum = ADS_UL_GetAllQueueDataNum();

        if (ADS_UL_IS_REACH_THRESHOLD(allUlQueueDataNum, ADS_UL_GET_SENDING_FLAG())) {
            ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);
            ADS_DBG_UL_QUE_HIT_THRES_TRIG_EVENT(1);
        }
    }

    return VOS_OK;
}


IMM_Zc* ADS_UL_GetPc5QueueNode(VOS_VOID)
{
    IMM_Zc *node = VOS_NULL_PTR;

    /* 获取队列第一个元素 */
    node = IMM_ZcDequeueHead(ADS_UL_GET_PC5_QUEUE_HEAD());

    return node;
}


VOS_VOID ADS_UL_ClearPc5Queue(VOS_VOID)
{
    /* 销毁队列中的数据 */
    ADS_UL_ClearQueue(ADS_UL_GET_PC5_QUEUE_HEAD());

    return;
}


VOS_VOID ADS_InitPc5Ctx(VOS_VOID)
{
    /* 初始化Pc5上下文 */
    IMM_ZcQueueHeadInit(ADS_UL_GET_PC5_QUEUE_HEAD());
    IMM_ZcQueueHeadInit(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD());

    ADS_UL_SET_PC5_MAX_QUEUE_LEN(ADS_UL_MAX_QUEUE_LENGTH);
    ADS_DL_SET_PC5_FRAGS_QUEUE_MAX_LEN(ADS_DL_PC5_FRAGS_QUE_MAX_LEN);

    /* 向底软注册PC5上行发送接口 */
    (VOS_VOID)mdrv_pcv_cb_register(ADS_Pc5DataIngress);
}
#endif


VOS_UINT32 ADS_UL_CheckAllQueueEmpty(VOS_UINT32 instanceIndex)
{
    VOS_UINT32  i;
    ADS_UL_Ctx *adsUlCtx = VOS_NULL_PTR;

    adsUlCtx = ADS_GetUlCtx(instanceIndex);

    for (i = ADS_RAB_ID_MIN; i < ADS_RAB_ID_MAX + 1; i++) {
        if (VOS_FALSE != adsUlCtx->adsUlQueue[i].isQueueValid) {
            break;
        }
    }

    /* 所有的PDP都去激活　 */
    if ((ADS_RAB_ID_MAX + 1) != i) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID ADS_UL_SetProtectTmrLen(VOS_UINT32 timerLen)
{
    g_adsCtx.adsIpfCtx.protectTmrLen = timerLen;
    return;
}


VOS_UINT32 ADS_UL_IsQueueExistent(VOS_UINT32 instanceIndex, VOS_UINT32 rabId)
{
    /* 队列为空 */
    if (VOS_NULL_PTR == ADS_UL_GET_QUEUE_LINK_PTR(instanceIndex, rabId)) {
        return VOS_ERR;
    } else {
        return VOS_OK;
    }
}


VOS_UINT32 ADS_UL_IsAnyQueueExist(VOS_VOID)
{
    VOS_UINT32 instance;
    VOS_UINT32 rabId;

    for (instance = 0; instance < ADS_INSTANCE_MAX_NUM; instance++) {
        for (rabId = ADS_RAB_ID_MIN; rabId <= ADS_RAB_ID_MAX; rabId++) {
            if (VOS_OK == ADS_UL_IsQueueExistent(instance, rabId)) {
                return VOS_TRUE;
            }
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 ADS_UL_InsertQueue(VOS_UINT32 instance, IMM_Zc *immZc, VOS_UINT32 rabId)
{
    VOS_UINT32              nonEmptyEvent;
    VOS_UINT32              allUlQueueDataNum;
    VOS_UINT32              slice;
    VOS_UINT                queueLen;
    VOS_ULONG               lockLevel;
    ADS_CDS_IpfPktTypeUint8 pktType;

    nonEmptyEvent = VOS_FALSE;

    /* 此接口不释放pstData，由上层模块根据函数返回值判断是否需要释放内存 */

    /* 队列加锁 */
    /*lint -e571*/
    VOS_SpinLockIntLock(ADS_UL_GET_QUEUE_LINK_SPINLOCK(instance, rabId), lockLevel);
    /*lint +e571*/

    /* 结点存在，但队列不存在 */
    if (VOS_OK != ADS_UL_IsQueueExistent(instance, rabId)) {
        /* 队列操作完成解锁 */
        VOS_SpinUnlockIntUnlock(ADS_UL_GET_QUEUE_LINK_SPINLOCK(instance, rabId), lockLevel);
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_InsertQueue:the queue is not ext!");
        ADS_DBG_UL_PKT_ENQUE_FAIL_NUM(1);
        return VOS_ERR;
    }

    queueLen = IMM_ZcQueueLen(ADS_UL_GET_QUEUE_LINK_PTR(instance, rabId));
    if (queueLen >= ADS_UL_GET_MAX_QUEUE_LENGTH(instance)) {
        /* 队列操作完成解锁 */
        VOS_SpinUnlockIntUnlock(ADS_UL_GET_QUEUE_LINK_SPINLOCK(instance, rabId), lockLevel);
        ADS_DBG_UL_PKT_ENQUE_FAIL_NUM(1);
        return VOS_ERR;
    }

    /* 保存ModemId/PktType/RabId到IMM */
    pktType = ADS_UL_GET_QUEUE_PKT_TYPE(instance, rabId);
    ADS_UL_SAVE_MODEMID_PKTTYEP_RABID_TO_IMM(immZc, instance, pktType, rabId);

    /* 插入队列前将数据打上时间戳 */
    slice = VOS_GetSlice();
    ADS_UL_SAVE_SLICE_TO_IMM(immZc, slice);

    /* 插入队列 */
    IMM_ZcQueueTail(ADS_UL_GET_QUEUE_LINK_PTR(instance, rabId), immZc);
    ADS_DBG_UL_PKT_ENQUE_SUCC_NUM(1);

    /* 队列由空变为非空 */
    if (1 == IMM_ZcQueueLen(ADS_UL_GET_QUEUE_LINK_PTR(instance, rabId))) {
        nonEmptyEvent = VOS_TRUE;
    }

    /* 队列操作完成解锁 */
    VOS_SpinUnlockIntUnlock(ADS_UL_GET_QUEUE_LINK_SPINLOCK(instance, rabId), lockLevel);

    allUlQueueDataNum = ADS_UL_GetAllQueueDataNum();

    if (VOS_TRUE == ADS_UL_GET_THRESHOLD_ACTIVE_FLAG()) {
        /*
         * (1).队列中数据已到攒包门限且当前没有在处理数据,触发上行缓存缓存处理事件
         * (2).队列由空变为非空时启动数据统计定时器以及保护定时器
         */
        ADS_UL_ADD_STAT_PKT_NUM(1);

        if (ADS_UL_IS_REACH_THRESHOLD(allUlQueueDataNum, ADS_UL_GET_SENDING_FLAG())) {
            ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);
            ADS_DBG_UL_QUE_HIT_THRES_TRIG_EVENT(1);
        }

        /* 队列由空变为非空 */
        if (VOS_TRUE == nonEmptyEvent) {
            ADS_StartTimer(TI_ADS_UL_DATA_STAT, ADS_UL_GET_STAT_TIMER_LEN());
            ADS_StartUlSendProtectTimer();
        }
    } else {
        /*
         * (1).队列由空变为非空时触发上行缓存处理事件
         * (2).队列中数据已到攒包门限的整数倍且当前没有在处理数据
         *     触发上行缓存缓存处理事件
         */
        if (VOS_TRUE == nonEmptyEvent) {
            ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);
            ADS_DBG_UL_QUE_NON_EMPTY_TRIG_EVENT(1);
        } else {
            if (ADS_UL_IS_REACH_THRESHOLD(allUlQueueDataNum, ADS_UL_GET_SENDING_FLAG())) {
                ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);
                ADS_DBG_UL_QUE_HIT_THRES_TRIG_EVENT(1);
            }
        }
    }

    return VOS_OK;
}


VOS_UINT32 ADS_UL_GetInstanceAllQueueDataNum(VOS_UINT32 instanceIndex)
{
    VOS_UINT32   i;
    VOS_UINT32   totalNum;
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instanceIndex]);

    totalNum = 0;

    for (i = ADS_RAB_ID_MIN; i < ADS_RAB_ID_MAX + 1; i++) {
        if (VOS_NULL_PTR != adsSpecCtx->adsUlCtx.adsUlQueue[i].adsUlLink) {
            totalNum += adsSpecCtx->adsUlCtx.adsUlQueue[i].adsUlLink->qlen;
        }
    }

    return totalNum;
}


VOS_UINT32 ADS_UL_GetAllQueueDataNum(VOS_VOID)
{
    VOS_UINT32 totalNum;
    VOS_UINT32 i;

    totalNum = 0;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++) {
        totalNum = totalNum + ADS_UL_GetInstanceAllQueueDataNum(i);
    }

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
    totalNum += IMM_ZcQueueLen(ADS_UL_GET_PC5_QUEUE_HEAD());
#endif

    return totalNum;
}


/*lint -sem(ADS_UL_SetQueue,custodial(4))*/
VOS_UINT32 ADS_UL_CreateQueue(VOS_UINT32 instanceIndex, VOS_UINT32 rabId, ADS_QciTypeUint8 prio,
                              ADS_CDS_IpfPktTypeUint8 pktType, VOS_UINT8 uc1XorHrpdUlIpfFlag)
{
    IMM_ZcHeader *ulQueue  = VOS_NULL_PTR;
    ADS_UL_Ctx   *adsUlCtx = VOS_NULL_PTR;

    adsUlCtx = ADS_GetUlCtx(instanceIndex);

    /* RabId的队列已经存在 */
    if (VOS_OK == ADS_UL_IsQueueExistent(instanceIndex, rabId)) {
        /* 对应的调度优先级也一样或者是比之前的要低，不更新QCI直接返回OK */
        if (prio >= adsUlCtx->adsUlQueue[rabId].prio) {
            return VOS_OK;
        }
        /* 如果对应的调度优先级比之前的要高，需要更新该PDP的队列优先级，并对队列管理进行排序 */
        else {
            ADS_UL_UpdateQueueInPdpModified(instanceIndex, prio, rabId);
            return VOS_OK;
        }
    }

    /* ucRabID的队列不存在, 需要创建队列头结点 */
    ulQueue = (IMM_ZcHeader *)PS_MEM_ALLOC(ACPU_PID_ADS_UL, sizeof(IMM_ZcHeader));

    if (VOS_NULL_PTR == ulQueue) {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_CreateQueue: pstUlQueue is null");
        return VOS_ERR;
    }

    /* 队列初始化 */
    IMM_ZcQueueHeadInit(ulQueue);

    /* 将队列信息更新到上行上下文 */
    ADS_UL_SetQueue(instanceIndex, rabId, VOS_TRUE, ulQueue, prio, pktType, uc1XorHrpdUlIpfFlag);

    /*
     * 队列不可能被用尽，一个RABID对应一个队列，而无效的已经在消息处理处屏蔽，
     * 故不需要判断是否满，可以直接重新排序
     */
    ADS_UL_OrderQueueIndex(instanceIndex, rabId);

    return VOS_OK;
}


VOS_VOID ADS_UL_ClearQueue(IMM_ZcHeader *queue)
{
    IMM_Zc    *node = VOS_NULL_PTR;
    VOS_UINT32 i;
    VOS_UINT32 queueCnt;

    queueCnt = IMM_ZcQueueLen(queue);

    for (i = 0; i < queueCnt; i++) {
        node = IMM_ZcDequeueHead(queue);

        /* 释放结点内容 */
        if (VOS_NULL_PTR != node) {
            IMM_ZcFreeAny(node);
        }
    }
}


VOS_VOID ADS_UL_DestroyQueue(VOS_UINT32 instanceIndex, VOS_UINT32 rabId)
{
    VOS_ULONG lockLevel;

    /* 结点存在但队列为空 */
    if (VOS_ERR == ADS_UL_IsQueueExistent(instanceIndex, rabId)) {
        /* Rab Id以及优先级置为无效值 */
        ADS_UL_SetQueue(instanceIndex, rabId, VOS_FALSE, VOS_NULL_PTR, ADS_QCI_TYPE_BUTT, ADS_PDP_TYPE_BUTT, VOS_FALSE);

        /* 根据最新的队列管理进行排序 */
        ADS_UL_UpdateQueueInPdpDeactived(instanceIndex, rabId);

        return;
    }

    /* 队列加锁 */
    /*lint -e571*/
    VOS_SpinLockIntLock(ADS_UL_GET_QUEUE_LINK_SPINLOCK(instanceIndex, rabId), lockLevel);
    /*lint +e571*/

    /* 销毁队列中的数据 */
    ADS_UL_ClearQueue(ADS_UL_GET_QUEUE_LINK_PTR(instanceIndex, rabId));

    /* 销毁队列头结点 */
    PS_MEM_FREE(ACPU_PID_ADS_DL, ADS_UL_GET_QUEUE_LINK_PTR(instanceIndex, rabId));

    /* 将队列信息更新到上行上下文 */
    ADS_UL_SetQueue(instanceIndex, rabId, VOS_FALSE, VOS_NULL_PTR, ADS_QCI_TYPE_BUTT, ADS_PDP_TYPE_BUTT, VOS_FALSE);

    /* 队列操作完成解锁 */
    VOS_SpinUnlockIntUnlock(ADS_UL_GET_QUEUE_LINK_SPINLOCK(instanceIndex, rabId), lockLevel);

    /* 根据最新的队列管理进行排序 */
    ADS_UL_UpdateQueueInPdpDeactived(instanceIndex, rabId);
}


VOS_UINT32 ADS_UL_GetInsertIndex(VOS_UINT32 instanceIndex, VOS_UINT32 rabId)
{
    VOS_UINT32  i;
    ADS_UL_Ctx *adsUlCtx;

    adsUlCtx = ADS_GetUlCtx(instanceIndex);

    /* 根据优先级来获取上行上下文中队列的Index值 */
    for (i = 0; i < ADS_RAB_NUM_MAX; i++) {
        if (adsUlCtx->prioIndex[i] == rabId) {
            break;
        }
    }

    return i;
}


VOS_VOID ADS_UL_OrderQueueIndex(VOS_UINT32 instanceIndex, VOS_UINT32 indexNum)
{
    VOS_UINT32  i;
    VOS_UINT32  j;
    ADS_UL_Ctx *adsUlCtx;

    adsUlCtx = ADS_GetUlCtx(instanceIndex);

    /* 如果该PDP的优先级比较高，需要插入到其他PDP的前面，比其优先级低的需要向后移一位 */
    for (i = 0; i < ADS_RAB_NUM_MAX; i++) {
        if (adsUlCtx->adsUlQueue[indexNum].prio < adsUlCtx->adsUlQueue[adsUlCtx->prioIndex[i]].prio) {
            for (j = ADS_RAB_NUM_MAX - 1; j > i; j--) {
                adsUlCtx->prioIndex[j] = adsUlCtx->prioIndex[j - 1];
            }
            adsUlCtx->prioIndex[i] = indexNum;

            break;
        }
    }
}


VOS_VOID ADS_UL_UpdateQueueInPdpModified(VOS_UINT32 instanceIndex, ADS_QciTypeUint8 prio, VOS_UINT32 rabId)
{
    VOS_UINT32  i;
    VOS_UINT32  indexNum;
    ADS_UL_Ctx *adsUlCtx;

    adsUlCtx = ADS_GetUlCtx(instanceIndex);

    /* 将修改的优先级更新到对于的队列管理中 */
    adsUlCtx->adsUlQueue[rabId].prio = prio;

    /* 获取修改的RABID在aucPrioIndex数组中位置 */
    indexNum = ADS_UL_GetInsertIndex(instanceIndex, rabId);

    /* 没有找到，则不做处理 */
    if (indexNum >= ADS_RAB_NUM_MAX) {
        return;
    }

    /* 先将修改对应位后面的向前移动一位 */
    for (i = indexNum; i < ADS_RAB_NUM_MAX - 1; i++) {
        adsUlCtx->prioIndex[i] = adsUlCtx->prioIndex[i + 1UL];
    }

    adsUlCtx->prioIndex[ADS_RAB_NUM_MAX - 1] = 0;

    /* 移动后相当于是重新插入到队列管理中 */
    ADS_UL_OrderQueueIndex(instanceIndex, rabId);
}


VOS_VOID ADS_UL_UpdateQueueInPdpDeactived(VOS_UINT32 instanceIndex, VOS_UINT32 rabId)
{
    VOS_UINT32  i;
    VOS_UINT32  indexNum;
    ADS_UL_Ctx *adsUlCtx;

    adsUlCtx = ADS_GetUlCtx(instanceIndex);

    /* 根据删除的PDP索引，将其后面的元素向前移动一位 */
    indexNum = ADS_UL_GetInsertIndex(instanceIndex, rabId);

    if (indexNum >= ADS_RAB_NUM_MAX) {
        return;
    }

    for (i = indexNum; i < ADS_RAB_NUM_MAX - 1; i++) {
        adsUlCtx->prioIndex[i] = adsUlCtx->prioIndex[i + 1UL];
    }

    adsUlCtx->prioIndex[ADS_RAB_NUM_MAX - 1] = 0;
}


VOS_VOID ADS_UL_SetQueue(VOS_UINT32 instanceIndex, VOS_UINT32 rabId, VOS_UINT8 isQueueValid, IMM_ZcHeader *ulQueue,
                         ADS_QciTypeUint8 prio, ADS_CDS_IpfPktTypeUint8 pktType, VOS_UINT8 uc1XorHrpdUlIpfFlag)
{
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].adsUlLink    = ulQueue;
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].isQueueValid = isQueueValid;
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].prio         = prio;
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].recordNum    = 0;
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].pktType      = pktType;
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].uc1XorHrpdUlIpfFlag = uc1XorHrpdUlIpfFlag;
#else
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.adsUlQueue[rabId].uc1XorHrpdUlIpfFlag = VOS_FALSE;
#endif
}


VOS_VOID ADS_UL_SndEvent(VOS_UINT32 event)
{
    if (1 == g_adsULTaskReadyFlag) {
        (VOS_VOID)VOS_EventWrite(g_adsULTaskId, event);
    }

    return;
}


VOS_VOID ADS_UL_ProcEvent(VOS_UINT32 event)
{
    if (event & ADS_UL_EVENT_DATA_PROC) {
        ADS_UL_WakeLock();
        ADS_UL_ProcLinkData();
        ADS_UL_WakeUnLock();
        ADS_DBG_UL_PROC_EVENT_NUM(1);
    }

    return;
}


VOS_VOID ADS_DL_SndEvent(VOS_UINT32 event)
{
    if (1 == g_adsDlTaskReadyFlag) {
        (VOS_VOID)VOS_EventWrite(g_adsDlTaskId, event);
    }

    return;
}


VOS_VOID ADS_DL_ProcEvent(VOS_UINT32 event)
{
    VOS_ULONG lockLevel;

    if (event & ADS_DL_EVENT_IPF_RD_INT) {
        ADS_DL_WakeLock();
        ADS_DL_ProcIpfResult();
        ADS_DL_WakeUnLock();
        ADS_DBG_DL_PROC_IPF_RD_EVENT_NUM(1);
    }

    if (event & ADS_DL_EVENT_IPF_ADQ_EMPTY_INT) {
        /*lint -e571*/
        VOS_SpinLockIntLock(&(g_adsCtx.adsIpfCtx.adqSpinLock), lockLevel);
        /*lint +e571*/
        ADS_DL_AllocMemForAdq();
        VOS_SpinUnlockIntUnlock(&(g_adsCtx.adsIpfCtx.adqSpinLock), lockLevel);
        ADS_DBG_DL_PROC_IPF_AD_EVENT_NUM(1);
    }

    if (event & ADS_DL_EVENT_IPF_FILTER_DATA_PROC) {
        ADS_DL_ProcIpfFilterQue();
    }

    return;
}


VOS_VOID ADS_DL_InitFcAssemParamInfo(VOS_VOID)
{
    ADS_DL_FcAssem *fcAssemInfo;

    fcAssemInfo = ADS_DL_GET_FC_ASSEM_INFO_PTR(ADS_INSTANCE_INDEX_0);

    fcAssemInfo->enableMask   = VOS_FALSE;
    fcAssemInfo->fcActiveFlg  = VOS_FALSE;
    fcAssemInfo->tmrCnt       = ADS_CURRENT_TICK;
    fcAssemInfo->rdCnt        = 0;
    fcAssemInfo->rateUpLev    = 0;
    fcAssemInfo->rateDownLev  = 0;
    fcAssemInfo->expireTmrLen = 0;

    return;
}


VOS_VOID ADS_DL_ResetFcAssemParamInfo(VOS_VOID)
{
    ADS_DL_FcAssem *fcAssemInfo;

    fcAssemInfo = ADS_DL_GET_FC_ASSEM_INFO_PTR(ADS_INSTANCE_INDEX_0);

    fcAssemInfo->fcActiveFlg = VOS_FALSE;
    fcAssemInfo->rdCnt       = 0;

    return;
}


VOS_UINT32 ADS_UL_EnableRxWakeLockTimeout(VOS_UINT32 value)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    if (value > ipfCntxt->rxWakeLockTimeout) {
        ipfCntxt->rxWakeLockTimeout = value;
    }

    return 0;
}


VOS_UINT32 ADS_UL_WakeLockTimeout(VOS_VOID)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;
    VOS_UINT32   ret;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    ret      = ipfCntxt->rxWakeLockTimeout;

    if (0 != ipfCntxt->rxWakeLockTimeout) {
        __pm_wakeup_event(&(ipfCntxt->rxWakeLock), ipfCntxt->rxWakeLockTimeout);
    }

    ipfCntxt->rxWakeLockTimeout = 0;

    return ret;
}


VOS_UINT32 ADS_UL_WakeLock(VOS_VOID)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;
    VOS_UINT32   ret;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    if (VOS_FALSE == ipfCntxt->wakeLockEnable) {
        return 0;
    }

    __pm_stay_awake(&(ipfCntxt->ulBdWakeLock));
    ipfCntxt->ulBdWakeLockCnt++;

    ret = ipfCntxt->ulBdWakeLockCnt;
    return ret;
}


VOS_UINT32 ADS_UL_WakeUnLock(VOS_VOID)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;
    VOS_UINT32   ret;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    if (VOS_FALSE == ipfCntxt->wakeLockEnable) {
        return 0;
    }

    ADS_UL_WakeLockTimeout();

    __pm_relax(&(ipfCntxt->ulBdWakeLock));
    ipfCntxt->ulBdWakeLockCnt--;

    ret = ipfCntxt->ulBdWakeLockCnt;
    return ret;
}


VOS_UINT32 ADS_DL_EnableTxWakeLockTimeout(VOS_UINT32 value)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    if (value > ipfCntxt->txWakeLockTimeout) {
        ipfCntxt->txWakeLockTimeout = value;
    }

    return 0;
}


VOS_UINT32 ADS_DL_WakeLockTimeout(VOS_VOID)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;
    VOS_UINT32   ret;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    ret      = ipfCntxt->txWakeLockTimeout;

    if (0 != ipfCntxt->txWakeLockTimeout) {
        __pm_wakeup_event(&(ipfCntxt->txWakeLock), ipfCntxt->txWakeLockTimeout);
    }

    ipfCntxt->txWakeLockTimeout = 0;

    return ret;
}


VOS_UINT32 ADS_DL_WakeLock(VOS_VOID)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;
    VOS_UINT32   ret;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    if (VOS_FALSE == ipfCntxt->wakeLockEnable) {
        return 0;
    }

    __pm_stay_awake(&(ipfCntxt->dlRdWakeLock));
    ipfCntxt->dlRdWakeLockCnt++;

    ret = ipfCntxt->dlRdWakeLockCnt;
    return ret;
}


VOS_UINT32 ADS_DL_WakeUnLock(VOS_VOID)
{
    ADS_IPF_Ctx *ipfCntxt = VOS_NULL_PTR;
    VOS_UINT32   ret;

    ipfCntxt = ADS_GET_IPF_CTX_PTR();
    if (VOS_FALSE == ipfCntxt->wakeLockEnable) {
        return 0;
    }

    ADS_DL_WakeLockTimeout();

    __pm_relax(&(ipfCntxt->dlRdWakeLock));
    ipfCntxt->dlRdWakeLockCnt--;

    ret = ipfCntxt->dlRdWakeLockCnt;
    return ret;
}


VOS_VOID ADS_IPF_MemMapRequset(IMM_Zc *immZc, VOS_UINT32 len, VOS_UINT8 isIn)
{
    VOS_VOID  *data = VOS_NULL_PTR;
    dma_addr_t dmaAddr;

    data    = (VOS_VOID *)IMM_ZcGetDataPtr(immZc);
    dmaAddr = dma_map_single(ADS_GET_IPF_DEV(), data, (size_t)len, (isIn) ? DMA_TO_DEVICE : DMA_FROM_DEVICE);
    ADS_IPF_SetMemDma(immZc, dmaAddr);
    return;
}


VOS_VOID ADS_IPF_MemMapByDmaRequset(IMM_Zc *immZc, VOS_UINT32 len, VOS_UINT8 isIn)
{
    VOS_VOID  *data = VOS_NULL_PTR;
    dma_addr_t dmaAddr;

    dmaAddr = ADS_IPF_GetMemDma(immZc);
    data    = phys_to_virt(dmaAddr);
    dma_map_single(ADS_GET_IPF_DEV(), data, (size_t)len, (isIn) ? DMA_TO_DEVICE : DMA_FROM_DEVICE);
    return;
}


VOS_VOID ADS_IPF_MemUnmapRequset(IMM_Zc *immZc, VOS_UINT32 len, VOS_UINT8 isIn)
{
    dma_addr_t dmaAddr;

    dmaAddr = ADS_IPF_GetMemDma(immZc);
    dma_unmap_single(ADS_GET_IPF_DEV(), dmaAddr, (size_t)len, (isIn) ? DMA_TO_DEVICE : DMA_FROM_DEVICE);
    return;
}


VOS_UINT32 ADS_IPF_IsSpeMem(IMM_Zc *immZc)
{
    return VOS_FALSE;
}


VOS_VOID ADS_IPF_SetMemDma(IMM_Zc *immZc, dma_addr_t dmaAddr)
{
    ADS_IMM_MEM_CB(immZc)->dmaAddr = dmaAddr;
    return;
}


dma_addr_t ADS_IPF_GetMemDma(IMM_Zc *immZc)
{
    return ADS_IMM_MEM_CB(immZc)->dmaAddr;
}


IMM_Zc* ADS_IPF_AllocMem(VOS_UINT32 poolId, VOS_UINT32 len, VOS_UINT32 reserveLen)
{
    IMM_Zc *immZc = VOS_NULL_PTR;

    immZc = (IMM_Zc *)IMM_ZcStaticAlloc(len);
    if (VOS_NULL_PTR == immZc) {
        ADS_DBG_DL_ADQ_ALLOC_SYS_MEM_FAIL_NUM(1);
        return VOS_NULL_PTR;
    }

    ADS_DBG_DL_ADQ_ALLOC_SYS_MEM_SUCC_NUM(1);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
    /* 操作顺序: 刷CACHE, 预留头部空间 */
    ADS_IPF_DL_MEM_MAP(immZc, len);
    IMM_ZcReserve(immZc, reserveLen);
#else
    /* 操作顺序: 预留头部空间，刷CACHE  */
    IMM_ZcReserve(immZc, reserveLen);
    ADS_IPF_DL_MEM_MAP(immZc, len);
#endif

    return immZc;
}


VOS_SEM ADS_GetULResetSem(VOS_VOID)
{
    return g_adsCtx.hULResetSem;
}


VOS_SEM ADS_GetDLResetSem(VOS_VOID)
{
    return g_adsCtx.hDLResetSem;
}


VOS_UINT8 ADS_GetUlResetFlag(VOS_VOID)
{
    return g_adsCtx.ulResetFlag;
}


VOS_VOID ADS_SetUlResetFlag(VOS_UINT8 flag)
{
    g_adsCtx.ulResetFlag = flag;

    return;
}



ADS_UL_Ctx* ADS_GetUlCtx(VOS_UINT32 instanceIndex)
{
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instanceIndex]);

    return &(adsSpecCtx->adsUlCtx);
}


ADS_DL_Ctx* ADS_GetDlCtx(VOS_UINT32 instanceIndex)
{
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instanceIndex]);

    return &(adsSpecCtx->adsDlCtx);
}


ADS_TIMER_Ctx* ADS_GetTiCtx(VOS_VOID)
{
    return g_adsCtx.adsTiCtx;
}


ADS_Ctx* ADS_GetAllCtx(VOS_VOID)
{
    return &g_adsCtx;
}


VOS_VOID ADS_InitUlCtx(VOS_UINT32 instanceIndex)
{
    VOS_UINT32   i;
    VOS_UINT32   rst;
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instanceIndex]);

    /* 默认从第一个队列开始调度 */
    adsSpecCtx->adsUlCtx.adsUlCurIndex = 0;

    for (i = 0; i < ADS_RAB_ID_MAX + 1; i++) {
        adsSpecCtx->adsUlCtx.adsUlQueue[i].adsUlLink    = VOS_NULL_PTR;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].isQueueValid = VOS_FALSE;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].prio         = ADS_QCI_TYPE_BUTT;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].recordNum    = 0;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].pktType      = ADS_CDS_IPF_PKT_TYPE_IP;

        /* 锁初始化 */
        VOS_SpinLockInit(&(adsSpecCtx->adsUlCtx.adsUlQueue[i].spinLock));
    }

    memset_s(adsSpecCtx->adsUlCtx.prioIndex, sizeof(adsSpecCtx->adsUlCtx.prioIndex), 0x00,
             sizeof(adsSpecCtx->adsUlCtx.prioIndex));

    /* 读NV，将优先级加权数读写到ADS上下文中 */

    rst = TAF_ACORE_NV_READ(instanceIndex, NV_ITEM_ADS_QUEUE_SCHEDULER_PRI, &(adsSpecCtx->adsUlCtx.queuePriNv),
                            sizeof(ADS_UL_QueueSchedulerPriNv));
    if (NV_OK != rst) {
        adsSpecCtx->adsUlCtx.queuePriNv.status = VOS_FALSE;

        for (i = 0; i < ADS_UL_QUEUE_SCHEDULER_PRI_MAX; i++) {
            adsSpecCtx->adsUlCtx.queuePriNv.priWeightedNum[i] = ADS_UL_DEFAULT_PRI_WEIGHTED_NUM;
        }

        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_InitUlCtx: NV read failed !");
    }

    if (VOS_FALSE == adsSpecCtx->adsUlCtx.queuePriNv.status) {
        for (i = 0; i < ADS_UL_QUEUE_SCHEDULER_PRI_MAX; i++) {
            adsSpecCtx->adsUlCtx.queuePriNv.priWeightedNum[i] = ADS_UL_DEFAULT_PRI_WEIGHTED_NUM;
        }
    }

    adsSpecCtx->adsUlCtx.ulMaxQueueLength = ADS_UL_MAX_QUEUE_LENGTH;

    return;
}


VOS_VOID ADS_InitDlCtx(VOS_UINT32 instance)
{
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;
    VOS_UINT32   i;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instance]);

    /* 初始化下行的RAB信息 */
    for (i = 0; i < ADS_RAB_NUM_MAX; i++) {
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rabId               = ADS_RAB_ID_INVALID;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].pktType             = ADS_CDS_IPF_PKT_TYPE_IP;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].exParam             = 0;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rcvDlDataFunc       = VOS_NULL_PTR;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rcvDlFilterDataFunc = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rcvRdLstDataFunc = VOS_NULL_PTR;
#endif
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].lstPkt = VOS_NULL_PTR;
    }

    return;
}


VOS_VOID ADS_InitStatsInfoCtx(VOS_VOID)
{
    ADS_StatsInfoCtx *dsFlowStatsCtx = VOS_NULL_PTR;

    dsFlowStatsCtx = ADS_GET_DSFLOW_STATS_CTX_PTR();

    dsFlowStatsCtx->ulDataStats.ulPeriodSndBytes = 0;
    dsFlowStatsCtx->ulDataStats.ulCurDataRate    = 0;
    dsFlowStatsCtx->dlDataStats.dlPeriodRcvBytes = 0;
    dsFlowStatsCtx->dlDataStats.dlCurDataRate    = 0;
}


VOS_VOID ADS_InitSpecCtx(VOS_VOID)
{
    VOS_UINT32 i;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++) {
        /* 初始化上行上下文 */
        ADS_InitUlCtx(i);

        /* 初始化下行上下文 */
        ADS_InitDlCtx(i);
    }

    /* 初始化底软动态组包参数 */
    ADS_DL_InitFcAssemParamInfo();
}


VOS_VOID ADS_ResetSpecUlCtx(VOS_UINT32 instance)
{
    VOS_UINT32   i;
    VOS_UINT32   rst;
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instance]);

    /* 默认从第一个队列开始调度 */
    adsSpecCtx->adsUlCtx.adsUlCurIndex = 0;

    for (i = 0; i < ADS_RAB_ID_MAX + 1; i++) {
        adsSpecCtx->adsUlCtx.adsUlQueue[i].adsUlLink    = VOS_NULL_PTR;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].isQueueValid = VOS_FALSE;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].prio         = ADS_QCI_TYPE_BUTT;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].recordNum    = 0;
        adsSpecCtx->adsUlCtx.adsUlQueue[i].pktType      = ADS_CDS_IPF_PKT_TYPE_IP;
    }

    memset_s(adsSpecCtx->adsUlCtx.prioIndex, sizeof(adsSpecCtx->adsUlCtx.prioIndex), 0x00,
             sizeof(adsSpecCtx->adsUlCtx.prioIndex));

    /* 读NV，将优先级加权数读写到ADS上下文中 */
    rst = TAF_ACORE_NV_READ(instance, NV_ITEM_ADS_QUEUE_SCHEDULER_PRI, &(adsSpecCtx->adsUlCtx.queuePriNv),
                            sizeof(ADS_UL_QueueSchedulerPriNv));
    if (NV_OK != rst) {
        adsSpecCtx->adsUlCtx.queuePriNv.status = VOS_FALSE;

        for (i = 0; i < ADS_UL_QUEUE_SCHEDULER_PRI_MAX; i++) {
            adsSpecCtx->adsUlCtx.queuePriNv.priWeightedNum[i] = ADS_UL_DEFAULT_PRI_WEIGHTED_NUM;
        }

        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_InitUlCtx: NV read failed !");
    }

    if (VOS_FALSE == adsSpecCtx->adsUlCtx.queuePriNv.status) {
        for (i = 0; i < ADS_UL_QUEUE_SCHEDULER_PRI_MAX; i++) {
            adsSpecCtx->adsUlCtx.queuePriNv.priWeightedNum[i] = ADS_UL_DEFAULT_PRI_WEIGHTED_NUM;
        }
    }

    adsSpecCtx->adsUlCtx.ulMaxQueueLength = ADS_UL_MAX_QUEUE_LENGTH;

    return;
}


VOS_VOID ADS_ResetSpecDlCtx(VOS_UINT32 instance)
{
    ADS_SpecCtx *adsSpecCtx = VOS_NULL_PTR;
    VOS_UINT32   i;

    adsSpecCtx = &(g_adsCtx.adsSpecCtx[instance]);

    /* 重置下行的RAB信息 */
    for (i = 0; i < ADS_RAB_NUM_MAX; i++) {
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rabId               = ADS_RAB_ID_INVALID;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].pktType             = ADS_CDS_IPF_PKT_TYPE_IP;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].exParam             = 0;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rcvDlDataFunc       = VOS_NULL_PTR;
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rcvDlFilterDataFunc = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].rcvRdLstDataFunc = VOS_NULL_PTR;
#endif
        adsSpecCtx->adsDlCtx.adsDlRabInfo[i].lstPkt = VOS_NULL_PTR;
    }

    return;
}


VOS_VOID ADS_ResetUlCtx(VOS_VOID)
{
    VOS_UINT32 i;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++) {
        /* 初始化上行上下文 */
        ADS_ResetSpecUlCtx(i);
    }

    /* 初始化数据统计的上下文 */
    ADS_InitStatsInfoCtx();

    return;
}


VOS_VOID ADS_ResetDlCtx(VOS_VOID)
{
    VOS_UINT32 i;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++) {
        /* 初始化下行上下文 */
        ADS_ResetSpecDlCtx(i);
    }

    /* 重置底软动态组包参数 */
    ADS_DL_ResetFcAssemParamInfo();

    return;
}


VOS_VOID ADS_ResetIpfCtx(VOS_VOID)
{
    /* 默认上行数据发送保护定时器时长为10ms */
    g_adsCtx.adsIpfCtx.protectTmrLen = TI_ADS_UL_SEND_LEN;

    /* 默认上行数据统计定时器时长为100ms */
    g_adsCtx.adsIpfCtx.ulAssemParmInfo.thresholdStatInfo.statTmrLen = 100;
    g_adsCtx.adsIpfCtx.ulAssemParmInfo.thresholdStatInfo.statPktNum = 0;

    /* 默认下行C核单独复位延时定时器时长为1s */
    g_adsCtx.adsIpfCtx.cCoreResetDelayTmrLen = 1000;

    /* 默认攒包的最大个数 */
    if (VOS_TRUE == g_adsCtx.adsIpfCtx.ulAssemParmInfo.activeFlag) {
        g_adsCtx.adsIpfCtx.thredHoldNum = ADS_UL_DATA_THRESHOLD_ONE;
    } else {
        g_adsCtx.adsIpfCtx.thredHoldNum = 32;
    }

    /* 默认数据不在发送 */
    g_adsCtx.adsIpfCtx.sendingFlg = VOS_FALSE;
}


VOS_VOID ADS_InitIpfCtx(VOS_VOID)
{
    ADS_UL_DynamicAssemInfo *ulAssemParmInfo = VOS_NULL_PTR;
    ADS_NV_DynamicThreshold  threshold;
    TAF_NV_AdsWakeLockCfg    wakeLockCfg;
    TAF_NV_AdsIpfModeCfg     ipfMode;
    VOS_UINT32               ret;
    VOS_UINT32               i;

    for (i = 0; i < ADS_DL_ADQ_MAX_NUM; i++) {
        memset_s(g_adsCtx.adsIpfCtx.ipfDlAdDesc[i], sizeof(g_adsCtx.adsIpfCtx.ipfDlAdDesc[i]), 0x00,
                 sizeof(g_adsCtx.adsIpfCtx.ipfDlAdDesc[i]));

        memset_s(&(g_adsCtx.adsIpfCtx.ipfDlAdRecord[i]), sizeof(g_adsCtx.adsIpfCtx.ipfDlAdRecord[i]), 0x00,
                 sizeof(g_adsCtx.adsIpfCtx.ipfDlAdRecord[i]));
    }

    memset_s(&(g_adsCtx.adsIpfCtx.ipfDlRdRecord), sizeof(g_adsCtx.adsIpfCtx.ipfDlRdRecord), 0x00,
             sizeof(g_adsCtx.adsIpfCtx.ipfDlRdRecord));

    /* 初始化上行源内存释放队列 */
    IMM_ZcQueueHeadInit(&g_adsCtx.adsIpfCtx.ulSrcMemFreeQue);

    /* 初始化上行BD BUFF */
    memset_s(g_adsCtx.adsIpfCtx.ipfUlBdCfgParam, sizeof(g_adsCtx.adsIpfCtx.ipfUlBdCfgParam), 0x00,
             sizeof(g_adsCtx.adsIpfCtx.ipfUlBdCfgParam));

    /* 初始化下行RD BUFF */
    memset_s(g_adsCtx.adsIpfCtx.ipfDlRdDesc, sizeof(g_adsCtx.adsIpfCtx.ipfDlRdDesc), 0x00,
             sizeof(g_adsCtx.adsIpfCtx.ipfDlRdDesc));

    memset_s(&wakeLockCfg, sizeof(wakeLockCfg), 0x00, sizeof(wakeLockCfg));

    /* 默认上行数据发送保护定时器时长为10ms */
    g_adsCtx.adsIpfCtx.protectTmrLen = TI_ADS_UL_SEND_LEN;

    /* 默认下行C核单独复位延时定时器时长为1s */
    g_adsCtx.adsIpfCtx.cCoreResetDelayTmrLen = 1000;

    ulAssemParmInfo = &g_adsCtx.adsIpfCtx.ulAssemParmInfo;

    memset_s(&threshold, sizeof(threshold), 0x00, sizeof(threshold));

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_ADS_DYNAMIC_THRESHOLD_CFG, &threshold, sizeof(ADS_NV_DynamicThreshold));
    if (NV_OK != ret) {
        ulAssemParmInfo->activeFlag                 = VOS_FALSE;
        ulAssemParmInfo->protectTmrExpCnt           = 0;
        ulAssemParmInfo->waterMarkLevel.waterLevel1 = 80;
        ulAssemParmInfo->waterMarkLevel.waterLevel2 = 150;
        ulAssemParmInfo->waterMarkLevel.waterLevel3 = 500;
        ulAssemParmInfo->waterMarkLevel.waterLevel4 = 0xFFFFFFFF;

        ulAssemParmInfo->thresholdLevel.threshold1 = 1;
        ulAssemParmInfo->thresholdLevel.threshold2 = 13;
        ulAssemParmInfo->thresholdLevel.threshold3 = 60;
        ulAssemParmInfo->thresholdLevel.threshold4 = 64;
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_InitIpfCtx: NV read failed !");
    }

    ulAssemParmInfo->activeFlag                 = threshold.activeFlag;
    ulAssemParmInfo->protectTmrExpCnt           = threshold.protectTmrExpCnt;
    ulAssemParmInfo->waterMarkLevel.waterLevel1 = threshold.waterMarkLevel.waterLevel1;
    ulAssemParmInfo->waterMarkLevel.waterLevel2 = threshold.waterMarkLevel.waterLevel2;
    ulAssemParmInfo->waterMarkLevel.waterLevel3 = threshold.waterMarkLevel.waterLevel3;
    ulAssemParmInfo->waterMarkLevel.waterLevel4 = threshold.waterMarkLevel.waterLevel4;

    ulAssemParmInfo->thresholdLevel.threshold1 = threshold.thresholdLevel.threshold1;
    ulAssemParmInfo->thresholdLevel.threshold2 = threshold.thresholdLevel.threshold2;
    ulAssemParmInfo->thresholdLevel.threshold3 = threshold.thresholdLevel.threshold3;
    ulAssemParmInfo->thresholdLevel.threshold4 = threshold.thresholdLevel.threshold4;

    /* 默认上行数据统计定时器时长为100ms */
    ulAssemParmInfo->thresholdStatInfo.statTmrLen = 100;
    ulAssemParmInfo->thresholdStatInfo.statPktNum = 0;

    /* 超时时长大于零才需要启动jiffies保护定时器 */
    if (0 != ulAssemParmInfo->protectTmrExpCnt) {
        ulAssemParmInfo->protectTmrCnt = ADS_CURRENT_TICK;
    }

    /* 默认攒包的最大个数 */
    if (VOS_TRUE == ulAssemParmInfo->activeFlag) {
        g_adsCtx.adsIpfCtx.thredHoldNum = ADS_UL_DATA_THRESHOLD_ONE;
    } else {
        g_adsCtx.adsIpfCtx.thredHoldNum = 32;
    }

    /* 默认数据不在发送 */
    g_adsCtx.adsIpfCtx.sendingFlg = VOS_FALSE;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
    memset_s(&g_adsDmaDev, sizeof(g_adsDmaDev), 0, sizeof(g_adsDmaDev));
    g_adsDmaDev.dma_mask = &g_adsDmaMask;
    g_dmaDev             = &g_adsDmaDev;
#endif

    wakeup_source_init(&g_adsCtx.adsIpfCtx.ulBdWakeLock, "ipf_bd_wake");
    wakeup_source_init(&g_adsCtx.adsIpfCtx.dlRdWakeLock, "ipf_rd_wake");

    wakeup_source_init(&g_adsCtx.adsIpfCtx.rxWakeLock, "ads_rx_wake");
    wakeup_source_init(&g_adsCtx.adsIpfCtx.txWakeLock, "ads_tx_wake");

    g_adsCtx.adsIpfCtx.wakeLockEnable = VOS_FALSE;

    g_adsCtx.adsIpfCtx.ulBdWakeLockCnt = 0;
    g_adsCtx.adsIpfCtx.dlRdWakeLockCnt = 0;

    g_adsCtx.adsIpfCtx.rxWakeLockTimeout = 0;
    g_adsCtx.adsIpfCtx.txWakeLockTimeout = 0;

    g_adsCtx.adsIpfCtx.txWakeLockTmrLen = 500;
    g_adsCtx.adsIpfCtx.rxWakeLockTmrLen = 500;

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_ADS_WAKE_LOCK_CFG, &wakeLockCfg, sizeof(TAF_NV_AdsWakeLockCfg));
    if (NV_OK == ret) {
        g_adsCtx.adsIpfCtx.wakeLockEnable   = wakeLockCfg.enable;
        g_adsCtx.adsIpfCtx.txWakeLockTmrLen = wakeLockCfg.txWakeTimeout;
        g_adsCtx.adsIpfCtx.rxWakeLockTmrLen = wakeLockCfg.rxWakeTimeout;
    }

    memset_s(&ipfMode, sizeof(ipfMode), 0x00, sizeof(ipfMode));

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_ADS_IPF_MODE_CFG, &ipfMode, (VOS_UINT32)sizeof(TAF_NV_AdsIpfModeCfg));
    if (NV_OK == ret) {
        g_adsCtx.adsIpfCtx.ipfMode = ipfMode.ipfMode;
    }

    IMM_ZcQueueHeadInit(ADS_GET_IPF_FILTER_QUE());

    VOS_SpinLockInit(&(g_adsCtx.adsIpfCtx.adqSpinLock));

    return;
}


VOS_VOID ADS_InitTiCtx(VOS_VOID)
{
    VOS_UINT32 i;

    for (i = 0; i < ADS_MAX_TIMER_NUM; i++) {
        g_adsCtx.adsTiCtx[i].hTimer = VOS_NULL_PTR;
    }

    init_timer(&g_adsCtx.ulSendProtectTimer);
    g_adsCtx.ulSendProtectTimer.function = ADS_UlSendProtectTimerExpire;
    g_adsCtx.ulSendProtectTimer.data     = VOS_NULL_PTR;

    return;
}


VOS_VOID ADS_InitResetSem(VOS_VOID)
{
    g_adsCtx.hULResetSem = VOS_NULL_PTR;
    g_adsCtx.hDLResetSem = VOS_NULL_PTR;

    /* 分配二进制信号量 */
    if (VOS_OK != VOS_SmBCreate("UL", 0, VOS_SEMA4_FIFO, &g_adsCtx.hULResetSem)) {
        ADS_TRACE_HIGH("Create ADS acpu UL_CNF sem failed!\n");
        ADS_DBG_UL_RESET_CREATE_SEM_FAIL_NUM(1);
        return;
    }

    if (VOS_OK != VOS_SmBCreate("DL", 0, VOS_SEMA4_FIFO, &g_adsCtx.hDLResetSem)) {
        ADS_TRACE_HIGH("Create ADS acpu DL_CNF sem failed!\n");
        ADS_DBG_DL_RESET_CREATE_SEM_FAIL_NUM(1);
        return;
    }

    return;
}


VOS_VOID ADS_ReadPacketErrorFeedbackCongfigNV(VOS_VOID)
{
    ADS_PacketErrorFeedbackCfg *config = VOS_NULL_PTR;
    TAF_NV_AdsErrorFeedbackCfg  nvConfig;
    VOS_UINT32                  ret;

    memset_s(&nvConfig, sizeof(nvConfig), 0x00, sizeof(nvConfig));

    /* 默认配置 */
    config          = ADS_DL_GET_PKT_ERR_FEEDBACK_CFG_PTR();
    config->enabled = 0;

    config->pktErrRateThres   = ADS_PKT_ERR_RATE_DEFAULT_THRESHOLD;
    config->minDetectDuration = msecs_to_jiffies(ADS_PKT_ERR_DETECT_DEFAULT_DURATION);
    config->maxDetectDuration =
        msecs_to_jiffies(ADS_PKT_ERR_DETECT_DEFAULT_DURATION + ADS_PKT_ERR_DETECT_DEFAULT_DELTA);

    /* 读取NV配置 */
    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_ADS_PACKET_ERROR_FEEDBACK_CFG, &nvConfig,
                            sizeof(TAF_NV_AdsErrorFeedbackCfg));
    if (NV_OK == ret) {
        config->enabled = nvConfig.enabled;

        if (ADS_IS_PKT_ERR_RATE_THRESHOLD_VALID(nvConfig.errorRateThreshold)) {
            config->pktErrRateThres = nvConfig.errorRateThreshold;
        }

        if (ADS_IS_PKT_ERR_DETECT_DURATION_VALID(nvConfig.detectDuration)) {
            config->minDetectDuration = msecs_to_jiffies(nvConfig.detectDuration);
            config->maxDetectDuration = msecs_to_jiffies(nvConfig.detectDuration + ADS_PKT_ERR_DETECT_DEFAULT_DELTA);
        }
    }

    return;
}


VOS_VOID ADS_InitCtx(VOS_VOID)
{
    memset_s(&g_adsStats, sizeof(g_adsStats), 0x00, sizeof(g_adsStats));

    /* 初始化每个实例的上下文 */
    ADS_InitSpecCtx();

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
    ADS_InitPc5Ctx();
#endif

    /* 初始化数据统计的上下文 */
    ADS_InitStatsInfoCtx();

    /* 初始化IPF相关的上下文 */
    ADS_InitIpfCtx();

    /* 初始化定时器上下文 */
    ADS_InitTiCtx();

    /* 初始化复位信号量 */
    ADS_InitResetSem();

    /* 初始化ADS过滤器上下文 */
    ADS_FilterInitCtx();

    /* 初始化当前实例索引值 */
    g_adsCtx.adsCurInstanceIndex = ADS_INSTANCE_INDEX_0;

    /* 读取错包反馈配置 */
    ADS_ReadPacketErrorFeedbackCongfigNV();

    mdrv_diag_conn_state_notify_fun_reg(ADS_MntnDiagConnStateNotifyCB);
    IPSMNTN_RegTraceCfgNotify(ADS_MntnTraceCfgNotifyCB);

    return;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))

VOS_INT32 ADS_PlatDevProbe(struct platform_device *dev)
{
    g_dmaDev = &(dev->dev);
    dma_set_mask_and_coherent(g_dmaDev, DMA_BIT_MASK(64));  //lint !e598 !e648

    return 0;
}


VOS_INT32 ADS_PlatDevRemove(struct platform_device *dev)
{
    return 0;
}

static const struct of_device_id g_adsPlatDevOfMatch[] = {
    {
        .compatible = "hisilicon,hisi-ads",
    },
    {},
};

static struct platform_driver g_adsPlatDevDriver = {
    .probe  = ADS_PlatDevProbe,
    .remove = ADS_PlatDevRemove,
    .driver = {
        .name = "hisi-ads",
        .of_match_table = of_match_ptr(g_adsPlatDevOfMatch),
    },
};


VOS_INT32 __init ADS_PlatDevInit(void)
{
    return platform_driver_register(&g_adsPlatDevDriver);
}


VOS_VOID __exit ADS_PlatDevExit(void)
{
    platform_driver_unregister(&g_adsPlatDevDriver);
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(ADS_PlatDevInit);
module_exit(ADS_PlatDevExit);
#endif
#endif

