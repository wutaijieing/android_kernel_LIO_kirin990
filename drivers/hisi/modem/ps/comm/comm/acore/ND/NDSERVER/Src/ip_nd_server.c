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


/*
 * 1 Include HeadFile
 */
#include "ip_nd_server.h"
#include "ads_ndis_interface.h"

#include "ads_dev_i.h"
#include "ads_device_interface.h"
#include "mdrv.h"
#include "ps_tag.h"
#include "securec.h"
#include "ps_ndis.h"

#define THIS_MODU ps_nd
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_IPNDSERVER_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */
/* ND SERVER数据信息 */
IP_NdserverAddrInfo     g_astNdServerAddrInfo[IP_NDSERVER_ADDRINFO_MAX_NUM];
IP_NdserverTeDetectBuf g_astNdServerTeDetectBuf;

/* ND SERVER本地保存的配置参数 */
VOS_UINT8  g_ucMFlag;          /* M标识 */
VOS_UINT8  g_ucOFlag;          /* O标识 */
VOS_UINT16 g_usRouterLifetime; /* Router Lifetime */
VOS_UINT32 g_ulReachableTime;  /* Reachable Time */
VOS_UINT32 g_ulRetransTimer;   /* Retrans Timer */

VOS_UINT32 g_ulNsTimerLen;         /* 非周期性NS定时器时长 */
VOS_UINT32 g_ulNsTimerMaxExpNum;   /* 非周期性NS最大超时次数 */
VOS_UINT32 g_ulPeriodicNsTimerLen; /* 周期性NS定时器时长 */
VOS_UINT32 g_ulPeriodicRaTimerLen; /* 周期性RA定时器时长，防止Router过期 */
VOS_UINT32 g_ulFirstNsTimerLen;    /* 收到重复地址检测后等待的定时器时长 */
VOS_UINT32 g_ulRaTimerLen;         /* 收到重复地址检测前周期发送RA定时器时长 */

/* 周期性发送RA时间计数器 */
VOS_UINT32 g_aulPeriodicRaTimeCnt[IP_NDSERVER_ADDRINFO_MAX_NUM];

/* 用于存储解析后的ND报文数据 */
IP_ND_Msg g_stNdMsgData;

/* ND SERVER使用的发包缓冲区 */
VOS_UINT8 g_aucSendMsgBuffer[IP_IPM_MTU];

/* ND SERVER报文统计信息 */
IP_NdserverPacketStatisticsInfo g_astNdServerPktStatInfo[IP_NDSERVER_ADDRINFO_MAX_NUM];

VOS_UINT8 g_aucInvalidIpv6Addr[IP_IPV6_ADDR_LEN] = {0};

extern VOS_UINT32 g_ulNvMtu;

/*
 * 3 Function
 */
extern VOS_UINT8 *Ndis_GetMacAddr(VOS_VOID);
extern VOS_UINT32 Ndis_SendMacFrm(const VOS_UINT8 *buf, VOS_UINT32 len, VOS_UINT8 rabId);
extern VOS_UINT32 Ndis_DlSendNcm(VOS_UINT8 rabId, ADS_PktTypeUint8 pktType, IMM_Zc *immZc);

/*
 * 功能描述: 临时保存Te IP地址，在TE不发重复地址检测的情况下用来获取TE MAC地
 *             址
 * 修改历史:
 *  1.日    期: 2011年5月17日
 *    修改内容: 新生成函数
 */
VOS_VOID IP_NDSERVER_SaveTeDetectIp(const VOS_UINT8 *teGlobalAddr)
{
    errno_t    rlt;
    VOS_UINT32 index = g_astNdServerTeDetectBuf.head;

    if ((IP_IPV6_IS_LINKLOCAL_ADDR(teGlobalAddr)) || (IP_IPV6_64BITPREFIX_EQUAL_ZERO(teGlobalAddr)) ||
        (IP_IPV6_IS_MULTICAST_ADDR(teGlobalAddr))) {
        /* 不是Global IP */
        return;
    }

    /* 相同Prefix的地址是否已经存在 */
    while (index != g_astNdServerTeDetectBuf.tail) {
        /*lint -e960*/
        if ((IP_TRUE == g_astNdServerTeDetectBuf.teIpBuf[index].valid) &&
            (0 == IP_MEM_CMP(teGlobalAddr, g_astNdServerTeDetectBuf.teIpBuf[index].teGlobalAddr,
                             ND_IP_IPV6_PREFIX_LENGTH))) {
            /*lint +e960*/
            /* 相同Prefix只保留一个地址 */

            rlt = memcpy_s(g_astNdServerTeDetectBuf.teIpBuf[index].teGlobalAddr, IP_IPV6_ADDR_LEN,
                             teGlobalAddr, IP_IPV6_ADDR_LEN);
            IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
            return;
        }
        index = TTF_MOD_ADD(index, 1, g_astNdServerTeDetectBuf.maxNum);
    }

    if (g_astNdServerTeDetectBuf.head ==
        TTF_MOD_ADD(g_astNdServerTeDetectBuf.tail, 1, g_astNdServerTeDetectBuf.maxNum)) {
        /* BUF满时覆盖最老的地址 */
        g_astNdServerTeDetectBuf.head = TTF_MOD_ADD(g_astNdServerTeDetectBuf.head, 1,
                                                      g_astNdServerTeDetectBuf.maxNum);
    }

    /* 保存IP地址 */
    rlt = memcpy_s(g_astNdServerTeDetectBuf.teIpBuf[g_astNdServerTeDetectBuf.tail].teGlobalAddr,
                     IP_IPV6_ADDR_LEN, teGlobalAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    g_astNdServerTeDetectBuf.teIpBuf[g_astNdServerTeDetectBuf.tail].valid = IP_TRUE;

    g_astNdServerTeDetectBuf.tail = TTF_MOD_ADD(g_astNdServerTeDetectBuf.tail, 1,
                                                  g_astNdServerTeDetectBuf.maxNum);
}

/*
 * 功能描述: 获取临时保存的TE IP地址
 * 修改历史:
 *  1.日    期: 2011年5月17日
 *    修改内容: 新生成函数
 */
VOS_UINT8 *IP_NDSERVER_GetTeDetectIp(const VOS_UINT8 *prefixAddr)
{
    VOS_UINT32 index = g_astNdServerTeDetectBuf.head;
    while (index != g_astNdServerTeDetectBuf.tail) {
        /*lint -e960*/
        if ((IP_TRUE == g_astNdServerTeDetectBuf.teIpBuf[index].valid) &&
            (0 == IP_MEM_CMP(prefixAddr, g_astNdServerTeDetectBuf.teIpBuf[index].teGlobalAddr,
                             ND_IP_IPV6_PREFIX_LENGTH))) {
            /*lint +e960*/
            g_astNdServerTeDetectBuf.teIpBuf[index].valid = IP_FALSE;

            /* Buf头部空隙处理 */
            while ((g_astNdServerTeDetectBuf.head != g_astNdServerTeDetectBuf.tail) &&
                   (IP_FALSE == g_astNdServerTeDetectBuf.teIpBuf[g_astNdServerTeDetectBuf.head].valid)) {
                g_astNdServerTeDetectBuf.head = TTF_MOD_ADD(g_astNdServerTeDetectBuf.head, 1,
                                                              g_astNdServerTeDetectBuf.maxNum);
            }

            return g_astNdServerTeDetectBuf.teIpBuf[index].teGlobalAddr;
        }
        index = TTF_MOD_ADD(index, 1, g_astNdServerTeDetectBuf.maxNum);
    }

    return IP_NULL_PTR;
}

/*
 * Description: 设置本地网络参数
 * History:
 *    1.      2011-04-01  Draft Enact
 */
VOS_VOID IP_NDSERVER_SetLocalParam(VOS_VOID)
{
    /* M标识，0，主机不能通过DHCPv6获取IPv6地址 */
    g_ucMFlag = 0;

    /* O标识，1，主机可以通过无状态DHCPv6获取其他参数，例如DNS服务器，SIP服务器； */
    g_ucOFlag = 1;

    /* Router Lifetime(秒) */
    g_usRouterLifetime = 9000;/* 路由生存期9000s */

    /* Reachable Time(毫秒) */
    g_ulReachableTime = 3600000;

    /* Retrans Timer(毫秒)，0，表示未知 */
    g_ulRetransTimer = 0;

    /* 非周期性NS定时器时长(毫秒) */
    g_ulNsTimerLen = 4000;

    /* 非周期性NS最大超时次数 */
    g_ulNsTimerMaxExpNum = 3;

    /* 周期性NS定时器时长(毫秒) */
    g_ulPeriodicNsTimerLen = 60000;

    /* 周期性RA，防止Router过期(毫秒) */
    g_ulPeriodicRaTimerLen = 3600000;

    /* 收到重复地址检测后等待的定时器时长(毫秒) */
    g_ulFirstNsTimerLen = 2000;

    /* 收到重复地址检测前周期发送RA的定时器时长(毫秒) */
    g_ulRaTimerLen = 15000;

    return;
}

/*
 * Description: 清除统计信息
 * History: VOS_VOID
 *    1.      2011-04-11  Draft Enact
 */
VOS_VOID IP_NDSERVER_DebugInit(VOS_VOID)
{
    VOS_UINT32 index;
    for (index = 0; index < IP_NDSERVER_ADDRINFO_MAX_NUM; index++) {
        (VOS_VOID)memset_s(IP_NDSERVER_GET_STATINFO_ADDR(index), sizeof(IP_NdserverPacketStatisticsInfo),
                           IP_NULL, sizeof(IP_NdserverPacketStatisticsInfo));
    }

    return;
}

/*
 * Description: 判断定时器名是否合法
 * Input: ulIndex     --- ND SERVER数据体索引号
 *                   enTimerType --- 定时器类型
 * History: VOS_UINT32
 *    1.      2011-04-02  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_IsTimerNameValid(VOS_UINT32 index, NDSER_TimerUint32 timerType)
{
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
        case IP_ND_SERVER_TIMER_FIRST_NS:
        case IP_ND_SERVER_TIMER_RA: {
            if (index < IP_NDSERVER_ADDRINFO_MAX_NUM) {
                return IP_TRUE;
            }

            break;
        }
        default:
            break;
    }

    return IP_FALSE;
}

/*
 * Description: 获取定时器
 * Input: enTimerType------------------定时器类型
 *                   ulIndex----------------------定时器索引号
 * History:
 *    1.      2011-04-02  Draft Enact
 */
IP_TimerInfo *IP_NDSERVER_GetTimer(VOS_UINT32 index, NDSER_TimerUint32 timerType)
{
    IP_TimerInfo *timerInfo = VOS_NULL_PTR;

    /* 根据定时器不同类型，获取定时器 */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
        case IP_ND_SERVER_TIMER_FIRST_NS:
        case IP_ND_SERVER_TIMER_RA:
            timerInfo = IP_NDSERVER_ADDRINFO_GET_TIMER(index);
            break;
        default:
            break;
    }

    return timerInfo;
}

/*
 * Description: 获取定时器时长
 * Input: enTimerType------------------定时器类型
 * History:
 *    1.      2011-04-02  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_GetTimerLen(NDSER_TimerUint32 timerType)
{
    VOS_UINT32 timerLen = IP_NULL;

    /* 根据定时器不同类型，定时器时长不同 */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
            timerLen = g_ulNsTimerLen;
            break;
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
            timerLen = g_ulPeriodicNsTimerLen;
            break;
        case IP_ND_SERVER_TIMER_FIRST_NS:
            timerLen = g_ulFirstNsTimerLen;
            break;
        case IP_ND_SERVER_TIMER_RA:
            timerLen = g_ulRaTimerLen;
            break;
        default:
            break;
    }

    return timerLen;
}
/*lint -e550*/
/*
 * Description: 打印定时器启动信息
 * Input: enTimerType------------------定时器类型
 *                   ulIndex----------------------定时器索引号
 * History:
 *    1.      2010-04-02  Draft Enact
 */
VOS_VOID IP_NDSERVER_PrintTimeStartInfo(VOS_UINT32 index, NDSER_TimerUint32 timerType)
{
    VOS_UINT32 epsbId;

    epsbId = IP_NDSERVER_GET_EPSBID(index);
    /* 根据定时器不同类型，打印相应信息 */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStartInfo:NORM:IP TimerStart(ulIndex): IP_ND_SERVER_TIMER_NS", index,
                           epsbId);
            break;
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStartInfo:NORM:IP TimerStart(ulIndex): IP_ND_SERVER_TIMER_PERIODIC_NS",
                           index, epsbId);
            break;
        case IP_ND_SERVER_TIMER_FIRST_NS:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStartInfo:NORM:IP TimerStart(ulIndex): IP_ND_SERVER_TIMER_FIRST_NS",
                           index, epsbId);
            break;
        case IP_ND_SERVER_TIMER_RA:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStartInfo:NORM:IP TimerStart(ulIndex): IP_ND_SERVER_TIMER_RA", index,
                           epsbId);
            break;
        default:
            break;
    }

    return;
}

/*
 * Description: 打印定时器关闭信息
 * Input: enTimerType------------------定时器类型
 *                   ulIndex----------------------定时器索引号
 * History:
 *    1.      2010-04-02  Draft Enact
 */
VOS_VOID IP_NDSERVER_PrintTimeStopInfo(VOS_UINT32 index, NDSER_TimerUint32 timerType)
{
    VOS_UINT32 epsbId;

    epsbId = IP_NDSERVER_GET_EPSBID(index);
    /* 根据定时器不同类型，打印相应信息 */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStopInfo:NORM:IP TimerStop(ulIndex): IP_ND_SERVER_TIMER_NS", index,
                           epsbId);
            break;
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStopInfo:NORM:IP TimerStop(ulIndex): IP_ND_SERVER_TIMER_PERIODIC_NS",
                           index, epsbId);
            break;
        case IP_ND_SERVER_TIMER_FIRST_NS:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStopInfo:NORM:IP TimerStop(ulIndex): IP_ND_SERVER_TIMER_FIRST_NS",
                           index, epsbId);
            break;
        case IP_ND_SERVER_TIMER_RA:
            IPND_INFO_LOG2(NDIS_NDSERVER_PID,
                           "IP_NDSERVER_PrintTimeStopInfo:NORM:IP TimerStop(ulIndex): IP_ND_SERVER_TIMER_RA", index,
                           epsbId);
            break;
        default:
            break;
    }

    return;
}
/*lint +e550*/
/*
 * Discription: 启动某一承载的某种类型的定时器
 * History:
 *      1.      2011-04-02  Draft Enact
 */
VOS_VOID IP_NDSERVER_TimerStart(VOS_UINT32 index, NDSER_TimerUint32 timerType)
{
    VOS_UINT32     timerLen;
    IP_TimerInfo  *timerInfo = VOS_NULL_PTR;

    /* 对ulIndex合法性判断 */
    if (IP_FALSE == IP_NDSERVER_IsTimerNameValid(index, timerType)) {
        /* 打印异常信息 */
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart: WARN: Input Para(ulIndex) err !", index,
                          timerType);
        return;
    }

    /* 根据消息对应的索引号和定时器类型,获取相关联的定时器 */
    timerInfo = IP_NDSERVER_GetTimer(index, timerType);
    if (timerInfo == VOS_NULL_PTR) {
        /* 打印异常信息 */
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:ERROR: Get Timer failed.");
        return;
    }

    /* 判断定时器是否打开，已打开则关闭 */
    if (VOS_NULL_PTR != timerInfo->hTm) {
        /* 关闭失败，则报警返回 */
        if (VOS_OK != PS_STOP_REL_TIMER(&(timerInfo->hTm))) {
            /* 打印异常信息 */
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:WARN: stop reltimer error!");
            return;
        }

        /* 打印异常信息 */
        IPND_INFO_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:(TimerType) Timer not close!", timerInfo->timerName,
                       index);
    }

    /* 根据定时器不同类型，定时器信息不同 */
    timerLen = IP_NDSERVER_GetTimerLen(timerType);
    if (timerLen == IP_NULL) {
        /* 打印异常信息 */
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:ERROR: start unreasonable reltimer.");
        return;
    }

    /* 设定定时器NAME,enTimerType设定定时器Para，打开失败则报警返回 */
    if (VOS_OK != PS_START_REL_TIMER(&(timerInfo->hTm), NDIS_NDSERVER_PID, timerLen, (VOS_UINT32)timerType,
                                     index, VOS_RELTIMER_NOLOOP)) {
        /* 打印异常信息 */
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStart:WARN: start reltimer error!");
        return;
    }

    /* 更新定时器类别 */
    timerInfo->timerName = timerType;

    /* 打印定时器启动信息 */
    IP_NDSERVER_PrintTimeStartInfo(index, timerType);

    return;
}

/*
 * Discription: 停止某一承载某种类型定时器
 * Return: None
 *      1.      2011-04-02  Draft Enact
 */
VOS_VOID IP_NDSERVER_TimerStop(VOS_UINT32 index, NDSER_TimerUint32 timerType)
{
    IP_TimerInfo *timerInfo = VOS_NULL_PTR;

    /* 对ulIndex合法性判断 */
    if (IP_FALSE == IP_NDSERVER_IsTimerNameValid(index, timerType)) {
        /* 打印异常信息 */
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop: WARN: Input Para(ulIndex) err !", index,
                          timerType);
        return;
    }

    /* 根据消息对应的索引号和定时器类型,获取相关联的定时器 */
    timerInfo = IP_NDSERVER_GetTimer(index, timerType);
    if (timerInfo == VOS_NULL_PTR) {
        /* 打印异常信息 */
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop:ERROR:Get Timer failed.");
        return;
    }

    /* 定时器处于打开状态，则关闭，否则，忽略 */
    if (VOS_NULL_PTR != timerInfo->hTm) {
        if (timerType != timerInfo->timerName) {
            /* 打印异常信息 */
            IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop: TimerType not match:", timerType,
                              timerInfo->timerName);
        }
        /* 关闭失败，则报警返回 */
        if (VOS_OK != PS_STOP_REL_TIMER(&(timerInfo->hTm))) {
            /* 打印异常信息 */
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerStop:WARN: stop reltimer error!");
            return;
        }

        /* 清除超时计数 */
        timerInfo->loopTimes = 0;

        /* 打印定时器关闭信息 */
        IP_NDSERVER_PrintTimeStopInfo(index, timerType);
    }

    return;
}

