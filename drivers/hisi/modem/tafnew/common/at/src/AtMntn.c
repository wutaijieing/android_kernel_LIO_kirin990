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

#include "AtMntn.h"
#include "AtInputProc.h"
#include "AtCtx.h"
#include "AtDataProc.h"
#include "TafLogPrivacyMatch.h"
#include "ATCmdProc.h"
#include "TafAcoreLogPrivacy.h"
#include "securec.h"
#include "mn_comm_api.h"
#include "GuNasLogFilter.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_AT_MNTN_C
#define AT_HSUART_PARITY_MAX_LEN 20
#define AT_HSUART_DCD_MAX_LEN 20
#define AT_HSUART_DTR_MAX_LEN 20
#define AT_HSUART_DSR_MAX_LEN 20
#define AT_HSUART_DCE_BY_DTE_MAX_LEN 20
#define AT_HSUART_DTE_BY_DCE_MAX_LEN 20
#define AT_HSUART_FC_STATE_MAX_LEN 20
#define AT_CLIENT_STATE_MAX_LEN 20
#define AT_CLIENT_MODE_MAX_LEN 20

AT_DebugInfo g_atDebugInfo = {VOS_FALSE};

AT_MntnStats g_atStatsInfo;

AT_MntnMsgRecordInfo g_atMsgRecordInfo;

AT_MntnPortInfoLog g_atMntnPortInfo[AT_MNTN_MAX_PORT_NUM];

VOS_UINT8 *g_atExcAddr = VOS_NULL_PTR;


VOS_VOID AT_InitMntnCtx(VOS_VOID)
{
    memset_s(&g_atMsgRecordInfo, sizeof(g_atMsgRecordInfo), 0x00, sizeof(g_atMsgRecordInfo));
    memset_s(g_atMntnPortInfo, sizeof(g_atMntnPortInfo), 0x00, sizeof(g_atMntnPortInfo));
}


VOS_VOID AT_SetPcuiCtrlConcurrentFlag(VOS_UINT8 flag)
{
    g_atDebugInfo.pcuiCtrlConcurrentFlg = flag;
}


VOS_UINT8 AT_GetPcuiCtrlConcurrentFlag(VOS_VOID)
{
    return g_atDebugInfo.pcuiCtrlConcurrentFlg;
}


VOS_VOID AT_SetPcuiPsCallFlag(VOS_UINT8 flag, VOS_UINT8 indexNum)
{
    g_atDebugInfo.pcuiPsCallFlg = flag;
    g_atDebugInfo.pcuiUserId    = AT_CLIENT_ID_APP;
}


VOS_UINT8 AT_GetPcuiPsCallFlag(VOS_VOID)
{
    return g_atDebugInfo.pcuiPsCallFlg;
}


VOS_UINT8 AT_GetPcuiUsertId(VOS_VOID)
{
    return g_atDebugInfo.pcuiUserId;
}


VOS_VOID AT_SetCtrlPsCallFlag(VOS_UINT8 flag, VOS_UINT8 indexNum)
{
    g_atDebugInfo.ctrlPsCallFlg = flag;
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    g_atDebugInfo.ctrlUserId = AT_CLIENT_ID_APP5;
#else
    g_atDebugInfo.ctrlUserId  = AT_CLIENT_ID_APP;
#endif
}


VOS_UINT8 AT_GetCtrlPsCallFlag(VOS_VOID)
{
    return g_atDebugInfo.ctrlPsCallFlg;
}


VOS_UINT8 AT_GetCtrlUserId(VOS_VOID)
{
    return g_atDebugInfo.ctrlUserId;
}


VOS_VOID AT_SetPcui2PsCallFlag(VOS_UINT8 flag, VOS_UINT8 indexNum)
{
    g_atDebugInfo.pcui2PsCallFlg = flag;
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    g_atDebugInfo.pcui2UserId = AT_CLIENT_ID_APP20;
#else
    g_atDebugInfo.pcui2UserId = AT_CLIENT_ID_APP;
#endif /* #if FEATURE_ON == FEATURE_VCOM_EXT */
}


VOS_UINT8 AT_GetPcui2PsCallFlag(VOS_VOID)
{
    return g_atDebugInfo.pcui2PsCallFlg;
}


VOS_UINT8 AT_GetPcui2UserId(VOS_VOID)
{
    return g_atDebugInfo.pcui2UserId;
}


VOS_VOID AT_SetUnCheckApPortFlg(VOS_UINT8 flag)
{
    if ((flag == 0) || (flag == 1)) {
        g_atDebugInfo.unCheckApPortFlg = flag;
    }
}


