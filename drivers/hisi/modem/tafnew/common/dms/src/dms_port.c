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

#include "dms_debug.h"
#include "dms_port_i.h"
#include "dms_port.h"
#include "dms_dev_i.h"
#include "mn_client.h"
#include "imm_interface.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */

#define THIS_FILE_ID PS_FILE_ID_DMS_PORT_C

struct DMS_PortInfo g_dmsPortInfo[DMS_PORT_BUTT] = {{0}};

STATIC const struct DMS_PortFuncTbl g_dmsPortFuncTbl[] = {
    [DMS_PORT_TYPE_COM] = {
        {.RcvData               = DMS_PORT_RcvData,},
        .RcvConnStatusChgInd    = DMS_PORT_RcvConnStatusChgInd,
        .RcvSwitchGwModeInd     = DMS_PORT_RcvSwitchGwModeInd,
    },

    [DMS_PORT_TYPE_MODEM] = {
        {.RcvPrivData           = DMS_PORT_RcvPrivData,},
        .RcvMscInd              = DMS_PORT_RcvMsc,
        .RcvConnectInd          = DMS_PORT_RcvConnectInd,
    },
#if (FEATURE_AT_HSUART == FEATURE_ON)
    [DMS_PORT_TYPE_HSUART] = {
        {.RcvPrivData           = DMS_PORT_RcvPrivData,},
        .RcvMscInd              = DMS_PORT_RcvMsc,
        .RcvEscapeSequence      = DMS_PORT_RcvEscapeSequence,
        .RcvWaterMarkInd        = DMS_PORT_RcvWaterMarkInd,
    },
#endif
    [DMS_PORT_TYPE_NDIS_DATA] = {
        .RcvConnectInd = DMS_PORT_RcvConnectInd,
    },
};


VOS_VOID DMS_PORT_Init(VOS_VOID)
{
    VOS_VOID        *devInfo = VOS_NULL_PTR;
    DMS_PortIdUint16 portId;
    VOS_UINT32       i;

    DMS_LOGI("enter.");

    for (i = 0; i < DMS_PORT_BUTT; i++) {
        portId                  = (DMS_PortIdUint16)i;
        g_dmsPortInfo[i].portId = portId;

        switch (portId) {
            case DMS_PORT_MODEM:
                g_dmsPortInfo[i].portType = DMS_PORT_TYPE_MODEM;
                devInfo                   = (VOS_VOID *)DMS_DEV_InitCtx(portId, &g_dmsPortFuncTbl[DMS_PORT_TYPE_MODEM]);
                break;
#if (FEATURE_ON == FEATURE_AT_HSUART)
            case DMS_PORT_HSUART:
                g_dmsPortInfo[i].portType = DMS_PORT_TYPE_HSUART;
                devInfo                   = (VOS_VOID *)DMS_DEV_InitCtx(portId,
                                                                        &g_dmsPortFuncTbl[DMS_PORT_TYPE_HSUART]);
                break;
#endif
            case DMS_PORT_NCM_DATA:
                g_dmsPortInfo[i].portType = DMS_PORT_TYPE_NDIS_DATA;
                devInfo = (VOS_VOID *)DMS_DEV_InitCtx(portId, &g_dmsPortFuncTbl[DMS_PORT_TYPE_NDIS_DATA]);
                break;

            default:
                g_dmsPortInfo[i].portType = DMS_PORT_TYPE_COM;
                devInfo                   = (VOS_VOID *)DMS_DEV_InitCtx(portId, &g_dmsPortFuncTbl[DMS_PORT_TYPE_COM]);
                break;
        }

        g_dmsPortInfo[portId].devInfo = devInfo;
    }

    DMS_DEV_InitDev();
}


