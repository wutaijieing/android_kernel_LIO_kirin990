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

#include "ppp_atcmd.h"
#include "pppc_pppa_interface.h"
#include "ppp_input.h"
#include "product_config.h"
#include "ppp_public.h"
#include "nv_id_gucnas.h"
#include "nv_stru_gucnas.h"
#include "acore_nv_id_gucttf.h"
#include "acore_nv_stru_gucttf.h"
#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
#include "ads_dev_i.h"
#else
#include "ads_device_interface.h"
#endif


#define THIS_FILE_ID PS_FILE_ID_PPP_ATCMD_C

#define PPPA_DEF_MRU 1500
#define PPPA_MIN_MRU 296
#define PPPA_DEF_ECHO_LOST_CNT 0xFFFF

typedef struct {
    VOS_UINT16 mru;
    VOS_UINT16 echoMaxLostCnt;
    VOS_UINT32 winsEnable : 1;
    VOS_UINT32 chapEnable : 1;
    VOS_UINT32 papEnable : 1;
    VOS_UINT32 resv : 29;
} PPPA_ConfigInfo;

enum PPP_MsgType {
    AT_PPP_CREATE_RAW_PPP_REQ = 0x01, /* PPP模块收到AT的CREATE RAW PPP指示 */
    AT_PPP_RELEASE_RAW_PPP_REQ        /* PPP模块收到AT的RELEASE RAW PPP指示 */
};
typedef VOS_UINT32 PPP_MsgTypeUint32;

__STATIC_ PPPA_ConfigInfo g_pppaCfg = {0};

VOS_VOID PPP_UpdateWinsConfig(VOS_UINT8 wins)
{
    g_pppaCfg.winsEnable = (wins == WINS_CONFIG_DISABLE) ? VOS_FALSE : VOS_TRUE;
}

__STATIC_ VOS_VOID PPPA_SendPppcCreateInd(VOS_VOID)
{
    PPPA_PPPC_CreatePpp *createPppInd = VOS_NULL_PTR;

    createPppInd = (PPPA_PPPC_CreatePpp*)PS_ALLOC_MSG_WITH_HEADER_LEN(PS_PID_APP_PPP, sizeof(PPPA_PPPC_CreatePpp));
    if (createPppInd == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "alloc Msg Fail");
        return;
    }
    TTF_SET_MSG_RECEIVER_ID(createPppInd, MSPS_PID_PPPC);
    createPppInd->msgId                 = ID_PPPA_PPPC_CREATE_PPP_IND;
    createPppInd->config.mru            = g_pppaCfg.mru;
    createPppInd->config.chapEnable     = g_pppaCfg.chapEnable;
    createPppInd->config.papEnable      = g_pppaCfg.papEnable;
    createPppInd->config.winsEnable     = g_pppaCfg.winsEnable;
    createPppInd->config.echoMaxLostCnt = g_pppaCfg.echoMaxLostCnt;

    (VOS_VOID)PS_SEND_MSG(PS_PID_APP_PPP, createPppInd);
}

VOS_VOID PPPA_InitCfgInfo(VOS_VOID)
{
    PPP_ConfigMruTypeNv mruNv  = {0};
    WINS_Config         winsNv = {0};
    NV_TtfPppConfig     cfgNv  = {0};

    g_pppaCfg.mru = PPPA_DEF_MRU;
    if ((GUCTTF_NV_Read(MODEM_ID_0, NV_ITEM_PPP_CONFIG_MRU_TYPE, &mruNv, sizeof(mruNv)) == NV_OK) &&
        (mruNv.pppConfigType >= PPPA_MIN_MRU) && (mruNv.pppConfigType <= PPPA_DEF_MRU)) {
        g_pppaCfg.mru = mruNv.pppConfigType;
    }

    g_pppaCfg.winsEnable = VOS_TRUE;
    if ((GUCTTF_NV_Read(MODEM_ID_0, NV_ITEM_WINS_CONFIG, &winsNv, sizeof(winsNv)) == NV_OK) && (winsNv.ucStatus != 0) &&
        (winsNv.ucWins == WINS_CONFIG_DISABLE)) {
        g_pppaCfg.winsEnable = VOS_FALSE;
    }

    g_pppaCfg.echoMaxLostCnt = PPPA_DEF_ECHO_LOST_CNT;
    g_pppaCfg.papEnable      = VOS_TRUE;
    if (GUCTTF_NV_Read(MODEM_ID_0, NV_ITEM_PPP_CONFIG, &cfgNv, sizeof(cfgNv)) == NV_OK) {
        g_pppaCfg.chapEnable     = cfgNv.chapEnable;
        g_pppaCfg.papEnable      = cfgNv.papEnable;
        g_pppaCfg.echoMaxLostCnt = cfgNv.lcpEchoMaxLostCnt;
    }
}

VOS_UINT32 PPPA_CreatePppReq(PPP_Id *pppId)
{
    if (PPPA_GetUsedFalg() == VOS_TRUE) {
        return VOS_ERR;
    }

    PPPA_SetUsedFlag(VOS_TRUE);
    *pppId = PPPA_PPP_ID;
    PPPA_SendPppcCreateInd();

    PPP_SetupHdlc(PPPA_PPP_ID, VOS_TRUE);

    return VOS_OK;
}