/*
 * Description: ND SERVER模块的初始化
 * History: VOS_VOID
 *    1.        2011-04-01  Draft Enact
 */
VOS_VOID IP_NDSERVER_Init(VOS_VOID)
{
    VOS_UINT32 index;
    VOS_UINT16 payLoad = IP_IPV6_PAYLOAD;
    VOS_UINT32 rtn;
    errno_t    rlt;

    IP_NDSERVER_SetLocalParam();
    IP_NDSERVER_DebugInit();

    for (index = 0; index < IP_NDSERVER_ADDRINFO_MAX_NUM; index++) {
        (VOS_VOID)memset_s(IP_NDSERVER_ADDRINFO_GET_ADDR(index), sizeof(IP_NdserverAddrInfo), IP_NULL,
                           sizeof(IP_NdserverAddrInfo));

        rlt = memcpy_s(IP_NDSERVER_ADDRINFO_GET_MACADDR(index), IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(),
                         IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s((IP_NDSERVER_ADDRINFO_GET_MACFRAME(index) + IP_MAC_ADDR_LEN),
                         (IP_ETH_MAC_HEADER_LEN - IP_MAC_ADDR_LEN), (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        /* payload长度为2个字节 */
        rlt = memcpy_s((IP_NDSERVER_ADDRINFO_GET_MACFRAME(index) + 2 * IP_MAC_ADDR_LEN),
                         (IP_ETH_MAC_HEADER_LEN - 2 * IP_MAC_ADDR_LEN), (VOS_UINT8 *)(&payLoad), 2);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        g_aulPeriodicRaTimeCnt[index] = g_ulPeriodicRaTimerLen / g_ulPeriodicNsTimerLen;

        /* 创建下行IP包缓存队列 */
        rtn = LUP_CreateQue(NDIS_NDSERVER_PID, &(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index)),
                              ND_IPV6_WAIT_ADDR_RSLT_Q_LEN);
        if (PS_SUCC != rtn) {
            PS_PRINTF_ERR("IP_NDSERVER_Init, LUP_CreateQue DlPktQue fail.\n");
            return;
        }
    }

    (VOS_VOID)memset_s(&g_astNdServerTeDetectBuf, sizeof(IP_NdserverTeDetectBuf), IP_NULL,
                       sizeof(IP_NdserverTeDetectBuf));

    g_astNdServerTeDetectBuf.head   = 0;
    g_astNdServerTeDetectBuf.tail   = 0;
    g_astNdServerTeDetectBuf.maxNum = 2 * IP_NDSERVER_ADDRINFO_MAX_NUM;

    return;
}

/*
 * 功能描述: ND SERVER PID初始化函数
 * 修改历史:
 *  1.日    期: 2011年12月7日
 *    修改内容: 新生成函数
 */
VOS_UINT32 APP_NdServer_Pid_InitFunc(enum VOS_InitPhaseDefine phase)
{
    switch (phase) {
        case VOS_IP_LOAD_CONFIG:
            IP_NDSERVER_Init();
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
    return 0;
}

/*
 * Description: 清除ND SERVER相关信息
 * Input: ulIndex --- ND SERVER结构索引号
 * History:
 *    1.     2011-04-01  Draft Enact
 */
VOS_VOID IP_NDSERVER_Stop(VOS_UINT32 index)
{
    IP_NdserverAddrInfo *infoAddr = IP_NULL_PTR;
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    infoAddr->validFlag = IP_FALSE;

    infoAddr->epsbId = 0;

    (VOS_VOID)memset_s(&infoAddr->ipv6NwPara, sizeof(ESM_IP_Ipv6NWPara), IP_NULL,
                       sizeof(ESM_IP_Ipv6NWPara));

    if (IP_NDSERVER_TE_ADDR_INEXISTENT != infoAddr->teAddrInfo.teAddrState) {
        IP_NDSERVER_SaveTeDetectIp(infoAddr->teAddrInfo.teGlobalAddr);
    }

    /* 清除TE地址记录 */
    (VOS_VOID)memset_s(&infoAddr->teAddrInfo, sizeof(IP_NdserverTeAddrInfo), IP_NULL,
                       sizeof(IP_NdserverTeAddrInfo));

    IP_NDSERVER_TimerStop(index, infoAddr->timerInfo.timerName);

    (VOS_VOID)memset_s(&infoAddr->ipSndNwMsg, sizeof(IP_SndMsg), IP_NULL, sizeof(IP_SndMsg));

    g_aulPeriodicRaTimeCnt[index] = g_ulPeriodicRaTimerLen / g_ulPeriodicNsTimerLen;

    return;
}

/*
 * Description: 释放下行缓存队列中的PKT
 * History: VOS_VOID
 *    1.       2011-12-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_ClearDlPktQue(VOS_UINT32 index)
{
    IMM_Zc      *immZc = VOS_NULL_PTR;
    VOS_INT32    lockKey;

    while (PS_TRUE != (LUP_IsQueEmpty(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index)))) {
        lockKey = VOS_SplIMP();
        if (PS_SUCC != LUP_DeQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index), (VOS_VOID **)(&immZc))) {
            VOS_Splx(lockKey);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ClearDlPktQue, LUP_DeQue return fail!");
            return;
        }
        VOS_Splx(lockKey);
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
    }

    return;
}

/*
 * Description: 处理NDIS发来的Ipv6PdnRel
 * History: VOS_VOID
 *    1.       2011-12-09  Draft Enact
 *    2.       2013-1-16  DSDA
 */
VOS_VOID NdSer_Ipv6PdnRel(VOS_UINT8 exRabId)
{
    VOS_UINT32 index;

    IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnRel is entered.");

    index = IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);

    if (index < IP_NDSERVER_ADDRINFO_MAX_NUM) {
        IP_NDSERVER_Stop(index);

        IP_NDSERVER_ClearDlPktQue(index); /* 释放下行PKT缓存队列 */
    } else {
        IPND_ERROR_LOG2(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnRel: Invalid Input:", index, exRabId);
    }

    return;
}

/*
 * Description: 供NDIS调用，查询对应承载的ND SERVER实体是否还有效
 * History: VOS_UINT32
 *    1.       2011-12-06  Draft Enact
 */
VOS_UINT32 NdSer_Ipv6PdnValid(VOS_UINT8 rabId)
{
    VOS_UINT32 index;

    index = IP_NDSERVER_GET_ADDR_INFO_INDEX(rabId);
    if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*
 * Description: 供NDIS调用，获取ND SERVER对应实体的完整MAC帧头
 * History: VOS_UINT8
 *    1.       2011-12-06  Draft Enact
 */
VOS_UINT8 *NdSer_GetMacFrm(VOS_UINT8 index, VOS_UINT8 *teAddrState)
{
    IP_NdserverAddrInfo *infoAddr = IP_NULL_PTR;

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    if (IP_TRUE != infoAddr->validFlag) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "pstInfoAddr ucValidFlag is not TRUE!", index);
        return VOS_NULL_PTR;
    }

    *teAddrState = infoAddr->teAddrInfo.teAddrState;

    return IP_NDSERVER_ADDRINFO_GET_MACFRAME(index);
}

/*
 * Description: 供NDIS调用，当未获得PC MAC地址时，缓存下行IP包
 * History: VOS_UINT8
 *    1.      2011-12-06  Draft Enact
 */
VOS_VOID NdSer_MacAddrInvalidProc(IMM_Zc *immZc, VOS_UINT8 index)
{
    IMM_Zc      *queHead = VOS_NULL_PTR;
    VOS_INT32    lockKey;

    IP_NDSERVER_AddMacInvalidPktNum(index);

    /* **********************下行IPV6数据包缓存******************************** */
    if (PS_TRUE == LUP_IsQueFull(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index))) {
        lockKey = VOS_SplIMP();
        if (PS_SUCC != LUP_DeQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index), (VOS_VOID **)(&queHead))) {
            VOS_Splx(lockKey);
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_MacAddrInvalidProc, LUP_DeQue return NULL");
            return;
        }
        VOS_Splx(lockKey);
        /*lint -e522*/
        IMM_ZcFree(queHead); /* 释放最早的IP包 */
        /*lint +e522*/
    }

    lockKey = VOS_SplIMP();
    if (PS_SUCC != LUP_EnQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index), immZc)) { /* 插入最新的IP包 */
        VOS_Splx(lockKey);
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_MacAddrInvalidProc, LUP_EnQue return NULL");
        return;
    }
    VOS_Splx(lockKey);

    IP_NDSERVER_AddEnquePktNum(index);

    return;
}

/*
 * Description: 查找第一个可用的IPv6前缀
 * Input: pstConfigParaInd ---- ESM_IP_NW_PARA_IND_STRU消息指针
 * Output: pulPrefixIndex   ---- 前缀索引指针
 * History:
 *      1.     2011-04-05  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_FindIpv6EffectivePrefix(const AT_NDIS_Ipv6PdnInfo *configParaInd,
                                               VOS_UINT32 *prefixIndex)
{
    VOS_UINT32              index;
    ND_IP_Ipv6Prefix       *ipv6Prefix = IP_NULL_PTR;

    /* 遍历前缀列表，查找A标识为1，前缀长度为64的前缀,prefixLen单位是bit */
    for (index = IP_NULL; index < configParaInd->prefixNum; index++) {
        ipv6Prefix = (ND_IP_Ipv6Prefix *)&configParaInd->prefixList[index];
        if ((IP_IPV6_OP_TRUE == ipv6Prefix->prefixAValue) &&
            ((ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH) * 8 == ipv6Prefix->prefixLen) &&
            (!IP_IPV6_64BITPREFIX_EQUAL_ZERO(ipv6Prefix->prefix)) &&
            (!IP_IPV6_IS_LINKLOCAL_ADDR(ipv6Prefix->prefix)) &&
            (!IP_IPV6_IS_MULTICAST_ADDR(ipv6Prefix->prefix))) {
            *prefixIndex = index;
            return IP_SUCC;
        }
    }

    return IP_FAIL;
}

