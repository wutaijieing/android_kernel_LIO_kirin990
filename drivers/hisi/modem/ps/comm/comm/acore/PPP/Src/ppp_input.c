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

#include "ppp_input.h"
#include "ppp_public.h"
#include "hdlc_interface.h"
#include "product_config.h"
#include "ttf_comm.h"
#include "gucttf_tag.h"
#include "securec.h"
#include "pppc_pppa_interface.h"

#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_OFF)
#include "hdlc_software.h"
#include "ppp_convert.h"
#else
#include "hdlc_hardware.h"
#endif

#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
#include "ads_dev_i.h"
#else
#include "ads_device_interface.h"
#endif


#define THIS_FILE_ID PS_FILE_ID_PPP_INPUT_C
#define THIS_MODU mod_ppp

#define PPPA_DATA_QUEUE_MAX_CNT 1500

typedef struct {
    PPP_ZcQueue         dataQ;          /* PPP数据队列，上下行数据都在其中 */
    VOS_UINT32          dataQMaxCnt;    /* PPP数据队列最大深度 */
    PPP_DataQStat       stat;           /* PPP数据队列的统计信息 */
    HTIMER              delayProtTimer; /* 延时处理定时器去 */
    volatile VOS_UINT32 notifyMsg;      /* 通知PPP处理数据 */
    VOS_SPINLOCK        spinLock;       /* ulNotifyMsg在多个任务中会被修改，因此需要使用自旋锁 */
    VOS_UINT32          pppTaskId;      /* PPP任务ID，通知PPP任务有数处理时使用 */
    VOS_UINT32          rawDataByPass;  /* RAWDATA拨号时数据是否是透传模式 */
} PPP_DataQCtrl;

/* PPP的数据队列结构体,上下行数据都在同一个队列中 */
PPP_DataQCtrl g_pppDataqCtrl;

#define PPPA_GET_DATAQ_CTRL() &g_pppDataqCtrl
#define PPPA_GET_STAT_ADDR() &(g_pppDataqCtrl.stat)

#define PPPA_DATA_DELAY_PROC_TIMER_LEN 10

VOS_BOOL   g_pppUsed          = VOS_FALSE;
VOS_UINT32 g_pppDelayTimerLen = PPPA_DATA_DELAY_PROC_TIMER_LEN;

VOS_VOID PPP_ClearDataQ(VOS_VOID)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();
    PPP_Zc        *mem       = (PPP_Zc*)PPP_ZC_DEQUEUE_HEAD(&(dataQCtrl->dataQ));

    while (mem != VOS_NULL_PTR) {
        PPP_MemFree(mem);
        mem = (PPP_Zc*)PPP_ZC_DEQUEUE_HEAD(&(dataQCtrl->dataQ));
    }
}

VOS_UINT32 PPP_DataQInit(VOS_VOID)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();
    VOS_UINT32     rslt      = VOS_OK;

    (VOS_VOID)memset_s(dataQCtrl, sizeof(PPP_DataQCtrl), 0, sizeof(PPP_DataQCtrl));

    PPP_ZC_QUEUE_INIT(&(dataQCtrl->dataQ));
    dataQCtrl->rawDataByPass = VOS_FALSE;
    dataQCtrl->notifyMsg     = VOS_TRUE;
    dataQCtrl->dataQMaxCnt   = PPPA_DATA_QUEUE_MAX_CNT;

    PPPA_SetDataStatAddr(&(dataQCtrl->stat));

    VOS_SpinLockInit(&(dataQCtrl->spinLock));

#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_ON)
    rslt = PPP_HDLC_HardInit();
#endif
    return rslt;
}

VOS_VOID PPPA_SaveTaskId(VOS_UINT32 taskId)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();

    dataQCtrl->pppTaskId = taskId;
}

VOS_UINT32 PPP_GetRawDataByPassMode(VOS_VOID)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();

    return dataQCtrl->rawDataByPass;
}

VOS_VOID PPP_SetRawDataByPassMode(VOS_UINT32 rawDataByPassMode)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();

    dataQCtrl->rawDataByPass = rawDataByPassMode;
}

VOS_UINT32 PPP_InputGetDataCnt(VOS_VOID)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();

    return PPP_ZC_GET_QUEUE_LEN(&(dataQCtrl->dataQ));
}


