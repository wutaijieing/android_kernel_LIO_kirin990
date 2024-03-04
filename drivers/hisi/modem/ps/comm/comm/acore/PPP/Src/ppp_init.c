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

#include "ppp_init.h"
#include "pppc_pppa_interface.h"
#include "product_config.h"
#include "ppp_public.h"

#include "ppp_input.h"
#include "mdrv_nvim.h"
#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_ON)
#include "hdlc_hardware_service.h"
#include "hdlc_hardware.h"
#else
#include "hdlc_software.h"
#endif
#include "nv_stru_gucnas.h"
#include "ttf_comm.h"
#include "gucttf_tag.h"
#include "ps_log_filter_interface.h"
#include "linux_interface.h"
#include "ppp_internal_msg.h"
#include "ppp_atcmd.h"
#include "securec.h"


#define THIS_FILE_ID PS_FILE_ID_PPP_INIT_C
#define THIS_MODU mod_ppp

#define PPP_TASK_PRI 143

#define PPPA_OFFSETOF(s, m) /*lint -e(545)*/ TTF_OFFSET_OF(s, m)

#define PPPA_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
    VOS_UINT16 msgId;
    VOS_UINT16 traceLen;
} PPPA_MsgTraceLenTable;

VOS_UINT8 g_logFilterCfg = VOS_TRUE;

VOS_VOID PPP_BindToCpu(VOS_VOID)
{
    struct cpumask mask;

    /* 获取当前线程的affinity */
    if (sched_getaffinity(current->pid, &mask) < 0) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "warning: unable to get cpu affinity\n");
        return;
    }

    /* 如果CPU核数超过1则去绑定核0 */
    if (cpumask_weight(&mask) > 1) {
        cpumask_clear_cpu(0, &mask);
        if (sched_setaffinity(current->pid, &mask) < 0) {
            PPP_MNTN_LOG(PS_PRINT_WARNING, "warning: unable to set cpu affinity\n");
        }
    }
}

VOS_VOID PPPA_ReadNv(VOS_VOID)
{
    VOS_UINT32              ret;
    NAS_NV_PrivacyFilterCfg privacyFilterCfg = {0};

    /* 读取脱敏控制NV */
    ret = GUCTTF_NV_Read(
        MODEM_ID_0, NV_ITEM_PRIVACY_LOG_FILTER_CFG, &privacyFilterCfg, sizeof(NAS_NV_PrivacyFilterCfg));
    if (ret != NV_OK) {
        TTF_LOG(PS_PID_APP_PPP, DIAG_MODE_COMM, PS_PRINT_ERROR, "read log filter nv fail");
        g_logFilterCfg = VOS_TRUE;
        return;
    }

    g_logFilterCfg = privacyFilterCfg.filterEnableFlg;
}

VOS_UINT8 PPPA_GetPrivacyFilterCfg(VOS_VOID)
{
    return VOS_TRUE;
}

VOS_VOID* PPPA_FilterPppcPppaMsg(MsgBlock *msg)
{
    VOS_UINT32            loop;
    PPPC_PPPA_CommMsg    *commMsg      = (PPPC_PPPA_CommMsg*)msg;
    MsgBlock             *traceMsg     = VOS_NULL_PTR;
    PPPA_MsgTraceLenTable traceTable[] = {
        { ID_PPPC_PPPA_NEGO_DATA_IND, PPPA_OFFSETOF(PPPC_PPPA_NegoData, data) },
        { ID_PPPC_PPPA_PDP_ACTIVE_REQ, PPPA_OFFSETOF(PPPC_PPPA_PdpActiveReq, config) }
    };

    if (PPPA_GetPrivacyFilterCfg() != VOS_TRUE) {
        return msg;
    }

    if (TTF_GET_MSG_RECEIVER_ID(msg) != PS_PID_APP_PPP) {
        return msg;
    }

    for (loop = 0; loop < PPPA_ARRAY_SIZE(traceTable); loop++) {
        if (commMsg->msgId == traceTable[loop].msgId) {
            traceMsg = (MsgBlock*)PS_MEM_ALLOC(MSPS_PID_PPPC, traceTable[loop].traceLen);
            if (traceMsg == VOS_NULL_PTR) {
                return VOS_NULL_PTR;
            }
            (VOS_VOID)memcpy_s(traceMsg, traceTable[loop].traceLen, msg, traceTable[loop].traceLen);
            traceMsg->ulLength = traceTable[loop].traceLen - VOS_MSG_HEAD_LENGTH;
            return traceMsg;
        }
    }
    return msg;
}