/*
 * Function Name: IP_NDSERVER_GetNwPara
 * Description: 从ID_ESM_IP_NW_PARA_IND中获取网络参数
 * Input: pstNwParaInd--------------消息指针
 * Output: pstNwParamTmp-------------网络参数指针
 * History: VOS_UINT32
 *    1.      2011-04-05  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_GetNwPara(ESM_IP_Ipv6NWPara *nwParamTmp,
                                 const AT_NDIS_Ipv6PdnInfo *nwParaInd)
{
    VOS_UINT32 prefixIndex;
    VOS_UINT32 rslt;
    errno_t    rlt;
    (VOS_VOID)memset_s(nwParamTmp, sizeof(ESM_IP_Ipv6NWPara), IP_NULL, sizeof(ESM_IP_Ipv6NWPara));

    /* 获取HOP LIMIT */
    nwParamTmp->curHopLimit = nwParaInd->curHopLimit;

    /* 获取MTU */
    if (IP_IPV6_OP_TRUE == nwParaInd->OpMtu) {
        nwParamTmp->opMtu = nwParaInd->OpMtu;
        nwParamTmp->mtuValue      = nwParaInd->Mtu;
    }

    /* 获取接口标识符 */
    rlt = memcpy_s(nwParamTmp->interfaceId, ND_IP_IPV6_IFID_LENGTH, nwParaInd->interfaceId,
                     ND_IP_IPV6_IFID_LENGTH);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* 保存地址前缀列表，目前只保存和使用第一个可用的IPv6前缀 */
    rslt = IP_NDSERVER_FindIpv6EffectivePrefix(nwParaInd, &prefixIndex);

    if (IP_SUCC != rslt) {
        return IP_FAIL;
    }

    rlt = memcpy_s(&nwParamTmp->prefixList[IP_NULL], sizeof(ND_IP_Ipv6Prefix),
                     &nwParaInd->prefixList[prefixIndex], sizeof(ND_IP_Ipv6Prefix));
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    nwParamTmp->prefixNum = 1;

    /* 获取DNS服务器列表 */
    rlt = memcpy_s(nwParamTmp->dnsSer.priDnsServer, IP_IPV6_ADDR_LEN, nwParaInd->dnsSer.priServer,
                     IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(nwParamTmp->dnsSer.secDnsServer, IP_IPV6_ADDR_LEN, nwParaInd->dnsSer.secServer,
                     IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    nwParamTmp->dnsSer.dnsSerNum = nwParaInd->dnsSer.serNum;

    /* 获取SIP服务器列表 */
    rlt = memcpy_s(nwParamTmp->sipSer.priSipServer, IP_IPV6_ADDR_LEN, nwParaInd->pcscfSer.priServer,
                     IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(nwParamTmp->sipSer.secSipServer, IP_IPV6_ADDR_LEN, nwParaInd->pcscfSer.secServer,
                     IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    nwParamTmp->sipSer.sipSerNum = nwParaInd->pcscfSer.serNum;

    return IP_SUCC;
}

/*
 * Description: RA报文头固定部分设置
 * Input: ulIndex --- ND SERVER实体索引
 * Output: pucData --- 报文缓冲指针
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormRaHeaderMsg(VOS_UINT32 index, VOS_UINT8 *data)
{
    ESM_IP_Ipv6NWPara *nwPara = IP_NULL_PTR;

    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    nwPara = IP_NDSERVER_ADDRINFO_GET_NWPARA(index);

    /* 类型 */
    *data = IP_ICMPV6_TYPE_RA;
    data++;

    /* 代码 */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* 跳过校验和 */
    data += 2;

    /* 当前跳限制 */
    *data = nwPara->curHopLimit;
    data++;

    /* 管理地址配置标志、其他有状态配置标志 */
    *data = (VOS_UINT8)(((g_ucMFlag & 0x1) << 7) | ((g_ucOFlag & 0x1) << 6));
    data++;

    /* 路由生存期 */
    IP_SetUint16Data(data, g_usRouterLifetime);
    data += 2;/* 路由生存周期需要占2个字节 */

    /* 可达时间 */
    IP_SetUint32Data(data, g_ulReachableTime);
    data += 4;/* 可达时间需要占4个字节 */

    /* 重发定时器 */
    IP_SetUint32Data(data, g_ulRetransTimer);
    data += 4;/* 重发定时器需要占4个字节 */

    return;
}

/*
 * Function Name: IP_NDSERVER_FormRaOptMsg
 * Description: RA报文选项部分设置
 * Input: ulIndex --- ND SERVER实体索引
 *                   pucMacAddr- 源链路层地址
 * Output: pucData --- 报文缓冲指针
 *                   pulLen ---- 报文长度指针
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormRaOptMsg(VOS_UINT32 index, const VOS_UINT8 *macAddr, VOS_UINT8 *data,
                                  VOS_UINT32 *len)
{
    ESM_IP_Ipv6NWPara        *nwPara = IP_NULL_PTR;
    VOS_UINT32                count;
    VOS_UINT32                tmpMtu;
    VOS_INT32                 rlt;
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    nwPara = IP_NDSERVER_ADDRINFO_GET_NWPARA(index);

    /* 类型 */
    *data = IP_ICMPV6_OPT_SRC_LINK_LAYER_ADDR;
    data++;

    /* 长度 */
    *data = 1;
    data++;

    /* 链路层地址 */
    rlt = memcpy_s(data, IP_MAC_ADDR_LEN, macAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_MAC_ADDR_LEN;

    (*len) += IP_ND_OPT_UNIT_LEN;

    if (IP_IPV6_OP_TRUE == nwPara->opMtu) {
        /* 类型 */
        *data = IP_ICMPV6_OPT_MTU;
        data++;

        /* 长度 */
        *data = 1;
        data++;

        /* 保留 */
        data += 2;

        /* MTU: 取NV中的MTU和RA中的MTU的最小值作为发给PC的MTU */
        tmpMtu = PS_MIN(g_ulNvMtu, nwPara->mtuValue);
        IP_SetUint32Data(data, tmpMtu);
        data += 4;/* MTU长度占4个字节 */

        (*len) += IP_ND_OPT_UNIT_LEN;
    }

    for (count = 0; count < nwPara->prefixNum; count++) {
        /* 类型 */
        *data = IP_ICMPV6_OPT_PREFIX_INFO;
        data++;

        /* 长度 */
        *data = 4;
        data++;

        /* 前缀长度 */
        *data = (VOS_UINT8)(nwPara->prefixList[count].prefixLen);
        data++;

        /* 链路上标志、自治标志 */
        *data = (VOS_UINT8)(((nwPara->prefixList[count].prefixLValue & 0x1) << 7) |
                               ((nwPara->prefixList[count].prefixAValue & 0x1) << 6));
        data++;

        /* 有效生存期 */
        IP_SetUint32Data(data, nwPara->prefixList[count].validLifeTime);
        data += 4;/* 有效生存期占4个字节 */

        /* 选用生存期 */
        IP_SetUint32Data(data, nwPara->prefixList[count].preferredLifeTime);
        data += 4;/* 选用生存期占4个字节 */

        /* 保留字段 */
        data += 4;

        /* 前缀 */
        rlt = memcpy_s(data, IP_IPV6_ADDR_LEN, nwPara->prefixList[count].prefix, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
        data += IP_IPV6_ADDR_LEN;

        (*len) += (4 * IP_ND_OPT_UNIT_LEN);
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_FormEtherHeaderMsg
 * Description: IPv6报头设置
 * Input: aucSrcMacAddr --- 源MAC地址
 *                   aucDstMacAddr --- 目的MAC地址
 * Output: pucData --- 报文缓冲指针
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormEtherHeaderMsg(const VOS_UINT8 *srcMacAddr, const VOS_UINT8 *dstMacAddr,
                                        VOS_UINT8 *data)
{
    VOS_INT32 rlt;
    /* 目的MAC */
    rlt = memcpy_s(data, IP_MAC_ADDR_LEN, dstMacAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_MAC_ADDR_LEN;

    /* 源MAC */
    rlt = memcpy_s(data, IP_MAC_ADDR_LEN, srcMacAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_MAC_ADDR_LEN;

    /* 类型 */
    IP_SetUint16Data(data, (VOS_UINT16)IP_GMAC_PAYLOAD_TYPE_IPV6);
    data += 2;

    return;
}

/*
 * Function Name: IP_NDSERVER_FormRaMsg
 * Description: 生成路由公告消息
 * Input: ulIndex ------- 处理RA发包实体索引
 *                   pstNdMsgData -- 目的信息参数
 * Output: pucSendBuff --- 发送RA报文缓冲
 *                   pulSendLen ---- 发送报文长度
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormRaMsg(VOS_UINT32 index, const IP_ND_Msg *ndMsgData, VOS_UINT8 *sendBuff,
                               VOS_UINT32 *sendLen)
{
    VOS_UINT32                  packetLen                    = IP_NULL;
    VOS_UINT8                  *data                        = sendBuff;
    VOS_UINT8                   srcIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8                   srcMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8                   dstIPAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                   dstMacAddr[IP_MAC_ADDR_LEN];
    IP_NdserverAddrInfo *infoAddr                    = IP_NULL_PTR;
    VOS_INT32                   rlt;
    /* 打印进入该函数 */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormRaMsg is entered.");

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    data += IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* 设置RA报头 */
    IP_NDSERVER_FormRaHeaderMsg(index, data);

    packetLen += IP_ICMPV6_RA_HEADER_LEN;

    /* 根据自身MAC地址生成link-local地址 */
    rlt = memcpy_s(srcMacAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr, srcMacAddr);
    IP_SetUint16Data(srcIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);
    rlt = memcpy_s(infoAddr->ueLinkLocalAddr, IP_IPV6_ADDR_LEN, srcIPAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* 设置RA报文选项 */
    IP_NDSERVER_FormRaOptMsg(index, srcMacAddr, (VOS_VOID *)(data + packetLen), &packetLen);

    /* 确定单播或组播方式 */
    if ((VOS_NULL_PTR != ndMsgData) && (IP_IPV6_IS_LINKLOCAL_ADDR(ndMsgData->srcIp)) &&
        (1 == ndMsgData->ndMsgStru.rsInfo.opSrcLinkLayerAddr)) {
        rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, ndMsgData->srcIp, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, ndMsgData->ndMsgStru.rsInfo.srcLinkLayerAddr,
                         IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, g_ndAllNodesMulticaseAddr, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, g_ndAllNodesMacAddr, IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    }

    data -= IP_IPV6_HEAD_LEN;

    /* 设置IPv6报头 */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, packetLen, data, IP_HEAD_PROTOCOL_ICMPV6);

    /* 生成ICMPv6报头校验和 */
    if (IP_SUCC != IP_BuildIcmpv6Checksum(data, IP_IPV6_HEAD_LEN)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormRaMsg: Build ICMPv6 Checksum failed.");
    }

    data -= IP_ETHERNET_HEAD_LEN;

    /* 设置以太报头 */
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, data);

    /* 返回报文长度 */
    *sendLen = packetLen + IP_IPV6_HEAD_LEN + IP_ETHERNET_HEAD_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_SendRaMsg
 * Description: 发送路由公告消息
 * Input: ulIndex ------- 处理RA发包实体索引
 *                   pstNdMsgData -- 目的信息参数
 * History: VOS_UINT32
 *    1.      2011-04-07  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_SendRaMsg(VOS_UINT32 index, const IP_ND_Msg *ndMsgData)
{
    VOS_UINT8 *sendBuff = VOS_NULL_PTR;
    VOS_UINT32 sendLen   = IP_NULL;
    VOS_UINT32 epsbId;

    IP_ASSERT_RTN(index < IP_NDSERVER_ADDRINFO_MAX_NUM, PS_FAIL);
    epsbId = IP_NDSERVER_GET_EPSBID(index);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    /* 调用形成RA消息函数 */
    IP_NDSERVER_FormRaMsg(index, ndMsgData, sendBuff, &sendLen);

    IP_NDSERVER_AddTransRaPktNum(index);
    IP_NDSERVER_AddTransPktTotalNum(index);

    /* 将RA消息发送到PC */
    return Ndis_SendMacFrm(sendBuff, sendLen, (VOS_UINT8)epsbId);
}

/*
 * Function Name: IP_NDSERVER_FormNaHeaderMsg
 * Description: NA报文头固定部分设置
 * Input: pucTargetIPAddr --- 目的IP地址
 *                   ucSolicitFlag   --- 请求标志
 * Output: pucData --- 报文缓冲指针
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormNaHeaderMsg(const VOS_UINT8 *targetIPAddr, VOS_UINT8 solicitFlag, VOS_UINT8 *data)
{
    VOS_INT32 rlt;
    /* 类型 */
    *data = IP_ICMPV6_TYPE_NA;
    data++;

    /* 代码 */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* 跳过校验和 */
    data += 2;

    /* 路由器标志、请求标志、覆盖标志 */
    *data = (VOS_UINT8)(0xa0 | ((solicitFlag & 0x1) << 6));
    data++;

    /* 保留 */
    data += 3;

    /* 目标地址 */
    rlt = memcpy_s(data, IP_IPV6_ADDR_LEN, targetIPAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_IPV6_ADDR_LEN;

    return;
}

/*
 * Description: NA报文选项部分设置
 * Input: pucMacAddr- 源链路层地址
 * Output: pucData --- 报文缓冲指针
 *                   pulLen ---- 报文长度指针
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormNaOptMsg(const VOS_UINT8 *macAddr, VOS_UINT8 *data, VOS_UINT32 *len)
{
    VOS_INT32 rlt;
    /* 类型 */
    *data = IP_ICMPV6_OPT_TGT_LINK_LAYER_ADDR;
    data++;

    /* 长度 */
    *data = 1;
    data++;

    /* 链路层地址 */
    rlt = memcpy_s(data, IP_MAC_ADDR_LEN, macAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_MAC_ADDR_LEN;

    (*len) += IP_ND_OPT_UNIT_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_FormNaMsg
 * Description: 生成邻居公告消息
 * Input: ulIndex ------- 处理NA发包实体索引
 *                   pstNdMsgData -- 目的信息参数
 * Output: pucSendBuff --- 发送NA报文缓冲
 *                   pulSendLen ---- 发送报文长度
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormNaMsg(const IP_ND_Msg *ndMsgData, VOS_UINT8 *sendBuff, VOS_UINT32 *sendLen)
{
    VOS_UINT32 packetLen                   = IP_NULL;
    VOS_UINT8 *data                        = sendBuff;
    VOS_UINT8  srcIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8  srcMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8  dstIPAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8  dstMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8  solicitFlag                  = IP_NULL;
    VOS_INT32  rlt;
    /* 打印进入该函数 */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormNaMsg is entered.");

    /* 根据自身MAC地址作为源MAC */
    rlt = memcpy_s(srcMacAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    if (VOS_NULL_PTR == ndMsgData) {
        /* 根据自身MAC地址生成link-local地址 */
        IP_ProduceIfaceIdFromMacAddr(srcIPAddr, srcMacAddr);
        IP_SetUint16Data(srcIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);
    } else {
        /* 使用目标地址作为源IP */
        rlt = memcpy_s(srcIPAddr, IP_IPV6_ADDR_LEN, ndMsgData->ndMsgStru.nsInfo.targetAddr, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    }

    /* 确定单播或组播方式 */
    if ((VOS_NULL_PTR != ndMsgData) && (!IP_IPV6_EQUAL_ALL_ZERO(ndMsgData->srcIp)) &&
        (!IP_IPV6_IS_MULTICAST_ADDR(ndMsgData->srcIp)) &&
        (1 == ndMsgData->ndMsgStru.nsInfo.opSrcLinkLayerAddr)) {
        rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, ndMsgData->srcIp, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, ndMsgData->ndMsgStru.nsInfo.srcLinkLayerAddr,
                         IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        solicitFlag = 1;
    } else {
        rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, g_ndAllNodesMulticaseAddr, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        rlt = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, g_ndAllNodesMacAddr, IP_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    }

    data += IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* 设置NA报头 */
    IP_NDSERVER_FormNaHeaderMsg(srcIPAddr, solicitFlag, data);

    packetLen += IP_ICMPV6_NA_HEADER_LEN;

    /* 设置NA可选项 */
    IP_NDSERVER_FormNaOptMsg(srcMacAddr, (VOS_VOID *)(data + packetLen), &packetLen);

    data -= IP_IPV6_HEAD_LEN;

    /* 设置IPv6报头 */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, packetLen, data, IP_HEAD_PROTOCOL_ICMPV6);

    /* 生成ICMPv6校验和 */
    if (IP_SUCC != IP_BuildIcmpv6Checksum(data, IP_IPV6_HEAD_LEN)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormNaMsg: Build ICMPv6 Checksum failed.");
    }

    data -= IP_ETHERNET_HEAD_LEN;

    /* 设置以太报头 */
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, data);

    /* 返回报文长度 */
    *sendLen = packetLen + IP_IPV6_HEAD_LEN + IP_ETHERNET_HEAD_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_SendNaMsg
 * Description: 发送邻居公告消息
 * Input: ulIndex ------- 处理NA发包实体索引
 *                   pstNdMsgData -- 目的信息参数
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_SendNaMsg(VOS_UINT32 index, const IP_ND_Msg *ndMsgData)
{
    VOS_UINT8 *sendBuff = VOS_NULL_PTR;
    VOS_UINT32 sendLen   = IP_NULL;
    VOS_UINT32 epsbId;
    IP_ASSERT_RTN(index < IP_NDSERVER_ADDRINFO_MAX_NUM, PS_FAIL);
    epsbId = IP_NDSERVER_GET_EPSBID(index);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    /* 调用形成NA消息函数 */
    IP_NDSERVER_FormNaMsg(ndMsgData, sendBuff, &sendLen);

    IP_NDSERVER_AddTransNaPktNum(index);
    IP_NDSERVER_AddTransPktTotalNum(index);

    return Ndis_SendMacFrm(sendBuff, sendLen, (VOS_UINT8)epsbId);
}

/*
 * Function Name: IP_NDSERVER_FormNsHeaderMsg
 * Description: NS报文头固定部分设置
 * Input: ulIndex --- ND SERVER实体索引
 * Output: pucData --- 报文缓冲指针
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormNsHeaderMsg(VOS_UINT32 index, VOS_UINT8 *data, const VOS_UINT8 *dstAddr)
{
    VOS_INT32 rlt;
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    /* 类型 */
    *data = IP_ICMPV6_TYPE_NS;
    data++;

    /* 代码 */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* 跳过校验和 */
    data += 2;

    /* 保留 */
    data += 4;

    /* 目标地址 */
    rlt = memcpy_s(data, IP_IPV6_ADDR_LEN, dstAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    data += IP_IPV6_ADDR_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_FormNsOptMsg
 * Description: NS报文选项部分设置
 * Input: pucMacAddr- 源链路层地址
 * Output: pucData --- 报文缓冲指针
 *                   pulLen ---- 报文长度指针
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormNsOptMsg(const VOS_UINT8 *macAddr, VOS_UINT8 *data, VOS_UINT32 *len)
{
    VOS_INT32 rlt;
    /* 类型 */
    *data = IP_ICMPV6_OPT_SRC_LINK_LAYER_ADDR;
    data++;

    /* 长度 */
    *data = 1;
    data++;

    /* 链路层地址 */
    rlt = memcpy_s(data, IP_MAC_ADDR_LEN, macAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    data += IP_MAC_ADDR_LEN;

    (*len) += IP_ND_OPT_UNIT_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_FormNsMsg
 * Description: 生成地址解析邻居请求消息
 * Input: ulIndex ------- 处理NS发包实体索引
 * Output: pucSendBuff --- 发送NS报文缓冲
 *                   pulSendLen ---- 发送报文长度
 * History: VOS_VOID
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_FormNsMsg(VOS_UINT32 index, VOS_UINT8 *sendBuff, VOS_UINT32 *sendLen,
                               const VOS_UINT8 *dstAddr)
{
    VOS_UINT32 packetLen                    = IP_NULL;
    VOS_UINT8 *data                        = sendBuff;
    VOS_UINT8  srcIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8  srcMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8  dstIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8  dstMacAddr[IP_MAC_ADDR_LEN] = {IP_NULL};
    VOS_INT32  rlt;
    /* 打印进入该函数 */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormNsMsg is entered.");

    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    /* 根据自身MAC地址生成link-local地址 */
    rlt = memcpy_s(srcMacAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr, srcMacAddr);
    IP_SetUint16Data(srcIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);

    /* 根据目的IP生成请求节点组播地址 */
    IP_ProduceSolicitedNodeMulticastIPAddr(dstIPAddr, dstAddr);
    IP_ProduceSolicitedNodeMulticastMacAddr(dstMacAddr, dstAddr);

    data += IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* 设置NS报头 */
    IP_NDSERVER_FormNsHeaderMsg(index, data, dstAddr);

    packetLen += IP_ICMPV6_NS_HEADER_LEN;

    /* 设置NS可选项 */
    IP_NDSERVER_FormNsOptMsg(srcMacAddr, (VOS_VOID *)(data + packetLen), &packetLen);

    data -= IP_IPV6_HEAD_LEN;

    /* 设置IPv6报头 */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, packetLen, data, IP_HEAD_PROTOCOL_ICMPV6);

    /* 生成ICMPv6校验和 */
    if (IP_SUCC != IP_BuildIcmpv6Checksum(data, IP_IPV6_HEAD_LEN)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormNsMsg: Build ICMPv6 Checksum failed.");
    }

    data -= IP_ETHERNET_HEAD_LEN;

    /* 设置以太报头 */
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, data);

    /* 返回报文长度 */
    *sendLen = packetLen + IP_IPV6_HEAD_LEN + IP_ETHERNET_HEAD_LEN;

    return;
}

/*
 * Function Name: IP_NDSERVER_SendNsMsg
 * Description: 发送地址解析邻居请求消息
 * Input: ulIndex ------- 处理NS发包实体索引
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_SendNsMsg(VOS_UINT32 index, const VOS_UINT8 *dstAddr)
{
    VOS_UINT8 *sendBuff = VOS_NULL_PTR;
    VOS_UINT32 sendLen   = IP_NULL;
    VOS_UINT32 epsbId;
    IP_ASSERT_RTN(index < IP_NDSERVER_ADDRINFO_MAX_NUM, PS_FAIL);
    epsbId = IP_NDSERVER_GET_EPSBID(index);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    /* 调用形成NS消息函数 */
    IP_NDSERVER_FormNsMsg(index, sendBuff, &sendLen, dstAddr);

    IP_NDSERVER_AddTransNsPktNum(index);
    IP_NDSERVER_AddTransPktTotalNum(index);

    return Ndis_SendMacFrm(sendBuff, sendLen, (VOS_UINT8)epsbId);
}

/*
 * 功能描述: 根据ExRabId查找NDSERVER实体的索引
 * 修改历史:
 *  1.日    期: 2013年1月15日
 *    修改内容: 新生成函数
 */
VOS_UINT32 NdSer_GetAddrInfoIdx(VOS_UINT8 exRabId)
{
    VOS_UINT32 i = 0;

    /* 查询是否已存在相应Entity */
    do {
        if ((PS_TRUE == g_astNdServerAddrInfo[i].validFlag) && (exRabId == g_astNdServerAddrInfo[i].epsbId)) {
            /* 找到相应实体 */
            return i;
        }

    } while ((++i) < IP_NDSERVER_ADDRINFO_MAX_NUM);

    return IP_NDSERVER_ADDRINFO_MAX_NUM;
}

/*
 * 功能描述: 分配一个空闲的NDSERVER实体
 * 修改历史:
 *  1.日    期: 2013年1月15日
 *    修改内容: 新生成函数
 */
IP_NdserverAddrInfo *NdSer_AllocAddrInfo(VOS_UINT32 *pUlIndex)
{
    VOS_UINT32 i = 0;

    if (IP_NULL_PTR == pUlIndex) {
        return IP_NULL_PTR;
    }

    *pUlIndex = IP_NDSERVER_ADDRINFO_MAX_NUM;

    do {
        if (PS_FALSE == g_astNdServerAddrInfo[i].validFlag) {
            /* 找到空闲实体 */
            *pUlIndex = i;
            return &g_astNdServerAddrInfo[i];
        }

    } while ((++i) < IP_NDSERVER_ADDRINFO_MAX_NUM);

    return IP_NULL_PTR;
}

/*
 * 功能描述: PDN IPV6地址信息检查
 * 修改历史:
 *  1.日    期: 2011年12月11日
 *    修改内容: 新生成函数
 *  2.日    期: 2013年1月15日
 *    修改内容: DSDA开发，使用ExRabId来映射NdSer实体
 */
VOS_UINT32 NdSer_CheckIpv6PdnInfo(const AT_NDIS_Ipv6PdnInfo *ipv6PdnInfo)
{
    VOS_UINT32        index;
    ND_IP_Ipv6Prefix *ipv6Prefix = IP_NULL_PTR;

    if (AT_NDIS_MAX_PREFIX_NUM_IN_RA <= ipv6PdnInfo->prefixNum) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "NdSer_CheckIpv6PdnInfo: Invalid IPV6 PrefixNum!",
                        ipv6PdnInfo->prefixNum);
        return IP_FAIL;
    }

    /* 遍历前缀列表，查找A标识为1，前缀长度为64的前缀，prefixLen单位为bit */
    for (index = IP_NULL; index < ipv6PdnInfo->prefixNum; index++) {
        ipv6Prefix = (ND_IP_Ipv6Prefix *)&ipv6PdnInfo->prefixList[index];
        if ((PS_TRUE == ipv6Prefix->prefixAValue) &&
            ((ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH) * 8 == ipv6Prefix->prefixLen) &&
            (!IP_IPV6_64BITPREFIX_EQUAL_ZERO(ipv6Prefix->prefix)) &&
            (!IP_IPV6_IS_LINKLOCAL_ADDR(ipv6Prefix->prefix)) &&
            (!IP_IPV6_IS_MULTICAST_ADDR(ipv6Prefix->prefix))) {
            return PS_SUCC;
        }
    }

    return PS_FAIL;
}

/*
 * Function Name: NdSer_Ipv6PdnInfoCfg
 * Description: 处理NDIS发来的Ipv6PdnInfoCfg
 * History:
 *    1.      2011-12-09  Draft Enact
 *    1.      2013-1-16  Modified for DSDA
 */
VOS_VOID NdSer_Ipv6PdnInfoCfg(VOS_UINT8 exRabId, const AT_NDIS_Ipv6PdnInfo *ipv6PdnInfo)
{
    VOS_UINT32                  index      = IP_NULL;
    IP_NdserverAddrInfo        *infoAddr  = IP_NULL_PTR;
    ESM_IP_Ipv6NWPara           nwParamTmp;
    VOS_INT32                   rlt;
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcEsmIpNwParaInd is entered.");

    /* 通过扩展RabId查找NDSERVER实体，如果查找不到，则分配一个空闲的 */
    index = IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);
    if (index < IP_NDSERVER_ADDRINFO_MAX_NUM) {
        infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    } else {
        /* 分配一个AddrInfo结构体，并且获得Index */
        infoAddr = NdSer_AllocAddrInfo(&index);
    }

    if (IP_NULL_PTR == infoAddr) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Get ND Server Entity failed.", exRabId);
        return;
    }

    /* 获取网络参数 */
    if (IP_SUCC != IP_NDSERVER_GetNwPara(&nwParamTmp, ipv6PdnInfo)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Get Nw Param fail.");
        return;
    }

    /* 判断是否未分配过路由前缀 */
    if (IP_TRUE != infoAddr->validFlag) {
        VOS_UINT8 *ipV6;
        ipV6 = IP_NDSERVER_GetTeDetectIp(nwParamTmp.prefixList[0].prefix);

        /* MT关机再开机后，ND SERVER收到网侧RA前，可能先收到TE的重复地址检测NS */
        if (IP_NULL_PTR != ipV6) {
            IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Using saved TeAddr to get MAC.");

            rlt = memcpy_s(infoAddr->teAddrInfo.teGlobalAddr, IP_IPV6_ADDR_LEN, ipV6,
                             ND_IP_IPV6_ADDR_LENGTH);
            IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
            infoAddr->teAddrInfo.teAddrState = IP_NDSERVER_TE_ADDR_INCOMPLETE;

            /* 启动收到重复地址检测后的等待定时器 */
            IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_FIRST_NS);
        } else {
            IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: None saved TeAddr.");

            /* 启动路由公告定时器 */
            IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_RA);
        }
    }
    /* 判断是否路由前缀发生了变化 */
    else if (0 != IP_MEM_CMP(infoAddr->ipv6NwPara.prefixList[0].prefix,
                             nwParamTmp.prefixList[0].prefix,
                             (ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH))) {
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg: Valid ND Server get new Nw Param.");

        /* 清除TE地址记录 */
        (VOS_VOID)memset_s(&infoAddr->teAddrInfo, sizeof(IP_NdserverTeAddrInfo), IP_NULL,
                           sizeof(IP_NdserverTeAddrInfo));

        /* 启动路由公告定时器 */
        IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_RA);
    }

    /* 将stNwParamTmp中的内容拷贝到pstInfoAddr->stIpv6NwPara中 */
    rlt = memcpy_s(&infoAddr->ipv6NwPara, sizeof(ESM_IP_Ipv6NWPara), &nwParamTmp,
                     sizeof(ESM_IP_Ipv6NWPara));
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* 设置承载号 */
    infoAddr->epsbId = exRabId;

    /* 置位ND SERVER结构体有效标志 */
    infoAddr->validFlag = IP_TRUE;

    /* 发送RA消息到PC */
    if (PS_SUCC != IP_NDSERVER_SendRaMsg(index, VOS_NULL_PTR)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_Ipv6PdnInfoCfg:Send RA Msg failed.");
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_DecodeRsData
 * Description: 对RS包进行格式转换
 * Input: pucSrcData ----------- 源报文指针
 *                   pstDestData ---------- 目的转换结构指针
 *                   ulIcmpv6HeadOffset --- ICMPv6报头偏移量
 * History:
 *    1.      2011-04-07  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_DecodeRsData(VOS_UINT8 *srcData, IP_ND_Msg *destData, VOS_UINT32 icmpv6HeadOffset)
{
    VOS_UINT32         payLoad;
    VOS_INT32          remainLen;
    VOS_UINT32         optType;
    VOS_UINT32         optLen;
    VOS_UINT8         *ipMsg = srcData;
    IP_ND_MsgRs       *rs    = &destData->ndMsgStru.rsInfo;
    VOS_INT32          rlt;
    /* 获取PAYLOAD */
    IP_GetUint16Data(payLoad, ipMsg + IP_IPV6_BASIC_HEAD_PAYLOAD_OFFSET);

    remainLen = (VOS_INT32)(payLoad + IP_IPV6_HEAD_LEN - (icmpv6HeadOffset + IP_ICMPV6_RS_HEADER_LEN));

    if (remainLen < 0) {
        IPND_ERROR_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeRsData: Invalid IPV6 PayLoad::!", payLoad,
                        icmpv6HeadOffset);
        return IP_FAIL;
    }

    rlt = memcpy_s(destData->srcIp, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(destData->desIp, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_DST_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /*lint -e679 -esym(679,*)*/
    ipMsg += icmpv6HeadOffset + IP_ICMPV6_RS_HEADER_LEN;
    /*lint -e679 +esym(679,*)*/

    while (remainLen >= IP_ND_OPT_UNIT_LEN) {
        optType = *ipMsg;
        optLen  = *(ipMsg + 1);

        if (0 == optLen) {
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeRsData: Invalid ND options length 0!");
            return IP_FAIL;
        }

        switch (optType) {
            case IP_ICMPV6_OPT_SRC_LINK_LAYER_ADDR: {
                if (1 == optLen) {
                    if (0 == rs->opSrcLinkLayerAddr) {
                        rlt = memcpy_s(rs->srcLinkLayerAddr, IP_MAC_ADDR_LEN, ipMsg + 2, IP_MAC_ADDR_LEN);
                        IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
                        rs->opSrcLinkLayerAddr = 1;
                    } else {
                        IPND_ERROR_LOG(NDIS_NDSERVER_PID,
                                       "IP_NDSERVER_DecodeRsData: Redundant Source Link-Layer Addr!");
                    }
                } else {
                    IPND_ERROR_LOG1(NDIS_NDSERVER_PID,
                                    "IP_NDSERVER_DecodeRsData: Invalid Source Link-Layer Addr Length:!", optLen);
                }
            } break;
            default:
                break;
        }

        remainLen -= (VOS_INT32)IP_GetNdOptionLen(optLen);
        /*lint -e679 -esym(679,*)*/
        ipMsg += IP_GetNdOptionLen(optLen);
        /*lint -e679 +esym(679,*)*/
    }

    if (0 != remainLen) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeRsData: Payload not match Package:!", remainLen);
    }

    return IP_SUCC;
}

/*
 * Function Name: IP_NDSERVER_RsMsgProc
 * Description: 处理RS消息
 * Input: ulIndex  -------------- 处理消息实体索引
 *                   pucSrcData ------------ IP数据报
 *                   ulIcmpv6HeadOffset ---- ICMPv6报文头偏移量
 * History:
 *    1.      2011-04-06  Draft Enact
 */
VOS_VOID IP_NDSERVER_RsMsgProc(VOS_UINT32 index, VOS_UINT8 *srcData, VOS_UINT32 icmpv6HeadOffset)
{
    VOS_UINT8                  *ipMsg     = srcData;
    IP_ND_Msg                  *ndMsgData = VOS_NULL_PTR;
    IP_NdserverAddrInfo        *infoAddr  = IP_NULL_PTR;
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_RsMsgProc is entered.");

    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    IP_NDSERVER_AddRcvRsPktNum(index);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    if (IP_TRUE != infoAddr->validFlag) {
        IP_NDSERVER_AddErrRsPktNum(index);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_RsMsgProc: ND Server info flag is invalid!");
        return;
    }

    ndMsgData = IP_NDSERVER_GET_NDMSGDATA_ADDR();
    (VOS_VOID)memset_s(ndMsgData, sizeof(IP_ND_Msg), IP_NULL, sizeof(IP_ND_Msg));

    if (IP_SUCC != IP_NDSERVER_DecodeRsData(ipMsg, ndMsgData, icmpv6HeadOffset)) {
        IP_NDSERVER_AddErrRsPktNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcessRsMsg: Invalid IPV6 RS Msg:!");
        return;
    }

    /* 发送RA消息到PC */
    if (PS_SUCC != IP_NDSERVER_SendRaMsg(index, ndMsgData)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_RsMsgProc:Send RA Msg failed.");
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_DecodeNsData
 * Description: 对NS包进行格式转换
 * Input: pucSrcData ----------- 源报文指针
 *                   pstDestData ---------- 目的转换结构指针
 *                   ulIcmpv6HeadOffset --- ICMPv6报头偏移量
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_DecodeNsData(VOS_UINT8 *srcData, IP_ND_Msg *destData, VOS_UINT32 icmpv6HeadOffset)
{
    VOS_INT32          rlt;
    VOS_UINT32         payLoad;
    VOS_INT32          remainLen;
    VOS_UINT32         optType;
    VOS_UINT32         optLen;
    VOS_UINT8         *ipMsg = srcData;
    IP_ND_MsgNs       *ns    = &destData->ndMsgStru.nsInfo;

    /* 获取PAYLOAD */
    IP_GetUint16Data(payLoad, ipMsg + IP_IPV6_BASIC_HEAD_PAYLOAD_OFFSET);

    remainLen = (VOS_INT32)(payLoad + IP_IPV6_HEAD_LEN - (icmpv6HeadOffset + IP_ICMPV6_NS_HEADER_LEN));

    if (remainLen < 0) {
        IPND_ERROR_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeNsData: Invalid IPV6 PayLoad::!", payLoad,
                        icmpv6HeadOffset);
        return IP_FAIL;
    }

    rlt = memcpy_s(destData->srcIp, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(destData->desIp, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_DST_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(ns->targetAddr, IP_IPV6_ADDR_LEN,
                     ipMsg + icmpv6HeadOffset + IP_ICMPV6_TARGET_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    /*lint -e679 -esym(679,*)*/
    ipMsg += icmpv6HeadOffset + IP_ICMPV6_NS_HEADER_LEN;
    /*lint -e679 +esym(679,*)*/

    while (remainLen >= IP_ND_OPT_UNIT_LEN) {
        optType = *ipMsg;
        optLen  = *(ipMsg + 1);

        if (0 == optLen) {
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeNsData: Invalid ND options length 0!");
            return IP_FAIL;
        }

        switch (optType) {
            case IP_ICMPV6_OPT_SRC_LINK_LAYER_ADDR: {
                if (1 == optLen) {
                    if (0 == ns->opSrcLinkLayerAddr) {
                        rlt = memcpy_s(ns->srcLinkLayerAddr, IP_MAC_ADDR_LEN, ipMsg + 2, IP_MAC_ADDR_LEN);
                        IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
                        ns->opSrcLinkLayerAddr = 1;
                    } else {
                        IPND_ERROR_LOG(NDIS_NDSERVER_PID,
                                       "IP_NDSERVER_DecodeNsData: Redundant Source Link-Layer Addr!");
                    }
                } else {
                    IPND_ERROR_LOG1(NDIS_NDSERVER_PID,
                                    "IP_NDSERVER_DecodeNsData: Invalid Source Link-Layer Addr Length:!", optLen);
                }
            } break;
            default:
                break;
        }

        remainLen -= (VOS_INT32)IP_GetNdOptionLen(optLen);
        /*lint -e679 -esym(679,*)*/
        ipMsg += IP_GetNdOptionLen(optLen);
        /*lint -e679 +esym(679,*)*/
    }

    if (0 != remainLen) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeNsData: Payload not match Package:!", remainLen);
    }

    return IP_SUCC;
}

/*
 * Function Name: IP_NDSERVER_IsSelfIPAddr
 * Description: 判断是否是自己的IP地址
 * Input: ulIndex  -------------- 处理消息实体索引
 *                   aucIPAddr ------------  IP地址指针
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_IsSelfIPAddr(VOS_UINT32 index, const VOS_UINT8 *iPAddr)
{
    VOS_UINT8                 linkLocalIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8                 globalIPAddr[IP_IPV6_ADDR_LEN]    = {IP_NULL};
    VOS_UINT8                 macAddr[IP_MAC_ADDR_LEN];
    ESM_IP_Ipv6NWPara        *nwPara                            = IP_NULL_PTR;
    VOS_INT32                 rlt;
    IP_ASSERT_RTN(index < IP_NDSERVER_ADDRINFO_MAX_NUM, IP_FALSE);

    /* 根据自身MAC地址生成link-local地址 */
    rlt = memcpy_s(macAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FALSE);
    IP_ProduceIfaceIdFromMacAddr(linkLocalIPAddr, macAddr);
    IP_SetUint16Data(linkLocalIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);

    IP_ProduceIfaceIdFromMacAddr(globalIPAddr, macAddr);
    nwPara = IP_NDSERVER_ADDRINFO_GET_NWPARA(index);
    rlt = memcpy_s(globalIPAddr, IP_IPV6_ADDR_LEN, nwPara->prefixList[0].prefix,
                     ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FALSE);

    /*lint -e960*/
    if ((IP_NULL == IP_MEM_CMP(iPAddr, linkLocalIPAddr, IP_IPV6_ADDR_LEN)) ||
        (IP_NULL == IP_MEM_CMP(iPAddr, globalIPAddr, IP_IPV6_ADDR_LEN))) {
        /*lint +e960*/
        return IP_TRUE;
    } else {
        return IP_FALSE;
    }
}

/*
 * Function Name: IP_NDSERVER_EqualAdvertisedPrefix
 * Description: 判断是否符合已公告的全球前缀
 * Input: ulIndex  -------------- 处理消息实体索引
 *                   aucIPAddr ------------  IP地址指针
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_EqualAdvertisedPrefix(VOS_UINT32 index, const VOS_UINT8 *iPAddr)
{
    ESM_IP_Ipv6NWPara *nwPara = IP_NULL_PTR;

    IP_ASSERT_RTN(index < IP_NDSERVER_ADDRINFO_MAX_NUM, IP_FALSE);
    nwPara = IP_NDSERVER_ADDRINFO_GET_NWPARA(index);

    if (IP_NULL ==
        IP_MEM_CMP(iPAddr, nwPara->prefixList[0].prefix, ND_IP_IPV6_ADDR_LENGTH - ND_IP_IPV6_IFID_LENGTH)) {
        return IP_TRUE;
    } else {
        return IP_FALSE;
    }
}

/*
 * Function Name: IP_NDSERVER_RcvTeDetectionAddr
 * Description: 收到重复地址检测地址
 * Input: ulIndex  -------------- ND SERVER实体索引
 *                   aucIPAddr ------------  IP地址指针
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_RcvTeDetectionAddr(VOS_UINT32 index, const VOS_UINT8 *iPAddr)
{
    IP_NdserverTeAddrInfo *teInfo = IP_NULL_PTR;
    VOS_INT32                      rlt;
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);
    /*lint -e960*/
    if ((IP_NDSERVER_TE_ADDR_REACHABLE == teInfo->teAddrState) &&
        (IP_NULL == IP_MEM_CMP(teInfo->teGlobalAddr, iPAddr, IP_IPV6_ADDR_LEN))) {
        /*lint +e960*/
        return;
    }

    IPND_WARNING_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_RcvTeDetectionAddr: Old Te Addr:", teInfo->teAddrState);
    rlt = memcpy_s(teInfo->teGlobalAddr, IP_IPV6_ADDR_LEN, iPAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    teInfo->teAddrState = IP_NDSERVER_TE_ADDR_INCOMPLETE;

    IPND_WARNING_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_RcvTeDetectionAddr: New Te Addr:", teInfo->teAddrState);

    return;
}

/*
 * Function Name: IP_NDSERVER_NsMsgProc
 * Description: 处理NS消息
 * Input: ulIndex  -------------- 处理消息实体索引
 *                   pucSrcData ------------ IP数据报
 *                   ulIcmpv6HeadOffset ---- ICMPv6报文头偏移量
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_NsMsgProc(VOS_UINT32 index, VOS_UINT8 *srcData, VOS_UINT32 icmpv6HeadOffset)
{
    VOS_UINT8                  *ipMsg     = srcData;
    IP_ND_Msg                  *ndMsgData = VOS_NULL_PTR;
    IP_NdserverAddrInfo        *infoAddr  = IP_NULL_PTR;
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc is entered.");
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    IP_NDSERVER_AddRcvNsPktNum(index);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    ndMsgData = IP_NDSERVER_GET_NDMSGDATA_ADDR();
    (VOS_VOID)memset_s(ndMsgData, sizeof(IP_ND_Msg), IP_NULL, sizeof(IP_ND_Msg));

    if (IP_SUCC != IP_NDSERVER_DecodeNsData(ipMsg, ndMsgData, icmpv6HeadOffset)) {
        IP_NDSERVER_AddErrNsPktNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc: Invalid IPV6 NS Msg:!");
        return;
    }

    if (IP_TRUE != infoAddr->validFlag) {
        /* MT关机再开机后，ND SERVER收到网侧RA前，可能先收到TE的重复地址检测NS */
        if ((IP_IPV6_EQUAL_ALL_ZERO(ndMsgData->srcIp)) &&
            (0 == ndMsgData->ndMsgStru.nsInfo.opSrcLinkLayerAddr)) {
            IP_NDSERVER_SaveTeDetectIp(ndMsgData->ndMsgStru.nsInfo.targetAddr);
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc: receive DAD NS packet before RA.");
        } else {
            IP_NDSERVER_AddErrNsPktNum(index);
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc: ND Server info flag is invalid!");
        }
        return;
    }

    if (IP_TRUE == IP_NDSERVER_IsSelfIPAddr(index, ndMsgData->ndMsgStru.nsInfo.targetAddr)) {
        /* 发送NA消息到PC */
        if (PS_SUCC != IP_NDSERVER_SendNaMsg(index, ndMsgData)) {
            IP_NDSERVER_AddTransPktFailNum(index);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc:Send NA Msg failed.");
        }
    } else {
        /* 判断是否是重复地址检测NS */
        if ((IP_IPV6_EQUAL_ALL_ZERO(ndMsgData->srcIp)) &&
            (0 == ndMsgData->ndMsgStru.nsInfo.opSrcLinkLayerAddr)) {
            if (IP_TRUE == IP_NDSERVER_EqualAdvertisedPrefix(index, ndMsgData->ndMsgStru.nsInfo.targetAddr)) {
                IP_NDSERVER_RcvTeDetectionAddr(index, ndMsgData->ndMsgStru.nsInfo.targetAddr);

                /* 启动收到重复地址检测后的等待定时器 */
                IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_FIRST_NS);

                IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc:Receive duplicate addr detection packet.");
                return;
            } else {
                IP_NDSERVER_AddErrNsPktNum(index);
                IPND_WARNING_LOG(
                    NDIS_NDSERVER_PID,
                    "IP_NDSERVER_NsMsgProc:Duplicate addr detection IP don't equal Advertised Prefix, discard!");
                return;
            }
        } else if ((!IP_IPV6_EQUAL_ALL_ZERO(ndMsgData->srcIp)) &&
                   (0 != ndMsgData->ndMsgStru.nsInfo.opSrcLinkLayerAddr)) {
            if (PS_SUCC != IP_NDSERVER_SendNaMsg(index, ndMsgData)) {
                IP_NDSERVER_AddTransPktFailNum(index);
                IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc:Send NA Msg failed.");
            }
        } else {
            IP_NDSERVER_AddErrNsPktNum(index);
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NsMsgProc:Target IP is not self addr, discard!");
            return;
        }
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_DecodeNaData
 * Description: 对NA包进行格式转换
 * Input: pucSrcData ----------- 源报文指针
 *                   pstDestData ---------- 目的转换结构指针
 *                   ulIcmpv6HeadOffset --- ICMPv6报头偏移量
 * History:
 *    1.      2011-04-09  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_DecodeNaData(VOS_UINT8 *srcData, IP_ND_Msg *destData, VOS_UINT32 icmpv6HeadOffset)
{
    VOS_UINT32         payLoad;
    VOS_INT32          remainLen;
    VOS_UINT32         optType;
    VOS_UINT32         optLen;
    VOS_UINT8         *ipMsg = srcData;
    IP_ND_MsgNa       *na    = &destData->ndMsgStru.naInfo;
    VOS_INT32          rlt;
    /* 获取PAYLOAD */
    IP_GetUint16Data(payLoad, ipMsg + IP_IPV6_BASIC_HEAD_PAYLOAD_OFFSET);

    remainLen = (VOS_INT32)(payLoad + IP_IPV6_HEAD_LEN - (icmpv6HeadOffset + IP_ICMPV6_NA_HEADER_LEN));

    if (remainLen < 0) {
        IPND_ERROR_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeNaData: Invalid IPV6 PayLoad::!", payLoad,
                        icmpv6HeadOffset);
        return IP_FAIL;
    }

    rlt = memcpy_s(destData->srcIp, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    rlt = memcpy_s(destData->desIp, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_DST_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    na->naRValue = ((*(ipMsg + icmpv6HeadOffset + IP_ICMPV6_NA_FLAG_OFFSET)) >> 7) & 0x01;
    na->naSValue = ((*(ipMsg + icmpv6HeadOffset + IP_ICMPV6_NA_FLAG_OFFSET)) >> 6) & 0x01;
    na->naOValue = ((*(ipMsg + icmpv6HeadOffset + IP_ICMPV6_NA_FLAG_OFFSET)) >> 5) & 0x01;

    rlt = memcpy_s(na->targetAddr, IP_IPV6_ADDR_LEN,
                     ipMsg + icmpv6HeadOffset + IP_ICMPV6_TARGET_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    /*lint -e679 -esym(679,*)*/
    ipMsg += icmpv6HeadOffset + IP_ICMPV6_NA_HEADER_LEN;
    /*lint -e679 +esym(679,*)*/

    while (remainLen >= IP_ND_OPT_UNIT_LEN) {
        optType = *ipMsg;
        optLen  = *(ipMsg + 1);

        if (0 == optLen) {
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeNaData: Invalid ND options length 0!");
            return IP_FAIL;
        }

        switch (optType) {
            case IP_ICMPV6_OPT_TGT_LINK_LAYER_ADDR: {
                if (1 == optLen) {
                    if (0 == na->opTargetLinkLayerAddr) {
                        rlt = memcpy_s(na->targetLinkLayerAddr, IP_MAC_ADDR_LEN, ipMsg + 2, IP_MAC_ADDR_LEN);
                        IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

                        na->opTargetLinkLayerAddr = 1;
                    } else {
                        IPND_ERROR_LOG(NDIS_NDSERVER_PID,
                                       "IP_NDSERVER_DecodeNaData: Redundant Target Link-Layer Addr!");
                    }
                } else {
                    IPND_ERROR_LOG1(NDIS_NDSERVER_PID,
                                    "IP_NDSERVER_DecodeNaData: Invalid Target Link-Layer Addr Length:!", optLen);
                }
            } break;
            default:
                break;
        }

        remainLen -= (VOS_INT32)IP_GetNdOptionLen(optLen);
        /*lint -e679 -esym(679,*)*/
        ipMsg += IP_GetNdOptionLen(optLen);
        /*lint -e679 +esym(679,*)*/
    }

    if (0 != remainLen) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_DecodeNaData: Payload not match Package:!", remainLen);
    }

    return IP_SUCC;
}

/*
 * Function Name: IP_NDSERVER_EqualSavedTeAddr
 * Description: 判断是否是已保存的TE地址
 * Input: ulIndex  -------------  ND SERVER实体索引
 *                   aucIPAddr ------------  IP地址指针
 *                   aucMACAddr -----------  MAC地址指针
 * History:
 *    1.      2011-04-09  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_EqualSavedTeAddr(VOS_UINT32 index, const VOS_UINT8 *iPAddr, const VOS_UINT8 *mACAddr)
{
    IP_NdserverTeAddrInfo *teInfo = IP_NULL_PTR;

    IP_ASSERT_RTN(index < IP_NDSERVER_ADDRINFO_MAX_NUM, IP_FALSE);
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);

    /*lint -e960*/
    if ((IP_NDSERVER_TE_ADDR_REACHABLE == teInfo->teAddrState) &&
        (IP_NULL == IP_MEM_CMP(teInfo->teGlobalAddr, iPAddr, IP_IPV6_ADDR_LEN)) &&
        (IP_NULL == IP_MEM_CMP(teInfo->teLinkLayerAddr, mACAddr, IP_MAC_ADDR_LEN))) {
        /*lint +e960*/
        return IP_TRUE;
    } else {
        return IP_FALSE;
    }
}
/*
 * Function Name: IP_NDSERVER_SendDlPkt
 * Description: 发送下行缓存的IPV6数据包
 * Input: ulIndex  -------------  ND SERVER实体索引
 * History:
 *    1.      2011-12-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_SendDlPkt(VOS_UINT32 index)
{
    IMM_Zc                 *immZc  = VOS_NULL_PTR;
    VOS_UINT8               rabId;
    ADS_PktTypeUint8        pktType;
    VOS_UINT16              app;
    VOS_INT32               lockKey;

    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    while (PS_TRUE != (LUP_IsQueEmpty(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index)))) {
        lockKey = VOS_SplIMP();
        if (PS_SUCC != LUP_DeQue(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index), (VOS_VOID **)(&immZc))) {
            VOS_Splx(lockKey);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_SendDlPkt, LUP_DeQue return fail!");
            return;
        }
        VOS_Splx(lockKey);

        /* 从 ImmZc中取出RabId和enPdpType */
        /*lint -e522*/
        app     = IMM_ZcGetUserApp(immZc);
        rabId   = (VOS_UINT8)(app & 0xFF); /* 保存的值实际是 扩展RabId */
        pktType = (VOS_UINT8)(app >> 8);

        if (PS_SUCC != Ndis_DlSendNcm(rabId, pktType, immZc)) {
            IMM_ZcFree(immZc);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_SendDlPkt, Ndis_DlSendNcm fail!");
        }
        /*lint +e522*/

        IP_NDSERVER_AddSendQuePktNum(index);
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_UpdateSpeMac
 * Description: 配置硬件MAC地址
 */
STATIC VOS_VOID IP_NDSERVER_UpdateSpeMac(VOS_UINT32 index,  const VOS_UINT8 *macAddr)
{
    IP_NdserverAddrInfo     *infoAddr   = VOS_NULL_PTR;
    VOS_INT32                rlt;

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    if ( infoAddr->validFlag != IP_TRUE) {
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_UpdateSpeMac: ND Server info flag is invalid!");
        return;
    }

    rlt = memcpy_s(g_stSpeMacHeader.srcAddr, ETH_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(),
                     ETH_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    rlt = memcpy_s(g_stSpeMacHeader.dstAddr, ETH_MAC_ADDR_LEN, macAddr, ETH_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);


#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
#endif

    IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_UpdateSpeMac OK");
    return;
}


/*
 * Function Name: IP_NDSERVER_UpdateTeAddrInfo
 * Description: 收到重复地址检测地址
 * Input: ulIndex  -------------  ND SERVER实体索引
 *                   aucGlobalIPAddr ------  全球IP地址指针
 *                   aucMACAddr -----------  MAC地址指针
 *                   aucLinkLocalIPAddr ---  链路本地IP地址指针
 * History:
 *    1.      2011-04-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_UpdateTeAddrInfo(VOS_UINT32 index, const VOS_UINT8 *globalIPAddr, const VOS_UINT8 *mACAddr,
                                      const VOS_UINT8 *linkLocalIPAddr)
{
    IP_NdserverTeAddrInfo *teInfo = IP_NULL_PTR;
    VOS_INT32                      rlt;
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);

    IPND_WARNING_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_UpdateTeAddrInfo: Old Te Addr:", teInfo->teAddrState);
    rlt = memcpy_s(teInfo->teGlobalAddr, IP_IPV6_ADDR_LEN, globalIPAddr, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    rlt = memcpy_s(teInfo->teLinkLayerAddr, IP_MAC_ADDR_LEN, mACAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* 更新完整MAC帧头中的目的MAC地址 */
    rlt = memcpy_s(IP_NDSERVER_ADDRINFO_GET_MACFRAME(index), IP_ETH_MAC_HEADER_LEN, mACAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    if (IP_IPV6_IS_LINKLOCAL_ADDR(linkLocalIPAddr)) {
        rlt = memcpy_s(teInfo->teLinkLocalAddr, IP_IPV6_ADDR_LEN, linkLocalIPAddr, IP_IPV6_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        (VOS_VOID)memset_s(teInfo->teLinkLocalAddr, IP_IPV6_ADDR_LEN, IP_NULL, IP_IPV6_ADDR_LEN);
    }

    teInfo->teAddrState = IP_NDSERVER_TE_ADDR_REACHABLE;

    /* 配置硬件MAC地址 */
    IP_NDSERVER_UpdateSpeMac(index, mACAddr);

    IPND_WARNING_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_UpdateTeAddrInfo: New Te Addr:", teInfo->teAddrState);
    /* 发送下行IP缓存队列中的数据包 */
    IP_NDSERVER_SendDlPkt(index);

    return;
}

/*
 * Function Name: IP_NDSERVER_NaMsgProc
 * Description: 处理NA消息
 * Input: ulIndex  -------------- 处理消息实体索引
 *                   pucSrcData ------------ IP数据报
 *                   ulIcmpv6HeadOffset ---- ICMPv6报文头偏移量
 * History:
 *    1.      2011-04-08  Draft Enact
 */
VOS_VOID IP_NDSERVER_NaMsgProc(VOS_UINT32 index, VOS_UINT8 *srcData, VOS_UINT32 icmpv6HeadOffset)
{
    VOS_UINT8                  *ipMsg     = srcData;
    IP_ND_Msg                  *ndMsgData = VOS_NULL_PTR;
    IP_NdserverAddrInfo        *infoAddr  = IP_NULL_PTR;
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NaMsgProc is entered.");
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    IP_NDSERVER_AddRcvNaPktNum(index);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    if (IP_TRUE != infoAddr->validFlag) {
        IP_NDSERVER_AddErrNaPktNum(index);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NaMsgProc: ND Server info flag is invalid!");
        return;
    }

    ndMsgData = IP_NDSERVER_GET_NDMSGDATA_ADDR();
    (VOS_VOID)memset_s(ndMsgData, sizeof(IP_ND_Msg), IP_NULL, sizeof(IP_ND_Msg));

    if (IP_SUCC != IP_NDSERVER_DecodeNaData(ipMsg, ndMsgData, icmpv6HeadOffset)) {
        IP_NDSERVER_AddErrNaPktNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NaMsgProc: Invalid IPV6 NS Msg:!");
        return;
    }

    if (0 == ndMsgData->ndMsgStru.naInfo.opTargetLinkLayerAddr) {
        IP_NDSERVER_AddErrNaPktNum(index);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID,
                         "IP_NDSERVER_NaMsgProc:Packet don't contain Target Linklayer Addr, discard!");
        return;
    }

    if ((IP_TRUE != IP_NDSERVER_IsSelfIPAddr(index, ndMsgData->desIp)) &&
        (!IP_IPV6_IS_MULTICAST_ADDR(ndMsgData->desIp))) {
        IP_NDSERVER_AddErrNaPktNum(index);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NaMsgProc:Destination IP don't contain self Addr, discard!");
        return;
    }

    if (IP_TRUE != IP_NDSERVER_EqualAdvertisedPrefix(index, ndMsgData->ndMsgStru.naInfo.targetAddr)) {
        IP_NDSERVER_AddErrNaPktNum(index);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_NaMsgProc:Target IP don't equal Advertised Prefix, discard!");
        return;
    }

    if (IP_TRUE != IP_NDSERVER_EqualSavedTeAddr(index, ndMsgData->ndMsgStru.naInfo.targetAddr,
                                                ndMsgData->ndMsgStru.naInfo.targetLinkLayerAddr)) {
        IP_NDSERVER_UpdateTeAddrInfo(index, ndMsgData->ndMsgStru.naInfo.targetAddr,
                                     ndMsgData->ndMsgStru.naInfo.targetLinkLayerAddr, ndMsgData->srcIp);
    }

    IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_PERIODIC_NS);

    return;
}

/*
 * Function Name: IP_NDSERVER_BuildTooBigICMPPkt
 * Description: 形成向PC回复的超长包响应
 * Input: pucSrcData ----------- 源报文指针
 *                   pstDestData ---------- 目的转换结构指针
 *                   ulIcmpv6HeadOffset --- ICMPv6报头偏移量
 * History:
 *    1.      2011-12-09  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_BuildTooBigICMPPkt(VOS_UINT32 index, VOS_UINT8 *srcData, VOS_UINT8 *destData,
                                          VOS_UINT32 dataLen)
{
    VOS_UINT8                     *ipMsg                       = srcData;
    VOS_UINT8                     *data                        = VOS_NULL_PTR;
    VOS_UINT8                      srcIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8                      srcMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8                      dstIPAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                      dstMacAddr[IP_MAC_ADDR_LEN];
    IP_NdserverTeAddrInfo         *teInfo                      = IP_NULL_PTR;
    VOS_UINT32                     datatemp; /* linux 4字节对齐 */
    VOS_INT32                      rlt;
    if (dataLen <= (IP_IPV6_HEAD_LEN + IP_ICMPV6_HEAD_LEN)) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_BuildTooBigICMPPkt: ulDataLen is too short.", dataLen);
        return IP_FAIL;
    }

    /* 根据自身MAC地址生成link-local地址 */
    rlt = memcpy_s(srcMacAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr, srcMacAddr);
    IP_SetUint16Data(srcIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);

    /* 得到目的IPV6地址 */
    rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* 得到目的MAC地址 */
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);
    rlt     = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, teInfo->teLinkLayerAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* 指向ICMP首部 */
    data = destData + IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* ICMP type域 */
    *data = IP_ICMPV6_PACKET_TOO_BIG;
    data++;

    /* ICMP code域 */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* ICMP 校验和域 */
    *(VOS_UINT16 *)(VOS_VOID *)data = 0;
    data += 2;

    /* MTU 域 */
    /*lint -e960*/
    datatemp = VOS_HTONL(dataLen); /* linux 4字节对齐 */
    rlt      = memcpy_s(data, sizeof(VOS_UINT32), (VOS_UINT8 *)(&datatemp), sizeof(VOS_UINT32)); /* linux 4字节对齐 */
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    /*lint +e960*/
    data += 4;

    /* 填写ICMP PayLoad部分 */
    rlt = memcpy_s(data, (IP_IPM_MTU - IP_ETHERNET_HEAD_LEN - IP_IPV6_HEAD_LEN - IP_ICMPV6_HEAD_LEN), ipMsg,
                     ((dataLen - IP_IPV6_HEAD_LEN) - IP_ICMPV6_HEAD_LEN));
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* 指向IPV6首部 */
    destData += IP_ETHERNET_HEAD_LEN;
    /* 填写IPV6头部 */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, (dataLen - IP_IPV6_HEAD_LEN), destData,
                            IP_HEAD_PROTOCOL_ICMPV6);

    /* 生成ICMPv6报头校验和 */
    if (IP_SUCC != IP_BuildIcmpv6Checksum(destData, IP_IPV6_HEAD_LEN)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormRaMsg: Build ICMPv6 Checksum failed.");
        return IP_FAIL;
    }

    /* 设置以太报头 */
    destData -= IP_ETHERNET_HEAD_LEN;
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, destData);

    return IP_SUCC;
}

