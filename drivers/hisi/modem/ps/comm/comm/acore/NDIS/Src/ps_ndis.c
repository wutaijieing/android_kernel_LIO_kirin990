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
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
/*lint -e767*/
#define THIS_FILE_ID PS_FILE_ID_PSNDIS_C
/*lint +e767*/

/*
 * 2 Declare the Global Variable
 */
#define C2A_QUE_SIZE 512         /* �ݶ�512 */
#define USB_DATAREQ_QUE_SIZE 512 /* �ݶ�512 */

#define NDIS_PERIOD_ARP_TMRNAME 1
#define NDIS_ARP_REQ_TMRNAME 2

#define NDIS_ARP_FRAME_REV_OFFSET (((VOS_UINT64)(&(((ETH_ArpFrame *)0)->rev[0]))) & 0xFFFFFFFF)

/*
 * 3 function
 */

/* arp�����м䲿�̶ֹ���ֵ */
VOS_UINT8 g_aucArpReqFixVal[ETH_ARP_FIXED_MSG_LEN] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01 };
/* arp��Ӧ�м䲿�̶ֹ���ֵ */
VOS_UINT8 g_aucArpRspFixVal[ETH_ARP_FIXED_MSG_LEN] = { 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02 };
/* �㲥��ַ��ȫ1 */
VOS_UINT8 g_aucBroadCastAddr[ETH_MAC_ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

VOS_UINT8 g_aucInvalidAddr[IPV4_ADDR_LEN] = {0};

/* ARP���� */
VOS_UINT32 g_ulPeriodicArpCyc = 3000; /* ������ARP�������� */

/* ͳ����Ϣ */
NDIS_StatInfo g_stNdisStatStru        = {0};

NDIS_Entity  g_astNdisEntity[NAS_NDIS_MAX_ITEM] = {{0}};
NDIS_Entity *g_pstNdisEntity                    = g_astNdisEntity;

VOS_UINT32 g_ulNvMtu = 1500; /* IPV6 MTUĬ��ȡֵ */

VOS_UINT32                g_ulNdisLomSwitch = 0;
SPE_MAC_EtherHeader g_stSpeMacHeader  = {{ 0x58, 0x02, 0x03, 0x04, 0x05, 0x06 },
                                              { 0x00, 0x11, 0x09, 0x64, 0x01, 0x01 },
                                              0x00000000 }; /* mac��ַ��ʼ��Ϊ�̶�ֵ */

/*
 * 3 Function
 */
/* ���� */
VOS_UINT32 Ndis_DlSpeSendNcm(NDIS_Entity *ndisEntity, ADS_PktTypeUint8 pktType, IMM_Zc *immZc);

extern VOS_UINT32 DIPC_Pid_InitFunc(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 DIPC_AtMsgProc(const MsgBlock *msgBlock);

extern VOS_UINT32 MUX_Pid_InitFunc(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 MUX_AtMsgProc(const MsgBlock *msgBlock);

/*
 * ��������: C�˵�����λʱNDIS�Ļص�������
 * �޸���ʷ:
 * 1.��    ��: 2019��10��10��
 *   �޸�����: �����ɺ���
 */
VOS_INT Ndis_ModemResetCb(drv_reset_cb_moment_e param, VOS_INT userData)
{
    VOS_UINT32  result;
    VOS_UINT32  resetCbBefore;         /* ������λBEFOREͳ�� */
    VOS_UINT32  resetCbAfter;          /* ������λAFTERͳ�� */
    if (MDRV_RESET_CB_BEFORE == param) {
        PS_PRINTF_ERR("NDIS before reset: enter.");

        /* Ndis ��ʼ�� */
        result = Ndis_Init();
        if (PS_SUCC != result) {
            PS_PRINTF_ERR("APP_Ndis_Pid_InitFunc, Ndis_Init fail!\n");
            return VOS_ERR;
        }

        /* ��λͳ��֮���ͳ��������0 */
        resetCbBefore = g_stNdisStatStru.resetCbBefore;
        resetCbAfter  = g_stNdisStatStru.resetCbAfter;
        (VOS_VOID)memset_s(&g_stNdisStatStru, sizeof(NDIS_StatInfo), 0x00,
                           sizeof(NDIS_StatInfo));

        /* ���Ӹ�λͳ�� */
        g_stNdisStatStru.resetCbBefore = resetCbBefore;
        g_stNdisStatStru.resetCbAfter  = resetCbAfter;
        NDIS_STAT_RESET_CB_BEFORE(1);

        PS_PRINTF_ERR("NDIS before reset: succ.");
        return VOS_OK;
    } else if (MDRV_RESET_CB_AFTER == param) {
        PS_PRINTF_ERR("NDIS after reset enter.");

        /* ���Ӹ�λͳ�� */
        NDIS_STAT_RESET_CB_AFTER(1);
        PS_PRINTF_ERR("NDIS after reset: succ.");
        return VOS_OK;
    } else {
        return VOS_ERROR;
    }
}


/*
 * ��������: ��ȡMAC��ַ
 * �޸���ʷ:
 *  1.��    ��: 2014��10��17��
 *    �޸�����: �����ɺ���
 */
VOS_UINT8 *Ndis_GetMacAddr(VOS_VOID)
{
    /* LTEЭ��ջMAC��ַ */
    static VOS_UINT8 gUcMacAddressPstable[] = {
        0x4c, 0x54, 0x99, 0x45, 0xe5, 0xd5
    };

    return gUcMacAddressPstable;
}

/*
 * ��������: ����Cnf��Ϣ��AT
 * �޸���ʷ:
 *  1.��    ��: 2011��3��16��
 *    �޸�����: �����ɺ���
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
        /* �쳣��ӡ */
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*
 * ��������: NDIS_ADS�������ݽ��ջص�����
 * �޸���ʷ:
 *  1.��    ��: 2011��12��9��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��16��
 *    �޸�����: DSDA���Կ���������޸�Ϊ��չ����ID
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

    /* ���Ӵ�ADS���յ������ݰ�����ͳ�� */
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
 * ��������: NDIS_ADS�������ݽ��ջص�����
 * �޸���ʷ:
 *  1.��    ��: 2018��08��16��
 *    �޸�����: �����ɺ���
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

    /* ���Ӵ�ADS���յ������ݰ�����ͳ�� */
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
 * ��������: App�˼�USBͨ��
 * �޸���ʷ:
 *  1.��    ��: 2011��1��31��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 AppNdis_UsbReadCb(VOS_INT32 rbIdHandle, VOS_VOID *pktNode)
{
    IMM_Zc *immZc = (IMM_Zc *)pktNode; /* ĿǰImmZc��sk_buff��ȫһ�£�ֱ��ǿת */

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

    /* �����쳣�ж� */
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

    /* �����ȡ������չRabId */
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
 * ��������: App�˼�SPEͨ��
 * �޸���ʷ:
 *  1.��    ��: 2015��1��31��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 AppNdis_SpeReadCb(VOS_INT32 lSpePort, VOS_VOID *pktNode)
{
    IMM_Zc *immZc = (IMM_Zc *)pktNode; /* ĿǰImmZc��sk_buff��ȫһ�£�ֱ��ǿת */

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

    /* �����쳣�ж� */
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

    /* �����ȡ������չRabId */
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
 * ��������: A��NDIS��ȡNV��ĳ�ʼ������
 * DHCP Lease Time, �趨��ΧΪ[1, 8784]Сʱ
 * Vodafone    24Сʱ
 * ����        72Сʱ
 * ʱ�侭��DHCP Lease Timeһ��ʱ��PC�������������⣬
 * ���DHCP��Լ���ڣ����PCͨ��NDISͨ��������������ݻ����Ŀ�Ĳ��ɴ����
 * Ŀǰ���ԣ���DHCP Lease TimeС�ڵ���4Sʱ����������Ӱ��ϴ����Զ�����С��ԼΪ1Сʱ
 * Ŀǰû����������8���DHCP Lease Time���ݶ�����Ϊ8784Сʱ(366��)
 * �޸���ʷ:
 *  1.��    ��: 2012��4��24��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_NvItemInit(VOS_VOID)
{
    VOS_UINT32                       rtn;
    VOS_UINT32                       dhcpLeaseHour;
    VOS_UINT32                       ipv6Mtu;
    NDIS_NV_DhcpLeaseHour            ndisDhcpLeaseHour = {0};
    TAF_NDIS_NvIpv6RouterMtu         ndisIPv6Mtu = {0};

    /* ��NV��ȡ����������Ϣ */
    /*lint -e718*/
    /*lint -e732*/
    /*lint -e746*/
    rtn = Ndis_NvimItem_Read(NV_ITEM_NDIS_DHCP_DEF_LEASE_TIME, &ndisDhcpLeaseHour,
                               sizeof(NDIS_NV_DhcpLeaseHour));
    if (PS_SUCC != rtn) {
        PS_PRINTF_ERR("Ndis_NvItemInit, Fail to read NV DHCP_LEASE_TIME: %d\n", rtn);
        ndisDhcpLeaseHour.dhcpLeaseHour = TTF_NDIS_DHCP_DEFAULT_LEASE_HOUR;
    }

    /* NVֵ�Ϸ����ж� */
    dhcpLeaseHour = ndisDhcpLeaseHour.dhcpLeaseHour;
    if ((0 < dhcpLeaseHour) && (dhcpLeaseHour <= TTF_NDIS_DHCP_MAX_LEASE_HOUR)) {
        /* hour -> second */
        g_ulLeaseTime = dhcpLeaseHour * 3600;
    }

    /* ��NV��ȡIPV6 MTU��Ϣ */
    rtn = Ndis_NvimItem_Read(NV_ITEM_IPV6_ROUTER_MTU, &ndisIPv6Mtu, sizeof(TAF_NDIS_NvIpv6RouterMtu));
    /*lint +e746*/
    /*lint +e732*/
    /*lint +e718*/

    if (PS_SUCC != rtn) {
        PS_PRINTF_ERR("Ndis_NvItemInit, Fail to read NV IPV6_MTU: %d\n!", rtn);
        ndisIPv6Mtu.ipv6RouterMtu = TTF_NDIS_IPV6_MTU_DEFAULT;
    }

    /* NVֵ�Ϸ����ж� */
    ipv6Mtu = ndisIPv6Mtu.ipv6RouterMtu;
    if (0 == ipv6Mtu) {
        g_ulNvMtu = TTF_NDIS_IPV6_MTU_DEFAULT;
    } else {
        g_ulNvMtu = ipv6Mtu;
    }

    return PS_SUCC;
}

/*
 * ��������: APP��NDIS���ܵĳ�ʼ������
 * �޸���ʷ:
 *  1.��    ��: 2011��2��10��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_Init(VOS_VOID)
{
    VOS_UINT32                     loop;
    VOS_UINT8                     *macAddr        = VOS_NULL_PTR;
    VOS_UINT16                     payLoad;
    VOS_INT32                      rlt;
    VOS_UINT32                     len            = 2; /*ip payloadΪ2���ֽ�*/
    NDIS_ArpPeriodTimer           *arpPeriodTimer = VOS_NULL_PTR;

    /*lint -e746*/
    macAddr = (VOS_UINT8 *)Ndis_GetMacAddr(); /* ��õ���MAC��ַ */
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

        /* ������ARP��ʱ����ʼ�� */
        arpPeriodTimer               = &(g_astNdisEntity[loop].ipV4Info.arpPeriodTimer);
        arpPeriodTimer->tm          = VOS_NULL_PTR;
        arpPeriodTimer->name       = NDIS_PERIOD_ARP_TMRNAME;
        arpPeriodTimer->timerValue = g_ulPeriodicArpCyc;
    }

    if (PS_SUCC != Ndis_NvItemInit()) { /* NV���ʼ�� */
        PS_PRINTF_ERR("Ndis_Init, Ndis_NvItemInit Fail!\n");
        return PS_FAIL;
    }

    return PS_SUCC;
}
/*
 * ��������: ���з����NCM���ݵķ���
 * �޸���ʷ:
 *  1.��    ��: 2011��2��11��
 *    �޸�����: �����ɺ���
 *     2.��    ��: 2015��2��11��
 *    �޸�����: SPE
 */
VOS_UINT32 Ndis_DlSendNcm(VOS_UINT8 exRabId, ADS_PktTypeUint8 pktType, IMM_Zc *immZc)
{
    VOS_UINT32        result;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;

    /* ʹ��ExRabId��ȡNDISʵ�� */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (IP_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, NDIS_GetEntityByRabId fail!");
        return PS_FAIL;
    }

    result = Ndis_DlUsbSendNcm(exRabId, pktType, immZc);
    return result;
}

/*
 * ��������: ���з����NCM���ݵķ���
 * �޸���ʷ:
 *  1.��    ��: 2011��2��11��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��17��
 *    �޸�����: DSDA������ʹ��ExRabId��ȡNDISʵ��
 *  3.��    ��: 2014��11��17��
 *    �޸�����: SPE���ԣ���USB��SPE�ֿ�����
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

    /* ʹ��ExRabId��ȡNDISʵ�� */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (IP_NULL_PTR == ndisEntity) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_DlSendNcm, NDIS_GetEntityByRabId fail!");
        return PS_FAIL;
    }

    handle = ndisEntity->handle;

    /* ���MAC֡ͷ������ImmZc�ӿڽ�MAC֡ͷ����ImmZc�� */
    if ((ADS_PKT_TYPE_IPV4 == pktType) /* ������ö�� */
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
            /* ��ucExRabId�����ݰ����ͷ���ImmZc��˽���������� */
            tmpApp = (pktType & 0xFF);
            app    = ((VOS_UINT16)(tmpApp << 8)) | (exRabId);
            IMM_ZcSetUserApp(immZc, app);

            NdSer_MacAddrInvalidProc(immZc, index);
            return PS_SUCC;
        }
    } else { /* ���ݰ����������֧�����Ͳ�һ�� */
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

    size = IMM_ZcGetUsedLen(immZc); /* ������̫��֡ͷ�ĳ��� */

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
 * ��������: ����TmerMsg
 * 1.��    ��: 2012��4��28��
 *   �޸�����: �����ɺ���
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
 * ��������: ����TmerMsg
 * �޸���ʷ:
 *  1.��    ��: 2012��4��28��
 *    �޸�����: �����ɺ���
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
 * ��������: DHCP����
 * �޸���ʷ:
 *  1.��    ��: 2011��2��11��
 *    �޸�����: �����ɺ���
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

    /* DHCP���� */
    NDIS_STAT_UL_RECV_DHCPPKT(1);
    IPV4_DHCP_ProcDhcpPkt(data, exRabId, pktMemLen);

    return;
}
/*
 * ��������: ����SpePort����RabId
 * �޸���ʷ:
 *  1.��    ��: 2011��12��9��
 *    �޸�����: �����ɺ���
 */
VOS_UINT8 Ndis_FindRabIdBySpePort(VOS_INT32 spePort, VOS_UINT16 frameType)
{
    VOS_UINT32        loop;
    VOS_UINT8         tmpRabType;
    NDIS_Entity      *ndisEntity = VOS_NULL_PTR;

    switch (frameType) {
        case ARP_PAYLOAD:
            tmpRabType = NDIS_ENTITY_IPV4; /* ARP��Ҳ����SPE */
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
            (tmpRabType == (ndisEntity->rabType & tmpRabType))) { /* ���ݰ��������������һ�� */
            return ndisEntity->rabId;
        }
    }

    return NDIS_INVALID_RABID;
}

