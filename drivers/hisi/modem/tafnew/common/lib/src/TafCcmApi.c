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

#include "taf_ccm_api.h"
#include "mn_comm_api.h"
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "NasMultiInstanceApi.h"
#endif
#include "securec.h"


#define THIS_FILE_ID PS_FILE_ID_TAF_CCM_API_C

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
extern VOS_UINT32 AT_GetDestPid(MN_CLIENT_ID_T clientId, VOS_UINT32 rcvPid);
#endif


MODULE_EXPORTED VOS_UINT32 TAF_CCM_CallCommonReq(TAF_Ctrl *ctrl, const VOS_VOID *para, VOS_UINT32 msgType,
                                                 VOS_UINT32 paraLen, ModemIdUint16 modemId)
{
    TAF_CCM_AppReqMsg *tafCcmAppMsg = VOS_NULL_PTR;
    VOS_UINT8         *msg          = VOS_NULL_PTR;
    VOS_UINT32         msgLen;
    VOS_UINT32         receiverPid;
    VOS_UINT32         senderPid;
    errno_t            memResult;
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
    senderPid   = AT_GetDestPid(ctrl->clientId, WUEPS_PID_TAF);
    receiverPid = AT_GetDestPid(ctrl->clientId, UEPS_PID_CCM);
#else
    senderPid   = NAS_MULTIINSTANCE_GetSpecModemPid(modemId, WUEPS_PID_TAF);
    receiverPid = NAS_MULTIINSTANCE_GetSpecModemPid(modemId, UEPS_PID_CCM);
#endif

    msgLen = 0;

    if ((para == VOS_NULL_PTR) && (paraLen != 0)) {
        return VOS_ERR;
    }

    /* 获取消息长度 */
    msgLen = paraLen + sizeof(TAF_CCM_AppReqMsg);

    msg = (VOS_UINT8 *)PS_ALLOC_MSG_WITH_HEADER_LEN(senderPid, msgLen);

    if (msg == VOS_NULL_PTR) {
        return VOS_ERR;
    }

    tafCcmAppMsg = (TAF_CCM_AppReqMsg *)msg;

    TAF_CfgMsgHdr((MsgBlock *)tafCcmAppMsg, senderPid, receiverPid, (msgLen - VOS_MSG_HEAD_LENGTH));

    tafCcmAppMsg->msgName = msgType;

    /* 填充消息头 */
    (VOS_VOID)memcpy_s(&(tafCcmAppMsg->ctrl), sizeof(tafCcmAppMsg->ctrl), ctrl, sizeof(TAF_Ctrl));

    /* 支持不带任何参数的消息结构 */
    if ((para != VOS_NULL_PTR) && (paraLen > 0)) {
        memResult = memcpy_s(tafCcmAppMsg->content, paraLen, (VOS_UINT8 *)para, paraLen);
        TAF_MEM_CHK_RTN_VAL(memResult, paraLen, paraLen);
    }

    if (PS_SEND_MSG(senderPid, msg) != VOS_OK) {
        return VOS_ERR;
    }

    return VOS_OK;
}

