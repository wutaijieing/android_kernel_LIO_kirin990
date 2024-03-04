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


#ifndef __SI_PIH_H__
#define __SI_PIH_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "product_config.h"
#include "vos.h"
#include "si_app_pih.h"
#include "usimm_ps_interface.h"

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "usimm_vsim_auth.h"
#include "om_list.h"
#endif /* (OSA_CPU_CCPU == VOS_OSA_CPU) */

#ifndef MODEM_FUSION_VERSION
#include "mdrv.h"
#include "omerrorlog.h"
#include "msp_diag_comm.h"
#else
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "mdrv_chr.h"
#endif
#include "mdrv_diag.h"
#endif

#include "mdrv_nvim.h"
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "si_acore_api.h"
#endif

#pragma pack(4)


#define SI_PIH_PIN_CODE_LEN (USIMM_PINNUMBER_LEN)

#define SI_PIH_APDU_HDR_LEN (USIMM_APDU_HEADLEN)

#define SI_PIH_ATFILE_NAME_MAX (40)

#define SI_PIH_FILE_START_INDEX (1)

#define SI_PIH_BCPID_REG_MAX (20)

#define SI_PIH_VSIM_AES_KEY_LEN (32)

#define SI_PIH_VSIM_MAX_CALL_BACK_KEY_LEN (4 * 1024)

#define SI_PIH_SEC_ICC_VSIM_VER (10000)

#ifdef PAM_ENG_ENABLED
#define PIH_USIMM_SCICHG_DEBUG_TIMER_LENTH (10000)
#define PIH_USIMM_SCICHG_DEBUG_TIMER_NAME (0xFF)
#endif

#define SI_PIH_POLL_TIMER_START(pTimer, pid, length, Name) \
    VOS_StartRelTimer(pTimer, pid, length, Name, 0, VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION)

#define SI_PIH_POLL_32K_TIMER_START(pTimer, pid, length, Name) VOS_StartDrxTimer(pTimer, pid, length, Name, 0)

#define SI_PIH_POLL_32K_TIMER_STOP(pTimer) VOS_StopDrxTimer(pTimer)

#define SI_PIH_POLL_REL_32K_TIMER_START(pTimer, pid, length, Name) \
    VOS_StartRelTimer(pTimer, pid, length, Name, 0, VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_0)

#define SI_PIH_POLL_REL_32K_TIMER_STOP(pTimer) VOS_StopRelTimer(pTimer)

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (VOS_OS_VER == VOS_WIN32))
#define PIH_GEN_LOG_MODULE(Level) (MDRV_DIAG_GEN_LOG_MODULE(MODEM_ID_0, DIAG_MODE_COMM, Level))

#define PIH_KEY_INFO_LOG(string)            (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define PIH_KEY_NORMAL_LOG(string)          (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define PIH_KEY_WARNING_LOG(string)         (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)
#define PIH_KEY_ERROR_LOG(string)           (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s", string)

#define PIH_KEY_INFO1_LOG(string, para1)    (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define PIH_KEY_NORMAL1_LOG(string, para1)  (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define PIH_KEY_WARNING1_LOG(string, para1) (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "WARNING%s,%d", string, para1)
#define PIH_KEY_ERROR1_LOG(string, para1)   (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s,%d", string, para1)

#define PIH_INFO_LOG(string)            (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define PIH_NORMAL_LOG(string)          (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define PIH_WARNING_LOG(string)         (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)
#define PIH_ERROR_LOG(string)           (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s", string)

#define PIH_INFO1_LOG(string, para1)    (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define PIH_NORMAL1_LOG(string, para1)  (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define PIH_WARNING1_LOG(string, para1) (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "WARNING%s,%d", string, para1)
#define PIH_ERROR1_LOG(string, para1)   (VOS_VOID)mdrv_diag_log_report(PIH_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s,%d", string, para1)

#elif (OSA_CPU_CCPU == VOS_OSA_CPU)

