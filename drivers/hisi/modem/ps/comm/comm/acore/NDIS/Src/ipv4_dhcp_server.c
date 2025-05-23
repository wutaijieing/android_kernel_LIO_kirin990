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
#include "vos.h"

#include "ps_ndis.h"
#include "ipv4_dhcp_server.h"
#include "ip_comm.h"
#include "mdrv.h"
#include "ps_tag.h"
#include "securec.h"

#define THIS_MODU ps_ndis
/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_IPV4DHCPSERVER_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */
extern VOS_UINT8        g_aucBroadCastAddr[];
extern NDIS_Entity g_astNdisEntity[];

/* 获得IPV4实体 */
#define IPV4_DHCP_GET_NDIS_ENTITY(Index) (&g_astNdisEntity[(Index)])


/* 定义组DHCP OFFER/ACK/NACK时的临时缓冲区 */
VOS_UINT8 g_aucDhcpBuff[ETH_MAX_FRAME_SIZE] = {0};
/* define global DHCP Server stat info struct */
IPV4_DhcpStatInfo g_stDhcpStatStru = {0};
/* define IP Layer Identity */
VOS_UINT16 g_usIpHdrIdentity = 0;
/* define DHCP Adddr Lease Time */
VOS_UINT32 g_ulLeaseTime = (60 * 60 * 24 * 3);

VOS_UINT8 g_aucMagicCookie[] = { 0x63, 0x82, 0x53, 0x63 };

/* define MACRO to access global var */
#define IP_GET_IP_IDENTITY() (g_usIpHdrIdentity++)
#define IPV4_DHCP_GET_BUFFER() (g_aucDhcpBuff)

/*
 * 3 Function
 */
extern VOS_UINT32 Ndis_SendMacFrm(const VOS_UINT8 *buf, VOS_UINT32 len, VOS_UINT8 rabId);
/*
 * Function Name: IP_CalcCRC16
 * Description: 计算CRC 16校验和
 * Input: VOS_UINT8 *pucData : crc src buf
 *                   VOS_UINT16 usLen: src buf len
 * Return: the crc16 result of Src buf
 *                   (PS: the result is network byte order)
 *    1.      2011-3-7  Draft Enact
 */
VOS_UINT16 IP_CalcCRC16(VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT32  checkSum      = 0;
    VOS_UINT16 *dataBuffer    = VOS_NULL_PTR;

    IP_NDIS_ASSERT_RTN(VOS_NULL_PTR != data, 0);

    if (0 == len) {
        return 0;
    }

    dataBuffer = (VOS_UINT16 *)((VOS_VOID *)data);

    while (len > 1) {
        checkSum += *(dataBuffer++);
        len -= sizeof(VOS_UINT16);
    }

    if (0 != len) {
        checkSum += *(VOS_UINT8 *)dataBuffer;
    }

    checkSum = (checkSum >> 16) + (checkSum & 0xffff);
    checkSum += (checkSum >> 16);

    return (VOS_UINT16)(~checkSum);
}

