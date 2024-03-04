/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:
 * Create: 2012/10/20
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 and
 *  only version 2 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3) Neither the name of Huawei nor the names of its contributors may
 *     be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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

#ifndef __IPNDSERVER_H__
#define __IPNDSERVER_H__

/*
 * 1 Include Headfile
 */
#include "vos.h"
#include "PsTypeDef.h"
#include "imm_interface.h"
#include "ip_comm.h"
#include "ip_ipm_global.h"
#include "at_ndis_interface.h"
#include "LUPQueue.h"

/*
 * 1.1 Cplusplus Announce
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*
 * #pragma pack(*)    设置字节对齐方式
 */
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*
 * 2 Macro
 */
#ifdef _lint
#define NDIS_NDSERVER_PID 420
#else
#define NDIS_NDSERVER_PID UEPS_PID_NDSERVER
#endif

/* 下行IP包缓存队列长度 */
#define ND_IPV6_WAIT_ADDR_RSLT_Q_LEN (10)

/* ND SERVER最大地址信息数 */
#define IP_NDSERVER_ADDRINFO_MAX_NUM (11)

#define IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId) (NdSer_GetAddrInfoIdx(exRabId))

#define IP_NDSERVER_GET_EPSBID(index) (g_astNdServerAddrInfo[index].epsbId)

#define IP_NDSERVER_ADDRINFO_GET_ADDR(index) (&g_astNdServerAddrInfo[index])

#define IP_NDSERVER_ADDRINFO_GET_EPSID(index) (g_astNdServerAddrInfo[index].epsbId)

#define IP_NDSERVER_ADDRINFO_GET_TIMER(index) (&(g_astNdServerAddrInfo[index].timerInfo))

#define IP_NDSERVER_ADDRINFO_GET_NWPARA(index) (&(g_astNdServerAddrInfo[index].ipv6NwPara))

#define IP_NDSERVER_ADDRINFO_GET_TEINFO(index) (&(g_astNdServerAddrInfo[index].teAddrInfo))

#define IP_NDSERVER_ADDRINFO_GET_MACADDR(index) (g_astNdServerAddrInfo[index].ueMacAddr)

#define IP_NDSERVER_ADDRINFO_GET_MACFRAME(index) (g_astNdServerAddrInfo[index].macFrmHdr)

#define IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index) (g_astNdServerAddrInfo[index].dlPktQue)

#define IP_NDSERVER_GET_NDMSGDATA_ADDR() (&g_stNdMsgData)

#define IP_NDSERVER_GET_SENDMSG_BUFFER() (g_aucSendMsgBuffer)

#define IP_NDSERVER_GET_STATINFO_ADDR(index) (&g_astNdServerPktStatInfo[index])
#define IP_NDSERVER_AddRcvPktTotalNum(index) (g_astNdServerPktStatInfo[index].rcvPktTotalNum++)
#define IP_NDSERVER_AddDiscPktNum(index) (g_astNdServerPktStatInfo[index].discPktNum++)
#define IP_NDSERVER_AddRcvNsPktNum(index) (g_astNdServerPktStatInfo[index].rcvNsPktNum++)
#define IP_NDSERVER_AddRcvNaPktNum(index) (g_astNdServerPktStatInfo[index].rcvNaPktNum++)
#define IP_NDSERVER_AddRcvRsPktNum(index) (g_astNdServerPktStatInfo[index].rcvRsPktNum++)
#define IP_NDSERVER_AddRcvEchoPktNum(index) (g_astNdServerPktStatInfo[index].rcvEchoPktNum++)
#define IP_NDSERVER_AddRcvBooBigPktNum(index) (g_astNdServerPktStatInfo[index].rcvTooBigPktNum++)
#define IP_NDSERVER_AddRcvDHCPV6PktNum(index) (g_astNdServerPktStatInfo[index].rcvDhcpv6PktNum++)
#define IP_NDSERVER_AddErrNsPktNum(index) (g_astNdServerPktStatInfo[index].errNsPktNum++)
#define IP_NDSERVER_AddErrNaPktNum(index) (g_astNdServerPktStatInfo[index].errNaPktNum++)
#define IP_NDSERVER_AddErrRsPktNum(index) (g_astNdServerPktStatInfo[index].errRsPktNum++)
#define IP_NDSERVER_AddErrEchoPktNum(index) (g_astNdServerPktStatInfo[index].errEchoPktNum++)
#define IP_NDSERVER_AddErrTooBigPktNum(index) (g_astNdServerPktStatInfo[index].errTooBigPktNum++)
#define IP_NDSERVER_AddErrDhcpv6PktNum(index) (g_astNdServerPktStatInfo[index].errDhcpv6PktNum++)
#define IP_NDSERVER_AddTransPktTotalNum(index) (g_astNdServerPktStatInfo[index].transPktTotalNum++)
#define IP_NDSERVER_AddTransPktFailNum(index) (g_astNdServerPktStatInfo[index].transPktFailNum++)
#define IP_NDSERVER_AddTransNsPktNum(index) (g_astNdServerPktStatInfo[index].transNsPktNum++)
#define IP_NDSERVER_AddTransNaPktNum(index) (g_astNdServerPktStatInfo[index].transNaPktNum++)
#define IP_NDSERVER_AddTransRaPktNum(index) (g_astNdServerPktStatInfo[index].transRaPktNum++)
#define IP_NDSERVER_AddTransEchoPktNum(index) (g_astNdServerPktStatInfo[index].transEchoPktNum++)
#define IP_NDSERVER_AddTransTooBigPktNum(index) (g_astNdServerPktStatInfo[index].transTooBigPktNum++)
#define IP_NDSERVER_AddTransDhcpv6PktNum(index) (g_astNdServerPktStatInfo[index].transDhcpv6PktNum++)
#define IP_NDSERVER_AddMacInvalidPktNum(index) (g_astNdServerPktStatInfo[index].macInvalidPktNum++)
#define IP_NDSERVER_AddEnquePktNum(index) (g_astNdServerPktStatInfo[index].enquePktNum++)
#define IP_NDSERVER_AddSendQuePktNum(index) (g_astNdServerPktStatInfo[index].sendQuePktNum++)