#define PIH_KEY_INFO_LOG(string)            USIMM_LogPrint(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Info:%s", string)
#define PIH_KEY_NORMAL_LOG(string)          USIMM_LogPrint(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Normal:%s", string)
#define PIH_KEY_WARNING_LOG(string)         USIMM_LogPrint(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Warning:%s", string)
#define PIH_KEY_ERROR_LOG(string)           USIMM_LogPrint(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Error:%s", string)

#define PIH_KEY_INFO1_LOG(string, para1)    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Info:%s,%d", string, para1)
#define PIH_KEY_NORMAL1_LOG(string, para1)  USIMM_LogPrint1(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Normal:%s,%d", string, para1)
#define PIH_KEY_WARNING1_LOG(string, para1) USIMM_LogPrint1(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Warning:%s,%d", string, para1)
#define PIH_KEY_ERROR1_LOG(string, para1)   USIMM_LogPrint1(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "Error:%s,%d", string, para1)

#define PIH_INFO_LOG(string)                USIMM_LogPrint(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)
#define PIH_NORMAL_LOG(string)              USIMM_LogPrint(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)
#define PIH_WARNING_LOG(string)             USIMM_LogPrint(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)
#define PIH_ERROR_LOG(string)               USIMM_LogPrint(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)

#define PIH_INFO1_LOG(string, para1)        USIMM_LogPrint1(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#define PIH_NORMAL1_LOG(string, para1)      USIMM_LogPrint1(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#define PIH_WARNING1_LOG(string, para1)     USIMM_LogPrint1(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#define PIH_ERROR1_LOG(string, para1)       USIMM_LogPrint1(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_PIH_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)

#endif

#define PIH_BIT_N(num) ((VOS_UINT32)0x01 << (num))

#define SI_PIH_MNC_TWO_BYTES_LEN (2)   /* MNC����Ϊ2 */
#define SI_PIH_MNC_THREE_BYTES_LEN (3) /* MNC����Ϊ3 */
#define SI_PIH_AD_MNC_LEN_POS (3)      /* AD�ļ���MNC�����ֶ�����λ�� */

#define SI_PIH_IMSI_MAX_LEN (8) /* IMSI��ָʾ���ȵ����ֵ */

#define PIH_SET_BIT(Data, BitNo) (Data |= (VOS_UINT8)(0x1 << BitNo))

#define BIT_ACTIVECARD (3) /* ��ǰ�Ƿ񼤻���� */
#define BIT_HANDLEVSIM (2) /* ��ǰ�Ƿ���VSIM�� */
#define BIT_VSIMSTATE (1)  /* ��ǰVSIM���Ƿ񼤻� */
#define BIT_CURCARDOK (0)  /* ��ǰ���Ƿ���λ */

#define SI_PIH_DH_PARAP_MAX (128) /* DH ���� P����󳤶� */
#define SI_PIH_DH_PARAG_MAX (1)   /* DH ���� G����󳤶� */

#define SI_PIH_SMEM_ENDFLAG (0x5A5A5A5A)

#define SI_PIH_HUK_LEN (0x10)
#define SI_PIH_HUK_BITS (128)

#define SI_PIH_APNSET_SMEM_ADDR (g_ulTEEShareAddr)
#define SI_PIH_APNSET_SMEM_LEN (0x400) /* Ԥ��1K */

#define SI_PIH_DHPARASET_SMEM_ADDR (SI_PIH_APNSET_SMEM_ADDR + SI_PIH_APNSET_SMEM_LEN)
#define SI_PIH_DHPARASET_SMEM_LEN (0x400) /* Ԥ��1K */

#define SI_PIH_VSIM_SMEM_ADDR (SI_PIH_DHPARASET_SMEM_ADDR + SI_PIH_DHPARASET_SMEM_LEN)
#define SI_PIH_VSIM_SMEM_LEN (0x1000) /* Ԥ��4K */

#define SI_PIH_GETCARDSTATUS_MAX (200)

