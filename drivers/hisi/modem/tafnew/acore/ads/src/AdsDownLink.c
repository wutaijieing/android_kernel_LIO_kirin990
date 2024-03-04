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
#include "AdsDownLink.h"
#include "AdsDebug.h"
#include "mdrv.h"
#include "AcpuReset.h"
#include "AdsFilter.h"
#include "ads_dhcp_interface.h"
#include "AdsMntn.h"
#include "ads_mip_pif.h"
#include "securec.h"

#include "mn_comm_api.h"
#include "securec.h"

/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_ADS_DOWNLINK_C

VOS_UINT32 g_adsDlDiscardPktFlag = VOS_FALSE; /* ADS����ֱ�Ӷ������� */


VOS_INT ADS_DL_CCpuResetCallback(drv_reset_cb_moment_e param, VOS_INT userData)
{
    ADS_CcpuResetInd *msg = VOS_NULL_PTR;

    /* ����Ϊ0��ʾ��λǰ���� */
    if (MDRV_RESET_CB_BEFORE == param) {
        ADS_TRACE_HIGH("before reset enter.\n");

        /* ������Ϣ */
        msg = (ADS_CcpuResetInd *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL, sizeof(ADS_CcpuResetInd));
        if (VOS_NULL_PTR == msg) {
            ADS_TRACE_HIGH("before reset: alloc msg failed.\n");
            return VOS_ERROR;
        }

        /* ��д��Ϣͷ */
        TAF_CfgMsgHdr((MsgBlock *)msg, ACPU_PID_ADS_DL, ACPU_PID_ADS_DL,
                      sizeof(ADS_CcpuResetInd) - VOS_MSG_HEAD_LENGTH);
        msg->msgId = ID_ADS_CCPU_RESET_START_IND;

        /* ����Ϣ */
        if (VOS_OK != PS_SEND_MSG(ACPU_PID_ADS_DL, msg)) {
            ADS_TRACE_HIGH("before reset: send msg failed.\n");
            return VOS_ERROR;
        }

        /* �ȴ��ظ��ź�����ʼΪ��״̬���ȴ���Ϣ��������ź��������� */
        if (VOS_OK != VOS_SmP(ADS_GetDLResetSem(), ADS_RESET_TIMEOUT_LEN)) {
            ADS_TRACE_HIGH("before reset VOS_SmP failed.\n");
            ADS_DBG_DL_RESET_LOCK_FAIL_NUM(1);
            return VOS_ERROR;
        }

        ADS_TRACE_HIGH("before reset succ.\n");
        return VOS_OK;
    }
    /* ��λ�� */
    else if (MDRV_RESET_CB_AFTER == param) {
        ADS_TRACE_HIGH("after reset enter.\n");

        /* ������Ϣ */
        msg = (ADS_CcpuResetInd *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL, sizeof(ADS_CcpuResetInd));
        if (VOS_NULL_PTR == msg) {
            ADS_TRACE_HIGH("after reset: alloc msg failed.\n");
            return VOS_ERROR;
        }

        /* ��д��Ϣͷ */
        TAF_CfgMsgHdr((MsgBlock *)msg, ACPU_PID_ADS_DL, ACPU_PID_ADS_DL,
                      sizeof(ADS_CcpuResetInd) - VOS_MSG_HEAD_LENGTH);
        msg->msgId = ID_ADS_CCPU_RESET_END_IND;

        /* ����Ϣ */
        if (VOS_OK != PS_SEND_MSG(ACPU_PID_ADS_DL, msg)) {
            ADS_TRACE_HIGH("after reset: send msg failed.\n");
            return VOS_ERROR;
        }

        ADS_TRACE_HIGH("after reset: succ.\n");
        ADS_DBG_DL_RESET_SUCC_NUM(1);
        return VOS_OK;
    } else {
        return VOS_ERROR;
    }
}


VOS_INT32 ADS_DL_IpfIntCB(VOS_VOID)
{
    if (ADS_IPF_MODE_INT == ADS_GET_IPF_MODE()) {
        ADS_DL_WakeLock();
        ADS_DL_ProcIpfResult();
        ADS_DL_WakeUnLock();
    } else {
        ADS_DL_SndEvent(ADS_DL_EVENT_IPF_RD_INT);
    }
    ADS_DBG_DL_RCV_IPF_RD_INT_NUM(1);

    return VOS_OK;
}


VOS_INT32 ADS_DL_IpfAdqEmptyCB(ipf_adq_empty_e eAdqEmpty)
{
    if (IPF_EMPTY_ADQ0 == eAdqEmpty) {
        ADS_DBG_DL_ADQ_RCV_AD0_EMPTY_INT_NUM(1);
    } else if (IPF_EMPTY_ADQ1 == eAdqEmpty) {
        ADS_DBG_DL_ADQ_RCV_AD1_EMPTY_INT_NUM(1);
    } else {
        ADS_DBG_DL_ADQ_RCV_AD0_EMPTY_INT_NUM(1);
        ADS_DBG_DL_ADQ_RCV_AD1_EMPTY_INT_NUM(1);
    }

    ADS_DL_SndEvent(ADS_DL_EVENT_IPF_ADQ_EMPTY_INT);
    ADS_DBG_DL_RCV_IPF_ADQ_EMPTY_INT_NUM(1);
    return VOS_OK;
}


VOS_VOID ADS_DL_RcvTiAdqEmptyExpired(VOS_UINT32 param, VOS_UINT32 timerName)
{
    /* ��������ADQ���жϴ����¼� */
    ADS_DL_SndEvent(ADS_DL_EVENT_IPF_ADQ_EMPTY_INT);
    ADS_DBG_DL_ADQ_EMPTY_TMR_TIMEOUT_NUM(1);
    return;
}


VOS_VOID ADS_DL_RcvTiCcoreResetDelayExpired(VOS_UINT32 param, VOS_UINT32 timerName)
{
    ADS_DL_RcvCcpuResetStartInd();
    return;
}


