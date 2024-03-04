/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description:
 * Create: 2012-10-10
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


/*
 * 1 Include HeadFile
 */
#include "imm_interface.h"
#include "mdrv_nvim.h"
#include "ps_ndis.h"
#include "LUPQueue.h"
#include "ipv4_dhcp_server.h"
#include "ps_common_def.h"
#include "msp_at.h"
#if (FEATURE_ON == FEATURE_LTE)
#include "msp_diag.h"
#endif
#include "vos.h"
#include "ip_nd_server.h"
#include "nv_stru_gucnas.h"
#include "acore_nv_stru_gucnas.h"
#include "ndis_drv.h"
#include "ps_iface_global_def.h"
#include "mdrv.h"
#include "ps_tag.h"
#include "securec.h"

#define THIS_MODU ps_ndis
/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_PSNDIS_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */
#define C2A_QUE_SIZE 512         /* 暂定512 */
#define USB_DATAREQ_QUE_SIZE 512 /* 暂定512 */

#define NDIS_PERIOD_ARP_TMRNAME 1
#define NDIS_ARP_REQ_TMRNAME 2

#define NDIS_ARP_FRAME_REV_OFFSET (((VOS_UINT64)(&(((ETH_ArpFrame *)0)->rev[0]))) & 0xFFFFFFFF)

/*
 * 3 function
 */

/* arp请求中间部分固定的值 */
VOS_UINT8 g_aucArpReqFixVal[ETH_ARP_FIXED_MSG_LEN] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01 };
/* arp响应中间部分固定的值 */
VOS_UINT8 g_aucArpRspFixVal[ETH_ARP_FIXED_MSG_LEN] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02 };
/* 广播地址，全1 */
VOS_UINT8 g_aucBroadCastAddr[ETH_MAC_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

VOS_UINT8 g_aucInvalidAddr[IPV4_ADDR_LEN] = {0};

/* ARP周期 */
VOS_UINT32 g_ulPeriodicArpCyc = 3000; /* 周期性ARP发送周期 */

/* 统计信息 */
NDIS_StatInfo g_stNdisStatStru        = {0};

NDIS_Entity  g_astNdisEntity[NAS_NDIS_MAX_ITEM] = {{0}};
NDIS_Entity *g_pstNdisEntity                    = g_astNdisEntity;

VOS_UINT32 g_ulNvMtu = 1500; /* IPV6 MTU默认取值 */

VOS_UINT32                g_ulNdisLomSwitch = 0;
SPE_MAC_EtherHeader g_stSpeMacHeader  = {{ 0x58, 0x02, 0x03, 0x04, 0x05, 0x06 },
                                              { 0x00, 0x11, 0x09, 0x64, 0x01, 0x01 },
                                              0x00000000 }; /* mac地址初始化为固定值 */

/*
 * 3 Function
 */
/* 声明 */
VOS_UINT32 Ndis_DlSpeSendNcm(NDIS_Entity *ndisEntity, ADS_PktTypeUint8 pktType, IMM_Zc *immZc);

extern VOS_UINT32 DIPC_Pid_InitFunc(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 DIPC_AtMsgProc(const MsgBlock *msgBlock);

extern VOS_UINT32 MUX_Pid_InitFunc(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 MUX_AtMsgProc(const MsgBlock *msgBlock);

/*
 * 功能描述: C核单独复位时NDIS的回调处理函数
 * 修改历史:
 * 1.日    期: 2019年10月10日
 *   修改内容: 新生成函数
 */
VOS_INT Ndis_ModemResetCb(drv_reset_cb_moment_e param, VOS_INT userData)
{
    VOS_UINT32  result;
    VOS_UINT32  resetCbBefore;         /* 单独复位BEFORE统计 */
    VOS_UINT32  resetCbAfter;          /* 单独复位AFTER统计 */
    if (MDRV_RESET_CB_BEFORE == param) {
        PS_PRINTF_ERR("NDIS before reset: enter.");

        /* Ndis 初始化 */
        result = Ndis_Init();
        if (PS_SUCC != result) {
            PS_PRINTF_ERR("APP_Ndis_Pid_InitFunc, Ndis_Init fail!\n");
            return VOS_ERR;
        }

        /* 复位统计之外的统计数据清0 */
        resetCbBefore = g_stNdisStatStru.resetCbBefore;
        resetCbAfter  = g_stNdisStatStru.resetCbAfter;
        (VOS_VOID)memset_s(&g_stNdisStatStru, sizeof(NDIS_StatInfo), 0x00,
                           sizeof(NDIS_StatInfo));

        /* 增加复位统计 */
        g_stNdisStatStru.resetCbBefore = resetCbBefore;
        g_stNdisStatStru.resetCbAfter  = resetCbAfter;
        NDIS_STAT_RESET_CB_BEFORE(1);

        PS_PRINTF_ERR("NDIS before reset: succ.");
        return VOS_OK;
    } else if (MDRV_RESET_CB_AFTER == param) {
        PS_PRINTF_ERR("NDIS after reset enter.");

        /* 增加复位统计 */
        NDIS_STAT_RESET_CB_AFTER(1);
        PS_PRINTF_ERR("NDIS after reset: succ.");
        return VOS_OK;
    } else {
        return VOS_ERROR;
    }
}


/*
 * 功能描述: 获取MAC地址
 * 修改历史:
 *  1.日    期: 2014年10月17日
 *    修改内容: 新生成函数
 */
VOS_UINT8 *Ndis_GetMacAddr(VOS_VOID)
{
    /* LTE协议栈MAC地址 */
    static VOS_UINT8 gUcMacAddressPstable[] = {
        0x4c, 0x54, 0x99, 0x45, 0xe5, 0xd5
    };

    return gUcMacAddressPstable;
}

/*
 * 功能描述: 发送Cnf消息到AT
 * 修改历史:
 *  1.日    期: 2011年3月16日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_SndMsgToAt(const VOS_UINT8 *buf, VOS_UINT16 msgLen, VOS_UINT32 msgId)
{
    MsgBlock        *msgBlock  = VOS_NULL_PTR;
    MSG_Header      *msgHeader = VOS_NULL_PTR;
    errno_t          rlt;

    /*lint -e516 -esym(516,*)*/
    msgBlock = (MsgBlock *)PS_ALLOC_MSG(NDIS_TASK_PID, msgLen - VOS_MSG_HEAD_LENGTH);
    /*lint -e516 +esym(516,*)*/

    if (VOS_NULL_PTR == msgBlock) {
        return PS_FAIL;
    }

    msgHeader = (MSG_Header *)(VOS_VOID *)msgBlock;

    rlt = memcpy_s(msgBlock->value, msgLen - VOS_MSG_HEAD_LENGTH, (buf + VOS_MSG_HEAD_LENGTH),
                     msgLen - VOS_MSG_HEAD_LENGTH);
    if (rlt != EOK) {
        PS_FREE_MSG(NDIS_TASK_PID, msgBlock);
        return PS_FAIL;
    }

    msgHeader->ulSenderPid   = NDIS_TASK_PID;
    msgHeader->ulReceiverPid = APP_AT_PID;
    msgHeader->msgName     = msgId;

    if (VOS_OK != PS_SEND_MSG(NDIS_TASK_PID, msgBlock)) {
        /* 异常打印 */
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*
 * 功能描述: NDIS_ADS下行数据接收回调函数
 * 修改历史:
 *  1.日    期: 2011年12月9日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月16日
 *    修改内容: DSDA特性开发，入参修改为扩展承载ID
 */
VOS_INT Ndis_DlAdsDataRcv(VOS_UINT8 exRabId, IMM_Zc *data, ADS_PktTypeUint8 pktType,
                          VOS_UINT32 exParam)
{
    if (VOS_NULL_PTR == data) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlAdsDataRcv, recv NULL PTR!");
        return PS_FAIL;
    }

    if ((PS_SUCC != Ndis_ChkRabIdValid(exRabId)) || (ADS_PKT_TYPE_BUTT <= pktType)) {
        /*lint -e522*/
        IMM_ZcFree(data);
        /*lint +e522*/
        NDIS_ERROR_LOG2(NDIS_TASK_PID, "Ndis_DlAdsDataRcv, recv RabId or PktType fail!", exRabId, pktType);
        NDIS_STAT_DL_DISCARD_ADSPKT(1);
        return PS_FAIL;
    }

    /* 增加从ADS接收到的数据包个数统计 */
    NDIS_STAT_DL_RECV_ADSPKT_SUCC(1);

    Ndis_LomTraceRcvDlData();
    if (PS_SUCC != Ndis_DlSendNcm(exRabId, pktType, data)) {
        /*lint -e522*/
        IMM_ZcFree(data);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlAdsDataRcv, Ndis_DlSendNcm fail!");
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*
 * 功能描述: NDIS_ADS下行数据接收回调函数
 * 修改历史:
 *  1.日    期: 2018年08月16日
 *    修改内容: 新生成函数
 */
VOS_INT Ndis_DlAdsDataRcvV2(unsigned long userData, IMM_Zc *data)
{
    ADS_PktTypeUint8 pktType;

    if (VOS_NULL_PTR == data) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlAdsDataRcvV2, recv NULL PTR!");
        return PS_FAIL;
    }

    if ((PS_SUCC != Ndis_ChkRabIdValid((VOS_UINT8)userData))) {
        /*lint -e522*/
        IMM_ZcFree(data);
        /*lint +e522*/
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_DlAdsDataRcvV2, recv RabId or PktType fail!", userData);
        NDIS_STAT_DL_DISCARD_ADSPKT(1);
        return PS_FAIL;
    }

    /* 增加从ADS接收到的数据包个数统计 */
    NDIS_STAT_DL_RECV_ADSPKT_SUCC(1);

    Ndis_LomTraceRcvDlData();
    if (IP_PAYLOAD == IMM_ZcGetProtocol(data)) {
        pktType = ADS_PKT_TYPE_IPV4;
    } else {
        pktType = ADS_PKT_TYPE_IPV6;
    }
    if (PS_SUCC != Ndis_DlSendNcm((VOS_UINT8)userData, pktType, data)) {
        /*lint -e522*/
        IMM_ZcFree(data);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlAdsDataRcvV2, Ndis_DlSendNcm fail!");
        return PS_FAIL;
    }

    return PS_SUCC;
}
/*
 * 功能描述: App核间USB通道
 * 修改历史:
 *  1.日    期: 2011年1月31日
 *    修改内容: 新生成函数
 */
VOS_UINT32 AppNdis_UsbReadCb(VOS_INT32 rbIdHandle, VOS_VOID *pktNode)
{
    IMM_Zc *immZc = (IMM_Zc *)pktNode; /* 目前ImmZc和sk_buff完全一致，直接强转 */

    VOS_UINT8             exRabId;
    VOS_UINT16            frameType;
    ETHFRM_Ipv4Pkt       *ipPacket = VOS_NULL_PTR;
    VOS_UINT8            *data     = VOS_NULL_PTR;
    VOS_UINT32            dataLen;

    if (VOS_NULL_PTR == immZc) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "AppNdis_UsbReadCb read NULL PTR!");
        return PS_FAIL;
    }

    data = IMM_ZcGetDataPtr(immZc);
    if (VOS_NULL_PTR == data) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, IMM_ZcGetDataPtr fail!");
        NDIS_STAT_UL_DISCARD_USBPKT(1);
        return PS_FAIL;
    }

    /* 长度异常判断 */
    dataLen = IMM_ZcGetUsedLen(immZc);
    if (dataLen < ETH_MAC_HEADER_LEN) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, ulDataLen less than ETH_MAC_HEADER_LEN!");
        NDIS_STAT_UL_DISCARD_USBPKT(1);
        return PS_FAIL;
    }

    ipPacket = (ETHFRM_Ipv4Pkt *)(VOS_VOID *)data;
    frameType = ipPacket->frameType;

    /* 这里获取的是扩展RabId */
    exRabId = Ndis_FindRabIdByHandle(rbIdHandle, frameType);

    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, Ndis_ChkRabIdValid fail!");
        NDIS_STAT_UL_DISCARD_USBPKT(1);
        return PS_FAIL;
    }

    NDIS_STAT_UL_RECV_USBPKT_SUCC(1);

    Ndis_LomTraceRcvUlData();
    Ndis_UlNcmFrmProc(exRabId, immZc);

    return PS_SUCC;
}
/*
 * 功能描述: App核间SPE通道
 * 修改历史:
 *  1.日    期: 2015年1月31日
 *    修改内容: 新生成函数
 */