#if (VOS_OS_VER != VOS_WIN32) /* PC Stub */
#define SI_PIH_TASKDELAY_TIMER_LEN (20)
#else
#define SI_PIH_TASKDELAY_TIMER_LEN (1)
#endif /* (VOS_OS_VER != VOS_WIN32) */

#define SI_PIH_TASKDELAY_SEM_LEN (1000)

#if (FEATURE_ON == FEATURE_VCOM_EXT)
#define SI_PIH_CTRL_INFO_SIZE (5) /* AT���ͨ��������4����+1 */
#else
#define SI_PIH_CTRL_INFO_SIZE (2)
#endif /* (FEATURE_ON == FEATURE_VCOM_EXT) */

/* �����������ͨ���Ż��SessionID, 0x1fֵ����USIMM_CHANNEL_NUMBER_MAX���趨 */
#define SI_PIH_MAKESESSION_ID(RandNum, ChannelID) ((RandNum & (~0x1f)) | ChannelID)

#define SI_PIH_SCI_ERR_COUNT_MAX_SLICE (30 * 60 * 32768) /* Err Logͳ���ϱ����ʱ�� */

enum SI_PIH_Req {
    SI_PIH_NULL_REQ        = 0,
    SI_PIH_FDN_ENABLE_REQ  = 1,
    SI_PIH_FDN_DISALBE_REQ = 2,
    SI_PIH_GACCESS_REQ     = 3,
    SI_PIH_BDN_QUERY_REQ   = 4,
    SI_PIH_FDN_QUERY_REQ   = 5,
    SI_PIH_ISDB_ACCESS_REQ = 7,
    SI_PIH_HVSST_QUERY_REQ  = 8,
    SI_PIH_HVSST_SET_REQ    = 9,
    SI_PIH_HVSDH_SET_REQ    = 10,
    SI_PIH_HVSDH_QRY_REQ    = 11,
    SI_PIH_HVSCONT_QRY_REQ  = 12,
    SI_PIH_FILE_WRITE_REQ   = 13,
    SI_PIH_CCHO_SET_REQ     = 14,
    SI_PIH_CCHC_SET_REQ     = 15,
    SI_PIH_CGLA_SET_REQ     = 16,
    SI_PIH_CARD_ATR_QRY_REQ = 17,
    SI_PIH_UICCAUTH_REQ     = 18,
    SI_PIH_URSM_REQ         = 19,
    SI_PIH_CARDTYPE_QUERY_REQ = 20,
    SI_PIH_CRSM_SET_REQ     = 21,
    SI_PIH_CRLA_SET_REQ     = 22,
    SI_PIH_SESSION_QRY_REQ  = 23,
    SI_PIH_SCICFG_SET_REQ   = 24,
    SI_PIH_SCICFG_QUERY_REQ = 25,
    SI_PIH_HVTEE_SET_REQ    = 26,
    SI_PIH_HVCHECKCARD_REQ  = 27,
    SI_PIH_CIMI_QRY_REQ     = 28,
    SI_PIH_CCIMI_QRY_REQ    = 29,

    SI_PIH_CCHP_SET_REQ          = 30,
    SI_PIH_CARDVOLTAGE_QUERY_REQ = 31,
    SI_PIH_PRIVATECGLA_SET_REQ   = 32,

    SI_PIH_CARDTYPEEX_QUERY_REQ = 33,

#if (FEATURE_ON == FEATURE_PHONE_SC)
    SI_PIH_SILENT_PIN_SET_REQ     = 34,
    SI_PIH_SILENT_PININFO_SET_REQ = 35,
#endif
    SI_PIH_ESIMSWITCH_SET_REQ  = 36,
    SI_PIH_ESIMSWITCH_QRY_REQ  = 37,
    SI_PIH_BWT_SET_REQ         = 38,
    SI_PIH_PRIVATECCHO_SET_REQ = 39,
    SI_PIH_PRIVATECCHP_SET_REQ = 40,

    SI_PIH_RCV_REFRESH_PID_IND = 41,

    SI_PIH_REQ_BUTT
};
typedef VOS_UINT32 SI_PIH_ReqUint32;