VOS_VOID PPP_NotiFyProcData(PPP_DataQCtrl *dataQCtrl)
{
    VOS_ULONG flags = 0UL;

    VOS_SpinLockIntLock(&(dataQCtrl->spinLock), flags);

    if (dataQCtrl->notifyMsg != VOS_TRUE) {
        VOS_SpinUnlockIntUnlock(&(dataQCtrl->spinLock), flags);
        return;
    }
    dataQCtrl->notifyMsg = VOS_FALSE;
    VOS_SpinUnlockIntUnlock(&(dataQCtrl->spinLock), flags);
    dataQCtrl->stat.sndMsgCnt++;
    (VOS_VOID)VOS_EventWrite(dataQCtrl->pppTaskId, PPP_RCV_DATA_EVENT);
}

VOS_UINT32 PPP_EnqueueData(PPP_Zc *immZc, PPP_DataTypeUint8 dataType, PPP_Id pppId)
{
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();
    PPP_ZcQueue   *dataQ     = &(dataQCtrl->dataQ);

    if (PPP_ZC_GET_QUEUE_LEN(dataQ) > dataQCtrl->dataQMaxCnt) {
        PPP_MemFree(immZc);
        dataQCtrl->stat.dropCnt++;
        PPP_NotiFyProcData(dataQCtrl);
        return PS_FAIL;
    }

    /* 将数据结点插入队列尾部 */
    PPP_ZC_ENQUEUE_TAIL(dataQ, immZc);

    if (PPP_ZC_GET_QUEUE_LEN(dataQ) > dataQCtrl->stat.qMaxCnt) {
        dataQCtrl->stat.qMaxCnt = PPP_ZC_GET_QUEUE_LEN(dataQ);
    }

    PPP_NotiFyProcData(dataQCtrl);

    return PS_SUCC;
} /* PPP_EnqueueData */

VOS_UINT32 PPP_PullPacketEvent(VOS_UINT16 pppId, IMM_Zc *immZc)
{
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();
    if (immZc == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP_PullPacketEvent, WARNING, pstImmZc is NULL!\r\n");

        return PS_FAIL;
    }

    dataStat->uplinkCnt++;

    /* 填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型 */
    PPP_ZC_SET_DATA_APP(immZc, (VOS_UINT16)(pppId << 8) | (VOS_UINT16)PPP_PULL_PACKET_TYPE);

    if (PPP_EnqueueData(immZc, PPP_PULL_PACKET_TYPE, (PPP_Id)pppId) != PS_SUCC) {
        dataStat->uplinkDropCnt++;
    }

    return PS_SUCC;
} /* PPP_PullPacketEvent */

VOS_UINT32 PPP_PullRawDataEvent(VOS_UINT16 pppId, IMM_Zc *immZc)
{
    PPP_Zc        *mem      = VOS_NULL_PTR;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    dataStat->uplinkCnt++;

    if (immZc == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP_PullRawDataEvent, WARNING, Alloc TTF mem fail!\r\n");

        return PS_FAIL;
    }

    if (PPP_GetRawDataByPassMode() == VOS_TRUE) {
#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
        VOS_UINT16 dataLen;

        /* 递交给ADS的数据需要预留MAC头，因此透传模式下需要重新申请内存 */
        dataLen = PPP_ZC_GET_DATA_LEN(immZc);
        if ((dataLen == 0) || (dataLen > PPP_ZC_MAX_DATA_LEN)) {
            dataStat->uplinkDropCnt++;
            PPP_MNTN_LOG1(PS_PRINT_WARNING, "DataLen err", dataLen);
            PPP_MemFree(immZc);
            return PS_FAIL;
        }

        mem = PPP_MemCopyAlloc(PPP_ZC_GET_DATA_PTR(immZc), dataLen, PPP_ZC_UL_RESERVE_LEN);
        PPP_MemFree(immZc);
        if (mem == VOS_NULL_PTR) {
            PPP_MNTN_LOG(PS_PRINT_WARNING, "Alloc Mem Fail");
            dataStat->uplinkDropCnt++;
            return PS_FAIL;
        }
#else
        mem = immZc;
#endif
        /* PPP模式透传模式下递交给ADS的是PPP报文，因此协议类型填0 */
        if (PPP_SendUlDataToAds(pppId, mem, 0) != PS_SUCC) {
            return PS_FAIL;
        }
    } else {
        /* 填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型 */
        PPP_ZC_SET_DATA_APP(immZc, (VOS_UINT16)(pppId << 8) | (VOS_UINT16)PPP_PULL_RAW_DATA_TYPE);

        if (PPP_EnqueueData(immZc, PPP_PULL_RAW_DATA_TYPE, (PPP_Id)pppId) != PS_SUCC) {
            dataStat->uplinkDropCnt++;
        }
    }

    return PS_SUCC;
} /* PPP_PullRawEvent */

