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

#include "taf_iface_api.h"

#include "ps_common_def.h"
#include "securec.h"
#include "securec.h"
#include "mn_comm_api.h"


/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_TAF_IFACE_API_C


VOS_UINT32 TAF_IFACE_SndDsmMsg(const VOS_UINT16 modemId, const VOS_UINT32 msgId, const VOS_VOID *data,
                               const VOS_UINT32 length)
{
    TAF_PS_Msg *msg = VOS_NULL_PTR;
    VOS_UINT32  sendPid;
    VOS_UINT32  rcvPid;
    errno_t     memResult;

    switch (modemId) {
        case MODEM_ID_0:
            sendPid = I0_WUEPS_PID_TAF;
            rcvPid  = I0_UEPS_PID_DSM;
            break;

        case MODEM_ID_1:
            sendPid = I1_WUEPS_PID_TAF;
            rcvPid  = I1_UEPS_PID_DSM;
            break;

        case MODEM_ID_2:
            sendPid = I2_WUEPS_PID_TAF;
            rcvPid  = I2_UEPS_PID_DSM;
            break;

        default:
            PS_PRINTF("TAF_IFACE_SndDsmMsg: ModemId is Error!");
            return VOS_ERR;
    }

    /* ������Ϣ */
    msg = (TAF_PS_Msg *)PS_ALLOC_MSG_WITH_HEADER_LEN(sendPid, sizeof(MSG_Header) + length);
    if (msg == VOS_NULL_PTR) {
        return VOS_ERR;
    }

    TAF_CfgMsgHdr((MsgBlock *)msg, sendPid, rcvPid, (sizeof(MSG_Header) + length - VOS_MSG_HEAD_LENGTH));

    msg->header.msgName = msgId;

    /* ��д��Ϣ���� */
    if (length > 0) {
        memResult = memcpy_s(msg->content, length, data, length);
        TAF_MEM_CHK_RTN_VAL(memResult, length, length);
    }

    /* ������Ϣ */
    if (PS_SEND_MSG(sendPid, msg) != VOS_OK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


MODULE_EXPORTED VOS_UINT32 TAF_IFACE_Up(const TAF_Ctrl *ctrl, const TAF_IFACE_Active *ifaceUp)
{
    errno_t         memResult;
    VOS_UINT32      result;
    TAF_IFACE_UpReq ifaceUpReq;

    /* ��ʼ�� */
    result = VOS_OK;
    memset_s(&ifaceUpReq, sizeof(ifaceUpReq), 0x00, sizeof(TAF_IFACE_UpReq));

    /* ����ID_MSG_TAF_IFACE_UP_REQ��Ϣ */
    ifaceUpReq.ctrl.moduleId = ctrl->moduleId;
    ifaceUpReq.ctrl.clientId = TAF_PS_GET_CLIENTID_FROM_EXCLIENTID(ctrl->clientId);
    ifaceUpReq.ctrl.opId     = ctrl->opId;

    memResult = memcpy_s(&ifaceUpReq.ifaceUp, sizeof(ifaceUpReq.ifaceUp), ifaceUp, sizeof(TAF_IFACE_Active));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(ifaceUpReq.ifaceUp), sizeof(TAF_IFACE_Active));

    /* ������Ϣ */
    result = TAF_IFACE_SndDsmMsg(TAF_PS_GET_MODEMID_FROM_EXCLIENTID(ctrl->clientId), ID_MSG_TAF_IFACE_UP_REQ,
                                 &ifaceUpReq, sizeof(TAF_IFACE_UpReq));
    memset_s(&ifaceUpReq, sizeof(ifaceUpReq), 0x00, sizeof(TAF_IFACE_UpReq));
    return result;
}


MODULE_EXPORTED VOS_UINT32 TAF_IFACE_Down(const TAF_Ctrl *ctrl, const TAF_IFACE_Deactive *ifaceDown)
{
    errno_t           memResult;
    VOS_UINT32        result;
    TAF_IFACE_DownReq ifaceDownReq;

    /* ��ʼ�� */
    result = VOS_OK;
    memset_s(&ifaceDownReq, sizeof(ifaceDownReq), 0x00, sizeof(TAF_IFACE_DownReq));

    /* ����ID_MSG_TAF_IFACE_DOWN_REQ��Ϣ */
    ifaceDownReq.ctrl.moduleId = ctrl->moduleId;
    ifaceDownReq.ctrl.clientId = TAF_PS_GET_CLIENTID_FROM_EXCLIENTID(ctrl->clientId);
    ifaceDownReq.ctrl.opId     = ctrl->opId;

    memResult = memcpy_s(&ifaceDownReq.ifaceDown, sizeof(ifaceDownReq.ifaceDown), ifaceDown,
                         sizeof(TAF_IFACE_Deactive));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(ifaceDownReq.ifaceDown), sizeof(TAF_IFACE_Deactive));

    /* ������Ϣ */
    result = TAF_IFACE_SndDsmMsg(TAF_PS_GET_MODEMID_FROM_EXCLIENTID(ctrl->clientId), ID_MSG_TAF_IFACE_DOWN_REQ,
                                 &ifaceDownReq, sizeof(TAF_IFACE_DownReq));

    return result;
}


VOS_UINT32 TAF_IFACE_GetDynamicPara(const TAF_Ctrl *ctrl, const VOS_UINT32 bitCid)
{
    VOS_UINT32                  result;
    TAF_IFACE_GetDynamicParaReq dynamicParaReq;

    /* ��ʼ�� */
    result = VOS_OK;
    memset_s(&dynamicParaReq, sizeof(dynamicParaReq), 0x00, sizeof(TAF_IFACE_GetDynamicParaReq));

    /* ����ID_MSG_TAF_IFACE_GET_DYNAMIC_INFO_REQ��Ϣ */
    dynamicParaReq.ctrl.moduleId = ctrl->moduleId;
    dynamicParaReq.ctrl.clientId = TAF_PS_GET_CLIENTID_FROM_EXCLIENTID(ctrl->clientId);
    dynamicParaReq.ctrl.opId     = ctrl->opId;
    dynamicParaReq.cid           = bitCid;

    /* ������Ϣ */
    result = TAF_IFACE_SndDsmMsg(TAF_PS_GET_MODEMID_FROM_EXCLIENTID(ctrl->clientId),
                                 ID_MSG_TAF_GET_IFACE_DYNAMIC_PARA_REQ, &dynamicParaReq,
                                 sizeof(TAF_IFACE_GetDynamicParaReq));

    return result;
}