VOS_UINT32 AppNdis_SpeReadCb(VOS_INT32 lSpePort, VOS_VOID *pktNode)
{
    IMM_Zc *immZc = (IMM_Zc *)pktNode; /* 目前ImmZc和sk_buff完全一致，直接强转 */

    VOS_UINT16            frameType;
    ETHFRM_Ipv4Pkt       *ipPacket = VOS_NULL_PTR;
    VOS_UINT8            *data     = VOS_NULL_PTR;
    VOS_UINT8             exRabId;
    VOS_UINT32            dataLen;

    if (VOS_NULL_PTR == immZc) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "AppNdis_UsbReadCb read NULL PTR!");
        return PS_FAIL;
    }

    data = IMM_ZcGetDataPtr(immZc);
    if (VOS_NULL_PTR == data) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, IMM_ZcGetDataPtr fail!");
        return PS_FAIL;
    }

    /* 长度异常判断 */
    dataLen = IMM_ZcGetUsedLen(immZc);
    if (dataLen < ETH_MAC_HEADER_LEN) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, ulDataLen less than ETH_MAC_HEADER_LEN!");
        return PS_FAIL;
    }

    ipPacket = (ETHFRM_Ipv4Pkt *)(VOS_VOID *)data;
    frameType = ipPacket->frameType;

    /* 这里获取的是扩展RabId */
    exRabId = Ndis_FindRabIdBySpePort(lSpePort, frameType);

    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, Ndis_ChkRabIdValid fail!");
        return PS_FAIL;
    }

    Ndis_LomTraceRcvUlData();
    Ndis_UlNcmFrmProc(exRabId, immZc);

    return PS_SUCC;
}

/*
 * 功能描述: A核NDIS读取NV项的初始化函数
 * DHCP Lease Time, 设定范围为[1, 8784]小时
 * Vodafone    24小时
 * 其他        72小时
 * 时间经过DHCP Lease Time一半时，PC会主动发起续租，
 * 如果DHCP租约超期，则从PC通过NDIS通道发往单板的数据会出现目的不可达错误
 * 目前测试，当DHCP Lease Time小于等于4S时，对于数传影响较大，所以定义最小租约为1小时
 * 目前没有遇到超过8天的DHCP Lease Time，暂定上限为8784小时(366天)
 * 修改历史:
 *  1.日    期: 2012年4月24日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_NvItemInit(VOS_VOID)
{
    VOS_UINT32                       rtn;
    VOS_UINT32                       dhcpLeaseHour;
    VOS_UINT32                       ipv6Mtu;
    NDIS_NV_DhcpLeaseHour            ndisDhcpLeaseHour = {0};
    TAF_NDIS_NvIpv6RouterMtu         ndisIPv6Mtu = {0};

    /* 从NV读取流控配置信息 */
    /*lint -e718*/
    /*lint -e732*/
    /*lint -e746*/
    rtn = Ndis_NvimItem_Read(NV_ITEM_NDIS_DHCP_DEF_LEASE_TIME, &ndisDhcpLeaseHour,
                               sizeof(NDIS_NV_DhcpLeaseHour));
    if (PS_SUCC != rtn) {
        PS_PRINTF_ERR("Ndis_NvItemInit, Fail to read NV DHCP_LEASE_TIME: %d\n", rtn);
        ndisDhcpLeaseHour.dhcpLeaseHour = TTF_NDIS_DHCP_DEFAULT_LEASE_HOUR;
    }

    /* NV值合法性判断 */
    dhcpLeaseHour = ndisDhcpLeaseHour.dhcpLeaseHour;
    if ((0 < dhcpLeaseHour) && (dhcpLeaseHour <= TTF_NDIS_DHCP_MAX_LEASE_HOUR)) {
        /* hour -> second */
        g_ulLeaseTime = dhcpLeaseHour * 3600;
    }

    /* 从NV读取IPV6 MTU信息 */
    rtn = Ndis_NvimItem_Read(NV_ITEM_IPV6_ROUTER_MTU, &ndisIPv6Mtu, sizeof(TAF_NDIS_NvIpv6RouterMtu));
    /*lint +e746*/
    /*lint +e732*/
    /*lint +e718*/

    if (PS_SUCC != rtn) {
        PS_PRINTF_ERR("Ndis_NvItemInit, Fail to read NV IPV6_MTU: %d\n!", rtn);
        ndisIPv6Mtu.ipv6RouterMtu = TTF_NDIS_IPV6_MTU_DEFAULT;
    }

    /* NV值合法性判断 */
    ipv6Mtu = ndisIPv6Mtu.ipv6RouterMtu;
    if (0 == ipv6Mtu) {
        g_ulNvMtu = TTF_NDIS_IPV6_MTU_DEFAULT;
    } else {
        g_ulNvMtu = ipv6Mtu;
    }

    return PS_SUCC;
}

/*
 * 功能描述: APP核NDIS功能的初始化函数
 * 修改历史:
 *  1.日    期: 2011年2月10日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_Init(VOS_VOID)
{
    VOS_UINT32                     loop;
    VOS_UINT8                     *macAddr        = VOS_NULL_PTR;
    VOS_UINT16                     payLoad;
    VOS_INT32                      rlt;
    VOS_UINT32                     len            = 2; /*ip payload为2个字节*/
    NDIS_ArpPeriodTimer           *arpPeriodTimer = VOS_NULL_PTR;

    /*lint -e746*/
    macAddr = (VOS_UINT8 *)Ndis_GetMacAddr(); /* 获得单板MAC地址 */
    /*lint -e746*/

    if (VOS_NULL_PTR == macAddr) {
        PS_PRINTF_ERR("Ndis_Init, Ndis_GetMacAddr Fail!\n");
        return PS_FAIL;
    }

    payLoad = IP_PAYLOAD;
    for (loop = 0; loop < NAS_NDIS_MAX_ITEM; loop++) {
        g_astNdisEntity[loop].rabType    = NDIS_RAB_NULL;
        g_astNdisEntity[loop].handle     = NDIS_INVALID_HANDLE;
        g_astNdisEntity[loop].rabId      = NDIS_INVALID_RABID;
        g_astNdisEntity[loop].used       = PS_FALSE;
        g_astNdisEntity[loop].spePort     = NDIS_INVALID_SPEPORT;
        g_astNdisEntity[loop].speIpfFlag = PS_FALSE;
        rlt = memcpy_s(g_astNdisEntity[loop].ipV4Info.macFrmHdr + ETH_MAC_ADDR_LEN, ETH_MAC_ADDR_LEN,
                         macAddr, ETH_MAC_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

        rlt = memcpy_s(g_astNdisEntity[loop].ipV4Info.macFrmHdr + (2 * ETH_MAC_ADDR_LEN), len,
                         (VOS_UINT8 *)(&payLoad), len);
        NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

        /* 周期性ARP定时器初始化 */
        arpPeriodTimer               = &(g_astNdisEntity[loop].ipV4Info.arpPeriodTimer);
        arpPeriodTimer->tm          = VOS_NULL_PTR;
        arpPeriodTimer->name       = NDIS_PERIOD_ARP_TMRNAME;
        arpPeriodTimer->timerValue = g_ulPeriodicArpCyc;
    }

    if (PS_SUCC != Ndis_NvItemInit()) { /* NV项初始化 */
        PS_PRINTF_ERR("Ndis_Init, Ndis_NvItemInit Fail!\n");
        return PS_FAIL;
    }

    return PS_SUCC;
}
/*
 * 功能描述: 下行方向的NCM数据的发送
 * 修改历史:
 *  1.日    期: 2011年2月11日
 *    修改内容: 新生成函数
 *     2.日    期: 2015年2月11日
 *    修改内容: SPE
 */
VOS_UINT32 Ndis_DlSendNcm(VOS_UINT8 exRabId, ADS_PktTypeUint8 pktType, IMM_Zc *immZc)
{
    VOS_UINT32        result;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;

    /* 使用ExRabId获取NDIS实体 */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (IP_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, NDIS_GetEntityByRabId fail!");
        return PS_FAIL;
    }

    result = Ndis_DlUsbSendNcm(exRabId, pktType, immZc);
    return result;
}

/*
 * 功能描述: 下行方向的NCM数据的发送
 * 修改历史:
 *  1.日    期: 2011年2月11日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月17日
 *    修改内容: DSDA开发，使用ExRabId获取NDIS实体
 *  3.日    期: 2014年11月17日
 *    修改内容: SPE特性，把USB和SPE分开处理
 */
VOS_UINT32 Ndis_DlUsbSendNcm(VOS_UINT8 exRabId, ADS_PktTypeUint8 pktType, IMM_Zc *immZc)
{
    VOS_UINT8         index;
    VOS_UINT8        *addData = VOS_NULL_PTR;
    VOS_INT32         handle;
    VOS_UINT32        size;
    NDIS_Entity       *ndisEntity = VOS_NULL_PTR;
    VOS_UINT8         teAddrState;
    VOS_UINT16        app;
    VOS_UINT16        tmpApp;

    /* 使用ExRabId获取NDIS实体 */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (IP_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, NDIS_GetEntityByRabId fail!");
        return PS_FAIL;
    }

    handle = ndisEntity->handle;

    /* 填充MAC帧头，调用ImmZc接口将MAC帧头填入ImmZc中 */
    if ((ADS_PKT_TYPE_IPV4 == pktType) /* 包类型枚举 */
        && (NDIS_ENTITY_IPV4 == (ndisEntity->rabType & NDIS_ENTITY_IPV4))) {
        addData = ndisEntity->ipV4Info.macFrmHdr;
    } else if ((ADS_PKT_TYPE_IPV6 == pktType) &&
               (NDIS_ENTITY_IPV6 == (ndisEntity->rabType & NDIS_ENTITY_IPV6))) {
        index = (VOS_UINT8)IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);
        if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, IP_NDSERVER_GET_ADDR_INFO_INDEX fail!");
            return PS_FAIL;
        }

        addData = NdSer_GetMacFrm(index, &teAddrState);
        if (VOS_NULL_PTR == addData) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, NdSer_GetMacFrm fail!");
            NDIS_STAT_DL_GET_IPV6MAC_FAIL(1);
            return PS_FAIL;
        }

        if (IP_NDSERVER_TE_ADDR_REACHABLE != teAddrState) {
            /* 将ucExRabId和数据包类型放入ImmZc的私有数据域中 */
            tmpApp = (pktType & 0xFF);
            app    = ((VOS_UINT16)(tmpApp << 8)) | (exRabId);
            IMM_ZcSetUserApp(immZc, app);

            NdSer_MacAddrInvalidProc(immZc, index);
            return PS_SUCC;
        }
    } else { /* 数据包类型与承载支持类型不一致 */
        NDIS_ERROR_LOG2(NDIS_TASK_PID, "Ndis_DlSendNcm, Rab is different from PktType!", ndisEntity->rabType,
                        pktType);
        NDIS_STAT_DL_PKT_DIFF_RAB_NUM(1);
        return PS_FAIL;
    }

    if (VOS_OK != IMM_ZcAddMacHead(immZc, addData)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, IMM_ZcAddMacHead fail!");
        NDIS_STAT_DL_ADDMACFRM_FAIL(1);
        return PS_FAIL;
    }

    size = IMM_ZcGetUsedLen(immZc); /* 加上以太网帧头的长度 */

    /*lint -e718*/
    if (0 != NDIS_UDI_WRITE(handle, immZc, size)) {
        NDIS_STAT_DL_SEND_USBPKT_FAIL(1);
        return PS_FAIL;
    }
    /*lint +e718*/

    NDIS_STAT_DL_SEND_USBPKT_SUCC(1);
    return PS_SUCC;
}