VOS_BOOL PPPA_GetUsedFalg(VOS_VOID)
{
    return g_pppUsed;
}
VOS_INT PPP_DlPacketProc(PPP_Id pppId, IMM_Zc *immZc)
{
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    if (immZc == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP_PushPacketEvent, WARNING, pstImmZc is NULL!\r\n");
        return PS_FAIL;
    }

    /* 如果该链接还没建立起来 */
    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        /* 该变量需要被初始化为0 */
        dataStat->downlinkDropCnt++;
        PPP_MNTN_LOG1(PS_PRINT_NORMAL, "ppp id err", dataStat->downlinkDropCnt);
        PPP_MemFree(immZc);
        return PS_FAIL;
    }

    /* 填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型 */
    PPP_ZC_SET_DATA_APP(immZc, (VOS_UINT16)(pppId << 8) | (VOS_UINT16)PPP_PUSH_PACKET_TYPE);

    if (PPP_EnqueueData(immZc, PPP_PUSH_PACKET_TYPE, pppId) != PS_SUCC) {
        dataStat->downlinkDropCnt++;
    }

    return PS_SUCC;
}

VOS_VOID PPPA_SendDlDataToAT(PPP_Id pppId, IMM_Zc *data)
{
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    AT_SendZcDataToModem(pppId, data);
    dataStat->downlinkSndDataCnt++;
}

VOS_INT PPP_DlRawDataProc(PPP_Id pppId, IMM_Zc *immZc)
{
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    if (immZc == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP_PushRawDataEvent, WARNING, pstImmZc is NULL!\r\n");

        return PS_FAIL;
    }

    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        dataStat->downlinkDropCnt++;
        PPP_MemFree(immZc);
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP_PushRawData, WARNING, Invalid PPP id, packet from GGSN droped\r\n");

        return PS_FAIL;
    }

    if (PPP_GetRawDataByPassMode() == VOS_TRUE) {
        PPPA_SendDlDataToAT(pppId, immZc);
    } else {
        /* 填充pstData的usApp字段:高8位放usPppId,低8位放PPP报文类型 */
        PPP_ZC_SET_DATA_APP(immZc, (VOS_UINT16)(pppId << 8) | (VOS_UINT16)PPP_PUSH_RAW_DATA_TYPE);

        if (PPP_EnqueueData(immZc, PPP_PUSH_RAW_DATA_TYPE, pppId) != PS_SUCC) {
            dataStat->downlinkDropCnt++;
            return PS_FAIL;
        }
    }

    return PS_SUCC;
}

#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
VOS_INT PPP_PushPacketEvent(VOS_ULONG userData, IMM_Zc *immZc)
{
    PPP_Id         pppId    = (PPP_Id)userData;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    dataStat->downlinkCnt++;
    return PPP_DlPacketProc(pppId, immZc);
}

VOS_INT PPP_PushRawDataEvent(VOS_ULONG userData, IMM_Zc *immZc)
{
    PPP_Id         pppId    = (PPP_Id)userData;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    dataStat->downlinkCnt++;
    return PPP_DlRawDataProc(pppId, immZc);
}
VOS_UINT32 PPP_SendUlDataToAds(VOS_UINT16 pppId, PPP_Zc *immZc, VOS_UINT16 proto)
{
    VOS_UINT8      ifaceId  = 0;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    /* 通过usPppId，寻找到ucIfaceId */
    if (At_PppId2IfaceId(pppId, &ifaceId) != VOS_OK) {
        dataStat->uplinkDropCnt++;
        PPP_MemFree(immZc);
        PPP_MNTN_LOG2(PS_PRINT_NORMAL, "Can not get IfaceId", pppId, ifaceId);

        return PS_FAIL;
    }

    IMM_ZcSetProtocol(immZc, proto);

    /* 数据发送给ADS，如果失败由ADS释放内存 */
    if (ads_iface_tx(ifaceId, immZc) != VOS_OK) {
        dataStat->uplinkDropCnt++;

        return PS_FAIL;
    }

    dataStat->uplinkSndDataCnt++;

    return PS_SUCC;
}