/*
 * ��������: ����Handle����RabId
 * �޸���ʷ:
 *  1.��    ��: 2011��12��9��
 *    �޸�����: �����ɺ���
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
            (tmpRabType == (ndisEntity->rabType & tmpRabType))) { /* ���ݰ��������������һ�� */
            return ndisEntity->rabId;
        }
    }

    return NDIS_INVALID_RABID;
}

/*
 * ��������: ������NCM֡�Ĵ���
 * �޸���ʷ:
 *  1.��    ��: 2011��2��11��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��16��
 *    �޸�����: DSDA����
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

    /* ARP���� */
    if (ARP_PAYLOAD == frameType) {
        /* �����쳣�ж� */
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

        /* ARP���������������RabId��������RabId��ΪNdisʵ��������� */
        (VOS_VOID)Ndis_ProcArpMsg((ETH_ArpFrame *)(VOS_VOID *)ipPacket, exRabId); /*lint !e527*/

        /* ������ARP�����Imm_ZcFree�ͷ�ImmZc */
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/

        return;
    }
    /* IPV6���������� */
    if (IPV6_PAYLOAD == frameType) {
        dataLen = IMM_ZcGetUsedLen(immZc);

        if ((dataLen - ETH_MAC_HEADER_LEN) > g_ulNvMtu) {
            NDIS_SPE_MEM_UNMAP(immZc, dataLen);

            IP_NDSERVER_ProcTooBigPkt(exRabId, ((VOS_UINT8 *)ipPacket + ETH_MAC_HEADER_LEN), g_ulNvMtu);

            NDIS_SPE_MEM_MAP(immZc, dataLen);

            /* ����Imm_ZcFree�ͷ�ImmZc */
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

    /* ��MAC����˺�ʣ���IP�����ͣ�ȥ��MAC֡ͷ��ݽ�ADS */
    if (VOS_OK != IMM_ZcRemoveMacHead(immZc)) {
        /*lint -e522*/
        IMM_ZcFree(immZc);
        /*lint +e522*/
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_UlNcmFrmProc, IMM_ZcRemoveMacHead fail!");
        return;
    }

    /* ���IPV4�����Ⱥ�ʵ��Skb���ȣ������һ�£����޸�Skb����Ϊʵ��IP������ */
    if (1 == ipv4Flag) {
        data = IMM_ZcGetDataPtr(immZc);
        if (VOS_NULL_PTR == data) {
            /*lint -e522*/
            IMM_ZcFree(immZc);
            /*lint +e522*/
            return;
        }

        NDIS_SPE_MEM_UNMAP(immZc, cacheLen);

        /* �����쳣�ж� */
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

/* NR�汾���ݲ�����ò��֣���NR Ndis������ʼ���ٵ����ò��� */
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
 * ��������: APP NDIS����PID��ʼ������
 * �޸���ʷ:
 *  1.��    ��: 2011��2��15��
 *    �޸�����: �����ɺ���
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

            /* ������ע��ص�����������C�˵�����λ�Ĵ��� */
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
 * ��������: ������̫��֡�ӿ�
 * �޸���ʷ:
 *  1.��    ��: 2011��2��14��
 *    �޸�����: �����ɺ���
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

    /* ���ݷ��� */
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
 * ��������: ����ARP Request ֡
 * �޸���ʷ:
 *  1.��    ��: 2011��2��11��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��22��
 *  �޸�����: DSDA
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

    if ((0 != reqArp->senderIP.ipAddr32bit) /* ����MAC OS ���ARP����,��Sender IPΪ0 */
        && (arpV4Info->ueIpInfo.ipAddr32bit != reqArp->senderIP.ipAddr32bit)) {
        /* ԴUE IP���������ò������������������ */
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_ProcReqArp,  SenderIP Error!", reqArp->senderIP.ipAddr);
        NDIS_STAT_PROC_ARP_FAIL(1);
        return PS_SUCC;
    }

    /* ����PC MAC��ַ */
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

    /* ���ARP���ظ���Ӧ */
    if ((reqArp->targetIP.ipAddr32bit == reqArp->senderIP.ipAddr32bit) || (0 == reqArp->senderIP.ipAddr32bit)) {
        return PS_SUCC;
    }

    /* ������Ӧ */
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

    /* ����ARP Reply */
    NDIS_STAT_DL_SEND_ARP_REPLY(1);
    (VOS_VOID)Ndis_SendMacFrm((VOS_UINT8 *)reqArp, sizeof(ETH_ArpFrame), rabId);

    return PS_SUCC;
}

/*
 * ��������: ����ARP Reply ֡,����PC��MAC��ַ
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��22��
 *    �޸�����: DSDA
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
        /* ����PC MAC��ַ */
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
 * ��������: IPV4 PDN��Ϣ����
 * �޸���ʷ:
 *  1.��    ��: 2011��3��15��
 *    �޸�����: �����ɺ���
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

    /* ʹ��������DNS */
    if (PS_TRUE == nasNdisInfo->opDnsPrim) {
        rlt = memcpy_s(ipV4Info->primDnsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->dnsPrimAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    } else {
        ipV4Info->primDnsAddr.ipAddr32bit = 0;
    }

    /* ʹ�������ø�DNS */
    if (PS_TRUE == nasNdisInfo->opDnsSec) {
        rlt = memcpy_s(ipV4Info->secDnsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->dnsSecAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        ipV4Info->secDnsAddr.ipAddr32bit = 0;
    }

    /* ʹ����������WINS */
    if (PS_TRUE == nasNdisInfo->opWinsPrim) {
        rlt = memcpy_s(ipV4Info->primWinsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->winsPrimAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);

    } else {
        ipV4Info->primWinsAddr.ipAddr32bit = 0;
    }

    /* ʹ�������ø�WINS */
    if (PS_TRUE == nasNdisInfo->opWinsSec) {
        rlt = memcpy_s(ipV4Info->secWinsAddr.ipAddr8bit, IPV4_ADDR_LEN,
                         nasNdisInfo->winsSecAddrInfo.ipv4AddrU8, IPV4_ADDR_LEN);
        NDIS_CHK_SEC_RETURN_NO_VAL(rlt != EOK);
    } else {
        ipV4Info->secWinsAddr.ipAddr32bit = 0;
    }

    /* PCSCF��ʱ������,���������� */

    ipV4Info->ipAssignStatus = IPV4_DHCP_ADDR_STATUS_FREE;

    return;
}

/*
 * ��������: NDIS����������ARP��ʱ��
 * �޸���ʷ:
 *  1.��    ��: 2012��4��19��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_StartARPTimer(NDIS_Entity *ndisEntity)
{
#if (VOS_OS_VER != VOS_WIN32)
    VOS_UINT32                  rtn;
    NDIS_ArpPeriodTimer        *arpPeriodTimer = VOS_NULL_PTR;

    /* ���ָ���ж� */
    if (VOS_NULL_PTR == ndisEntity) {
        return PS_FAIL;
    }

    /* ����Ƿ���Ҫ����ARP��ʱ�� */
    if ((NDIS_ENTITY_IPV4 != (ndisEntity->rabType & NDIS_ENTITY_IPV4)) ||
        (PS_TRUE == ndisEntity->ipV4Info.arpInitFlg)) {
        return PS_SUCC;
    }

    arpPeriodTimer = &(ndisEntity->ipV4Info.arpPeriodTimer);

    /* ����������У���ͣ�� */
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
 * ��������: NDISֹͣ������ARP��ʱ��
 * �޸���ʷ:
 *  1.��    ��: 2012��4��19��
 *    �޸�����: �����ɺ���
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
 * ��������: Ndis���ExRabIdȡֵ�Ƿ��ںϷ���Χ��
 * �޸���ʷ:
 *  1.��    ��: 2018��08��21��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_ChkRabIdValid(VOS_UINT8 exRabId)
{
    /* B5000���Ժ�İ汾�£�Ndis������ΪIFACE ID */
    if (exRabId >= PS_IFACE_ID_BUTT) {
        return PS_FAIL;
    }

    return PS_SUCC;
}
#if (VOS_OS_VER != VOS_WIN32)
/*
 * ��������: ׮����
 * �޸���ʷ:
 *  1.��    ��: 2018��08��21��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 ADS_DL_RegDlDataCallback(VOS_UINT8 rabId, RCV_DL_DATA_FUNC pFunc, VOS_UINT32 exParam)
{
    return PS_SUCC;
}
#endif
#else

#if (VOS_OS_VER != VOS_WIN32)
/*
 * ��������: ׮����
 * �޸���ʷ:
 *  1.��    ��: 2018��08��21��
 *    �޸�����: �����ɺ���
 */
int ads_iface_register_rx_handler(VOS_UINT8 ifaceId, struct ads_iface_rx_handler_s *rxHandler)
{
    return PS_SUCC;
}
#endif
/*
 * ��������: Ndis���ExRabIdȡֵ�Ƿ��ںϷ���Χ��
 * �޸���ʷ:
 *  1.��    ��: 2012��12��7��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��15��
 *    �޸�����: DSDA���Կ���:��ModemID��RabId�������
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
 * ��������: ����ExRabId����NDISʵ��
 * �޸���ʷ:
 *  1.��    ��: 2013��1��15��
 *    �޸�����: �����ɺ���
 */
NDIS_Entity *NDIS_GetEntityByRabId(VOS_UINT8 exRabId)
{
    VOS_UINT16 i = 0;

    /* ��ѯ�Ƿ��Ѵ�����ӦEntity */
    do {
        if ((PS_TRUE == g_astNdisEntity[i].used) && (exRabId == g_astNdisEntity[i].rabId)) {
            /* �ҵ���Ӧʵ�� */
            return &g_astNdisEntity[i];
        }

    } while ((++i) < NAS_NDIS_MAX_ITEM);

    return VOS_NULL_PTR;
}

/*
 * ��������: ����һ�����е�NDISʵ��
 * �޸���ʷ:
 *  1.��    ��: 2013��1��15��
 *    �޸�����: �����ɺ���
 */
NDIS_Entity *NDIS_AllocEntity(VOS_VOID)
{
    VOS_UINT16 i = 0;

    /* ���ص�һ�����е�ʵ�� */
    do {
        if (PS_FALSE == g_astNdisEntity[i].used) {
            /* �ҵ�����ʵ�� */
            return &g_astNdisEntity[i];
        }

    } while ((++i) < NAS_NDIS_MAX_ITEM);

    return VOS_NULL_PTR;
}

/*
 * ��������: PDN IPV4��ַ��Ϣ���
 * �޸���ʷ:
 *  1.��    ��: 2011��12��11��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_CheckIpv4PdnInfo(const AT_NDIS_Ipv4PdnInfo *ipv4PdnInfo)
{
    if (PS_FALSE == ipv4PdnInfo->opPdnAddr) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_CheckIpv4PdnInfo,  pstIpv4PdnInfo->bitOpPdnAddr is false!");
        return PS_FAIL;
    }

    /* PDN��ַ�����ص�ַ���Ϊȫ0����Ҳʧ�� */
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
 * ��������: ���������ò����Ƿ�Ϸ�
 * �޸���ʷ:
 *  1.��    ��: 2011��12��23��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_PdnV4PdnCfg(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo, NDIS_Entity *ndisEntity)
{
    /* IPV4��ַ��� */
    if (PS_FALSE == nasNdisInfo->opIpv4PdnInfo) { /* ԭ��ָʾIPV4��Ϣ��Ч */
        NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnV4PdnCfg,  bitOpIpv4PdnInfo is false!");
        return PS_FAIL;
    }

    if (PS_SUCC != Ndis_CheckIpv4PdnInfo(&(nasNdisInfo->ipv4PdnInfo))) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnV4PdnCfg,  Ndis_CheckIpv4PdnInfo fail!");
        return PS_FAIL;
    }

    Ndis_Ipv4PdnInfoCfg(&(nasNdisInfo->ipv4PdnInfo), ndisEntity);

    /* ����NDISʵ��������� */
    ndisEntity->rabType |= NDIS_ENTITY_IPV4;

    return PS_SUCC;
}

/*
 * ��������: IPV6 PDN��Ϣ����
 * �޸���ʷ:
 *  1.��    ��: 2011��12��23��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_PdnV6PdnCfg(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo, NDIS_Entity *ndisEntity)
{
    VOS_UINT8 exRabId;

    /* IPV6��ַ��� */
    if (PS_FALSE == nasNdisInfo->opIpv6PdnInfo) { /* ԭ��ָʾIPV6��Ϣ��Ч */
        NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnV6PdnCfg,  bitOpIpv6PdnInfo is false!");
        return PS_FAIL;
    }

    exRabId = NDIS_FORM_EXBID(nasNdisInfo->modemId, nasNdisInfo->rabId);

    if (PS_SUCC != NdSer_CheckIpv6PdnInfo(&(nasNdisInfo->ipv6PdnInfo))) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnV6PdnCfg,  NdSer_CheckIpv6PdnInfo fail!");
        return PS_FAIL;
    }

    /* ��ND SERVER API  ����IPV6��ַ��Ϣ��ND SERVER */
    NdSer_Ipv6PdnInfoCfg(exRabId, &(nasNdisInfo->ipv6PdnInfo));

    /* ����NDISʵ������ */
    ndisEntity->rabType |= NDIS_ENTITY_IPV6;

    return PS_SUCC;
}