VOS_UINT32 DMS_PORT_RegClientInfo(DMS_PortIdUint16 portId, struct DMS_ClientInfo *clientInfo)
{
    DMS_LOGI("enter.");

    if (portId >= DMS_PORT_BUTT) {
        DMS_LOGE("portId Abnormal, %d", portId);
        return VOS_ERR;
    }

    if (clientInfo == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "clientInfo is null.");
        return VOS_ERR;
    }

    if (clientInfo->clientType >= DMS_CLIENT_TYPE_BUTT) {
        DMS_PORT_LOGE(portId, "clientType Abnormal, %d", clientInfo->clientType);
        return VOS_ERR;
    }

    if (clientInfo->clientId >= AT_CLIENT_ID_BUTT) {
        DMS_PORT_LOGE(portId, "clientId Abnormal, %d", clientInfo->clientId);
        return VOS_ERR;
    }

    if (clientInfo->clientCBTbl == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "clientCBTbl is NULL.");
        return VOS_ERR;
    }

    g_dmsPortInfo[portId].clientInfo.clientType  = clientInfo->clientType;
    g_dmsPortInfo[portId].clientInfo.clientId    = clientInfo->clientId;
    g_dmsPortInfo[portId].clientInfo.clientCBTbl = clientInfo->clientCBTbl;

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_PORT_IsPortInited(DMS_PortIdUint16 portId)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (portId >= DMS_PORT_BUTT) {
        DMS_LOGE("portId:%d is invalid.", portId);
        return VOS_FALSE;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);

    if (portInfo->portId != portId || portInfo->devInfo == VOS_NULL_PTR ||
        portInfo->clientInfo.clientCBTbl == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "Get PortId is %d .", portInfo->portId);
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


STATIC VOS_UINT32 DMS_PORT_IsInputDataParaValid(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT32 len)
{
    DMS_LOGI("enter.");

    if (portId >= DMS_PORT_BUTT || data == VOS_NULL_PTR || len == 0) {
        DMS_LOGE("portid: %d, len: %d .", portId, len);
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID DMS_PORT_OpenPort(DMS_PortIdUint16 portId)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           ret;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("open the port after port inited.");
        return;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_Open(portInfo->devInfo);

    if (ret != VOS_OK) {
        DMS_PORT_LOGE(portId, "open the port failed.");
    }
}


VOS_VOID DMS_PORT_ClosePort(DMS_PortIdUint16 portId)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           ret;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("closen the port after port inited.");
        return;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_Close(portInfo->devInfo);

    if (ret != VOS_OK) {
        DMS_PORT_LOGE(portId, "close the port failed.");
    }
}

/* DMS_PortFuncTbl相关函数 */

VOS_INT32 DMS_PORT_RcvData(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT32 len)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT32                    validFlg;
    VOS_INT32                     ret;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    validFlg = DMS_PORT_IsInputDataParaValid(portId, data, len);

    if (validFlg == VOS_FALSE) {
        DMS_LOGE("data invalid.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvData != VOS_NULL_PTR) {
        ret = clientCBTbl->RcvData(clientId, data, len);
    } else {
        DMS_PORT_LOGE(portId, "clientReadCB is null.");
        ret = VOS_ERR;
    }

    return ret;
}


VOS_INT32 DMS_PORT_RcvPrivData(DMS_PortIdUint16 portId, VOS_VOID *data)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_INT32                     ret;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    if (data == VOS_NULL_PTR) {
        DMS_LOGE("data is null.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvPrivData != VOS_NULL_PTR) {
        ret = clientCBTbl->RcvPrivData(clientId, data);
    } else {
        DMS_PORT_LOGE(portId, "RcvPrivData is null.");
        ret = VOS_ERR;
    }

    return ret;
}


VOS_VOID DMS_PORT_RcvConnStatusChgInd(DMS_PortIdUint16 portId, enum ncm_ioctrl_connect_stus status)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvConnStatusChgInd != VOS_NULL_PTR) {
        clientCBTbl->RcvConnStatusChgInd(clientId, status);
    } else {
        DMS_PORT_LOGE(portId, "clientConnStatusChgCB is null.");
    }
}


VOS_VOID DMS_PORT_RcvSwitchGwModeInd(DMS_PortIdUint16 portId)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvSwitchGwModeInd != VOS_NULL_PTR) {
        clientCBTbl->RcvSwitchGwModeInd(clientId);
    } else {
        DMS_PORT_LOGE(portId, "RcvSwitchGwModeInd is null.");
    }

    return;
}


