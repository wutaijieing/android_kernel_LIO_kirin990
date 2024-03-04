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



#include "taf_type_def.h"
#include "si_pb.h"
#include "product_config.h"
#if (FEATURE_PHONE_SC == FEATURE_ON)
#include "om_private.h"
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "usimm_api.h"
#endif
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "si_pih.h"
#endif

#if (VOS_OS_VER == VOS_WIN32)
#include "ut_mem.h"
#endif

#define THIS_FILE_ID PS_FILE_ID_PBAPI_C
#define THIS_MODU mod_pam_pb

#if (FEATURE_PHONE_SC == FEATURE_ON)

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (defined(DMT)))

VOS_UINT32 SI_PB_GetReceiverPid(MN_CLIENT_ID_T clientId, VOS_UINT32 *receiverPid)
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
        *receiverPid = I0_MAPS_PB_PID;
    } else if (SI_PIH_CARD_SLOT_1 == slotId) {
        *receiverPid = I1_MAPS_PB_PID;
    } else {
        *receiverPid = I2_MAPS_PB_PID;
    }
#else
    *receiverPid = I0_MAPS_PIH_PID;
#endif

    return VOS_OK;
}


SI_UINT32 SI_PB_Read(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage, SI_UINT16 index1,
                     SI_UINT16 index2)
{
    return TAF_FAILURE;
}


SI_UINT32 SI_PB_SRead(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage, SI_UINT16 index1,
                      SI_UINT16 index2)
{
    return TAF_FAILURE;
}


SI_UINT32 SI_PB_Query(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId)
{
    return TAF_FAILURE;
}