/*
 * ��������: NDIS��AT���ص�����ȷ�Ͻ������
 * �޸���ʷ:
 *  1.��    ��: 2012��4��25��
 *    �޸�����: �����ɺ���
 */
VOS_UINT8 Ndis_AtCnfResultProc(const AT_NDIS_PdnInfoCfgReq *nasNdisInfo, VOS_UINT32 v4Ret,
                               VOS_UINT32 v6Ret)
{
    VOS_UINT8 result;

    /* �������ý����AT��������CNFԭ�� */
    if ((PS_TRUE == nasNdisInfo->opIpv4PdnInfo) && (PS_TRUE == nasNdisInfo->opIpv6PdnInfo)) {
        if ((PS_SUCC == v4Ret) && (PS_SUCC == v6Ret)) { /* IPV4��IPV6���ö��ɹ� */
            result = AT_NDIS_PDNCFG_CNF_SUCC;
        } else if (PS_SUCC == v4Ret) { /* ֻ��IPV4���óɹ� */
            result = AT_NDIS_PDNCFG_CNF_IPV4ONLY_SUCC;
        } else { /* ֻ��IPV6���óɹ� */
            result = AT_NDIS_PDNCFG_CNF_IPV6ONLY_SUCC;
        }
    } else if (PS_TRUE == nasNdisInfo->opIpv4PdnInfo) { /* ֻ������IPV4 */
        result = AT_NDIS_PDNCFG_CNF_IPV4ONLY_SUCC;
    } else { /* ֻ������IPV6 */
        result = AT_NDIS_PDNCFG_CNF_IPV6ONLY_SUCC;
    }

    return result;
}
/*
 * ��������: PDN��ַ��Ϣ��������
 * �޸���ʷ:
 *  1.��    ��: 2011��12��31��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��15��
 *    �޸�����: DSDA���ԣ���Ҫ���ӶԽӿ���Ϣ��ModemId�Ĵ���
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

    /* �����쳣��� */
    if ((sizeof(AT_NDIS_PdnInfoCfgReq) - VOS_MSG_HEAD_LENGTH) > nasNdisInfo->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc: input msg length less than struc",
                        nasNdisInfo->msgId);
        return;
    }

    /* ������չ��RabId */
    exRabId    = NDIS_FORM_EXBID(nasNdisInfo->modemId, nasNdisInfo->rabId);
    handle     = nasNdisInfo->handle;
    lSpePort     = nasNdisInfo->iSpePort;
    speIpfFlag = nasNdisInfo->ipfFlag;

    cfgCnf.result  = AT_NDIS_PDNCFG_CNF_FAIL;
    cfgCnf.rabId   = nasNdisInfo->rabId;
    cfgCnf.modemId = nasNdisInfo->modemId;

    /* ExRabId������Χ��Ч�Լ�顣�����ʧ�ܣ���ֱ����AT�ظ�����ʧ�� */
    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf),
                                  ID_AT_NDIS_PDNINFO_CFG_CNF);
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc,  Ndis_ChkRabIdValid fail!");
        return;
    }

    /* �������ExRabId���Ҳ���NDISʵ�壬�����һ�����е�NDISʵ�� */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        /* ������䲻�����е�NDISʵ�壬�򷵻� */
        ndisEntity = NDIS_AllocEntity();
        if (VOS_NULL_PTR == ndisEntity) {
            /* ��AT�ظ�PDN����ʧ�� */
            (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf),
                                      ID_AT_NDIS_PDNINFO_CFG_CNF);
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc,  NDIS_AllocEntity failed!");
            return;
        }

        /* �ó���֮ǰû�ж�Ӧ��NDISʵ�壬������Чֵ */
        ndisEntity->rabType    = NDIS_RAB_NULL;
        ndisEntity->handle     = NDIS_INVALID_HANDLE;
        ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
        ndisEntity->speIpfFlag = PS_FALSE;
    }

    v4Ret = Ndis_PdnV4PdnCfg(nasNdisInfo, ndisEntity);
    v6Ret = Ndis_PdnV6PdnCfg(nasNdisInfo, ndisEntity);

    if ((PS_FAIL == v6Ret) && (PS_FAIL == v4Ret)) { /* ���IPV4��IPV6����ָʾ��Ϣ����Ч��Ҳ��Ϊ����ʧ�� */
        /* ��AT�ظ�PDN����ʧ�� */
        (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&cfgCnf, sizeof(AT_NDIS_PdnInfoCfgCnf),
                                  ID_AT_NDIS_PDNINFO_CFG_CNF);
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_PdnInfoCfgProc,  Ipv4 and Ipv6 Cfg all fail!");
        return;
    }

    ndisEntity->used       = PS_TRUE;   /* ���ø�NDISʵ��Ϊʹ��״̬ */
    ndisEntity->rabId      = exRabId; /* ����չRabId�浽��ӦNDISʵ���� */
    ndisEntity->handle     = handle;  /* ����Handle��NDISʵ���� */
    ndisEntity->spePort     = lSpePort;  /* ����SPE Port��NDISʵ���� */
    ndisEntity->speIpfFlag = speIpfFlag;

    cfgCnf.result  = Ndis_AtCnfResultProc(nasNdisInfo, v4Ret, v6Ret);
    cfgCnf.rabType = ndisEntity->rabType;

    /* �������ڷ���ARP�Ķ�ʱ�� */
    if (PS_SUCC != Ndis_StartARPTimer(ndisEntity)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_ConfigArpInfo StartTmr Failed!");
        return;
    }

    ndisEntity->spePort = NDIS_INVALID_SPEPORT;

    /* ��ADSע�����лص�:ֻע��һ�� */
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
 * ��������: PDN�ͷ�
 * �޸���ʷ:
 *  1.��    ��: 2011��12��7��
 *    �޸�����: �û����ں��޸�
 *  1.��    ��: 2013��1��15��
 *    �޸�����: DSDA���Կ���:
 */