VOS_UINT32 ADS_DL_ConfigAdq(ipf_ad_type_e adType, VOS_UINT ipfAdNum)
{
    ipf_ad_desc_s    *adDesc   = VOS_NULL_PTR;
    ADS_IPF_AdRecord *adRecord = VOS_NULL_PTR;
    IMM_Zc           *immZc    = VOS_NULL_PTR;
    VOS_UINT32        poolId;
    VOS_UINT32        tmp;
    VOS_UINT32        cnt;
    VOS_UINT32        len;
    VOS_UINT32        slice;
    VOS_INT32         rslt;

    slice = VOS_GetSlice();

    if (IPF_AD_0 == adType) {
        poolId = ADS_IPF_MEM_POOL_ID_AD0;
        len    = ADS_IPF_AD0_MEM_BLK_SIZE;
    } else {
        poolId = ADS_IPF_MEM_POOL_ID_AD1;
        len    = ADS_IPF_AD1_MEM_BLK_SIZE;
    }

    for (cnt = 0; cnt < ipfAdNum; cnt++) {
        immZc = ADS_IPF_AllocMem(poolId, len, IMM_MAC_HEADER_RES_LEN);
        if (VOS_NULL_PTR == immZc) {
            ADS_DBG_DL_ADQ_ALLOC_MEM_FAIL_NUM(1);
            break;
        }

        ADS_DBG_DL_ADQ_ALLOC_MEM_SUCC_NUM(1);

        /* ��дAD������: OUTPUT0 ---> Ŀ�ĵ�ַ; OUTPUT1 ---> SKBUFF */
        adDesc = ADS_DL_GET_IPF_AD_DESC_PTR(adType, cnt);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
#ifndef CONFIG_NEW_PLATFORM
        adDesc->out_ptr0 = (VOS_UINT32)virt_to_phys((VOS_VOID *)immZc->data);
        adDesc->out_ptr1 = (VOS_UINT32)virt_to_phys((VOS_VOID *)immZc);
#else
        adDesc->out_ptr0 = (modem_phy_addr)(VOS_UINT_PTR)virt_to_phys((VOS_VOID *)immZc->data);
        adDesc->out_ptr1 = (modem_phy_addr)(VOS_UINT_PTR)virt_to_phys((VOS_VOID *)immZc);
#endif
#else
#ifndef CONFIG_NEW_PLATFORM
        adDesc->out_ptr0 = (VOS_UINT32)ADS_IPF_GetMemDma(immZc);
        adDesc->out_ptr1 = (VOS_UINT32)virt_to_phys((VOS_VOID *)immZc);
#else
        adDesc->out_ptr0 = (modem_phy_addr)ADS_IPF_GetMemDma(immZc);
        adDesc->out_ptr1 = (modem_phy_addr)(VOS_UINT_PTR)virt_to_phys((VOS_VOID *)immZc);
#endif
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0) */

        /* ��¼��������Ϣ */
        if (adType < IPF_AD_MAX) {
            adRecord = ADS_DL_GET_IPF_AD_RECORD_PTR(adType);

            if (adRecord->pos >= ADS_IPF_AD_BUFF_RECORD_NUM) {
                adRecord->pos = 0;
            }

            adRecord->adBuff[adRecord->pos].slice = slice;
#ifndef CONFIG_NEW_PLATFORM
            adRecord->astAdBuff[adRecord->ulPos].ulPhyAddr = adDesc->out_ptr1;
#else
            adRecord->adBuff[adRecord->pos].phyAddr = (VOS_VOID *)adDesc->out_ptr1;
#endif
            adRecord->adBuff[adRecord->pos].immZc = immZc;
            adRecord->pos++;
        }
    }

    if (0 == cnt) {
        return 0;
    }

    /* ���뵽AD����Ҫ����ADQ */
    rslt = mdrv_ipf_config_dlad(adType, cnt, ADS_DL_GET_IPF_AD_DESC_PTR(adType, 0));
    if (IPF_SUCCESS != rslt) {
        /* ����ʧ�ܣ��ͷ��ڴ� */
        tmp = cnt;
        for (cnt = 0; cnt < tmp; cnt++) {
            adDesc = ADS_DL_GET_IPF_AD_DESC_PTR(adType, cnt);
#ifndef CONFIG_NEW_PLATFORM
            immZc = (IMM_Zc *)phys_to_virt((unsigned long)adDesc->out_ptr1);
#else
            immZc = (IMM_Zc *)phys_to_virt((VOS_UINT_PTR)adDesc->out_ptr1);  //lint !e661
#endif
            IMM_ZcFreeAny(immZc);
            ADS_DBG_DL_ADQ_FREE_MEM_NUM(1);
        }

        ADS_DBG_DL_ADQ_CFG_IPF_FAIL_NUM(1);
        return 0;
    }

    /* ���ʵ�����õ�AD��Ŀ */
    ADS_DBG_DL_ADQ_CFG_AD_NUM(cnt);
    ADS_DBG_DL_ADQ_CFG_IPF_SUCC_NUM(1);
    return cnt;
}


VOS_VOID ADS_DL_AllocMemForAdq(VOS_VOID)
{
    VOS_INT32  rslt;
    VOS_UINT32 ipfAd0Num;
    VOS_UINT32 ipfAd1Num;
    VOS_UINT32 actAd0Num;
    VOS_UINT32 actAd1Num;

    ipfAd0Num = 0;
    ipfAd1Num = 0;
    actAd0Num = 0;
    actAd1Num = 0;

    /* ��ȡ����ADQ�Ŀ��е�AD���� */
    rslt = mdrv_ipf_get_dlad_num(&ipfAd0Num, &ipfAd1Num);
    if (IPF_SUCCESS != rslt) {
        ADS_DBG_DL_ADQ_GET_FREE_AD_FAIL_NUM(1);
        return;
    }

    ADS_DBG_DL_ADQ_GET_FREE_AD_SUCC_NUM(1);

    /* �������ô��ڴ��ADQ1 */
    if (0 != ipfAd1Num) {
        actAd1Num = ADS_DL_ConfigAdq(IPF_AD_1, ipfAd1Num);
        ADS_DBG_DL_ADQ_CFG_AD1_NUM(actAd1Num);
    }

    /* ������С�ڴ��ADQ0 */
    if (0 != ipfAd0Num) {
        actAd0Num = ADS_DL_ConfigAdq(IPF_AD_0, ipfAd0Num);
        ADS_DBG_DL_ADQ_CFG_AD0_NUM(actAd0Num);
    }

    /* AD0Ϊ�ջ���AD1Ϊ����Ҫ����������ʱ�� */
    if (((0 == actAd0Num) && (ADS_IPF_DLAD_START_TMR_THRESHOLD < ipfAd0Num)) ||
        ((0 == actAd1Num) && (ADS_IPF_DLAD_START_TMR_THRESHOLD < ipfAd1Num))) {
        /* �������ADQ�κ�һ���������벻���ڴ棬����ʱ�� */
        ADS_StartTimer(TI_ADS_DL_ADQ_EMPTY, TI_ADS_DL_ADQ_EMPTY_LEN);
    }

    return;
}


VOS_VOID ADS_DL_SendNdClientDataInd(IMM_Zc *immZc)
{
    ADS_NDCLIENT_DataInd *msg  = VOS_NULL_PTR;
    VOS_UINT8            *data = VOS_NULL_PTR;
    VOS_UINT32            dataLen;
    VOS_UINT32            result;

    dataLen = IMM_ZcGetUsedLen(immZc);

    /* ������Ϣ */
    msg = (ADS_NDCLIENT_DataInd *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL,
                                                               sizeof(ADS_NDCLIENT_DataInd) + dataLen - 2);
    if (VOS_NULL_PTR == msg) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendNdClientDataInd: pstMsg is NULL!");
        return;
    }

    /* ��д��Ϣ���� */
    TAF_CfgMsgHdr((MsgBlock *)msg, ACPU_PID_ADS_DL, UEPS_PID_NDCLIENT,
                  sizeof(ADS_NDCLIENT_DataInd) + dataLen - 2 - VOS_MSG_HEAD_LENGTH);
    msg->msgId   = ID_ADS_NDCLIENT_DATA_IND;
    msg->modemId = ADS_DL_GET_MODEMID_FROM_IMM(immZc);
    msg->rabId   = ADS_DL_GET_RABID_FROM_IMM(immZc);
    msg->len     = (VOS_UINT16)dataLen;

    /* ������������ */
    data = IMM_ZcGetDataPtr(immZc);
    if (memcpy_s(msg->data, dataLen, data, dataLen) != EOK) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendNdClientDataInd: memcpy failed!");
    }

    /* ������Ϣ */
    result = PS_SEND_MSG(ACPU_PID_ADS_DL, msg);
    if (VOS_OK != result) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendNdClientDataInd: Send msg fail!");
    }

    return;
}


VOS_VOID ADS_DL_SendDhcpClientDataInd(IMM_Zc *immZc)
{
    ADS_DHCP_DataInd *msg  = VOS_NULL_PTR;
    VOS_UINT8        *data = VOS_NULL_PTR;
    VOS_UINT32        dataLen;
    VOS_UINT32        result;

    dataLen = IMM_ZcGetUsedLen(immZc);

    /* ������Ϣ */
    msg = (ADS_DHCP_DataInd *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL, sizeof(ADS_DHCP_DataInd) + dataLen);

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == msg) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendDhcpClientDataInd: pstMsg is NULL!");
        return;
    }

    /* ��д��Ϣ���� */
    TAF_CfgMsgHdr((MsgBlock *)msg, ACPU_PID_ADS_DL, UEPS_PID_DHCP,
                  sizeof(ADS_DHCP_DataInd) + dataLen - VOS_MSG_HEAD_LENGTH);
    msg->msgId   = ID_ADS_DHCP_DATA_IND;
    msg->modemId = ADS_DL_GET_MODEMID_FROM_IMM(immZc);
    msg->rabId   = ADS_DL_GET_RABID_FROM_IMM(immZc);
    msg->len     = dataLen;

    /* �������� */
    data = IMM_ZcGetDataPtr(immZc);
    if (memcpy_s(msg->data, dataLen, data, dataLen) != EOK) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendDhcpClientDataInd: memcpy fail!");
    }

    /* ����VOS����ԭ�� */
    result = PS_SEND_MSG(ACPU_PID_ADS_DL, msg);
    if (VOS_OK != result) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendDhcpClientDataInd: Send msg fail!");
    }

    return;
}


