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

#ifndef _DMS_DEV_H_
#define _DMS_DEV_H_

#include "taf_type_def.h"
#include "dms_port_i.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DMS_INIT_HANDLE             0  /* 端口初始化handle值，用于初始化不需要open的端口，便于流程统一 */

#define DMS_UART_UL_DATA_BUFF_SIZE  1536
#define DMS_UART_UL_DATA_BUFF_NUM   16

#define DMS_MODEM_UL_DATA_BUFF_SIZE 1536
#define DMS_MODEM_UL_DATA_BUFF_NUM  16


#define DMS_DEV_ITEM(port_id, dev_id,dev_ops)  \
    [port_id] = { \
    .handle = UDI_INVALID_HANDLE, \
    .portId = port_id, \
    .devId = dev_id, \
    .portFuncTbl = VOS_NULL_PTR, \
    .ops = dev_ops,}

enum DMS_DevType {
    DMS_DEV_TYPE_APPVCOM,
    DMS_DEV_TYPE_ACM_COM,
    DMS_DEV_TYPE_NCM_CTRL,
    DMS_DEV_TYPE_NCM_DATA,
    DMS_DEV_TYPE_ACM_MODEM,
    DMS_DEV_TYPE_UART,
    DMS_DEV_TYPE_HSUART,
    DMS_DEV_TYPE_SOCK,

    DMS_DEV_TYPE_BUTT,
};
typedef VOS_UINT8 DMS_DevTypeUint8;

struct DMS_DevInfo {
    VOS_INT32                           handle;
    DMS_PortIdUint16                       portId;
    VOS_UINT16                          devId;

    const struct DMS_PortFuncTbl       *portFuncTbl;
    const struct DMS_DevOps            *ops;
};

struct DMS_DevOps {
    VOS_UINT32 (*Open)(struct DMS_DevInfo *dev);
    VOS_UINT32 (*Close)(struct DMS_DevInfo *dev);
    VOS_UINT32 (*SendDataSync)(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len);
    VOS_UINT32 (*SendDataAsync)(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len);
    VOS_UINT32 (*SendPrivData)(struct DMS_DevInfo *dev, IMM_Zc *data);
    VOS_UINT32 (*SendMscCmd)(struct DMS_DevInfo *dev, struct modem_msc_stru *msc);
    VOS_UINT32 (*SetIpv6Dns)(struct DMS_DevInfo *dev, struct ncm_ipv6_dns *dns);
    VOS_UINT32 (*ChangeFlowCtrl)(struct DMS_DevInfo *dev, VOS_UINT32 enableFlg);
    VOS_UINT32 (*ChangeConnSpeed)(struct DMS_DevInfo *dev, struct ncm_ioctl_connection_speed *connSpeed);
    VOS_UINT32 (*ChangeConnLinkState)(struct DMS_DevInfo *dev, VOS_UINT32 linkStatus);
    VOS_UINT32 (*FreeUlDataBuff)(struct DMS_DevInfo *dev, VOS_VOID *data);
    VOS_UINT32 (*ConfigFlowCtrl)(struct DMS_DevInfo *dev, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag);
    VOS_UINT32 (*SetCharFrame)(struct DMS_DevInfo *dev, VOS_UINT32 udiDataLen, VOS_UINT32 udiStpLen, VOS_UINT32 udiParity);
    VOS_UINT32 (*SetBaud)(struct DMS_DevInfo *dev, VOS_UINT32 baudRate);
    VOS_UINT32 (*SetACShell)(struct DMS_DevInfo *dev, VOS_UINT32 mode);
    VOS_UINT32 (*FlushTxData)(struct DMS_DevInfo *dev);
};

extern struct DMS_DevInfo g_dmsDevInfo[DMS_PORT_BUTT];


/*lint -esym(528,DMS_DEV_GetDevEntityByPortId)*/
static inline struct DMS_DevInfo* DMS_DEV_GetDevEntityByPortId(DMS_PortIdUint16 portId)
{
    return &g_dmsDevInfo[portId];
}



/*lint -esym(528,DMS_DEV_GetDevPortTblByPortId)*/
static inline const struct DMS_PortFuncTbl* DMS_DEV_GetDevPortTblByPortId(DMS_PortIdUint16 portId)
{
    return g_dmsDevInfo[portId].portFuncTbl;
}


/*lint -esym(528,DMS_DEV_GetDevHandlerByPortId)*/
static inline VOS_INT32 DMS_DEV_GetDevHandlerByPortId(DMS_PortIdUint16 portId)
{
    return g_dmsDevInfo[portId].handle;
}

extern VOS_VOID DMS_DEV_RcvConnectStateInd(DMS_PortIdUint16 portId, VOS_UINT8 connState);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