/*
 * 功能描述: 处理TmerMsg
 * 1.日    期: 2012年4月28日
 *   修改内容: 新生成函数
 */
VOS_VOID Ndis_ProcARPTimerExp(VOS_VOID)
{
    VOS_UINT32                  loop;
    NDIS_Entity           *ndisEntity     = VOS_NULL_PTR;
    NDIS_Ipv4Info        *ipV4Info       = VOS_NULL_PTR;
    NDIS_ArpPeriodTimer *arpPeriodTimer = VOS_NULL_PTR;

    for (loop = 0; loop < NAS_NDIS_MAX_ITEM; loop++) {
        ndisEntity = &g_astNdisEntity[loop];
        if (NDIS_ENTITY_IPV4 == (ndisEntity->rabType & NDIS_ENTITY_IPV4)) {
            ipV4Info       = &(ndisEntity->ipV4Info);
            arpPeriodTimer = &(ipV4Info->arpPeriodTimer);

            if (PS_TRUE == ipV4Info->arpInitFlg) {
#if (VOS_OS_VER != VOS_WIN32)
                Ndis_StopARPTimer(arpPeriodTimer);
#endif
            } else {
                (VOS_VOID)Ndis_SendRequestArp(&(ndisEntity->ipV4Info), ndisEntity->rabId);
            }
        }
    }

    return;
}

/*
 * 功能描述: 处理TmerMsg
 * 修改历史:
 *  1.日    期: 2012年4月28日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_ProcTmrMsg(const REL_TimerMsgBlock *rcvMsg)
{
    if (sizeof(REL_TimerMsgBlock) - VOS_MSG_HEAD_LENGTH > rcvMsg->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_ProcTmrMsg, input msg length less than struc", rcvMsg->ulLength);
        return;
    }

    switch (rcvMsg->name) {
        case NDIS_PERIOD_ARP_TMRNAME:
            Ndis_ProcARPTimerExp();
            break;
        default:
            NDIS_INFO_LOG1(NDIS_TASK_PID, "Ndis_ProcTmrMsg, Recv other Timer", rcvMsg->name);
            break;
    }

    return;
}

/*
 * 功能描述: DHCP处理
 * 修改历史:
 *  1.日    期: 2011年2月11日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_DHCPPkt_Proc(VOS_VOID *rcvMsg)
{
    ADS_NDIS_DataInd       *adsNdisMsg = (ADS_NDIS_DataInd *)rcvMsg;
    VOS_UINT8              *data       = VOS_NULL_PTR;
    NDIS_Entity            *ndisEntity = VOS_NULL_PTR;
    VOS_UINT8               exRabId;
    VOS_UINT32              pktMemLen;

    data = IMM_ZcGetDataPtr(adsNdisMsg->data);
    if (VOS_NULL_PTR == data) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DHCPPkt_Proc, IMM_ZcGetDataPtr fail!");
        return;
    }
    pktMemLen = IMM_ZcGetUsedLen(adsNdisMsg->data);

    exRabId = NDIS_FORM_EXBID(adsNdisMsg->modemId, adsNdisMsg->rabId);
    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DHCPPkt_Proc, Ndis_ChkRabIdValid fail!");
        return;
    }

    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DHCPPkt_Proc, NDIS_GetEntityByRabId fail!");
        return;
    }

    if (NDIS_ENTITY_IPV4 != (ndisEntity->rabType & NDIS_ENTITY_IPV4)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DHCPPkt_Proc, Rab not support IPV4!");
        return;
    }

    /* DHCP处理 */
    NDIS_STAT_UL_RECV_DHCPPKT(1);
    IPV4_DHCP_ProcDhcpPkt(data, exRabId, pktMemLen);

    return;
}
/*
 * 功能描述: 根据SpePort查找RabId
 * 修改历史:
 *  1.日    期: 2011年12月9日
 *    修改内容: 新生成函数
 */
VOS_UINT8 Ndis_FindRabIdBySpePort(VOS_INT32 spePort, VOS_UINT16 frameType)
{
    VOS_UINT32        loop;
    VOS_UINT8         tmpRabType;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;

    switch (frameType) {
        case ARP_PAYLOAD:
            tmpRabType = NDIS_ENTITY_IPV4; /* ARP包也经过SPE */
            break;
        case IP_PAYLOAD:
            tmpRabType = NDIS_ENTITY_IPV4;
            break;
        case IPV6_PAYLOAD:
            tmpRabType = NDIS_ENTITY_IPV6;
            break;
        default:
            NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_FindRabIdByHandle, FrameType Error!", frameType);
            return NDIS_INVALID_RABID;
    }
    for (loop = 0; loop < NAS_NDIS_MAX_ITEM; loop++) {
        ndisEntity = &g_astNdisEntity[loop];

        if ((spePort == ndisEntity->spePort) &&
            (tmpRabType == (ndisEntity->rabType & tmpRabType))) { /* 数据包类型与承载类型一致 */
            return ndisEntity->rabId;
        }
    }

    return NDIS_INVALID_RABID;
}

/*
 * 功能描述: 根据Handle查找RabId
 * 修改历史:
 *  1.日    期: 2011年12月9日
 *    修改内容: 新生成函数
 */
VOS_UINT8 Ndis_FindRabIdByHandle(VOS_INT32 rabHandle, VOS_UINT16 frameType)
{
    VOS_UINT32        loop;
    VOS_UINT8         tmpRabType;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;

    if ((ARP_PAYLOAD == frameType) || (IP_PAYLOAD == frameType)) {
        tmpRabType = NDIS_ENTITY_IPV4;
    } else if (IPV6_PAYLOAD == frameType) {
        tmpRabType = NDIS_ENTITY_IPV6;
    } else {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_FindRabIdByHandle, FrameType Error!", frameType);
        return NDIS_INVALID_RABID;
    }

    for (loop = 0; loop < NAS_NDIS_MAX_ITEM; loop++) {
        ndisEntity = &g_astNdisEntity[loop];

        if ((rabHandle == ndisEntity->handle) &&
            (tmpRabType == (ndisEntity->rabType & tmpRabType))) { /* 数据包类型与承载类型一致 */
            return ndisEntity->rabId;
        }
    }

    return NDIS_INVALID_RABID;
}

/*
 * 功能描述: 对上行NCM帧的处理
 * 修改历史:
 *  1.日    期: 2011年2月11日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月16日
 *    修改内容: DSDA开发
 */
VOS_VOID Ndis_UlNcmFrmProc(VOS_UINT8 exRabId, IMM_Zc *immZc)
{
    VOS_UINT16            frameType;
    ETHFRM_Ipv4Pkt       *ipPacket = VOS_NULL_PTR;
    ETH_Ipfixhdr         *ipFixHdr = VOS_NULL_PTR;
    VOS_UINT8            *data     = VOS_NULL_PTR;
    VOS_UINT32            dataLen;
    VOS_UINT32            ipLen;
    VOS_UINT32            ipv4Flag = 0;
    VOS_UINT32            cacheLen;

    data = IMM_ZcGetDataPtr(immZc);
    if (VOS_NULL_PTR == data) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, IMM_ZcGetDataPtr fail!");
        NDIS_STAT_UL_DISCARD_USBPKT(1);
        return;
    }

    ipPacket = (ETHFRM_Ipv4Pkt *)(VOS_VOID *)data;
    frameType = ipPacket->frameType;

    /* ARP处理 */
    if (ARP_PAYLOAD == frameType) {
        /* 长度异常判断 */
        dataLen = IMM_ZcGetUsedLen(immZc);
        /*lint -e413*/
        if (dataLen < NDIS_ARP_FRAME_REV_OFFSET) {
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, ulDataLen less than NDIS_ARP_FRAME_REV_OFFSET!");
            NDIS_STAT_UL_DISCARD_USBPKT(1);
            return;
        }

        /* ARP处理函数入参中增加RabId，后续以RabId作为Ndis实体遍历索引 */
        (VOS_VOID)Ndis_ProcArpMsg((ETH_ArpFrame *)(VOS_VOID *)ipPacket, exRabId); /*lint !e527*/

        /* 处理完ARP后调用Imm_ZcFree释放ImmZc */
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/

        return;
    }
    /* IPV6超长包处理 */
    if (IPV6_PAYLOAD == frameType) {
        dataLen = IMM_ZcGetUsedLen(immZc);

        if ((dataLen - ETH_MAC_HEADER_LEN) > g_ulNvMtu) {
            NDIS_SPE_MEM_UNMAP(immZc, dataLen);

            IP_NDSERVER_ProcTooBigPkt(exRabId, ((VOS_UINT8 *)ipPacket + ETH_MAC_HEADER_LEN), g_ulNvMtu);

            NDIS_SPE_MEM_MAP(immZc, dataLen);

            /* 调用Imm_ZcFree释放ImmZc */
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/

            return;
        }
    }

    if (IP_PAYLOAD == frameType) {
        ipv4Flag = 1;
        dataLen  = IMM_ZcGetUsedLen(immZc);
        cacheLen = (dataLen < NDIS_SPE_CACHE_HDR_SIZE) ? dataLen : NDIS_SPE_CACHE_HDR_SIZE;
    }

    /* 经MAC层过滤后剩余的IP包发送，去掉MAC帧头后递交ADS */
    if (VOS_OK != IMM_ZcRemoveMacHead(immZc)) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, IMM_ZcRemoveMacHead fail!");
        return;
    }

    /* 检查IPV4包长度和实际Skb长度，如果不一致，则修改Skb长度为实际IP包长度 */
    if (1 == ipv4Flag) {
        data = IMM_ZcGetDataPtr(immZc);
        if (VOS_NULL_PTR == data) {
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/
            return;
        }

        NDIS_SPE_MEM_UNMAP(immZc, cacheLen);

        /* 长度异常判断 */
        /*lint -e644*/
        if (dataLen < sizeof(ETH_Ipfixhdr)) {
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, ulDataLen less than size of ETH_Ipfixhdr!");
            NDIS_STAT_UL_DISCARD_USBPKT(1);
            return;
        }
        /*lint +e644*/
        ipFixHdr = (ETH_Ipfixhdr *)((VOS_VOID *)data);
        ipLen     = IP_NTOHS(ipFixHdr->totalLen);
        if (ipLen < immZc->len) {
            immZc->tail -= (immZc->len - ipLen);
            immZc->len = ipLen;
        }

        NDIS_SPE_MEM_MAP(immZc, cacheLen);
    }

