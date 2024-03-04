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



#include "si_app_stk.h"
#include "si_stk.h"
#include "product_config.h"

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "si_pih.h"
#endif

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "usimm_ps_interface.h"
#include "usimm_interface.h"
#include "si_ccore_api.h"
#endif

#if (VOS_OS_VER == VOS_WIN32)
#include "ut_mem.h"
#endif


#define THIS_FILE_ID PS_FILE_ID_STK_API_C
#define THIS_MODU mod_pam_stk

/* Envelope 命令回复列表 */
VOS_UINT8 g_callCtrlRsp[] = {
    CAP_CFG_PARA_TAG, SUBADDRESS_TAG, ALPHA_IDENTIFIER_TAG, BC_REPEAT_INDICATOR_TAG, CAP_CFG_PARA_TAG
};

VOS_UINT8 g_moSMSCtrlRsp[] = {ALPHA_IDENTIFIER_TAG};

SI_STK_Taglist g_stkEnvelopeDecodeList[2] = {
    { SI_STK_ENVELOPE_CALLCRTL, sizeof(g_callCtrlRsp), g_callCtrlRsp },
    { SI_STK_ENVELOPE_SMSCRTL, sizeof(g_moSMSCtrlRsp), g_moSMSCtrlRsp }
};

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (defined(DMT)))

VOS_UINT32 SI_STK_GetReceiverPid(MN_CLIENT_ID_T clientId, VOS_UINT32 *receiverPid)
{
#if (MULTI_MODEM_NUMBER > 1)
    ModemIdUint16         modemId;
    SI_PIH_CardSlotUint32  slotId;

    /* 调用接口获取Modem ID */
    if (VOS_OK != AT_GetModemIdFromClient(clientId, &modemId)) {
        return VOS_ERR;
    }

    slotId = SI_GetSlotIdByModemId(modemId);
    if (SI_PIH_CARD_SLOT_BUTT <= slotId) {
        return VOS_ERR;
    }

    if (SI_PIH_CARD_SLOT_0 == slotId) {
        *receiverPid = I0_MAPS_STK_PID;
    } else if (SI_PIH_CARD_SLOT_1 == slotId) {
        *receiverPid = I1_MAPS_STK_PID;
    } else {
        *receiverPid = I2_MAPS_STK_PID;
    }
#else
    *receiverPid = I0_MAPS_STK_PID;
#endif
    return VOS_OK;
}