#else
/* IP类型拨号，接收到Um/Uu口的数据 */
VOS_INT PPP_PushPacketEvent(VOS_UINT8 rabId, IMM_Zc *immZc, ADS_PktTypeUint8 pktType, VOS_UINT32 exParam)
{
    PPP_Id pppId;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    dataStat->downlinkCnt++;

    if (!PPP_RAB_TO_PPPID(&pppId, rabId)) {
        dataStat->downlinkDropCnt++;
        PPP_MemFree(immZc);
        PPP_MNTN_LOG1(PS_PRINT_NORMAL, "PPP, PPP_PushPacketEvent, NORMAL, Can not get PPP Id, RabId <1>", rabId);

        return PS_FAIL;
    }

    return PPP_DlPacketProc(pppId, immZc);
}

/* PPP类型拨号，接收到Um/Uu口的数据 */
VOS_INT PPP_PushRawDataEvent(VOS_UINT8 rabId, IMM_Zc *immZc, ADS_PktTypeUint8 pktType, VOS_UINT32 exParam)
{
    PPP_Id pppId;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    dataStat->downlinkCnt++;

    /* 通过RabId，寻找到PPP ID和相应的实体 */
    if (!PPP_RAB_TO_PPPID(&pppId, rabId)) {
        PPP_MemFree(immZc);
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "PPP, PPP_PushRawDataEvent, WARNING, Can not get PPP Id, RabId <1>", rabId);

        return PS_FAIL;
    }

    return PPP_DlRawDataProc(pppId, immZc);
}
VOS_UINT32 PPP_SendUlDataToAds(VOS_UINT16 pppId, PPP_Zc *immZc, VOS_UINT16 proto)
{
    VOS_UINT8 rabId = 0;
    PPP_DataQStat *dataStat = PPPA_GET_STAT_ADDR();

    /* 通过usPppId，寻找到usRabId */
    if (!PPP_PPPID_TO_RAB(pppId, &rabId)) {
        dataStat->uplinkDropCnt++;
        PPP_MemFree(immZc);
        PPP_MNTN_LOG2(
            PS_PRINT_NORMAL, "PPP, PPP_PushPacketEvent, WARNING, Can not get PPP Id %d, RabId %d", pppId, rabId);

        return PS_FAIL;
    }

    /* 数据发送给ADS，如果失败由ADS释放内存 */
    if (ADS_UL_SendPacket(immZc, rabId) != VOS_OK) {
        dataStat->uplinkDropCnt++;

        return PS_FAIL;
    }

    dataStat->uplinkSndDataCnt++;

    return PS_SUCC;
}

#endif

VOS_VOID PPPA_SetNotifyFlag(VOS_UINT32 notifyFlag)
{
    VOS_ULONG      flags     = 0UL;
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();

    VOS_SpinLockIntLock(&(dataQCtrl->spinLock), flags); /*lint !e571*/
    dataQCtrl->notifyMsg = notifyFlag;
    VOS_SpinUnlockIntUnlock(&(dataQCtrl->spinLock), flags);
}

