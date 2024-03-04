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



#include "si_pih.h"
#if ((VOS_OSA_CPU == OSA_CPU_ACPU) || (defined(DMT)))
#include "si_pb.h"
#include "si_typedef.h"
#include "product_config.h"
#include "at_oam_interface.h"
#include "om_private.h"
#include "pam_tag.h"
#endif

#if (VOS_OS_VER == VOS_WIN32)
#include "ut_mem.h"
#endif


#define THIS_FILE_ID PS_FILE_ID_PIH_API_C
#define THIS_MODU mod_pam_pih

#if ((VOS_OSA_CPU == OSA_CPU_ACPU) || (defined(DMT)))

VOS_UINT32 SI_PIH_GetReceiverPid(MN_CLIENT_ID_T clientId, VOS_UINT32 *receiverPid)
{
#if (MULTI_MODEM_NUMBER > 1)
    ModemIdUint16         modemId;
    SI_PIH_CardSlotUint32 slotId;

    /* 调用接口获取Modem ID */
    if (VOS_OK != AT_GetModemIdFromClient(clientId, &modemId)) {
        return VOS_ERR;
    }

    slotId = SI_GetSlotIdByModemId(modemId);
    if (SI_PIH_CARD_SLOT_BUTT <= slotId) {
        return VOS_ERR;
    }

    if (SI_PIH_CARD_SLOT_0 == slotId) {
        *receiverPid = I0_MAPS_PIH_PID;
    } else if (SI_PIH_CARD_SLOT_1 == slotId) {
        *receiverPid = I1_MAPS_PIH_PID;
    } else {
        *receiverPid = I2_MAPS_PIH_PID;
    }
#else
    *receiverPid = I0_MAPS_PIH_PID;
#endif
    return VOS_OK;
}