VOS_VOID ADS_DL_SendMipDataInd(IMM_Zc *immZc)
{
    ADS_MIP_DataInd *msg  = VOS_NULL_PTR;
    VOS_UINT8       *data = VOS_NULL_PTR;
    VOS_UINT32       dataLen;

    dataLen = IMM_ZcGetUsedLen(immZc);

    /* ������Ϣ */
    msg = (ADS_MIP_DataInd *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL, sizeof(ADS_MIP_DataInd) + dataLen - 4);

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == msg) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendMipDataInd: pstMsg is NULL!");
        return;
    }

    /* ��д��Ϣ���� */
    TAF_CfgMsgHdr((MsgBlock *)msg, ACPU_PID_ADS_DL, MSPS_PID_MIP,
                  sizeof(ADS_MIP_DataInd) + dataLen - 4 - VOS_MSG_HEAD_LENGTH);
    msg->msgId   = ID_ADS_MIP_DATA_IND;
    msg->modemId = ADS_DL_GET_MODEMID_FROM_IMM(immZc);
    msg->rabId   = ADS_DL_GET_RABID_FROM_IMM(immZc);
    msg->len     = (VOS_UINT16)dataLen;

    /* �������� */
    data = IMM_ZcGetDataPtr(immZc);
    if (memcpy_s(msg->data, dataLen, data, dataLen) != EOK) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendMipDataInd: memcpy failed!");
    }

    /* ����VOS����ԭ�� */
    (VOS_VOID)PS_SEND_MSG(ACPU_PID_ADS_DL, msg);

    return;
}


VOS_VOID ADS_DL_FreeIpfUsedAd0(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 adNum;

    ipf_ad_desc_s *adDesc = VOS_NULL_PTR;

    adNum = 0;

    adDesc = (ipf_ad_desc_s *)PS_MEM_ALLOC(ACPU_PID_ADS_DL, sizeof(ipf_ad_desc_s) * IPF_DLAD0_DESC_SIZE);

    if (VOS_NULL_PTR == adDesc) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_FreeIpfUsedAd0: pstAdDesc is null");
        return;
    }

    memset_s(adDesc, (sizeof(ipf_ad_desc_s) * IPF_DLAD0_DESC_SIZE), 0x00,
             (sizeof(ipf_ad_desc_s) * IPF_DLAD0_DESC_SIZE));

    if (IPF_SUCCESS == mdrv_ipf_get_used_dlad(IPF_AD_0, (VOS_UINT32 *)&adNum, adDesc)) {
        /* �ͷ�ADQ0���ڴ� */
        for (i = 0; i < PS_MIN(adNum, IPF_DLAD0_DESC_SIZE); i++) {
#ifndef CONFIG_NEW_PLATFORM
            IMM_ZcFreeAny((IMM_Zc *)phys_to_virt((unsigned long)adDesc[i].out_ptr1));
#else
            IMM_ZcFreeAny((IMM_Zc *)phys_to_virt((VOS_UINT_PTR)adDesc[i].out_ptr1));
#endif
        }
    } else {
        ADS_DBG_DL_ADQ_GET_IPF_AD0_FAIL_NUM(1);
    }

    ADS_DBG_DL_ADQ_FREE_AD0_NUM(PS_MIN(adNum, IPF_DLAD0_DESC_SIZE));

    PS_MEM_FREE(ACPU_PID_ADS_DL, adDesc);

    return;
}


VOS_VOID ADS_DL_FreeIpfUsedAd1(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 adNum;

    ipf_ad_desc_s *adDesc = VOS_NULL_PTR;

    adNum = 0;

    adDesc = (ipf_ad_desc_s *)PS_MEM_ALLOC(ACPU_PID_ADS_DL, sizeof(ipf_ad_desc_s) * IPF_DLAD1_DESC_SIZE);

    if (VOS_NULL_PTR == adDesc) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_FreeIpfUsedAd1: pstAdDesc is null");
        return;
    }

    memset_s(adDesc, (sizeof(ipf_ad_desc_s) * IPF_DLAD1_DESC_SIZE), 0x00,
             (sizeof(ipf_ad_desc_s) * IPF_DLAD1_DESC_SIZE));

    if (IPF_SUCCESS == mdrv_ipf_get_used_dlad(IPF_AD_1, (VOS_UINT32 *)&adNum, adDesc)) {
        /* �ͷ�ADQ1���ڴ� */
        for (i = 0; i < PS_MIN(adNum, IPF_DLAD1_DESC_SIZE); i++) {
#ifndef CONFIG_NEW_PLATFORM
            IMM_ZcFreeAny((IMM_Zc *)phys_to_virt((unsigned long)adDesc[i].out_ptr1));
#else
            IMM_ZcFreeAny((IMM_Zc *)phys_to_virt((VOS_UINT_PTR)adDesc[i].out_ptr1));
#endif
        }
    } else {
        ADS_DBG_DL_ADQ_GET_IPF_AD1_FAIL_NUM(1);
    }

    ADS_DBG_DL_ADQ_FREE_AD1_NUM(PS_MIN(adNum, IPF_DLAD1_DESC_SIZE));

    PS_MEM_FREE(ACPU_PID_ADS_DL, adDesc);

    return;
}


VOS_VOID ADS_DL_SndCdsErrorInd(VOS_UINT16 modemId, VOS_UINT32 rabId)
{
    ADS_CDS_ErrInd *msg = VOS_NULL_PTR;

    /* ����OSA��Ϣ */
    msg = (ADS_CDS_ErrInd *)ADS_DL_ALLOC_MSG_WITH_HDR(sizeof(ADS_CDS_ErrInd));
    if (VOS_NULL_PTR == msg) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SndCdsErrorInd: Alloc MSG failed.\n");
        return;
    }

    /* �����Ϣ���� */
    if (memset_s(ADS_DL_GET_MSG_ENTITY(msg), ADS_DL_GET_MSG_LENGTH(msg), 0x00, ADS_DL_GET_MSG_LENGTH(msg)) != EOK) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SndCdsErrorInd: memset fail \n");
    }

    /* ��д��Ϣͷ */
    ADS_DL_CFG_CDS_MSG_HDR(msg, ID_ADS_CDS_ERR_IND);

    /* ��д��Ϣ���� */
    msg->modemId = modemId;
    msg->rabId   = (VOS_UINT8)rabId;

    /* ������Ϣ */
    ADS_DL_SEND_MSG(msg);
    return;
}


VOS_VOID ADS_DL_SndIntraPacketErrorInd(VOS_UINT32 instance, VOS_UINT16 bearerMask)
{
    ADS_PacketErrorInd *msg = VOS_NULL_PTR;

    /* ����OSA��Ϣ */
    msg = (ADS_PacketErrorInd *)ADS_DL_ALLOC_MSG_WITH_HDR(sizeof(ADS_PacketErrorInd));
    if (VOS_NULL_PTR == msg) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SndIntraPacketErrorInd: Alloc MSG failed.\n");
        return;
    }

    /* ��д��Ϣͷ */
    ADS_DL_CFG_INTRA_MSG_HDR(msg, ID_ADS_PACKET_ERROR_IND);

    /* ��д��Ϣ���� */
    msg->modemId    = (VOS_UINT16)instance;
    msg->bearerMask = bearerMask;

    /* ������Ϣ */
    ADS_DL_SEND_MSG(msg);
    return;
}