__STATIC_ VOS_VOID PPPA_EventMntnInfo(VOS_UINT16 pppId, VOS_UINT32 event)
{
    PPP_EventMntnInfo traceMsg = {0};

    TTF_SET_MSG_SENDER_ID(&traceMsg, PS_PID_APP_PPP);
    TTF_SET_MSG_RECEIVER_ID(&traceMsg, PS_PID_APP_PPP);
    TTF_SET_MSG_LEN(&traceMsg, sizeof(traceMsg) - VOS_MSG_HEAD_LENGTH);

    traceMsg.msgname = event;
    traceMsg.pppId   = pppId;

    PPP_MNTN_TRACE_MSG(&traceMsg);
}

VOS_UINT32 PPPA_CreateRawDataPppReq(PPP_Id *pppId)
{
    if (PPPA_GetUsedFalg() == VOS_TRUE) {
        return VOS_ERR;
    }

    PPPA_SetUsedFlag(VOS_TRUE);
    *pppId = PPPA_PPP_ID;
    PPP_SetupHdlc(PPPA_PPP_ID, VOS_FALSE);
    PPPA_EventMntnInfo(PPPA_PPP_ID, AT_PPP_CREATE_RAW_PPP_REQ);

    return VOS_OK;
}

__STATIC_ VOS_UINT32 PPPA_ReleasePppReq(PPP_Id pppId)
{
    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        return VOS_ERR;
    }

    PPPA_SetUsedFlag(VOS_FALSE);
    PPP_ClearDataQ();
    PPPA_SendPppcCommMsg(ID_PPPA_PPPC_RELEASE_PPP_IND);

    return VOS_OK;
}

__STATIC_ VOS_UINT32 PPPA_ReleaseRawDataPppReq(PPP_Id pppId)
{
    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        return VOS_ERR;
    }

    PPPA_SetUsedFlag(VOS_FALSE);
    PPP_ReleaseHdlc(PPPA_PPP_ID);
    PPPA_EventMntnInfo(pppId, AT_PPP_RELEASE_RAW_PPP_REQ);

    return VOS_OK;
}

__STATIC_ VOS_VOID PPPA_SendPppcPdpActRsp(PPPA_PdpActiveResult *pdpActRslt)
{
    PPPA_PPPC_PdpActiveRsp *pdpActRsp = VOS_NULL_PTR;

    pdpActRsp = (PPPA_PPPC_PdpActiveRsp*)PS_ALLOC_MSG_WITH_HEADER_LEN(PS_PID_APP_PPP, sizeof(PPPA_PPPC_PdpActiveRsp));
    if (pdpActRsp == VOS_NULL_PTR) {
        PPP_MNTN_LOG(PS_PRINT_WARNING, "alloc Msg Fail");
        return;
    }
    TTF_SET_MSG_RECEIVER_ID(pdpActRsp, MSPS_PID_PPPC);
    pdpActRsp->msgId  = ID_PPPA_PPPC_PDP_ACTIVE_RSP;
    pdpActRsp->result = *pdpActRslt;

    (VOS_VOID)PS_SEND_MSG(PS_PID_APP_PPP, pdpActRsp);
}

#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
VOS_UINT32 PPPA_RcvConfigInfoInd(PPP_Id pppId, PPPA_PdpActiveResult *atPppIndConfigInfo)
{
    VOS_UINT8                     ifaceId = 0;
    VOS_INT32                     result;
    struct ads_iface_rx_handler_s rxHandler = {0};

    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "PPP not ues", pppId);
        return VOS_ERR;
    }

    /* 通过usPppId，寻找到ucIfaceId */
    if (At_PppId2IfaceId(pppId, &ifaceId) != VOS_OK) {
        PPP_MNTN_LOG2(PS_PRINT_NORMAL, "Can not get Iface Id ", pppId, ifaceId);
        return VOS_ERR;
    }

    /* 这个时候PDP已经激活，注册上行数据接收接口 */
    rxHandler.user_data     = pppId;
    rxHandler.rx_func       = (ads_iface_rx_func_t)PPP_PushPacketEvent;
    rxHandler.rx_cmplt_func = VOS_NULL_PTR;
    result                  = ads_iface_register_rx_handler(ifaceId, &rxHandler);
    if (result != VOS_OK) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "Register DL CB failed!", ifaceId);
        return VOS_ERR;
    }

    PPPA_SendPppcPdpActRsp(atPppIndConfigInfo);

    return VOS_OK;
}