VOS_VOID* PPPA_FilterPppaPppcMsg(MsgBlock *msg)
{
    VOS_UINT32            loop;
    PPPC_PPPA_CommMsg    *commMsg      = (PPPC_PPPA_CommMsg*)msg;
    MsgBlock             *traceMsg     = VOS_NULL_PTR;
    PPPA_MsgTraceLenTable traceTable[] = {
        { ID_PPPA_PPPC_NEGO_DATA_IND, PPPA_OFFSETOF(PPPC_PPPA_NegoData, data) },
        { ID_PPPA_PPPC_PDP_ACTIVE_RSP, PPPA_OFFSETOF(PPPA_PPPC_PdpActiveRsp, result) }
    };

    if (PPPA_GetPrivacyFilterCfg() != VOS_TRUE) {
        return msg;
    }

    if (TTF_GET_MSG_RECEIVER_ID(msg) != MSPS_PID_PPPC) {
        return msg;
    }

    for (loop = 0; loop < PPPA_ARRAY_SIZE(traceTable); loop++) {
        if (commMsg->msgId == traceTable[loop].msgId) {
            traceMsg = (MsgBlock*)PS_MEM_ALLOC(PS_PID_APP_PPP, traceTable[loop].traceLen);
            if (traceMsg == VOS_NULL_PTR) {
                return VOS_NULL_PTR;
            }
            (VOS_VOID)memcpy_s(traceMsg, traceTable[loop].traceLen, msg, traceTable[loop].traceLen);
            traceMsg->ulLength = traceTable[loop].traceLen - VOS_MSG_HEAD_LENGTH;
            return traceMsg;
        }
    }
    return msg;
}

VOS_VOID PPPA_PppcLogFilterInit(VOS_VOID)
{
    PS_OM_LayerMsgReplaceCBReg(PS_PID_APP_PPP, PPPA_FilterPppaPppcMsg);

    PS_OM_LayerMsgReplaceCBReg(MSPS_PID_PPPC, PPPA_FilterPppcPppaMsg);
}

VOS_UINT32 PPP_PidInit(enum VOS_InitPhaseDefine initPhase)
{
    switch (initPhase) {
        case VOS_IP_LOAD_CONFIG:
            PPPA_InitCfgInfo();

            PPPA_ReadNv();

            PPPA_PppcLogFilterInit();

            /* 初始化PPP的数据队列 */
            if (PPP_DataQInit() != VOS_OK) {
                return VOS_ERR;
            }
            break;

        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;
    }

    return VOS_OK;
}

VOS_VOID PPPA_ProcTimerMsg(struct MsgCB *msg)
{
    REL_TimerMsgBlock *psMsg = (REL_TimerMsgBlock*)msg;

    if (psMsg->name == PPPA_DATA_DELAY_PROC_TIMER) {
        PPPA_DataEventProc();
    } else {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "Unknow Timer!\n", psMsg->name);
    }
}

VOS_VOID PPPA_RcvPppcNegoData(PPPC_PPPA_NegoData *negoData)
{
    PPP_Zc *mem = VOS_NULL_PTR;

    if (negoData->dataLen != negoData->ulLength - (sizeof(PPPC_PPPA_NegoData) - VOS_MSG_HEAD_LENGTH)) {
        PPP_MNTN_LOG2(PS_PRINT_WARNING, "wrong ulLength!\n", negoData->dataLen, negoData->ulLength);
        return;
    }

    mem = PPP_MemCopyAlloc(negoData->data, negoData->dataLen, 0);
    PPPA_SendDlDataToAT(PPPA_PPP_ID, mem);
    if (negoData->pktType == PPP_ECHO_REQ) {
        PPPA_InputProcEchoReq();
    }
}