/*
 * Function Name: IPV4_DHCP_SendDhcpToEth
 * Description: 将DHCP包封装成Ethnet帧发送到Ethnet上
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_SendDhcpToEth(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                                 const NDIS_Ipv4Info *ipV4Entity, VOS_UINT16 dhcpLen, VOS_UINT8 rabId)
{
    ETH_Ipfixhdr           *iPFixHdr     = VOS_NULL_PTR;
    IPV4_Udpipfakehdr      *udpIpFakeHdr = VOS_NULL_PTR;
    VOS_UINT16              checkSum;
    VOS_UINT16              curTotalLen;
    VOS_UINT16              udpCrcLen;
    VOS_UINT32              dstIp = 0xffffffff;
    errno_t                 rlt;
    if ((VOS_NULL_PTR == dhcpAnalyse) || (VOS_NULL_PTR == ipV4Entity)) {
        IP_ERROR_LOG("IPV4_DHCP_SendDhcpToEth : Input Para Error");
        return;
    }

    if (IPV4_DHCP_OPTION_OFFSET >= dhcpLen) {
        IP_ERROR_LOG1("IPV4_DHCP_SendDhcpToEth : Input Len too Small ", dhcpLen);
        return;
    }

    /* unicast */
    if (IP_CAST_TYPE_UNICAST == dhcpAnalyse->castFlg) {
        dstIp = dhcpAnalyse->srcIPAddr.ipAddr32bit;
    }

    /* UDPIP Fake Hdr Offset, Same to IP Hdr Offset */
    udpIpFakeHdr = (IPV4_Udpipfakehdr *)(IPV4_DHCP_GET_BUFFER() + ETH_MAC_HEADER_LEN);
    /* Fill Fake Hdr */
    (VOS_VOID)memset_s(udpIpFakeHdr->rec0, sizeof(udpIpFakeHdr->rec0), 0, sizeof(udpIpFakeHdr->rec0));

    udpIpFakeHdr->srcIP.ipAddr32bit = ipV4Entity->gwIpInfo.ipAddr32bit;
    udpIpFakeHdr->dstIP.ipAddr32bit = dstIp;
    udpIpFakeHdr->protocol       = IP_PROTOCOL_UDP;
    udpIpFakeHdr->all0           = 0;
    udpIpFakeHdr->length         = IP_HTONS(dhcpLen + IP_UDP_HDR_LEN);
    /* Fill Udp Hdr */
    udpIpFakeHdr->srcPort  = UDP_DHCP_SERVICE_PORT;
    udpIpFakeHdr->dstPort  = UDP_DHCP_CLIENT_PORT;
    udpIpFakeHdr->checksum = 0;
    udpIpFakeHdr->len      = udpIpFakeHdr->length;

    curTotalLen = dhcpLen + IP_UDP_HDR_LEN;

    /* Calc UDP CRC16 */
    udpCrcLen = curTotalLen + IPV4_FIX_HDR_LEN;
    checkSum  = IP_CalcCRC16((VOS_UINT8 *)udpIpFakeHdr, udpCrcLen);
    if (0 == checkSum) {
        checkSum = 0xFFFF;
    }
    udpIpFakeHdr->checksum = checkSum;

    /* IP */
    iPFixHdr = (ETH_Ipfixhdr *)(IPV4_DHCP_GET_BUFFER() + ETH_MAC_HEADER_LEN);
    /* Fill IP Fix Header */
    iPFixHdr->ipHdrLen       = IPV4_FIX_HDR_LEN >> 2;
    iPFixHdr->ipVer          = IP_IPV4_VERSION;
    iPFixHdr->serviceType    = 0;
    iPFixHdr->totalLen       = IP_HTONS(curTotalLen + IPV4_FIX_HDR_LEN);
    iPFixHdr->identification = IP_GET_IP_IDENTITY();
    iPFixHdr->identification = IP_HTONS(iPFixHdr->identification);
    iPFixHdr->offset         = 0;
    iPFixHdr->ttl            = IPV4_HDR_TTL;
    iPFixHdr->protocol       = IP_PROTOCOL_UDP;
    iPFixHdr->checkSum       = 0;
    iPFixHdr->srcAddr        = ipV4Entity->gwIpInfo.ipAddr32bit;
    iPFixHdr->destAddr       = dstIp;

    /* IP HDR CRC16 */
    iPFixHdr->checkSum = IP_CalcCRC16((VOS_UINT8 *)iPFixHdr, IPV4_FIX_HDR_LEN);

    curTotalLen = curTotalLen + IPV4_FIX_HDR_LEN;

    /* MAC */
    rlt = memcpy_s(IPV4_DHCP_GET_BUFFER(), ETH_MAC_HEADER_LEN, ipV4Entity->macFrmHdr, ETH_MAC_HEADER_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    /* Update Dst Mac Addr */
    if (IP_CAST_TYPE_UNICAST == dhcpAnalyse->castFlg) {
        rlt = memcpy_s(IPV4_DHCP_GET_BUFFER(), ETH_MAC_ADDR_LEN, ipV4Entity->ueMacAddr, ETH_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        rlt = memcpy_s(IPV4_DHCP_GET_BUFFER(), ETH_MAC_ADDR_LEN, g_aucBroadCastAddr, ETH_MAC_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    }

    curTotalLen += ETH_MAC_HEADER_LEN;

    if (PS_SUCC != Ndis_SendMacFrm(IPV4_DHCP_GET_BUFFER(), curTotalLen, rabId)) {
        IP_ERROR_LOG("IPV4_DHCP_SendDhcpToEth:Send Dhcp Packet Error");
        IPV4_DHCP_STAT_SEND_PKT_FAIL(1);
    } else {
        IPV4_DHCP_STAT_SEND_PKT_SUCC(1);
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_FormDhcpHdrStru
 * Description: form dhcp packet fix header
 * Input: VOS_UINT8 *pucDhcpData : 调用者保证长度大于DHCP固定头长度
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_FormDhcpHdrStru(const IPV4_DhcpAnalyseResult *dhcpAnalyse, VOS_UINT8 *dhcpData)
{
    IPV4_DhcpProtocl *dhcpHdr = VOS_NULL_PTR;
    errno_t           rlt;
    if ((VOS_NULL_PTR == dhcpData) || (VOS_NULL_PTR == dhcpAnalyse)) {
        IP_ERROR_LOG("IPV4_DHCP_FormDhcpHdrStru : Input Para NULL");
        return;
    }

    dhcpHdr = (IPV4_DhcpProtocl *)((VOS_VOID *)dhcpData);

    dhcpHdr->op                        = IPV4_DHCP_OPERATION_REPLY;
    dhcpHdr->hardwareType              = IPV4_DHCP_ETHERNET_HTYPE;
    dhcpHdr->hardwareLength            = ETH_MAC_ADDR_LEN;
    dhcpHdr->hops                      = 0;
    dhcpHdr->transactionID             = dhcpAnalyse->transactionID;
    dhcpHdr->seconds                   = 0;
    dhcpHdr->flags                     = 0;
    dhcpHdr->clientIPAddr.ipAddr32bit     = 0;
    dhcpHdr->relayIPAddr.ipAddr32bit      = 0;
    dhcpHdr->nextServerIPAddr.ipAddr32bit = 0;
    rlt = memcpy_s(dhcpHdr->clientHardwardAddr, ETH_MAC_ADDR_LEN, dhcpAnalyse->hardwareAddr,
                     ETH_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* Magic Cookie */
    rlt = memcpy_s(dhcpHdr->magicCookie, IPV4_DHCP_HEAD_COOKIE_LEN, g_aucMagicCookie, sizeof(g_aucMagicCookie));
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    return;
}

/*
 * Function Name: IPV4_DHCP_FormOption
 * Description: 生成Option选项
 * Output: pusOptionLen : formed option len
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_FormOption(const NDIS_Ipv4Info *ipV4Entity, VOS_UINT8 *option, VOS_UINT8 dhcpType,
                              VOS_UINT16 *optionLen)

{
    IPV4_DhcpOptionItem        *optionItem = VOS_NULL_PTR;
    VOS_UINT8                   dnsLen;
    VOS_UINT32                  timeLen;
    VOS_UINT8                  *optionAddr = VOS_NULL_PTR;
    VOS_UINT8                   winsLen;
    errno_t                     rlt;
    if ((VOS_NULL_PTR == ipV4Entity) || (VOS_NULL_PTR == option) || (VOS_NULL_PTR == optionLen)) {
        IP_ERROR_LOG("IPV4_DHCP_FormOption : Input Para Is NULL");
        return;
    }

    optionAddr = option;
    /* 报文类型 */
    optionItem                    = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType      = IPV4_DHCP_OPTION_MSG_TYPE;
    optionItem->optionLen       = 1;
    optionItem->optionValue[0] = dhcpType;
    *optionLen += 3;
    optionAddr += 3;

    /* 子网掩码 */
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_SUBNET_MASK;
    optionItem->optionLen  = IPV4_ADDR_LEN;
    rlt = memcpy_s(optionItem->optionValue, IPV4_ADDR_LEN, &(ipV4Entity->nmIpInfo), IPV4_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    *optionLen += IPV4_ADDR_LEN + 2;
    optionAddr += IPV4_ADDR_LEN + 2;

    /* 默认网关 */
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_ROUTER_IP;
    optionItem->optionLen  = IPV4_ADDR_LEN;
    rlt = memcpy_s(optionItem->optionValue, IPV4_ADDR_LEN, &(ipV4Entity->gwIpInfo), IPV4_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    *optionLen += IPV4_ADDR_LEN + 2;
    optionAddr += IPV4_ADDR_LEN + 2;

    /* DNS */
    dnsLen                    = 0;
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_DOMAIN_NAME_SERVER;
    if (0 != ipV4Entity->primDnsAddr.ipAddr32bit) {
        rlt = memcpy_s(optionItem->optionValue, IPV4_ADDR_LEN, &(ipV4Entity->primDnsAddr), IPV4_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
        dnsLen += IPV4_ADDR_LEN;
    }

    if (0 != ipV4Entity->secDnsAddr.ipAddr32bit) {
        rlt = memcpy_s(optionItem->optionValue + dnsLen, IPV4_ADDR_LEN, &(ipV4Entity->secDnsAddr),
                         IPV4_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        dnsLen += IPV4_ADDR_LEN;
    }

    optionItem->optionLen = dnsLen;
    if (0 != dnsLen) {
        *optionLen += dnsLen + 2;
        optionAddr += dnsLen + 2;
    }

    /* DHCP SERVER IP */
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_SERVER_IP;
    optionItem->optionLen  = IPV4_ADDR_LEN;
    rlt = memcpy_s(optionItem->optionValue, IPV4_ADDR_LEN, &(ipV4Entity->gwIpInfo), IPV4_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    *optionLen += IPV4_ADDR_LEN + 2;
    optionAddr += IPV4_ADDR_LEN + 2;

    /* 租期 */
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_LEASE_TIME;
    optionItem->optionLen  = IPV4_DHCP_OPTION_LEASE_TIME_LEN;
    /* 转换为网络字节序 */
    timeLen = IPV4_DHCP_LEASE_TIME;
    timeLen = IP_HTONL(timeLen);
    rlt = memcpy_s(optionItem->optionValue, IPV4_DHCP_OPTION_LEASE_TIME_LEN, &timeLen,
                     IPV4_DHCP_OPTION_LEASE_TIME_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    *optionLen += IPV4_DHCP_OPTION_LEASE_TIME_LEN + 2;
    optionAddr += IPV4_DHCP_OPTION_LEASE_TIME_LEN + 2;

    /* Renewal (T1) Time Value */
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_T1;
    optionItem->optionLen  = IPV4_DHCP_OPTION_LEASE_TIME_LEN;
    timeLen                   = (VOS_UINT32)IPV4_DHCP_T1;
    timeLen                   = IP_HTONL(timeLen);
    rlt = memcpy_s(optionItem->optionValue, IPV4_DHCP_OPTION_LEASE_TIME_LEN, &timeLen,
                     IPV4_DHCP_OPTION_LEASE_TIME_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    *optionLen += IPV4_DHCP_OPTION_LEASE_TIME_LEN + 2;
    optionAddr += IPV4_DHCP_OPTION_LEASE_TIME_LEN + 2;

    /* Renewal (T2) Time Value */
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_T2;
    optionItem->optionLen  = IPV4_DHCP_OPTION_LEASE_TIME_LEN;
    timeLen                   = (VOS_UINT32)IPV4_DHCP_T2;
    timeLen                   = IP_HTONL(timeLen);
    rlt = memcpy_s(optionItem->optionValue, IPV4_DHCP_OPTION_LEASE_TIME_LEN, &timeLen,
                     IPV4_DHCP_OPTION_LEASE_TIME_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    *optionLen += IPV4_DHCP_OPTION_LEASE_TIME_LEN + 2;
    optionAddr += IPV4_DHCP_OPTION_LEASE_TIME_LEN + 2;

    /* WINS : NetBios Name Server */
    winsLen                   = 0;
    optionItem               = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionAddr);
    optionItem->optionType = IPV4_DHCP_OPTION_NETBIOS_NAME_SERVER;
    if (0 != ipV4Entity->primWinsAddr.ipAddr32bit) {
        rlt = memcpy_s(optionItem->optionValue, IPV4_ADDR_LEN, &(ipV4Entity->primWinsAddr), IPV4_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        winsLen += IPV4_ADDR_LEN;
    }

    if (0 != ipV4Entity->secWinsAddr.ipAddr32bit) {
        rlt = memcpy_s(optionItem->optionValue + winsLen, IPV4_ADDR_LEN, &(ipV4Entity->secWinsAddr),
                         IPV4_ADDR_LEN);
        IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

        winsLen += IPV4_ADDR_LEN;
    }

    optionItem->optionLen = winsLen;
    if (0 != winsLen) {
        *optionLen += winsLen + 2;
        optionAddr += winsLen + 2;
    }

    /*  End Option 0xff option结束标志 */
    *optionAddr = 0xFF;
    *optionLen += 1;

    return;
}

/*
 * Function Name: IPV4_DHCP_FormOfferMsg
 * Description: 生成DHCP OFFER消息，
 * Return: 成功返回PS_SUCC;失败返回PS_FAIL
 *    1.      2011-3-7  Draft Enact
 */
VOS_UINT32 IPV4_DHCP_FormOfferMsg(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                                  const NDIS_Ipv4Info *ipV4Entity, VOS_UINT16 *len)
{
    VOS_UINT8 *dhcpPktBuf    = VOS_NULL_PTR;
    VOS_UINT32 dhcpOffset;
    VOS_UINT16 dhcpOptionLen = 0;
    VOS_INT32  rlt;
    if ((VOS_NULL_PTR == dhcpAnalyse) || (VOS_NULL_PTR == ipV4Entity) || (VOS_NULL_PTR == len)) {
        IP_ERROR_LOG("IPV4_DHCP_FormOfferMsg : Input Para NULL");
        return PS_FAIL;
    }

    /* 将BUF 清 零 */
    (VOS_VOID)memset_s(IPV4_DHCP_GET_BUFFER(), ETH_MAX_FRAME_SIZE, 0, ETH_MAX_FRAME_SIZE);

    dhcpOffset  = ETH_MAC_HEADER_LEN + IPV4_FIX_HDR_LEN + IP_UDP_HDR_LEN;
    dhcpPktBuf = IPV4_DHCP_GET_BUFFER() + dhcpOffset;

    IPV4_DHCP_FormDhcpHdrStru(dhcpAnalyse, dhcpPktBuf);

    /* 设置your ip address */
    rlt = memcpy_s(dhcpPktBuf + IPV4_DHCP_HEAD_YIADDR_OFFSET, IPV4_ADDR_LEN, ipV4Entity->ueIpInfo.ipAddr8bit,
                     IPV4_ADDR_LEN);
    IP_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    /* dhcp option 设置 */
    dhcpPktBuf += IPV4_DHCP_OPTION_OFFSET;
    IPV4_DHCP_FormOption(ipV4Entity, dhcpPktBuf, IPV4_DHCP_MSG_OFFER, &dhcpOptionLen);

    /* 返回写入的长度 */
    *len = dhcpOptionLen + IPV4_DHCP_OPTION_OFFSET;

    return PS_SUCC;
}

/*
 * Function Name: IPV4_DHCP_FormAckMsg
 * Description: 生成DHCP ACK消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_FormAckMsg(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                              const NDIS_Ipv4Info *ipV4Entity, VOS_UINT16 *len)
{
    VOS_UINT8 *dhcpPktBuf    = VOS_NULL_PTR;
    VOS_UINT32 dhcpOffset;
    VOS_UINT16 dhcpOptionLen = 0;
    VOS_INT32  rlt;
    IP_NDIS_ASSERT(VOS_NULL_PTR != dhcpAnalyse);
    IP_NDIS_ASSERT(VOS_NULL_PTR != ipV4Entity);
    IP_NDIS_ASSERT(VOS_NULL_PTR != len);

    /* 将BUF 清 零 */
    (VOS_VOID)memset_s(IPV4_DHCP_GET_BUFFER(), ETH_MAX_FRAME_SIZE, 0, ETH_MAX_FRAME_SIZE);

    dhcpOffset  = ETH_MAC_HEADER_LEN + IPV4_FIX_HDR_LEN + IP_UDP_HDR_LEN;
    dhcpPktBuf = IPV4_DHCP_GET_BUFFER() + dhcpOffset;

    /* 设置DHCP报文头中固定部分 */
    IPV4_DHCP_FormDhcpHdrStru(dhcpAnalyse, dhcpPktBuf);

    /* 设置your ip address */
    rlt = memcpy_s(dhcpPktBuf + IPV4_DHCP_HEAD_YIADDR_OFFSET, IPV4_ADDR_LEN, ipV4Entity->ueIpInfo.ipAddr8bit,
                     IPV4_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* Form Option */
    dhcpPktBuf += IPV4_DHCP_OPTION_OFFSET;
    IPV4_DHCP_FormOption(ipV4Entity, dhcpPktBuf, IPV4_DHCP_MSG_ACK, &dhcpOptionLen);

    /* 返回写入的长度 */
    *len = dhcpOptionLen + IPV4_DHCP_OPTION_OFFSET;

    return;
}

/*
 * Function Name: IPV4_DHCP_FormNakMsg
 * Description: 生成DHCP NACK消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_FormNackMsg(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                               const NDIS_Ipv4Info *ipV4Entity, VOS_UINT16 *len)
{
    VOS_UINT8                  *dhcpPktBuf = VOS_NULL_PTR;
    VOS_UINT32                  dhcpOffset;
    VOS_UINT16                  dhcpOptionLen;
    IPV4_DhcpOptionItem        *optionItem = VOS_NULL_PTR;
    VOS_INT32                   rlt;
    if ((VOS_NULL_PTR == dhcpAnalyse) || (VOS_NULL_PTR == ipV4Entity) || (VOS_NULL_PTR == len)) {
        IP_ERROR_LOG("IPV4_DHCP_FormNackMsg : Input Para NULL");
        return;
    }

    /* 将BUF 清 零 */
    (VOS_VOID)memset_s(IPV4_DHCP_GET_BUFFER(), ETH_MAX_FRAME_SIZE, 0, ETH_MAX_FRAME_SIZE);

    dhcpOffset  = ETH_MAC_HEADER_LEN + IPV4_FIX_HDR_LEN + IP_UDP_HDR_LEN;
    dhcpPktBuf = IPV4_DHCP_GET_BUFFER() + dhcpOffset;

    /* Fill Dhcp Fix Hdr,and Keep your ip address to 0 */
    IPV4_DHCP_FormDhcpHdrStru(dhcpAnalyse, dhcpPktBuf);

    /* Form NACK Option */
    dhcpOptionLen = 0;
    dhcpPktBuf += IPV4_DHCP_OPTION_OFFSET;

    /* DHCP Msg Type */
    optionItem                    = (IPV4_DhcpOptionItem *)(dhcpPktBuf);
    optionItem->optionType      = IPV4_DHCP_OPTION_MSG_TYPE;
    optionItem->optionLen       = 1;
    optionItem->optionValue[0] = IPV4_DHCP_MSG_NAK;
    dhcpOptionLen += 3;
    dhcpPktBuf += 3;

    /* DHCP SERVER IP */
    optionItem               = (IPV4_DhcpOptionItem *)dhcpPktBuf;
    optionItem->optionType = IPV4_DHCP_OPTION_SERVER_IP;
    optionItem->optionLen  = IPV4_ADDR_LEN;
    rlt = memcpy_s(optionItem->optionValue, IPV4_ADDR_LEN, &(ipV4Entity->gwIpInfo), IPV4_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    dhcpOptionLen += IPV4_ADDR_LEN + 2;
    dhcpPktBuf += IPV4_ADDR_LEN + 2;

    /*  End Option 0xff option结束标志 */
    *dhcpPktBuf = 0xFF;
    dhcpOptionLen += 1;

    /* 返回写入的长度 */
    *len = dhcpOptionLen + IPV4_DHCP_OPTION_OFFSET;

    return;
}

/*
 * Function Name: IPV4_DHCP_FindIPV4Entity
 * Description: 根据DHCP Client MAC Addr查找相应的IPV4实体;
 *                   由调用者保证入参ExRabId对应一个NDIS实体
 * History:
 *    1.      2011-3-7  Draft Enact
 *    2.      2011-12-9
 *    3.      2013-1-16  DSDA
 */
NDIS_Ipv4Info *IPV4_DHCP_FindIPV4Entity(VOS_UINT8 exRabId)
{
    NDIS_Entity    *ndisEntity = VOS_NULL_PTR;
    NDIS_Ipv4Info  *ipv4Info   = VOS_NULL_PTR;

    /* 由调用者保证入参ExRabId对应一个NDIS实体 */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        return VOS_NULL_PTR;
    }
    ipv4Info = &(ndisEntity->ipV4Info);

    return ipv4Info;
}

/*
 * Function Name: IPV4_DHCP_DiscoverMsgProc
 * Description: 处理DHCP DISCOVER消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_DiscoverMsgProc(IPV4_DhcpAnalyseResult *dhcpAnalyse, VOS_UINT8 rabId)
{
    NDIS_Ipv4Info *ipV4Entity = VOS_NULL_PTR;
    VOS_UINT16     dhcpLength;

    if (VOS_NULL_PTR == dhcpAnalyse) {
        IP_ERROR_LOG("IPV4_DHCP_DiscoverMsgProc : Input Para pstDhcpAnalyse is NULL");
        return;
    }

    /* find the dhcp sever entity by ue mac */
    ipV4Entity = IPV4_DHCP_FindIPV4Entity(rabId);
    if (VOS_NULL_PTR == ipV4Entity) {
        IP_ERROR_LOG("IPV4_DHCP_DiscoverMsgProc : Not Available IPV4 Entity");
        IPV4_DHCP_STAT_PROC_ERR(1);
        return;
    }

    dhcpAnalyse->castFlg = IP_CAST_TYPE_BRODCAST;

    /* form dhcp offer msg */
    dhcpLength = 0;
    if (PS_SUCC != IPV4_DHCP_FormOfferMsg(dhcpAnalyse, ipV4Entity, &dhcpLength)) {
        IP_ERROR_LOG("IPV4_DHCP_DiscoverMsgProc : Form Offer Msg Fail");
        IPV4_DHCP_STAT_PROC_ERR(1);
        return;
    }

    /* send dhcp pkt to ethnet */
    IPV4_DHCP_SendDhcpToEth(dhcpAnalyse, ipV4Entity, dhcpLength, rabId);
    IPV4_DHCP_STAT_SEND_OFFER(1);

    return;
}
/*
 * Function Name: IPV4_DHCP_SelectingRequestMsgProc
 * Description: 处理Selecting状态下的请求消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_SelectingRequestMsgProc(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                                           NDIS_Ipv4Info *ipV4Entity, VOS_UINT8 rabId)
{
    VOS_UINT16 dhcpAckLen = 0;

    if ((VOS_NULL_PTR == dhcpAnalyse) || (VOS_NULL_PTR == ipV4Entity)) {
        IP_ERROR_LOG("IPV4_DHCP_SelectingRequestMsgProc : Input Para is NULL");
        return;
    }

    /* 判断当前消息 */
    if ((dhcpAnalyse->serverIPAddr.ipAddr32bit == ipV4Entity->gwIpInfo.ipAddr32bit) &&
        (dhcpAnalyse->requestIPAddr.ipAddr32bit == ipV4Entity->ueIpInfo.ipAddr32bit)) {
        /* 生成DHCP ACK并发送 */
        IPV4_DHCP_FormAckMsg(dhcpAnalyse, ipV4Entity, &dhcpAckLen);
        IPV4_DHCP_SendDhcpToEth(dhcpAnalyse, ipV4Entity, dhcpAckLen, rabId);

        IPV4_DHCP_STAT_SEND_ACK(1);

        /* 设置IP地址为已分配 */
        ipV4Entity->ipAssignStatus = IPV4_DHCP_ADDR_STATUS_ASSIGNED;
    } else {
        /* 未选择本 DHCP Server,设置IP地址未分配 */
        ipV4Entity->ipAssignStatus = IPV4_DHCP_ADDR_STATUS_FREE;
        IP_ERROR_LOG("IPV4_DHCP_SelectingRequestMsgProc : The Client Has not Select Local IP");
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_OtherRequestMsgProc
 * Description: 处理非Selecting状态下的请求消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_OtherRequestMsgProc(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                                       const NDIS_Ipv4Info *ipV4Entity, VOS_UINT8 rabId)
{
    VOS_UINT16 dhcpAckLen = 0;

    if ((VOS_NULL_PTR == dhcpAnalyse) || (VOS_NULL_PTR == ipV4Entity)) {
        IP_ERROR_LOG("IPV4_DHCP_OtherRequestMsgProc : Input Para is NULL");
        return;
    }

    /* 若请求IP同UE IP相同，则回ACK;否则回复NACK */
    if (dhcpAnalyse->clientIPAddr.ipAddr32bit == ipV4Entity->ueIpInfo.ipAddr32bit) {
        IPV4_DHCP_FormAckMsg(dhcpAnalyse, ipV4Entity, &dhcpAckLen);
        IPV4_DHCP_SendDhcpToEth(dhcpAnalyse, ipV4Entity, dhcpAckLen, rabId);
        IPV4_DHCP_STAT_SEND_ACK(1);
    } else {
        IPV4_DHCP_FormNackMsg(dhcpAnalyse, ipV4Entity, &dhcpAckLen);
        IPV4_DHCP_SendDhcpToEth(dhcpAnalyse, ipV4Entity, dhcpAckLen, rabId);
        /* NACK ERROR LOG */
        IP_ERROR_LOG("IPV4_DHCP_OtherRequestMsgProc : The Client Renew/Rebinding Request Fail");
        IPV4_DHCP_STAT_SEND_NACK(1);
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_InitRebootRequestProc
 * Description: 处理Client 在Init-Reboot状态下的请求消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_InitRebootRequestProc(const IPV4_DhcpAnalyseResult *dhcpAnalyse,
                                         NDIS_Ipv4Info *ipV4Entity, VOS_UINT8 rabId)
{
    VOS_UINT16 dhcpAckLen = 0;

    if ((VOS_NULL_PTR == dhcpAnalyse) || (VOS_NULL_PTR == ipV4Entity)) {
        IP_ERROR_LOG("IPV4_DHCP_InitRebootRequestProc : Input Para is NULL");
        return;
    }

    /* 若请求IP同UE IP相同，则回ACK;否则回复NACK */
    if (dhcpAnalyse->requestIPAddr.ipAddr32bit == ipV4Entity->ueIpInfo.ipAddr32bit) {
        IPV4_DHCP_FormAckMsg(dhcpAnalyse, ipV4Entity, &dhcpAckLen);
        IPV4_DHCP_SendDhcpToEth(dhcpAnalyse, ipV4Entity, dhcpAckLen, rabId);
        ipV4Entity->ipAssignStatus = IPV4_DHCP_ADDR_STATUS_ASSIGNED;
        IPV4_DHCP_STAT_SEND_ACK(1);
    } else {
        IPV4_DHCP_FormNackMsg(dhcpAnalyse, ipV4Entity, &dhcpAckLen);
        IPV4_DHCP_SendDhcpToEth(dhcpAnalyse, ipV4Entity, dhcpAckLen, rabId);
        /* NACK ERROR LOG */
        IP_ERROR_LOG("IPV4_DHCP_InitRebootRequestProc : The Init-Reboot Client Request IP Fail");
        IPV4_DHCP_STAT_SEND_NACK(1);
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_RequestMsgProc
 * Description: 处理DHCP REQUEST消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_RequestMsgProc(IPV4_DhcpAnalyseResult *dhcpAnalyse, VOS_UINT8 rabId)
{
    NDIS_Ipv4Info *ipV4Entity = VOS_NULL_PTR;

    if (VOS_NULL_PTR == dhcpAnalyse) {
        IP_ERROR_LOG("IPV4_DHCP_RequestMsgProc : Input Para NULL");
        return;
    }

    ipV4Entity = IPV4_DHCP_FindIPV4Entity(rabId);
    if (VOS_NULL_PTR == ipV4Entity) {
        IP_ERROR_LOG("IPV4_DHCP_RequestMsgProc : Not Available IPV4 Entity");
        IPV4_DHCP_STAT_PROC_ERR(1);
        return;
    }

    /*
     * Client messages from different states
     * |              |INIT-REBOOT  |SELECTING    |RENEWING     |REBINDING |
     * |broad/unicast |broadcast    |broadcast    |unicast      |broadcast |
     * |server-ip     |MUST NOT     |MUST         |MUST NOT     |MUST NOT  |
     * |requested-ip  |MUST         |MUST         |MUST NOT     |MUST NOT  |
     * |ciaddr        |zero         |zero         |IP address   |IP address|
     */
    if (0 != dhcpAnalyse->serverIPAddr.ipAddr32bit) {
        /* Selecting Request */
        dhcpAnalyse->castFlg = IP_CAST_TYPE_BRODCAST;
        IPV4_DHCP_SelectingRequestMsgProc(dhcpAnalyse, ipV4Entity, rabId);
    } else if (0 == dhcpAnalyse->clientIPAddr.ipAddr32bit) {
        /* INIT-REBOOT Request */
        dhcpAnalyse->castFlg = IP_CAST_TYPE_BRODCAST;
        IPV4_DHCP_InitRebootRequestProc(dhcpAnalyse, ipV4Entity, rabId);
    } else {
        /* renew or rebinding */
        dhcpAnalyse->castFlg = IP_CAST_TYPE_UNICAST;
        IPV4_DHCP_OtherRequestMsgProc(dhcpAnalyse, ipV4Entity, rabId);
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_ReleaseMsgProc
 * Description: 处理DHCP RELEASE消息
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_ReleaseMsgProc(const IPV4_DhcpAnalyseResult *dhcpAnalyse, VOS_UINT8 rabId)
{
    NDIS_Ipv4Info *ipV4Entity = VOS_NULL_PTR;

    if (VOS_NULL_PTR == dhcpAnalyse) {
        IP_ERROR_LOG("IPV4_DHCP_ReleaseMsgProc : Input Para pstDhcpAnalyse is NULL");
        return;
    }

    ipV4Entity = IPV4_DHCP_FindIPV4Entity(rabId);
    if (VOS_NULL_PTR == ipV4Entity) {
        IP_ERROR_LOG("IPV4_DHCP_DiscoverMsgProc : Not Available IPV4 Entity");
        IPV4_DHCP_STAT_PROC_ERR(1);
        return;
    }

    /* 修改分配状态 : 未分配 */
    ipV4Entity->ipAssignStatus = IPV4_DHCP_ADDR_STATUS_FREE;

    return;
}

/*
 * Function Name: IPV4_DHCP_ProcDhcpMsg
 * Description: 处理DHCP数据报
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_ProcDhcpMsg(IPV4_DhcpAnalyseResult *dhcpAnalyse, VOS_UINT8 rabId)
{
    NDIS_Ipv4Info *ipV4Entity = VOS_NULL_PTR;
    VOS_INT32      rlt;
    if (VOS_NULL_PTR == dhcpAnalyse) {
        IP_ERROR_LOG("IPV4_DHCP_ProcDhcpMsg : Input Para pstDhcpAnalyse is NULL");
        return;
    }

    /* find the dhcp sever entity by ue mac */
    ipV4Entity = IPV4_DHCP_FindIPV4Entity(rabId);
    if (VOS_NULL_PTR == ipV4Entity) {
        IP_ERROR_LOG("IPV4_DHCP_ProcDhcpMsg : Not Available IPV4 Entity");
        IPV4_DHCP_STAT_PROC_ERR(1);
        return;
    }

    /* save the ue mac addr,and update the mac hdr */
    rlt = memcpy_s(ipV4Entity->ueMacAddr, ETH_MAC_ADDR_LEN, dhcpAnalyse->hardwareAddr, ETH_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    rlt = memcpy_s(ipV4Entity->macFrmHdr, ETH_MAC_ADDR_LEN, dhcpAnalyse->hardwareAddr, ETH_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    ipV4Entity->arpInitFlg = PS_TRUE;

    switch (dhcpAnalyse->msgType) {
        case IPV4_DHCP_MSG_DISCOVER:
            IPV4_DHCP_DiscoverMsgProc(dhcpAnalyse, rabId);
            IPV4_DHCP_STAT_RECV_DISCOVER_MSG(1);
            break;

        case IPV4_DHCP_MSG_REQUEST:
            IPV4_DHCP_RequestMsgProc(dhcpAnalyse, rabId);
            IPV4_DHCP_STAT_RECV_REQUEST_MSG(1);
            break;

        case IPV4_DHCP_MSG_RELEASE:
            IPV4_DHCP_ReleaseMsgProc(dhcpAnalyse, rabId);
            IPV4_DHCP_STAT_RECV_RELEASE_MSG(1);
            break;

        case IPV4_DHCP_MSG_DECLINE:
            /* 调用DECLINE处理函数 */
            IP_ERROR_LOG("IP_DHCPV4SERVER_ProcRabmDataInd:DECLINE Msg is received!");
            IPV4_DHCP_STAT_RECV_OTHER_TYPE_MSG(1);
            break;

        case IPV4_DHCP_MSG_INFORM:
            /* 调用INFORM处理函数 */
            IP_ERROR_LOG("IP_DHCPV4SERVER_ProcRabmDataInd:INFORM Msg is received!");
            IPV4_DHCP_STAT_RECV_OTHER_TYPE_MSG(1);
            break;

        default:
            IP_ERROR_LOG("IP_DHCPV4SERVER_ProcRabmDataInd:Illegal DHCPV4 Msg!");
            IPV4_DHCP_STAT_RECV_OTHER_TYPE_MSG(1);
            break;
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_AnalyseDhcpPkt
 * Description: 解析DHCP报文
 * Output: IPV4_DhcpAnalyseResult *pstAnalyseRst : DHCP报文解析结果
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_AnalyseDhcpPkt(VOS_UINT8 *dhcp, VOS_UINT32 dhcpLen,
                                  IPV4_DhcpAnalyseResult *analyseRst)
{
    IPV4_DhcpProtocl     *dhcpFixHdr  = VOS_NULL_PTR;
    VOS_UINT8            *optionStart = VOS_NULL_PTR;
    VOS_UINT8            *optionEnd   = VOS_NULL_PTR;
    IPV4_DhcpOptionItem  *optionStru  = VOS_NULL_PTR;
    VOS_INT32             rlt;
    if ((VOS_NULL_PTR == dhcp) || (VOS_NULL_PTR == analyseRst)) {
        IP_ERROR_LOG("IPV4_DHCP_AnalyseDhcpPkt : Input Para is NULL");
        return;
    }

    if (IPV4_DHCP_OPTION_OFFSET > dhcpLen) {
        IP_ERROR_LOG1("IPV4_DHCP_AnalyseDhcpPkt : Dhcp Packet is small ", dhcpLen);
        return;
    }

    /* 记录DHCP固定头中的信息 */
    dhcpFixHdr                          = (IPV4_DhcpProtocl *)((VOS_VOID *)dhcp);
    analyseRst->transactionID         = dhcpFixHdr->transactionID;
    analyseRst->clientIPAddr.ipAddr32bit = dhcpFixHdr->clientIPAddr.ipAddr32bit;
    analyseRst->yourIPAddr.ipAddr32bit   = dhcpFixHdr->yourIPAddr.ipAddr32bit;
    analyseRst->hardwareLen           = ETH_MAC_ADDR_LEN;
    rlt = memcpy_s(analyseRst->hardwareAddr, ETH_MAC_ADDR_LEN, dhcpFixHdr->clientHardwardAddr,
                     ETH_MAC_ADDR_LEN);
    IP_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    /* Option Start and End Address,not include Magic Code */
    optionStart = dhcp + IPV4_DHCP_OPTION_OFFSET;
    /* Remove End */
    optionEnd = optionStart + dhcpLen - IPV4_DHCP_OPTION_OFFSET;

    while (optionStart < optionEnd) {
        optionStru = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionStart);

        if (IPV4_DHCP_OPTION_MSG_TYPE == optionStru->optionType) {
            rlt = memcpy_s(&(analyseRst->msgType), IPV4_DHCP_OPTION_MSG_TYPE_LEN, optionStru->optionValue,
                             IPV4_DHCP_OPTION_MSG_TYPE_LEN);
            if (rlt != EOK) {
                optionStart += optionStru->optionLen + IP_DHCPV4_OPTION_ITEM_HDR_LEN;
                continue;
            }
        } else if (IPV4_DHCP_OPTION_REQUEST_IP_ADDR == optionStru->optionType) {
            rlt = memcpy_s(&(analyseRst->requestIPAddr), IPV4_ADDR_LEN, optionStru->optionValue,
                             IPV4_ADDR_LEN);
            if (rlt != EOK) {
                optionStart += optionStru->optionLen + IP_DHCPV4_OPTION_ITEM_HDR_LEN;
                continue;
            }
        } else if (IPV4_DHCP_OPTION_SERVER_IP == optionStru->optionType) {
            rlt = memcpy_s(&(analyseRst->serverIPAddr), IPV4_ADDR_LEN, optionStru->optionValue,
                             IPV4_ADDR_LEN);
            if (rlt != EOK) {
                optionStart += optionStru->optionLen + IP_DHCPV4_OPTION_ITEM_HDR_LEN;
                continue;
            }
        } else if (IPV4_DHCP_OPTION_PAD_OPTIOIN == optionStru->optionType) {
            /* PAD Only 1 byte */
            optionStart++;
            continue;
        } else if (IPV4_DHCP_OPTION_END_OPTION == optionStru->optionType) {
            break;
        } else {
        }

        optionStart += optionStru->optionLen + IP_DHCPV4_OPTION_ITEM_HDR_LEN;
    }

    return;
}

/*
 * Function Name: IPV4_DHCP_DhcpPktIPInfoCheck
 * Description: DHCP报文IP层信息检查
 * Input: VOS_UINT8  *pucIpPkt 指向IP头开始的位置
 *    1.      2011-3-7  Draft Enact
 */
VOS_UINT32 IPV4_DHCP_DhcpPktIPInfoCheck(VOS_UINT8 *ipPkt, VOS_UINT32 ipMemLen)
{
    ETH_Ipfixhdr *ipHdr = VOS_NULL_PTR;
    VOS_UINT32    ipHdrLen;
    VOS_UINT32    ipLen;

    /* IP头内存存在 */
    if (sizeof(ETH_Ipfixhdr) > ipMemLen) {
        return PS_FALSE;
    }

    ipHdr   = (ETH_Ipfixhdr *)((VOS_VOID *)ipPkt);
    ipHdrLen = ipHdr->ipHdrLen << 2;
    ipLen    = IP_NTOHS(ipHdr->totalLen);

    /* 非IPv4数据包 */
    if (IP_IPV4_VERSION != ipHdr->ipVer) {
        return PS_FALSE;
    }

    /* 非UDP包 */
    if (IP_PROTOCOL_UDP != ipHdr->protocol) {
        return PS_FALSE;
    }

    /* IP头中长度合法性判断 */
    if ((ipLen > ipMemLen) || (ipHdrLen >= ipMemLen) || (ipHdrLen >= ipLen)) {
        return PS_FALSE;
    }

    /* 存在UDP头 */
    if ((ipHdrLen + IP_UDP_HDR_LEN) > ipMemLen) {
        return PS_FALSE;
    }

    return PS_TRUE;
}

/*
 * Function Name: IPV4_DHCP_DhcpPktUdpInfoCheck
 * Description: DHCP报文UDP信息检查
 * Input: VOS_UINT8  *pucIpPkt 指向UDP头开始的位置
 * History: VOS_VOID
 *    1.      2011-3-7  Draft Enact
 */
VOS_UINT32 IPV4_DHCP_DhcpPktUdpInfoCheck(VOS_UINT8 *udp, VOS_UINT32 udpMemLen)
{
    ETH_Udphdr *udpHdr = VOS_NULL_PTR;
    VOS_UINT32  udpLen;

    /* 头长度判断 */
    if (IP_UDP_HDR_LEN > udpMemLen) {
        return PS_FALSE;
    }

    /* UDP长度合法性 */
    udpHdr = (ETH_Udphdr *)((VOS_VOID *)udp);
    udpLen  = IP_NTOHS(udpHdr->len);
    if (udpLen > udpMemLen) {
        return PS_FALSE;
    }

    /* 端口号67 */
    if (UDP_DHCP_SERVICE_PORT != udpHdr->dstPort) {
        return PS_FALSE;
    }

    return PS_TRUE;
}

/*
 * Function Name: IPV4_DHCP_DhcpPktDhcpInfoCheck
 * Description: DHCP报文UDP信息检查
 * Input: VOS_UINT8  *pucIpPkt 指向UDP头开始的位置
 * History:
 *    1.      2011-3-7  Draft Enact
 */
VOS_UINT32 IPV4_DHCP_DhcpPktDhcpInfoCheck(VOS_UINT8 *dhcp, VOS_UINT32 dhcpMemLen)
{
    IPV4_DhcpProtocl     *dhcpFixHdr  = VOS_NULL_PTR;
    IPV4_DhcpOptionItem  *optionStru  = VOS_NULL_PTR;
    VOS_UINT8            *optionStart = VOS_NULL_PTR;
    VOS_UINT8            *optionEnd   = VOS_NULL_PTR;

    /* DHCP固定头存在 */
    if (sizeof(IPV4_DhcpProtocl) > dhcpMemLen) {
        return PS_FALSE;
    }

    /* 地址长度非法 */
    dhcpFixHdr = (IPV4_DhcpProtocl *)((VOS_VOID *)dhcp);
    if (dhcpFixHdr->hardwareLength > ETH_MAC_ADDR_LEN) {
        return PS_FALSE;
    }

    /* OPTION判断 */
    optionStart = dhcp + sizeof(IPV4_DhcpProtocl);
    optionEnd   = optionStart + (dhcpMemLen - sizeof(IPV4_DhcpProtocl));
    while (optionStart < optionEnd) {
        optionStru = (IPV4_DhcpOptionItem *)((VOS_VOID *)optionStart);
        if (IPV4_DHCP_OPTION_PAD_OPTIOIN == optionStru->optionType) {
            /* PAD Only 1 byte */
            optionStart++;
        } else if (IPV4_DHCP_OPTION_END_OPTION == optionStru->optionType) {
            break;
        } else {
            /* 判断OptionLen是否有效 */
            if ((optionStart + IP_DHCPV4_OPTION_ITEM_HDR_LEN) > optionEnd) {
                return PS_FALSE;
            }

            optionStart += optionStru->optionLen + IP_DHCPV4_OPTION_ITEM_HDR_LEN;
        }
    }

    if (optionStart > optionEnd) {
        return PS_FALSE;
    }

    return PS_TRUE;
}

/*
 * Function Name: IPV4_DHCP_IsDhcpPacket
 * Description: 判断是否是DHCP报文
 * Input: VOS_UINT8  *pucIpPkt 指向IP头开始的位置
 * History:
 *    1.      2011-3-7  Draft Enact
 */
VOS_UINT32 IPV4_DHCP_IsDhcpPacket(VOS_UINT8 *ipPkt, VOS_UINT32 pktMemLen)
{
    ETH_Ipfixhdr *ipHdr = VOS_NULL_PTR;
    VOS_UINT32    ipHdrLen;
    VOS_UINT8    *udp = VOS_NULL_PTR;
    VOS_UINT32    udpMemLen;
    VOS_UINT8    *dhcp = VOS_NULL_PTR;
    VOS_UINT32    dhcpMemLen;

    if (VOS_NULL_PTR == ipPkt) {
        return PS_FALSE;
    }

    /* IP层判断 */
    if (PS_TRUE != IPV4_DHCP_DhcpPktIPInfoCheck(ipPkt, pktMemLen)) {
        return PS_FALSE;
    }

    /* UDP判断 */
    ipHdr    = (ETH_Ipfixhdr *)((VOS_VOID *)ipPkt);
    ipHdrLen  = ipHdr->ipHdrLen << 2;
    udp      = ipPkt + ipHdrLen;
    udpMemLen = pktMemLen - ipHdrLen;
    if (PS_TRUE != IPV4_DHCP_DhcpPktUdpInfoCheck(udp, udpMemLen)) {
        return PS_FALSE;
    }

    /* DHCP判断 */
    dhcp      = udp + IP_UDP_HDR_LEN;
    dhcpMemLen = udpMemLen - IP_UDP_HDR_LEN;
    if (PS_TRUE != IPV4_DHCP_DhcpPktDhcpInfoCheck(dhcp, dhcpMemLen)) {
        return PS_FALSE;
    }

    return PS_TRUE;
}

/*
 * Function Name: IPV4_DHCP_ProcDhcpPkt
 * Description: 处理DHCP报文
 * Input: VOS_UINT8  *pucIpPkt 指向IP头开始的位置
 * History:
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_ProcDhcpPkt(VOS_UINT8 *ipPkt, VOS_UINT8 rabId, VOS_UINT32 pktMemLen)
{
    IPV4_DhcpAnalyseResult dhcpRst;
    ETH_Ipfixhdr          *ipFixHdr = VOS_NULL_PTR;
    VOS_UINT32             dhcpOffset;
    VOS_UINT32             dhcpLen;
    if (VOS_NULL_PTR == ipPkt) {
        IP_ERROR_LOG("IPV4_DHCP_ProcDhcpPkt : Input Para Is NULL");
        return;
    }

    /* not dhcp packet */
    if (PS_TRUE != IPV4_DHCP_IsDhcpPacket(ipPkt, pktMemLen)) {
        return;
    }

    /* record src ip and dst ip in the ip hdr */
    (VOS_VOID)memset_s(&dhcpRst, sizeof(IPV4_DhcpAnalyseResult), 0, sizeof(IPV4_DhcpAnalyseResult));

    ipFixHdr                    = (ETH_Ipfixhdr *)((VOS_VOID *)ipPkt);
    dhcpRst.srcIPAddr.ipAddr32bit = ipFixHdr->srcAddr;
    dhcpRst.dstIpAddr.ipAddr32bit = ipFixHdr->destAddr;

    /* analyse the dhcp packet */
    dhcpOffset = (ipFixHdr->ipHdrLen << 2) + IP_UDP_HDR_LEN;
    dhcpLen    = pktMemLen - dhcpOffset;

    if (ETH_MAX_FRAME_SIZE <= dhcpLen) {
        IP_ERROR_LOG1("IPV4_DHCP_ProcDhcpPkt : Dhcp Len Error,Len : ", dhcpLen);
        return;
    }

    IPV4_DHCP_AnalyseDhcpPkt(ipPkt + dhcpOffset, dhcpLen, &dhcpRst);

    /* proc analysis dhcp packet result */
    IPV4_DHCP_ProcDhcpMsg(&dhcpRst, rabId);

    IPV4_DHCP_STAT_RECV_DHCP_PKT(1);

    return;
}

/*
 * Function Name: IPV4_DHCP_ShowDebugInfo
 * Description: 打印DHCP Server调试信息
 * History:
 *    1.      2011-3-7  Draft Enact
 */
VOS_VOID IPV4_DHCP_ShowDebugInfo(VOS_VOID)
{
    PS_PRINTF_ERR("Recv Dhcp Packet              :    %d \r\n", g_stDhcpStatStru.recvDhcpPkt);
    PS_PRINTF_ERR("Recv Dhcp Discover  Msg       :    %d \r\n", g_stDhcpStatStru.recvDiscoverMsg);
    PS_PRINTF_ERR("Recv Dhcp Request   Msg       :    %d \r\n", g_stDhcpStatStru.recvRequestMsg);
    PS_PRINTF_ERR("Recv Dhcp Selecting Msg       :    %d \r\n", g_stDhcpStatStru.recvSelectingReqMsg);
    PS_PRINTF_ERR("Recv Dhcp Other     Msg       :    %d \r\n", g_stDhcpStatStru.recvOtherReqMsg);
    PS_PRINTF_ERR("Recv Dhcp Release   Msg       :    %d \r\n", g_stDhcpStatStru.recvReleaseMsg);
    PS_PRINTF_ERR("Recv Dhcp Other Typ Msg       :    %d \r\n", g_stDhcpStatStru.recvOtherTypeMsg);
    PS_PRINTF_ERR("Proc Dhcp Packet    Error     :    %d \r\n", g_stDhcpStatStru.procErr);
    PS_PRINTF_ERR("Send Dhcp Offer               :    %d \r\n", g_stDhcpStatStru.sendOffer);
    PS_PRINTF_ERR("Send Dhcp Ack                 :    %d \r\n", g_stDhcpStatStru.sendAck);
    PS_PRINTF_ERR("Send Dhcp NACK                :    %d \r\n", g_stDhcpStatStru.sendNack);
    PS_PRINTF_ERR("Send Dhcp Mac Frame Succ      :    %d \r\n", g_stDhcpStatStru.sendDhcpPktSucc);
    PS_PRINTF_ERR("Send Dhcp Mac Frame Fail      :    %d \r\n", g_stDhcpStatStru.sendDhcpPktFail);

    return;
}

