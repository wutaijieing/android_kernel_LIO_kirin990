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

#ifndef _DMS_PORT_H_
#define _DMS_PORT_H_

#include "taf_type_def.h"
#include "dms_port_i.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

enum DMS_PortType {
    DMS_PORT_TYPE_COM, /* 该port类型包含pcui、ctrl、pcui2、ndis ctrl、appvcom、uart、sock等端口 */
    DMS_PORT_TYPE_MODEM,
#if (FEATURE_AT_HSUART == FEATURE_ON)
    DMS_PORT_TYPE_HSUART,
#endif
    DMS_PORT_TYPE_NDIS_DATA,

    DMS_PORT_TYPE_BUTT,
};
typedef VOS_UINT8 DMS_PortTypeUint8;

struct DMS_PortInfo {
    DMS_PortIdUint16  portId;
    DMS_PortTypeUint8 portType;
    VOS_UINT8         reserv;

    struct DMS_ClientInfo clientInfo;
    VOS_VOID             *devInfo;
};

extern struct DMS_PortInfo g_dmsPortInfo[DMS_PORT_BUTT];

VOS_INT32 DMS_PORT_RcvData(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT32 len);
VOS_INT32 DMS_PORT_RcvPrivData(DMS_PortIdUint16 portId, VOS_VOID *data);
VOS_VOID  DMS_PORT_RcvConnStatusChgInd(DMS_PortIdUint16 portId, enum ncm_ioctrl_connect_stus status);
VOS_VOID  DMS_PORT_RcvSwitchGwModeInd(DMS_PortIdUint16 portId);
VOS_VOID  DMS_PORT_RcvMsc(DMS_PortIdUint16 portId, struct modem_msc_stru *dceMsc);
VOS_VOID  DMS_PORT_RcvEscapeSequence(DMS_PortIdUint16 portId);
VOS_VOID  DMS_PORT_RcvWaterMarkInd(DMS_PortIdUint16 portId, water_level level);
VOS_VOID  DMS_PORT_RcvConnectInd(DMS_PortIdUint16 portId, VOS_UINT8 connState);

/*lint -esym(528,DMS_PORT_GetPortInfo)*/
static inline struct DMS_PortInfo* DMS_PORT_GetPortInfo(DMS_PortIdUint16 portId)
{
    return &g_dmsPortInfo[portId];
}

/*lint -esym(528,DMS_PORT_GetClientCBTbl)*/
static inline const struct DMS_ClientCBTbl* DMS_PORT_GetClientCBTbl(DMS_PortIdUint16 portId)
{
    return g_dmsPortInfo[portId].clientInfo.clientCBTbl;
}

/*lint -esym(528,DMS_PORT_GetClientId)*/
static inline VOS_UINT16 DMS_PORT_GetClientId(DMS_PortIdUint16 portId)
{
    return g_dmsPortInfo[portId].clientInfo.clientId;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