VOS_VOID ADS_DL_RecordPacketErrorStats(IMM_Zc *immZc)
{
    ADS_PacketErrorFeedbackCfg *feedbackCfg = VOS_NULL_PTR;
    ADS_BearerPacketErrorStats *pktErrStats = VOS_NULL_PTR;
    ADS_DL_ImmPrivCb           *immPriv     = VOS_NULL_PTR;

    feedbackCfg = ADS_DL_GET_PKT_ERR_FEEDBACK_CFG_PTR();
    if (VOS_FALSE == feedbackCfg->enabled) {
        return;
    }

    immPriv = ADS_DL_IMM_PRIV_CB(immZc);
    if (ADS_CDS_IPF_PKT_TYPE_IP == ADS_DL_GET_PKT_TYPE(immPriv->modemId, immPriv->rabId)) {
        pktErrStats = ADS_DL_GET_PKT_ERR_STATS_PTR(immPriv->modemId, immPriv->rabId);
        if (immPriv->u.ipfResult & ADS_DL_IPF_RESULT_PKT_ERR_FEEDBACK_MASK) {
            if (0 == pktErrStats->errorPktNum) {
                pktErrStats->minDetectExpires = ADS_CURRENT_TICK + feedbackCfg->minDetectDuration;
                pktErrStats->maxDetectExpires = ADS_CURRENT_TICK + feedbackCfg->maxDetectDuration;
            }

            pktErrStats->errorPktNum++;
        }

        if (0 != pktErrStats->errorPktNum) {
            pktErrStats->totalPktNum++;
        }
    }
}


VOS_UINT32 ADS_DL_CalcPacketErrorRate(VOS_UINT32 errorPktNum, VOS_UINT32 totalPktNum)
{
    VOS_UINT32 pktErrRate;

    if ((0 == totalPktNum) || (errorPktNum > totalPktNum)) {
        pktErrRate = 100;
    } else if (errorPktNum <= ADS_DL_ERROR_PACKET_NUM_THRESHOLD) {
        pktErrRate = errorPktNum * 100 / totalPktNum;
    } else {
        pktErrRate = errorPktNum / (totalPktNum / 100);
    }

    return pktErrRate;
}


VOS_VOID ADS_DL_FeedBackPacketErrorIfNeeded(VOS_VOID)
{
    ADS_PacketErrorFeedbackCfg *feedbackCfg = VOS_NULL_PTR;
    ADS_BearerPacketErrorStats *pktErrStats = VOS_NULL_PTR;
    VOS_UINT32                  pktErrRate;
    VOS_UINT16                  bearerMask;
    VOS_UINT32                  instance;
    VOS_UINT32                  rabId;

    feedbackCfg = ADS_DL_GET_PKT_ERR_FEEDBACK_CFG_PTR();
    if (VOS_FALSE == feedbackCfg->enabled) {
        return;
    }

    for (instance = 0; instance < ADS_INSTANCE_MAX_NUM; instance++) {
        bearerMask = 0;

        for (rabId = ADS_RAB_ID_MIN; rabId <= ADS_RAB_ID_MAX; rabId++) {
            pktErrStats = ADS_DL_GET_PKT_ERR_STATS_PTR(instance, rabId);
            if (0 != pktErrStats->errorPktNum) {
                if (ADS_TIME_IN_RANGE(ADS_CURRENT_TICK, pktErrStats->minDetectExpires, pktErrStats->maxDetectExpires)) {
                    pktErrRate = ADS_DL_CalcPacketErrorRate(pktErrStats->errorPktNum, pktErrStats->totalPktNum);

                    if (pktErrRate >= feedbackCfg->pktErrRateThres) {
                        bearerMask = ADS_BIT16_SET(bearerMask, rabId);
                    }

                    pktErrStats->errorPktNum = 0;
                    pktErrStats->totalPktNum = 0;
                }

                if (ADS_TIME_AFTER(ADS_CURRENT_TICK, pktErrStats->maxDetectExpires)) {
                    pktErrStats->errorPktNum = 0;
                    pktErrStats->totalPktNum = 0;
                    continue;
                }
            }
        }

        if (0 != bearerMask) {
            ADS_DL_SndIntraPacketErrorInd(instance, bearerMask);
        }
    }

    return;
}


VOS_UINT32 ADS_DL_ValidateImmMem(IMM_Zc *immZc)
{
    ADS_DL_ImmPrivCb *immPriv = VOS_NULL_PTR;

    immPriv = ADS_DL_IMM_PRIV_CB(immZc);

    /* ���MODEMID */
    if (!ADS_IS_MODEMID_VALID(immPriv->modemId)) {
        ADS_DBG_DL_RMNET_MODEMID_ERR_NUM(1);
        return VOS_ERR;
    }

    /* ���RABID */
    if (!ADS_IS_RABID_VALID(immPriv->rabId)) {
        ADS_DBG_DL_RMNET_RABID_ERR_NUM(1);
        return VOS_ERR;
    }

    return VOS_OK;
}

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)

VOS_VOID ADS_DL_FreePc5FragQueue(VOS_VOID)
{
    IMM_Zc *immZc = VOS_NULL_PTR;

    while (IMM_ZcQueueLen(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD()) > 0) {
        immZc = IMM_ZcDequeueTail(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD());
        IMM_ZcFreeAny(immZc);
        ADS_DBG_DL_PC5_FRAG_FREE_NUM(1);
    }

    ADS_DL_CLEAR_PC5_FRAGS_LEN();
    ADS_DL_SET_NEXT_PC5_FRAG_SEQ(0);
}


VOS_UINT32 ADS_DL_InsertPc5FragInQueue(IMM_Zc *immZc)
{
    VOS_UINT32 queLen;
    VOS_UINT8  fragSeq;
    VOS_UINT8  nextSeq;

    queLen  = IMM_ZcQueueLen(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD());
    nextSeq = ADS_DL_GET_NEXT_PC5_FRAG_SEQ();
    fragSeq = ADS_GET_PC5_FRAG_SEQ(immZc);

    if ((queLen >= ADS_DL_GET_PC5_FRAGS_QUEUE_MAX_LEN()) || (fragSeq != nextSeq)) {
        IMM_ZcFreeAny(immZc);
        ADS_DBG_DL_PC5_FRAG_FREE_NUM(1);
        ADS_DL_FreePc5FragQueue();
        return VOS_ERR;
    }

    IMM_ZcQueueTail(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD(), immZc);
    ADS_DL_UPDATE_PC5_FRAGS_LEN(IMM_ZcGetUsedLen(immZc));
    ADS_DL_SET_NEXT_PC5_FRAG_SEQ(fragSeq + 1);
    ADS_DBG_DL_PC5_FRAG_ENQUE_NUM(1);
    return VOS_OK;
}


IMM_Zc* ADS_DL_AssemblePc5Frags(VOS_VOID)
{
    IMM_Zc    *frag    = VOS_NULL_PTR;
    IMM_Zc    *newSkb  = VOS_NULL_PTR;
    VOS_UINT8 *skbData = VOS_NULL_PTR;
    VOS_UINT32 dataBufLen;
    errno_t    retVal;

    newSkb = IMM_ZcStaticAlloc(ADS_DL_GET_PC5_FRAGS_LEN());

    if (newSkb == VOS_NULL_PTR) {
        ADS_DL_FreePc5FragQueue();
        return VOS_NULL_PTR;
    }

    skbData    = IMM_ZcPut(newSkb, ADS_DL_GET_PC5_FRAGS_LEN());
    dataBufLen = IMM_ZcGetUsedLen(newSkb);

    while (IMM_ZcQueueLen(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD()) > 0) {
        frag   = IMM_ZcDequeueHead(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD());
        retVal = memcpy_s(skbData, dataBufLen, IMM_ZcGetDataPtr(frag), IMM_ZcGetUsedLen(frag));

        if (retVal != EOK) {
            IMM_ZcFreeAny(frag);
            ADS_DBG_DL_PC5_FRAG_FREE_NUM(1);
            ADS_DL_FreePc5FragQueue();
            IMM_ZcFreeAny(newSkb);
            ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_AssemblePc5Frags: memcpy fail.");
            return VOS_NULL_PTR;
        }

        skbData += IMM_ZcGetUsedLen(frag);
        dataBufLen -= IMM_ZcGetUsedLen(frag);
        IMM_ZcFreeAny(frag);
        ADS_DBG_DL_PC5_FRAG_RELEASE_NUM(1);
    }

    ADS_DL_CLEAR_PC5_FRAGS_LEN();
    ADS_DL_SET_NEXT_PC5_FRAG_SEQ(0);
    ADS_DBG_DL_PC5_FRAG_ASSEM_SUCC_NUM(1);
    return newSkb;
}