VOS_VOID DMS_PORT_RcvMsc(DMS_PortIdUint16 portId, struct modem_msc_stru *dceMsc)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    if (dceMsc == VOS_NULL_PTR) {
        DMS_LOGE("dceMsc is null.");
        return;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvMscInd != VOS_NULL_PTR) {
        clientCBTbl->RcvMscInd(clientId, dceMsc);
    } else {
        DMS_PORT_LOGE(portId, "clientMscReadCB is null.");
    }

    return;
}

#if (FEATURE_AT_HSUART == FEATURE_ON)

VOS_VOID DMS_PORT_RcvEscapeSequence(DMS_PortIdUint16 portId)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvEscapeSequence != VOS_NULL_PTR) {
        clientCBTbl->RcvEscapeSequence(clientId);
    } else {
        DMS_PORT_LOGE(portId, "clientEscapeSequenceCB is null.");
    }

    return;
}


VOS_VOID DMS_PORT_RcvWaterMarkInd(DMS_PortIdUint16 portId, water_level level)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvWaterMarkInd != VOS_NULL_PTR) {
        clientCBTbl->RcvWaterMarkInd(clientId, level);
    } else {
        DMS_PORT_LOGE(portId, "clientWaterMarkDetectCB is null.");
    }

    return;
}
#endif  // (FEATURE_AT_HSUART == FEATURE_ON)


VOS_VOID DMS_PORT_RcvConnectInd(DMS_PortIdUint16 portId, VOS_UINT8 connState)
{
    const struct DMS_ClientCBTbl *clientCBTbl = VOS_NULL_PTR;
    VOS_UINT16                    clientId;

    DMS_LOGI("enter.");

    /* 需要关注下具体端口 */
    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return;
    }

    clientCBTbl = DMS_PORT_GetClientCBTbl(portId);
    clientId    = DMS_PORT_GetClientId(portId);

    if (clientCBTbl->RcvConnectInd == VOS_NULL_PTR) {
        DMS_PORT_LOGW(portId, "ClientConnectIndCB is null.");
    }

    /* 端口连上 */
    if (connState == DMS_CONNECT) {
        DMS_PORT_OpenPort(portId);

        if (clientCBTbl->RcvConnectInd != VOS_NULL_PTR) {
            clientCBTbl->RcvConnectInd(clientId, connState);
        }
    } else {
        if (clientCBTbl->RcvConnectInd != VOS_NULL_PTR) {
            clientCBTbl->RcvConnectInd(clientId, connState);
        }

        DMS_PORT_ClosePort(portId);
    }

    return;
}