/*
 * 4 Enum
 */
/*
 * 协议表格:
 * ASN.1描述:
 * 枚举说明: NDSERVER定时器枚举
 */
enum NDSER_Timer {
    /* ND SERVER定时器 */
    IP_ND_SERVER_TIMER_NS          = 0,
    IP_ND_SERVER_TIMER_PERIODIC_NS = 1,
    IP_ND_SERVER_TIMER_FIRST_NS    = 2,
    IP_ND_SERVER_TIMER_RA          = 3,

    IP_ND_SERVER_TIMER_BUTT
};
typedef VOS_UINT32 NDSER_TimerUint32;

/*
 * 协议表格:
 * ASN.1描述:
 * 枚举说明: TE地址状态类型
 */
enum IP_NdserverTeAddrState {
    IP_NDSERVER_TE_ADDR_INEXISTENT = 0, /* 不存在 */
    IP_NDSERVER_TE_ADDR_INCOMPLETE = 1, /* 未完成 */
    IP_NDSERVER_TE_ADDR_REACHABLE  = 2, /* 可达 */
    IP_NDSERVER_TE_ADDR_BUTT
};
typedef VOS_UINT8 IP_NdserverTeAddrStateUint8;

/*
 * 5 STRUCT
 */
/*
 * 结构名: IP_NdserverTeAddrInfo
 * 协议表格:
 * ASN.1描述:
 * 结构说明: ND SERVER保存的TE地址结构体
 */
typedef struct {
    VOS_UINT8                            teLinkLocalAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                            teGlobalAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                            teLinkLayerAddr[IP_MAC_ADDR_LEN];
    IP_NdserverTeAddrStateUint8 teAddrState;
    VOS_UINT8                            reserved[1];
} IP_NdserverTeAddrInfo;

/*
 * 结构名: IP_Ipv6Addr
 * 协议表格:
 * ASN.1描述:
 * 结构说明: IPV6地址结构体
 */
typedef struct {
    VOS_UINT32 valid;
    VOS_UINT8  teGlobalAddr[IP_IPV6_ADDR_LEN];
} IP_Ipv6Addr;

/*
 * 结构名: IP_NdserverTeDetectBuf
 * 协议表格:
 * ASN.1描述:
 * 结构说明: ND SERVER保存的TE地址结构体，用于MAC地址获取
 */
typedef struct {
    VOS_UINT32        head;
    VOS_UINT32        tail;
    VOS_UINT32        maxNum;
    IP_Ipv6Addr teIpBuf[2 * (IP_NDSERVER_ADDRINFO_MAX_NUM)];
} IP_NdserverTeDetectBuf;

/*
 * 结构名: IP_NdserverAddrInfo
 * 协议表格:
 * ASN.1描述:
 * 结构说明: ND SERVER数据结构体
 */
typedef struct {
    VOS_UINT8 epsbId;
    VOS_UINT8 validFlag;
    VOS_UINT8 reserved[14];
    /* 为了同64位操作系统兼容，放在8字节对齐位置 */
    LUP_QUEUE_STRU *dlPktQue; /* 下行包缓存队列 */
    IP_SndMsg ipSndNwMsg;

    ESM_IP_Ipv6NWPara ipv6NwPara;

    IP_NdserverTeAddrInfo teAddrInfo;

    VOS_UINT8 ueLinkLocalAddr[IP_IPV6_ADDR_LEN];

    IP_TimerInfo timerInfo;

    VOS_UINT8 ueMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8 macFrmHdr[IP_ETH_MAC_HEADER_LEN];
} IP_NdserverAddrInfo;