/*
 * 功能描述: 处理PC发送的超长包
 * 修改历史:
 *  1.日    期: 2011年12月9日
 *    修改内容: 新生成函数
 */
VOS_VOID IP_NDSERVER_ProcTooBigPkt(VOS_UINT8 rabId, VOS_UINT8 *srcData, VOS_UINT32 dataLen)
{
    VOS_UINT32                  index;
    VOS_UINT8                  *ipMsg    = srcData;
    VOS_UINT8                  *sendBuff = VOS_NULL_PTR;
    IP_NdserverAddrInfo *infoAddr = IP_NULL_PTR;

    index = IP_NDSERVER_GET_ADDR_INFO_INDEX(rabId);
    if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTooBigPkt: ND Server info flag is invalid!");
        return;
    }

    IP_NDSERVER_AddRcvBooBigPktNum(index);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    if (IP_SUCC != IP_NDSERVER_BuildTooBigICMPPkt(index, ipMsg, sendBuff, dataLen)) {
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTooBigPkt: IP_NDSERVER_BuildTooBigICMPPkt FAIL!");
        return;
    }

    /* 将超长包响应发送到PC */
    if (PS_FAIL == Ndis_SendMacFrm(sendBuff, dataLen + IP_ETHERNET_HEAD_LEN, infoAddr->epsbId)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTooBigPkt:Send TooBigPkt failed.");
        return;
    }

    IP_NDSERVER_AddTransTooBigPktNum(index);
    IP_NDSERVER_AddTransPktTotalNum(index);

    return;
}

