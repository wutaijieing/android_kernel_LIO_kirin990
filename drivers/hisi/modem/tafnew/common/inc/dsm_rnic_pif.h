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

#ifndef __DSM_RNIC_PIF_H__
#define __DSM_RNIC_PIF_H__

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#pragma pack(4)

#define DSM_RNIC_MSG_ID_HEADER 0x0000

/* 最大IPV6地址长度，不包括":" */
#define RNICITF_MAX_IPV6_ADDR_LEN 16

/*
 * 枚举说明: DSM与RNIC的消息定义
 */
enum DMS_RNIC_MsgId {
    ID_DSM_RNIC_IFACE_CFG_IND = DSM_RNIC_MSG_ID_HEADER + 0x01,
    ID_DSM_RNIC_IFACE_REL_IND = DSM_RNIC_MSG_ID_HEADER + 0x02,

    ID_DSM_RNIC_MSG_ID_ENUM_BUTT
};
typedef VOS_UINT32 DSM_RNIC_MsgIdUint32;


typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgId;

    VOS_UINT32 opIpv4PdnInfo : 1;
    VOS_UINT32 opIpv6PdnInfo : 1;
    VOS_UINT32 opSpare : 30;

    VOS_UINT8 ifaceId; /* 全局网卡ID */
    union {
        VOS_UINT8 rabId;        /* 承载号, 范围[5, 15] */
        VOS_UINT8 pduSessionId; /* PduSession ID */
    };
    ModemIdUint16 modemId; /* Modem ID */

    VOS_UINT32 ipv4Addr; /* IPV4的IP地址，主机序 */
    /* 从 PDP上下文带来的IPV6地址长度，不包括":" */
    VOS_UINT8 ipv6Addr[RNICITF_MAX_IPV6_ADDR_LEN];
} DSM_RNIC_IfaceCfgInd;


typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgId;

    VOS_UINT32 opIpv4PdnInfo : 1;
    VOS_UINT32 opIpv6PdnInfo : 1;
    VOS_UINT32 opSpare : 30;

    VOS_UINT8 ifaceId; /* 全局网卡ID */
    VOS_UINT8 rsv[3];
} DSM_RNIC_IfaceRelInd;

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* __DSM_RNIC_PIF_H__ */
