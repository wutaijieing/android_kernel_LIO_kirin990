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

#include "mn_comm_api.h"
#include "at_mn_interface.h"
#include "mn_client.h"
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "NasMultiInstanceApi.h"
#endif
#include "securec.h"


#define THIS_FILE_ID PS_FILE_ID_MN_COMM_API_C

#if (OSA_CPU_ACPU == VOS_OSA_CPU) || (defined(DMT))
extern VOS_UINT32 AT_GetDestPid(MN_CLIENT_ID_T clientId, VOS_UINT32 rcvPid);
#endif


VOS_VOID MN_GetAppReqMsgLen(VOS_UINT32 paraLen, VOS_UINT32 *msgLen)
{
    VOS_UINT32 msgLenTemp;

    if (msgLen == VOS_NULL_PTR) {
        return;
    }

    if (paraLen <= 4) {
        msgLenTemp = sizeof(MN_APP_ReqMsg);
    } else {
        msgLenTemp = (sizeof(MN_APP_ReqMsg) + paraLen) - 4;
    }

    *msgLen = msgLenTemp;

    return;
}


VOS_VOID MN_FillAppReqMsgHeader(MN_APP_ReqMsg *msg, MN_CLIENT_ID_T clientId, MN_OPERATION_ID_T opId, VOS_UINT16 msgType,
                                VOS_UINT32 receiverPid)
{
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
    ModemIdUint16 modemId;
    VOS_UINT32    rcvPid;
#endif

    MN_APP_ReqMsg *appMsgHeader = VOS_NULL_PTR;

    appMsgHeader           = msg;
    appMsgHeader->clientId = clientId;
    appMsgHeader->opId     = opId;
    appMsgHeader->msgName  = msgType;
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    VOS_SET_RECEIVER_ID(appMsgHeader, AT_GetDestPid(clientId, receiverPid));
#else
    modemId = NAS_MULTIINSTANCE_GetCurrInstanceModemId(WUEPS_PID_TAF);

    rcvPid = NAS_MULTIINSTANCE_GetSpecModemPid(modemId, receiverPid);

    VOS_SET_RECEIVER_ID(appMsgHeader, rcvPid);
#endif

    return;
}


VOS_VOID MN_FillAppReqMsgPara(VOS_VOID *sndMsgPara, const VOS_VOID *para, VOS_UINT32 len)
{
    errno_t    memResult;
    VOS_UINT32 msgLen;
    if (sndMsgPara == VOS_NULL_PTR) {
        return;
    }

    if (len > 0) {
        MN_GetAppReqMsgLen(len, &msgLen);
        memResult = memcpy_s(sndMsgPara, msgLen - (sizeof(MN_APP_ReqMsg) - 4), para, len);
        TAF_MEM_CHK_RTN_VAL(memResult, msgLen - (sizeof(MN_APP_ReqMsg) - 4), len);
    }

    return;
}


VOS_UINT32 MN_FillAndSndAppReqMsg(const MN_CLIENT_OperationId *clientOperationId, VOS_UINT16 msgType,
                                  const VOS_VOID *para, VOS_UINT32 paraLen, VOS_UINT32 receiverPid)
{
    VOS_UINT8 *msg = VOS_NULL_PTR;
    VOS_UINT32 ret;
    VOS_UINT32 msgLen;
    errno_t    memResult;
    VOS_UINT8 *msgPara = VOS_NULL_PTR;

    if ((para == VOS_NULL_PTR) && (paraLen > 0)) {
        return TAF_FAILURE;
    }

    msgPara = (VOS_UINT8 *)para;

    /* 获取消息长度 */
    MN_GetAppReqMsgLen(paraLen, &msgLen);

    /* 申请消息 */
    msg = (VOS_UINT8 *)PS_ALLOC_MSG(WUEPS_PID_AT, msgLen - VOS_MSG_HEAD_LENGTH);

    if (msg == VOS_NULL_PTR) {
        return TAF_FAILURE;
    }

    memResult = memset_s((msg + VOS_MSG_HEAD_LENGTH), (msgLen - VOS_MSG_HEAD_LENGTH), 0x00,
                         (msgLen - VOS_MSG_HEAD_LENGTH));
    TAF_MEM_CHK_RTN_VAL(memResult, (msgLen - VOS_MSG_HEAD_LENGTH), (msgLen - VOS_MSG_HEAD_LENGTH));

    /* 填充消息头 */
    MN_FillAppReqMsgHeader((MN_APP_ReqMsg *)msg, clientOperationId->clientId, clientOperationId->opId, msgType,
                           receiverPid);

    /* 填充消息参数 */
    MN_FillAppReqMsgPara(&msg[sizeof(MN_APP_ReqMsg) - 4], msgPara, paraLen);

    ret = PS_SEND_MSG(WUEPS_PID_AT, msg);

    if (ret != VOS_OK) {
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


VOS_VOID TAF_CfgMsgHdr(MsgBlock *msg, VOS_UINT32 sndPid, VOS_UINT32 receiverPid, VOS_UINT32 length)
{
    errno_t memResult;
    memResult = memset_s((VOS_INT8 *)msg + VOS_MSG_HEAD_LENGTH, length, 0x00, length);
    TAF_MEM_CHK_RTN_VAL(memResult, length, length);

#if (VOS_OS_VER == VOS_WIN32)
    msg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    msg->ulReceiverCpuId = VOS_LOCAL_CPUID;
#endif
    VOS_SET_SENDER_ID(msg, sndPid);
    VOS_SET_RECEIVER_ID(msg, receiverPid);
    VOS_SET_MSG_LEN(msg, length);

    return;
}


VOS_VOID* TAF_AllocPrivacyMsg(VOS_UINT32 pid, const MsgBlock *msg, VOS_UINT32 *length)
{
    VOS_UINT32 msgLen;
    VOS_UINT8 *privacyMsg = VOS_NULL_PTR;

    msgLen = VOS_GET_MSG_LEN(msg) + VOS_MSG_HEAD_LENGTH;

    if (msgLen <= VOS_MSG_HEAD_LENGTH) {
        *length = 0;
        return VOS_NULL_PTR;
    }

    privacyMsg = (VOS_UINT8 *)VOS_MemAlloc(pid, DYNAMIC_MEM_PT, msgLen);

    *length = msgLen;

    return (VOS_VOID *)privacyMsg;
}