VOS_VOID ADS_DL_ProcPc5Pkt(IMM_Zc *immZc)
{
    VOS_UINT32 queLen;

    queLen = IMM_ZcQueueLen(ADS_DL_GET_PC5_FRAGS_QUEUE_HEAD());
    ADS_DBG_DL_RDQ_RX_PC5_PKT_NUM(1);

    /* �׷�Ƭ */
    if (ADS_PC5_IS_FIRST_FRAG(immZc)) {
        if (queLen != 0) {
            ADS_DL_FreePc5FragQueue();
        }

        ADS_DL_InsertPc5FragInQueue(immZc);
        return;
    }

    /* �м��Ƭ */
    if (ADS_PC5_IS_MID_FRAG(immZc)) {
        if (queLen == 0) {
            IMM_ZcFreeAny(immZc);
            ADS_DBG_DL_PC5_FRAG_FREE_NUM(1);
        } else {
            ADS_DL_InsertPc5FragInQueue(immZc);
        }
        return;
    }

    /* β��Ƭ */
    if (ADS_PC5_IS_LAST_FRAG(immZc)) {
        if (queLen == 0) {
            IMM_ZcFreeAny(immZc);
            ADS_DBG_DL_PC5_FRAG_FREE_NUM(1);
            return;
        }

        if (ADS_DL_InsertPc5FragInQueue(immZc) != VOS_OK) {
            return;
        }

        immZc = ADS_DL_AssemblePc5Frags();
        if (immZc == VOS_NULL_PTR) {
            return;
        }
    }

    /* �������� */
    if (ADS_PC5_IS_NORM_PKT(immZc)) {
        if (queLen != 0) {
            ADS_DL_FreePc5FragQueue();
        }
    }

    ADS_MNTN_REC_DL_PC5_INFO(immZc);
    ADS_DBG_DL_RDQ_RX_PC5_PKT_LEN(IMM_ZcGetUsedLen(immZc));
    (VOS_VOID)mdrv_pcv_xmit(immZc);
    ADS_DBG_DL_RDQ_TX_PC5_PKT_NUM(1);
}
#endif


VOS_VOID ADS_DL_Xmit(IMM_Zc *immZc, VOS_UINT32 instance, VOS_UINT32 rabId)
{
    RCV_DL_DATA_FUNC rcvDlDataFunc = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
    RCV_RD_LAST_DATA_FUNC rcvRdLstDataFunc = VOS_NULL_PTR;
#endif
    IMM_Zc          *lstImmZc = VOS_NULL_PTR;
    VOS_UINT32       exParam;
    VOS_UINT16       ipfResult;
    ADS_PktTypeUint8 ipType;
    VOS_UINT8        exRabId;

    rcvDlDataFunc = ADS_DL_GET_DATA_CALLBACK_FUNC(instance, rabId);
#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
    rcvRdLstDataFunc = ADS_DL_GET_RD_LST_DATA_CALLBACK_FUNC(instance, rabId);
#endif

    /* ��ȡ��������� */
    lstImmZc = ADS_DL_GET_LST_DATA_PTR(instance, rabId);
    if (VOS_NULL_PTR != lstImmZc) {
        ipfResult = ADS_DL_GET_IPF_RESULT_FORM_IMM(lstImmZc);
        ipType    = ADS_DL_GET_IPTYPE_FROM_IPF_RESULT(ipfResult);
        exParam   = ADS_DL_GET_DATA_EXPARAM(instance, rabId);
        exRabId   = ADS_BUILD_EXRABID(instance, rabId);

        rcvDlDataFunc = ADS_DL_GET_DATA_CALLBACK_FUNC(instance, rabId);
        if (VOS_NULL_PTR != rcvDlDataFunc) {
            (VOS_VOID)rcvDlDataFunc(exRabId, lstImmZc, ipType, exParam);

#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
            /* ���һ������ */
            rcvRdLstDataFunc = ADS_DL_GET_RD_LST_DATA_CALLBACK_FUNC(instance, rabId);
            if ((VOS_NULL_PTR == immZc) && (VOS_NULL_PTR != rcvRdLstDataFunc)) {
                rcvRdLstDataFunc(exParam);
            }
#endif
            ADS_DBG_DL_RMNET_TX_PKT_NUM(1);
        } else {
            IMM_ZcFreeAny(lstImmZc);
            ADS_DBG_DL_RMNET_NO_FUNC_FREE_PKT_NUM(1);
        }
    }

    ADS_DL_SET_LST_DATA_PTR(instance, rabId, immZc);

    return;
}


VOS_VOID ADS_DL_ProcTxData(IMM_Zc *immZc)
{
    VOS_UINT32 instance;
    VOS_UINT32 rabId;

    if (VOS_NULL_PTR != immZc) {
        instance = ADS_DL_GET_MODEMID_FROM_IMM(immZc);
        rabId    = ADS_DL_GET_RABID_FROM_IMM(immZc);

        ADS_DL_Xmit(immZc, instance, rabId);
    } else {
        for (instance = 0; instance < ADS_INSTANCE_MAX_NUM; instance++) {
            for (rabId = ADS_RAB_ID_MIN; rabId <= ADS_RAB_ID_MAX; rabId++) {
                ADS_DL_Xmit(VOS_NULL_PTR, instance, rabId);
            }
        }
    }

    return;
}


IMM_Zc* ADS_DL_RdDescTransImmMem(const ipf_rd_desc_s *rdDesc, VOS_UINT32 slice)
{
    ADS_IPF_RdRecord *rdRecord = VOS_NULL_PTR;
    IMM_Zc           *immZc    = VOS_NULL_PTR;
    VOS_UINT32        cacheLen;

    /* ��OUTָ��ת��ΪIMM�ڴ�ָ�� */
    immZc = (IMM_Zc *)phys_to_virt((VOS_UINT_PTR)ADS_DL_GET_IPF_RD_DESC_OUT_PTR(rdDesc));
    if (VOS_NULL_PTR == immZc) {
        return VOS_NULL_PTR;
    }

    /* ��¼��������Ϣ */
    rdRecord = ADS_DL_GET_IPF_RD_RECORD_PTR();
    if (rdRecord->pos >= ADS_IPF_RD_BUFF_RECORD_NUM) {
        rdRecord->pos = 0;
    }

    rdRecord->rdBuff[rdRecord->pos].slice  = slice;
    rdRecord->rdBuff[rdRecord->pos].attr   = rdDesc->attribute;
    rdRecord->rdBuff[rdRecord->pos].pktLen = rdDesc->pkt_len;
    rdRecord->rdBuff[rdRecord->pos].immZc  = immZc;
    rdRecord->rdBuff[rdRecord->pos].outPtr = (VOS_VOID *)ADS_DL_GET_IPF_RD_DESC_OUT_PTR(rdDesc);
    rdRecord->pos++;

    /* ͳһˢCACHE */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
    cacheLen = rdDesc->pkt_len + IMM_MAC_HEADER_RES_LEN;
#else
    cacheLen = rdDesc->pkt_len;
#endif
    ADS_IPF_DL_MEM_UNMAP(immZc, cacheLen);

    /* ����������ʵ���� */
    IMM_ZcPut(immZc, rdDesc->pkt_len);

    /* ������������Ϣ: u16Result/u16UsrField1 */
    ADS_DL_SAVE_RD_DESC_TO_IMM(immZc, rdDesc);

    return immZc;
}


