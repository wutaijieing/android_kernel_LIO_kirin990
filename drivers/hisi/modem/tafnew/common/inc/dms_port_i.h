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

#ifndef _DMS_PORT_I_H_
#define _DMS_PORT_I_H_

#include <product_config.h>
#include "vos.h"
#include "mdrv_udi.h"
#include "mdrv_hsuart.h"
#include "imm_mem_ps.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

enum DMS_PortId {
    DMS_PORT_APP,
    DMS_PORT_APP1,
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    DMS_PORT_APP2,
    DMS_PORT_APP3,
    DMS_PORT_APP4,
    DMS_PORT_APP5,
    DMS_PORT_APP6,
    DMS_PORT_APP7,
    DMS_PORT_APP8,
    DMS_PORT_APP9,
    DMS_PORT_APP10,
    DMS_PORT_APP11,
    DMS_PORT_APP12,
    DMS_PORT_APP13,
    DMS_PORT_APP14,
    DMS_PORT_APP15,
    DMS_PORT_APP16,
    DMS_PORT_APP17,
    DMS_PORT_APP18,
    DMS_PORT_APP19,
    DMS_PORT_APP20,
    DMS_PORT_APP21,
    DMS_PORT_APP22,
    DMS_PORT_APP23,
    DMS_PORT_APP24,
    DMS_PORT_APP25,
    DMS_PORT_APP26,

    DMS_PORT_APP27,
    DMS_PORT_APP28,
    DMS_PORT_APP29,
    DMS_PORT_APP30,
    DMS_PORT_APP31,
    DMS_PORT_APP32,
    DMS_PORT_APP33,
    DMS_PORT_APP34,
    DMS_PORT_APP35,
    DMS_PORT_APP36,
    DMS_PORT_APP37,
    DMS_PORT_APP38,
    DMS_PORT_APP39,
    DMS_PORT_APP40,
    DMS_PORT_APP41,
    DMS_PORT_APP42,
    DMS_PORT_APP43,
    DMS_PORT_APP44,
    DMS_PORT_APP45,
    DMS_PORT_APP46,
    DMS_PORT_APP47,
    DMS_PORT_APP48,
    DMS_PORT_APP49,
    DMS_PORT_APP50,
    DMS_PORT_APP51,
    DMS_PORT_APP52,
#endif
    // DMS_PORT_APP_MAX = DMS_PORT_PCUI - 1,

    DMS_PORT_PCUI,
    DMS_PORT_CTRL,
    DMS_PORT_PCUI2,
    DMS_PORT_NCM_CTRL,
    DMS_PORT_UART,
    DMS_PORT_SOCK,

    DMS_PORT_MODEM,

#if (FEATURE_AT_HSUART == FEATURE_ON)
    DMS_PORT_HSUART,
#endif
    DMS_PORT_NCM_DATA,

    DMS_PORT_BUTT
};
typedef VOS_UINT16 DMS_PortIdUint16;

enum DMS_ClientType {
    DMS_CLIENT_TYPE_COM,
    DMS_CLIENT_TYPE_MODEM,
#if (FEATURE_AT_HSUART == FEATURE_ON)
    DMS_CLIENT_TYPE_HSUART,
#endif
    DMS_CLIENT_TYPE_NDIS_DATA,

    DMS_CLIENT_TYPE_BUTT,
};
typedef VOS_UINT8 DMS_ClientTypeUint8;

enum DMS_ConnectStat {
    DMS_CONNECT,
    DMS_DISCONNECT,
    DMS_CONNECT_BUTT,
};
typedef VOS_UINT8 DMS_ConnectStatUint8;

struct DMS_ClientCBTbl {
    union {
        VOS_INT (*RcvData)(VOS_UINT16 clientId, VOS_UINT8 *data, VOS_UINT32 len);
        VOS_INT (*RcvPrivData)(VOS_UINT16 clientId, VOS_VOID *data);
    };

    VOS_VOID (*RcvConnectInd)(VOS_UINT16 clientId, VOS_UINT8 connState);
    VOS_VOID (*RcvMscInd)(VOS_UINT16 clientId, struct modem_msc_stru *dceMsc);
    VOS_VOID (*RcvEscapeSequence)(VOS_UINT16 clientId);
    VOS_VOID (*RcvWaterMarkInd)(VOS_UINT16 clientId, water_level level);
    VOS_VOID (*RcvConnStatusChgInd)(VOS_UINT16 clientId, enum ncm_ioctrl_connect_stus status);
    VOS_VOID (*RcvSwitchGwModeInd)(VOS_UINT16 clientId);
};

struct DMS_ClientInfo {
    VOS_UINT16                    clientId;
    DMS_ClientTypeUint8           clientType;
    VOS_UINT8                     reserv;
    const struct DMS_ClientCBTbl *clientCBTbl;
};

VOS_UINT32 DMS_PORT_RegClientInfo(DMS_PortIdUint16 portId, struct DMS_ClientInfo *clientInfo);

VOS_VOID DMS_PORT_Init(VOS_VOID);

/* 返回值：发送成功：VOS_OK；发送失败：VOS_ERR */
VOS_UINT32 DMS_PORT_SendDataSync(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT16 len);

/* 返回值：发送成功：VOS_OK；发送失败：VOS_ERR */
VOS_UINT32 DMS_PORT_SendData(DMS_PortIdUint16 portId, VOS_UINT8 *data, VOS_UINT16 len);
VOS_UINT32 DMS_PORT_SendPrivData(DMS_PortIdUint16 portId, IMM_Zc *immZc);

VOS_UINT32 DMS_PORT_SendMscCmd(DMS_PortIdUint16 portId, struct modem_msc_stru *msc);

/* ncm和uart等端口的其他属性配置API接口 */
VOS_UINT32 DMS_PORT_SetIpv6Dns(VOS_UINT16 portId, struct ncm_ipv6_dns *dns);
VOS_UINT32 DMS_PORT_ChangeFlowCtrl(VOS_UINT16 portId, VOS_UINT32 enableFlg);
VOS_UINT32 DMS_PORT_ChangeConnSpeed(VOS_UINT16 portId, struct ncm_ioctl_connection_speed *connSpeed);
VOS_UINT32 DMS_PORT_ChangeConnLinkState(VOS_UINT16 portId, VOS_UINT32 linkStatus);

#if (FEATURE_AT_HSUART == FEATURE_ON)
VOS_UINT32 DMS_PORT_ConfigFlowCtrl(VOS_UINT16 portId, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag);
VOS_UINT32 DMS_PORT_SetCharFrame(VOS_UINT16 portId, VOS_UINT32 udiDataLen, VOS_UINT32 udiStpLen, VOS_UINT32 udiParity);
VOS_UINT32 DMS_PORT_SetBaud(VOS_UINT16 portId, VOS_UINT32 baudRate);
VOS_UINT32 DMS_PORT_SetACShell(VOS_UINT16 portId, VOS_UINT32 mode);
VOS_UINT32 DMS_PORT_FlushTxData(VOS_UINT16 portId);
#endif

VOS_INT32  DMS_PORT_GetPortHandle(VOS_UINT16 portId);
VOS_UINT32 DMS_PORT_FreeUlDataBuff(VOS_UINT16 portId, VOS_VOID *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