/* NR版本上暂不编译该部分，待NR Ndis迭代开始后再调整该部分 */
#if (FEATURE_OFF == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    if (VOS_OK != ADS_UL_SendPacket(immZc, exRabId)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, ADS_UL_SendPacket fail!");
        return;
    }
#else
    IMM_ZcSetProtocol(immZc, frameType);
    if (VOS_OK != ads_iface_tx(exRabId, immZc)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, ads_iface_tx fail!")
        return;
    }
#endif

    NDIS_STAT_UL_SEND_ADSPKT(1);

    return;
}

/*
 * 功能描述: APP NDIS下行PID初始化函数
 * 修改历史:
 *  1.日    期: 2011年2月15日
 *    修改内容: 新生成函数
 */
VOS_UINT32 APP_Ndis_Pid_InitFunc(enum VOS_InitPhaseDefine phase)
{
    VOS_UINT32 result;

    switch (phase) {
        case VOS_IP_LOAD_CONFIG:

            result = Ndis_Init();

            if (PS_SUCC != result) {
                PS_PRINTF_ERR("APP_Ndis_Pid_InitFunc, Ndis_Init fail!\n");
                return VOS_ERR;
            }

            /* 给底软注册回调函数，用于C核单独复位的处理 */
            (VOS_VOID)mdrv_sysboot_register_reset_notify(PS_NDIS_FUNC_PROC_NAME, Ndis_ModemResetCb, 0,
                                               RESET_PRIORITY_NDIS);
            break;
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;
        default:
            break;
    }

    return VOS_OK;
}

/*
 * 功能描述: 发送以太网帧接口
 * 修改历史:
 *  1.日    期: 2011年2月14日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_SendMacFrm(const VOS_UINT8 *buf, VOS_UINT32 len, VOS_UINT8 exRabId)
{
    IMM_Zc           *immZc = VOS_NULL_PTR;
    VOS_INT32         lRtn;
    VOS_INT32         handle;
    VOS_UINT8        *pdata       = VOS_NULL_PTR;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;
    errno_t           rlt;
    if (VOS_NULL_PTR == buf) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, pucBuf is NULL!");
        return PS_FAIL;
    }

    immZc = IMM_ZcStaticAlloc(len);
    if (VOS_NULL_PTR == immZc) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, IMM_ZcStaticAlloc fail!");
        return PS_FAIL;
    }

    pdata = (VOS_UINT8 *)IMM_ZcPut(immZc, len);
    if (VOS_NULL_PTR == pdata) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, IMM_ZcPut fail!");
        return PS_FAIL;
    }

    rlt = memcpy_s(pdata, len, buf, len);
    if (rlt != EOK) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        return PS_FAIL;
    }

    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, NDIS_GetEntityByRabId fail!");
        return PS_FAIL;
    }

    handle = ndisEntity->handle;

    /* 数据发送 */
    lRtn = NDIS_UDI_WRITE(handle, immZc, len);

    if (0 != lRtn) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_SendMacFrm, udi_write fail!");
        NDIS_STAT_DL_SEND_ARPDHCPPKT_FAIL(1);
        return PS_FAIL;
    }
    return PS_SUCC;
}

/*
 * 功能描述: 处理ARP Request 帧
 * 修改历史:
 *  1.日    期: 2011年2月11日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月22日
 *  修改内容: DSDA
 */