VOS_VOID ADS_DL_ProcIpfFilterData(IMM_Zc *immZc)
{
    VOS_UINT32 cacheLen;
    VOS_UINT16 ipfResult;

    /* ͳһˢCACHE */
    ipfResult = ADS_DL_GET_IPF_RESULT_FORM_IMM(immZc);
    cacheLen  = IMM_ZcGetUsedLen(immZc) + IMM_MAC_HEADER_RES_LEN;

    ADS_IPF_SPE_MEM_UNMAP(immZc, cacheLen);

    /*
     * ƥ�����й��˹��������
     * BearId 19: NDClient������Ҫת����NDClient
     * BearId 17: DHCPv6������Ҫת����DHCP
     * [0, 15]����Ϊ�Ƿ����ݰ�;
     * [16, 18, 20, 21]Ŀǰֱ���ͷ�
     */
    if (CDS_ADS_DL_IPF_BEARER_ID_ICMPV6 == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(ipfResult)) {
        ADS_DL_SendNdClientDataInd(immZc);
        ADS_DBG_DL_RDQ_RX_ND_PKT_NUM(1);
    } else if (CDS_ADS_DL_IPF_BEARER_ID_DHCPV6 == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(ipfResult)) {
        ADS_DL_SendDhcpClientDataInd(immZc);
        ADS_DBG_DL_RDQ_RX_DHCP_PKT_NUM(1);
    } else if (CDS_ADS_DL_IPF_BEARER_ID_MIP_ADV == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(ipfResult)) {
        ADS_DL_SendMipDataInd(immZc);
        ADS_DBG_DL_RDQ_RX_ND_PKT_NUM(1);
    } else if (CDS_ADS_DL_IPF_BEARER_ID_MIP_REG_REPLY == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(ipfResult)) {
        ADS_DL_SendMipDataInd(immZc);
        ADS_DBG_DL_RDQ_RX_ND_PKT_NUM(1);
    } else {
        ADS_DBG_DL_RDQ_FILTER_ERR_PKT_NUM(1);
    }

    ADS_IPF_SPE_MEM_MAP(immZc, cacheLen);
    IMM_ZcFreeAny(immZc);

    return;
}


VOS_VOID ADS_DL_ProcIpfFilterQue(VOS_VOID)
{
    IMM_Zc *immZc = VOS_NULL_PTR;

    for (;;) {
        immZc = IMM_ZcDequeueHead(ADS_GET_IPF_FILTER_QUE());
        if (VOS_NULL_PTR == immZc) {
            break;
        }

        ADS_DL_ProcIpfFilterData(immZc);
    }

    return;
}


VOS_VOID ADS_DL_ProcIpfResult(VOS_VOID)
{
    ADS_DL_IpfResult *ipfResult = VOS_NULL_PTR;
    ipf_rd_desc_s    *rdDesc    = VOS_NULL_PTR;
    IMM_Zc           *immZc     = VOS_NULL_PTR;
    VOS_UINT32        rdNum     = IPF_DLRD_DESC_SIZE;
    VOS_UINT32        txTimeout = 0;
    VOS_UINT32        cnt;
    VOS_ULONG         lockLevel;
    VOS_UINT32        rnicNapiPollQueLen;
    VOS_UINT32        slice;

    /*
     * IPF_RD_DESC_S��u16Result����
     * [15]pkt_len_err     IP�����������÷�Χ��
     * [14]bd_cd_noeq      BD��len��CD�ĳ��Ȳ��ȴ�����ʾ
     * [13]pkt_parse_err   ���ݽ�������ָʾ
     * [12]bd_pkt_noeq     BD��len��IP��ͷָʾ��len���ȴ���ָʾ
     * [11]head_len_err    IPV4���ȴ���ָʾ�źţ�IPV6����鳤��
     * [10]version_err     �汾�Ŵ���ָʾ
     * [9]ip_type          IP�����ͣ�
     *                     0 ��ʾIPV4
     *                     1 ��ʾIPV6
     * [8]ff_type          ��Ƭ����һ����Ƭ�Ƿ�����ϲ�ͷָʾ
     *                     0 ��ʾ��Ƭ����һ����Ƭ�����ϲ�ͷ(IP��δ��ƬʱҲΪ0)
     *                     1 ��ʾ��Ƭ����һ����Ƭ�����ϲ�ͷ
     * [7:6]pf_type        IP����Ƭָʾ����
     *                     0 ��ʾIP��δ��Ƭ
     *                     1 ��ʾIP����Ƭ����Ϊ��һ����Ƭ��
     *                     2 ��ʾ��Ƭ����Ϊ���һ����Ƭ��
     *                     3 ��ʾ��Ƭ����Ϊ�м��Ƭ
     * [0:5]bear_id        ���غţ����Ϊ0x3F�������й�������ƥ��
     */

    /* ��ȡRD DESC */
    rdDesc = ADS_DL_GET_IPF_RD_DESC_PTR(0);
    mdrv_ipf_get_dlrd(&rdNum, rdDesc);

    /* ��ȡ��RDΪ0 */
    if (0 == rdNum) {
        ADS_DBG_DL_RDQ_GET_RD0_NUM(1);
        return;
    }

#if (FEATURE_OFF == FEATURE_LTE)
    mdrv_wdt_feed(0);
#endif

    /* ����RDͳ�Ƹ��� */
    ADS_DBG_DL_RDQ_RX_RD_NUM(rdNum);

    /* ������AD���ٴ���RD */
    /*lint -e571*/
    VOS_SpinLockIntLock(&(g_adsCtx.adsIpfCtx.adqSpinLock), lockLevel);
    /*lint +e571*/
    ADS_DL_AllocMemForAdq();
    VOS_SpinUnlockIntUnlock(&(g_adsCtx.adsIpfCtx.adqSpinLock), lockLevel);

    slice = VOS_GetSlice();

    for (cnt = 0; cnt < rdNum; cnt++) {
        rdDesc = ADS_DL_GET_IPF_RD_DESC_PTR(cnt);

        /* ת��ΪIMM�ڴ� */
        immZc = ADS_DL_RdDescTransImmMem(rdDesc, slice);
        if (VOS_NULL_PTR == immZc) {
            ADS_DBG_DL_RDQ_TRANS_MEM_FAIL_NUM(1);
            continue;
        }

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
        /* ��ȡ����PC5 IPF�������� */
        if (IPF_LTEV_DLFC == rdDesc->fc_head) {
            ADS_DL_ProcPc5Pkt(immZc);
        } else
#endif
        {
            /* У��IMM�ڴ� */
            if (VOS_OK != ADS_DL_ValidateImmMem(immZc)) {
                IMM_ZcFreeAny(immZc);
                continue;
            }

            /* ͳ���쳣���� */
            ADS_DL_RecordPacketErrorStats(immZc);

            /* ͳ�������������յ��������ֽ���������������ѯ */
            ADS_RECV_DL_PERIOD_PKT_NUM(rdDesc->pkt_len);

            /* ��ʵ�� */
            rnicNapiPollQueLen = 0;

            ADS_MNTN_REC_DL_IP_INFO(immZc, ADS_DL_GET_IPF_RSLT_USR_FIELD1_FROM_IMM(immZc), rdDesc->usr_field2,
                                    rdDesc->usr_field3, rnicNapiPollQueLen);

            /* ��ȡIPF RESULT */
            ipfResult = (ADS_DL_IpfResult *)&(rdDesc->result);

            /* BearId 0x3F: �����������ݰ���Ҫת����NDIS/PPP/RNIC */
            if (CDS_ADS_DL_IPF_BEARER_ID_INVALID == ipfResult->bearedId) {
                if (rdDesc->result & ADS_DL_IPF_RESULT_PKT_ERR_MASK) {
                    ADS_DBG_DL_RDQ_RX_ERR_PKT_NUM(1);
                }

                txTimeout = ADS_DL_TX_WAKE_LOCK_TMR_LEN;

                if (VOS_TRUE == g_adsDlDiscardPktFlag) {
                    IMM_ZcFreeAny(immZc);
                    continue;
                }

                ADS_DL_ProcTxData(immZc);
                ADS_DBG_DL_RDQ_RX_NORM_PKT_NUM(1);
            } else {
                if (VOS_FALSE != VOS_CheckInterrupt()) {
                    /* ������л��� */
                    IMM_ZcQueueTail(ADS_GET_IPF_FILTER_QUE(), immZc);
                } else {
                    ADS_DL_ProcIpfFilterData(immZc);
                }
            }
        }
    }

    /* �������һ������ */
    ADS_DL_ProcTxData(VOS_NULL_PTR);

    if (0 != IMM_ZcQueueLen(ADS_GET_IPF_FILTER_QUE())) {
        ADS_DL_SndEvent(ADS_DL_EVENT_IPF_FILTER_DATA_PROC);
    }

    /* ����������ʾ��Э��ջ */
    ADS_DL_FeedBackPacketErrorIfNeeded();

    ADS_MNTN_RPT_ALL_DL_PKT_INFO();

    ADS_DL_EnableTxWakeLockTimeout(txTimeout);
    return;
}