VOS_VOID PPP_ProcDataNotify(VOS_VOID)
{
    PPP_Id         pppId;
    VOS_BOOL       delayProc = VOS_FALSE;
    PPP_DataQStat *dataStat  = PPPA_GET_STAT_ADDR();
    PPP_DataQCtrl *dataQCtrl = PPPA_GET_DATAQ_CTRL();

    PPP_Zc *mem = (PPP_Zc*)PPP_ZC_PEEK_QUEUE_HEAD(&dataQCtrl->dataQ);

    dataStat->procMsgCnt++;

    /* 队列为空的时候返回空指针 */
    if (mem == VOS_NULL_PTR) {
        PPP_MNTN_LOG2(LOG_LEVEL_WARNING, "PPP_ProcDataNotify, WARNING, queue is null!", dataQCtrl->notifyMsg,
            PPP_ZC_GET_QUEUE_LEN(&(dataQCtrl->dataQ)));
        PPPA_SetNotifyFlag(VOS_TRUE);
        return;
    }

    /* 处理该结点(结点的释放动作已经在各处理函数内部完成，无需再释放结点) */
    pppId = (PPP_ZC_GET_DATA_APP(mem) & 0xFF00) >> 8;

#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_OFF)
    delayProc = PPP_HDLC_SoftProcData(pppId, &g_pppDataqCtrl.dataQ);
#else
    delayProc = PPP_HDLC_HardProcData(pppId, &g_pppDataqCtrl.dataQ);
#endif

    /* 如果需要延时处理则启动定时器，此时不需设置通知标记 */
    if (delayProc == VOS_TRUE) {
        VOS_StartRelTimer(&(dataQCtrl->delayProtTimer), PS_PID_APP_PPP, g_pppDelayTimerLen, PPPA_DATA_DELAY_PROC_TIMER,
            (VOS_UINT32)pppId, VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_0);
        return;
    }

    PPPA_SetNotifyFlag(VOS_TRUE);
    if (PPP_ZC_GET_QUEUE_LEN(&(dataQCtrl->dataQ)) > 0) {
        PPP_NotiFyProcData(dataQCtrl);
    }
}
/* 目前协商报文是通过VOS消息发送，每秒最多发送100个协商报文 */
#define PPPA_MAX_NEGO_PKT_CNT_PER_SEC 100

/* 目前为32K时钟，1s为32768个slice */
#define PPPA_SLICE_CNT_OF_ONE_SEC 32768

VOS_VOID PPPA_SendPppcNegoData(PPP_Zc *negoData, VOS_UINT16 proto)
{
    PPPC_PPPA_NegoData *negoMsg    = VOS_NULL_PTR;
    VOS_UINT16          msgLen     = sizeof(PPPC_PPPA_NegoData) + PPP_ZC_GET_DATA_LEN(negoData);
    static VOS_UINT16   msgSendCnt = 0;
    static VOS_UINT32   beginTime  = 0;
    VOS_UINT32          currTime;

    if (msgSendCnt == 0) {
        /* 发送第一个报文时记录起始时间 */
        beginTime = mdrv_timer_get_normal_timestamp();
    } else {
        currTime = mdrv_timer_get_normal_timestamp();
        if (currTime - beginTime > PPPA_SLICE_CNT_OF_ONE_SEC) {
            /* 当前时间和起始时间间隔超过1s，重新开始计数 */
            msgSendCnt = 0;
            beginTime  = currTime;
        } else {
            /* 1s内发送的协商报文数量大于100，不能继续发送协商报文 */
            if (msgSendCnt > PPPA_MAX_NEGO_PKT_CNT_PER_SEC) {
                PPP_MNTN_LOG(PS_PRINT_WARNING, "too many nego pkt");
                return;
            }
        }
    }
    msgSendCnt++;
    negoMsg = (PPPC_PPPA_NegoData*)PS_ALLOC_MSG_WITH_HEADER_LEN(PS_PID_APP_PPP, msgLen);
    if (negoMsg == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "alloc Msg Fail");
        return;
    }
    TTF_SET_MSG_RECEIVER_ID(negoMsg, MSPS_PID_PPPC);
    negoMsg->msgId   = ID_PPPA_PPPC_NEGO_DATA_IND;
    negoMsg->proto   = proto;
    negoMsg->dataLen = PPP_ZC_GET_DATA_LEN(negoData);
    negoMsg->pktType = PPPC_PPPA_PKT_TYPE_BUTT;

    if (negoMsg->dataLen > 0) {
        PPPA_SF_CHK(memcpy_s(negoMsg->data, negoMsg->dataLen, PPP_ZC_GET_DATA_PTR(negoData), negoMsg->dataLen));
    }

    (VOS_VOID)PS_SEND_MSG(PS_PID_APP_PPP, negoMsg);
}