VOS_UINT32 Ndis_ProcReqArp(ETH_ArpFrame *reqArp, VOS_UINT8 rabId)
{
    VOS_UINT32           tgtIpAddr   = reqArp->targetIP.ipAddr32bit;
    NDIS_Entity         *ndisEntity = VOS_NULL_PTR;
    NDIS_Ipv4Info       *arpV4Info  = VOS_NULL_PTR;
    errno_t              rlt;
    ndisEntity = NDIS_GetEntityByRabId(rabId);
    if (VOS_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_ProcReqArp,  NDIS_GetEntityByRabId Error!", rabId);
        NDIS_STAT_PROC_ARP_FAIL(1);
        return PS_FAIL;
    }

    arpV4Info = &ndisEntity->ipV4Info;

    if ((0 != reqArp->senderIP.ipAddr32bit) /* 兼容MAC OS 免费ARP类型,其Sender IP为0 */
        && (arpV4Info->ueIpInfo.ipAddr32bit != reqArp->senderIP.ipAddr32bit)) {
        /* 源UE IP与网侧配置不符，这种情况不处理 */
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_ProcReqArp,  SenderIP Error!", reqArp->senderIP.ipAddr);
        NDIS_STAT_PROC_ARP_FAIL(1);
        return PS_SUCC;
    }

    /* 更新PC MAC地址 */
    rlt = memcpy_s(arpV4Info->ueMacAddr, ETH_MAC_ADDR_LEN, reqArp->senderAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
    rlt = memcpy_s(arpV4Info->macFrmHdr, ETH_MAC_HEADER_LEN, reqArp->senderAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    arpV4Info->arpInitFlg = PS_TRUE;

    rlt = memcpy_s(g_stSpeMacHeader.srcAddr, ETH_MAC_ADDR_LEN, (arpV4Info->macFrmHdr + ETH_MAC_ADDR_LEN),
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    rlt = memcpy_s(g_stSpeMacHeader.dstAddr, ETH_MAC_ADDR_LEN, reqArp->srcAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);


#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
#endif

    /* 免费ARP不回复响应 */
    if ((reqArp->targetIP.ipAddr32bit == reqArp->senderIP.ipAddr32bit) || (0 == reqArp->senderIP.ipAddr32bit)) {
        return PS_SUCC;
    }

    /* 发送响应 */
    rlt = memcpy_s(reqArp->dstAddr, ETH_MAC_ADDR_LEN, reqArp->srcAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    rlt = memcpy_s(reqArp->targetAddr, ETH_MAC_ADDR_LEN, reqArp->srcAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    reqArp->targetIP.ipAddr32bit = reqArp->senderIP.ipAddr32bit;

    rlt = memcpy_s(reqArp->srcAddr, ETH_MAC_ADDR_LEN, (arpV4Info->macFrmHdr + ETH_MAC_ADDR_LEN),
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    rlt = memcpy_s(reqArp->senderAddr, ETH_MAC_ADDR_LEN, (arpV4Info->macFrmHdr + ETH_MAC_ADDR_LEN),
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    reqArp->senderIP.ipAddr32bit = tgtIpAddr;

    /* opcode */
    reqArp->opCode = ETH_ARP_RSP_TYPE;

    /* 发送ARP Reply */
    NDIS_STAT_DL_SEND_ARP_REPLY(1);
    (VOS_VOID)Ndis_SendMacFrm((VOS_UINT8 *)reqArp, sizeof(ETH_ArpFrame), rabId);

    return PS_SUCC;
}

/*
 * 功能描述: 处理ARP Reply 帧,更新PC的MAC地址
 * 修改历史:
 *  1.日    期: 2009年12月31日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月22日
 *    修改内容: DSDA
 */
VOS_UINT32 Ndis_ProcReplyArp(const ETH_ArpFrame *rspArp, VOS_UINT8 rabId)
{
    NDIS_Ipv4Info *arpV4Info  = VOS_NULL_PTR;
    VOS_UINT32     targetIP   = rspArp->targetIP.ipAddr32bit;
    NDIS_Entity   *ndisEntity;
    errno_t        rlt;
    ndisEntity = NDIS_GetEntityByRabId(rabId);
    if (VOS_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_ProcReplyArp,  NDIS_GetEntityByRabId Error!", rabId);
        return PS_FAIL;
    }

    arpV4Info = &ndisEntity->ipV4Info;

    if (targetIP == ndisEntity->ipV4Info.gwIpInfo.ipAddr32bit) {
        /* 更新PC MAC地址 */
        rlt = memcpy_s(arpV4Info->ueMacAddr, ETH_MAC_ADDR_LEN, rspArp->senderAddr, ETH_MAC_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

        rlt = memcpy_s(arpV4Info->macFrmHdr, ETH_MAC_ADDR_LEN, rspArp->senderAddr, ETH_MAC_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
        arpV4Info->arpInitFlg = PS_TRUE;

        arpV4Info->arpRepFlg = PS_TRUE;

        return PS_SUCC;
    }

    return PS_FAIL;
}

/*
 * 功能描述: IPV4 PDN信息配置
 * 修改历史:
 *  1.日    期: 2011年3月15日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_Ipv4PdnInfoCfg(const AT_NDIS_Ipv4PdnInfo *nasNdisInfo, NDIS_Entity *ndisEntity)
{
    NDIS_Ipv4Info         *ipV4Info    = &(ndisEntity->ipV4Info);
    errno_t                rlt;

    ipV4Info->arpInitFlg = PS_FALSE;
    ipV4Info->arpRepFlg  = PS_FALSE;

    rlt = memcpy_s(ipV4Info->ueIpInfo.ipAddr8bit, IPV4_ADDR_LEN, nasNdisInfo->PdnAddrInfo.ipv4AddrU8,
                     IPV4_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    rlt = memcpy_s(ipV4Info->gwIpInfo.ipAddr8bit, IPV4_ADDR_LEN, nasNdisInfo->gateWayAddrInfo.ipv4AddrU8,
                     IPV4_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    rlt = memcpy_s(ipV4Info->nmIpInfo.ipAddr8bit, IPV4_ADDR_LEN, nasNdisInfo->subnetMask.ipv4AddrU8,
                     IPV4_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* 使能则配置DNS */
    if (PS_TRUE == nasNdisInfo->opDnsPrim) {
        rlt = memcpy_s(ipV4Info->primDnsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->dnsPrimAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    } else {
        ipV4Info->primDnsAddr.ipAddr32bit = 0;
    }

    /* 使能则配置辅DNS */
    if (PS_TRUE == nasNdisInfo->opDnsSec) {
        rlt = memcpy_s(ipV4Info->secDnsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->dnsSecAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        ipV4Info->secDnsAddr.ipAddr32bit = 0;
    }

    /* 使能则配置主WINS */
    if (PS_TRUE == nasNdisInfo->opWinsPrim) {
        rlt = memcpy_s(ipV4Info->primWinsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->winsPrimAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    } else {
        ipV4Info->primWinsAddr.ipAddr32bit = 0;
    }

    /* 使能则配置辅WINS */
    if (PS_TRUE == nasNdisInfo->opWinsSec) {
        rlt = memcpy_s(ipV4Info->secWinsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->winsSecAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        ipV4Info->secWinsAddr.ipAddr32bit = 0;
    }

    /* PCSCF暂时不操作,待需求描述 */

    ipV4Info->ipAssignStatus = IPV4_DHCP_ADDR_STATUS_FREE;

    return;
}

/*
 * 功能描述: NDIS启动周期性ARP定时器
 * 修改历史:
 *  1.日    期: 2012年4月19日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_StartARPTimer(NDIS_Entity *ndisEntity)
{
#if (VOS_OS_VER != VOS_WIN32)
    VOS_UINT32                  rtn;
    NDIS_ArpPeriodTimer        *arpPeriodTimer = VOS_NULL_PTR;

    /* 入参指针判断 */
    if (VOS_NULL_PTR == ndisEntity) {
        return PS_FAIL;
    }

    /* 检查是否需要启动ARP定时器 */
    if ((NDIS_ENTITY_IPV4 != (ndisEntity->rabType & NDIS_ENTITY_IPV4)) ||
        (PS_TRUE == ndisEntity->ipV4Info.arpInitFlg)) {
        return PS_SUCC;
    }

    arpPeriodTimer = &(ndisEntity->ipV4Info.arpPeriodTimer);

    /* 如果还在运行，则停掉 */
    if (VOS_NULL_PTR != arpPeriodTimer->tm) {
        Ndis_StopARPTimer(arpPeriodTimer);
    }

    rtn = VOS_StartRelTimer(&(arpPeriodTimer->tm), PS_PID_APP_NDIS, arpPeriodTimer->timerValue,
                              arpPeriodTimer->name, 0, VOS_RELTIMER_LOOP, VOS_TIMER_PRECISION_0);
    if (VOS_OK != rtn) {
        arpPeriodTimer->tm = VOS_NULL_PTR;
        return PS_FAIL;
    }
#endif

    return PS_SUCC;
}

/*
 * 功能描述: NDIS停止周期性ARP定时器
 * 修改历史:
 *  1.日    期: 2012年4月19日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_StopARPTimer(NDIS_ArpPeriodTimer *arpPeriodTimer)
{
    if (VOS_NULL_PTR != arpPeriodTimer->tm) {
        if (VOS_OK != VOS_StopRelTimer(&(arpPeriodTimer->tm))) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_StopARPTimer, VOS_StopRelTimer fail!");
        }
        arpPeriodTimer->tm = VOS_NULL_PTR;
    }

    return;
}
#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
/*
 * 功能描述: Ndis检查ExRabId取值是否在合法范围内
 * 修改历史:
 *  1.日    期: 2018年08月21日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_ChkRabIdValid(VOS_UINT8 exRabId)
{
    /* B5000及以后的版本下，Ndis的索引为IFACE ID */
    if (exRabId >= PS_IFACE_ID_BUTT) {
        return PS_FAIL;
    }

    return PS_SUCC;
}
#if (VOS_OS_VER != VOS_WIN32)
/*
 * 功能描述: 桩函数
 * 修改历史:
 *  1.日    期: 2018年08月21日
 *    修改内容: 新生成函数
 */
VOS_UINT32 ADS_DL_RegDlDataCallback(VOS_UINT8 rabId, RCV_DL_DATA_FUNC pFunc, VOS_UINT32 exParam)
{
    return PS_SUCC;
}
#endif
#else

#if (VOS_OS_VER != VOS_WIN32)
/*
 * 功能描述: 桩函数
 * 修改历史:
 *  1.日    期: 2018年08月21日
 *    修改内容: 新生成函数
 */
int ads_iface_register_rx_handler(VOS_UINT8 ifaceId, struct ads_iface_rx_handler_s *rxHandler)
{
    return PS_SUCC;
}
#endif
/*
 * 功能描述: Ndis检查ExRabId取值是否在合法范围内
 * 修改历史:
 *  1.日    期: 2012年12月7日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月15日
 *    修改内容: DSDA特性开发:对ModemID和RabId均做检查
 */
VOS_UINT32 Ndis_ChkRabIdValid(VOS_UINT8 exRabId)
{
    VOS_UINT16 modemId;
    VOS_UINT8  rabId;

    modemId = NDIS_GET_MODEMID_FROM_EXBID(exRabId);
    if (modemId >= MODEM_ID_BUTT) {
        return PS_FAIL;
    }

    rabId = NDIS_GET_BID_FROM_EXBID(exRabId);
    if ((rabId < MIN_VAL_EPSID) || (rabId > MAX_VAL_EPSID)) {
        return PS_FAIL;
    }

    return PS_SUCC;
}
#endif

/*
 * 功能描述: 根据ExRabId查找NDIS实体
 * 修改历史:
 *  1.日    期: 2013年1月15日
 *    修改内容: 新生成函数
 */
NDIS_Entity *NDIS_GetEntityByRabId(VOS_UINT8 exRabId)
{
    VOS_UINT16 i = 0;

    /* 查询是否已存在相应Entity */
    do {
        if ((PS_TRUE == g_astNdisEntity[i].used) && (exRabId == g_astNdisEntity[i].rabId)) {
            /* 找到相应实体 */
            return &g_astNdisEntity[i];
        }

    } while ((++i) < NAS_NDIS_MAX_ITEM);

    return VOS_NULL_PTR;
}

/*
 * 功能描述: 分配一个空闲的NDIS实体
 * 修改历史:
 *  1.日    期: 2013年1月15日
 *    修改内容: 新生成函数
 */
NDIS_Entity *NDIS_AllocEntity(VOS_VOID)
{
    VOS_UINT16 i = 0;

    /* 返回第一个空闲的实体 */
    do {
        if (PS_FALSE == g_astNdisEntity[i].used) {
            /* 找到空闲实体 */
            return &g_astNdisEntity[i];
        }

    } while ((++i) < NAS_NDIS_MAX_ITEM);

    return VOS_NULL_PTR;
}

/*
 * 功能描述: PDN IPV4地址信息检查
 * 修改历史:
 *  1.日    期: 2011年12月11日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_CheckIpv4PdnInfo(const AT_NDIS_Ipv4PdnInfo *ipv4PdnInfo)
{
    if (PS_FALSE == ipv4PdnInfo->opPdnAddr) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_CheckIpv4PdnInfo,  pstIpv4PdnInfo->bitOpPdnAddr is false!");
        return PS_FAIL;
    }

    /* PDN地址和网关地址如果为全0，则也失败 */
    if (0 == VOS_MemCmp(ipv4PdnInfo->PdnAddrInfo.ipv4AddrU8, g_aucInvalidAddr, IPV4_ADDR_LEN)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_CheckIpv4PdnInfo,  stPDNAddrInfo all zero!");
        return PS_FAIL;
    }

    if (0 == VOS_MemCmp(ipv4PdnInfo->gateWayAddrInfo.ipv4AddrU8, g_aucInvalidAddr, IPV4_ADDR_LEN)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_CheckIpv4PdnInfo,  stGateWayAddrInfo all zero!");
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*
 * 功能描述: 检查参数配置参数是否合法
 * 修改历史:
 *  1.日    期: 2011年12月23日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_PdnV4PdnCfg(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo, NDIS_Entity *ndisEntity)
{
    /* IPV4地址检查 */
    if (PS_FALSE == nasNdisInfo->opIpv4PdnInfo) { /* 原语指示IPV4信息无效 */
        NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnV4PdnCfg,  bitOpIpv4PdnInfo is false!");
        return PS_FAIL;
    }

    if (PS_SUCC != Ndis_CheckIpv4PdnInfo(&(nasNdisInfo->ipv4PdnInfo))) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnV4PdnCfg,  Ndis_CheckIpv4PdnInfo fail!");
        return PS_FAIL;
    }

    Ndis_Ipv4PdnInfoCfg(&(nasNdisInfo->ipv4PdnInfo), ndisEntity);

    /* 更新NDIS实体承载属性 */
    ndisEntity->rabType |= NDIS_ENTITY_IPV4;

    return PS_SUCC;
}

/*
 * 功能描述: IPV6 PDN信息配置
 * 修改历史:
 *  1.日    期: 2011年12月23日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_PdnV6PdnCfg(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo, NDIS_Entity *ndisEntity)
{
    VOS_UINT8 exRabId;

    /* IPV6地址检查 */
    if (PS_FALSE == nasNdisInfo->opIpv6PdnInfo) { /* 原语指示IPV6信息无效 */
        NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnV6PdnCfg,  bitOpIpv6PdnInfo is false!");
        return PS_FAIL;
    }

    exRabId = NDIS_FORM_EXBID(nasNdisInfo->modemId, nasNdisInfo->rabId);

    if (PS_SUCC != NdSer_CheckIpv6PdnInfo(&(nasNdisInfo->ipv6PdnInfo))) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnV6PdnCfg,  NdSer_CheckIpv6PdnInfo fail!");
        return PS_FAIL;
    }

    /* 调ND SERVER API  配置IPV6地址信息给ND SERVER */
    NdSer_Ipv6PdnInfoCfg(exRabId, &(nasNdisInfo->ipv6PdnInfo));

    /* 更新NDIS实体属性 */
    ndisEntity->rabType |= NDIS_ENTITY_IPV6;

    return PS_SUCC;
}

/*
 * 功能描述: NDIS向AT返回的配置确认结果处理
 * 修改历史:
 *  1.日    期: 2012年4月25日
 *    修改内容: 新生成函数
 */
VOS_UINT8 Ndis_AtCnfResultProc(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo, VOS_UINT32 v4Ret,
                               VOS_UINT32 v6Ret)
{
    VOS_UINT8 result;

    /* 根据配置结果向AT返回配置CNF原语 */
    if ((PS_TRUE == nasNdisInfo->opIpv4PdnInfo) && (PS_TRUE == nasNdisInfo->opIpv6PdnInfo)) {
        if ((PS_SUCC == v4Ret) && (PS_SUCC == v6Ret)) { /* IPV4和IPV6配置都成功 */
            result = AT_NDIS_PDNCFG_CNF_SUCC;
        } else if (PS_SUCC == v4Ret) { /* 只有IPV4配置成功 */
            result = AT_NDIS_PDNCFG_CNF_IPV4ONLY_SUCC;
        } else { /* 只有IPV6配置成功 */
            result = AT_NDIS_PDNCFG_CNF_IPV6ONLY_SUCC;
        }
    } else if (PS_TRUE == nasNdisInfo->opIpv4PdnInfo) { /* 只配置了IPV4 */
        result = AT_NDIS_PDNCFG_CNF_IPV4ONLY_SUCC;
    } else { /* 只配置了IPV6 */
        result = AT_NDIS_PDNCFG_CNF_IPV6ONLY_SUCC;
    }

    return result;
}
/*
 * 功能描述: PDN地址信息参数配置
 * 修改历史:
 *  1.日    期: 2011年12月31日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月15日
 *    修改内容: DSDA特性，主要增加对接口消息中ModemId的处理
 */
VOS_VOID Ndis_PdnInfoCfgProc(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo)
{
    VOS_UINT8                   exRabId;
    VOS_INT32                   handle;
    NDIS_Entity                *ndisEntity = VOS_NULL_PTR;
    AT_NDIS_PdnInfoCfgCnf       cfgCnf;
    VOS_UINT32                  v4Ret;
    VOS_UINT32                  v6Ret;
    VOS_INT32                   lSpePort;
    VOS_UINT32                  speIpfFlag;

    NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc entered!");

    /* 长度异常检查 */
    if ((sizeof(AT_NDIS_PdnInfoCfgReq) - VOS_MSG_HEAD_LENGTH) > nasNdisInfo->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc: input msg length less than struc",
                        nasNdisInfo->msgId);
        return;
    }

    /* 生成扩展的RabId */
    exRabId    = NDIS_FORM_EXBID(nasNdisInfo->modemId, nasNdisInfo->rabId);
    handle     = nasNdisInfo->handle;
    lSpePort     = nasNdisInfo->iSpePort;
    speIpfFlag = nasNdisInfo->ipfFlag;

    cfgCnf.result  = AT_NDIS_PDNCFG_CNF_FAIL;
    cfgCnf.rabId   = nasNdisInfo->rabId;
    cfgCnf.modemId = nasNdisInfo->modemId;

    /* ExRabId参数范围有效性检查。若检查失败，则直接向AT回复配置失败 */
    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf),
                                  ID_AT_NDIS_PDNINFO_CFG_CNF);
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc,  Ndis_ChkRabIdValid fail!");
        return;
    }

    /* 如果根据ExRabId查找不到NDIS实体，则分配一个空闲的NDIS实体 */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        /* 如果分配不到空闲的NDIS实体，则返回 */
        ndisEntity = NDIS_AllocEntity();
        if (VOS_NULL_PTR == ndisEntity) {
            /* 向AT回复PDN配置失败 */
            (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf),
                                      ID_AT_NDIS_PDNINFO_CFG_CNF);
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc,  NDIS_AllocEntity failed!");
            return;
        }

        /* 该承载之前没有对应的NDIS实体，故填无效值 */
        ndisEntity->rabType    = NDIS_RAB_NULL;
        ndisEntity->handle     = NDIS_INVALID_HANDLE;
        ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
        ndisEntity->speIpfFlag = PS_FALSE;
    }

    v4Ret = Ndis_PdnV4PdnCfg(nasNdisInfo, ndisEntity);
    v6Ret = Ndis_PdnV6PdnCfg(nasNdisInfo, ndisEntity);

    if ((PS_FAIL == v6Ret) && (PS_FAIL == v4Ret)) { /* 如果IPV4和IPV6配置指示信息都无效，也认为配置失败 */
        /* 向AT回复PDN配置失败 */
        (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf),
                                  ID_AT_NDIS_PDNINFO_CFG_CNF);
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc,  Ipv4 and Ipv6 Cfg all fail!");
        return;
    }

    ndisEntity->used       = PS_TRUE;   /* 设置该NDIS实体为使用状态 */
    ndisEntity->rabId      = exRabId; /* 将扩展RabId存到对应NDIS实体中 */
    ndisEntity->handle     = handle;  /* 保存Handle到NDIS实体中 */
    ndisEntity->spePort     = lSpePort;  /* 保存SPE Port到NDIS实体中 */
    ndisEntity->speIpfFlag = speIpfFlag;

    cfgCnf.result  = Ndis_AtCnfResultProc(nasNdisInfo, v4Ret, v6Ret);
    cfgCnf.rabType = ndisEntity->rabType;

    /* 启动周期发送ARP的定时器 */
    if (PS_SUCC != Ndis_StartARPTimer(ndisEntity)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ConfigArpInfo StartTmr Failed!");
        return;
    }

    ndisEntity->spePort = NDIS_INVALID_SPEPORT;

    /* 向ADS注册下行回调:只注册一次 */
    if (VOS_OK != (ADS_DL_RegDlDataCallback(exRabId, Ndis_DlAdsDataRcv, 0))) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc, ADS_DL_RegDlDataCallback fail!");
        return;
    }

    /*lint -e718*/
    if (VOS_OK != NDIS_UDI_IOCTL(ndisEntity->handle, NCM_IOCTL_REG_UPLINK_RX_FUNC, AppNdis_UsbReadCb)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc, regist AppNdis_UsbReadCb fail!");
        return;
    }
    /*lint +e718*/

    (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf), ID_AT_NDIS_PDNINFO_CFG_CNF);

    return;
}

