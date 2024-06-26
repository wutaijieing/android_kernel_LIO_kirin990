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


#include "si_emat.h"


#define THIS_FILE_ID PS_FILE_ID_SI_EMAT_API_C
#define THIS_MODU mod_pam_emat

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (defined(DMT)))

VOS_UINT32 SI_EMAT_GetReceiverPid(MN_CLIENT_ID_T clientId, VOS_UINT32 *receiverPid)
{
#if ((MULTI_MODEM_NUMBER > 1) && (FEATURE_PHONE_SC == FEATURE_ON))
    ModemIdUint16         modemId;
    SI_PIH_CardSlotUint32  slotId;

    /* 调用接口获取Modem ID */
    if (AT_GetModemIdFromClient(clientId, &modemId) != VOS_OK) {
        return VOS_ERR;
    }

    slotId = SI_GetSlotIdByModemId(modemId);
    if (slotId >= SI_PIH_CARD_SLOT_BUTT) {
        return VOS_ERR;
    }

    if (slotId == SI_PIH_CARD_SLOT_0) {
        *receiverPid = I0_MAPS_EMAT_PID;
    } else if (slotId == SI_PIH_CARD_SLOT_1) {
        *receiverPid = I1_MAPS_EMAT_PID;
    } else {
        *receiverPid = I2_MAPS_EMAT_PID;
    }
#else
    *receiverPid = I0_MAPS_EMAT_PID;
#endif
    return VOS_OK;
}


VOS_UINT32 SI_EMAT_EsimCleanProfile(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_EMAT_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32               receiverPid;

    if (SI_EMAT_GetReceiverPid(clientId, &receiverPid) != VOS_OK) {
        EMAT_ERROR_LOG("SI_EMAT_SetEsimCleanProfile:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_EMAT_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_EMAT_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (msg == VOS_NULL_PTR) {
        EMAT_WARNING_LOG("SI_EMAT_SetEsimCleanProfile: AllocMsg Failed.");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_EMAT_ESIM_CLEAN_REQ;
    msg->eventType   = SI_EMAT_EVENT_ESIM_CLEAN_CNF;

    if (VOS_SendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        EMAT_WARNING_LOG("SI_EMAT_SetEsimCleanProfile:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_EMAT_EsimCheckProfile(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_EMAT_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32               receiverPid;

    if (SI_EMAT_GetReceiverPid(clientId, &receiverPid) != VOS_OK) {
        EMAT_ERROR_LOG("SI_EMAT_EsimCheckProfile:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_EMAT_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_EMAT_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (msg == VOS_NULL_PTR) {
        EMAT_WARNING_LOG("SI_EMAT_EsimCheckProfile: AllocMsg Failed.");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_EMAT_ESIM_CHECK_REQ;
    msg->eventType   = SI_EMAT_EVENT_ESIM_CHECK_CNF;

    if (VOS_SendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        EMAT_WARNING_LOG("SI_EMAT_EsimCheckProfile:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_EMAT_EsimEidQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_EMAT_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32               receiverPid;

    if (SI_EMAT_GetReceiverPid(clientId, &receiverPid) != VOS_OK) {
        EMAT_ERROR_LOG("SI_EMAT_EsimEidQuery:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_EMAT_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_EMAT_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (msg == VOS_NULL_PTR) {
        EMAT_WARNING_LOG("SI_EMAT_EsimEidQuery: AllocMsg Failed.");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_EMAT_ESIM_EID_QUERY_REQ;
    msg->eventType   = SI_EMAT_EVENT_GET_ESIMEID_CNF;

    if (VOS_SendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        EMAT_WARNING_LOG("SI_EMAT_EsimEidQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_UINT32 SI_EMAT_EsimPKIDQuery(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    SI_EMAT_MsgHeader *msg = VOS_NULL_PTR;
    VOS_UINT32               receiverPid;

    if (SI_EMAT_GetReceiverPid(clientId, &receiverPid) != VOS_OK) {
        EMAT_ERROR_LOG("SI_EMAT_EsimPKIDQuery:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_EMAT_MsgHeader *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_EMAT_MsgHeader) - VOS_MSG_HEAD_LENGTH);
    if (msg == VOS_NULL_PTR) {
        EMAT_WARNING_LOG("SI_EMAT_EsimPKIDQuery:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->msgName     = SI_EMAT_ESIM_PKID_QUERY_REQ;
    msg->eventType   = SI_EMAT_EVENT_GET_ESIMPKID_CNF;

    if (VOS_SendMsg(WUEPS_PID_AT, msg) != VOS_OK) {
        EMAT_WARNING_LOG("SI_EMAT_EsimPKIDQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}
#endif