/*
 * Function Name: IP_NDSERVER_FormEchoReply
 * Description: 形成向PC回复的ECHO REPLY
 * Input: pucSrcData ----------- 源报文指针
 *                   pstDestData ---------- 目的转换结构指针
 *                   ulIcmpv6HeadOffset --- ICMPv6报头偏移量
 * History:
 *    1.      2011-12-09  Draft Enact
 */
VOS_UINT32 IP_NDSERVER_FormEchoReply(VOS_UINT32 index, VOS_UINT8 *srcData, VOS_UINT8 *destData,
                                     VOS_UINT32 dataLen)
{
    VOS_UINT8                     *ipMsg                       = srcData;
    VOS_UINT8                     *data                        = VOS_NULL_PTR;
    VOS_UINT8                      srcIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8                      srcMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8                      dstIPAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                      dstMacAddr[IP_MAC_ADDR_LEN];
    IP_NdserverTeAddrInfo         *teInfo                      = IP_NULL_PTR;
    VOS_INT32                      rlt;

    if (dataLen <= IP_IPV6_HEAD_LEN) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "IP_NDSERVER_FormEchoReply: ulDataLen is too short.", dataLen);
        return IP_FAIL;
    }

    /* 根据自身MAC地址生成link-local地址 */
    rlt = memcpy_s(srcMacAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr, srcMacAddr);
    IP_SetUint16Data(srcIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);

    /* 得到目的IPV6地址 */
    rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* 得到目的MAC地址 */
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);
    rlt     = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, teInfo->teLinkLayerAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* 指向ICMP首部 */
    data = destData + IP_ETHERNET_HEAD_LEN + IP_IPV6_HEAD_LEN;

    /* 填写ICMP报文 */
    ipMsg += IP_IPV6_HEAD_LEN;
    rlt = memcpy_s(data, (IP_IPM_MTU - IP_ETHERNET_HEAD_LEN - IP_IPV6_HEAD_LEN), ipMsg,
                     (dataLen - IP_IPV6_HEAD_LEN));
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, IP_FAIL);

    /* ICMP type域 */
    *data = IP_ICMPV6_TYPE_ECHOREPLY;
    data++;

    /* ICMP code域 */
    *data = IP_IPV6_ND_VALID_CODE;
    data++;

    /* ICMP 校验和域 */
    *(VOS_UINT16 *)(VOS_VOID *)data = 0;

    /* 指向IPV6首部 */
    destData += IP_ETHERNET_HEAD_LEN;
    /* 填写IPV6头部 */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, (dataLen - IP_IPV6_HEAD_LEN), destData,
                            IP_HEAD_PROTOCOL_ICMPV6);

    /* 生成ICMPv6报头校验和 */
    if (IP_SUCC != IP_BuildIcmpv6Checksum(destData, IP_IPV6_HEAD_LEN)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_FormEchoReply: Build ICMPv6 Checksum failed.");
        return IP_FAIL;
    }

    /* 设置以太报头 */
    destData -= IP_ETHERNET_HEAD_LEN;
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, destData);

    return IP_SUCC;
}