VOS_UINT32 ADS_DL_IsFcAssemTuneNeeded(VOS_UINT32 rdNum)
{
    ADS_DL_FcAssem *fcAssemInfo;
    VOS_UINT32      rslt = VOS_FALSE;

    /* ��ȡ������ֵ���� */
    fcAssemInfo = ADS_DL_GET_FC_ASSEM_INFO_PTR(ADS_INSTANCE_INDEX_0);

    if (0 != fcAssemInfo->enableMask) {
        fcAssemInfo->rdCnt += rdNum;

        if (ADS_TIME_AFTER_EQ(ADS_CURRENT_TICK, (fcAssemInfo->tmrCnt + fcAssemInfo->expireTmrLen))) {
            if (VOS_FALSE == fcAssemInfo->fcActiveFlg) {
                if (fcAssemInfo->rdCnt >= fcAssemInfo->rateUpLev) {
                    ADS_DBG_DL_FC_ACTIVATE_NUM(1);
                    fcAssemInfo->fcActiveFlg = VOS_TRUE;
                    rslt                     = VOS_TRUE;
                }
            } else {
                if (fcAssemInfo->rdCnt <= fcAssemInfo->rateDownLev) {
                    fcAssemInfo->fcActiveFlg = VOS_FALSE;
                }
            }

            ADS_DBG_DL_FC_TMR_TIMEOUT_NUM(1);
            fcAssemInfo->tmrCnt = ADS_CURRENT_TICK;
            fcAssemInfo->rdCnt  = 0;
        }
    }

    return rslt;
}


VOS_UINT32 ADS_DL_RegDlDataCallback(VOS_UINT8 exRabId, RCV_DL_DATA_FUNC func, VOS_UINT32 exParam)
{
    ADS_DL_RabInfo *dlRabInfo = VOS_NULL_PTR;
    VOS_UINT32      instance;
    VOS_UINT32      rabId;

    /* ���MODEMID */
    instance = ADS_GET_MODEMID_FROM_EXRABID(exRabId);
    if (!ADS_IS_MODEMID_VALID(instance)) {
        ADS_ERROR_LOG1(ACPU_PID_ADS_DL, "ADS_DL_RegDlDataCallback: ModemId is invalid! <ModemId>", instance);
        return VOS_ERR;
    }

    /* ���RABID */
    rabId = ADS_GET_RABID_FROM_EXRABID(exRabId);
    if (!ADS_IS_RABID_VALID(rabId)) {
        ADS_ERROR_LOG2(ACPU_PID_ADS_DL, "ADS_DL_RegDlDataCallback: RabId is invalid! <ModemId>,<RabId>", instance,
                       rabId);
        return VOS_ERR;
    }

    /* �����������ݻص����� */
    dlRabInfo                = ADS_DL_GET_RAB_INFO_PTR(instance, rabId);
    dlRabInfo->rabId         = rabId;
    dlRabInfo->exParam       = exParam;
    dlRabInfo->rcvDlDataFunc = func;

    return VOS_OK;
}


VOS_UINT32 ADS_DL_RegFilterDataCallback(VOS_UINT8 rabId, ADS_FilterIpAddrInfo *filterIpAddr, RCV_DL_DATA_FUNC func)
{
    ADS_DL_RabInfo *dlRabInfo = VOS_NULL_PTR;
    VOS_UINT32      instanceIndex;
    VOS_UINT32      realRabId;

    /* ucRabId�ĸ�2��bit��ʾmodem id */
    instanceIndex = ADS_GET_MODEMID_FROM_EXRABID(rabId);
    realRabId     = ADS_GET_RABID_FROM_EXRABID(rabId);

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(realRabId)) {
        ADS_WARNING_LOG1(ACPU_PID_ADS_DL, "ADS_DL_RegFilterDataCallback: ucRabId is", realRabId);
        return VOS_ERR;
    }

    dlRabInfo = ADS_DL_GET_RAB_INFO_PTR(instanceIndex, realRabId);

    /* ����ADS�������ݹ��˻ص����� */
    dlRabInfo->rabId               = realRabId;
    dlRabInfo->rcvDlFilterDataFunc = func;

    /* ������˵�ַ��Ϣ */
    ADS_FilterSaveIPAddrInfo(filterIpAddr);

    return VOS_OK;
}


VOS_UINT32 ADS_DL_DeregFilterDataCallback(VOS_UINT32 rabId)
{
    ADS_DL_RabInfo *dlRabInfo = VOS_NULL_PTR;
    VOS_UINT32      instanceIndex;
    VOS_UINT32      realRabId;

    /* ucRabId�ĸ�2��bit��ʾmodem id */
    instanceIndex = ADS_GET_MODEMID_FROM_EXRABID(rabId);
    realRabId     = ADS_GET_RABID_FROM_EXRABID(rabId);

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(realRabId)) {
        ADS_WARNING_LOG1(ACPU_PID_ADS_DL, "ADS_DL_DeregFilterDataCallback: ulRabId is", realRabId);
        return VOS_ERR;
    }

    dlRabInfo = ADS_DL_GET_RAB_INFO_PTR(instanceIndex, realRabId);

    /* ȥע��ADS�������ݹ��˻ص����� */
    dlRabInfo->rcvDlFilterDataFunc = VOS_NULL_PTR;

    /* ���������Ϣ */
    ADS_FilterReset();

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)

VOS_UINT32 ADS_DL_RegNapiCallback(VOS_UINT8 exRabId, RCV_RD_LAST_DATA_FUNC lastDataFunc, VOS_UINT32 exParam)
{
    ADS_DL_RabInfo *dlRabInfo = VOS_NULL_PTR;
    VOS_UINT32      instance;
    VOS_UINT32      rabId;

    /* ���MODEMID */
    instance = ADS_GET_MODEMID_FROM_EXRABID(exRabId);
    if (!ADS_IS_MODEMID_VALID(instance)) {
        ADS_ERROR_LOG1(ACPU_PID_ADS_DL, "ADS_DL_RegRdLastDataCallback: ModemId is invalid! <ModemId>", instance);
        return VOS_ERR;
    }

    /* ���RABID */
    rabId = ADS_GET_RABID_FROM_EXRABID(exRabId);
    if (!ADS_IS_RABID_VALID(rabId)) {
        ADS_ERROR_LOG2(ACPU_PID_ADS_DL, "ADS_DL_RegRdLastDataCallback: RabId is invalid! <ModemId>,<RabId>", instance,
                       rabId);
        return VOS_ERR;
    }

    /* �����������ݻص����� */
    dlRabInfo                   = ADS_DL_GET_RAB_INFO_PTR(instance, rabId);
    dlRabInfo->rabId            = rabId;
    dlRabInfo->exParam          = exParam;
    dlRabInfo->rcvRdLstDataFunc = lastDataFunc;

    return VOS_OK;
}
#endif