enum SI_PIH_Lock {
    SI_PIH_UNLOCK = 0,
    SI_PIH_LOCKED = 1,
    SI_PIH_LOCK_BUTT
};
typedef VOS_UINT32 SI_PIH_LockUint32;

enum SI_PIH_CardstateReport {
    SI_PIH_NEED_REPORT    = 0,
    SI_PIH_NO_NEED_REPORT = 1,
    SI_PIH_REPORT_BUTT
};

enum SI_PIH_HvsstHandleState {
    /* OP_ActiveCard OP_HandleVsim   OP_VsimState    OP_CurCardOK */
    SI_PIH_HVSST_DEACTIVE_RSIM_AGAIN = 0x00,
    /* 0             0               0               0 */ /* ������� */
    SI_PIH_HVSST_DEACTIVE_RSIM = 0x01,
    /* 0             0               0               1 */ /* ��Ҫ���� */
    SI_PIH_HVSST_STATE_ERROR2 = 0x02,
    /* 0             0               1               0 */ /* VSIM���������λ��ȥ����Ӳ�� */
    SI_PIH_HVSST_STATE_ERROR3 = 0x03,
    /* 0             0               1               1 */ /* VSIM����ʱ��ȥ����Ӳ�� */
    SI_PIH_HVSST_STATE_ERROR4 = 0x04,
    /* 0             1               0               0 */ /* Ӳ�����ڣ�VSIMδ�򿪣�ȥ����VSIM */
    SI_PIH_HVSST_STATE_ERROR5 = 0x05,
    /* 0             1               0               1 */ /* Ӳ���ڣ�VSIMδ�򿪣�ȥ����VSIM */
    SI_PIH_HVSST_DEACTIVE_VSIM_AGAIN = 0x06,
    /* 0             1               1               0 */ /* ������� */
    SI_PIH_HVSST_DEACTIVE_VSIM = 0x07,
    /* 0             1               1               1 */ /* ��Ҫ���� */
    SI_PIH_HVSST_ACTIVE_RSIM = 0x08,
    /* 1             0               0               0 */ /* ��Ҫ���� */
    SI_PIH_HVSST_ACTIVE_RSIM_AGAIN = 0x09,
    /* 1             0               0               1 */ /* �ظ�����Ӳ�� */
    SI_PIH_HVSST_ACTIVE_RSIM_AGAIN2 = 0x0A,
    /* 1             0               1               0 */ /* VSIM����ʧ��,����Ӳ�� */
    SI_PIH_HVSST_STATE_ERROR11 = 0x0B,
    /* 1             0               1               1 */ /* VSIM����ʱ��,����Ӳ�� */
    SI_PIH_HVSST_ACTIVE_VSIM = 0x0C,
    /* 1             1               0               0 */ /* ��Ҫ���� */
    SI_PIH_HVSST_ACTIVE_ERROR13 = 0x0D,
    /* 1             1               0               1 */ /* Ӳ����λʱ�򼤻�VSIM */
    SI_PIH_HVSST_ACTIVE_VSIM_AGAIN = 0x0E,
    /* 1             1               1               0 */ /* VSIM����ʱ��,ʹ��VSIM */
    SI_PIH_HVSST_ACTIVE_VSIM_AGAIN2 = 0x0F,
    /* 1             1               1               1 */ /* ������� */
    SI_PIH_HVSST_HANDLE_STATE_BUTT
};
typedef VOS_UINT8 SI_PIH_HvsstHandleStateUint8;

enum SI_PIH_HvteeDataflag {
    SI_PIH_HVTEE_APNSET   = 0xA5A5A001,
    SI_PIH_HVTEE_DHSET    = 0xA5A5A002,
    SI_PIH_HVTEE_VSIMDATA = 0xA5A5A003,
    SI_PIH_HVTEE_DATAFLAG_BUTT
};
typedef VOS_UINT32 SI_PIH_HvteeDataflagUint32;