/*
 * Function Name: IP_NDSERVER_EchoRequestMsgProc
 * Description: 处理ECHO REQUEST消息
 * Input: ulIndex  -------------- 处理消息实体索引
 *                   pucSrcData ------------ IP数据报
 *                   ulIcmpv6HeadOffset ---- ICMPv6报文头偏移量
 * History:
 *    1.      2011-12-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_EchoRequestMsgProc(VOS_UINT32 index, VOS_UINT8 *srcData, VOS_UINT32 dataLen)
{
    VOS_UINT8                  *ipMsg                     = srcData;
    VOS_UINT8                  *sendBuff                  = VOS_NULL_PTR;
    IP_NdserverAddrInfo        *infoAddr                  = IP_NULL_PTR;
    VOS_UINT8                   pktAddr[IP_IPV6_ADDR_LEN];
    VOS_INT32                   rlt;
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_EchoRequestMsgProc is entered.");

    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);
    IP_NDSERVER_AddRcvEchoPktNum(index);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    if (IP_TRUE != infoAddr->validFlag) {
        IP_NDSERVER_AddErrEchoPktNum(index);
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_EchoRequestMsgProc: ND Server info flag is invalid!");
        return;
    }

    rlt = memcpy_s(pktAddr, IP_IPV6_ADDR_LEN, ipMsg + IP_IPV6_DST_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    if (IP_TRUE != IP_NDSERVER_IsSelfIPAddr(index, pktAddr)) {
        IP_NDSERVER_AddErrEchoPktNum(index);
        return;
    }

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    if (IP_SUCC != IP_NDSERVER_FormEchoReply(index, ipMsg, sendBuff, dataLen)) {
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_EchoRequestMsgProc: IP_NDSERVER_FormEchoReply fail!");
        return;
    }

    /* 将ECHO REPLY发送到PC */
    if (PS_FAIL == Ndis_SendMacFrm(sendBuff, dataLen + IP_ETHERNET_HEAD_LEN, infoAddr->epsbId)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_EchoRequestMsgProc:Send ECHO REPLY failed.");
        return;
    }

    IP_NDSERVER_AddTransEchoPktNum(index);
    IP_NDSERVER_AddTransPktTotalNum(index);

    return;
}

/*
 * Function Name: NdSer_NdAndEchoPktProc
 * Description: 接收PC发送的ND和ECHO REQUEST报文
 * History:
 *    1.      2011-12-09  Draft Enact
 *    2.          2013-1-15  DSDA
 */
VOS_VOID NdSer_NdAndEchoPktProc(VOS_VOID *rcvMsg)
{
    ADS_NDIS_DataInd           *adsNdisMsg       = (ADS_NDIS_DataInd *)rcvMsg;
    VOS_UINT32                  icmpv6HeadOffset = IP_NULL;
    IP_Icmpv6TypeUint32         icmpv6MsgType;
    VOS_UINT32                  index;
    VOS_UINT8                  *data             = VOS_NULL_PTR;
    VOS_UINT32                  dataLen;
    VOS_UINT8                   exRabId;
    IP_NdserverAddrInfo *nDSerAddrInfoTmp = VOS_NULL_PTR;

    IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_NdAndEchoPktProc is entered.");

    dataLen = IMM_ZcGetUsedLen(adsNdisMsg->data);
    data   = IMM_ZcGetDataPtr(adsNdisMsg->data);
    if (VOS_NULL_PTR == data) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_NdAndEchoPktProc, IMM_ZcGetDataPtr return NULL");
        return;
    }

    exRabId = ND_FORM_EXEPSID(adsNdisMsg->modemId, adsNdisMsg->rabId);

#if (FEATURE_OFF == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    if (IP_SUCC != ND_CheckEpsIdValid(exRabId)) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "NdSer_NdAndEchoPktProc, IMM_ZcGetDataPtr return NULL", exRabId);
        return;
    }