/* 将PPP勾包结构发送到OM，并填入公共信息字段信息 */
VOS_VOID PPP_FrameMntnInfo(PPP_FrameMntn *ptrPppMntnSt, VOS_UINT16 dataLen)
{
    TTF_SET_MSG_SENDER_ID(ptrPppMntnSt, PS_PID_APP_PPP);
    TTF_SET_MSG_RECEIVER_ID(ptrPppMntnSt, MSPS_PID_PPPC);
    TTF_SET_MSG_LEN(ptrPppMntnSt, (dataLen + sizeof(PPP_FrameMntn)) - VOS_MSG_HEAD_LENGTH);
    ptrPppMntnSt->msgName   = PPP_RECV_PROTO_PACKET_TYPE;
    ptrPppMntnSt->pppPhase  = 0;
    ptrPppMntnSt->ipcpState = 0;
    ptrPppMntnSt->lcpState  = 0;
    ptrPppMntnSt->pppId     = PPPA_PPP_ID;
    ptrPppMntnSt->dataLen   = dataLen;

    PPP_MNTN_TRACE_MSG(ptrPppMntnSt);
}
/* 将TTF_MEM结构的PPP帧进行勾包 */
VOS_VOID PPP_TtfMemFrameMntnInfo(PPP_Zc *mem, VOS_UINT16 proto)
{
    VOS_UINT16     frameLen          = 0;
    PPP_FrameMntn *ptrPppFrameMntnSt = VOS_NULL_PTR;
    VOS_UINT8     *buff              = VOS_NULL_PTR;
    VOS_UINT32     ret               = PS_FAIL;

    /* PPP 帧长度 */
    frameLen = (VOS_UINT16)PPP_ZC_GET_DATA_LEN(mem);

    ptrPppFrameMntnSt = (PPP_FrameMntn*)PS_MEM_ALLOC(PS_PID_APP_PPP, frameLen + sizeof(PPP_FrameMntn) + sizeof(proto));
    if (ptrPppFrameMntnSt == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP, PPP_TtfMemFrameMntnInfo, ERROR, Call VOS_MemAlloc fail!\n");
        return;
    }

    /* 填入PPP帧协议类型,网络字节序 */
    buff                   = (VOS_UINT8*)(ptrPppFrameMntnSt + 1);
    *((VOS_UINT16*)(buff)) = (VOS_HTONS(proto));
    (buff) += 2;

    /* 填入帧内容,原始复制即可 */
    ret = PPP_MemGet(mem, 0, buff, frameLen);
    if (ret != PS_SUCC) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "PPP, Ppp_frame_MntnInfo, ERROR, TTF_MemGet Fail!\n");
        PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);
        return;
    }

    /*
     * 发送可维可测信息
     * 长度: 除可维可测信息头部外数据部分载荷
     *       涵盖帧长 + 2byte protocol字段
     */
    PPP_FrameMntnInfo(ptrPppFrameMntnSt, frameLen + sizeof(proto));

    /* 释放内存 */
    PS_MEM_FREE(PS_PID_APP_PPP, ptrPppFrameMntnSt);
}

VOS_VOID PPP_HDLC_ProcIpModeUlData(VOS_UINT16 pppId, PPP_Zc *mem, VOS_UINT16 proto)
{
    if (proto != PPPA_IP) {
        /* 把PPP协商包作为可维可测信息,IP包不做可维可测维护 */
        PPP_TtfMemFrameMntnInfo(mem, proto);
        PPPA_SendPppcNegoData(mem, proto);
        PPP_MemFree(mem);
    } else {
        PPP_SendUlDataToAds(pppId, mem, PPPA_ETH_IPV4_PROTO);
    }
}

VOS_VOID PPP_SetupHdlc(VOS_UINT16 pppId, VOS_BOOL ipMode)
{
#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_OFF)
    PPP_HDLC_SetUp(pppId, ipMode);
#else
    PPP_ServiceHdlcHardSetUp(pppId);
#endif
}

VOS_VOID PPP_ReleaseHdlc(VOS_UINT16 pppId)
{
#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_OFF)
    PPP_ConvRelease(pppId);
#endif
}

VOS_VOID PPPA_SetUsedFlag(VOS_BOOL isUsed)
{
    g_pppUsed = isUsed;
}

VOS_VOID PPPA_DataEventProc(VOS_VOID)
{
#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_ON)
    PPP_ServiceHdlcHardOpenClk();
#endif

    PPP_ProcDataNotify();

#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_ON)
    PPP_ServiceHdlcHardCloseClk();
#endif
}

VOS_VOID PPPA_InputProcEchoReq(VOS_VOID)
{
    static VOS_UINT32 preDropCnt = 0;
    PPP_DataQStat    *dataStat   = PPPA_GET_STAT_ADDR();

    if (dataStat->dropCnt == preDropCnt) {
        return;
    }
    preDropCnt = dataStat->dropCnt;
    PPPA_SendPppcCommMsg(ID_PPPA_PPPC_IGNORE_ECHO);
}