/*
 * 功能描述: PDN释放
 * 修改历史:
 *  1.日    期: 2011年12月7日
 *    修改内容: 用户面融合修改
 *  1.日    期: 2013年1月15日
 *    修改内容: DSDA特性开发:
 */
VOS_VOID Ndis_PdnRel(const AT_NDIS_PdnInfoRelReq *nasNdisRel)
{
    VOS_UINT8                    exRabId;
    NDIS_Entity                 *ndisEntity     = VOS_NULL_PTR;
    NDIS_ArpPeriodTimer         *arpPeriodTimer = VOS_NULL_PTR;
    AT_NDIS_PdnInfoRelCnf        relCnf;

    NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnRel entered!");

    /* 长度异常检查 */
    if ((sizeof(AT_NDIS_PdnInfoRelReq) - VOS_MSG_HEAD_LENGTH) > nasNdisRel->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_PdnRel: input msg length less than struc", nasNdisRel->msgId);
        return;
    }

    relCnf.result  = AT_NDIS_FAIL;
    relCnf.rabId   = nasNdisRel->rabId;
    relCnf.modemId = nasNdisRel->modemId;

    exRabId = NDIS_FORM_EXBID(nasNdisRel->modemId, nasNdisRel->rabId);
    if (PS_FAIL == Ndis_ChkRabIdValid(exRabId)) {
        (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&relCnf, sizeof(AT_NDIS_PdnInfoRelCnf),
                                  ID_AT_NDIS_PDNINFO_REL_CNF);
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnRel,  Ndis_ChkRabIdValid fail!");
        return;
    }

    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&relCnf, sizeof(AT_NDIS_PdnInfoRelCnf),
                                  ID_AT_NDIS_PDNINFO_REL_CNF);
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnRel,  NDIS_GetEntityByRabId failed!");
        return;
    }

    arpPeriodTimer = &(ndisEntity->ipV4Info.arpPeriodTimer);

    relCnf.rabType = ndisEntity->rabType;

    /* 如果周期性ARP定时器还在运行，则停掉 */
    Ndis_StopARPTimer(arpPeriodTimer);

    /* 调用ND SERVER API 释放该RabId对应ND SERVER实体 */
    if (NDIS_ENTITY_IPV6 == (ndisEntity->rabType & NDIS_ENTITY_IPV6)) {
        NdSer_Ipv6PdnRel(exRabId);
    }

    /* 更新该RabId对应NDIS实体为空 */
    ndisEntity->rabType    = NDIS_RAB_NULL;
    ndisEntity->rabId      = NDIS_INVALID_RABID;
    ndisEntity->handle     = NDIS_INVALID_HANDLE;
    ndisEntity->used       = PS_FALSE;
    ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
    ndisEntity->speIpfFlag = PS_FALSE;

    /* NDIS向AT回复释放确认原语 */
    relCnf.result = AT_NDIS_SUCC;
    (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&relCnf, sizeof(AT_NDIS_PdnInfoRelCnf), ID_AT_NDIS_PDNINFO_REL_CNF);

    return;
}

/*
 * 功能描述: IFACE UP CFG消息内容转成PND INFO CFG消息内容
 * 修改历史:
 *  1.日    期: 2018年08月16日
 *    修改内容: 新增
 */

VOS_VOID Ndis_ATIfaceUpCfgTransToPdnInfoCfg(const AT_NDIS_IfaceUpConfigInd *ndisIFaceInfo,
                                            AT_NDIS_PdnInfoCfgReq *ndisPdnInfo)
{
    errno_t rlt;
    (VOS_VOID)memset_s((VOS_UINT8 *)ndisPdnInfo, sizeof(AT_NDIS_PdnInfoCfgReq), 0,
                       sizeof(AT_NDIS_PdnInfoCfgReq));

    ndisPdnInfo->msgId          = ndisIFaceInfo->msgId;
    ndisPdnInfo->opIpv4PdnInfo = ndisIFaceInfo->opIpv4PdnInfo;
    ndisPdnInfo->opIpv6PdnInfo = ndisIFaceInfo->opIpv6PdnInfo;
    ndisPdnInfo->modemId        = NDIS_GET_MODEMID_FROM_EXBID(ndisIFaceInfo->iFaceId); /* 将Iface ID转成ModemId+RabId */
    ndisPdnInfo->rabId = NDIS_GET_BID_FROM_EXBID(ndisIFaceInfo->iFaceId); /* 将Iface ID转成ModemId+RabId */
    ndisPdnInfo->handle = ndisIFaceInfo->handle;
    rlt = memcpy_s(&ndisPdnInfo->ipv4PdnInfo, sizeof(AT_NDIS_Ipv4PdnInfo),
                     &ndisIFaceInfo->ipv4PdnInfo, sizeof(AT_NDIS_Ipv4PdnInfo));
    NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    rlt = memcpy_s(&ndisPdnInfo->ipv6PdnInfo, sizeof(AT_NDIS_Ipv6PdnInfo),
                     &ndisIFaceInfo->ipv6PdnInfo, sizeof(AT_NDIS_Ipv6PdnInfo));
    NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    return;
}

/*
 * 功能描述: IFACE UP消息处理
 * 修改历史:
 *  1.日    期: 2018年08月16日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_IfaceUpCfgProc(const AT_NDIS_IfaceUpConfigInd *ifacInfo)
{
    VOS_UINT8                     exRabId;
    VOS_INT32                     handle;
    NDIS_Entity                  *ndisEntity = VOS_NULL_PTR;
    VOS_UINT32                    v4Ret;
    VOS_UINT32                    v6Ret;
    AT_NDIS_PdnInfoCfgReq         pdnInfo;
    struct ads_iface_rx_handler_s ifaceRxHandle;

    NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc entered!");

    /* 长度异常检查 */
    if ((sizeof(AT_NDIS_IfaceUpConfigInd) - VOS_MSG_HEAD_LENGTH) > ifacInfo->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc: input msg length less than struc", ifacInfo->msgId);
        return;
    }

    /* 生成扩展的RabId */
    exRabId = ifacInfo->iFaceId;
    handle  = ifacInfo->handle;

    /* ExRabId参数范围有效性检查。若检查失败，则直接向AT回复配置失败 */
    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc,  Ndis_ChkRabIdValid fail!");
        return;
    }

    /* 如果根据ExRabId查找不到NDIS实体，则分配一个空闲的NDIS实体 */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        /* 如果分配不到空闲的NDIS实体，则返回 */
        ndisEntity = NDIS_AllocEntity();
        if (VOS_NULL_PTR == ndisEntity) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc,  NDIS_AllocEntity failed!");
            return;
        }

        /* 该承载之前没有对应的NDIS实体，故填无效值 */
        ndisEntity->rabType    = NDIS_RAB_NULL;
        ndisEntity->handle     = NDIS_INVALID_HANDLE;
        ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
        ndisEntity->speIpfFlag = PS_FALSE;
    }

    Ndis_ATIfaceUpCfgTransToPdnInfoCfg(ifacInfo, &pdnInfo);
    v4Ret = Ndis_PdnV4PdnCfg(&pdnInfo, ndisEntity);
    v6Ret = Ndis_PdnV6PdnCfg(&pdnInfo, ndisEntity);

    if ((PS_FAIL == v6Ret) && (PS_FAIL == v4Ret)) { /* 如果IPV4和IPV6配置指示信息都无效，也认为配置失败 */
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc,  Ipv4 and Ipv6 Cfg all fail!");
        return;
    }

    ndisEntity->used       = PS_TRUE;   /* 设置该NDIS实体为使用状态 */
    ndisEntity->rabId      = exRabId; /* 将扩展RabId存到对应NDIS实体中 */
    ndisEntity->handle     = handle;  /* 保存Handle到NDIS实体中 */
    ndisEntity->spePort     = 0;         /* 保存SPE Port到NDIS实体中 */
    ndisEntity->speIpfFlag = 0;

    /* 启动周期发送ARP的定时器 */
    if (PS_SUCC != Ndis_StartARPTimer(ndisEntity)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc StartTmr Failed!");
        return;
    }

    ndisEntity->spePort = NDIS_INVALID_SPEPORT;

    (VOS_VOID)memset_s(&ifaceRxHandle, sizeof(ifaceRxHandle), 0, sizeof(ifaceRxHandle));
    ifaceRxHandle.user_data     = exRabId;
    ifaceRxHandle.rx_func       = Ndis_DlAdsDataRcvV2;
    ifaceRxHandle.rx_cmplt_func = VOS_NULL_PTR;

    /* 向ADS注册下行回调 */
    if (VOS_OK != ads_iface_register_rx_handler(exRabId, &ifaceRxHandle)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc, ADS_DL_RegDlDataCallback fail!");
        return;
    }

    /*lint -e718*/
    if (VOS_OK != NDIS_UDI_IOCTL(ndisEntity->handle, NCM_IOCTL_REG_UPLINK_RX_FUNC, AppNdis_UsbReadCb)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc, regist AppNdis_UsbReadCb fail!");
        return;
    }
    /*lint +e718*/