enum SI_PIH_RaccessSrcType {
    SI_PIH_RACCESS_FROM_PIH = 0,
    SI_PIH_RACCESS_FROM_AT
};
typedef VOS_UINT8 SI_PIH_RaccessSrcTypeUint8;

enum SI_PIH_InfoList {
    SI_PIH_INFO_USED_LIST_ID = 0,
    SI_PIH_INFO_FREE_LIST_ID,
    SI_PIH_INFO_LIST_BUTT
};

typedef VOS_UINT8 SI_PIH_InfoListUint8;


enum SI_PIH_ErrLogAlmId {
    SI_PIH_ERR_LOG_ALM_REQ_RPT_FAIL            = 0x01, /* ��Errlog��ѯ�ϱ�(�Ѿ�ʹ��) */
    SI_PIH_ERR_LOG_ALM_ACTIVE_INIT_ERR_RPT     = 0x02, /* ��ʼ��ʧ�������ϱ� */
    SI_PIH_ERR_LOG_ALM_ACTIVE_COUNT_ERR_RPT    = 0x03, /* ͳ����ֵ�ﵽ�涨ʱ��������ϱ� */
    SI_PIH_ERR_LOG_ALM_CARDFETCH_IND_RPT       = 0x04, /* ����fetch�¼������ϱ� */
    SI_PIH_ERR_LOG_ALM_REFRESH_PLMN_IND_RPT    = 0x05, /* REFRESH PLMN�ϱ� */
    SI_PIH_ERR_LOG_ALM_INIT_CARD_FILE_FAIL_RPT = 0x06, /* ��������ʼ���쳣�ϱ� */
    SI_PIH_ERR_LOG_ALM_UICC_SWITCH_ICC_RPT     = 0x07, /* UICC����ʼ���쳣��ICC�� */
    SI_PIH_ERR_LOG_ALM_SIMCARD_BUSY_RPT        = 0x08, /* ����һ��ʱ����һֱ����BUSY */
    SI_PIH_ERR_LOG_ALM_FCP_DATA_ERROR_RPT      = 0x09, /* FCP�����쳣 */
    SI_PIH_ERR_LOG_ALM_AUTH_ERROR_RPT          = 0x0A, /* ��Ȩʧ�� */

    SI_PIH_ERR_LOG_ALM_ID_BUTT
};
typedef VOS_UINT16 SI_PIH_ErrLogAlmIdUint16;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)

typedef struct {
    chr_data_header_s header;
    VOS_UINT8         data[4];
} SI_PIH_ErrLogActiveRpt;
#endif

typedef struct {
    VOS_UINT32 startCountSlice;
    VOS_UINT32 cardSciErrCount;
    VOS_UINT32 cardSciUpdateErrCount;
    VOS_UINT32 cardSciReadErrCount;
    VOS_UINT32 cardSciAuthErrCount;
    VOS_UINT32 cardSciRAccessErrCount;
    VOS_UINT32 cardSciEnvelopeErrCount;
    VOS_UINT32 rsv1;
    VOS_UINT32 rsv2;
    VOS_UINT32 endCountSlice;
} SI_PIH_CardSciErrInfo;


typedef struct {
    VOS_UINT32 slice;
    VOS_UINT32 cardFetchType;
} SI_PIH_CardFetchInfo;


typedef struct {
    VOS_UINT32 errStep;
} SI_PIH_UiccSwitchIccChrInfo;


typedef struct {
    VOS_UINT32 nvTimeLen;
} SI_PIH_SimCardBusyChrInfo;


typedef struct {
    VOS_UINT16 fileId;
    VOS_UINT16 rsv;
} SI_PIH_FcpDataErrorChrInfo;


typedef struct {
    VOS_UINT32 failNum;
    VOS_UINT8  sw1;
    VOS_UINT8  sw2;
    VOS_UINT16 rsv;
} SI_PIH_AuthFailChrInfo;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32   msgName; /* ��Ϣ�� */
    VOS_UINT16   client;  /* �ͻ���ID */
    VOS_UINT8    opId;
    VOS_UINT8    rsv;
    SI_PIH_EventUint32 eventType;
} SI_PIH_MsgHeader;