VOS_UINT32 DMS_PORT_SendDataSync(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT16 len)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           validFlg;
    VOS_UINT32           ret;

    DMS_LOGI("enter.");

    validFlg = DMS_PORT_IsInputDataParaValid(portId, data, (VOS_UINT32)len);

    if (validFlg == VOS_FALSE) {
        DMS_LOGE("data invalid.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_SendDataSync(portInfo->devInfo, data, len);

    if (ret != VOS_OK) {
        DMS_PORT_LOGE(portId, "send fail.");
    }

    return ret;
}


STATIC VOS_UINT32 DMS_PORT_SendDataAsync(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT16 len)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           validFlg;
    VOS_UINT32           ret;

    DMS_LOGI("enter.");

    validFlg = DMS_PORT_IsInputDataParaValid(portId, data, (VOS_UINT32)len);

    if (validFlg == VOS_FALSE) {
        DMS_LOGE("data invalid.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_SendDataAsync(portInfo->devInfo, data, len);

    if (ret != VOS_OK) {
        DMS_PORT_LOGE(portId, "send fail.");
    }

    return ret;
}


VOS_UINT32 DMS_PORT_SendData(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT16 len)
{
    /* hsuart口默认走异步发送接口，hsuart的同步接口由调用方显示调用 */
    if (portId != DMS_PORT_UART) {
        return DMS_PORT_SendDataAsync(portId, data, len);
    } else {
        return DMS_PORT_SendDataSync(portId, data, len);
    }
}


VOS_UINT32 DMS_PORT_SendPrivData(DMS_PortIdUint16 portId, IMM_Zc *immZc)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           ret;

    DMS_LOGI("enter.");

    if (immZc == VOS_NULL_PTR) {
        DMS_LOGE("data is null.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        IMM_ZcFree(immZc);
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_SendPrivData(portInfo->devInfo, immZc);

    if (ret != VOS_OK) {
        DMS_PORT_LOGE(portId, "send fail.");
    }

    return ret;
}


VOS_UINT32 DMS_PORT_SendMscCmd(DMS_PortIdUint16 portId, struct modem_msc_stru *msc)
{
    struct DMS_PortInfo *port = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (msc == VOS_NULL_PTR) {
        DMS_LOGE("msc is NULL PTR.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    port = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_SendMscCmd(port->devInfo, msc);
}


VOS_UINT32 DMS_PORT_SetIpv6Dns(VOS_UINT16 portId, struct ncm_ipv6_dns *dns)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (dns == VOS_NULL_PTR) {
        DMS_LOGE("dns is NULL PTR.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_SetIpv6Dns(portInfo->devInfo, dns);
}


VOS_UINT32 DMS_PORT_ChangeFlowCtrl(VOS_UINT16 portId, VOS_UINT32 enableFlg)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_ChangeFlowCtrl(portInfo->devInfo, enableFlg);
}


VOS_UINT32 DMS_PORT_ChangeConnSpeed(VOS_UINT16 portId, struct ncm_ioctl_connection_speed *connSpeed)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (connSpeed == VOS_NULL_PTR) {
        DMS_LOGE("connSpeed is null.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_ChangeConnSpeed(portInfo->devInfo, connSpeed);
}


VOS_UINT32 DMS_PORT_ChangeConnLinkState(VOS_UINT16 portId, VOS_UINT32 linkStatus)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_ChangeConnLinkState(portInfo->devInfo, linkStatus);
}


VOS_INT32 DMS_PORT_GetPortHandle(VOS_UINT16 portId)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_INT32            handle;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERROR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    handle   = DMS_DEV_GetHandle(portInfo->devInfo);
    return handle;
}


VOS_UINT32 DMS_PORT_FreeUlDataBuff(VOS_UINT16 portId, VOS_VOID *data)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           ret;

    DMS_LOGI("enter.");

    if (data == VOS_NULL_PTR) {
        DMS_LOGE("data is null.");
        return VOS_ERR;
    }

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_FreeUlDataBuff(portInfo->devInfo, data);

    return ret;
}

#if (FEATURE_AT_HSUART == FEATURE_ON)

VOS_UINT32 DMS_PORT_ConfigFlowCtrl(VOS_UINT16 portId, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_ConfigFlowCtrl(portInfo->devInfo, rtsFlag, ctsFlag);
}


VOS_UINT32 DMS_PORT_SetCharFrame(VOS_UINT16 portId, VOS_UINT32 udiDataLen, VOS_UINT32 udiStpLen, VOS_UINT32 udiParity)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_SetCharFrame(portInfo->devInfo, udiDataLen, udiStpLen, udiParity);
}


VOS_UINT32 DMS_PORT_SetBaud(VOS_UINT16 portId, VOS_UINT32 baudRate)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_SetBaud(portInfo->devInfo, baudRate);
}


VOS_UINT32 DMS_PORT_SetACShell(VOS_UINT16 portId, VOS_UINT32 mode)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    return DMS_DEV_SetACShell(portInfo->devInfo, mode);
}


VOS_UINT32 DMS_PORT_FlushTxData(VOS_UINT16 portId)
{
    struct DMS_PortInfo *portInfo = VOS_NULL_PTR;
    VOS_UINT32           ret;

    if (DMS_PORT_IsPortInited(portId) == VOS_FALSE) {
        DMS_LOGE("port not inited.");
        return VOS_ERR;
    }

    portInfo = DMS_PORT_GetPortInfo(portId);
    ret      = DMS_DEV_FlushTxData(portInfo->devInfo);

    return ret;
}
#endif