VOS_UINT32 SI_PIH_FdnEnable(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT8 *pin2, VOS_UINT32 len)
{
    SI_PIH_FdnEnableReq *msg = VOS_NULL_PTR;
    VOS_UINT32                  receiverPid;
    errno_t                     ret;

    if (pin2 == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_FdnEnable:PIN NULL.");
        return TAF_FAILURE;
    }

    if ((len > SI_PIH_PIN_CODE_LEN) || (len == 0)) {
        PIH_ERROR_LOG("SI_PIH_FdnEnable:PIN's len Error.");
        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_FdnEnable:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

#if (FEATURE_OFF == FEATURE_PHONE_SC)
    if (PB_INIT_FINISHED != g_pbInitState.pbInitStep) {
        PIH_ERROR_LOG("SI_PIH_FdnEnable:PB is Busy.");
        return TAF_FAILURE;
    }
#endif

    msg = (SI_PIH_FdnEnableReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                      sizeof(SI_PIH_FdnEnableReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_FdnEnable:WARNING AllocMsg FAILED.");
        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_FDN_ENABLE_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_FDN_CNF;

    (VOS_VOID)memset_s(msg->pin2, SI_PIH_PIN_CODE_LEN, 0xff, SI_PIH_PIN_CODE_LEN);

    ret = memcpy_s(msg->pin2, SI_PIH_PIN_CODE_LEN, pin2, len);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_FdnEnable:WARNING SendMsg FAILED.");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_FdnDisable(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT8 *pin2, VOS_UINT32 len)
{
    SI_PIH_FdnDisableReq *msg = VOS_NULL_PTR;
    VOS_UINT32                   receiverPid;
    errno_t                      ret;

    if (pin2 == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_FdnDisable:PIN NULL.");
        return TAF_FAILURE;
    }

    if ((len > SI_PIH_PIN_CODE_LEN) || (len == 0)) {
        PIH_ERROR_LOG("SI_PIH_FdnDisable:PIN's len Error.");
        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_FdnDisable:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

#if (FEATURE_OFF == FEATURE_PHONE_SC)
    if (PB_INIT_FINISHED != g_pbInitState.pbInitStep) {
        PIH_ERROR_LOG("SI_PIH_FdnEnable:PB is Busy.");
        return TAF_FAILURE;
    }
#endif

    msg = (SI_PIH_FdnDisableReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                       sizeof(SI_PIH_FdnDisableReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING AllocMsg FAILED");
        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_FDN_DISALBE_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_FDN_CNF;

    (VOS_VOID)memset_s(msg->pin2, SI_PIH_PIN_CODE_LEN, 0xff, SI_PIH_PIN_CODE_LEN);

    ret = memcpy_s(msg->pin2, SI_PIH_PIN_CODE_LEN, pin2, len);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING SendMsg FAILED");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_FdnBdnQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                              SI_PIH_QueryTypeUint32 queryType)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_FdnBdnQuery:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING AllocMsg FAILED");
        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;

    if (SI_PIH_FDN_QUERY == queryType) {
        msg->eventType = SI_PIH_EVENT_FDN_CNF;
        msg->msgName   = SI_PIH_FDN_QUERY_REQ;
    } else {
        msg->eventType = SI_PIH_EVENT_BDN_CNF;
        msg->msgName   = SI_PIH_BDN_QUERY_REQ;
    }

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING SendMsg FAILED");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_GenericAccessReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CsimCommand *data)
{
    SI_PIH_GaccessReq *msg = VOS_NULL_PTR;
    VOS_UINT32               receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    if (data == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq:Get NULL PTR.");
        return TAF_FAILURE;
    }

    if ((data->len == 0) || (data->len > SI_APDU_MAX_LEN)) {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq: Data Len is error");
        return TAF_FAILURE;
    }

    if ((data->len > SI_PIH_APDU_HDR_LEN) && (data->command[4] != data->len - SI_PIH_APDU_HDR_LEN)) {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq: Data Len is Not Eq P3");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_GaccessReq *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_GaccessReq) - VOS_MSG_HEAD_LENGTH +
                                                   data->len);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_GenericAccessReq: AllocMsg FAILED");
        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_GACCESS_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_GENERIC_ACCESS_CNF;
    msg->dataLen                 = data->len;

    (VOS_VOID)memcpy_s(msg->data, data->len, data->command, data->len);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING SendMsg FAILED");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_IsdbAccessReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                                SI_PIH_IsdbAccessCommand *data)
{
    return TAF_FAILURE;
}


VOS_UINT32 SI_PIH_CchoSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CchoCommand *cchoCmd)
{
    SI_PIH_CchoSetReqMsg *msg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;
    errno_t                   ret;

    /* 参数检测 */
    if (cchoCmd == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_CchoSetReq: NULL PTR.");

        return TAF_FAILURE;
    }

    if ((0 == cchoCmd->aidLen) || (USIMM_AID_LEN_MAX < cchoCmd->aidLen)) {
        PIH_ERROR_LOG("SI_PIH_CchoSetReq: AID length is incorrect.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CchoSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CchoSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT, (VOS_UINT32)(sizeof(SI_PIH_CchoSetReqMsg) -
                                                                                 VOS_MSG_HEAD_LENGTH));
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CchoSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_CCHO_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_CCHO_SET_CNF;
    msg->aidLen                  = cchoCmd->aidLen;

    ret = memcpy_s(msg->adfName, sizeof(msg->adfName), cchoCmd->adfName, cchoCmd->aidLen);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CchoSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_CchpSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CchpCommand *cchpCmd)
{
    SI_PIH_CchpSetReqMsg *msg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;
    errno_t                   ret;

    /* 参数检测 */
    if (cchpCmd == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_CchpSetReq: NULL_PTR.");

        return TAF_FAILURE;
    }

    if ((0 == cchpCmd->aidLen) || (USIMM_AID_LEN_MAX < cchpCmd->aidLen)) {
        PIH_ERROR_LOG("SI_PIH_CchpSetReq: AID length is incorrect.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CchpSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CchpSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT, (VOS_UINT32)(sizeof(SI_PIH_CchpSetReqMsg) -
                                                                                 VOS_MSG_HEAD_LENGTH));
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CchpSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_CCHP_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_CCHP_SET_CNF;
    msg->apdup2                  = cchpCmd->apdup2;
    msg->aidLen                  = cchpCmd->aidLen;

    ret = memcpy_s(msg->adfName, sizeof(msg->adfName), cchpCmd->adfName, cchpCmd->aidLen);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CchpSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_PrivateCchoSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                                    SI_PIH_CchoCommand *cchoCmd)
{
    SI_PIH_CchoSetReqMsg *msg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;
    errno_t                   ret;

    /* 参数检测 */
    if (cchoCmd == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_PrivateCchoSetReq: NULL_PTR.");

        return TAF_FAILURE;
    }

    if (cchoCmd->aidLen > USIMM_AID_LEN_MAX) {
        PIH_ERROR_LOG("SI_PIH_PrivateCchoSetReq: AID length is incorrect.");

        return TAF_FAILURE;
    }

    if (SI_PIH_GetReceiverPid(clientId, &receiverPid) != VOS_OK) {
        PIH_ERROR_LOG("SI_PIH_PrivateCchoSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CchoSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT, (VOS_UINT32)(sizeof(SI_PIH_CchoSetReqMsg) -
                                                                                 VOS_MSG_HEAD_LENGTH));
    if (msg == VOS_NULL_PTR) {
        PIH_WARNING_LOG("SI_PIH_PrivateCchoSetReq: VOS_AllocMsg FAILED.");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_PRIVATECCHO_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_PRIVATECCHO_SET_CNF;
    msg->aidLen                  = cchoCmd->aidLen;

    if (cchoCmd->aidLen != 0) {
        ret = memcpy_s(msg->adfName, sizeof(msg->adfName), cchoCmd->adfName, cchoCmd->aidLen);

        PAM_PRINT_SECFUN_RESULT(ret);
    }

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_PrivateCchoSetReq: VOS_SendMsg FAILED.");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_PrivateCchpSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                                    SI_PIH_CchpCommand *cchpCmd)
{
    SI_PIH_CchpSetReqMsg *msg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;
    errno_t                   ret;

    /* 参数检测 */
    if (cchpCmd == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_PrivateCchpSetReq: NULL_PTR.");

        return TAF_FAILURE;
    }

    if (cchpCmd->aidLen > USIMM_AID_LEN_MAX) {
        PIH_ERROR_LOG("SI_PIH_PrivateCchpSetReq: AID length is incorrect.");

        return TAF_FAILURE;
    }

    if (SI_PIH_GetReceiverPid(clientId, &receiverPid) != VOS_OK) {
        PIH_ERROR_LOG("SI_PIH_PrivateCchpSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CchpSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT, (VOS_UINT32)(sizeof(SI_PIH_CchpSetReqMsg) -
                                                                                 VOS_MSG_HEAD_LENGTH));
    if (msg == VOS_NULL_PTR) {
        PIH_WARNING_LOG("SI_PIH_PrivateCchpSetReq: VOS_AllocMsg FAILED.");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_PRIVATECCHP_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_PRIVATECCHP_SET_CNF;
    msg->apdup2                  = cchpCmd->apdup2;
    msg->aidLen                  = cchpCmd->aidLen;

    if (cchpCmd->aidLen != 0) {
        ret = memcpy_s(msg->adfName, sizeof(msg->adfName), cchpCmd->adfName, cchpCmd->aidLen);

        PAM_PRINT_SECFUN_RESULT(ret);
    }

    if (VOS_SendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        PIH_WARNING_LOG("SI_PIH_PrivateCchpSetReq: VOS_SendMsg FAILED.");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_CchcSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT32 sessionID)
{
    SI_PIH_CchcSetReqMsg *msg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CchcSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CchcSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT,
                                                      sizeof(SI_PIH_CchcSetReqMsg) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CchcSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_CCHC_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_CCHC_SET_CNF;
    msg->sessionId               = sessionID;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CchcSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_CglaSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CglaCommand *data)
{
    SI_PIH_CglaReq *msg = VOS_NULL_PTR;
    VOS_UINT32            receiverPid;
    errno_t               ret;

    /* 参数检测 */
    if (data == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_CglaSetReq: NULL_PTR.");

        return TAF_FAILURE;
    }

    if (((SI_APDU_MAX_LEN + 1) < data->len) || (data->len == 0)) {
        PIH_ERROR_LOG("SI_PIH_CglaSetReq:Command length is incorrect.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CglaSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CglaReq *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_CglaReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CglaSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_CGLA_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_CGLA_SET_CNF;
    msg->sessionId               = data->sessionID;
    msg->dataLen                 = data->len;

    ret = memcpy_s(msg->data, sizeof(msg->data), data->command, data->len);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CglaSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_GetCardATRReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_GetCardATRReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_GetCardATRReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->msgName     = SI_PIH_CARD_ATR_QRY_REQ;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->eventType   = SI_PIH_EVENT_CARD_ATR_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_GetCardATRReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_HvSstSet(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_HvsstSet *hvSStSet)
{
    SI_PIH_HvsstReq *msg = VOS_NULL_PTR;
    VOS_UINT32             receiverPid;

    /* 参数检测 */
    if (hvSStSet == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_HvSstSet: NULL_PTR.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_HvSstSet:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_HvsstReq *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_HvsstReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_HvSstSet:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.msgName     = SI_PIH_HVSST_SET_REQ;
    msg->msgHeader.eventType   = SI_PIH_EVENT_HVSST_SET_CNF;

    (VOS_VOID)memcpy_s(&msg->hvSSTData, sizeof(SI_PIH_HvsstSet), hvSStSet, sizeof(SI_PIH_HvsstSet));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_HvSstSet:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_HvSstQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_HvSstQuery:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_HvSstQuery:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_HVSST_QUERY_REQ;
    msg->eventType   = SI_PIH_EVENT_HVSST_QUERY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_HvSstQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_SciCfgSet(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CardSlotUint32 card0Slot,
                            SI_PIH_CardSlotUint32  card1Slot, SI_PIH_CardSlotUint32  card2Slot)
{
    SI_PIH_ScicfgSetReq *msg = VOS_NULL_PTR;
    VOS_UINT32                  receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_SciCfgSet:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_PIH_ScicfgSetReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                        sizeof(SI_PIH_ScicfgSetReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_SciCfgSet:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.msgName     = SI_PIH_SCICFG_SET_REQ;
    msg->msgHeader.eventType   = SI_PIH_EVENT_SCICFG_SET_CNF;

    msg->card0Slot = card0Slot;
    msg->card1Slot = card1Slot;
    msg->card2Slot = card2Slot;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_SciCfgSet:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}



VOS_UINT32 SI_PIH_SciCfgQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_SciCfgQuery:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_SciCfgQuery:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_SCICFG_QUERY_REQ;
    msg->eventType   = SI_PIH_EVENT_SCICFG_QUERY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_SciCfgQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_BwtSet(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT16 protectTime)
{
    SI_PIH_BwtSetReq *bwtSetMsg = VOS_NULL_PTR;
    VOS_UINT32               receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_BwtSet:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    bwtSetMsg = (SI_PIH_BwtSetReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                           sizeof(SI_PIH_BwtSetReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == bwtSetMsg) {
        PIH_WARNING_LOG("SI_PIH_BwtSet:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    bwtSetMsg->msgHeader.ulReceiverPid = receiverPid;
    bwtSetMsg->msgHeader.client      = clientId;
    bwtSetMsg->msgHeader.opId        = opId;
    bwtSetMsg->msgHeader.msgName     = SI_PIH_BWT_SET_REQ;
    bwtSetMsg->msgHeader.eventType   = SI_PIH_EVENT_BWT_SET_CNF;

    bwtSetMsg->protectTime = protectTime;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, bwtSetMsg)) {
        PIH_WARNING_LOG("SI_PIH_BwtSet:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_VOID SI_PIH_AcpuInit(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_VSIM)
#ifdef CONFIG_TZDRIVER
    VOS_UINT8 uuid[] = {
        0x47, 0x91, 0xe8, 0xab, 0x61, 0xcd, 0x3f, 0xf4, 0x71, 0xc4, 0x1a, 0x31, 0x7e, 0x40, 0x53, 0x12
    };

    if (VOS_OK != TC_NS_RegisterServiceCallbackFunc((VOS_CHAR *)uuid, SI_PIH_TEETimeOutCB, VOS_NULL_PTR)) {
        mdrv_err("<SI_PIH_AcpuInit> Reg TEE Timeout CB FUN Fail\n");
    }

    mdrv_debug("<SI_PIH_AcpuInit> Reg TEE Timeout CB FUN Ok\n");
#endif /* CONFIG_TZDRIVER */
#endif /* (FEATURE_ON == FEATURE_VSIM) */

    return;
}

#if (FEATURE_VSIM == FEATURE_ON)

VOS_UINT32 SI_PIH_GetSecIccVsimVer(VOS_VOID)
{
    return SI_PIH_SEC_ICC_VSIM_VER;
}

#ifdef CONFIG_TZDRIVER

VOS_VOID SI_PIH_TEETimeOutCB(VOS_VOID *timerDataCb)
{
    TEEC_TIMER_Property *timerData = VOS_NULL_PTR;
    MN_APP_PihAtCnf   *msg    = VOS_NULL_PTR;

    if (timerDataCb == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_TEETimeOutCB: para error!");

        return;
    }

    msg = (MN_APP_PihAtCnf *)VOS_AllocMsg(MAPS_PIH_PID, sizeof(MN_APP_PihAtCnf) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_ERROR_LOG("SI_PIH_TEETimeOutCB: Alloc Msg Failed!");

        return;
    }

    timerData = (TEEC_TIMER_Property *)timerDataCb;

    msg->pihAtEvent.eventType = SI_PIH_EVENT_TEETIMEOUT_IND;

    msg->pihAtEvent.pihError = TAF_ERR_NO_ERROR;

    msg->pihAtEvent.pihEvent.teeTimeOut.data = timerData->timeType;

    msg->ulReceiverPid = WUEPS_PID_AT;

    msg->msgId = PIH_AT_EVENT_CNF;

    msg->pihAtEvent.clientId = (MN_CLIENT_ALL & AT_BROADCAST_CLIENT_ID_MODEM_0);

    (VOS_VOID)VOS_SendMsg(MAPS_PIH_PID, msg);

    return;
}
#endif /* CONFIG_TZDRIVER */


VOS_UINT32 SI_PIH_HvCheckCardQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    return TAF_SUCCESS;
}

#endif /* end of (FEATURE_VSIM == FEATURE_ON) */

#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 SI_PIH_UiccAuthReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_UiccAuth *data)
{
    SI_PIH_UiccauthReq *msg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_UiccAuthReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if (VOS_NULL_PTR == data) {
        PIH_WARNING_LOG("SI_PIH_UiccAuthReq:Para Check Error");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_UiccauthReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                      sizeof(SI_PIH_UiccauthReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_UiccAuthReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_UICCAUTH_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_UICCAUTH_CNF;

    (VOS_VOID)memcpy_s(&msg->authData, sizeof(SI_PIH_UiccAuth), data, sizeof(SI_PIH_UiccAuth));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_UiccAuthReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_AccessUICCFileReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_AccessFile *data)
{
    SI_PIH_AccessfileReq *msg = VOS_NULL_PTR;
    VOS_UINT32                  receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_AccessUICCFileReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if (VOS_NULL_PTR == data) {
        PIH_WARNING_LOG("SI_PIH_AccessUICCFileReq:Para Check Error");

        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_AccessfileReq *)VOS_AllocMsg(WUEPS_PID_AT, (VOS_UINT32)(sizeof(SI_PIH_AccessfileReq) -
                                                                                   VOS_MSG_HEAD_LENGTH));
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_AccessUICCFileReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_URSM_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_URSM_CNF;

    (VOS_VOID)memcpy_s(&msg->cmdData, sizeof(SI_PIH_AccessFile), data, sizeof(SI_PIH_AccessFile));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_AccessUICCFileReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}
#endif /* (FEATURE_ON == FEATURE_IMS) */


VOS_UINT32 SI_PIH_CardTypeQueryProc(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT32 msgName,
                                    SI_PIH_EventUint32 eventType)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CardTypeQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT,
                                                  (VOS_UINT32)sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CardTypeQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = msgName;
    msg->eventType   = eventType;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CardTypeQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_CardTypeExQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    return SI_PIH_CardTypeQueryProc(clientId, opId, SI_PIH_CARDTYPEEX_QUERY_REQ, SI_PIH_EVENT_CARDTYPEEX_QUERY_CNF);
}

#if (FEATURE_ON == FEATURE_PHONE_SC)

VOS_UINT32 SI_PIH_GetSilentPinInfoReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT8 *pin,
                                      VOS_UINT32 len)
{
    SI_PIH_SilentPininfoReq *msg = VOS_NULL_PTR;
    VOS_UINT32                      receiverPid;
    errno_t                         ret;

    if (pin == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_GetSilentPinInfoReq:PIN NULL.");
        return TAF_FAILURE;
    }

    if ((len > USIMM_PINNUMBER_LEN) || (len == 0)) {
        PIH_ERROR_LOG("SI_PIH_GetSilentPinInfoReq:PIN's len Error.");
        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_GetSilentPinInfoReq: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_SilentPininfoReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                            (VOS_UINT32)sizeof(SI_PIH_SilentPininfoReq) -
                                                            VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_GetSilentPinInfoReq: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_SILENT_PININFO_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_SILENT_PININFO_SET_CNF;
    msg->dataLen                 = USIMM_PINNUMBER_LEN;

    (VOS_VOID)memset_s(msg->data, USIMM_PINNUMBER_LEN, 0xff, USIMM_PINNUMBER_LEN);

    ret = memcpy_s(msg->data, USIMM_PINNUMBER_LEN, pin, USIMM_PINNUMBER_LEN);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_ReserveMsg(WUEPS_PID_AT, (MsgBlock *)msg)) {
        PIH_WARNING_LOG("SI_PIH_GetSilentPinInfoReq:reserve Msg FAILED");

        /* 清空跨核消息中含有的敏感信息 */
        (VOS_VOID)memset_s(msg->data, USIMM_PINNUMBER_LEN, 0XFF, USIMM_PINNUMBER_LEN);

        (VOS_VOID)VOS_FreeMsg(WUEPS_PID_AT, msg);

        return TAF_FAILURE;
    }

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_GetSilentPinInfoReq:WARNING SendMsg FAILED");

        /* 清空跨核消息中含有的敏感信息 */
        (VOS_VOID)memset_s(msg->data, USIMM_PINNUMBER_LEN, 0XFF, USIMM_PINNUMBER_LEN);

        (VOS_VOID)VOS_FreeReservedMsg(WUEPS_PID_AT, msg);

        return TAF_FAILURE;
    }

    /* 清空跨核消息中含有的敏感信息 */
    (VOS_VOID)memset_s(msg->data, USIMM_PINNUMBER_LEN, 0XFF, USIMM_PINNUMBER_LEN);

    (VOS_VOID)VOS_FreeReservedMsg(WUEPS_PID_AT, msg);

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_SetSilentPinReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CryptoPin *cryptoPin)
{
    SI_PIH_SilentPinReq *msg = VOS_NULL_PTR;
    VOS_UINT32                  receiverPid;

    if (cryptoPin == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_SetSilentPinReq: pstCryptoPin is null.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_SetSilentPinReq: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_SilentPinReq *)VOS_AllocMsg(WUEPS_PID_AT, (VOS_UINT32)sizeof(SI_PIH_SilentPinReq) -
                                                        VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_SetSilentPinReq: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_SILENT_PIN_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_SILENT_PIN_SET_CNF;

    (VOS_VOID)memcpy_s(&(msg->cryptoPin), sizeof(SI_PIH_CryptoPin), cryptoPin,
                       sizeof(SI_PIH_CryptoPin));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_SetSilentPinReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}
#endif


VOS_UINT32 SI_PIH_EsimSwitchSetFunc(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId,
                                SI_PIH_EsimSwitchSet *esimSwitchSet)
{
    SI_PIH_EsimswitchReq *msg = VOS_NULL_PTR;
    VOS_UINT32                  receiverPid;

    if (VOS_NULL_PTR == esimSwitchSet) {
        PIH_WARNING_LOG("SI_PIH_EsimSwitchSetFunc:Parameter is Wrong");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_EsimSwitchSetFunc:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_EsimswitchReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                      sizeof(SI_PIH_EsimswitchReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_EsimSwitchSetFunc:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.msgName     = SI_PIH_ESIMSWITCH_SET_REQ;
    msg->msgHeader.eventType   = SI_PIH_EVENT_ESIMSWITCH_SET_CNF;

    msg->esimSwitchData.slot     = esimSwitchSet->slot;
    msg->esimSwitchData.cardType = esimSwitchSet->cardType;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_EsimSwitchSetFunc:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_EsimSwitchQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_EsimSwitchQuery:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_EsimSwitchQuery:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_ESIMSWITCH_QRY_REQ;
    msg->eventType   = SI_PIH_EVENT_ESIMSWITCH_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_EsimSwitchQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_CardTypeQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    return SI_PIH_CardTypeQueryProc(clientId, opId, SI_PIH_CARDTYPE_QUERY_REQ, SI_PIH_EVENT_CARDTYPE_QUERY_CNF);
}


VOS_UINT32 SI_PIH_CardVoltageQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CardVoltageQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT,
                                                  (VOS_UINT32)(sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH));
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CardVoltageQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_CARDVOLTAGE_QUERY_REQ;
    msg->eventType   = SI_PIH_EVENT_CARDVOLTAGE_QUERY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CardVoltageQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_PIH_PrivateCglaSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_CglaCommand *data)
{
    SI_PIH_CglaReq *msg = VOS_NULL_PTR;
    VOS_UINT32            receiverPid;
    errno_t               ret;

    if (data == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_PrivateCglaSetReq:PIN NULL.");
        return TAF_FAILURE;
    }

    if (((SI_APDU_MAX_LEN + 1) < data->len) || (data->len == 0)) {
        PIH_ERROR_LOG("SI_PIH_CglaSetReq:Command length is incorrect.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CglaHandleReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    msg = (SI_PIH_CglaReq *)VOS_AllocMsg(WUEPS_PID_AT,
                                                  (VOS_UINT32)(sizeof(SI_PIH_CglaReq) - VOS_MSG_HEAD_LENGTH));
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CglaHandleReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->msgHeader.ulReceiverPid = receiverPid;
    msg->msgHeader.msgName     = SI_PIH_PRIVATECGLA_SET_REQ;
    msg->msgHeader.client      = clientId;
    msg->msgHeader.opId        = opId;
    msg->msgHeader.eventType   = SI_PIH_EVENT_PRIVATECGLA_SET_CNF;
    msg->sessionId               = data->sessionID;
    msg->dataLen                 = data->len;

    ret = memcpy_s(msg->data, sizeof(msg->data), data->command, data->len);

    PAM_PRINT_SECFUN_RESULT(ret);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CglaHandleReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

VOS_UINT32 SI_PIH_CrsmSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_Crsm *crsmPara)
{
    SI_PIH_CrsmSetReqMsg *crsmMsg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;

    if (crsmPara == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_CrsmSetReq: pstCrsmPara is null.");
        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CrsmSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    crsmMsg = (SI_PIH_CrsmSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT,
                                                          sizeof(SI_PIH_CrsmSetReqMsg) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == crsmMsg) {
        PIH_WARNING_LOG("SI_PIH_CrsmSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    crsmMsg->msgHeader.ulReceiverPid = receiverPid;
    crsmMsg->msgHeader.msgName     = SI_PIH_CRSM_SET_REQ;
    crsmMsg->msgHeader.client      = clientId;
    crsmMsg->msgHeader.opId        = opId;
    crsmMsg->msgHeader.eventType   = SI_PIH_EVENT_CRSM_SET_CNF;

    (VOS_VOID)memcpy_s(&(crsmMsg->msgContent), sizeof(SI_PIH_Crsm), crsmPara, sizeof(SI_PIH_Crsm));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, crsmMsg)) {
        PIH_WARNING_LOG("SI_PIH_CrsmSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

VOS_UINT32 SI_PIH_CrlaSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PIH_Crla *crlaPara)
{
    SI_PIH_CrlaSetReqMsg *crlaMsg = VOS_NULL_PTR;
    VOS_UINT32                receiverPid;

    if (crlaPara == VOS_NULL_PTR) {
        PIH_ERROR_LOG("SI_PIH_CrlaSetReq: pstCrlaPara is null.");
        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CrlaSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* 分配消息内存 */
    crlaMsg = (SI_PIH_CrlaSetReqMsg *)VOS_AllocMsg(WUEPS_PID_AT,
                                                          sizeof(SI_PIH_CrlaSetReqMsg) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == crlaMsg) {
        PIH_WARNING_LOG("SI_PIH_CrlaSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    crlaMsg->msgHeader.ulReceiverPid = receiverPid;
    crlaMsg->msgHeader.msgName     = SI_PIH_CRLA_SET_REQ;
    crlaMsg->msgHeader.client      = clientId;
    crlaMsg->msgHeader.opId        = opId;
    crlaMsg->msgHeader.eventType   = SI_PIH_EVENT_CRLA_SET_CNF;

    (VOS_VOID)memcpy_s(&(crlaMsg->msgContent), sizeof(SI_PIH_Crla), crlaPara, sizeof(SI_PIH_Crla));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, crlaMsg)) {
        PIH_WARNING_LOG("SI_PIH_CchcSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

VOS_UINT32 SI_PIH_CardSessionQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CardSessionQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_SESSION_QRY_REQ;
    msg->eventType   = SI_PIH_EVENT_SESSION_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

VOS_UINT32 SI_PIH_CimiSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CardSessionQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CimiSetReq: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_CIMI_QRY_REQ;
    msg->eventType   = SI_PIH_EVENT_CIMI_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CimiSetReq: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

VOS_UINT32 SI_PIH_CCimiSetReq(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_PIH_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32              receiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(clientId, &receiverPid)) {
        PIH_ERROR_LOG("SI_PIH_CardSessionQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    msg = (SI_PIH_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_PIH_CCIMI_QRY_REQ;
    msg->eventType   = SI_PIH_EVENT_CCIMI_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}
#endif /* ((OSA_CPU_ACPU == VOS_OSA_CPU) || (defined(DMT))) */