VOS_VOID Ndis_PdnRel(const AT_NDIS_PdnInfoRelReq *nasNdisRel)
{
    VOS_UINT8                    exRabId;
    NDIS_Entity                 *ndisEntity     = VOS_NULL_PTR;
    NDIS_ArpPeriodTimer         *arpPeriodTimer = VOS_NULL_PTR;
    AT_NDIS_PdnInfoRelCnf        relCnf;

    NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_PdnRel entered!");

    /* �����쳣��� */
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

    /* ���������ARP��ʱ���������У���ͣ�� */
    Ndis_StopARPTimer(arpPeriodTimer);

    /* ����ND SERVER API �ͷŸ�RabId��ӦND SERVERʵ�� */
    if (NDIS_ENTITY_IPV6 == (ndisEntity->rabType & NDIS_ENTITY_IPV6)) {
        NdSer_Ipv6PdnRel(exRabId);
    }

    /* ���¸�RabId��ӦNDISʵ��Ϊ�� */
    ndisEntity->rabType    = NDIS_RAB_NULL;
    ndisEntity->rabId      = NDIS_INVALID_RABID;
    ndisEntity->handle     = NDIS_INVALID_HANDLE;
    ndisEntity->used       = PS_FALSE;
    ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
    ndisEntity->speIpfFlag = PS_FALSE;

    /* NDIS��AT�ظ��ͷ�ȷ��ԭ�� */
    relCnf.result = AT_NDIS_SUCC;
    (VOS_VOID)Ndis_SndMsgToAt((VOS_UINT8 *)&relCnf, sizeof(AT_NDIS_PdnInfoRelCnf), ID_AT_NDIS_PDNINFO_REL_CNF);

    return;
}