typedef struct {
    SI_PIH_MsgHeader msgHeader;
    VOS_UINT8        pin2[SI_PIH_PIN_CODE_LEN]; /* PIN2�룬���ϲ����δ����PIN2����Ϊȫ0 */
} SI_PIH_FdnEnableReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader;
    VOS_UINT8        pin2[SI_PIH_PIN_CODE_LEN]; /* PIN2�룬���ϲ����δ����PIN2����Ϊȫ0 */
} SI_PIH_FdnDisableReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader;
    VOS_UINT32       dataLen;
    VOS_UINT8        data[8];
} SI_PIH_GaccessReq;


typedef struct {
    SI_PIH_MsgHeader       msgHeader; /* PIH��Ϣͷ    */
    VOS_UINT32             dataLen;   /* PIH��Ϣ����  */
    VOS_UINT8              data[8];  /* PIH��Ϣ����  */
} SI_PIH_IsdbAccessReqMsg;

typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    SI_PIH_HvsstSet  hvSSTData;
} SI_PIH_HvsstReq;


typedef struct {
    SI_PIH_MsgHeader     msgHeader; /* PIH��Ϣͷ */
    SI_PIH_EsimSwitchSet esimSwitchData;
} SI_PIH_EsimswitchReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    VOS_UINT8              fileNameLen;
    VOS_UINT8              fileName[SI_PIH_ATFILE_NAME_MAX];
    VOS_UINT8              ref;
    VOS_UINT8              totalNum;
    VOS_UINT8              index;
    VOS_UINT32             fileDataLen;
    VOS_UINT8              fileData[4];
} SI_PIH_FileWriteReq;

typedef struct {
    SI_PIH_LockUint32  pIHLock;
    VOS_UINT16              client;
    VOS_UINT8               opId;
    VOS_UINT8               resv;
    VOS_UINT32              cmdType;
    SI_PIH_EventUint32            eventType;
} SI_PIH_CtrlInfo;

typedef struct {
    SI_PIH_PollTimerStateUint32  pollState;
    VOS_UINT32                         pollData;
    VOS_UINT32                         timeLen;
    VOS_UINT32                         timerName;
    HTIMER                             timer;
} SI_PIH_PollTime;

typedef struct {
    VOS_UINT8 iMSILen;
    VOS_UINT8 imsi[9];
    VOS_UINT8 rsv[2];
} SI_PIH_Imsi;

typedef struct {
    USIMM_FilePathInfo filePath;
    VOS_UINT16               rspLen;
    VOS_UINT8                rspCotent[USIMM_T0_APDU_MAX_LEN];
    VOS_UINT8                rcv[2];
} SI_PIH_CsimCtrl;

typedef struct {
    VOS_UINT16 index;
    VOS_UINT16 refNum;
    VOS_UINT16 totalNum;
    VOS_UINT16 usRsv;
    FILE      *fpFile;
} SI_PIH_FwritePara;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgName;     /* ��Ϣ���� */
    VOS_UINT8  content[4]; /* �������� */
} SI_PIH_HookMsg;


typedef struct {
    SI_PIH_MsgHeader msgHeader;      /* PIH��Ϣͷ */
    VOS_UINT32       aidLen;         /* AID�ĳ��� */
    VOS_UINT8        adfName[USIMM_AID_LEN_MAX]; /* ���ǵ����ƶ�����AID���ȼ������󽫳�������1��(��usimm���벻ƥ��) */
} SI_PIH_CchoSetReqMsg;


typedef struct {
    SI_PIH_MsgHeader msgHeader;      /* PIH��Ϣͷ */
    VOS_UINT32       aidLen;         /* AID�ĳ��� */
    VOS_UINT8 adfName[USIMM_AID_LEN_MAX]; /* ���ǵ����ƶ�����AID���ȼ������󽫳�������1��(��usimm���벻ƥ��) */
    VOS_UINT8 apdup2;                      /* APDU�����P2���� */
    VOS_UINT8 rsv[3];
} SI_PIH_CchpSetReqMsg;


typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    VOS_UINT32       sessionId; /* �߼�ͨ���� */
} SI_PIH_CchcSetReqMsg;


typedef struct {
    SI_PIH_MsgHeader msgHeader;                  /* PIH��Ϣͷ    */
    VOS_UINT32             sessionId;                  /* �߼�ͨ���� */
    VOS_UINT32             dataLen;                    /* �����  */
    VOS_UINT8              data[SI_APDU_MAX_LEN + 1]; /* ��������, ��LE�ֶζ�һ���ֽ� */
    VOS_UINT8              rsv[3];
} SI_PIH_CglaReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader;
    VOS_UINT32             dataLen;
    VOS_UINT8              data[4];
} SI_PIH_HvsdhSetReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader;
} SI_PIH_HvsQryReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    SI_PIH_UiccAuth   authData;
} SI_PIH_UiccauthReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    SI_PIH_AccessFile cmdData;
} SI_PIH_AccessfileReq;

typedef struct {
    SI_PIH_MsgHeader       msgHeader; /* PIH��Ϣͷ */
    SI_PIH_CardSlotUint32  card0Slot;
    SI_PIH_CardSlotUint32  card1Slot;
    SI_PIH_CardSlotUint32  card2Slot;
} SI_PIH_ScicfgSetReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    VOS_UINT16             protectTime;
    VOS_UINT16             usRsv;
} SI_PIH_BwtSetReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    SI_PIH_HvteeSet  hvtee;
} SI_PIH_HvteeSetReq;

typedef struct {
    SI_PIH_MsgHeader msgHeader;
} SI_PIH_HvcheckcardReq;

typedef struct {
    USIMM_CardAppUint32  appType;
    VOS_UINT16                efId;      /* �ļ�ID */
    VOS_UINT8                 recordNum; /* �ļ���¼�ţ��������ļ���0 */
    VOS_UINT8                 rsv;       /* ���� */
} SI_PIH_GetfileInfo;

typedef struct {
    USIMM_CardAppUint32  appType;
    VOS_UINT16                efId;       /* �ļ�ID */
    VOS_UINT8                 recordNum;  /* �ļ���¼�ţ��������ļ���0 */
    VOS_UINT8                 rsv;        /* ���� */
    VOS_UINT32                efLen;      /* �������ݳ��� */
    VOS_UINT8                *efContent; /* ������������ */
} SI_PIH_SetfileInfo;


typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    SI_PIH_Crsm       msgContent;
} SI_PIH_CrsmSetReqMsg;


typedef struct {
    SI_PIH_MsgHeader msgHeader; /* PIH��Ϣͷ */
    SI_PIH_Crla       msgContent;
} SI_PIH_CrlaSetReqMsg;

typedef struct {
    VOS_UINT32 appType;
    VOS_UINT32 sessionId;
} SI_PIH_Channelappinfo;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)

typedef struct {
    OM_LIST_NODE_STRU listNode;
    VOS_UINT16        client;
    VOS_UINT8         opId;
    VOS_UINT8         resv;
    VOS_UINT32        cmdType;
    SI_PIH_EventUint32      eventType;
} SI_PIH_CtrlInfoNode;
#endif


typedef struct {
    SI_PIH_FileInfo fileInfo;
    VOS_BOOL              bIsNeedCheck; /* �Ƿ����������Զ�ȡ */
} SI_PIH_NeedCheckFile;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)

typedef struct {
    SI_PIH_NeedCheckFile  needCheckList[SI_PIH_KEYFILE_MAX_NUM];
    VOS_UINT32                   receiverPid;      /* �ؼ��ļ���⵽֮�������ϢPID */
    USIMM_CheckKeyFileNv nVCheckFileCfg;   /* NV����ʱ������ */
    VOS_UINT8                    needCheckFileNum; /* ����Ҫ��ȡ�ļ�����(ÿ�μ�⵽�ؼ��ļ���������һ) */
    VOS_UINT8                    checkFileNum;     /* NAS��Ҫ����ļ����� */
    VOS_UINT8                    timerCount;       /* ��ʱ����Ĵ��� */
    VOS_UINT8                    rsv;
} SI_PIH_KeyFileList;
#endif