VOS_VOID PPPA_RcvPppcPdpActReq(PPPC_PPPA_PdpActiveReq *msg)
{
    if (msg->ulLength != sizeof(PPPC_PPPA_PdpActiveReq) - VOS_MSG_HEAD_LENGTH) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "wrong ulLength!\n", msg->ulLength);
        return;
    }

    At_RcvTeConfigInfoReq(PPPA_PPP_ID, &((msg)->config));
}

VOS_VOID PPPA_RcvPppcPppRelInd(PPPC_PPPA_CommMsg *msg)
{
    if (msg->ulLength != sizeof(PPPC_PPPA_CommMsg) - VOS_MSG_HEAD_LENGTH) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "wrong ulLength!\n", msg->ulLength);
        return;
    }

    At_RcvPppReleaseInd(PPPA_PPP_ID);

    PPP_ReleaseHdlc(PPPA_PPP_ID);
}

VOS_VOID PPPA_RcvHdlcCfgInfo(PPPC_PPPA_HdlcCfg *hdlcCfg)
{
#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_ON)
    HDLC_CfgInfo saveCfg = {0};
#endif

    if (hdlcCfg->ulLength != sizeof(PPPC_PPPA_HdlcCfg) - VOS_MSG_HEAD_LENGTH) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "wrong ulLength!\n", hdlcCfg->ulLength);
        return;
    }

#if (FEATURE_HARDWARE_HDLC_FUNC == FEATURE_ON)
    saveCfg.hisAcf = hdlcCfg->hisAcf;
    saveCfg.hisPcf = hdlcCfg->hisPcf;
    saveCfg.isAccm = hdlcCfg->hisAccm;
    saveCfg.myAcf  = hdlcCfg->myAcf;
    saveCfg.myPcf  = hdlcCfg->myPcf;
    PPP_HDLC_HardUpdateCfg(&saveCfg);
#else
    PPP_HDLC_UpdateCfg(PPPA_PPP_ID, hdlcCfg->hisAcf, hdlcCfg->hisPcf, hdlcCfg->hisAccm);
#endif
}

VOS_VOID PPPA_PppcMsgProc(struct MsgCB *msg)
{
    PPPC_PPPA_CommMsg *commMsg = (PPPC_PPPA_CommMsg*)msg;

    switch (commMsg->msgId) {
        case ID_PPPC_PPPA_NEGO_DATA_IND:
            PPPA_RcvPppcNegoData((PPPC_PPPA_NegoData*)msg);
            break;
        case ID_PPPC_PPPA_PDP_ACTIVE_REQ:
            PPPA_RcvPppcPdpActReq((PPPC_PPPA_PdpActiveReq*)msg);
            break;
        case ID_PPPC_PPPA_PPP_RELEASE_IND:
            PPPA_RcvPppcPppRelInd(commMsg);
            break;
        case ID_PPPC_PPPA_HDLC_CFG_IND:
            PPPA_RcvHdlcCfgInfo((PPPC_PPPA_HdlcCfg*)msg);
            break;
        default:
            PPP_MNTN_LOG1(PS_PRINT_ERROR, "err msg", commMsg->msgId);
    }
}

VOS_VOID PPPA_PppaMsgProc(struct MsgCB *msg)
{
    PPP_InternalMsgHeader *ctrlOper = (PPP_InternalMsgHeader*)msg;

    switch (ctrlOper->msgId) {
        case ID_PPPA_AT_CTRL_OPERATION_MSG:
            PPPA_AtCtrlOperMsgProc(ctrlOper);
            break;
        default:
            PPP_MNTN_LOG1(PS_PRINT_ERROR, "err msg", ctrlOper->msgId);
    }
}