/*
 * ��������: IFACE UP CFG��Ϣ����ת��PND INFO CFG��Ϣ����
 * �޸���ʷ:
 *  1.��    ��: 2018��08��16��
 *    �޸�����: ����
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
    ndisPdnInfo->modemId        = NDIS_GET_MODEMID_FROM_EXBID(ndisIFaceInfo->iFaceId); /* ��Iface IDת��ModemId+RabId */
    ndisPdnInfo->rabId = NDIS_GET_BID_FROM_EXBID(ndisIFaceInfo->iFaceId); /* ��Iface IDת��ModemId+RabId */
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
 * ��������: IFACE UP��Ϣ����
 * �޸���ʷ:
 *  1.��    ��: 2018��08��16��
 *    �޸�����: �����ɺ���
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

    /* �����쳣��� */
    if ((sizeof(AT_NDIS_IfaceUpConfigInd) - VOS_MSG_HEAD_LENGTH) > ifacInfo->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc: input msg length less than struc", ifacInfo->msgId);
        return;
    }

    /* ������չ��RabId */
    exRabId = ifacInfo->iFaceId;
    handle  = ifacInfo->handle;

    /* ExRabId������Χ��Ч�Լ�顣�����ʧ�ܣ���ֱ����AT�ظ�����ʧ�� */
    if (PS_SUCC != Ndis_ChkRabIdValid(exRabId)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc,  Ndis_ChkRabIdValid fail!");
        return;
    }

    /* �������ExRabId���Ҳ���NDISʵ�壬�����һ�����е�NDISʵ�� */
    ndisEntity = NDIS_GetEntityByRabId(exRabId);
    if (VOS_NULL_PTR == ndisEntity) {
        /* ������䲻�����е�NDISʵ�壬�򷵻� */
        ndisEntity = NDIS_AllocEntity();
        if (VOS_NULL_PTR == ndisEntity) {
            NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc,  NDIS_AllocEntity failed!");
            return;
        }

        /* �ó���֮ǰû�ж�Ӧ��NDISʵ�壬������Чֵ */
        ndisEntity->rabType    = NDIS_RAB_NULL;
        ndisEntity->handle     = NDIS_INVALID_HANDLE;
        ndisEntity->spePort     = NDIS_INVALID_SPEPORT;
        ndisEntity->speIpfFlag = PS_FALSE;
    }

    Ndis_ATIfaceUpCfgTransToPdnInfoCfg(ifacInfo, &pdnInfo);
    v4Ret = Ndis_PdnV4PdnCfg(&pdnInfo, ndisEntity);
    v6Ret = Ndis_PdnV6PdnCfg(&pdnInfo, ndisEntity);

    if ((PS_FAIL == v6Ret) && (PS_FAIL == v4Ret)) { /* ���IPV4��IPV6����ָʾ��Ϣ����Ч��Ҳ��Ϊ����ʧ�� */
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc,  Ipv4 and Ipv6 Cfg all fail!");
        return;
    }

    ndisEntity->used       = PS_TRUE;   /* ���ø�NDISʵ��Ϊʹ��״̬ */
    ndisEntity->rabId      = exRabId; /* ����չRabId�浽��ӦNDISʵ���� */
    ndisEntity->handle     = handle;  /* ����Handle��NDISʵ���� */
    ndisEntity->spePort     = 0;         /* ����SPE Port��NDISʵ���� */
    ndisEntity->speIpfFlag = 0;

    /* �������ڷ���ARP�Ķ�ʱ�� */
    if (PS_SUCC != Ndis_StartARPTimer(ndisEntity)) {
        NDIS_ERROR_LOG(NDIS_TASK_PID, "Ndis_IfaceUpCfgProc StartTmr Failed!");
        return;
    }

    ndisEntity->spePort = NDIS_INVALID_SPEPORT;

    (VOS_VOID)memset_s(&ifaceRxHandle, sizeof(ifaceRxHandle), 0, sizeof(ifaceRxHandle));
    ifaceRxHandle.user_data     = exRabId;
    ifaceRxHandle.rx_func       = Ndis_DlAdsDataRcvV2;
    ifaceRxHandle.rx_cmplt_func = VOS_NULL_PTR;

    /* ��ADSע�����лص� */
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
 * ��������: IFACE DOWN��Ϣ����
 * �޸���ʷ:
 *  1.��    ��: 2018��08��16��
 *    �޸�����: �����ɺ���
 */