typedef struct {
    USIMM_PhyCardTypeUint32    phyCardType; /* ����״̬ */
    USIMM_CardAppServicUint32  gutlSvcStatus;
    USIMM_CardTypeUint32       gutlCardType; /* USIM/SIM */
    USIMM_CardAppServicUint32  cdmaSvcStatus;
    USIMM_CardTypeUint32       cdmaCardType; /* CSIM/UIM */
    USIMM_CardAppServicUint32  isimSvcStatus;
    USIMM_CardTypeUint32       isimCardType; /* ISIM */
    USIMM_CardStatusAddinfo addInfo;         /* ��״̬��Чʱ�����ʹ���������Ϣ */
} SI_PIH_CardStatus;


typedef VOS_UINT32 (*PFSIPIHPIDMSGPROC)(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);


typedef struct {
    VOS_PID           senderPid;
    PFSIPIHPIDMSGPROC procFunc; /* ������ */
} SI_PIH_PIDMSGPROC_FUNC;


typedef struct {
    SI_PIH_MsgHeader msgHeader;
    SI_PIH_CryptoPin cryptoPin;
} SI_PIH_SilentPinReq;


typedef struct {
    SI_PIH_MsgHeader msgHeader;
    VOS_UINT32       dataLen;
    VOS_UINT8        data[USIMM_PINNUMBER_LEN];
} SI_PIH_SilentPininfoReq;


typedef struct {
    SI_PIH_MsgHeader msgHeader;
    ModemIdUint16   modemId;
    VOS_UINT16             rsv;
    VOS_UINT32             regPID;
} SI_PIH_RegRefreshPidReq;


typedef VOS_VOID (*SiPihUsimMsgProcFunc)(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *pMsg);


typedef struct {
    VOS_UINT32           msgName;
    SiPihUsimMsgProcFunc procFunc;
} SI_PIH_UsimMsgProcList;

extern VOS_MsgHookFunc vos_MsgHook;

extern VOS_UINT32 g_pihUsimBCPid[USIMM_SLOT_SIZE][SI_PIH_BCPID_REG_MAX];

extern VOS_UINT32 g_pihRefreshBCPid[USIMM_SLOT_SIZE][SI_PIH_BCPID_REG_MAX];

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
extern VOS_VOID SI_PIH_InitTEEShareAddr(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_PIH_Stop32KCheckStatusTimer(SI_PIH_CardSlotUint32 slotId, HTIMER *timer);

extern VOS_UINT32 SI_PIH_Start32KCheckStatusTimer(SI_PIH_CardSlotUint32 slotId, HTIMER *timer,
                                                  VOS_UINT32 timerLen, VOS_UINT32 timerName);

extern SI_PIH_PollTime *SI_PIH_GetPollTimerAddr(SI_PIH_CardSlotUint32  slotId,
                                                      SI_PIH_TimerNameUint32  timerName);

extern VOS_VOID SI_PIH_PollTimerPro(SI_PIH_CardSlotUint32 slotId, SI_PIH_PollTime *pIHTimer);

extern SI_PIH_CardStatus *SI_PIH_GetCardInfoAddr(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_PIH_CheckGCFTestCard(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_PIH_SendDeactiveReqMsg(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 sendPara);

#if (FEATURE_ON == FEATURE_PTM)
extern VOS_VOID SI_PIH_ErrLogVarInit(SI_PIH_CardSlotUint32 slotId);
#endif

extern VOS_VOID SI_PIH_MNTNDataHook(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 hookMsgName,
                                    VOS_UINT32 hookDataLen, VOS_UINT8 *hookData);

#endif

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