VOS_UINT8 AT_GetUnCheckApPortFlg(VOS_VOID)
{
    return g_atDebugInfo.unCheckApPortFlg;
}


VOS_VOID AT_MntnTraceEvent(struct MsgCB *msg)
{
    VOS_VOID *logPrivacyMsg = VOS_NULL_PTR;

    /* at模块内部维测消息, 增加脱敏处理 */
    logPrivacyMsg = msg;

    if (AT_GetPrivacyFilterEnableFlg() == VOS_TRUE) {
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
        if (GUNAS_FilterAtToAtMsg((PS_MsgHeader *)msg) == VOS_NULL_PTR) {
            return;
        }

        /* cnas at命令脱敏过滤函数处理 */
        logPrivacyMsg = AT_PrivacyMatchAtCmd((struct MsgCB *)msg);
        if (logPrivacyMsg == VOS_NULL_PTR) {
            return;
        }
#endif
    }

    mdrv_diag_trace_report(logPrivacyMsg);

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    /* 如果脱敏处理函数申请了新的at命令字符串，释放掉 */
    if (logPrivacyMsg != msg) {
        VOS_MemFree(WUEPS_PID_AT, logPrivacyMsg);
    }
#endif

    return;
}


VOS_VOID AT_MntnTraceInputMsc(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc)
{
    AT_MntnMsc mntnMsc;
    errno_t    memResult;

    /* 填写消息头 */
    AT_MNTN_CFG_MSG_HDR(&mntnMsc, ID_AT_MNTN_INPUT_MSC, (sizeof(AT_MntnMsc) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    mntnMsc.portId = indexNum;
    memResult      = memcpy_s(&(mntnMsc.dceMscInfo), sizeof(mntnMsc.dceMscInfo), dceMsc, sizeof(struct modem_msc_stru));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(mntnMsc.dceMscInfo), sizeof(struct modem_msc_stru));

    /* 发送消息 */
    AT_MntnTraceEvent((struct MsgCB *)&mntnMsc);

    return;
}


VOS_VOID AT_MntnTraceOutputMsc(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc)
{
    AT_MntnMsc mntnMsc;
    errno_t    memResult;

    /* 填写消息头 */
    AT_MNTN_CFG_MSG_HDR(&mntnMsc, ID_AT_MNTN_OUTPUT_MSC, (sizeof(AT_MntnMsc) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    mntnMsc.portId = indexNum;
    memResult      = memcpy_s(&(mntnMsc.dceMscInfo), sizeof(mntnMsc.dceMscInfo), dceMsc, sizeof(struct modem_msc_stru));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(mntnMsc.dceMscInfo), sizeof(struct modem_msc_stru));

    /* 发送消息 */
    AT_MntnTraceEvent((struct MsgCB *)&mntnMsc);

    return;
}


VOS_VOID AT_MntnTraceStartFlowCtrl(VOS_UINT8 indexNum, AT_FC_DeviceTypeUint32 fcDevive)
{
    AT_MntnFlowCtrl mntnFlowCtrl;

    /* 填写消息头 */
    AT_MNTN_CFG_MSG_HDR(&mntnFlowCtrl, ID_AT_MNTN_START_FLOW_CTRL, (sizeof(AT_MntnFlowCtrl) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    mntnFlowCtrl.portId = indexNum;
    mntnFlowCtrl.device = fcDevive;

    /* 发送消息 */
    AT_MntnTraceEvent((struct MsgCB *)&mntnFlowCtrl);

    return;
}


VOS_VOID AT_MntnTraceStopFlowCtrl(VOS_UINT8 indexNum, AT_FC_DeviceTypeUint32 fcDevive)
{
    AT_MntnFlowCtrl mntnFlowCtrl;

    /* 填写消息头 */
    AT_MNTN_CFG_MSG_HDR(&mntnFlowCtrl, ID_AT_MNTN_STOP_FLOW_CTRL, (sizeof(AT_MntnFlowCtrl) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    mntnFlowCtrl.portId = indexNum;
    mntnFlowCtrl.device = fcDevive;

    /* 发送消息 */
    AT_MntnTraceEvent((struct MsgCB *)&mntnFlowCtrl);

    return;
}


VOS_VOID AT_MntnTraceRegFcPoint(VOS_UINT8 indexNum, AT_FC_PointTypeUint32 fcPoint)
{
    AT_MntnFcPoint mntnFcPoint;

    /* 填写消息头 */
    AT_MNTN_CFG_MSG_HDR(&mntnFcPoint, ID_AT_MNTN_REG_FC_POINT, (sizeof(AT_MntnFcPoint) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    mntnFcPoint.portId = indexNum;
    mntnFcPoint.point  = fcPoint;

    /* 发送消息 */
    AT_MntnTraceEvent((struct MsgCB *)&mntnFcPoint);

    return;
}


VOS_VOID AT_MntnTraceDeregFcPoint(VOS_UINT8 indexNum, AT_FC_PointTypeUint32 fcPoint)
{
    AT_MntnFcPoint mntnFcPoint;

    /* 填写消息头 */
    AT_MNTN_CFG_MSG_HDR(&mntnFcPoint, ID_AT_MNTN_DEREG_FC_POINT, (sizeof(AT_MntnFcPoint) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息内容 */
    mntnFcPoint.portId = indexNum;
    mntnFcPoint.point  = fcPoint;

    /* 发送消息 */
    AT_MntnTraceEvent((struct MsgCB *)&mntnFcPoint);

    return;
}


VOS_VOID AT_MntnTraceCmdResult(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 dataLen)
{
    AT_Msg             *msg = VOS_NULL_PTR;
    AT_InterMsgIdUint32 eventId;
    VOS_UINT32          length;
    errno_t             memResult;
    ModemIdUint16 modemId;

    AT_GetAtMsgStruMsgLength(dataLen, &length);

    /* 申请消息内存 */
    msg = (AT_Msg *)PS_MEM_ALLOC(WUEPS_PID_AT, (length + VOS_MSG_HEAD_LENGTH));
    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_MNTN_TraceCmdResult:ERROR:Alloc Mem Fail.");
        return;
    }

    /* 填写消息头 */
    eventId = AT_GetResultMsgID(indexNum);
    AT_MNTN_CFG_MSG_HDR(msg, eventId, length);

    /* 填写消息内容 */
    msg->type     = 0x1;
    msg->indexNum = indexNum;
    msg->len      = dataLen;

    modemId = MODEM_ID_0;
    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        modemId = MODEM_ID_0;
    }

    msg->modemId   = (VOS_UINT8)modemId;
    msg->versionId = 0xAA;

    msg->filterAtType = (VOS_UINT8)g_logPrivacyAtCmd;

    AT_GetUserTypeFromIndex(indexNum, &msg->userType);

    if (dataLen > 0) {
        memResult = memcpy_s(msg->value, dataLen, data, dataLen);
        TAF_MEM_CHK_RTN_VAL(memResult, dataLen, dataLen);
    }

    /* 填写消息内容 */
    AT_MntnTraceEvent((struct MsgCB *)msg);

    /* 释放消息内存 */
    /*lint -save -e830 */
    PS_MEM_FREE(WUEPS_PID_AT, msg);
    /*lint -restore */
    return;
}



VOS_VOID AT_MntnTraceContextData(VOS_VOID)
{
    NAS_AT_SDT_AT_PART_ST                  *sndMsgCB   = VOS_NULL_PTR;
    NAS_AT_OUTSIDE_RUNNING_CONTEXT_PART_ST *outsideCtx = VOS_NULL_PTR;
    AT_ModemSmsCtx                         *smsCtx     = VOS_NULL_PTR;
    errno_t                                 memResult;
    ModemIdUint16                           modemId;
    /*lint -save -e516 */
    sndMsgCB = (NAS_AT_SDT_AT_PART_ST *)PS_ALLOC_MSG(WUEPS_PID_AT, sizeof(NAS_AT_SDT_AT_PART_ST) - VOS_MSG_HEAD_LENGTH);
    /*lint -restore */
    if (sndMsgCB == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_MNTN_TraceContextData:ERROR: Alloc Memory Fail.");
        return;
    }

    TAF_CfgMsgHdr((MsgBlock *)sndMsgCB, WUEPS_PID_AT, WUEPS_PID_AT,
                  sizeof(NAS_AT_SDT_AT_PART_ST) - VOS_MSG_HEAD_LENGTH);

    sndMsgCB->type        = AT_PC_REPLAY_MSG;
    sndMsgCB->msgID       = ID_AT_MNTN_PC_REPLAY_MSG;
    sndMsgCB->reserved[0] = 0;
    sndMsgCB->reserved[1] = 0;
    sndMsgCB->reserved[2] = 0;

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        smsCtx     = AT_GetModemSmsCtxAddrFromModemId(modemId);
        outsideCtx = &sndMsgCB->outsideCtx[modemId];

        outsideCtx->atCscsType       = g_atCscsType;
        outsideCtx->atCsdhType       = smsCtx->csdhType;
        outsideCtx->opId             = g_opId;
        outsideCtx->atCsmsMsgVersion = smsCtx->csmsMsgVersion;
        outsideCtx->atCmgfMsgFormat  = smsCtx->cmgfMsgFormat;

        memResult = memcpy_s(&outsideCtx->atCnmiType, sizeof(outsideCtx->atCnmiType), &(smsCtx->cnmiType),
                             sizeof(smsCtx->cnmiType));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(outsideCtx->atCnmiType), sizeof(smsCtx->cnmiType));

        memResult = memcpy_s(&outsideCtx->atCscaCsmpInfo, sizeof(outsideCtx->atCscaCsmpInfo), &(smsCtx->cscaCsmpInfo),
                             sizeof(smsCtx->cscaCsmpInfo));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(outsideCtx->atCscaCsmpInfo), sizeof(smsCtx->cscaCsmpInfo));

        memResult = memcpy_s(&outsideCtx->atCpmsInfo, sizeof(outsideCtx->atCpmsInfo), &(smsCtx->cpmsInfo),
                             sizeof(smsCtx->cpmsInfo));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(outsideCtx->atCpmsInfo), sizeof(smsCtx->cpmsInfo));
    }

    AT_MntnTraceEvent((struct MsgCB *)sndMsgCB);
    /*lint -save -e516 */
    PS_FREE_MSG(WUEPS_PID_AT, sndMsgCB);
    /*lint -restore */

    return;
}


VOS_VOID AT_MntnTraceClientData(VOS_VOID)
{
    NAS_AT_SdtAtClientTable *sndMsgCB = VOS_NULL_PTR;
    TAF_UINT8                loop;
    /*lint -save -e516 */
    sndMsgCB = (NAS_AT_SdtAtClientTable *)PS_ALLOC_MSG(WUEPS_PID_AT,
                                                       sizeof(NAS_AT_SdtAtClientTable) - VOS_MSG_HEAD_LENGTH);
    /*lint -restore */
    if (sndMsgCB == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_MNTN_TraceClientData:ERROR: Alloc Memory Fail.");
        return;
    }

    TAF_CfgMsgHdr((MsgBlock *)sndMsgCB, WUEPS_PID_AT, WUEPS_PID_AT,
                  sizeof(NAS_AT_SdtAtClientTable) - VOS_MSG_HEAD_LENGTH);

    sndMsgCB->msgID       = ID_AT_MNTN_PC_REPLAY_CLIENT_TAB;
    sndMsgCB->type        = AT_PC_REPLAY_MSG_CLIENT_TAB;
    sndMsgCB->reserved[0] = 0;
    sndMsgCB->reserved[1] = 0;
    sndMsgCB->reserved[2] = 0;

    for (loop = 0; loop < AT_MAX_CLIENT_NUM; loop++) {
        sndMsgCB->atClientTab[loop].clientId = g_atClientTab[loop].clientId;
        sndMsgCB->atClientTab[loop].opId     = g_atClientTab[loop].opId;
        sndMsgCB->atClientTab[loop].used     = g_atClientTab[loop].used;
        sndMsgCB->atClientTab[loop].userType = g_atClientTab[loop].userType;
        sndMsgCB->atClientTab[loop].mode     = g_atClientTab[loop].mode;
        sndMsgCB->atClientTab[loop].indMode  = g_atClientTab[loop].indMode;
    }

    AT_MntnTraceEvent((struct MsgCB *)sndMsgCB);
    /*lint -save -e516 */
    PS_FREE_MSG(WUEPS_PID_AT, sndMsgCB);
    /*lint -restore */
    return;
}


VOS_VOID AT_MntnTraceRPTPORT(VOS_VOID)
{
    AT_MntnRptport *sndMsgCB = VOS_NULL_PTR;
    VOS_UINT8       loop;
    /*lint -save -e516 */
    sndMsgCB = (AT_MntnRptport *)PS_ALLOC_MSG(WUEPS_PID_AT, sizeof(AT_MntnRptport) - VOS_MSG_HEAD_LENGTH);
    /*lint -restore */
    if (sndMsgCB == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_MNTN_TraceRPTPORT:ERROR: Alloc Memory Fail.");
        return;
    }

    TAF_CfgMsgHdr((MsgBlock *)sndMsgCB, WUEPS_PID_AT, WUEPS_PID_AT, sizeof(AT_MntnRptport) - VOS_MSG_HEAD_LENGTH);

    sndMsgCB->msgId = ID_AT_MNTN_RPT_PORT;

    for (loop = 0; loop < AT_MAX_CLIENT_NUM; loop++) {
        sndMsgCB->atRptPort[loop].atClientTabIndex = loop;
        sndMsgCB->atRptPort[loop].modemId          = g_atClientCtx[loop].clientConfiguration.modemId;
        sndMsgCB->atRptPort[loop].reportFlg        = g_atClientCtx[loop].clientConfiguration.reportFlg;
    }

    AT_MntnTraceEvent((struct MsgCB *)sndMsgCB);
    /*lint -save -e516 */
    PS_FREE_MSG(WUEPS_PID_AT, sndMsgCB);
    /*lint -restore */
    return;
}


VOS_VOID AT_RecordAtMsgInfo(VOS_UINT32 sendPid, VOS_UINT32 msgName, VOS_UINT32 sliceStart, VOS_UINT32 sliceEnd)
{
    VOS_UINT32 indexNum;

    if (g_atMsgRecordInfo.currentIndex >= AT_MNTN_MSG_RECORD_MAX_NUM) {
        g_atMsgRecordInfo.currentIndex = 0;
    }

    indexNum = g_atMsgRecordInfo.currentIndex;

    g_atMsgRecordInfo.atMntnMsgRecord[indexNum].sendPid    = sendPid;
    g_atMsgRecordInfo.atMntnMsgRecord[indexNum].msgName    = msgName;
    g_atMsgRecordInfo.atMntnMsgRecord[indexNum].sliceStart = sliceStart;
    g_atMsgRecordInfo.atMntnMsgRecord[indexNum].sliceEnd   = sliceEnd;

    g_atMsgRecordInfo.currentIndex++;

    return;
}


VOS_UINT16 AT_MntnTransferClientIdToMntnIndex(VOS_UINT16 clientId)
{
    VOS_UINT16 mntnIndex = AT_CLIENT_ID_BUTT;

    /* 当前仅对pcui ctrl口以及appvcom-appvcom34共37个端口抓取维测 */
    if (clientId <= AT_CLIENT_ID_CTRL) {
        mntnIndex = clientId;
    }

    if ((clientId >= AT_CLIENT_ID_APP) && (clientId < AT_MNTN_MAX_PORT_CLIENT_ID)) {
        /* mntn的端口是从pcui、ctrl、appvcom0、appvcom1...开始的,+2是为了加上pcui、ctrl的索引 */
        mntnIndex = clientId - AT_CLIENT_ID_APP + 2;
    }

    return mntnIndex;
}


VOS_VOID AT_MntnIncRcvStreamCnt(VOS_UINT16 indexNum)
{
    VOS_UINT16 mntnIndex;

    mntnIndex = AT_MntnTransferClientIdToMntnIndex(indexNum);

    if (mntnIndex == AT_CLIENT_ID_BUTT) {
        return;
    }

    g_atMntnPortInfo[mntnIndex].rcvStreamCnt++;
}


VOS_VOID AT_MntnIncSndStreamCnt(VOS_UINT16 indexNum)
{
    VOS_UINT16 mntnIndex;

    mntnIndex = AT_MntnTransferClientIdToMntnIndex(indexNum);

    if (mntnIndex == AT_CLIENT_ID_BUTT) {
        return;
    }

    g_atMntnPortInfo[mntnIndex].sndStreamCnt++;
}


VOS_VOID AT_MntnSaveCurCmdName(VOS_UINT16 indexNum)
{
    errno_t    memResult;
    VOS_UINT16 mntnIndex;

    mntnIndex = AT_MntnTransferClientIdToMntnIndex(indexNum);

    if (mntnIndex == AT_CLIENT_ID_BUTT) {
        return;
    }

    if ((g_atParseCmd.cmdName.cmdNameLen > 0) && (g_atParseCmd.cmdName.cmdNameLen <= AT_CMD_NAME_LEN)) {
        /* 最后保留一个字符串结束符 0 */
        memResult = memcpy_s(g_atMntnPortInfo[mntnIndex].cmdName, sizeof(g_atMntnPortInfo[mntnIndex].cmdName) - 1,
                             g_atParseCmd.cmdName.cmdName, sizeof(g_atMntnPortInfo[mntnIndex].cmdName) - 1);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(g_atMntnPortInfo[mntnIndex].cmdName) - 1,
                            sizeof(g_atMntnPortInfo[mntnIndex].cmdName) - 1);
    }
}


VOS_VOID AT_MntnSaveExcLog(VOS_VOID)
{
    AT_MntnSaveExcLogInfo *excLogBuffer = VOS_NULL_PTR;
    VOS_UINT32             atClientId;
    VOS_UINT32             i;
    errno_t                memResult;

    /* 申请失败，直接退出 */
    if (g_atExcAddr == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_MNTN_SaveExcLog:No memory allocated");
        return;
    }

    memResult = memset_s(g_atExcAddr, AT_MNTN_SAVE_EXC_LOG_LENGTH, 0, AT_MNTN_SAVE_EXC_LOG_LENGTH);
    TAF_MEM_CHK_RTN_VAL(memResult, AT_MNTN_SAVE_EXC_LOG_LENGTH, AT_MNTN_SAVE_EXC_LOG_LENGTH);

    excLogBuffer           = (AT_MntnSaveExcLogInfo *)g_atExcAddr;
    excLogBuffer->beginTag = AT_MNTN_DUMP_BEGIN_TAG;
    excLogBuffer->endTag   = AT_MNTN_DUMP_END_TAG;

    memResult = memcpy_s(&(excLogBuffer->msgInfo), sizeof(excLogBuffer->msgInfo), &g_atMsgRecordInfo,
                         sizeof(excLogBuffer->msgInfo));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(excLogBuffer->msgInfo), sizeof(excLogBuffer->msgInfo));

    memResult = memcpy_s(&excLogBuffer->portInfo[0], sizeof(excLogBuffer->portInfo), g_atMntnPortInfo,
                         sizeof(g_atMntnPortInfo));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(excLogBuffer->portInfo), sizeof(g_atMntnPortInfo));

    atClientId = AT_CLIENT_ID_PCUI;

    for (i = 0; i < AT_MNTN_MAX_PORT_NUM; i++) {
        if (i == 0) {
            atClientId = AT_CLIENT_ID_PCUI;
        }

        /* 仅统计pcui、ctrl以及appvcom0到appvcom34端口，所以当index为2时，需要把clientid置位app */
        if (i == 2) {
            atClientId = AT_CLIENT_ID_APP;
        }

        excLogBuffer->portInfo[i].clientStatus = g_parseContext[atClientId].clientStatus;
        atClientId++;
    }
}


VOS_VOID AT_RegisterDumpCallBack(VOS_VOID)
{
    /* 调用底软接口进行申请内存 */
    /* 分配内存 */
    g_atExcAddr = (VOS_UINT8 *)mdrv_om_register_field(OM_AP_AT, "AT dump", VOS_NULL_PTR, VOS_NULL_PTR,
                                                      AT_MNTN_SAVE_EXC_LOG_LENGTH, 0);

    /* 申请失败打印异常信息，内存申请异常时仍然注册，回调函数中有地址申请失败保护 */
    if (g_atExcAddr == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_RegisterDumpCallBack:No memory allocated");
    }

    (VOS_VOID)mdrv_om_register_callback("AT_EXCLOG", (dump_hook)AT_MntnSaveExcLog);
}

#if (FEATURE_AT_HSUART == FEATURE_ON)

VOS_VOID AT_ShowHsUartConfigInfo(VOS_VOID)
{
    AT_UartLineCtrl    *lineCtrl                                      = VOS_NULL_PTR;
    AT_UartFlowCtrl    *flowCtrl                                      = VOS_NULL_PTR;
    AT_UartRiCfg       *uartRiCfgInfo                                 = VOS_NULL_PTR;
    AT_UartPhyCfg      *uartPhyCfgInfo                                = VOS_NULL_PTR;
    AT_UartFormatParam *formatParam                                   = VOS_NULL_PTR;
    VOS_CHAR            acParityStr[][AT_HSUART_PARITY_MAX_LEN]       = { "ODD", "EVEN", "MARK", "SPACE", "NONE" };
    VOS_CHAR            acDcdStr[][AT_HSUART_DCD_MAX_LEN]             = { "ALWAYS ON", "CONNECT ON" };
    VOS_CHAR            acDtrStr[][AT_HSUART_DTR_MAX_LEN]             = { "IGNORE", "SWITCH CMD MODE", "HANGUP CALL" };
    VOS_CHAR            acDsrStr[][AT_HSUART_DSR_MAX_LEN]             = { "ALWAYS ON", "CONNECT ON" };
    VOS_CHAR            acDceByDteStr[][AT_HSUART_DCE_BY_DTE_MAX_LEN] =
                            { "NONE", "XON OR XOFF REMOVE", "RTS", "XON_OR_XOFF_PASS" };
    VOS_CHAR            acDteByDceStr[][AT_HSUART_DTE_BY_DCE_MAX_LEN] = { "NONE", "XON OR XOFF REMOVE", "CTS" };

    lineCtrl       = AT_GetUartLineCtrlInfo();
    flowCtrl       = AT_GetUartFlowCtrlInfo();
    uartRiCfgInfo  = AT_GetUartRiCfgInfo();
    uartPhyCfgInfo = AT_GetUartPhyCfgInfo();
    formatParam    = AT_HSUART_GetFormatParam(uartPhyCfgInfo->frame.format);

    PS_PRINTF_INFO("[1] HSUART PHY                      \n");
    PS_PRINTF_INFO("BaudRate:                   %d\n", uartPhyCfgInfo->baudRate);
    PS_PRINTF_INFO("Format:                     %d\n", uartPhyCfgInfo->frame.format);
    PS_PRINTF_INFO("->Data Bit Length:          %d\n", formatParam->dataBitLength);
    PS_PRINTF_INFO("->Stop Bit Length:          %d\n", formatParam->stopBitLength);
    PS_PRINTF_INFO("->Parity Bit Length:        %d\n", formatParam->parityBitLength);
    PS_PRINTF_INFO("Parity Type:                %s\n", acParityStr[uartPhyCfgInfo->frame.parity]);

    PS_PRINTF_INFO("[2] HSUART LINE                     \n");
    PS_PRINTF_INFO("DCD[%d]:                    %s\n", lineCtrl->dcdMode, acDcdStr[lineCtrl->dcdMode]);
    PS_PRINTF_INFO("DTR[%d]:                    %s\n", lineCtrl->dtrMode, acDtrStr[lineCtrl->dtrMode]);
    PS_PRINTF_INFO("DSR[%d]:                    %s\n", lineCtrl->dsrMode, acDsrStr[lineCtrl->dsrMode]);

    PS_PRINTF_INFO("[3] HSUART FLOW CTRL                \n");
    PS_PRINTF_INFO("DCE BY DTE[%d]:             %s\n", flowCtrl->dceByDte, acDceByDteStr[flowCtrl->dceByDte]);
    PS_PRINTF_INFO("DTE BY DCE[%d]:             %s\n", flowCtrl->dteByDce, acDteByDceStr[flowCtrl->dteByDce]);

    PS_PRINTF_INFO("[4] HSUART RI                       \n");
    PS_PRINTF_INFO("SMS RI ON (ms):             %u\n", uartRiCfgInfo->smsRiOnInterval);
    PS_PRINTF_INFO("SMS RI OFF (ms):            %u\n", uartRiCfgInfo->smsRiOffInterval);
    PS_PRINTF_INFO("VOICE RI ON (ms):           %u\n", uartRiCfgInfo->voiceRiOnInterval);
    PS_PRINTF_INFO("VOICE RI OFF (ms):          %u\n", uartRiCfgInfo->voiceRiOffInterval);
    PS_PRINTF_INFO("VOICE RI Cycle Times:       %d\n", uartRiCfgInfo->voiceRiCycleTimes);

    return;
}


VOS_VOID AT_ShowHsUartFcState(VOS_VOID)
{
    VOS_CHAR        acFcStateStr[][AT_HSUART_FC_STATE_MAX_LEN] = { "START", "STOP" };
    AT_IoLevelUint8 ioLevel;

    ioLevel = AT_GetIoLevel(AT_CLIENT_ID_HSUART, IO_CTRL_CTS);

    PS_PRINTF_INFO("HSUART FLOW CTRL STATE              \n");
    PS_PRINTF_INFO("FLOW CTRL STATE:            %s\n", acFcStateStr[ioLevel]);

    return;
}
#endif


VOS_VOID AT_ShowPsFcIdState(VOS_UINT32 fcid)
{
    if (fcid >= FC_ID_BUTT) {
        PS_PRINTF_WARNING("<AT_ShowFcStatsInfo> Fcid overtop, ulFcid = %d\n", fcid);
        return;
    }

    PS_PRINTF_INFO("Indicate the validity of the corresponding node of FCID,      Used = %d\n",
                   g_fcIdMaptoFcPri[fcid].used);
    PS_PRINTF_INFO("Indicate the priority of the corresponding node of FCID,      FcPri = %d\n",
                   g_fcIdMaptoFcPri[fcid].fcPri);
    PS_PRINTF_INFO("Indicate the RABID mask of the corresponding node of FCID,    RabIdMask = %d\n",
                   g_fcIdMaptoFcPri[fcid].rabIdMask);
    PS_PRINTF_INFO("Indicate the ModemId of the corresponding node of FCID,       ModemId = %d\n",
                   g_fcIdMaptoFcPri[fcid].modemId);

    return;
}



VOS_VOID AT_ShowAllClientState(VOS_VOID)
{
    VOS_UINT8 i;
    VOS_CHAR  acStateStr[][AT_CLIENT_STATE_MAX_LEN] = { "READY", "PEND" };
    VOS_CHAR  acModeStr[][AT_CLIENT_MODE_MAX_LEN]  = { "CMD", "DATA", "ONLINE_CMD" };

    PS_PRINTF_INFO("The All Client State: \n");

    for (i = 0; i < AT_MAX_CLIENT_NUM; i++) {
        PS_PRINTF_INFO("Client[%d] State: %s    Mode: %s\n", i, acStateStr[g_parseContext[i].clientStatus],
                       acModeStr[g_atClientTab[i].mode]);
    }

    return;
}


VOS_VOID AT_SetClientState(VOS_UINT8 indexNum, VOS_UINT8 state)
{
    if ((indexNum >= AT_MAX_CLIENT_NUM) || (state > AT_FW_CLIENT_STATUS_PEND)) {
        return;
    }

    g_parseContext[indexNum].clientStatus = state;

    return;
}


VOS_VOID AT_SetClientMode(VOS_UINT8 indexNum, VOS_UINT8 mode)
{
    if ((indexNum >= AT_MAX_CLIENT_NUM) || (mode > AT_ONLINE_CMD_MODE)) {
        return;
    }

    g_atClientTab[indexNum].mode = mode;

    return;
}


VOS_VOID AT_ShowUsedClient(VOS_VOID)
{
    AT_PortBuffCfgInfo *portCfg = VOS_NULL_PTR;
    VOS_UINT32          indexNum;
    VOS_UINT8           i;
    VOS_CHAR            acStateStr[][AT_CLIENT_STATE_MAX_LEN] = { "READY", "PEND" };
    VOS_CHAR            acModeStr[][AT_CLIENT_STATE_MAX_LEN]  = { "CMD", "DATA", "ONLINE_CMD" };

    portCfg = AT_GetPortBuffCfgInfo();

    PS_PRINTF_INFO("The All Used Client State: \n");
    for (i = 0; i < portCfg->num; i++) {
        indexNum = portCfg->usedClientId[i];
        PS_PRINTF_INFO("Client[%d] State: %s    Mode: %s\n", indexNum,
                       acStateStr[g_parseContext[indexNum].clientStatus], acModeStr[g_atClientTab[indexNum].mode]);
    }

    return;
}


VOS_VOID AT_ShowClientCtxInfo(VOS_VOID)
{
    VOS_UINT8               i;
    AT_ClientCfgMapTab     *cfgMapTbl = VOS_NULL_PTR;
    AT_ClientConfiguration *clientCfg = VOS_NULL_PTR;

    PS_PRINTF_INFO("The All Used Client Config: \n");
    for (i = 0; i < AT_GET_CLIENT_CFG_TAB_LEN(); i++) {
        cfgMapTbl = AT_GetClientCfgMapTbl(i);
        clientCfg = AT_GetClientConfig(cfgMapTbl->clientId);
        PS_PRINTF_INFO("Client[%s] modem:%d, reportFlag:%d\n", cfgMapTbl->portName, clientCfg->modemId,
                       clientCfg->reportFlg);
    }
}


VOS_VOID AT_Help(VOS_VOID)
{
    PS_PRINTF_INFO("PS Debug Info               \n");
    PS_PRINTF_INFO("<AT_ShowPsEntityInfo>         Show PS Call Entity Info           \n");
    PS_PRINTF_INFO("<AT_ShowPsFcIdState(ulFcid)>  Show Flow Control Node Info        \n");
    PS_PRINTF_INFO("UART Debug Info             \n");
    PS_PRINTF_INFO("<AT_ShowHsUartConfigInfo>     Show HSUART Config Info            \n");
    PS_PRINTF_INFO("<AT_ShowHsUartNvStats>        Show HSUART NV Stats              \n");
    PS_PRINTF_INFO("<AT_ShowHsUartIoctlStats>     Show HSUART IOCTL Stats           \n");
    PS_PRINTF_INFO("<AT_ShowHsUartDataStats>      Show HAUART Data Stats            \n");
    PS_PRINTF_INFO("<AT_ShowHsUartFcState>        Show HAUART Flow Control State     \n");
    PS_PRINTF_INFO("MODEM Debug Info            \n");
    PS_PRINTF_INFO("<AT_ShowModemDataStats>       Show Modem Data Stats             \n");
    PS_PRINTF_INFO("Others Debug Info           \n");
    PS_PRINTF_INFO("<AT_ShowAllClientState>       Show All Client Port State         \n");
    PS_PRINTF_INFO("<AT_ShowClientCtxInfo>        Show Client Configure Context Info \n");
    PS_PRINTF_INFO("<AT_ShowIPv6IIDMgrInfo>       Show IPv6 Interface ID Manage Info \n");

    return;
}