VOS_VOID Ndis_IfaceDownCfgProc(const AT_NDIS_IfaceDownConfigInd *ifacInfo)

{
    VOS_UINT8            exRabId;
    NDIS_Entity         *ndisEntity     = VOS_NULL_PTR;
    NDIS_ArpPeriodTimer *arpPeriodTimer = VOS_NULL_PTR;

    NDIS_INFO_LOG(NDIS_TASK_PID, "Ndis_IfaceDownCfgProc entered!");

    /* �����쳣��� */
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

    /* ���������ARP��ʱ���������У���ͣ�� */
    Ndis_StopARPTimer(arpPeriodTimer);

    /* ����ND SERVER API �ͷŸ�RabId��ӦND SERVERʵ�� */
    if (NDIS_ENTITY_IPV6 == (ndisEntity->rabType & NDIS_ENTITY_IPV6)) {
        NdSer_Ipv6PdnRel(exRabId);
    }

    /* ���¸�RabId��ӦNDISʵ��Ϊ�� */
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
 * ��������: ����ARP Request ֡��Ethenet��
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 Ndis_SendRequestArp(NDIS_Ipv4Info *arpInfoItem, VOS_UINT8 exRabId)
{
    ETH_ArpFrame arpReq;
    errno_t      rlt;
    /* ֮ǰһ�η��͵�Req��δ�ܵ�Reply���� */
    if (PS_FALSE == arpInfoItem->arpRepFlg) {
        NDIS_STAT_ARPREPLY_NOTRECV(1);
        /* ��һ�θ澯��־ */
    }

    (VOS_VOID)memset_s((VOS_UINT8 *)&arpReq, sizeof(ETH_ArpFrame), 0, sizeof(ETH_ArpFrame));

    /* ��ARP Request */
    rlt = memcpy_s(arpReq.dstAddr, ETH_MAC_ADDR_LEN, g_aucBroadCastAddr, ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    rlt = memcpy_s(arpReq.srcAddr, ETH_MAC_ADDR_LEN, arpInfoItem->macFrmHdr + ETH_MAC_ADDR_LEN,
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);
    arpReq.frameType = ARP_PAYLOAD;

    /* ����Ĺ̶����� */
    rlt = memcpy_s(((VOS_UINT8 *)&arpReq + ETH_MAC_HEADER_LEN), ETH_ARP_FIXED_MSG_LEN, g_aucArpReqFixVal,
                     ETH_ARP_FIXED_MSG_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    /* Payload���ֵ�MAC��ַ���� */
    (VOS_VOID)memset_s(arpReq.targetAddr, ETH_MAC_ADDR_LEN, 0, ETH_MAC_ADDR_LEN);
    rlt = memcpy_s(arpReq.senderAddr, ETH_MAC_ADDR_LEN, arpInfoItem->macFrmHdr + ETH_MAC_ADDR_LEN,
                     ETH_MAC_ADDR_LEN);
    NDIS_CHK_SEC_RETURN_VAL(rlt != EOK, PS_FAIL);

    /* ����IP */
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
 * ��������: ���������������ARP֡
 * �޸���ʷ:
 *  1.��    ��: 2009��12��31��
 *    �޸�����: �����ɺ���
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
 * �޸���ʷ:
 * 1.��    ��: 2011��3��16��
 *   �޸�����: �����ɺ���
 */
VOS_VOID Ndis_AtMsgProc(const MsgBlock *msgBlock)
{
    AT_NDIS_MsgIdUint32 msgId;

    COVERITY_TAINTED_SET(msgBlock->value); /* ӥ�۲�׮ */

    /* �����쳣���� */
    if (sizeof(MSG_Header) - VOS_MSG_HEAD_LENGTH > msgBlock->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_AtMsgProc: input msg length less than struc MSG_Header",
                        msgBlock->ulLength);
        return;
    }

    msgId = ((MSG_Header *)(VOS_VOID *)msgBlock)->msgName;

    switch (msgId) {
        case ID_AT_NDIS_PDNINFO_CFG_REQ: /* ������Ϣ�Ĳ�ͬ����AT��ͬ������ */
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
 * ��������: NDIS����ADS��Ϣ������
 * �޸���ʷ:
 *  1.��    ��: 2011��12��15��
 *    �޸�����: �����ɺ���
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
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV4: /* DHCP�� */
            Ndis_DHCPPkt_Proc(adsNdisMsg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV6: /* DHCPV6�� */
            NdSer_DhcpV6PktProc(adsNdisMsg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_ICMPV6: /* ND��ECHO REQUEST�� */
            NdSer_NdAndEchoPktProc(adsNdisMsg);
            break;

        default:
            NDIS_WARNING_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgProc:Other Msg!", adsNdisMsg->ipPacketType);
            break;
    }

    /* ������ɺ��ͷ�ImmZc */
    /*lint -e522*/
    IMM_ZcFree(adsNdisMsg->data);
    /*lint +e522*/

    return;
}

/*
 * ��������: ADS V2��Ϣ����ת��V1��ʽ
 * �޸���ʷ:
 *  1.��    ��: 2018��08��16��
 *    �޸�����: �����ɺ���
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
 * ��������: NDIS����ADS��Ϣ������
 * �޸���ʷ:
 *  1.��    ��: 2011��12��15��
 *    �޸�����: �����ɺ���
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
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV4: /* DHCP�� */
            Ndis_DHCPPkt_Proc(&adsNdisV1Msg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_DHCPV6: /* DHCPV6�� */
            NdSer_DhcpV6PktProc(&adsNdisV1Msg);
            break;
        case ADS_NDIS_IP_PACKET_TYPE_ICMPV6: /* ND��ECHO REQUEST�� */
            NdSer_NdAndEchoPktProc(&adsNdisV1Msg);
            break;

        default:
            NDIS_WARNING_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgProcV2:Other Msg!", adsNdisV1Msg.ipPacketType);
            break;
    }

    /* ������ɺ��ͷ�ImmZc */
    /*lint -e522*/
    IMM_ZcFree(adsNdisMsg->data);
    /*lint +e522*/

    return;
}

/*
 * ��������: NDIS����ADS��Ϣ������
 * �޸���ʷ:
 *  1.��    ��: 2018��08��16��
 *    �޸�����: �����ɺ���
 */
VOS_VOID Ndis_AdsMsgDispatch(const MsgBlock *msgBlock)
{
    AT_NDIS_MsgIdUint32 msgId;

    COVERITY_TAINTED_SET(msgBlock->value); /* ӥ�۲�׮ */

    /* �����쳣���� */
    if (sizeof(MSG_Header) - VOS_MSG_HEAD_LENGTH > msgBlock->ulLength) {
        NDIS_ERROR_LOG1(NDIS_TASK_PID, "Ndis_AdsMsgDispatch: input msg length less than struc MSG_Header",
                        msgBlock->ulLength);
        return;
    }

    msgId = ((MSG_Header *)(VOS_VOID *)msgBlock)->msgName;

    switch (msgId) {
        case ID_ADS_NDIS_DATA_IND: /* ������Ϣ�Ĳ�ͬ����AT��ͬ������ */
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
 * ��������: NDIS���ո�ģ����Ϣ������
 * �޸���ʷ:
 *  1.��    ��: 2011��2��15��
 *    �޸�����: �����ɺ���
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

        case ACPU_PID_ADS_UL: /* ADSͨ��OSA��Ϣ����DHCP��ND SERVER����NDISģ�� */
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
 * ��������: NDIS��FID��ʼ������
 * �޸���ʷ:
 *  1.��    ��: 2008��9��17��
 *    �޸�����: �����ɺ���
 */
VOS_UINT32 APP_NDIS_FidInit(enum VOS_InitPhaseDefine phase)
{
    VOS_UINT32 result;

    switch (phase) {
        case VOS_IP_LOAD_CONFIG:

            /* ע��NDIS PID */
            result = VOS_RegisterPIDInfo(NDIS_TASK_PID, (InitFunType)APP_Ndis_Pid_InitFunc,
                                           (MsgFunType)APP_Ndis_PidMsgProc);
            if (VOS_OK != result) {
                PS_PRINTF_ERR("APP_NDIS_FidInit, register NDIS PID fail!\n");
                return VOS_ERR;
            }

            /* ע��ND SERVER PID */
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

/* ======================================ͳ����Ϣ============================== */
/*
 * ��������: ��ʾ�շ���ҵ��������Ϣ
 *  �޸���ʷ:
 *   1.��    ��: 2009��6��16��
 *     �޸�����: �����ɺ���
 * �޸���ʷ:
 *   2.��    ��: 2013��1��22��
 *     �޸�����: DSDA
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
 * ��������: ��ӡIP��ַ��Ϣ
 * �޸���ʷ:
 *  1.��    ��: 2009��6��16��
 *    �޸�����: �����ɺ���
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
 * ��������: ��ʾ��Ч��ʵ����Ϣ
 * �޸���ʷ:
 *  1.��    ��: 2011��3��16��
 *    �޸�����: �����ɺ���
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
 * ��������: ��ʾ���е�ʵ����Ϣ
 * �޸���ʷ:
 *  1.��    ��: 2011��4��18��
 *    �޸�����: �����ɺ���
 *  2.��    ��: 2013��1��16��
 *    �޸�����: DSDA
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