#endif
    index = IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);
    if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        /*
         * MT关机再开机后，ND SERVER收到网侧RA前，可能先收到TE的重复地址检测NS ，为了能够处理此类消息，
         * 此处临时申请一个ND Ser Addr Info
         */
        IPND_INFO_LOG1(NDIS_NDSERVER_PID, "NdSer_NdAndEchoPktProc: Get Addr Info, will alloc new addr info for NS msg",
                       adsNdisMsg->rabId);
        nDSerAddrInfoTmp = NdSer_AllocAddrInfo(&index);
        if (IP_NULL_PTR == nDSerAddrInfoTmp) {
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_AllocAddrInfo: return NULL!");
            return;
        }
        nDSerAddrInfoTmp->validFlag = IP_FALSE;
    }

    IP_NDSERVER_AddRcvPktTotalNum(index);

    /* 判断消息是否合法的ND消息 */
    if (IP_TRUE != IP_IsValidNdMsg(data, dataLen, &icmpv6HeadOffset)) {
        IP_NDSERVER_AddDiscPktNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_NdAndEchoPktProc: Invalid ND Msg!");
        return;
    }

    /* 取ICMPV6消息中的TYPE字段 */
    icmpv6MsgType = *(data + icmpv6HeadOffset);

    switch (icmpv6MsgType) {
        case IP_ICMPV6_TYPE_RS:
            IP_NDSERVER_RsMsgProc(index, data, icmpv6HeadOffset);
            break;
        case IP_ICMPV6_TYPE_NS:
            IP_NDSERVER_NsMsgProc(index, data, icmpv6HeadOffset);
            break;
        case IP_ICMPV6_TYPE_NA:
            IP_NDSERVER_NaMsgProc(index, data, icmpv6HeadOffset);
            break;
        case IP_ICMPV6_TYPE_ECHOREQUEST:
            IP_NDSERVER_EchoRequestMsgProc(index, data, dataLen);
            break;

        default:
            IP_NDSERVER_AddDiscPktNum(index);
            IPND_WARNING_LOG1(NDIS_NDSERVER_PID, "NdSer_NdAndEchoPktProc: Ignored IPV6 ND Msg:!", icmpv6MsgType);
            break;
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_SendDhcp6Reply
 * Description: 组装并发送DHCPV6报文
 * History:
 *    1.          2011-12-6  Draft Enact
 */
/*lint -e778*/
/*lint -e572*/
VOS_UINT32 IP_NDSERVER_SendDhcp6Reply(VOS_UINT32 index, const ND_IP_Ipv6DnsSer *dnsSer,
                                      VOS_UINT8 *dhcpInfoReqData, VOS_UINT16 reqIp6PktLen)
{
    NDIS_Ipv6Hdr                  *dhcpReplyIpv6Hdr            = VOS_NULL_PTR;
    IP_UdpHeadInfo                *dhcpReplyUdpHdr             = VOS_NULL_PTR;
    IP_Ipv6DhcpDnsOption           ipv6DhcpDnsOpt;
    IP_Ipv6PktDhcpOptHdr          *dhcpClientIdOpt             = VOS_NULL_PTR;
    IP_Ipv6PktDhcpOptHdr          *dhcpRequestOpt              = VOS_NULL_PTR;
    IP_Ipv6PktDhcpDuidLLOpt        dhcpDuidLLOpt;
    VOS_UINT8                     *dhcpReplyDhcpHdrOffset      = VOS_NULL_PTR; /* 移动指针 */
    VOS_UINT16                     replyUdpDataLen;
    VOS_UINT16                     dhcpReplyPktLen;
    VOS_UINT16                     dnsOptLen                   = 0;
    VOS_UINT16                     reqDhcpOptLen;
    VOS_UINT16                     dhcpClientIdOptLen;
    VOS_UINT16                     dhcpRequestDnsOptLen        = 0;
    VOS_UINT32                     dhcpReqOptCodeLoopCnt;
    IP_Ipv6PktDhcpOptHdr          *reqDhcpOptHdr               = VOS_NULL_PTR;
    VOS_UINT8                      srcIPAddr[IP_IPV6_ADDR_LEN] = {IP_NULL};
    VOS_UINT8                      srcMacAddr[IP_MAC_ADDR_LEN];
    VOS_UINT8                      dstIPAddr[IP_IPV6_ADDR_LEN];
    VOS_UINT8                      dstMacAddr[IP_MAC_ADDR_LEN];
    IP_NdserverTeAddrInfo         *teInfo                      = IP_NULL_PTR;
    VOS_UINT8                     *sendBuff                    = VOS_NULL_PTR;
    VOS_UINT8                      epsId;
    VOS_UINT32                     tmpDestLen;
    errno_t                        rlt;
    if (reqIp6PktLen <= ((IP_IPV6_HEAD_LEN + IP_UDP_HEAD_LEN) + IP_UDP_DHCP_HDR_SIZE)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_SendDhcp6Reply, No Option Len!");
        return PS_FAIL;
    }

    /* 20160122 V722 coverity --begin */
    (VOS_VOID)memset_s(&ipv6DhcpDnsOpt, sizeof(IP_Ipv6DhcpDnsOption), 0, sizeof(IP_Ipv6DhcpDnsOption));

    (VOS_VOID)memset_s(&dhcpDuidLLOpt, sizeof(IP_Ipv6PktDhcpDuidLLOpt), 0,
                       sizeof(IP_Ipv6PktDhcpDuidLLOpt));

    /* 20160122 V722 coverity --end */

    reqDhcpOptLen   = reqIp6PktLen - ((IP_IPV6_HEAD_LEN + IP_UDP_HEAD_LEN) + IP_UDP_DHCP_HDR_SIZE);
    replyUdpDataLen = IP_UDP_HEAD_LEN + IP_UDP_DHCP_HDR_SIZE + IP_IPV6_DHCP_OPT_CLIENT_ID_LEN;
    reqDhcpOptHdr  = (IP_Ipv6PktDhcpOptHdr *)(VOS_VOID *)(dhcpInfoReqData + IP_IPV6_HEAD_LEN +
                                                                  IP_UDP_HEAD_LEN + IP_UDP_DHCP_HDR_SIZE);

    /* 获取 DHCP Client ID Option */
    (VOS_VOID)TTF_NDIS_Ipv6GetDhcpOption(reqDhcpOptHdr, reqDhcpOptLen, IP_IPV6_DHCP_OPT_CLIEND_ID,
                                         (IP_Ipv6PktDhcpOptHdr **)&dhcpClientIdOpt, 0);

    if (VOS_NULL_PTR != dhcpClientIdOpt) {
        dhcpClientIdOptLen = VOS_NTOHS(dhcpClientIdOpt->dhcpOptLen);
        replyUdpDataLen += (dhcpClientIdOptLen + 4);/* 4表示2个字节的option-code+2字节的option-len */
    }

    /* 获取 DHCP Request Option */
    (VOS_VOID)TTF_NDIS_Ipv6GetDhcpOption(reqDhcpOptHdr, reqDhcpOptLen, IP_IPV6_DHCP_OPT_REQUEST,
                                         (IP_Ipv6PktDhcpOptHdr **)&dhcpRequestOpt, 0);

    if (VOS_NULL_PTR != dhcpRequestOpt) {
        for (dhcpReqOptCodeLoopCnt = 0; dhcpReqOptCodeLoopCnt < (dhcpRequestOpt->dhcpOptLen);
             dhcpReqOptCodeLoopCnt += 2) {
            if (IP_IPV6_DHCP6_OPT_DNS_SERVERS ==
                VOS_NTOHS(*((VOS_UINT16 *)(&(dhcpRequestOpt->dhcpOptData[dhcpReqOptCodeLoopCnt]))))) {
                dnsOptLen = dnsSer->dnsSerNum;
                if ((0 == dnsOptLen) || (IP_IPV6_MAX_DNS_NUM < dnsOptLen)) {
                    IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_SendDhcp6Reply, DNS Option is err!");
                    return PS_FAIL;
                }

                dhcpRequestDnsOptLen = (VOS_UINT16)(dnsOptLen * sizeof(NDIS_Ipv6Addr));
                replyUdpDataLen += (dhcpRequestDnsOptLen + 4);/* 4表示2个字节的option-code+2字节的option-len */
                break;
            }
        }
    }

    if (replyUdpDataLen <= (IP_UDP_HEAD_LEN + IP_UDP_DHCP_HDR_SIZE + IP_IPV6_DHCP_OPT_CLIENT_ID_LEN)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_SendDhcp6Reply, No content need to reply!");
        return PS_SUCC;
    }

    dhcpReplyPktLen = IP_IPV6_HEAD_LEN + replyUdpDataLen;

    /* 根据自身MAC地址生成link-local地址 */
    rlt = memcpy_s(srcMacAddr, IP_MAC_ADDR_LEN, (VOS_VOID *)Ndis_GetMacAddr(), IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
    IP_ProduceIfaceIdFromMacAddr(srcIPAddr, srcMacAddr);
    IP_SetUint16Data(srcIPAddr, IP_IPV6_LINK_LOCAL_PREFIX);

    /* 得到目的IPV6地址 */
    rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, dhcpInfoReqData + IP_IPV6_SRC_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    /* 得到目的MAC地址 */
    teInfo = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);
    rlt     = memcpy_s(dstMacAddr, IP_MAC_ADDR_LEN, teInfo->teLinkLayerAddr, IP_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    sendBuff = IP_NDSERVER_GET_SENDMSG_BUFFER();
    (VOS_VOID)memset_s(sendBuff, IP_IPM_MTU, IP_NULL, IP_IPM_MTU);

    /* 指向IPV6首部 */
    sendBuff += IP_ETHERNET_HEAD_LEN;

    dhcpReplyIpv6Hdr     = (NDIS_Ipv6Hdr *)(sendBuff);
    dhcpReplyUdpHdr      = (IP_UdpHeadInfo *)(dhcpReplyIpv6Hdr + 1);
    dhcpReplyDhcpHdrOffset = (VOS_UINT8 *)(dhcpReplyUdpHdr + 1);

    /* 填充 IP 头 */
    IP_ND_FormIPv6HeaderMsg(srcIPAddr, dstIPAddr, replyUdpDataLen, (VOS_UINT8 *)dhcpReplyIpv6Hdr,
                            IP_HEAD_PROTOCOL_UDP);

    /* 填充 UDP 头 */
    (VOS_VOID)TTF_NDIS_InputUDPHead((VOS_UINT8 *)dhcpReplyUdpHdr, IP_IPV6_DHCP6_UE_PORT, IP_IPV6_DHCP6_PC_PORT,
                                    replyUdpDataLen);

    /* 填充 DHCP reply 头 */
    (*dhcpReplyDhcpHdrOffset) = IP_IPV6_DHCP6_TYPE_REPLY;
    dhcpReplyDhcpHdrOffset += 1;
    rlt = memcpy_s(dhcpReplyDhcpHdrOffset, IP_IPV6_DHCP6_TRANS_ID_LEN,
                     ((dhcpInfoReqData + IP_IPV6_HEAD_LEN) + IP_UDP_HEAD_LEN) + 1, IP_IPV6_DHCP6_TRANS_ID_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    dhcpReplyDhcpHdrOffset += IP_IPV6_DHCP6_TRANS_ID_LEN;

    /* 填充 DHCP ServerID option (DUID 使用第三种方式即 DUID-LL) */
    dhcpDuidLLOpt.dhcpDuidType   = VOS_HTONS(IP_IPV6_DHCP_DUID_LL_OPT_TYPE);
    dhcpDuidLLOpt.dhcpDuidHWType = VOS_HTONS(IP_IPV6_HW_TYPE);
    rlt = memcpy_s(dhcpDuidLLOpt.linkLayerAddr, IP_IPV6_DHCP_OPT_LINKADDR_SIZE, srcMacAddr,
                     IP_IPV6_DHCP_OPT_LINKADDR_SIZE);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    (*(VOS_UINT16 *)dhcpReplyDhcpHdrOffset) = VOS_HTONS(IP_IPV6_DHCP_OPT_SERVER_ID);
    dhcpReplyDhcpHdrOffset += sizeof(VOS_UINT16);
    (*(VOS_UINT16 *)dhcpReplyDhcpHdrOffset) = VOS_HTONS(IP_IPV6_DHCP_DUID_LL_OPT_LEN);
    dhcpReplyDhcpHdrOffset += sizeof(VOS_UINT16);
    rlt = memcpy_s(dhcpReplyDhcpHdrOffset, IP_IPV6_DHCP_DUID_LL_OPT_LEN, (VOS_UINT8 *)(&dhcpDuidLLOpt),
                     IP_IPV6_DHCP_DUID_LL_OPT_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    dhcpReplyDhcpHdrOffset += IP_IPV6_DHCP_DUID_LL_OPT_LEN;

    /* 填充 DHCP CLient ID option */
    tmpDestLen = IP_IPM_MTU - IP_ETHERNET_HEAD_LEN - sizeof(NDIS_Ipv6Hdr) - sizeof(IP_UdpHeadInfo) -
                   IP_IPV6_DHCP6_REPLY_HDR_LEN - IP_IPV6_DHCP_DUID_LL_OPT_LEN;
    if (VOS_NULL_PTR != dhcpClientIdOpt) {
        /* 4表示2个字节的option-code+2字节的option-len */
        if (((VOS_UINT32)(VOS_NTOHS(dhcpClientIdOpt->dhcpOptLen) + 4)) > tmpDestLen) {
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_SendDhcp6Reply, DHCP CLient ID option is err!");
            return PS_FAIL;
        }
        rlt = memcpy_s(dhcpReplyDhcpHdrOffset, tmpDestLen, (VOS_UINT8 *)dhcpClientIdOpt,
                         (VOS_NTOHS(dhcpClientIdOpt->dhcpOptLen) + 4));
        IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
        dhcpReplyDhcpHdrOffset += (VOS_NTOHS(dhcpClientIdOpt->dhcpOptLen) + 4);
        tmpDestLen -= TTF_MIN(tmpDestLen, (VOS_UINT32)(VOS_NTOHS(dhcpClientIdOpt->dhcpOptLen) + 4));
    }

    /* 填充 DHCP DNS OPTION */
    if (0 != dhcpRequestDnsOptLen) {
        ipv6DhcpDnsOpt.optionCode = VOS_HTONS(IP_IPV6_DHCP6_OPT_DNS_SERVERS);
        rlt = memcpy_s(&ipv6DhcpDnsOpt.ipv6DNS[0], (sizeof(NDIS_Ipv6Addr)), dnsSer->priDnsServer,
                         (sizeof(NDIS_Ipv6Addr)));
        IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

        if (IP_IPV6_MAX_DNS_NUM == dnsOptLen) {
            rlt = memcpy_s(&ipv6DhcpDnsOpt.ipv6DNS[1], (sizeof(NDIS_Ipv6Addr)), dnsSer->secDnsServer,
                             (sizeof(NDIS_Ipv6Addr)));
            IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
        }
        ipv6DhcpDnsOpt.optionLen = VOS_HTONS((VOS_UINT16)(IP_IPV6_ADDR_LEN * dnsOptLen));
        rlt = memcpy_s(dhcpReplyDhcpHdrOffset, tmpDestLen, (VOS_UINT8 *)&ipv6DhcpDnsOpt,
                         (4 + (dnsOptLen * sizeof(NDIS_Ipv6Addr))));
        IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

        dhcpReplyDhcpHdrOffset += (4 + (dnsOptLen * sizeof(NDIS_Ipv6Addr)));
    }

    dhcpReplyUdpHdr->udpCheckSum = TTF_NDIS_Ipv6_CalcCheckSum((VOS_UINT8 *)dhcpReplyUdpHdr, replyUdpDataLen,
                                                             &dhcpReplyIpv6Hdr->srcAddr, &dhcpReplyIpv6Hdr->dstAddr,
                                                             IP_HEAD_PROTOCOL_UDP);

    /* 指向以太网首部 */
    sendBuff -= IP_ETHERNET_HEAD_LEN;
    IP_NDSERVER_FormEtherHeaderMsg(srcMacAddr, dstMacAddr, sendBuff);

    dhcpReplyPktLen += IP_ETHERNET_HEAD_LEN;

    IP_NDSERVER_AddTransDhcpv6PktNum(index);
    IP_NDSERVER_AddTransPktTotalNum(index);

    epsId = IP_NDSERVER_ADDRINFO_GET_EPSID(index);

    /* 限制reply消息长度 */
    dhcpReplyPktLen = TTF_MIN(dhcpReplyPktLen, IP_IPM_MTU);

    /* 将DHCPV6 REPLY消息发送到PC */
    return Ndis_SendMacFrm(sendBuff, dhcpReplyPktLen, epsId);
}
/*lint +e778*/
/*lint +e572*/

/*
 * Function Name: NdSer_DhcpV6PktProc
 * Description: 处理DHCPV6报文
 * History:
 *    1.          2011-12-6  Draft Enact
 *    2.          2013-1-15  DSDA
 */
VOS_VOID NdSer_DhcpV6PktProc(VOS_VOID *rcvMsg)
{
    VOS_UINT16                  srcPort;
    VOS_UINT16                  dstPort;
    VOS_UINT8                   msgType;
    VOS_UINT8                  *udpData    = VOS_NULL_PTR;

    ADS_NDIS_DataInd           *adsNdisMsg = (ADS_NDIS_DataInd *)rcvMsg;
    VOS_UINT32                  index;
    VOS_UINT8                  *data       = VOS_NULL_PTR;
    VOS_UINT32                  dataLen;
    IP_NdserverAddrInfo        *infoAddr   = IP_NULL_PTR;
    ND_IP_Ipv6DnsSer           *dnsSer     = VOS_NULL_PTR;
    VOS_UINT8                   exRabId;

    IPND_INFO_LOG(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc is entered.");

    dataLen = IMM_ZcGetUsedLen(adsNdisMsg->data);
    data   = IMM_ZcGetDataPtr(adsNdisMsg->data);
    if (VOS_NULL_PTR == data) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc, IMM_ZcGetDataPtr return NULL!");
        return;
    }

    exRabId = ND_FORM_EXEPSID(adsNdisMsg->modemId, adsNdisMsg->rabId);

#if (FEATURE_OFF == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    if (IP_SUCC != ND_CheckEpsIdValid(exRabId)) {
        IPND_ERROR_LOG1(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc, ucRabId is Invalid!", exRabId);
        return;
    }
#endif

    index = IP_NDSERVER_GET_ADDR_INFO_INDEX(exRabId);
    if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        IPND_ERROR_LOG2(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc: Invalid Input:", exRabId, index);
        return;
    }

    IP_NDSERVER_AddRcvDHCPV6PktNum(index);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    udpData = data + IP_IPV6_HEAD_LEN;
    srcPort  = *(VOS_UINT16 *)(VOS_VOID *)udpData;
    dstPort  = *(VOS_UINT16 *)(VOS_VOID *)(udpData + 2); /* 从srcport偏移两个字节获取dstport */

    if ((IP_IPV6_DHCP6_PC_PORT != VOS_NTOHS(srcPort)) || (IP_IPV6_DHCP6_UE_PORT != VOS_NTOHS(dstPort))) {
        IP_NDSERVER_AddErrDhcpv6PktNum(index);
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc, usSrcPort, usDstPort is err!", srcPort, dstPort);
        return;
    }

    msgType = *(udpData + IP_UDP_HEAD_LEN);
    if (IP_IPV6_DHCP6_INFOR_REQ != msgType) {
        IP_NDSERVER_AddErrDhcpv6PktNum(index);
        IPND_WARNING_LOG1(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc, ucMsgType is err!", msgType);
        return;
    }

    dnsSer = &(infoAddr->ipv6NwPara.dnsSer);

    /* Nd Server没有DNS情况下收到Dhcpv6 information request,丢弃该报文 */
    if (0 == dnsSer->dnsSerNum) {
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "NdSer_DhcpV6PktProc, No DNS information exists");
        return;
    }

    (VOS_VOID)IP_NDSERVER_SendDhcp6Reply(index, dnsSer, data, (VOS_UINT16)dataLen);

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgNsExp
 * Description: 发送邻居请求后，等待邻居公告超时
 * Input: pMsg -------- 消息指针
 * History:
 *      1.      2011-04-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_ProcTimerMsgNsExp(const VOS_VOID *ipMsg)
{
    VOS_UINT32              index;
    IP_NdserverAddrInfo    *infoAddr = IP_NULL_PTR;
    IP_NdserverTeAddrInfo  *teInfo   = IP_NULL_PTR;

    /* 打印进入该函数 */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp is entered.");

    /* 从消息中取出ND SERVER索引 */
    index = IP_GetTimerPara(ipMsg);
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    teInfo   = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);

    if ((IP_TRUE != infoAddr->validFlag) ||
        (IP_NDSERVER_TE_ADDR_INEXISTENT == infoAddr->teAddrInfo.teAddrState)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp: ND Server state err!");
        return;
    }

    /* 判定超时次数是否小于规定超时次数 */
    if (infoAddr->timerInfo.loopTimes < g_ulNsTimerMaxExpNum) {
        /* 发送NS消息到PC */
        if (PS_SUCC != IP_NDSERVER_SendNsMsg(index, teInfo->teGlobalAddr)) {
            IP_NDSERVER_AddTransPktFailNum(index);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp:Send NS Msg failed.");
        }

        /* 将定时器超时次数加1 */
        infoAddr->timerInfo.loopTimes++;

        /* 重启邻居请求定时器 */
        IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_NS);
    } else {
        /* 清除定时器信息 */
        infoAddr->timerInfo.loopTimes = 0;

        /* 如果未完成状态的TE地址不响应地址解析请求，则清除TE地址，重发RA */
        if (IP_NDSERVER_TE_ADDR_INCOMPLETE == infoAddr->teAddrInfo.teAddrState) {
            infoAddr->teAddrInfo.teAddrState = IP_NDSERVER_TE_ADDR_INEXISTENT;

            /* 发送RA消息到PC */
            if (PS_SUCC != IP_NDSERVER_SendRaMsg(index, VOS_NULL_PTR)) {
                IP_NDSERVER_AddTransPktFailNum(index);
                IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgNsExp:Send RA Msg failed.");
            }

            /* 启动路由公告定时器 */
            IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_RA);

            IP_NDSERVER_ClearDlPktQue(index); /* 清除下行缓存队列中的PKT */
        } else {
            /* 启动周期性邻居请求定时器 */
            IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_PERIODIC_NS);
        }
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgPeriodicNsExp
 * Description: 周期性邻居请求超时
 * Input: pMsg -------- 消息指针
 * History:
 *      1.      2011-04-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_ProcTimerMsgPeriodicNsExp(const VOS_VOID *ipMsg)
{
    VOS_UINT32              index;
    IP_NdserverAddrInfo    *infoAddr = IP_NULL_PTR;
    IP_NdserverTeAddrInfo  *teInfo   = IP_NULL_PTR;

    /* 打印进入该函数 */
    IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp is entered.");

    /* 从消息中取出ND SERVER索引 */
    index = IP_GetTimerPara(ipMsg);
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    teInfo   = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);

    if ((IP_TRUE != infoAddr->validFlag) ||
        (IP_NDSERVER_TE_ADDR_INEXISTENT == infoAddr->teAddrInfo.teAddrState)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp: ND Server state err!");
        return;
    }

    /* 增加周期性发送RA计时，防止Router过期 */
    if (0 == g_aulPeriodicRaTimeCnt[index]--) {
        g_aulPeriodicRaTimeCnt[index] = g_ulPeriodicRaTimerLen / g_ulPeriodicNsTimerLen;
        ;

        /* 发送RA消息到PC */
        if (PS_SUCC != IP_NDSERVER_SendRaMsg(index, VOS_NULL_PTR)) {
            IP_NDSERVER_AddTransPktFailNum(index);
            IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp:Send RA Msg failed.");
        }
    }

    /* 发送NS消息到PC */
    if (PS_SUCC != IP_NDSERVER_SendNsMsg(index, teInfo->teGlobalAddr)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgPeriodicNsExp:Send NS Msg failed.");
    }

    /* 清除定时器信息 */
    infoAddr->timerInfo.loopTimes = 0;
    /* 启动邻居请求定时器 */
    IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_NS);

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgPeriodicNsExp
 * Description: 收到重复地址检测后等待定时器超时
 * Input: pMsg -------- 消息指针
 * History:
 *      1.      2011-04-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_ProcTimerMsgFirstNsExp(const VOS_VOID *ipMsg)
{
    VOS_UINT32              index;
    IP_NdserverAddrInfo    *infoAddr = IP_NULL_PTR;
    IP_NdserverTeAddrInfo  *teInfo   = IP_NULL_PTR;

    /* 打印进入该函数 */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgFirstNsExp is entered.");

    /* 从消息中取出ND SERVER索引 */
    index = IP_GetTimerPara(ipMsg);
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);
    teInfo   = IP_NDSERVER_ADDRINFO_GET_TEINFO(index);

    if ((IP_TRUE != infoAddr->validFlag) ||
        (IP_NDSERVER_TE_ADDR_INEXISTENT == infoAddr->teAddrInfo.teAddrState)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgFirstNsExp: ND Server state err!");
        return;
    }

    /* 发送NS消息到PC */
    if (PS_SUCC != IP_NDSERVER_SendNsMsg(index, teInfo->teGlobalAddr)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgFirstNsExp:Send NS Msg failed.");
    }

    /* 清除定时器信息 */
    infoAddr->timerInfo.loopTimes = 0;
    /* 启动邻居请求定时器 */
    IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_NS);

    return;
}

/*
 * Function Name: IP_NDSERVER_ProcTimerMsgRaExp
 * Description: 收到重复地址检测前周期发送路由公告超时
 * Input: pMsg -------- 消息指针
 * History:
 *      1.      2011-04-09  Draft Enact
 */
VOS_VOID IP_NDSERVER_ProcTimerMsgRaExp(const VOS_VOID *ipMsg)
{
    VOS_INT32                   rlt;
    VOS_UINT32                  index;
    IP_NdserverAddrInfo        *infoAddr                    = IP_NULL_PTR;
    VOS_UINT8                   dstIPAddr[IP_IPV6_ADDR_LEN];
    IMM_Zc                     *immZc                       = VOS_NULL_PTR;
#if (VOS_OS_VER != VOS_WIN32)
    VOS_UINT8                  *data                        = VOS_NULL_PTR;
    VOS_INT32                   lockKey;
#endif
    /* 打印进入该函数 */
    IPND_INFO_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp is entered.");

    /* 从消息中取出ND SERVER索引 */
    index = IP_GetTimerPara(ipMsg);
    IP_ASSERT(index < IP_NDSERVER_ADDRINFO_MAX_NUM);

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    if (IP_TRUE != infoAddr->validFlag) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp: ND Server state err!");
        return;
    }

    /* 发送RA消息到PC */
    if (PS_SUCC != IP_NDSERVER_SendRaMsg(index, VOS_NULL_PTR)) {
        IP_NDSERVER_AddTransPktFailNum(index);
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp:Send RA Msg failed.");
    }

    /* 清除定时器信息 */
    infoAddr->timerInfo.loopTimes = 0;
    /* 启动路由公告定时器 */
    IP_NDSERVER_TimerStart(index, IP_ND_SERVER_TIMER_RA);

#if (VOS_OS_VER != VOS_WIN32)
    /* *********** 下行IP包的目的地址作为NS包的目标地址，进行地址解析 ********* */
    lockKey = VOS_SplIMP();
    if (PS_SUCC != LUP_PeekQueHead(IP_NDSERVER_ADDRINFO_GET_DLPKTQUE(index), (VOS_VOID **)(&immZc))) {
        VOS_Splx(lockKey);
        return;
    }
    VOS_Splx(lockKey);

    data = IMM_ZcGetDataPtr(immZc);
    if (VOS_NULL_PTR == data) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp, IMM_ZcGetDataPtr return NULL");
        return;
    }

    /* 得到目的IPV6地址，触发NS发送 */
    rlt = memcpy_s(dstIPAddr, IP_IPV6_ADDR_LEN, data + IP_IPV6_DST_ADDR_OFFSET, IP_IPV6_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    if (PS_SUCC != IP_NDSERVER_SendNsMsg(index, dstIPAddr)) {
        IPND_ERROR_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_ProcTimerMsgRaExp, IP_NDSERVER_SendNsMsg return NULL");
        return;
    }
#endif

    return;
}