VOS_UINT32 PPP_RegDlDataCallback(PPP_Id pppId)
{
    VOS_UINT8                     ifaceId = 0;
    VOS_INT32                     result;
    struct ads_iface_rx_handler_s rxHandler = {0};

    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "PPP not ues", pppId);
        return VOS_ERR;
    }

    /* 通过usPppId，寻找到ucIfaceId */
    if (At_PppId2IfaceId(pppId, &ifaceId) != VOS_OK) {
        PPP_MNTN_LOG2(PS_PRINT_NORMAL, "Can not get Iface Id ", pppId, ifaceId);
        return VOS_ERR;
    }

    /* 这个时候PDP已经激活，注册上行数据接收接口 */
    rxHandler.user_data     = pppId;
    rxHandler.rx_func       = (ads_iface_rx_func_t)PPP_PushRawDataEvent;
    rxHandler.rx_cmplt_func = VOS_NULL_PTR;
    result                  = ads_iface_register_rx_handler(ifaceId, &rxHandler);

    if (result != VOS_OK) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "Register DL CB failed", ifaceId);
        return VOS_ERR;
    }

    return VOS_OK;
}

#else
VOS_UINT32 PPPA_RcvConfigInfoInd(PPP_Id pppId, PPPA_PdpActiveResult *atPppIndConfigInfo)
{
    VOS_UINT8  rabId = 0;
    VOS_UINT32 result;

    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "PPP not ues", pppId);
        return VOS_ERR;
    }

    /* 通过usPppId，寻找到usRabId */
    if (!PPP_PPPID_TO_RAB(pppId, &rabId)) {
        PPP_MNTN_LOG2(PS_PRINT_NORMAL, "Can not get PPP Id", pppId, rabId);
        return VOS_ERR;
    }

    /* 这个时候PDP已经激活，注册上行数据接收接口。另外当前不支持PPP类型拨号。 */
    result = ADS_DL_RegDlDataCallback(rabId, (RCV_DL_DATA_FUNC)PPP_PushPacketEvent, 0);
    if (result != VOS_OK) {
        PPP_MNTN_LOG1(PS_PRINT_NORMAL, "Register DL CB failed", rabId);

        return VOS_ERR;
    }

    PPPA_SendPppcPdpActRsp(atPppIndConfigInfo);

    return VOS_OK;
}

VOS_UINT32 PPP_RegDlDataCallback(PPP_Id pppId)
{
    VOS_UINT8  rabId = 0;
    VOS_UINT32 result;

    if (PPPA_GetUsedFalg() != VOS_TRUE) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "PPP not ues", pppId);
        return VOS_ERR;
    }

    /* 通过usPppId，寻找到usRabId */
    if (!PPP_PPPID_TO_RAB(pppId, &rabId)) {
        PPP_MNTN_LOG2(PS_PRINT_NORMAL, "Can not get PPP Id", pppId, rabId);

        return VOS_ERR;
    }

    /* 这个时候PDP已经激活，注册上行数据接收接口 */
    result = ADS_DL_RegDlDataCallback(rabId, (RCV_DL_DATA_FUNC)PPP_PushRawDataEvent, 0);
    if (result != VOS_OK) {
        PPP_MNTN_LOG1(PS_PRINT_WARNING, "Register DL CB failed", rabId);
        return VOS_ERR;
    }

    return VOS_OK;
}

#endif

VOS_UINT32 PPP_RcvAtCtrlOperEvent(VOS_UINT16 pppId, PPP_AtCtrlOperTypeUint32 operType)
{
    PPP_AT_CtrlOperCmd *ctrlOper = VOS_NULL_PTR;

    if (operType >= PPP_AT_CTRL_BUTT) {
        PPP_MNTN_LOG2(LOG_LEVEL_WARNING, "invalid para!", pppId, operType);
        return VOS_ERR;
    }

    ctrlOper = (PPP_AT_CtrlOperCmd*)PS_ALLOC_MSG_WITH_HEADER_LEN(PS_PID_APP_PPP, sizeof(PPP_AT_CtrlOperCmd));
    if (ctrlOper == VOS_NULL_PTR) {
        PPP_MNTN_LOG2(LOG_LEVEL_WARNING, "alloc msg fail!", pppId, operType);
        return VOS_ERR;
    }

    TTF_SET_MSG_RECEIVER_ID(ctrlOper, PS_PID_APP_PPP);
    ctrlOper->msgHdr.msgId = ID_PPPA_AT_CTRL_OPERATION_MSG;
    ctrlOper->msgHdr.pppId = pppId;
    ctrlOper->operType     = operType;

    return PS_SEND_MSG(PS_PID_APP_PPP, ctrlOper);
}

VOS_VOID PPPA_AtCtrlOperMsgProc(PPP_InternalMsgHeader *msg)
{
    const PPP_AT_CtrlOperCmd* ctrlOper = (const PPP_AT_CtrlOperCmd*)msg;
    PPP_AtCtrlOperTypeUint32  operType = ctrlOper->operType;
    PPP_Id                    pppId    = ctrlOper->msgHdr.pppId;

    switch (operType) {
        case PPP_AT_CTRL_REL_PPP_REQ:
            PPPA_ReleasePppReq(pppId);
            break;
        case PPP_AT_CTRL_REL_PPP_RAW_REQ:
            PPPA_ReleaseRawDataPppReq(pppId);
            break;
        case PPP_AT_CTRL_HDLC_DISABLE:
            PPP_MNTN_LOG(LOG_LEVEL_WARNING, "Disable Hdlc");
            break;
        default:
            PPP_MNTN_LOG1(LOG_LEVEL_WARNING, "operType is ERROR!", operType);
            break;
    }
}