#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
#endif

    return;
}

/*
 * 功能描述: IFACE DOWN消息处理
 * 修改历史:
 *  1.日    期: 2018年08月16日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_IfaceDownCfgProc(const AT_NDIS_IfaceDownConfigInd *ifacInfo)

{
    VOS_UINT8            exRabId;
    NDIS_Entity         *ndisEntity     = VOS_NULL_PTR;
    NDIS_ArpPeriodTimer *arpPeriodTimer = VOS_NULL_PTR;

    NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_IfaceDownCfgProc entered!");

    /* 长度异常检查 */
    if ((sizeof(AT_NDIS_IfaceDownConfigInd) - VOS_MSG_HEAD_LENGTH) > ifacInfo->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_IfaceDownCfgProc: input msg length less than struc", ifacInfo->msgId);
        return;
    }

    exRabId = ifacInfo->iFaceId;
    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "pstIfacInfo,  Ndis_ChkRabIdValid fail!");
        return;
    }

    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "pstIfacInfo,  NDIS_GetEntityByRabId failed!");
        return;
    }

    arpPeriodTimer = &(ndisEntity->ipV4Info.arpPeriodTimer);

    /* 如果周期性ARP定时器还在运行，则停掉 */
    Ndis_StopARPTimer(arpPeriodTimer);

    /* 调用ND SERVER API 释放该RabId对应ND SERVER实体 */
    if (NDIS_ENTITY_IPV6 == (ndisEntity->rabType & NDIS_ENTITY_IPV6)) {
        NdSer_Ipv6PdnRel(exRabId);
    }

    /* 更新该RabId对应NDIS实体为空 */
    ndisEntity->rabType    = NDIS_RAB_NULL;
    ndisEntity->rabId      = NDIS_INVALID_RABID;
    ndisEntity->handle     = NDIS_INVALID_HANDLE;
    ndisEntity->used       = PS_FALSE;
    ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
    ndisEntity->speIpfFlag = PS_FALSE;

    return;
}

/* NDIS ARP PROC Begin */
/*
 * 功能描述: 发送ARP Request 帧到Ethenet上
 * 修改历史:
 *  1.日    期: 2009年12月31日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_SendRequestArp(NDIS_Ipv4Info *arpInfoItem, VOS_UINT8 exRabId)
{
    ETH_ArpFrame arpReq;
    errno_t      rlt;
    /* 之前一次发送的Req尚未受到Reply反馈 */
    if (PS_FALSE == arpInfoItem->arpRepFlg) {
        NDIS_STAT_ARPREPLY_NOTRECV(1);
        /* 做一次告警日志 */
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)&arpReq, sizeof(ETH_ArpFrame), 0, sizeof(ETH_ArpFrame));

    /* 组ARP Request */
    rlt = memcpy_s(arpReq.dstAddr, ETH_MAC_ADDR_LEN, g_aucBroadCastAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    rlt = memcpy_s(arpReq.srcAddr, ETH_MAC_ADDR_LEN, arpInfoItem->macFrmHdr + ETH_MAC_ADDR_LEN,
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
    arpReq.frameType = ARP_PAYLOAD;

    /* 请求的固定部分 */
    rlt = memcpy_s(((VOS_UINT8 *)&arpReq + ETH_MAC_HEADER_LEN), ETH_ARP_FIXED_MSG_LEN, g_aucArpReqFixVal,
                     ETH_ARP_FIXED_MSG_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    /* Payload部分的MAC地址设置 */
    (VOS_VOID)memset_s(arpReq.targetAddr, ETH_MAC_ADDR_LEN, 0, ETH_MAC_ADDR_LEN);
    rlt = memcpy_s(arpReq.senderAddr, ETH_MAC_ADDR_LEN, arpInfoItem->macFrmHdr + ETH_MAC_ADDR_LEN,
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    /* 单板IP */
    arpReq.senderIP.ipAddr32bit = arpInfoItem->gwIpInfo.ipAddr32bit;
    arpReq.targetIP.ipAddr32bit = arpInfoItem->ueIpInfo.ipAddr32bit;

    if (PS_SUCC != Ndis_SendMacFrm((VOS_UINT8 *)&arpReq, sizeof(ETH_ArpFrame), exRabId)) {
        arpInfoItem->arpRepFlg = PS_TRUE;
        NDIS_STAT_DL_SEND_ARP_REQUEST_FAIL(1);
        return PS_FAIL;
    }

    NDIS_STAT_DL_SEND_ARP_REQUEST_SUCC(1);

    arpInfoItem->arpRepFlg = PS_FALSE;

    return PS_SUCC;
}

/*
 * 功能描述: 处理底软发送上来的ARP帧
 * 修改历史:
 *  1.日    期: 2009年12月31日
 *    修改内容: 新生成函数
 */
VOS_UINT32 Ndis_ProcArpMsg(ETH_ArpFrame *arpMsg, VOS_UINT8 rabId)
{
    /* Arp Request */
    if (0 == VOS_MemCmp((VOS_UINT8 *)arpMsg + ETH_MAC_HEADER_LEN, g_aucArpReqFixVal, ETH_ARP_FIXED_MSG_LEN)) {
        NDIS_STAT_UL_RECV_ARP_REQUEST(1);
        return Ndis_ProcReqArp(arpMsg, rabId);
    }
    /* ARP Reply */
    else if (0 == VOS_MemCmp((VOS_UINT8 *)arpMsg + ETH_MAC_HEADER_LEN, g_aucArpRspFixVal, ETH_ARP_FIXED_MSG_LEN)) {
        NDIS_STAT_DL_RECV_ARP_REPLY(1);
        return Ndis_ProcReplyArp(arpMsg, rabId);
    } else {
        NDIS_STAT_PROC_ARP_FAIL(1);
        return PS_FAIL;
    }
}

/*
 * 修改历史:
 * 1.日    期: 2011年3月16日
 *   修改内容: 新生成函数
 */
VOS_VOID Ndis_AtMsgProc(const MsgBlock *msgBlock)
{
    AT_NDIS_MsgIdUint32 msgId;

    COVERITY_TAINTED_SET(msgBlock->value); /* 鹰眼插桩 */

    /* 长度异常保护 */
    if (sizeof(MSG_Header) - VOS_MSG_HEAD_LENGTH > msgBlock->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_AtMsgProc: input msg length less than struc MSG_Header",
                        msgBlock->ulLength);
        return;
    }

    msgId = ((MSG_Header *)(VOS_VOID *)msgBlock)->msgName;

    switch (msgId) {
        case ID_AT_NDIS_PDNINFO_CFG_REQ: /* 根据消息的不同处理AT不同的请求 */
            Ndis_PdnInfoCfgProc((AT_NDIS_PdnInfoCfgReq *)(VOS_VOID *)msgBlock);
            break;

        case ID_AT_NDIS_PDNINFO_REL_REQ:
            Ndis_PdnRel((AT_NDIS_PdnInfoRelReq *)(VOS_VOID *)msgBlock);
            break;

        case ID_AT_NDIS_IFACE_UP_CONFIG_IND:
            Ndis_IfaceUpCfgProc((AT_NDIS_IfaceUpConfigInd *)(VOS_VOID *)msgBlock);
            break;

        case ID_AT_NDIS_IFACE_DOWN_CONFIG_IND:
            Ndis_IfaceDownCfgProc((AT_NDIS_IfaceDownConfigInd *)(VOS_VOID *)msgBlock);
            break;

        default:
            NDIS_WARNING_LOG(NDIS_TASK_PID, "Ndis_AtMsgProc:Error Msg!");
            break;
    }

    return;
}

/*
 * 功能描述: NDIS接收ADS消息处理函数
 * 修改历史:
 *  1.日    期: 2011年12月15日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_AdsMsgProc(const MsgBlock *msgBlock)
{
    ADS_NDIS_DataInd *adsNdisMsg = (ADS_NDIS_DataInd *)(VOS_VOID *)msgBlock;

    if (sizeof(ADS_NDIS_DataInd) - VOS_MSG_HEAD_LENGTH > msgBlock->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgProc, input msg length less than struc", msgBlock->ulLength);
        return;
    }
    if (VOS_NULL_PTR == adsNdisMsg->data) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_AdsMsgProc recv NULL PTR!");
        return;
    }

    if (ID_ADS_NDIS_DATA_IND != adsNdisMsg->msgId) {
        /*lint -e522*/
        IMM_ZcFree(adsNdisMsg->data);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_AdsMsgProc, MsgId error!");
        return;
    }

    switch (adsNdisMsg->ipPacketType) {
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV4: /* DHCP包 */
            Ndis_DHCPPkt_Proc(adsNdisMsg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV6: /* DHCPV6包 */
            NdSer_DhcpV6PktProc(adsNdisMsg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_ICMPV6: /* ND和ECHO REQUEST包 */
            NdSer_NdAndEchoPktProc(adsNdisMsg);
            break;

        default:
            NDIS_WARNING_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgProc:Other Msg!", adsNdisMsg->ipPacketType);
            break;
    }

    /* 处理完成后释放ImmZc */
    /*lint -e522*/
    IMM_ZcFree(adsNdisMsg->data);
    /*lint +e522*/

    return;
}

/*
 * 功能描述: ADS V2消息内容转成V1格式
 * 修改历史:
 *  1.日    期: 2018年08月16日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_AdsV2MsgTransToV1Msg(ADS_NDIS_DataIndV2 *v2Msg, ADS_NDIS_DataInd *v1Msg)
{
    (VOS_VOID)memset_s(v1Msg, sizeof(ADS_NDIS_DataInd), 0, sizeof(ADS_NDIS_DataInd));

    v1Msg->msgId        = ID_ADS_NDIS_DATA_IND;
    v1Msg->modemId      = NDIS_GET_MODEMID_FROM_EXBID(v2Msg->ifaceId);
    v1Msg->rabId        = NDIS_GET_BID_FROM_EXBID(v2Msg->ifaceId);
    v1Msg->ipPacketType = v2Msg->ipPacketType;
    v1Msg->data         = v2Msg->data;

    return;
}

/*
 * 功能描述: NDIS接收ADS消息处理函数
 * 修改历史:
 *  1.日    期: 2011年12月15日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_AdsMsgProcV2(const MsgBlock *msgBlock)
{
    ADS_NDIS_DataIndV2        *adsNdisMsg = (ADS_NDIS_DataIndV2 *)(VOS_VOID *)msgBlock;
    ADS_NDIS_DataInd           adsNdisV1Msg;

    if (sizeof(ADS_NDIS_DataIndV2) - VOS_MSG_HEAD_LENGTH > msgBlock->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgProcV2, input msg length less than struc", msgBlock->ulLength);
        return;
    }
    if (VOS_NULL_PTR == adsNdisMsg->data) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_AdsMsgProcV2 recv NULL PTR!");
        return;
    }

    Ndis_AdsV2MsgTransToV1Msg(adsNdisMsg, &adsNdisV1Msg);

    switch (adsNdisV1Msg.ipPacketType) {
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV4: /* DHCP包 */
            Ndis_DHCPPkt_Proc(&adsNdisV1Msg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV6: /* DHCPV6包 */
            NdSer_DhcpV6PktProc(&adsNdisV1Msg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_ICMPV6: /* ND和ECHO REQUEST包 */
            NdSer_NdAndEchoPktProc(&adsNdisV1Msg);
            break;

        default:
            NDIS_WARNING_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgProcV2:Other Msg!", adsNdisV1Msg.ipPacketType);
            break;
    }

    /* 处理完成后释放ImmZc */
    /*lint -e522*/
    IMM_ZcFree(adsNdisMsg->data);
    /*lint +e522*/

    return;
}

/*
 * 功能描述: NDIS接收ADS消息处理函数
 * 修改历史:
 *  1.日    期: 2018年08月16日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_AdsMsgDispatch(const MsgBlock *msgBlock)
{
    AT_NDIS_MsgIdUint32 msgId;

    COVERITY_TAINTED_SET(msgBlock->value); /* 鹰眼插桩 */

    /* 长度异常保护 */
    if (sizeof(MSG_Header) - VOS_MSG_HEAD_LENGTH > msgBlock->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgDispatch: input msg length less than struc MSG_Header",
                        msgBlock->ulLength);
        return;
    }

    msgId = ((MSG_Header *)(VOS_VOID *)msgBlock)->msgName;

    switch (msgId) {
        case ID_ADS_NDIS_DATA_IND: /* 根据消息的不同处理AT不同的请求 */
            Ndis_AdsMsgProc(msgBlock);
            break;

        case ID_ADS_NDIS_DATA_IND_V2:
            Ndis_AdsMsgProcV2(msgBlock);
            break;

        default:
            NDIS_WARNING_LOG(NDIS_TASK_PID, "Ndis_AdsMsgDispatch:Error Msg!");
            break;
    }

    return;
}