extern VOS_MsgHookFunc vos_MsgHook;
VOS_VOID PPP_MsgProc(struct MsgCB *msg)
{
    if (msg == VOS_NULL_PTR) {
        return;
    }

    if (vos_MsgHook != VOS_NULL_PTR) {
        (vos_MsgHook)((VOS_VOID*)msg);
    }

    switch (TTF_GET_MSG_SENDER_ID(msg)) {
        case VOS_PID_TIMER:
            PPPA_ProcTimerMsg(msg);
            break;
        case MSPS_PID_PPPC:
            PPPA_PppcMsgProc(msg);
            break;
        case PS_PID_APP_PPP:
            PPPA_PppaMsgProc(msg);
            break;
        default:
            PPP_MNTN_LOG1(PS_PRINT_ERROR, "err msg", TTF_GET_MSG_SENDER_ID(msg));
    }
}

VOS_VOID PPP_FidTask(VOS_UINT32 para1, VOS_UINT32 para2, VOS_UINT32 para3, VOS_UINT32 para4)
{
    MsgBlock  *msg         = VOS_NULL_PTR;
    VOS_UINT32 event       = 0;
    VOS_UINT32 taskId      = 0;
    VOS_UINT32 rtn         = VOS_ERR;
    VOS_UINT32 eventMask   = 0;
    VOS_UINT32 expectEvent = 0;

    taskId = VOS_GetCurrentTaskID();
    if (taskId == PS_NULL_UINT32) {
        PPP_MNTN_LOG(PS_PRINT_ERROR, "PPP_FidTask: ERROR, TaskID is invalid.");
        return;
    }

    if (VOS_CreateEvent(taskId) != VOS_OK) {
        PPP_MNTN_LOG(PS_PRINT_ERROR, "PPP_FidTask: ERROR, create event fail.");
        return;
    }

    PPP_BindToCpu();

    expectEvent = PPP_RCV_DATA_EVENT | VOS_MSG_SYNC_EVENT;
    eventMask   = VOS_EVENT_ANY | VOS_EVENT_WAIT;

    PPPA_SaveTaskId(taskId);

    for (;;) {
        rtn = VOS_EventRead(expectEvent, eventMask, 0, &event);
        if (rtn != VOS_OK) {
            PPP_MNTN_LOG(PS_PRINT_ERROR, "PPP_FidTask: ERROR, read event error.");
            continue;
        }

        /* 事件处理 */
        if ((event & PPP_RCV_DATA_EVENT) == PPP_RCV_DATA_EVENT) {
            PPPA_DataEventProc();
            continue;
        }

        msg = (MsgBlock*)VOS_GetMsg(taskId);
        if (msg != VOS_NULL_PTR) {
            if (TTF_GET_MSG_RECEIVER_ID(msg) == PS_PID_APP_PPP) {
                PPP_MsgProc(msg);
            }

            (VOS_VOID)VOS_FreeMsg(PS_PID_APP_PPP, msg);
        }
    }
}

VOS_UINT32 PPP_FidInit(enum VOS_InitPhaseDefine ip)
{
    VOS_UINT32 rslt;

    switch (ip) {
        case VOS_IP_LOAD_CONFIG:
            /* 上行PID初始化 */
            rslt = VOS_RegisterPIDInfo(PS_PID_APP_PPP, (InitFunType)PPP_PidInit, (MsgFunType)PPP_MsgProc);
            if (rslt != VOS_OK) {
                TTF_PRINT_ERR("[init]<PPP_FidInit>, register PPP PID fail!\n");
                return VOS_ERR;
            }

            rslt = VOS_RegisterMsgTaskEntry(ACPU_FID_PPP, PPP_FidTask);
            if (rslt != VOS_OK) {
                TTF_PRINT_ERR("[init]<PPP_FidInit>, VOS_RegisterMsgTaskEntry fail!\n");
                return VOS_ERR;
            }

            /* 任务优先级 */
            rslt = VOS_RegisterTaskPrio(ACPU_FID_PPP, VOS_PRIORITY_P4);
            if (rslt != VOS_OK) {
                TTF_PRINT_ERR("[init]<PPP_FidInit>, register priority fail!\n");
                return VOS_ERR;
            }

            break;

        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;
    }

    return VOS_OK;
}