VOS_UINT32 SI_STK_SendReqMsg(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT32 msgName, VOS_UINT32 cmdType,
                             VOS_UINT32 dataLen, VOS_UINT8 *data)
{
    SI_STK_Req *sTKReq = VOS_NULL_PTR;
    VOS_UINT32       sendPid;
    VOS_UINT32       receiverPid;

    if (VOS_OK != SI_STK_GetReceiverPid(clientId, &receiverPid)) {
        STK_ERROR_LOG("SI_STK_SendReqMsg:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    sendPid = WUEPS_PID_AT;

    sTKReq = (SI_STK_Req *)VOS_AllocMsg(sendPid, sizeof(SI_STK_Req) - VOS_MSG_HEAD_LENGTH + dataLen);
    if (VOS_NULL_PTR == sTKReq) {
        STK_ERROR_LOG("SI_STK_SendReqMsg: VOS_AllocMsg Return Error");
        return VOS_ERR;
    }

    sTKReq->msgName       = msgName;
    sTKReq->ulReceiverPid = receiverPid;
    sTKReq->opId          = opId;
    sTKReq->clientId      = clientId;
    sTKReq->satType       = cmdType;
    sTKReq->datalen       = dataLen;

    if (dataLen != 0) {
        (VOS_VOID)memcpy_s(sTKReq->data, dataLen, data, dataLen);
    }

    if (VOS_OK != VOS_SendMsg(sendPid, sTKReq)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 SI_STK_GetMainMenu(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    return SI_STK_SendReqMsg(clientId, opId, SI_STK_GETMAINMNUE, SI_STK_NOCMDDATA, 0, VOS_NULL_PTR);
}


VOS_UINT32 SI_STK_GetSTKCommand(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_STK_CmdTypeUint32 cmdType)
{
    return SI_STK_SendReqMsg(clientId, opId, SI_STK_GETCOMMAND, cmdType, 0, VOS_NULL_PTR);
}


VOS_UINT32 SI_STK_QuerySTKCommand(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
#if ((1 < MULTI_MODEM_NUMBER) && (!defined(DMT)) && (FEATURE_PHONE_SC == FEATURE_ON))
    return VOS_ERR;
#else
    return SI_STK_SendReqMsg(clientId, opId, SI_STK_QUERYCOMMAND, SI_STK_NOCMDDATA, 0, VOS_NULL_PTR);
#endif
}


VOS_UINT32 SI_STK_DataSendSimple(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_SendDataTypeUint32 sendType,
                                 VOS_UINT32 dataLen, VOS_UINT8 *data)
{
    return SI_STK_SendReqMsg(clientId, opId, SI_STK_SIMPLEDOWN, sendType, dataLen, data);
}


VOS_UINT32 SI_STK_TerminalResponse(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_STK_TerminalRsp *tRStru)
{
#if ((1 < MULTI_MODEM_NUMBER) && (!defined(DMT)) && (FEATURE_PHONE_SC == FEATURE_ON))
    return VOS_ERR;
#else

    if (tRStru == VOS_NULL_PTR) {
        STK_ERROR_LOG("SI_STK_TerminalResponse: The input parameter is null.");

        return VOS_ERR;
    }

    return SI_STK_SendReqMsg(clientId, opId, SI_STK_TRDOWN, SI_STK_NOCMDDATA, sizeof(SI_STK_TerminalRsp),
                             (VOS_UINT8 *)tRStru);
#endif
}


VOS_UINT32 SI_STKDualIMSIChangeReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
#if ((1 < MULTI_MODEM_NUMBER) && (!defined(DMT)) && (FEATURE_PHONE_SC == FEATURE_ON))
    return TAF_FAILURE;
#else
    SI_STK_Req *sTKReq = VOS_NULL_PTR;

    sTKReq = (SI_STK_Req *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_STK_Req) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == sTKReq) {
        STK_ERROR_LOG("SI_STKDualIMSIChangeReq: VOS_AllocMsg Return Error");
        return VOS_ERR;
    }

    sTKReq->clientId      = clientId;
    sTKReq->opId          = opId;
    sTKReq->msgName       = SI_STK_IMSICHG;
    sTKReq->ulReceiverPid = I0_MAPS_STK_PID;

    if (VOS_OK != VOS_SendMsg(I0_MAPS_STK_PID, sTKReq)) {
        STK_ERROR_LOG("SI_STKDualIMSIChangeReq: VOS_SendMsg Return Error");
        return VOS_ERR;
    }

    return VOS_OK;
#endif
}


VOS_UINT32 SI_STKIsDualImsiSupport(VOS_VOID)
{
#if ((1 < MULTI_MODEM_NUMBER) && (!defined(DMT)) && (FEATURE_PHONE_SC == FEATURE_ON))
    return TAF_FAILURE;
#else
    VOS_UINT16 dualIMSIFlag = 0;

    if (NV_OK != mdrv_nv_read(NV_ITEM_NV_HUAWEI_DOUBLE_IMSI_CFG_I, &dualIMSIFlag, sizeof(VOS_UINT16))) {
        STK_WARNING_LOG("STK_InitGobal: Read NV_ITEM_NV_HUAWEI_DOUBLE_IMSI_CFG_I Fail");
    }

    /* 前后两个自节均为1，Dual IMSI功能才开启，第一个字节为NV激活标志，第二个为使能位 */
    if (STK_NV_ENABLED == dualIMSIFlag) {
        return VOS_TRUE;
    }

    return VOS_FALSE;
#endif
}


VOS_UINT32 SI_STK_MenuSelectionFunc(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_STK_Envelope *eNStru)
{
#if ((1 < MULTI_MODEM_NUMBER) && (!defined(DMT)) && (FEATURE_PHONE_SC == FEATURE_ON))
    return TAF_FAILURE;
#else
    if ((VOS_NULL_PTR == eNStru) || (SI_STK_ENVELOPE_MENUSEL != eNStru->envelopeType)) {
        STK_ERROR_LOG("SI_STK_MenuSelectionFunc: The Input Data is Error");

        return VOS_ERR;
    }

    return SI_STK_SendReqMsg(clientId, opId, SI_STK_MENUSELECTION, eNStru->envelopeType,
                             sizeof(SI_STK_Envelope), (SI_UINT8 *)eNStru);
#endif
}


VOS_UINT32 SI_STK_SetUpCallConfirm(MN_CLIENT_ID_T clientId, SI_STK_SetupCallConfirmUint32 action)
{
    /* 参数检查 */
    if (SI_STK_SETUPCALL_BUTT <= action) {
        STK_ERROR_LOG("SI_STK_SetUpCallConfirm: The Input Para is Error");

        return VOS_ERR;
    }

    return SI_STK_SendReqMsg(clientId, 0, SI_STK_SETUPCALL_CONFIRM, SI_STK_SETUPCALL, sizeof(VOS_UINT32),
                             (VOS_UINT8 *)&action);
}
#endif

#if ((OSA_CPU_CCPU == VOS_OSA_CPU) || (defined(DMT)))

MODULE_EXPORTED VOS_VOID SI_STKGetCurImsiSign(ModemIdUint16 modemId, VOS_UINT16 *dualIMSIEnable,
                                              VOS_UINT32 *curImsiSign)
{
    VOS_UINT32                   i;
    VOS_UINT32                   j;
    SI_SAT_SetUpMenuData *setUpMenuData = VOS_NULL_PTR;
    SI_STK_ImsichgMatch    iMSIMatch      = {0};
    SI_PIH_CardSlotUint32  slotId;

    if (modemId >= MODEM_ID_BUTT) {
        return;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    *curImsiSign = SI_STK_GetIMSIChgCtrlAddr(slotId)->curImsiSign;

    *dualIMSIEnable = VOS_FALSE;

    setUpMenuData = SI_STK_GetSetUpMenuDataAddr(slotId);
    /* 判断是否存在主菜单 */
    if (SI_STK_MENU_EXIST != setUpMenuData->usatTag) {
        STK_WARNING_LOG("SI_STKGetCurImsiSign: There is any Main Menu Content");

        return;
    }

    if (NV_OK !=
        SI_NVReadexBySlotId(slotId, NV_ITEM_STK_DUAL_IMSI_CTRL, &iMSIMatch, sizeof(SI_STK_ImsichgMatch))) {
        STK_WARNING_LOG("SI_STKGetCurImsiSign: Read NV_ITEM_STK_DUAL_IMSI_CTRL Fail");

        return;
    }

    if (VOS_FALSE == SI_STK_CheckDualSimCtrlNvDataValid(slotId, &iMSIMatch)) {
        STK_WARNING_LOG("SI_STKGetCurImsiSign: Read NV_ITEM_STK_DUAL_IMSI_CTRL Success, data invalid");

        return;
    }

    /* 在主菜单中匹配，如果匹配失败pusDualIMSIEnable设置为Disable， */
    for (i = 0; i < setUpMenuData->setUpMenu.itemNum; i++) {
        for (j = 0; j < iMSIMatch.matchStrCnt; j++) {
            if (VOS_TRUE ==
                SI_STK_StrStr(setUpMenuData->setUpMenu.item[i].itemText, iMSIMatch.matchStr[j].matchStr,
                              setUpMenuData->setUpMenu.item[i].len, iMSIMatch.matchStr[j].strLen)) {
                *dualIMSIEnable = VOS_TRUE;

                return;
            }
        }
    }

    return;
}


MODULE_EXPORTED VOS_VOID SI_STK_CCResultInd(ModemIdUint16 modemId, SI_STK_EnvelopeRsp *rspData)
{
    /*lint -e813*/
    SI_STK_EventInfo       event;
    SI_PIH_CardSlotUint32  slotId;
    errno_t                      ret;

    if (modemId >= MODEM_ID_BUTT) {
        return;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    (VOS_VOID)memset_s(&event, sizeof(event), 0, sizeof(event));

    event.cbEvent                               = SI_STK_CC_RESULT_IND_EVENT;
    event.errorNo                               = VOS_OK;
    event.cmdDataInfo.cmdStru.ccIndInfo.result = (SI_UINT8)rspData->rsult;

    if (USSD_STRING_TAG == (rspData->resp.callCtrlRsp.specialData.tag & 0x7F)) {
        event.cmdDataInfo.cmdStru.ccIndInfo.type = SI_STK_USSD_CALL_CTRL;
    } else if (SS_STRING_TAG == (rspData->resp.callCtrlRsp.specialData.tag & 0x7F)) {
        event.cmdDataInfo.cmdStru.ccIndInfo.type = SI_STK_SS_CALL_CTRL;
    } else if (ADDRESS_TAG == (rspData->resp.callCtrlRsp.specialData.tag & 0x7F)) {
        event.cmdDataInfo.cmdStru.ccIndInfo.type = SI_STK_MO_CALL_CTRL;
    } else {
        return;
    }

    /* COPY aplhaid 字段 */
    if (VOS_TRUE == rspData->resp.callCtrlRsp.opAlaph) {
        event.cmdDataInfo.cmdStru.ccIndInfo.alphaIdInfo.alphaLen =
            (rspData->resp.callCtrlRsp.alphaId.len > SI_STK_DATA_MAX_LEN)
            ? SI_STK_DATA_MAX_LEN
            : rspData->resp.callCtrlRsp.alphaId.len;

        ret = memcpy_s(event.cmdDataInfo.cmdStru.ccIndInfo.alphaIdInfo.alphaId, SI_STK_DATA_MAX_LEN,
                       rspData->resp.callCtrlRsp.alphaId.alphabet,
                       event.cmdDataInfo.cmdStru.ccIndInfo.alphaIdInfo.alphaLen);

        PAM_PRINT_SECFUN_RESULT(ret);
    }

    if (VOS_TRUE == rspData->resp.callCtrlRsp.opSepcialData) {
        /* 解析类型字段 */
        event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.ctrlDataInfo.dataType =
            rspData->resp.callCtrlRsp.specialData.value[0];

        if (rspData->resp.callCtrlRsp.specialData.len > 1) {
            /* 数据字段copy */
            event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.ctrlDataInfo.dataLen =
                rspData->resp.callCtrlRsp.specialData.len - 1;

            ret = memcpy_s(event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.ctrlDataInfo.data, SI_STK_DATA_MAX_LEN,
                           &rspData->resp.callCtrlRsp.specialData.value[1],
                           rspData->resp.callCtrlRsp.specialData.len - 1);

            PAM_PRINT_SECFUN_RESULT(ret);
        }
    }

    SI_STKCallBack_BroadCast(slotId, &event);
    /*lint +e813*/
    return;
}


MODULE_EXPORTED VOS_VOID SI_STK_SMSCtrlResultInd(ModemIdUint16 modemId, SI_STK_EnvelopeRsp *rspData)
{
    // 需要确认addr1是目的地址还是addr2
    /*lint -e813*/
    SI_STK_EventInfo       event;
    SI_PIH_CardSlotUint32  slotId;
    errno_t                      ret;

    if (modemId >= MODEM_ID_BUTT) {
        return;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    (VOS_VOID)memset_s(&event, sizeof(event), 0, sizeof(event));

    event.cbEvent                               = SI_STK_SMSCTRL_RESULT_IND_EVENT;
    event.errorNo                               = VOS_OK;
    event.cmdDataInfo.cmdStru.ccIndInfo.result = (SI_UINT8)rspData->rsult;
    event.cmdDataInfo.cmdStru.ccIndInfo.type   = SI_STK_SMS_CTRL;

    /* COPY aplhaid 字段 */
    if (VOS_TRUE == rspData->resp.moSmsCtrlRsp.opAlaph) {
        event.cmdDataInfo.cmdStru.ccIndInfo.alphaIdInfo.alphaLen =
            (rspData->resp.moSmsCtrlRsp.alphaId.len > SI_STK_DATA_MAX_LEN)
            ? SI_STK_DATA_MAX_LEN
            : rspData->resp.moSmsCtrlRsp.alphaId.len;

        ret = memcpy_s(event.cmdDataInfo.cmdStru.ccIndInfo.alphaIdInfo.alphaId, SI_STK_DATA_MAX_LEN,
                       rspData->resp.moSmsCtrlRsp.alphaId.alphabet,
                       event.cmdDataInfo.cmdStru.ccIndInfo.alphaIdInfo.alphaLen);

        PAM_PRINT_SECFUN_RESULT(ret);
    }

    if (VOS_TRUE == rspData->resp.moSmsCtrlRsp.opAddr1) {
        /* 解析目的地址类型字段 */
        event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.moSmsCtrlInfo.dstAddrInfo.numType =
            rspData->resp.moSmsCtrlRsp.addr1.numType;
        /* copy目的地址 */
        event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.moSmsCtrlInfo.dstAddrInfo.addrLen =
            rspData->resp.moSmsCtrlRsp.addr1.len;

        ret = memcpy_s(event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.moSmsCtrlInfo.dstAddrInfo.addr,
                       SI_STK_ADDR_MAX_LEN, rspData->resp.moSmsCtrlRsp.addr1.addr,
                       rspData->resp.moSmsCtrlRsp.addr1.len);

        PAM_PRINT_SECFUN_RESULT(ret);
    }

    if (VOS_TRUE == rspData->resp.moSmsCtrlRsp.opAddr2) {
        /* 解析服务中心地址类型字段 */
        event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.moSmsCtrlInfo.serCenterAddrInfo.numType =
            rspData->resp.moSmsCtrlRsp.addr2.numType;

        /* copy服务中心号码 */
        event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.moSmsCtrlInfo.serCenterAddrInfo.addrLen =
            rspData->resp.moSmsCtrlRsp.addr2.len;

        ret = memcpy_s(event.cmdDataInfo.cmdStru.ccIndInfo.uInfo.moSmsCtrlInfo.serCenterAddrInfo.addr,
                       SI_STK_ADDR_MAX_LEN, rspData->resp.moSmsCtrlRsp.addr2.addr,
                       rspData->resp.moSmsCtrlRsp.addr2.len);

        PAM_PRINT_SECFUN_RESULT(ret);
    }

    SI_STKCallBack_BroadCast(slotId, &event);
    /*lint +e813*/
    return;
}


VOS_VOID SI_STK_DecodeCCRspSpecial(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 dataLen, VOS_UINT8 *cmdData,
                                   VOS_UINT32 spareBuffSize, SI_STK_EnvelopeRsp *rspData)
{
    VOS_UINT8 tagList[] = { ADDRESS_TAG, SS_STRING_TAG, USSD_STRING_TAG, PDP_CONTEXT_ACTIVATION_PAR_TAG, EPSPDN_ACTIVE_PARA_TAG };
    VOS_UINT32 i;
    VOS_UINT32 cCTagOffset;
    VOS_PID    stkPid;
    errno_t    ret;

    stkPid = USIMM_GetPidBySlotId(I0_MAPS_STK_PID, slotId);

    for (i = 0; i < sizeof(tagList); i++) {
        cCTagOffset = SI_STKFindTag(tagList[i], cmdData, spareBuffSize, dataLen, 1);
        if (cCTagOffset != SI_STK_TAGNOTFOUND) {
            rspData->resp.callCtrlRsp.specialData.tag = cmdData[cCTagOffset - 1] & 0x7F;
            rspData->resp.callCtrlRsp.specialData.len = cmdData[cCTagOffset];

            if (rspData->resp.callCtrlRsp.specialData.len > 0) {
                rspData->resp.callCtrlRsp.specialData.value = (SI_UINT8 *)VOS_MemAlloc(stkPid, DYNAMIC_MEM_PT,
                                                                                            cmdData[cCTagOffset]);

                if (VOS_NULL_PTR != rspData->resp.callCtrlRsp.specialData.value) {
                    /*lint -save -e679 */
                    ret = memcpy_s(rspData->resp.callCtrlRsp.specialData.value, cmdData[cCTagOffset],
                                   &cmdData[cCTagOffset + 1], cmdData[cCTagOffset]);
                    /*lint -restore */

                    PAM_PRINT_SECFUN_RESULT(ret);

                    rspData->resp.callCtrlRsp.opSepcialData = 1;
                }
            }

            return;
        }
    }

    return;
}


/*lint -save -e679 */
VOS_VOID SI_STK_DecodeMoSmsRspSpecial(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 dataLen,
                                      VOS_UINT8 *cmdData, VOS_UINT32 spareBuffSize,
                                      SI_STK_EnvelopeRsp *rspData)
{
    VOS_UINT32 tagOffset;
    VOS_PID    stkPid;

    stkPid = USIMM_GetPidBySlotId(I0_MAPS_STK_PID, slotId);

    rspData->resp.moSmsCtrlRsp.opAddr1 = 0;
    rspData->resp.moSmsCtrlRsp.opAddr2 = 0;

    tagOffset = SI_STKFindTag(ADDRESS_TAG, cmdData, spareBuffSize, dataLen, 1);
    if (tagOffset != SI_STK_TAGNOTFOUND) {
        rspData->resp.moSmsCtrlRsp.addr1.len   = 0;
        rspData->resp.moSmsCtrlRsp.addr1.addr = VOS_NULL_PTR;

        if (cmdData[tagOffset] > 1) {
            rspData->resp.moSmsCtrlRsp.addr1.len     = cmdData[tagOffset] - 1;
            rspData->resp.moSmsCtrlRsp.addr1.numType = cmdData[tagOffset + 1];
            rspData->resp.moSmsCtrlRsp.addr1.addr =
                (SI_UINT8 *)VOS_MemAlloc(stkPid, DYNAMIC_MEM_PT, rspData->resp.moSmsCtrlRsp.addr1.len);

            if (VOS_NULL_PTR != rspData->resp.moSmsCtrlRsp.addr1.addr) {
                (VOS_VOID)memcpy_s(rspData->resp.moSmsCtrlRsp.addr1.addr,
                                   rspData->resp.moSmsCtrlRsp.addr1.len, &cmdData[tagOffset + 2],
                                   rspData->resp.moSmsCtrlRsp.addr1.len);

                rspData->resp.moSmsCtrlRsp.opAddr1 = 1;
            }
        }
    }

    tagOffset = SI_STKFindTag(ADDRESS_TAG, cmdData, spareBuffSize, dataLen, 2);
    if (tagOffset != SI_STK_TAGNOTFOUND) {
        rspData->resp.moSmsCtrlRsp.addr2.len   = 0;
        rspData->resp.moSmsCtrlRsp.addr2.addr = VOS_NULL_PTR;

        if (cmdData[tagOffset] > 1) {
            rspData->resp.moSmsCtrlRsp.addr2.len     = cmdData[tagOffset] - 1;
            rspData->resp.moSmsCtrlRsp.addr2.numType = cmdData[tagOffset + 1];
            rspData->resp.moSmsCtrlRsp.addr2.addr =
                (SI_UINT8 *)VOS_MemAlloc(stkPid, DYNAMIC_MEM_PT, rspData->resp.moSmsCtrlRsp.addr2.len);

            if (VOS_NULL_PTR != rspData->resp.moSmsCtrlRsp.addr2.addr) {
                (VOS_VOID)memcpy_s(rspData->resp.moSmsCtrlRsp.addr2.addr,
                                   rspData->resp.moSmsCtrlRsp.addr2.len, &cmdData[tagOffset + 2],
                                   rspData->resp.moSmsCtrlRsp.addr2.len);

                rspData->resp.moSmsCtrlRsp.opAddr2 = 1;
            }
        }
    }

    return;
}
/*lint -restore */


MODULE_EXPORTED VOS_UINT32 SI_STK_EnvelopeRsp_Decode(ModemIdUint16        modemId,
                                                     SI_STK_EnvelopeTypeUint32 dataType, VOS_UINT32 dataLen,
                                                     VOS_UINT8 *cmdData, SI_STK_EnvelopeRsp *rspData)
{
    VOS_UINT32             i;
    VOS_UINT32             offset;
    SI_PIH_CardSlotUint32  slotId;

    if (modemId >= MODEM_ID_BUTT) {
        return VOS_ERR;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    if ((cmdData == VOS_NULL_PTR) || (rspData == VOS_NULL_PTR)) {
        STK_ERROR_LOG("SI_STKCommCodeData: The Input Parameter is Error");

        return VOS_ERR;
    }

    rspData->envelopeType = dataType;
    rspData->rsult       = cmdData[0];

    if (cmdData[0] > 0x80) { /* 确定主动命令的长度字节 */
        offset = 0x02;
    } else {
        offset = 0x01;
    }

    if ((offset + 1) >= dataLen) {
        return VOS_ERR;
    }

    for (i = 0; i < ARRAYSIZE(g_stkEnvelopeDecodeList); i++) {
        if (dataType == g_stkEnvelopeDecodeList[i].cmdType) {
            SI_STKDecodeTagList(slotId, &cmdData[offset], dataLen - offset,
                                g_stkEnvelopeDecodeList[i].tagList, g_stkEnvelopeDecodeList[i].tagLen,
                                (SI_SAT_Commdata *)rspData);

            break;
        }
    }

    if (i >= ARRAYSIZE(g_stkEnvelopeDecodeList)) {
        STK_ERROR_LOG("SI_STKCommCodeData: The Input enDataType is Error");

        return VOS_ERR;
    }

    if (dataType == SI_STK_ENVELOPE_CALLCRTL) {
        /*lint -save -e679 */
        SI_STK_DecodeCCRspSpecial(slotId, (VOS_UINT32)cmdData[offset], &cmdData[offset + 1],
                                  dataLen - (offset + 1), rspData);
        /*lint -restore */
    }

    if (dataType == SI_STK_ENVELOPE_SMSCRTL) {
        /*lint -save -e679 */
        SI_STK_DecodeMoSmsRspSpecial(slotId, (VOS_UINT32)cmdData[offset], &cmdData[offset + 1],
                                     dataLen - (offset + 1), rspData);
        /*lint -restore */
    }

    return VOS_OK;
}


VOS_UINT32 SI_STK_EnvelopeData_Code_ParaCheck(ModemIdUint16 modemId, SI_STK_Envelope *eNStru,
                                              VOS_UINT32 *dataLen, USIMM_U8LvData *eventData,
                                              VOS_UINT32 *dataOffset)
{
    SI_PIH_CardSlotUint32  slotId;

    if (modemId >= MODEM_ID_BUTT) {
        return VOS_ERR;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    if ((eNStru == VOS_NULL_PTR) || (dataLen == VOS_NULL_PTR) || (eventData == VOS_NULL_PTR) ||
        (eventData->data == VOS_NULL_PTR) || (eventData->dataLen <= 2) || (dataOffset == VOS_NULL_PTR)) {
        STK_ERROR_LOG("SI_STK_EnvelopeData_Code: The Input Data is Error");

        return VOS_ERR;
    }

    return VOS_OK;
}


MODULE_EXPORTED VOS_UINT32 SI_STK_EnvelopeData_Code(ModemIdUint16 modemId, SI_STK_Envelope *eNStru,
                                                    VOS_UINT32 *dataLen, USIMM_U8LvData *eventData,
                                                    VOS_UINT32 *dataOffset)
{
    SI_PIH_CardSlotUint32  slotId;
    USIMM_U8LvData         cmdData;
    errno_t                      ret;
    VOS_UINT32                   totalLen;

    if (SI_STK_EnvelopeData_Code_ParaCheck(modemId, eNStru, dataLen, eventData, dataOffset) != VOS_OK) {
        return VOS_ERR;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    SI_STK_InitEnvelope(eventData->data, eventData->dataLen, &eNStru->deviceId,
                        (VOS_UINT8)eNStru->envelopeType);

    if (eNStru->envelopeType == SI_STK_ENVELOPE_CALLCRTL) {
        if (eNStru->envelope.callCtrl.opSepcialData != 0) {
            if (eventData->dataLen <= 9) {
                STK_ERROR_LOG1("SI_STK_EnvelopeData_Code: dataLen  err", eventData->dataLen);
                return VOS_ERR;
            }

            totalLen = ((VOS_UINT32)eNStru->envelope.callCtrl.specialData.len + eventData->data[2] + 2);

            if (totalLen > USIMM_APDU_DATA_MAXLEN) {
                STK_ERROR_LOG1("SI_STK_EnvelopeData_Code: totalLen  err", totalLen);

                return VOS_ERR;
            }

            eventData->data[2] += eNStru->envelope.callCtrl.specialData.len + 2;
            eventData->data[7] = eNStru->envelope.callCtrl.specialData.tag;
            eventData->data[8] = eNStru->envelope.callCtrl.specialData.len;

            /* 调用该API的函数，传入的pucData长度为256 */
            ret = memcpy_s(&eventData->data[9], (eventData->dataLen - 9),
                           eNStru->envelope.callCtrl.specialData.value,
                           eNStru->envelope.callCtrl.specialData.len);
            if (ret != EOK) {
                STK_ERROR_LOG1("SI_STK_EnvelopeData_Code: memcpy_s  errno", (VOS_UINT32)ret);

                return VOS_ERR;
            }
        }
    }

    cmdData.dataLen = eventData->dataLen - 1;
    cmdData.data   = &eventData->data[1];

    if (SI_STKCommCodeData(slotId, &cmdData, SI_CODE_ENVELOPE_DATA, eNStru->envelopeType,
                           (SI_SAT_Commdata *)eNStru) != VOS_OK) {
        STK_ERROR_LOG("SI_STK_EnvelopeData_Code: The Code Data is Error");

        return VOS_ERR;
    }

    if (eventData->data[2] > 0x7F) {
        eventData->data[1] = 0x81;

        *dataLen = eventData->data[2] + 3;

        *dataOffset = 0;
    } else {
        *dataLen = eventData->data[2] + 2;

        *dataOffset = 1;
    }

    return VOS_OK;
}

MODULE_EXPORTED VOS_VOID SI_STK_EnvelopeRspDataFree(ModemIdUint16 modemId, SI_STK_EnvelopeRsp *data)
{
    VOS_UINT32                   i;
    VOS_UINT32                   j;
    VOS_UINT32                   offset = 0;
    VOS_UINT8                   *dataTemp    = VOS_NULL_PTR;
    VOS_PID                      stkPid;
    SI_PIH_CardSlotUint32  slotId;

    if (modemId >= MODEM_ID_BUTT) {
        return;
    }

    slotId = USIMM_GetSlotIdByModemId(modemId);

    stkPid = USIMM_GetPidBySlotId(I0_MAPS_STK_PID, slotId);

    if (VOS_NULL_PTR == data) {
        STK_ERROR_LOG("SI_STK_EnvelopeRspDataFree: The Input Parameter is Error");

        return;
    }

    dataTemp = (VOS_UINT8 *)&data->resp + sizeof(VOS_UINT32);

    for (i = 0; i < STK_ARRAYSIZE(g_stkEnvelopeDecodeList); i++) {
        if (data->envelopeType == g_stkEnvelopeDecodeList[i].cmdType) {
            for (j = 0; j < g_stkEnvelopeDecodeList[i].tagLen; j++) {
                offset = SI_STKTagDataFree(slotId, *(g_stkEnvelopeDecodeList[i].tagList + j), dataTemp);

                dataTemp += offset;
            }

            break;
        }
    }

    if ((data->envelopeType == SI_STK_ENVELOPE_CALLCRTL) && (data->resp.callCtrlRsp.opSepcialData != 0)) {
        (VOS_VOID)VOS_MemFree(stkPid, data->resp.callCtrlRsp.specialData.value);
    }

    if (data->envelopeType == SI_STK_ENVELOPE_SMSCRTL) {
        if (data->resp.moSmsCtrlRsp.opAddr1 != 0) {
            (VOS_VOID)VOS_MemFree(stkPid, data->resp.moSmsCtrlRsp.addr1.addr);
        }

        if (data->resp.moSmsCtrlRsp.opAddr2 != 0) {
            (VOS_VOID)VOS_MemFree(stkPid, data->resp.moSmsCtrlRsp.addr2.addr);
        }
    }

    return;
}

#endif