/*
 * 功能描述: NDIS接收各模块消息处理函数
 * 修改历史:
 *  1.日    期: 2011年2月15日
 *    修改内容: 新生成函数
 */
VOS_VOID APP_Ndis_PidMsgProc(MsgBlock *msgBlock)
{
    if (VOS_NULL_PTR == msgBlock) {
        PS_PRINTF_INFO("Error:APP_Ndis_DLPidMsgProc Parameter pRcvMsg is NULL!");
        return;
    }

    switch (msgBlock->ulSenderPid) {
        case DOPRA_PID_TIMER:
            /*lint -e826*/
            Ndis_ProcTmrMsg((REL_TimerMsgBlock *)msgBlock);
            /*lint +e826*/
            break;

        case APP_AT_PID:
            Ndis_AtMsgProc(msgBlock);
            break;

        case ACPU_PID_ADS_UL: /* ADS通过OSA消息发送DHCP和ND SERVER包给NDIS模块 */
            Ndis_AdsMsgDispatch(msgBlock);
            break;

        default:
            NDIS_WARNING_LOG(NDIS_TASK_PID, "Warning:APP_Ndis_PidMsgProc Recv not expected msg!");
            break;
    }

    return;
}

/*lint -e40*/
/*
 * 功能描述: NDIS的FID初始化函数
 * 修改历史:
 *  1.日    期: 2008年9月17日
 *    修改内容: 新生成函数
 */
VOS_UINT32 APP_NDIS_FidInit(enum VOS_InitPhaseDefine phase)
{
    VOS_UINT32 result;

    switch (phase) {
        case VOS_IP_LOAD_CONFIG:

            /* 注册NDIS PID */
            result = VOS_RegisterPIDInfo(NDIS_TASK_PID, (InitFunType)APP_Ndis_Pid_InitFunc,
                                           (MsgFunType)APP_Ndis_PidMsgProc);
            if (VOS_OK != result) {
                PS_PRINTF_ERR("APP_NDIS_FidInit, register NDIS PID fail!\n");
                return VOS_ERR;
            }

            /* 注册ND SERVER PID */
            result = VOS_RegisterPIDInfo(NDIS_NDSERVER_PID, (InitFunType)APP_NdServer_Pid_InitFunc,
                                           (MsgFunType)APP_NdServer_PidMsgProc);
            if (VOS_OK != result) {
                PS_PRINTF_ERR("APP_NDIS_FidInit, register NDSERVER PID fail!\n");
                return VOS_ERR;
            }

            result = VOS_RegisterMsgTaskPrio(NDIS_TASK_FID, VOS_PRIORITY_P4);
            if (VOS_OK != result) {
                PS_PRINTF_ERR("APP_NDIS_FidInit, register priority fail!\n");
                return VOS_ERR;
            }
            break;
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        case VOS_IP_BUTT:
            break;
        default:
            break;
    }

    return PS_SUCC;
}
/*lint +e40*/

#if (FEATURE_ON == FEATURE_LTE)

VOS_UINT32 Ndis_MsgHook(VOS_UINT8 *data, VOS_UINT32 length, AT_NDIS_MsgIdUint32 msgId)
{
    mdrv_diag_trace_report((VOS_VOID *)data);
    return VOS_OK;
}
#endif
VOS_UINT32 g_ulGUNdisOMSwitch = PS_FALSE;
VOS_VOID   GU_NDIS_OM_SWITCH_ON(VOS_VOID)
{
    g_ulGUNdisOMSwitch = PS_TRUE;
    return;
}

VOS_VOID GU_NDIS_OM_SWITCH_OFF(VOS_VOID)
{
    g_ulGUNdisOMSwitch = PS_FALSE;
    return;
}

/* ======================================统计信息============================== */
/*
 * 功能描述: 显示收发的业务数据信息
 *  修改历史:
 *   1.日    期: 2009年6月16日
 *     修改内容: 新生成函数
 * 修改历史:
 *   2.日    期: 2013年1月22日
 *     修改内容: DSDA
 */
VOS_VOID Ndis_ShowStat(VOS_VOID)
{
    PS_PRINTF_ERR("uplink discard packets num           %d\n", g_stNdisStatStru.dicardUsbFrmNum);
    PS_PRINTF_ERR("uplink packets recevied from USB     %d\n", g_stNdisStatStru.recvUsbPktSuccNum);
    PS_PRINTF_ERR("uplink packets sent to ADS           %d\n", g_stNdisStatStru.sendPktToAdsSucNum);
    PS_PRINTF_ERR("downlink discarded ADS packets       %d\n", g_stNdisStatStru.dicardAdsPktNum);
    PS_PRINTF_ERR("downlink received ADS succ           %d\n", g_stNdisStatStru.recvAdsPktSuccNum);
    PS_PRINTF_ERR("downlink get IPV6 MAC fail           %d\n", g_stNdisStatStru.getIpv6MacFailNum);
    PS_PRINTF_ERR("downlink packet diff with Rab        %d\n", g_stNdisStatStru.dlPktDiffRabNum);
    PS_PRINTF_ERR("downlink add MAC head fail           %d\n", g_stNdisStatStru.addMacHdrFailNum);
    PS_PRINTF_ERR("downlink send packet fail            %d\n", g_stNdisStatStru.dlSendPktFailNum);
    PS_PRINTF_ERR("downlink send packet succ            %d\n", g_stNdisStatStru.dlSendPktSuccNum);

    PS_PRINTF_ERR("\nrecv DHCP                          %d\n", g_stNdisStatStru.recvDhcpPktNum);
    PS_PRINTF_ERR("recv ARP Request                     %d\n", g_stNdisStatStru.recvArpReq);
    PS_PRINTF_ERR("recv ARP Reply                       %d\n", g_stNdisStatStru.recvArpReply);
    PS_PRINTF_ERR("proc error ARP packets               %d\n", g_stNdisStatStru.procArpError);
    PS_PRINTF_ERR("send ARP Req succ                    %d\n", g_stNdisStatStru.sendArpReqSucc);
    PS_PRINTF_ERR("send ARP Req fail                    %d\n", g_stNdisStatStru.sendArpReqFail);
    PS_PRINTF_ERR("send ARP Req No Reply                %d\n", g_stNdisStatStru.arpReplyNotRecv);
    PS_PRINTF_ERR("send ARP Reply                       %d\n", g_stNdisStatStru.sendArpReply);
    PS_PRINTF_ERR("send ARP or DHCP or ND fail          %d\n", g_stNdisStatStru.sendArpDhcpNDFailNum);

    return;
}

/*
 * 功能描述: 打印IP地址信息
 * 修改历史:
 *  1.日    期: 2009年6月16日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_PrintIpv4Addr(const VOS_UINT8 *ipaddr)
{
    if ((ipaddr[0] == 0) && (ipaddr[1] == 0) && (ipaddr[2] == 0) && (ipaddr[3] == 0)) {
        PS_PRINTF_ERR("                   addr not config\n");
        return;
    }

    return;
}

/*
 * 功能描述: 显示有效的实体信息
 * 修改历史:
 *  1.日    期: 2011年3月16日
 *    修改内容: 新生成函数
 */
VOS_VOID Ndis_ShowValidEntity(VOS_UINT16 modemId, VOS_UINT8 rabId)
{
    NDIS_Entity      *entity = VOS_NULL_PTR;
    VOS_UINT8         exRabId;

    exRabId = NDIS_FORM_EXBID(modemId, rabId);
    entity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == entity) {
        PS_PRINTF_ERR("             no right NDIS entity    \n");
        return;
    }

    PS_PRINTF_ERR("                 ModemID:  %d\n", NDIS_GET_MODEMID_FROM_EXBID(entity->rabId));
    PS_PRINTF_ERR("                 EPS bearer ID:  %d\n", NDIS_GET_BID_FROM_EXBID(entity->rabId));
    PS_PRINTF_ERR("             ARP got flag:  %d\n", entity->ipV4Info.arpInitFlg);
    PS_PRINTF_ERR(" ARP recv reply flag:  %d\n", entity->ipV4Info.arpRepFlg);

    PS_PRINTF_ERR("\n======================================================\n");
}

/*
 * 功能描述: 显示所有的实体信息
 * 修改历史:
 *  1.日    期: 2011年4月18日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月16日
 *    修改内容: DSDA
 */
VOS_VOID Ndis_ShowAllEntity(VOS_VOID)
{
    VOS_UINT32   loop;
    NDIS_Entity *entity = VOS_NULL_PTR;

    for (loop = 0; loop < NAS_NDIS_MAX_ITEM; loop++) {
        entity = &g_pstNdisEntity[loop];
        if (PS_FALSE == entity->used) {
            PS_PRINTF_ERR("                 ModemID:  %d\n", NDIS_GET_MODEMID_FROM_EXBID(entity->rabId));
            PS_PRINTF_ERR("                 EPS bearer ID %d inactive\n", NDIS_GET_BID_FROM_EXBID(entity->rabId));
            continue;
        }

        PS_PRINTF_ERR("                 ModemID:  %d\n", NDIS_GET_MODEMID_FROM_EXBID(entity->rabId));
        PS_PRINTF_ERR("                 EPS bearer ID:  %d\n", NDIS_GET_BID_FROM_EXBID(entity->rabId));
        PS_PRINTF_ERR("             ARP got flag:  %d\n", entity->ipV4Info.arpInitFlg);
        PS_PRINTF_ERR(" ARP recv reply flag:  %d\n", entity->ipV4Info.arpRepFlg);

        PS_PRINTF_ERR("\n======================================================\n");
    }
}

VOS_VOID NDIS_OpenLatency(VOS_VOID)
{
    g_ulNdisLomSwitch = 1;
    return;
}

VOS_VOID NDIS_CloseLatency(VOS_VOID)
{
    g_ulNdisLomSwitch = 0;
    return;
}

VOS_VOID Ndis_LomTraceRcvUlData(VOS_VOID)
{
    if (1 == g_ulNdisLomSwitch) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "User plane latency trace: NDIS rcv UL data");
    }
    return;
}

VOS_VOID Ndis_LomTraceRcvDlData(VOS_VOID)
{
    if (1 == g_ulNdisLomSwitch) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "User plane latency trace: NDIS rcv DL data");
    }
    return;
}