VOS_UINT32 ADS_DL_RcvTafPdpStatusInd(MsgBlock *msg)
{
    ADS_PdpStatusInd *pdpStatusInd = VOS_NULL_PTR;
    ADS_DL_RabInfo         *dlRabInfo = VOS_NULL_PTR;
    VOS_UINT32              instanceIndex;
    ADS_CDS_IpfPktTypeUint8 pktType;

    pdpStatusInd = (ADS_PdpStatusInd *)msg;

    pktType       = ADS_CDS_IPF_PKT_TYPE_IP;
    instanceIndex = pdpStatusInd->modemId;

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(pdpStatusInd->rabId)) {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_RcvTafPdpStatusInd: Rab Id is invalid");
        return VOS_ERR;
    }

    if (ADS_PDP_PPP == pdpStatusInd->pdpType) {
        pktType = ADS_CDS_IPF_PKT_TYPE_PPP;
    }

    dlRabInfo = ADS_DL_GET_RAB_INFO_PTR(instanceIndex, pdpStatusInd->rabId);

    /* PDP���� */
    if (ADS_PDP_STATUS_ACT == pdpStatusInd->pdpStatus) {
        /* ����ADS�������ݻص���RABID */
        dlRabInfo->rabId   = pdpStatusInd->rabId;
        dlRabInfo->pktType = pktType;
    }
    /* PDPȥ����  */
    else if (ADS_PDP_STATUS_DEACT == pdpStatusInd->pdpStatus) {
        /* ���ADS�������ݻص����� */
        dlRabInfo->rabId   = ADS_RAB_ID_INVALID;
        dlRabInfo->pktType = ADS_CDS_IPF_PKT_TYPE_IP;
        dlRabInfo->exParam = 0;

        if (ADS_CLEAN_RCV_CB_FUNC_TURE == pdpStatusInd->cleanRcvCbFlag) {
            dlRabInfo->rcvDlDataFunc = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
            dlRabInfo->rcvRdLstDataFunc = VOS_NULL_PTR;
#endif
        } else {
            ADS_NORMAL_LOG(ACPU_PID_ADS_DL, "ADS_DL_RcvTafPdpStatusInd: Not clean ADS DL data call back func");
        }
    } else {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_RcvTafPdpStatusInd: Pdp Status is invalid");
    }

    return VOS_OK;
}


VOS_UINT32 ADS_DL_RcvCcpuResetStartInd(VOS_VOID)
{
    VOS_UINT32 indexNum;

    ADS_TRACE_HIGH("proc reset msg: enter.\n");

    /* ֹͣ���������Ķ�ʱ�� */
    for (indexNum = 0; indexNum < ADS_MAX_TIMER_NUM; indexNum++) {
        ADS_StopTimer(ACPU_PID_ADS_DL, indexNum, ADS_TIMER_STOP_CAUSE_USER);
    }

    /* �������������� */
    ADS_ResetDlCtx();

    /* �ͷ�IPF��AD */
    ADS_DL_FreeIpfUsedAd1();
    ADS_DL_FreeIpfUsedAd0();

    for (indexNum = 0; indexNum < ADS_DL_ADQ_MAX_NUM; indexNum++) {
        memset_s(ADS_DL_GET_IPF_AD_RECORD_PTR(indexNum), sizeof(ADS_IPF_AdRecord), 0x00, sizeof(ADS_IPF_AdRecord));
    }

    /* �ͷ��ź�����ʹ�õ���API����������� */
    VOS_SmV(ADS_GetDLResetSem());

    ADS_TRACE_HIGH("proc reset msg: leave.\n");
    return VOS_OK;
}


VOS_UINT32 ADS_DL_RcvCcpuResetEndInd(MsgBlock *msg)
{
    VOS_ULONG lockLevel;
    ADS_TRACE_HIGH("proc reset msg: enter.\n");

    memset_s(ADS_DL_GET_IPF_RD_RECORD_PTR(), sizeof(ADS_IPF_RdRecord), 0x00, sizeof(ADS_IPF_RdRecord));

    /* ��λIPF */
    mdrv_ipf_reinit_dlreg();

    /* ���³�ʼ��ADQ */
    /*lint -e571*/
    VOS_SpinLockIntLock(&(g_adsCtx.adsIpfCtx.adqSpinLock), lockLevel);
    /*lint +e571*/
    ADS_DL_AllocMemForAdq();
    VOS_SpinUnlockIntUnlock(&(g_adsCtx.adsIpfCtx.adqSpinLock), lockLevel);

    ADS_TRACE_HIGH("proc reset msg: leave.\n");
    return VOS_OK;
}


VOS_UINT32 ADS_DL_RcvTafMsg(MsgBlock *msg)
{
    MSG_Header *msgTmp;

    msgTmp = (MSG_Header *)msg;

    switch (msgTmp->msgName) {
        case ID_APS_ADS_PDP_STATUS_IND:
            ADS_DL_RcvTafPdpStatusInd(msg);
            break;

        default:
            break;
    }

    return VOS_OK;
}


VOS_UINT32 ADS_DL_RcvCdsMsg(MsgBlock *msg)
{
    /* ��ʱû��Ҫ�������Ϣ������յ���Ϣ�����д��� */
    return VOS_ERR;
}


VOS_VOID ADS_DL_ProcIntraPacketErrorInd(MsgBlock *msg)
{
    ADS_PacketErrorInd *msgTemp = VOS_NULL_PTR;
    VOS_UINT32          rabId;

    msgTemp = (ADS_PacketErrorInd *)msg;

    for (rabId = ADS_RAB_ID_MIN; rabId <= ADS_RAB_ID_MAX; rabId++) {
        if (ADS_BIT16_IS_SET(msgTemp->bearerMask, rabId)) {
            ADS_DL_SndCdsErrorInd(msgTemp->modemId, rabId);
        }
    }

    return;
}


VOS_UINT32 ADS_DL_RcvAdsDlMsg(MsgBlock *msg)
{
    MSG_Header *msgTemp;

    msgTemp = (MSG_Header *)msg;

    switch (msgTemp->msgName) {
        case ID_ADS_CCPU_RESET_START_IND:
            /* �ȴ�IPF���������ݰ�գ�ADS_DL�ٸ�λ���ȴ�ʱ��1�� */
            ADS_StartTimer(TI_ADS_DL_CCORE_RESET_DELAY, ADS_DL_GET_CCORE_RESET_DELAY_TIMER_LEN());
            break;

        case ID_ADS_CCPU_RESET_END_IND:
            ADS_DL_RcvCcpuResetEndInd(msg);
            break;

        case ID_ADS_PACKET_ERROR_IND:
            ADS_DL_ProcIntraPacketErrorInd(msg);
            break;

        default:
            ADS_NORMAL_LOG1(ACPU_PID_ADS_DL, "ADS_DL_RcvAdsDlMsg: rcv error msg id %d\r\n", msgTemp->msgName);
            break;
    }

    return VOS_OK;
}


VOS_VOID ADS_DL_ProcMsg(MsgBlock *msg)
{
    if (VOS_NULL_PTR == msg) {
        return;
    }

    /* ��Ϣ�ķַ����� */
    switch (VOS_GET_SENDER_ID(msg)) {
        /* ����APS����Ϣ */
        case I0_WUEPS_PID_TAF:
        case I1_WUEPS_PID_TAF:
        case I2_WUEPS_PID_TAF:
            ADS_DL_RcvTafMsg(msg);
            return;

        /* ����CDS����Ϣ */
        case UEPS_PID_CDS:
            ADS_DL_RcvCdsMsg(msg);
            return;

        /* ����ADS DL����Ϣ */
        case ACPU_PID_ADS_DL:
            ADS_DL_RcvAdsDlMsg(msg);
            return;

        default:
            return;
    }
}