SI_UINT32 SI_PB_Set(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage)
{
    SI_PB_SetReq *msg = VOS_NULL_PTR;
    VOS_UINT32          receiverPid;

    if (SI_PB_STORAGE_FD != storage) {
        PB_ERROR_LOG("SI_PB_Set:Double Modem only support the FDN");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PB_GetReceiverPid(clientId, &receiverPid)) {
        PB_ERROR_LOG("SI_PB_Set:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_PB_SetReq *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_SetReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PB_ERROR_LOG("SI_PB_Set:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->msgName     = SI_PB_SET_REQ;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->storage     = storage;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PB_ERROR_LOG("SI_PB_Set:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


SI_UINT32 SI_PB_Add(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage,
                    SI_PB_Record *record)
{
    SI_PB_AddRep *msg = VOS_NULL_PTR;
    VOS_UINT32          receiverPid;

    if (VOS_OK != SI_PB_GetReceiverPid(clientId, &receiverPid)) {
        PB_ERROR_LOG("SI_PB_Add:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if (VOS_NULL_PTR == record) {
        PB_ERROR_LOG("SI_PB_Add:pRecord is a NULL pointer");

        return TAF_FAILURE;
    }

    msg = (SI_PB_AddRep *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_AddRep) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PB_ERROR_LOG("SI_PB_Add:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->msgName     = SI_PB_ADD_REQ;
    msg->client      = clientId;
    msg->opId        = opId;

    msg->storage = SI_PB_STORAGE_FD; /* 只能够操作FDN号码 */

    record->index = 1;

    (VOS_VOID)memcpy_s(&msg->record, sizeof(SI_PB_Record), record, sizeof(SI_PB_Record));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PB_ERROR_LOG("SI_PB_Add:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

SI_UINT32 SI_PB_SAdd(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage,
                     SI_PB_Record *record)
{
    return TAF_FAILURE;
}


SI_UINT32 SI_PB_Modify(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage,
                       SI_PB_Record *record)
{
    SI_PB_ModifyRep *msg = VOS_NULL_PTR;
    VOS_UINT32             receiverPid;

    if (VOS_OK != SI_PB_GetReceiverPid(clientId, &receiverPid)) {
        PB_ERROR_LOG("SI_PB_Modify:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if (VOS_NULL_PTR == record) {
        PB_ERROR_LOG("SI_PB_Modify:pRecord is a NULL pointer");

        return TAF_FAILURE;
    }

    msg = (SI_PB_ModifyRep *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_ModifyRep) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PB_ERROR_LOG("SI_PB_Modify:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->msgName     = SI_PB_MODIFY_REQ;
    msg->client      = clientId;
    msg->opId        = opId;

    msg->storage = SI_PB_STORAGE_FD; /* 只能够操作FDN号码 */

    (VOS_VOID)memcpy_s(&msg->record, sizeof(SI_PB_Record), record, sizeof(SI_PB_Record));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PB_ERROR_LOG("SI_PB_Modify:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


SI_UINT32 SI_PB_SModify(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage,
                        SI_PB_Record *record)
{
    return TAF_FAILURE;
}


SI_UINT32 SI_PB_Delete(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage, SI_UINT16 index)
{
    SI_PB_DeleteReq *msg = VOS_NULL_PTR;
    VOS_UINT32             receiverPid;

    if (VOS_OK != SI_PB_GetReceiverPid(clientId, &receiverPid)) {
        PB_ERROR_LOG("SI_PB_Delete:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    msg = (SI_PB_DeleteReq *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PB_DeleteReq) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == msg) {
        PB_ERROR_LOG("SI_PB_Delete:VOS_AllocMsg Failed");

        return TAF_FAILURE;
    }

    msg->ulReceiverPid = receiverPid;
    msg->msgName     = SI_PB_DELETE_REQ;
    msg->client      = clientId;
    msg->opId        = opId;
    msg->index       = index;

    msg->storage = SI_PB_STORAGE_FD; /* 只能够操作FDN号码 */

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, msg)) {
        PB_ERROR_LOG("SI_PB_Delete:VOS_SendMsg Failed");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


SI_UINT32 SI_PB_Search(MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, SI_PB_StorateTypeUint32 storage, SI_UINT8 length,
                       SI_UINT8 *content)
{
    return TAF_FAILURE;
}


VOS_UINT32 SI_PB_GetStorateType(VOS_VOID)
{
    return SI_PB_STORAGE_UNSPECIFIED; /* 返回当前未指定 */
}


VOS_UINT32 SI_PB_GetSPBFlag(VOS_VOID)
{
    return VOS_FALSE; /* 返回状态关闭 */
}

#endif

#if ((OSA_CPU_CCPU == VOS_OSA_CPU) || (defined(DMT)))

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32 SI_PB_GetXeccNumber(SI_PIH_CardSlotUint32 slotId, SI_PB_EccData *eccData)
{
    VOS_UINT32 result;
    VOS_UINT32 num;
    VOS_UINT8 *temp    = VOS_NULL_PTR;
    VOS_UINT8  pbOffset = 0;
    VOS_UINT32 i;
    VOS_UINT32 j;
    VOS_UINT32 copyLen;
    errno_t    ret;

    /* 输入参数检测 */
    if (VOS_NULL_PTR == eccData) {
        PB_ERROR_LOG("SI_PB_GetXeccNumber Error: Para is incorrect.");

        return VOS_ERR;
    }

    (VOS_VOID)memset_s(eccData, sizeof(SI_PB_EccData), 0, sizeof(SI_PB_EccData));

    result = SI_PB_LocateRecord(slotId, PB_XECC, 1, 1, &pbOffset);
    /* 当前电话本不存在或者初始化未完成 */
    if (VOS_OK != result) {
        PB_ERROR_LOG("SI_PB_GetXeccNumber Error: SI_PB_LocateRecord Return Failed");

        eccData->eccExists  = SI_PB_CONTENT_INVALID;
        eccData->reocrdNum = VOS_NULL;

        return VOS_ERR;
    }

    eccData->eccExists = SI_PB_CONTENT_VALID;

    if (VOS_NULL_PTR == g_pbContent[slotId][pbOffset].content) {
        PB_ERROR_LOG("SI_PB_GetXeccNumber Error: content is NULL");

        return VOS_ERR;
    }

    num = ((g_pbContent[slotId][pbOffset].totalNum > USIM_MAX_ECC_RECORDS)
             ? USIM_MAX_ECC_RECORDS
             : g_pbContent[slotId][pbOffset].totalNum);

    temp = g_pbContent[slotId][pbOffset].content;

    copyLen = PAM_GetMin(g_pbContent[slotId][pbOffset].numberLen, USIM_ECC_LEN);

    for (i = 0, j = 0; i < num; i++) { /* 根据数据结构最大长度循环 */
        result = SI_PB_CheckEccValidity(slotId, temp[0]);
        if (VOS_ERR == result) { /* 当前记录内容无效 */
            PB_INFO_LOG("SI_PB_GetXeccNumber Info: The Ecc number is Empty");
        } else { /* 转换当前记录内容 */
            PB_INFO_LOG("SI_PB_GetXeccNumber Info: The Ecc number is Not Empty");

            ret = memcpy_s(eccData->eccRecord[j].eccCode, sizeof(eccData->eccRecord[j].eccCode),
                           temp, copyLen);

            PAM_PRINT_SECFUN_RESULT(ret);

            eccData->eccRecord[j].len = (VOS_UINT8)copyLen;

            j++;
        }

        temp += g_pbContent[slotId][pbOffset].recordLen;
    }

    eccData->reocrdNum = j;

    return VOS_OK;
}
#endif

#endif

#endif /* (FEATURE_ON == FEATURE_PHONE_SC) */