/*
 * Function Name: IP_NDSERVER_TimerMsgDistr
 * Description: ND SERVER TIMER消息分发函数
 * History:
 *      1.     2011-04-02  Draft Enact
 */
NDSER_TimerUint32 IP_NDSERVER_TimerMsgDistr(const VOS_VOID *rcvMsg)
{
    VOS_UINT32              index;
    NDSER_TimerUint32       timerType;
    IP_TimerInfo           *timerInfo = VOS_NULL_PTR;

    /* 从消息中取出ulIndex和enTimerType */
    index     = IP_GetTimerPara(rcvMsg);
    timerType = (NDSER_TimerUint32)IP_GetTimerName(rcvMsg);

    /* 判断合法性 */
    if (IP_FALSE == IP_NDSERVER_IsTimerNameValid(index, timerType)) {
        /* 打印异常信息 */
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: Invalid Timer Name or Para !");
        return IP_MSG_DISCARD;
    }

    /* 根据消息对应的索引号和定时器类型,获取相关联的定时器 */
    timerInfo = IP_NDSERVER_GetTimer(index, timerType);
    if (timerInfo == VOS_NULL_PTR) {
        /* 打印异常信息 */
        IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: Get Timer failed.");
        return IP_MSG_DISCARD;
    }

    /* 判断消息类型与定时器类型是否一致 */
    if (timerType != timerInfo->timerName) {
        /* 打印异常信息 */
        IPND_WARNING_LOG2(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: TimerType not match:", timerType,
                          timerInfo->timerName);
        return IP_MSG_DISCARD;
    }

    /* 定时器超时处理 */
    switch (timerType) {
        case IP_ND_SERVER_TIMER_NS:
            IP_NDSERVER_ProcTimerMsgNsExp(rcvMsg);
            break;
        case IP_ND_SERVER_TIMER_PERIODIC_NS:
            IP_NDSERVER_ProcTimerMsgPeriodicNsExp(rcvMsg);
            break;
        case IP_ND_SERVER_TIMER_FIRST_NS:
            IP_NDSERVER_ProcTimerMsgFirstNsExp(rcvMsg);
            break;
        case IP_ND_SERVER_TIMER_RA:
            IP_NDSERVER_ProcTimerMsgRaExp(rcvMsg);
            break;
        default:
            IPND_WARNING_LOG(NDIS_NDSERVER_PID, "IP_NDSERVER_TimerMsgDistr: Illegal Timer Type!");
            break;
    }

    return IP_MSG_HANDLED;
}

/*
 * 功能描述: PP NDIS上行PID消息处理函数
 * 修改历史:
 *  1.日    期: 2012年12月9日
 *    修改内容: 新生成函数
 */
VOS_VOID APP_NdServer_PidMsgProc(MsgBlock *rcvMsg)
{
    if (VOS_NULL_PTR == rcvMsg) {
        PS_PRINTF_INFO("Error:APP_Ndis_DLPidMsgProc Parameter pRcvMsg is NULL!");
        return;
    }
    switch (rcvMsg->ulSenderPid) {
        case DOPRA_PID_TIMER:
            (VOS_VOID)IP_NDSERVER_TimerMsgDistr(rcvMsg);
            break;
        default:
            break;
    }

    return;
}

/*
 * Function Name: IP_NDSERVER_CmdHelp
 * Description: IP NDSERVER模块命令显示
 * History:
 *    1.      2011-04-11  Draft Enact
 */
VOS_VOID IP_NDSERVER_CmdHelp(VOS_VOID)
{
    PS_PRINTF_ERR("\r\n");
    PS_PRINTF_ERR("********************** IP NDSERVER CMD LIST*********************\r\n");
    PS_PRINTF_ERR("%-30s : %s\r\n", "IP_NDSERVER_ShowLocalNwParamInfo", "show local net para");
    PS_PRINTF_ERR("%-30s : %s\r\n", "IP_NDSERVER_ShowAddrInfo(index)", "show addr info(0)");
    PS_PRINTF_ERR("%-30s : %s\r\n", "IP_NDSERVER_ShowStatInfo(index)", "show stat info(0)");
    PS_PRINTF_ERR("*******************************************************************\r\n");
    PS_PRINTF_ERR("\r\n");

    return;
}

/*
 * Function Name: IP_NDSERVER_ShowLocalNwParamInfo
 * Description: 显示本地保存的网络参数信息
 * History:
 *    1.      2011-04-11  Draft Enact
 */
VOS_VOID IP_NDSERVER_ShowLocalNwParamInfo(VOS_VOID)
{
    PS_PRINTF_ERR("************************Local Nw Param Info***********************\r\n");
    PS_PRINTF_ERR("manage addr config flag %d\r\n", g_ucMFlag);
    PS_PRINTF_ERR("other stated config flag %d\r\n", g_ucOFlag);
    PS_PRINTF_ERR("router lifetime(s) %d\r\n", g_usRouterLifetime);
    PS_PRINTF_ERR("Reachable Time(ms) %d\r\n", g_ulReachableTime);
    PS_PRINTF_ERR("Retrans Timer(ms) %d\r\n", g_ulRetransTimer);
    PS_PRINTF_ERR("Neighbor solicition TimerLen %d\r\n", g_ulNsTimerLen);
    PS_PRINTF_ERR("Neighbor solicition Timer Max Timerout Num %d\r\n", g_ulNsTimerMaxExpNum);
    PS_PRINTF_ERR("Periodic NsTimer Len %d\r\n", g_ulPeriodicNsTimerLen);
    PS_PRINTF_ERR("PeriodicRaTimerLen(ms) %d\r\n", g_ulPeriodicRaTimerLen);
    PS_PRINTF_ERR("FirstNsTimerLen(ms) %d\r\n", g_ulFirstNsTimerLen);
    PS_PRINTF_ERR("RaTimerLen(ms) %d\r\n", g_ulRaTimerLen);

    return;
}

/*
 * Function Name: IP_NDSERVER_ShowAddrInfo
 * Description: 显示某实体地址参数信息
 * Input: ulIndex --- 实体索引
 * History:
 *    1.      2011-04-11  Draft Enact
 */
VOS_VOID IP_NDSERVER_ShowAddrInfo(VOS_UINT32 index)
{
    IP_NdserverAddrInfo        *infoAddr = IP_NULL_PTR;
    VOS_UINT32                  cnt;
    VOS_UINT32                  tmp;

    if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        tmp = IP_NDSERVER_ADDRINFO_MAX_NUM;
        PS_PRINTF_ERR("IP_NDSERVER_ShowAddrInfo:input para range:0-%d\r\n", tmp - 1);
        return;
    }

    infoAddr = IP_NDSERVER_ADDRINFO_GET_ADDR(index);

    PS_PRINTF_ERR("**************************ND SERVER entity info(%d)*************************\r\n", index);
    PS_PRINTF_ERR("valid flag %d\r\n", infoAddr->validFlag);
    PS_PRINTF_ERR("EpsbId %d\r\n", infoAddr->epsbId);

    PS_PRINTF_ERR("************Net Config Para************\r\n");
    PS_PRINTF_ERR("MTU: %d\r\n", infoAddr->ipv6NwPara.opMtu ? infoAddr->ipv6NwPara.mtuValue : 0);
    PS_PRINTF_ERR("CurHopLimit %d\r\n", infoAddr->ipv6NwPara.curHopLimit);
    PS_PRINTF_ERR("interface ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                  infoAddr->ipv6NwPara.interfaceId[0], infoAddr->ipv6NwPara.interfaceId[1],
                  infoAddr->ipv6NwPara.interfaceId[2], infoAddr->ipv6NwPara.interfaceId[3],
                  infoAddr->ipv6NwPara.interfaceId[4], infoAddr->ipv6NwPara.interfaceId[5],
                  infoAddr->ipv6NwPara.interfaceId[6], infoAddr->ipv6NwPara.interfaceId[7]);

    PS_PRINTF_ERR("************prefix list************\r\n");
    PS_PRINTF_ERR("Prefix Num: %d\r\n", infoAddr->ipv6NwPara.prefixNum);
    for (cnt = 0; cnt < infoAddr->ipv6NwPara.prefixNum; cnt++) {
        PS_PRINTF_ERR("Prefix Len: %d\r\n", infoAddr->ipv6NwPara.prefixList[cnt].prefixLen);
        PS_PRINTF_ERR("autonomy flag: %d\r\n", infoAddr->ipv6NwPara.prefixList[cnt].prefixAValue);
        PS_PRINTF_ERR("online flag %d\r\n", infoAddr->ipv6NwPara.prefixList[cnt].prefixLValue);
        PS_PRINTF_ERR("ValidLifeTime: %d\r\n", infoAddr->ipv6NwPara.prefixList[cnt].validLifeTime);
        PS_PRINTF_ERR("PreferredLifeTime: %d\r\n", infoAddr->ipv6NwPara.prefixList[cnt].preferredLifeTime);
        PS_PRINTF_ERR("prefix %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[0],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[1],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[2],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[3],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[4],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[5],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[6],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[7],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[8],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[9],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[10],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[11],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[12],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[13],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[14],
                      infoAddr->ipv6NwPara.prefixList[cnt].prefix[15]);
    }

    PS_PRINTF_ERR("************DNS SERVER LIST************\r\n");
    PS_PRINTF_ERR("DNS SERVER NUM: %d\r\n", infoAddr->ipv6NwPara.dnsSer.dnsSerNum);
    if (infoAddr->ipv6NwPara.dnsSer.dnsSerNum >= 1) {
        PS_PRINTF_ERR(
            "Prime DNS SERVER: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[0],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[1],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[2],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[3],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[4],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[5],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[6],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[7],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[8],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[9],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[10],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[11],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[12],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[13],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[14],
            infoAddr->ipv6NwPara.dnsSer.priDnsServer[15]);
    }
    if (infoAddr->ipv6NwPara.dnsSer.dnsSerNum >= 2) {
        PS_PRINTF_ERR(
            "Second DNS SERVER: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[0],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[1],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[2],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[3],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[4],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[5],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[6],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[7],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[8],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[9],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[10],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[11],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[12],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[13],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[14],
            infoAddr->ipv6NwPara.dnsSer.secDnsServer[15]);
    }
    PS_PRINTF_ERR("************SIP SERVER LIST************\r\n");
    PS_PRINTF_ERR("SIP SERVER NUM: %d\r\n", infoAddr->ipv6NwPara.sipSer.sipSerNum);
    if (infoAddr->ipv6NwPara.sipSer.sipSerNum >= 1) {
        PS_PRINTF_ERR(
            "Prime SIP SERVER: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            infoAddr->ipv6NwPara.sipSer.priSipServer[0],
            infoAddr->ipv6NwPara.sipSer.priSipServer[1],
            infoAddr->ipv6NwPara.sipSer.priSipServer[2],
            infoAddr->ipv6NwPara.sipSer.priSipServer[3],
            infoAddr->ipv6NwPara.sipSer.priSipServer[4],
            infoAddr->ipv6NwPara.sipSer.priSipServer[5],
            infoAddr->ipv6NwPara.sipSer.priSipServer[6],
            infoAddr->ipv6NwPara.sipSer.priSipServer[7],
            infoAddr->ipv6NwPara.sipSer.priSipServer[8],
            infoAddr->ipv6NwPara.sipSer.priSipServer[9],
            infoAddr->ipv6NwPara.sipSer.priSipServer[10],
            infoAddr->ipv6NwPara.sipSer.priSipServer[11],
            infoAddr->ipv6NwPara.sipSer.priSipServer[12],
            infoAddr->ipv6NwPara.sipSer.priSipServer[13],
            infoAddr->ipv6NwPara.sipSer.priSipServer[14],
            infoAddr->ipv6NwPara.sipSer.priSipServer[15]);
    }
    if (infoAddr->ipv6NwPara.sipSer.sipSerNum >= 2) {
        PS_PRINTF_ERR(
            "Second SIP SERVER: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            infoAddr->ipv6NwPara.sipSer.secSipServer[0],
            infoAddr->ipv6NwPara.sipSer.secSipServer[1],
            infoAddr->ipv6NwPara.sipSer.secSipServer[2],
            infoAddr->ipv6NwPara.sipSer.secSipServer[3],
            infoAddr->ipv6NwPara.sipSer.secSipServer[4],
            infoAddr->ipv6NwPara.sipSer.secSipServer[5],
            infoAddr->ipv6NwPara.sipSer.secSipServer[6],
            infoAddr->ipv6NwPara.sipSer.secSipServer[7],
            infoAddr->ipv6NwPara.sipSer.secSipServer[8],
            infoAddr->ipv6NwPara.sipSer.secSipServer[9],
            infoAddr->ipv6NwPara.sipSer.secSipServer[10],
            infoAddr->ipv6NwPara.sipSer.secSipServer[11],
            infoAddr->ipv6NwPara.sipSer.secSipServer[12],
            infoAddr->ipv6NwPara.sipSer.secSipServer[13],
            infoAddr->ipv6NwPara.sipSer.secSipServer[14],
            infoAddr->ipv6NwPara.sipSer.secSipServer[15]);
    }

    PS_PRINTF_ERR("************TE ADDR INFO************\r\n");
    PS_PRINTF_ERR("TE ADDR STATE: %d\r\n", infoAddr->teAddrInfo.teAddrState);

    PS_PRINTF_ERR("************TIMER STATE************\r\n");
    PS_PRINTF_ERR("timer type %d\r\n", infoAddr->timerInfo.timerName);
    PS_PRINTF_ERR("timer expire times %d\r\n", infoAddr->timerInfo.loopTimes);
    PS_PRINTF_ERR("Periodic RaTime Cnt %d\r\n", g_aulPeriodicRaTimeCnt[index]);

    return;
}

/*
 * Function Name: IP_NDSERVER_ShowStatInfo
 * Description: 显示报文统计信息
 * Input: ulIndex --- 实体索引
 * History:
 *    1.      2011-04-11  Draft Enact
 */
VOS_VOID IP_NDSERVER_ShowStatInfo(VOS_UINT32 index)
{
    IP_NdserverPacketStatisticsInfo *pktStatInfo = IP_NULL_PTR;
    VOS_UINT32                       tmp;

    if (index >= IP_NDSERVER_ADDRINFO_MAX_NUM) {
        tmp = IP_NDSERVER_ADDRINFO_MAX_NUM;
        PS_PRINTF_ERR("IP_NDSERVER_ShowStatInfo:input para range:0-%d\r\n", tmp - 1);
        return;
    }

    pktStatInfo = IP_NDSERVER_GET_STATINFO_ADDR(index);

    PS_PRINTF_ERR("****************************packet stat info(%d)****************************\r\n", index);
    PS_PRINTF_ERR("Receive Packet Total Num:            %d\r\n", pktStatInfo->rcvPktTotalNum);
    PS_PRINTF_ERR("Discard Packet Num:                  %d\r\n", pktStatInfo->discPktNum);
    PS_PRINTF_ERR("RcvNsPktNum:                         %d\r\n", pktStatInfo->rcvNsPktNum);
    PS_PRINTF_ERR("ulRcvNaPktNum :                      %d\r\n", pktStatInfo->rcvNaPktNum);
    PS_PRINTF_ERR("RcvRsPktNum:                         %d\r\n", pktStatInfo->rcvRsPktNum);
    PS_PRINTF_ERR("RcvEchoPktNum:                       %d\r\n", pktStatInfo->rcvEchoPktNum);
    PS_PRINTF_ERR("RcvTooBigPktNum:                     %d\r\n", pktStatInfo->rcvTooBigPktNum);
    PS_PRINTF_ERR("RcvDhcpv6PktNum:                     %d\r\n", pktStatInfo->rcvDhcpv6PktNum);
    PS_PRINTF_ERR("ErrNsPktNum:                         %d\r\n", pktStatInfo->errNsPktNum);
    PS_PRINTF_ERR("ErrNaPktNum:                         %d\r\n", pktStatInfo->errNaPktNum);
    PS_PRINTF_ERR("ErrRsPktNum :                        %d\r\n", pktStatInfo->errRsPktNum);
    PS_PRINTF_ERR("ErrEchoPktNum:                       %d\r\n", pktStatInfo->errEchoPktNum);
    PS_PRINTF_ERR("ErrTooBigPktNum:                     %d\r\n", pktStatInfo->errTooBigPktNum);
    PS_PRINTF_ERR("ErrDhcpv6PktNum:                     %d\r\n", pktStatInfo->errDhcpv6PktNum);
    PS_PRINTF_ERR("TransPktTotalNum:                    %d\r\n", pktStatInfo->transPktTotalNum);
    PS_PRINTF_ERR("TransPktFailNum:                     %d\r\n", pktStatInfo->transPktFailNum);
    PS_PRINTF_ERR("TransNsPktNum:                       %d\r\n", pktStatInfo->transNsPktNum);
    PS_PRINTF_ERR("TransNaPktNum :                      %d\r\n", pktStatInfo->transNaPktNum);
    PS_PRINTF_ERR("TransRaPktNum :                      %d\r\n", pktStatInfo->transRaPktNum);
    PS_PRINTF_ERR("TransEchoPktNum :                    %d\r\n", pktStatInfo->transEchoPktNum);
    PS_PRINTF_ERR("TransTooBigPktNum :                  %d\r\n", pktStatInfo->transTooBigPktNum);
    PS_PRINTF_ERR("TransDhcpv6PktNum :                  %d\r\n", pktStatInfo->transDhcpv6PktNum);
    PS_PRINTF_ERR("MacInvalidPktNum :                   %d\r\n", pktStatInfo->macInvalidPktNum);
    PS_PRINTF_ERR("EnquePktNum :                        %d\r\n", pktStatInfo->enquePktNum);
    PS_PRINTF_ERR("EnquePktNum :                        %d\r\n", pktStatInfo->sendQuePktNum);

    return;
}