/*
 * 结构名: IP_NdserverPacketStatisticsInfo
 * 协议表格:
 * ASN.1描述:
 * 结构说明: ND SERVER报文统计数据结构体
 */
typedef struct {
    VOS_UINT32 rcvPktTotalNum;    /* 收到数据包总数 */
    VOS_UINT32 discPktNum;        /* 丢弃数据包数量 */
    VOS_UINT32 rcvNsPktNum;       /* 收到NS数据包数量 */
    VOS_UINT32 rcvNaPktNum;       /* 收到NA数据包数量 */
    VOS_UINT32 rcvRsPktNum;       /* 收到RS数据包数量 */
    VOS_UINT32 rcvEchoPktNum;     /* 收到PING包数量 */
    VOS_UINT32 rcvTooBigPktNum;   /* 收到IPV6超长包数量 */
    VOS_UINT32 rcvDhcpv6PktNum;   /* 收到DHCPV6包数量 */
    VOS_UINT32 errNsPktNum;       /* 错误NS数据包数量 */
    VOS_UINT32 errNaPktNum;       /* 错误NA数据包数量 */
    VOS_UINT32 errRsPktNum;       /* 错误RS数据包数量 */
    VOS_UINT32 errEchoPktNum;     /* 错误ECHO REQ数据包数量 */
    VOS_UINT32 errTooBigPktNum;   /* 错误超长包数量 */
    VOS_UINT32 errDhcpv6PktNum;   /* 错误DHCPV6数据包数量 */
    VOS_UINT32 transPktTotalNum;  /* 发送数据包总数 */
    VOS_UINT32 transPktFailNum;   /* 发送数据包失败数量 */
    VOS_UINT32 transNsPktNum;     /* 发送NS数据包数量 */
    VOS_UINT32 transNaPktNum;     /* 发送NA数据包数量 */
    VOS_UINT32 transRaPktNum;     /* 发送RA数据包数量 */
    VOS_UINT32 transEchoPktNum;   /* 发送ECHO REPLY数据包数量 */
    VOS_UINT32 transTooBigPktNum; /* 发送IPV6超长包响应数量 */
    VOS_UINT32 transDhcpv6PktNum; /* 发送DHCPV6 REPLY数据包数量 */
    VOS_UINT32 macInvalidPktNum;  /* 下行IP包发送时PC MAC地址无效的统计量 */
    VOS_UINT32 enquePktNum;       /* 下行成功缓存的IP包个数 */
    VOS_UINT32 sendQuePktNum;     /* 下行成功发送缓存的IP包个数 */
} IP_NdserverPacketStatisticsInfo;


/*
 * 7 Extern Global Variable
 */
extern IP_NdserverAddrInfo g_astNdServerAddrInfo[];

/*
 * 8 Fuction Extern
 */
extern VOS_VOID IP_NDSERVER_Init(VOS_VOID);

VOS_UINT32 NdSer_CheckIpv6PdnInfo(const AT_NDIS_Ipv6PdnInfo *ipv6PdnInfo);
VOS_VOID   NdSer_Ipv6PdnInfoCfg(VOS_UINT8 rabId, const AT_NDIS_Ipv6PdnInfo *ipv6PdnInfo);
VOS_VOID   NdSer_DhcpV6PktProc(VOS_VOID *rcvMsg);
VOS_UINT32 NdSer_GetAddrInfoIdx(VOS_UINT8 exRabId);
VOS_UINT8 *NdSer_GetMacFrm(VOS_UINT8 index, VOS_UINT8 *teAddrState);
VOS_VOID   NdSer_Ipv6PdnRel(VOS_UINT8 rabId);
VOS_UINT32 NdSer_Ipv6PdnValid(VOS_UINT8 rabId);
VOS_VOID   NdSer_NdAndEchoPktProc(VOS_VOID *rcvMsg);
VOS_UINT32 APP_NdServer_Pid_InitFunc(enum VOS_InitPhaseDefine ePhase);
VOS_VOID   IP_NDSERVER_ProcTooBigPkt(VOS_UINT8 rabId, VOS_UINT8 *srcData, VOS_UINT32 dataLen);
VOS_VOID   APP_NdServer_PidMsgProc(MsgBlock *rcvMsg);
VOS_UINT32 IP_NDSERVER_SendNsMsg(VOS_UINT32 index, const VOS_UINT8 *dstAddr);
VOS_VOID   NdSer_MacAddrInvalidProc(IMM_Zc *immZc, VOS_UINT8 index);

IP_NdserverAddrInfo *NdSer_AllocAddrInfo(VOS_UINT32 *pUlIndex);



#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of ip_nd_server.h */
