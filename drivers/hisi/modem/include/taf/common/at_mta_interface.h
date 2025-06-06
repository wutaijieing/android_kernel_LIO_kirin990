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

#ifndef __AT_MTA_INTERFACE_H__
#define __AT_MTA_INTERFACE_H__

#include "PsTypeDef.h"
#include "at_mn_interface.h"
#include "mta_comm_interface.h"
#include "nv_stru_sys.h"
#include "nv_stru_gucnas.h"

#include "AtParse.h"
#include "nas_comm_packet_ser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef WIN32
#pragma warning(disable : 4200) /* zero-sized array in struct/union */
#endif

#pragma pack(4)

#define MTA_CPOS_XML_MAX_LEN (1024)   /* MTA接收AT下发的XML码流最大长度 */
#define MTA_CPOSR_XML_MAX_LEN (1024)  /* MTA向AT上报的XML码流最大长度为1024 */
#define MTA_SIMLOCK_PASSWORD_LEN (16) /* 锁网锁卡解锁密码的长度 */
#define MTA_MAX_EPDU_NAME_LEN (32)    /* EPDU码流上报中Name的长度 */
#define MTA_MAX_EPDU_BODY_LEN (500)   /* EPDU码流大小 */

#define MTA_CLIENTID_BROADCAST (0xFFFF)
#define MTA_INVALID_TAB_INDEX (0x00)

#define AT_MTA_WRR_AUTOTEST_MAX_PARA_NUM (10)   /* autotest命令下发参数最大个数 */
#define MTA_AT_WRR_AUTOTEST_MAX_RSULT_NUM (20)  /* autotest命令上报结果最大个数 */
#define MTA_AT_WRR_ONE_MEANRPT_MAX_CELL_NUM (8) /* 一次测量报告中小区最大个数 */
#define MTA_AT_WRR_MAX_MEANRPT_NUM (10)         /* 上报最近测量报告的最大个数 */

#define MTA_AT_WRR_MAX_NCELL_NUM (8)

#define AT_MTA_GAS_AUTOTEST_MAX_PARA_NUM (10)
#define MTA_AT_GAS_AUTOTEST_MAX_RSULT_NUM (20) /* autotest命令上报结果最大个数 */

#define AT_MTA_RPT_CFG_MAX_SIZE (8)

#define AT_MTA_HANDLEDECT_MIN_TYPE (0)
#define AT_MTA_HANDLEDECT_MAX_TYPE (4)

#define AT_RSA_CIPHERTEXT_LEN (256)

#define AT_MTA_INTEREST_FREQ_MAX_NUM (0x05)
#define AT_MTA_MBMS_AVL_SERVICE_MAX_NUM (0x10)
#define AT_MTA_ISMCOEX_BANDWIDTH_NUM (0x06)

#define AT_MTA_MEID_DATA_LEN (7)

#define MTA_AT_EFRUIMID_OCTET_LEN_EIGHT (8)
#define MTA_AT_EFRUIMID_OCTET_LEN_FIVE (5)

#define MTA_AT_UIMID_OCTET_LEN (4)

#define AT_MTA_GPS_XO_COEF_NUM (4)

#if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)
#define MTA_FREQLOCK_MAX_MODE (4)
#endif

#define MTA_AT_MAX_BESTFREQ_GROUPNUM (8)

#define MTA_AT_MAX_DIE_ID_LEN (32)

#define MTA_AT_MAX_RFFE_DIE_ID_LEN (16)
#define MTA_AT_MAX_RFFE_DIE_ID_COUNT (4)
#define MTA_AT_RFFE_DIE_ID_VALID (1)

#define MTA_PMU_MAX_DIE_SN_LEN (20)

#define AT_MAX_RS_INFO_NUM (32) /* LRRC最大RS信息个数 */
#define AT_RS_INFO_MULTI (100)  /* LRRC将RSRP、RSRQ的值乘100，再报给MTA */

#define AT_MTA_PSEUCELL_PLMN_LEN_FIVE (5)
#define AT_MTA_PSEUCELL_PLMN_LEN_SIX (6)
#define AT_MTA_PSEUCELL_LAC_LEN (4)
#define AT_MTA_PSEUCELL_CELLID_LEN (8)
#define AT_MTA_PLMN_MCC_LEN (3)
#define AT_MTA_PLMN_MNC_LEN_TWO (2)
#define AT_MTA_PLMN_MNC_LEN_THREE (3)

#define MTA_AT_PESN_NV_DATA_LEN (4)
#define MTA_AT_MEID_NV_DATA_LEN_NEW (7)

#define AT_MTA_EPDU_NAME_LENGTH_MAX (32)
#define AT_MTA_EPDU_CONTENT_LENGTH_MAX (250)
#define AT_MTA_EPDU_CONTENT_STRING_LEN_MAX (500)

#define AT_MTA_MAX_BAND_NUM (8)     /* 最多支持8个Band */
#define AT_MTA_BAND_INFO_OFFSET (3) /* band信息偏移参数值 */
#define AT_MTA_MAX_SUB_BAND_NUM (4) /* 每个Band的频率可以分为4个子BAND */
#define AT_MTA_BAND_INFO_LEN (72)   /* 每个Band信息的长度信息 */
#define MTA_AT_CA_MAX_CELL_NUM (8)
#define MTA_AT_UTC_MAX_LENGTH (21)

#define MTA_AT_BOOSTER_IND_MAX_NUM (248)    /* Booster特性最多支持LRRC给MTA上报248字节码流 */
#define MTA_AT_BOOSTER_QRY_CNF_MAX_NUM (40) /* Booster特性最多支持查询结果字节码流 */
#define MTA_AT_UE_CAP_INFO_IND_MAX_LEN 2400 /* UE能力最多支持LRRC给MTA上报2400字节码流 */

#define MTA_AT_EPS_NETWORK_BYTE (2)

#define MTA_AT_MAX_STATEII_LEVEL_ITEM 4

#define AT_MTA_VERSION_INFO_LEN (10) /* 机器学习模型中版本号对应的长度 */

#define UCOM_NV_DCXO_SAMPLE_H_MAX_NUM (40)

#define AT_MTA_UECAP_MAX_NR_BAND_NUM (32)

#define AT_MTA_NRRCCAP_PARA_MAX_NUM (10)

#define MTA_AT_MAX_CC_NUMBER (4)
#define MTA_AT_MAX_CODE_NUMBER (2)
#define MTA_AT_INVALID_MCS_VALUE (0xFF)

#define MTA_AT_MAX_ALLOWED_AND_CFG_NSSAI_PLMN_NUM 16

#define AT_MTA_LTE_SUPPORT_OVERHEATING_PARA_NUM 7
#define AT_MTA_NR_SUPPORT_OVERHEATING_PARA_NUM 13
#define AT_MTA_NOT_SUPPORT_OVERHEATING_PARA_NUM 2
#define MTA_AT_MAX_LADN_DNN_NUM 8

#define MTA_AT_MAX_NR_NCELL_RPT_NUM 4
#define MTA_AT_MAX_NR_NCELL_BEAM_RPT_NUM 4
#define MTA_AT_MAX_NR_SSB_BEAM_NUM 8
#define MTA_AT_NR_RSRP_INVALID_VALUE 0x7FFF
#define MTA_AT_NR_SSBID_INVALID_VALUE 0xFF

#if (FEATURE_LTEV == FEATURE_ON)
#define MTA_AT_LTEV_BAND_MAX_NUM 64
#endif

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
#define MTA_NRPHY_MAX_PARA_NUM 15
#endif

enum AT_MTA_MsgType {
    /* 消息名称 */ /* 消息ID */ /* 备注, 生成ASN */
    /* AT发给MTA的消息 */
    ID_AT_MTA_CPOS_SET_REQ      = 0x0000, /* _H2ASN_MsgChoice AT_MTA_CposReq        */
    ID_AT_MTA_CGPSCLOCK_SET_REQ = 0x0001, /* _H2ASN_MsgChoice AT_MTA_CgpsclockReq   */
    ID_AT_MTA_SIMLOCKUNLOCK_SET_REQ = 0x0003, /* _H2ASN_MsgChoice AT_MTA_SimlockunlockReq */

    ID_AT_MTA_QRY_NMR_REQ = 0x0004, /* _H2ASN_MsgChoice AT_MTA_QryNmrReq */

    /* _H2ASN_MsgChoice AT_MTA_ReselOffsetCfgSetNtf */
    ID_AT_MTA_RESEL_OFFSET_CFG_SET_NTF = 0x0005,
    ID_AT_MTA_WRR_AUTOTEST_QRY_REQ     = 0x0006, /* _H2ASN_MsgChoice AT_MTA_WrrAutotestQryPara */
    ID_AT_MTA_WRR_CELLINFO_QRY_REQ     = 0x0007, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_WRR_MEANRPT_QRY_REQ      = 0x0008, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_WRR_FREQLOCK_SET_REQ     = 0x0009, /* _H2ASN_MsgChoice MTA_AT_WrrFreqlockCtrl     */
    ID_AT_MTA_WRR_RRC_VERSION_SET_REQ  = 0x000A, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_WRR_CELLSRH_SET_REQ      = 0x000B, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_WRR_FREQLOCK_QRY_REQ     = 0x000C, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_WRR_RRC_VERSION_QRY_REQ  = 0x000D, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_WRR_CELLSRH_QRY_REQ      = 0x000E, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_BODY_SAR_SET_REQ        = 0x000F, /* _H2ASN_MsgChoice AT_MTA_BodySarSetReq */
    ID_AT_MTA_CURC_SET_NOTIFY         = 0x0010, /* _H2ASN_MsgChoice AT_MTA_CurcSetNotify */
    ID_AT_MTA_CURC_QRY_REQ            = 0x0011, /* _H2ASN_MsgChoice AT_MTA_CurcQryReq */
    ID_AT_MTA_UNSOLICITED_RPT_SET_REQ = 0x0012, /* _H2ASN_MsgChoice AT_MTA_UnsolicitedRptSetReq */
    ID_AT_MTA_UNSOLICITED_RPT_QRY_REQ = 0x0013, /* _H2ASN_MsgChoice AT_MTA_UnsolicitedRptQryReq */

    ID_AT_MTA_IMEI_VERIFY_QRY_REQ   = 0x0014, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_CGSN_QRY_REQ          = 0x0015, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_NCELL_MONITOR_SET_REQ = 0x0016, /* _H2ASN_MsgChoice AT_MTA_NcellMonitorSetReq */
    ID_AT_MTA_NCELL_MONITOR_QRY_REQ = 0x0017, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_REFCLKFREQ_SET_REQ = 0x0018, /* _H2ASN_MsgChoice AT_MTA_RefclkfreqSetReq */
    ID_AT_MTA_REFCLKFREQ_QRY_REQ = 0x0019, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_HANDLEDECT_SET_REQ = 0x001A, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_HANDLEDECT_QRY_REQ = 0x001B, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_ECID_SET_REQ = 0x001C, /* _H2ASN_MsgChoice AT_MTA_EcidSetReq */

    ID_AT_MTA_MIPICLK_QRY_REQ = 0x001D, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_SET_DPDTTEST_FLAG_REQ = 0x001E, /* _H2ASN_MsgChoice AT_MTA_SetDpdttestFlagReq */
    ID_AT_MTA_SET_DPDT_VALUE_REQ    = 0x001F, /* _H2ASN_MsgChoice AT_MTA_SetDpdtValueReq */
    ID_AT_MTA_QRY_DPDT_VALUE_REQ    = 0x0020, /* _H2ASN_MsgChoice AT_MTA_QryDpdtValueReq */

    ID_AT_MTA_RRC_PROTECT_PS_REQ = 0x0021,
    ID_AT_MTA_PHY_INIT_REQ       = 0x0022,

    ID_AT_MTA_SET_JAM_DETECT_REQ = 0x0023, /* _H2ASN_MsgChoice AT_MTA_SetJamDetectReq */

    ID_AT_MTA_SET_FREQ_LOCK_REQ = 0x0024, /* _H2ASN_MsgChoice AT_MTA_SetFreqLockReq */

    ID_AT_MTA_SET_GSM_FREQLOCK_REQ = 0x0025, /* _H2ASN_MsgChoice AT_MTA_SetGsmFreqlockReq */
    ID_AT_MTA_SET_FEMCTRL_REQ      = 0x0026,

    ID_AT_MTA_NVWRSECCTRL_SET_REQ = 0x0027,

    /* _H2ASN_MsgChoice AT_MTA_MbmsServiceOptionSetReq */
    ID_AT_MTA_MBMS_SERVICE_OPTION_SET_REQ = 0x0028,
    ID_AT_MTA_MBMS_PREFERENCE_SET_REQ     = 0x0029, /* _H2ASN_MsgChoice AT_MTA_MbmsPreferenceSetReq */
    ID_AT_MTA_SIB16_NETWORK_TIME_QRY_REQ  = 0x002A, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_BSSI_SIGNAL_LEVEL_QRY_REQ   = 0x002B, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_NETWORK_INFO_QRY_REQ        = 0x002C, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_EMBMS_STATUS_QRY_REQ        = 0x002D, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    /* _H2ASN_MsgChoice AT_MTA_MbmsUnsolicitedCfgSetReq */
    ID_AT_MTA_MBMS_UNSOLICITED_CFG_SET_REQ = 0x002E,
    /* _H2ASN_MsgChoice AT_MTA_LowPowerConsumptionSetReq */
    ID_AT_MTA_LTE_LOW_POWER_SET_REQ = 0x002F,
    /* _H2ASN_MsgChoice AT_MTA_MbmsInterestlistSetReq */
    ID_AT_MTA_INTEREST_LIST_SET_REQ = 0x0030,
    /* _H2ASN_MsgChoice AT_MTA_MbmsServiceStateSetReq */
    ID_AT_MTA_MBMS_SERVICE_STATE_SET_REQ    = 0x0031,
    ID_AT_MTA_MBMS_AVL_SERVICE_LIST_QRY_REQ = 0x0032, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_LTE_WIFI_COEX_SET_REQ         = 0x0033, /* _H2ASN_MsgChoice AT_MTA_LteWifiCoexSetReq */
    ID_AT_MTA_LTE_WIFI_COEX_QRY_REQ         = 0x0034, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_SET_FR_REQ = 0x0035,

    ID_AT_MTA_MEID_SET_REQ = 0x0036, /* _H2ASN_MsgChoice AT_MTA_MeidSetReq */
    ID_AT_MTA_MEID_QRY_REQ = 0x0037,

#if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)
    ID_AT_MTA_SET_M2M_FREQLOCK_REQ = 0x0038, /* _H2ASN_MsgChoice AT_MTA_SetM2MFreqlockReq */
    ID_AT_MTA_QRY_M2M_FREQLOCK_REQ = 0x0039, /* _H2ASN_MsgChoice AT_MTA_Reserve */
#endif

    ID_AT_MTA_TRANSMODE_QRY_REQ = 0x0044,

    ID_AT_MTA_UE_CENTER_SET_REQ = 0x0045, /* _H2ASN_MsgChoice AT_MTA_SetUeCenterReq */
    ID_AT_MTA_UE_CENTER_QRY_REQ = 0x0046,

    ID_AT_MTA_SET_NETMON_SCELL_REQ = 0x0047,
    ID_AT_MTA_SET_NETMON_NCELL_REQ = 0x0048,

    ID_AT_MTA_RFICSSIRD_QRY_REQ = 0x0049,

    ID_AT_MTA_ANQUERY_QRY_REQ = 0x004A,
    ID_AT_MTA_CSNR_QRY_REQ    = 0x004B,
    ID_AT_MTA_CSQLVL_QRY_REQ  = 0x004C,

    ID_AT_MTA_EVDO_SYS_EVENT_SET_REQ = 0x004d, /* _H2ASN_MsgChoice AT_MTA_EvdoSysEventSetReq */
    ID_AT_MTA_EVDO_SIG_MASK_SET_REQ  = 0x004e, /* _H2ASN_MsgChoice AT_MTA_EvdoSigMaskSetReq */

    /* _H2ASN_MsgChoice AT_MTA_SetXcposrReq */
    ID_AT_MTA_XCPOSR_SET_REQ = 0x004F,
    /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_XCPOSR_QRY_REQ = 0x0050,
    /* _H2ASN_MsgChoice AT_MTA_SetXcposrrptReq */
    ID_AT_MTA_XCPOSRRPT_SET_REQ = 0x0051,
    /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_XCPOSRRPT_QRY_REQ      = 0x0052,
    ID_AT_MTA_CLEAR_HISTORY_FREQ_REQ = 0x0053, /* _H2ASN_MsgChoice AT_MTA_ClearHistoryFreqReq */

    /* _H2ASN_MsgChoice AT_MTA_SetSensorReq */
    ID_AT_MTA_SET_SENSOR_REQ = 0x0054,
    /* _H2ASN_MsgChoice AT_MTA_SetScreenReq */
    ID_AT_MTA_SET_SCREEN_REQ = 0x0055,
    /* _H2ASN_MsgChoice MTA_AT_FratIgnitionQryCnf */
    ID_AT_MTA_FRAT_IGNITION_QRY_REQ = 0x0056,
    /* _H2ASN_MsgChoice MTA_AT_FratIgnitionSetCnf */
    ID_AT_MTA_FRAT_IGNITION_SET_REQ = 0x0057,

    ID_AT_MTA_MODEM_TIME_SET_REQ = 0x0058,

    ID_AT_MTA_RX_TEST_MODE_SET_REQ = 0x0059,

    ID_AT_MTA_SLAVE_SET_REQ = 0x005A,

    ID_AT_MTA_BESTFREQ_SET_REQ = 0x005B, /* AT_MTA_BestfreqSetReq */
    ID_AT_MTA_BESTFREQ_QRY_REQ = 0x005C,

    ID_AT_MTA_RFIC_DIE_ID_QRY_REQ = 0x005D,

    ID_AT_MTA_CRRCONN_SET_REQ     = 0x005E,
    ID_AT_MTA_CRRCONN_QRY_REQ     = 0x005F,
    ID_AT_MTA_VTRLQUALRPT_SET_REQ = 0x0060,

    ID_AT_MTA_PMU_DIE_SN_QRY_REQ = 0x0061,

    ID_AT_MTA_MODEM_CAP_UPDATE_REQ = 0x0062,

    ID_AT_MTA_TAS_CFG_REQ = 0x0063,
    ID_AT_MTA_TAS_QRY_REQ = 0x0064,

    ID_AT_MTA_MIPI_WREX_REQ = 0x0065,
    ID_AT_MTA_MIPI_RDEX_REQ = 0x0066,

    ID_AT_MTA_RS_INFO_QRY_REQ = 0x0067,

    ID_AT_MTA_PHY_COM_CFG_SET_REQ = 0x0068,

    ID_AT_MTA_ERRCCAP_CFG_SET_REQ = 0x0069,
    ID_AT_MTA_ERRCCAP_QRY_SET_REQ = 0x006A,

    ID_AT_MTA_QRY_GSM_FREQLOCK_REQ  = 0x006B,
    ID_AT_MTA_DEL_CELLENTITY_REQ    = 0x006C,
    ID_AT_MTA_PSEUCELL_INFO_SET_REQ = 0x006D,

    ID_AT_MTA_POWER_DET_QRY_REQ = 0x006E, /* _H2ASN_MsgChoice AT_MTA_Reserve */

    ID_AT_MTA_MIPIREAD_SET_REQ = 0x006F,

    ID_AT_MTA_NO_CARD_MODE_SET_REQ = 0x0070,
    ID_AT_MTA_NO_CARD_MODE_QRY_REQ = 0x0071,

    /* _H2ASN_MsgChoice AT_MTA_ChrAlarmRlatCfgSetReq */
    ID_AT_MTA_CHRALARMRLAT_CFG_SET_REQ = 0x0072,

    ID_AT_MTA_ECC_CFG_SET_REQ = 0x0073,

    ID_AT_MTA_EPDU_SET_REQ = 0x0074,

    ID_AT_MTA_LTE_CA_CELL_RPT_CFG_SET_REQ = 0x0075,
    ID_AT_MTA_LTE_CA_CELL_RPT_CFG_QRY_REQ = 0x0076,
    ID_AT_MTA_LTE_CA_CFG_SET_REQ          = 0x0077,
    ID_AT_MTA_LTE_CA_CELLEX_QRY_REQ       = 0x0078,

    ID_AT_MTA_FINE_TIME_SET_REQ = 0x0079,
    /* _H2ASN_MsgChoice AT_MTA_GasAutotestQryPara    */
    ID_AT_MTA_GAS_AUTOTEST_QRY_REQ = 0x0080,

    ID_AT_MTA_COMM_BOOSTER_SET_REQ = 0x0081, /* _H2ASN_MsgChoice AT_MTA_CommBoosterSetReq */
    ID_AT_MTA_COMM_BOOSTER_QRY_REQ = 0x0082, /* _H2ASN_MsgChoice AT_MTA_CommBoosterQueryReq */

    ID_AT_MTA_NVLOAD_SET_REQ = 0x0083,

    ID_AT_MTA_SMS_DOMAIN_SET_REQ = 0x0084,
    ID_AT_MTA_SMS_DOMAIN_QRY_REQ = 0x0085,
    /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_AT_MTA_EPS_NETWORK_QRY_REQ = 0x0086,

    ID_AT_MTA_SET_SAMPLE_REQ = 0x0087,

    ID_AT_MTA_GPS_LOCSET_SET_REQ = 0x0088,

    ID_AT_MTA_CCLK_QRY_REQ = 0x0089,

    ID_AT_MTA_GAME_MODE_SET_REQ = 0x008A, /* _H2ASN_MsgChoice AT_MTA_COMM_GAME_MODE_SET_STRU */

    ID_AT_MTA_PSEUDBTS_SET_REQ = 0x008B,

    ID_AT_MTA_SUBCLFSPARAM_SET_REQ = 0x008C,
    ID_AT_MTA_SUBCLFSPARAM_QRY_REQ = 0x008D,

    ID_AT_MTA_NV_REFRESH_SET_REQ = 0x008E, /* _H2ASN_MsgChoice AT_MTA_NvRefreshSetReq */

#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    ID_AT_MTA_SLT_TEST_SET_REQ = 0X008F,
#endif

    ID_AT_MTA_CDMA_MODEM_CAP_SET_REQ = 0x0090,

    ID_AT_MTA_CDMA_CAP_RESUME_SET_REQ = 0x0091,

    ID_AT_MTA_PHYMIPIWRITE_SET_REQ = 0x0092,

    ID_AT_MTA_FORCESYNC_SET_REQ = 0x0093,

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_LENDC_QRY_REQ       = 0x0094,
    ID_AT_MTA_NR_FREQLOCK_SET_REQ = 0x0095,
    ID_AT_MTA_NR_FREQLOCK_QRY_REQ = 0x0096,
#endif

    ID_AT_MTA_SET_LTEPWRDISS_REQ = 0x0097,

    ID_AT_MTA_GNSS_NTY = 0x0098,

    ID_AT_MTA_SFEATURE_QRY_REQ     = 0x0099,
    ID_AT_MTA_LOW_PWR_MODE_SET_REQ = 0x009A,
    ID_AT_MTA_LL2_COM_CFG_SET_REQ  = 0x009D,
    ID_AT_MTA_LL2_COM_CFG_QRY_REQ  = 0x009E,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_NL2_COM_CFG_SET_REQ = 0x009F,
    ID_AT_MTA_NL2_COM_CFG_QRY_REQ = 0x00A0,

    ID_AT_MTA_NRRCCAP_CFG_SET_REQ = 0x00A1,
    ID_AT_MTA_NRRCCAP_QRY_REQ     = 0x00A2,
    ID_AT_MTA_NRPWRCTRL_SET_REQ   = 0x00A3,
#endif

    ID_AT_MTA_SARREDUCTION_NTF = 0x00A4,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_SET_TRX_TAS_REQ          = 0x00A5,
    ID_AT_MTA_QRY_TRX_TAS_REQ          = 0x00A6,
    ID_AT_MTA_MULTI_PMU_DIE_ID_SET_REQ = 0x00A7,
    ID_AT_MTA_5G_OPTION_CFG_REQ        = 0X00A8,
    ID_AT_MTA_5G_OPTION_QRY_REQ        = 0X00A9,

    ID_AT_MTA_5G_NSSAI_SET_REQ = 0x00AA,
    ID_AT_MTA_5G_NSSAI_QRY_REQ = 0x00AB,
#endif
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_SET_NETMON_SSCELL_REQ = 0x00AE,
#endif

    ID_AT_MTA_RFFE_DIE_ID_QRY_REQ       = 0x00AF,
    ID_AT_MTA_OVERHEATING_CFG_SET_REQ   = 0x00B0,
    ID_AT_MTA_OVERHEATING_CFG_QRY_REQ   = 0x00B1,
    ID_AT_MTA_LRRC_UE_CAP_PARAM_SET_REQ = 0x00B2,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_NRRC_UE_CAP_PARAM_SET_REQ = 0x00B3,
#endif
    ID_AT_MTA_UART_TEST_REQ = 0x00B4,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_SET_LADN_RPT_REQ          = 0x00B5,
    ID_AT_MTA_QRY_LADN_INFO_REQ         = 0x00B6,
    ID_AT_MTA_NPDCP_SLEEP_THRES_CFG_REQ = 0x00B7,
    ID_AT_MTA_NR_SSB_ID_QRY_REQ         = 0x00B8,
    ID_AT_MTA_NR_NW_CAP_INFO_RPT_CFG_SET_REQ = 0x00BD,
#endif
    ID_AT_MTA_NV_REFRESH_RSP = 0x2002, /* 防止AT->MTA新增消息和ID_MTA_NV_REFRESH_RSP取值重复 */

    /* MTA发给AT的消息 */
    ID_MTA_AT_CPOS_SET_CNF          = 0x1000, /* _H2ASN_MsgChoice MTA_AT_CposCnf        */
    ID_MTA_AT_CGPSCLOCK_SET_CNF     = 0x1001, /* _H2ASN_MsgChoice MTA_AT_CgpsclockCnf   */
    ID_MTA_AT_CPOSR_IND             = 0x1002, /* _H2ASN_MsgChoice MTA_AT_CposrInd       */
    ID_MTA_AT_XCPOSRRPT_IND         = 0x1003, /* _H2ASN_MsgChoice MTA_AT_XcposrrptInd   */
    ID_MTA_AT_APSEC_SET_CNF         = 0x1004, /* _H2ASN_MsgChoice MTA_AT_APSEC_CNF_STRU       */
    ID_MTA_AT_SIMLOCKUNLOCK_SET_CNF = 0x1005, /* _H2ASN_MsgChoice MTA_AT_SimlockunlockCnf */

    ID_MTA_AT_QRY_NMR_CNF = 0x1006, /* _H2ASN_MsgChoice MTA_AT_QryNmrCnf */

    /* _H2ASN_MsgChoice MTA_AT_WrrAutotestQryCnf       */
    ID_MTA_AT_WRR_AUTOTEST_QRY_CNF = 0x1007,
    /* _H2ASN_MsgChoice MTA_AT_WrrCellinfoQryCnf       */
    ID_MTA_AT_WRR_CELLINFO_QRY_CNF = 0x1008,
    /* _H2ASN_MsgChoice MTA_AT_WrrMeanrptQryCnf        */
    ID_MTA_AT_WRR_MEANRPT_QRY_CNF = 0x1009,
    /* _H2ASN_MsgChoice MTA_AT_WrrFreqlockSetCnf       */
    ID_MTA_AT_WRR_FREQLOCK_SET_CNF = 0x100A,
    /* _H2ASN_MsgChoice MTA_AT_WrrRrcVersionSetCnf    */
    ID_MTA_AT_WRR_RRC_VERSION_SET_CNF = 0x100B,
    /* _H2ASN_MsgChoice MTA_AT_WrrCellsrhSetCnf        */
    ID_MTA_AT_WRR_CELLSRH_SET_CNF = 0x100C,
    /* _H2ASN_MsgChoice MTA_AT_WrrFreqlockQryCnf       */
    ID_MTA_AT_WRR_FREQLOCK_QRY_CNF = 0x100D,
    /* _H2ASN_MsgChoice MTA_AT_WrrRrcVersionQryCnf    */
    ID_MTA_AT_WRR_RRC_VERSION_QRY_CNF = 0x100E,
    /* _H2ASN_MsgChoice MTA_AT_WrrCellsrhQryCnf        */
    ID_MTA_AT_WRR_CELLSRH_QRY_CNF = 0x100F,

    ID_MTA_AT_BODY_SAR_SET_CNF = 0x1010, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */

    ID_MTA_AT_CURC_QRY_CNF            = 0x1011, /* _H2ASN_MsgChoice MTA_AT_CurcQryCnf */
    ID_MTA_AT_UNSOLICITED_RPT_SET_CNF = 0x1012, /* _H2ASN_MsgChoice MTA_AT_UnsolicitedRptSetCnf */
    ID_MTA_AT_UNSOLICITED_RPT_QRY_CNF = 0x1013, /* _H2ASN_MsgChoice MTA_AT_UnsolicitedRptQryCnf */

    ID_MTA_AT_IMEI_VERIFY_QRY_CNF   = 0x1014, /* _H2ASN_MsgChoice AT_MTA_Reserve */
    ID_MTA_AT_CGSN_QRY_CNF          = 0x1015, /* _H2ASN_MsgChoice MTA_AT_CgsnQryCnf */
    ID_MTA_AT_NCELL_MONITOR_SET_CNF = 0x1016, /* _H2ASN_MsgChoice MTA_AT_ResultCnf        */
    /* _H2ASN_MsgChoice MTA_AT_NcellMonitorQryCnf        */
    ID_MTA_AT_NCELL_MONITOR_QRY_CNF = 0x1017,
    /* _H2ASN_MsgChoice MTA_AT_NcellMonitorInd        */
    ID_MTA_AT_NCELL_MONITOR_IND = 0x1018,

    ID_MTA_AT_REFCLKFREQ_SET_CNF = 0x1019, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_REFCLKFREQ_QRY_CNF = 0x101A, /* _H2ASN_MsgChoice MTA_AT_RefclkfreqQryCnf */
    ID_MTA_AT_REFCLKFREQ_IND     = 0x101B, /* _H2ASN_MsgChoice MTA_AT_RefclkfreqInd */

    ID_MTA_AT_HANDLEDECT_SET_CNF = 0x101C, /* _H2ASN_MsgChoice MTA_AT_HandledectSetCnf */
    ID_MTA_AT_HANDLEDECT_QRY_CNF = 0x101D, /* _H2ASN_MsgChoice MTA_AT_HandledectQryCnf */

    ID_MTA_AT_PS_TRANSFER_IND = 0x101E, /* _H2ASN_MsgChoice MTA_AT_PsTransferInd */

    ID_MTA_AT_ECID_SET_CNF = 0x101F, /* _H2ASN_MsgChoice MTA_AT_EcidSetCnf */

    ID_MTA_AT_MIPICLK_QRY_CNF  = 0x1020, /* _H2ASN_MsgChoice MTA_AT_RfLcdMipiclkCnf */
    ID_MTA_AT_MIPICLK_INFO_IND = 0x1021, /* _H2ASN_MsgChoice MTA_AT_RfLcdMipiclkInd */

    ID_MTA_AT_SET_DPDTTEST_FLAG_CNF = 0x1022, /* _H2ASN_MsgChoice MTA_AT_SetDpdttestFlagCnf */
    ID_MTA_AT_SET_DPDT_VALUE_CNF    = 0x1023, /* _H2ASN_MsgChoice MTA_AT_SetDpdtValueCnf */
    ID_MTA_AT_QRY_DPDT_VALUE_CNF    = 0x1024, /* _H2ASN_MsgChoice MTA_AT_QryDpdtValueCnf */

    ID_MTA_AT_RRC_PROTECT_PS_CNF = 0x1025,
    ID_MTA_AT_PHY_INIT_CNF       = 0x1026,

    ID_MTA_AT_SET_JAM_DETECT_CNF = 0x1027, /* _H2ASN_MsgChoice MTA_AT_SetJamDetectCnf */
    ID_MTA_AT_JAM_DETECT_IND     = 0x1028, /* _H2ASN_MsgChoice MTA_AT_JamDetectInd */

    ID_MTA_AT_SET_FREQ_LOCK_CNF = 0x1029, /* _H2ASN_MsgChoice MTA_AT_SetFreqLockCnf */

    ID_MTA_AT_SET_GSM_FREQLOCK_CNF = 0x102A, /* _H2ASN_MsgChoice MTA_AT_SetGsmFreqlockCnf */

    ID_AT_MTA_SET_FEMCTRL_CNF = 0x102B,

    ID_MTA_AT_XPASS_INFO_IND = 0x102C,

    ID_MTA_AT_NVWRSECCTRL_SET_CNF = 0x102D,

    ID_MTA_AT_MBMS_SERVICE_OPTION_SET_CNF = 0x102E, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_MBMS_PREFERENCE_SET_CNF     = 0x102F, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    /* _H2ASN_MsgChoice MTA_AT_MbmsSib16NetworkTimeQryCnf */
    ID_MTA_AT_SIB16_NETWORK_TIME_QRY_CNF = 0x1030,
    /* _H2ASN_MsgChoice MTA_AT_MbmsBssiSignalLevelQryCnf */
    ID_MTA_AT_BSSI_SIGNAL_LEVEL_QRY_CNF = 0x1031,
    /* _H2ASN_MsgChoice MTA_AT_MbmsNetworkInfoQryCnf */
    ID_MTA_AT_NETWORK_INFO_QRY_CNF         = 0x1032,
    ID_MTA_AT_EMBMS_STATUS_QRY_CNF         = 0x1033, /* _H2ASN_MsgChoice MTA_AT_EmbmsStatusQryCnf */
    ID_MTA_AT_MBMS_UNSOLICITED_CFG_SET_CNF = 0x1034, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_MBMS_SERVICE_EVENT_IND       = 0x1035, /* _H2ASN_MsgChoice MTA_AT_MbmsServiceEventInd */
    ID_MTA_AT_LTE_LOW_POWER_SET_CNF        = 0x1036, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_INTEREST_LIST_SET_CNF        = 0x1037, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_MBMS_SERVICE_STATE_SET_CNF   = 0x1038, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    /* _H2ASN_MsgChoice MTA_AT_MbmsAvlServiceListQryCnf */
    ID_MTA_AT_MBMS_AVL_SERVICE_LIST_QRY_CNF = 0x1039,
    ID_MTA_AT_LTE_WIFI_COEX_SET_CNF         = 0x103A, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_LTE_WIFI_COEX_QRY_CNF         = 0x103B, /* _H2ASN_MsgChoice MTA_AT_LteWifiCoexQryCnf */

    ID_MTA_AT_SET_FR_CNF = 0x103C,

    ID_MTA_AT_MEID_SET_CNF = 0x103d, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */

    ID_MTA_AT_RFICSSIRD_QRY_CNF = 0x103e,

    ID_MTA_AT_MEID_QRY_CNF = 0x103f, /* _H2ASN_MsgChoice MTA_AT_MeidQryCnf */

#if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)
    ID_MTA_AT_SET_M2M_FREQLOCK_CNF = 0x1040, /* _H2ASN_MsgChoice AT_MTA_SET_M2M_FREQLOCK_CNF_STRU */
    ID_MTA_AT_QRY_M2M_FREQLOCK_CNF = 0x1041, /* _H2ASN_MsgChoice MTA_AT_QRY_FREQLOCK_CNF_STRU */
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_LENDC_QRY_CNF       = 0x1042,
    ID_MTA_AT_LENDC_INFO_IND      = 0x1043,
    ID_MTA_AT_NR_FREQLOCK_SET_CNF = 0x1044,
    ID_MTA_AT_NR_FREQLOCK_QRY_CNF = 0x1045,
#endif

    ID_MTA_AT_TRANSMODE_QRY_CNF = 0x1052,

    ID_MTA_AT_UE_CENTER_SET_CNF = 0x1053,
    ID_MTA_AT_UE_CENTER_QRY_CNF = 0x1054, /* _H2ASN_MsgChoice MTA_AT_QryUeCenterCnf */

    ID_MTA_AT_SET_NETMON_SCELL_CNF = 0x1055, /* _H2ASN_MsgChoice MTA_AT_NetmonCellInfo */
    ID_MTA_AT_SET_NETMON_NCELL_CNF = 0x1056, /* _H2ASN_MsgChoice MTA_AT_NetmonCellInfo */

    ID_AT_MTA_QRY_AFC_CLK_FREQ_XOCOEF_REQ = 0x1057, /* _H2ASN_MsgChoice AT_MTA_QryAfcClkFreqXocoefReq */
    ID_MTA_AT_QRY_AFC_CLK_FREQ_XOCOEF_CNF = 0x1058, /* _H2ASN_MsgChoice MTA_AT_QryAfcClkFreqXocoefCnf */
    ID_MTA_AT_ANQUERY_QRY_CNF             = 0x1059, /* _H2ASN_MsgChoice MTA_AT_AntennaInfoQryCnf */
    ID_MTA_AT_CSNR_QRY_CNF                = 0x105A, /* _H2ASN_MsgChoice MTA_AT_CsnrQryCnf */
    ID_MTA_AT_CSQLVL_QRY_CNF              = 0x105B, /* _H2ASN_MsgChoice MTA_AT_CsqlvlQryCnf */

    ID_MTA_AT_EVDO_SYS_EVENT_CNF       = 0x105c, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_EVDO_SIG_MASK_CNF        = 0x105d, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_EVDO_REVA_RLINK_INFO_IND = 0x105e,
    ID_MTA_AT_EVDO_SIG_EXEVENT_IND     = 0x105f,

    /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_XCPOSR_SET_CNF = 0x1060,
    /* _H2ASN_MsgChoice MTA_AT_QryXcposrCnf */
    ID_MTA_AT_XCPOSR_QRY_CNF = 0x1061,
    /* _H2ASN_MsgChoice MTA_AT_ResultCnf */
    ID_MTA_AT_XCPOSRRPT_SET_CNF = 0x1062,
    /* _H2ASN_MsgChoice MTA_AT_QryXcposrrptCnf */
    ID_MTA_AT_XCPOSRRPT_QRY_CNF      = 0x1063,
    ID_MTA_AT_CLEAR_HISTORY_FREQ_CNF = 0x1064, /* _H2ASN_MsgChoice MTA_AT_ResultCnf */

    ID_MTA_AT_SET_SENSOR_CNF = 0x1065, /* _H2ASN_MsgChoice MTA_AT_SetSensorCnf */
    ID_MTA_AT_SET_SCREEN_CNF = 0x1066, /* _H2ASN_MsgChoice MTA_AT_SetScreenCnf */
    /* _H2ASN_MsgChoice MTA_AT_FratIgnitionQryCnf */
    ID_MTA_AT_FRAT_IGNITION_QRY_CNF = 0x1067,
    /* _H2ASN_MsgChoice MTA_AT_FratIgnitionSetCnf */
    ID_MTA_AT_FRAT_IGNITION_SET_CNF = 0x1068,

    ID_MTA_AT_SET_MODEM_TIME_CNF = 0x1069,

    ID_MTA_AT_RX_TEST_MODE_SET_CNF = 0x106A,

    ID_MTA_AT_AFC_CLK_UNLOCK_CAUSE_IND = 0x106B,

    ID_MTA_AT_SIB16_TIME_UPDATE_IND = 0x106C,

    ID_MTA_AT_SLAVE_SET_CNF = 0x106D,

    ID_MTA_AT_BESTFREQ_SET_CNF = 0x106E,
    /* _H2ASN_MsgChoice MTA_AT_BEST_FREQ_CASE_STRU */
    ID_MTA_AT_BEST_FREQ_INFO_IND = 0x106F,
    ID_MTA_AT_BESTFREQ_QRY_CNF   = 0x1070,

    ID_MTA_AT_RFIC_DIE_ID_QRY_CNF = 0x1071,

    ID_MTA_AT_CRRCONN_SET_CNF     = 0x1072,
    ID_MTA_AT_CRRCONN_QRY_CNF     = 0x1073,
    ID_MTA_AT_CRRCONN_STATUS_IND  = 0x1074,
    ID_MTA_AT_VTRLQUALRPT_SET_CNF = 0x1075,
    ID_MTA_AT_RL_QUALITY_INFO_IND = 0x1076,
    ID_MTA_AT_VIDEO_DIAG_INFO_RPT = 0x1077,

    ID_MTA_AT_PMU_DIE_SN_QRY_CNF = 0x1078,

    ID_MTA_AT_MODEM_CAP_UPDATE_CNF = 0x1079,

    /* _H2ASN_MsgChoice MTA_AT_LteCategoryInfoInd */
    ID_MTA_AT_LTE_CATEGORY_INFO_IND = 0x107A,

    ID_MTA_AT_TAS_TEST_CFG_CNF = 0x1080,
    ID_MTA_AT_TAS_TEST_QRY_CNF = 0x1081,

    ID_MTA_AT_ACCESS_STRATUM_REL_IND = 0x1082,

    ID_MTA_AT_MIPI_WREX_CNF = 0x1083,
    ID_MTA_AT_MIPI_RDEX_CNF = 0x1084,

    ID_MTA_AT_RS_INFO_QRY_CNF = 0x1085,

    ID_MTA_AT_PHY_COM_CFG_SET_CNF = 0x1086,

    ID_MTA_AT_ERRCCAP_CFG_SET_CNF = 0x1087,
    ID_MTA_AT_ERRCCAP_QRY_SET_CNF = 0x1088,

    /* _H2ASN_MsgChoice MTA_AT_QryGsmFreqlockCnf */
    ID_MTA_AT_QRY_GSM_FREQLOCK_CNF = 0x1089,
    ID_MTA_AT_DEL_CELLENTITY_CNF   = 0x108A,

    ID_MTA_AT_POWER_DET_QRY_CNF = 0x108B,

    ID_MTA_AT_PSEUCELL_INFO_SET_CNF = 0x108C,

    ID_MTA_AT_MIPIREAD_SET_CNF = 0x108D,

    ID_MTA_AT_NO_CARD_MODE_SET_CNF = 0x108E,
    ID_MTA_AT_NO_CARD_MODE_QRY_CNF = 0x108F,

    ID_MTA_AT_CHRALARMRLAT_CFG_SET_CNF = 0x1090,

    ID_MTA_AT_ECC_CFG_SET_CNF = 0x1091,
    ID_MTA_AT_ECC_STATUS_IND  = 0x1092,

    ID_MTA_AT_EPDU_SET_CNF   = 0x1093,
    ID_MTA_AT_EPDUR_DATA_IND = 0x1094,

    ID_MTA_AT_LTE_CA_CFG_SET_CNF     = 0x1095,
    ID_MTA_AT_LTE_CA_CELLEX_QRY_CNF  = 0x1096,
    ID_MTA_AT_LTE_CA_CELLEX_INFO_NTF = 0x1097,

    ID_MTA_AT_LTE_CA_CELL_RPT_CFG_SET_CNF = 0x1098,
    ID_MTA_AT_LTE_CA_CELL_RPT_CFG_QRY_CNF = 0x1099,

    ID_MTA_AT_FINE_TIME_SET_CNF = 0x109A,
    ID_MTA_AT_SIB_FINE_TIME_NTF = 0x109B,
    ID_MTA_AT_LPP_FINE_TIME_NTF = 0x109C,

    ID_MTA_AT_PHY_COMM_ACK_IND = 0x109D,

    /* _H2ASN_MsgChoice MTA_AT_GasAutotestQryCnf */
    ID_MTA_AT_GAS_AUTOTEST_QRY_CNF = 0x109E,

    ID_MTA_AT_COMM_BOOSTER_SET_CNF   = 0x109F, /* _H2ASN_MsgChoice MTA_AT_CommBoosterSetCnf */
    ID_MTA_AT_COMM_BOOSTER_IND       = 0x10A0, /* _H2ASN_MsgChoice MTA_AT_CommBoosterInd */
    ID_MTA_AT_COMM_BOOSTER_QUERY_CNF = 0x10A1, /* _H2ASN_MsgChoice MTA_AT_CommBoosterQueryCnf */

    ID_MTA_AT_NVLOAD_SET_CNF = 0x10A2,

    ID_MTA_AT_SMS_DOMAIN_SET_CNF = 0x10A3, /* _H2ASN_MsgChoice MTA_AT_SmsDomainSetCnf */
    ID_MTA_AT_SMS_DOMAIN_QRY_CNF = 0x10A4, /* _H2ASN_MsgChoice MTA_AT_SmsDomainQryCnf */

    /* _H2ASN_MsgChoice MTA_AT_EpsNetworkQryCnf */
    ID_MTA_AT_EPS_NETWORK_QRY_CNF = 0x10A5,
    /* _H2ASN_MsgChoice MTA_AT_EPS_NETWORK_INFO_IND_STRU */
    ID_MTA_AT_EPS_NETWORK_INFO_IND = 0x10A6,

    ID_MTA_AT_SET_SAMPLE_CNF = 0x10A7,

    ID_MTA_AT_GPS_LOCSET_SET_CNF = 0x10A8,

    ID_MTA_AT_CCLK_QRY_CNF = 0x10A9,

    ID_MTA_AT_TEMP_PROTECT_IND = 0x10AA,

    ID_AT_MTA_GAME_MODE_SET_CNF = 0x10AB, /* _H2ASN_MsgChoice MTA_AT_GameModeSetCfn */

    ID_MTA_AT_PSEUDBTS_SET_CNF = 0x10AC,

    ID_MTA_AT_SUBCLFSPARAM_SET_CNF = 0X10AD,
    ID_MTA_AT_SUBCLFSPARAM_QRY_CNF = 0X10AE,

    ID_MTA_AT_NV_REFRESH_SET_CNF = 0x10AF, /* _H2ASN_MsgChoice MTA_AT_NvRefreshSetCnf */

#if (FEATURE_ON == FEATURE_PHONE_ENG_AT_CMD)
    ID_AT_MTA_SLT_TEST_SET_CNF = 0x10B0,
#endif

    ID_MTA_AT_CDMA_MODEM_CAP_SET_CNF = 0x10B1,

    ID_MTA_AT_CDMA_CAP_RESUME_SET_CNF = 0x10B2,

    ID_MTA_AT_PHYMIPIWRITE_SET_CNF = 0x10B3,

    ID_MTA_AT_SFEATURE_QRY_CNF     = 0x10B4,
    ID_MTA_AT_LOW_PWR_MODE_SET_CNF = 0x10B5,

    ID_MTA_AT_LL2_COM_CFG_SET_CNF = 0x10B8,
    ID_MTA_AT_LL2_COM_CFG_QRY_CNF = 0x10B9,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_NL2_COM_CFG_SET_CNF = 0x10BA,
    ID_MTA_AT_NL2_COM_CFG_QRY_CNF = 0x10BB,

    ID_MTA_AT_NRRCCAP_CFG_SET_CNF        = 0x10BC,
    ID_MTA_AT_NRRCCAP_QRY_CNF            = 0x10BD,
    ID_MTA_AT_NRPWRCTRL_SET_CNF          = 0x10BE,
    ID_MTA_AT_NPDCP_SLEEP_TRHRES_CFG_CNF = 0x10BF,
    ID_MTA_AT_NR_NW_CAP_INFO_REPORT_IND      = 0x10C4,
    ID_MTA_AT_NR_NW_CAP_INFO_RPT_CFG_SET_CNF = 0x10C5,
#endif

    ID_AT_MTA_QRY_JAM_DETECT_REQ = 0x1100,
    ID_MTA_AT_QRY_JAM_DETECT_CNF = 0x1101,

    ID_MTA_AT_PSEUD_BTS_IDENT_IND = 0x1102,

    ID_MTA_AT_FORCESYNC_SET_CNF = 0X1103,

    ID_AT_MTA_SET_LTEPWRDISS_CNF = 0X1104,

    ID_AT_MTA_DCXO_SAMPLE_QRY_REQ = 0X1105,
    ID_MTA_AT_DCXO_SAMPLE_QRY_CNF = 0X1106,

    ID_AT_MTA_MCS_SET_REQ     = 0X1107,
    ID_MTA_AT_MCS_SET_CNF     = 0X1108,
    ID_AT_MTA_TXPOWER_QRY_REQ = 0X1109,
    ID_MTA_AT_TXPOWER_QRY_CNF = 0X110A,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_AT_MTA_NTXPOWER_QRY_REQ = 0X110B,
    ID_MTA_AT_NTXPOWER_QRY_CNF = 0X110C,
#endif
    ID_AT_MTA_HFREQINFO_QRY_REQ = 0X110D,
    ID_MTA_AT_HFREQINFO_QRY_CNF = 0X110E,
    ID_MTA_AT_HFREQINFO_IND     = 0X110F,

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_SET_TRX_TAS_CNF = 0X1110,
    ID_MTA_AT_QRY_TRX_TAS_CNF = 0X1111,

#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_5G_OPTION_SET_CNF = 0X1112,
    ID_MTA_AT_5G_OPTION_QRY_CNF = 0X1113,
    ID_MTA_AT_5G_NSSAI_SET_CNF  = 0X1114,
    ID_MTA_AT_5G_NSSAI_QRY_CNF  = 0X1115,
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_SET_NETMON_SSCELL_CNF = 0x1118,
#endif
    ID_MTA_AT_RFFE_DIE_ID_QRY_CNF       = 0x1119,
    ID_MTA_AT_OVERHEATING_CFG_SET_CNF   = 0x111A,
    ID_MTA_AT_OVERHEATING_CFG_QRY_CNF   = 0x111B,
    ID_MTA_AT_LRRC_UE_CAP_PARA_INFO_NTF = 0x111C,
    ID_MTA_AT_LRRC_UE_CAP_PARA_SET_CNF  = 0x111D,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_NRRC_UE_CAP_PARA_SET_CNF  = 0x111E,
    ID_MTA_AT_NRRC_UE_CAP_PARA_INFO_NTF = 0x111F,
#endif

    ID_MTA_AT_UART_TEST_CNF      = 0x1120,
    ID_MTA_AT_UART_TEST_RSLT_IND = 0X1121,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_SET_LADN_RPT_CNF  = 0x1122,
    ID_MTA_AT_QRY_LADN_INFO_CNF = 0x1123,
    ID_MTA_AT_LADN_INFO_IND     = 0x1124,
#endif
    ID_MTA_AT_HSRCELLINFO_IND = 0x1125,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    ID_MTA_AT_NR_SSB_ID_QRY_CNF = 0x1126,
#endif

#if (FEATURE_LTEV_WL == FEATURE_ON)
    ID_MTA_AT_FILE_WRITE_CNF = 0x1130,
    ID_MTA_AT_FILE_READ_CNF  = 0x1131,
    ID_AT_MTA_FILE_WRITE_REQ = 0x1132,
    ID_AT_MTA_FILE_READ_REQ  = 0x1133,
#endif
    ID_AT_MTA_LTE_CONN_RECOVERY_NTF = 0x1134,

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    ID_AT_MTA_NRPHY_COM_CFG_SET_REQ = 0x1135,
    ID_MTA_AT_NRPHY_COM_CFG_SET_CNF = 0x1136,
#endif
    /* 最后一条消息 */
    ID_AT_MTA_MSG_TYPE_BUTT

};
typedef VOS_UINT32 AT_MTA_MsgTypeUint32;


enum AT_SetSltTestType {
    AT_SET_SGMII_TEST      = 1, /* SGMII数模接口功能测试 */
    AT_SET_RGMII_TEST      = 2, /* RGMII数模接口功能测试 */
    AT_SET_PCIE_TEST       = 3, /* PCIe模拟PHY接口连接测试 */
    AT_SET_SPI_TEST        = 4, /* SPI master和slave对接环回测试 */
    AT_SET_SDCARD_TEST     = 5, /* MMC0接口及SD卡读写测试 */
    AT_SET_L2CACHE_TEST    = 6, /* L2CACHE测试 */
    AT_SET_HIFI_TEST       = 7, /* HIFI测试 */
    AT_SET_SLT_SERDES_TEST = 8, /* SERDES 测试 */
    AT_SET_SLT_ALINK_TEST  = 9, /* alink 测试 */
    AT_SET_SLT_TEST_BUTT
};
typedef VOS_UINT8 AT_SetSltTestTypeUint8;


enum MTA_AT_Result {
    /* 与AT模块对应的标准错误码 */
    MTA_AT_RESULT_NO_ERROR = 0x000000, /* 消息处理正常 */
    MTA_AT_RESULT_ERROR,               /* 消息处理出错 */
    MTA_AT_RESULT_INCORRECT_PARAMETERS,
    MTA_AT_RESULT_OPTION_TIMEOUT,
    MTA_AT_RESULT_OPERATION_NOT_ALLOWED, /* 当前操作不可用 */
    MTA_AT_RESULT_FUNC_DISABLE,          /* 消息处理功能关闭 */
    /* 预留对应AT标准命令错误码 */

    /* 装备命令特有错误码 */
    MTA_AT_RESULT_DEVICE_ERROR_BASE = 0x100000,
    MTA_AT_RESULT_DEVICE_SEC_IDENTIFY_FAIL,         /* 产线鉴权失败 */
    MTA_AT_RESULT_DEVICE_SEC_SIGNATURE_FAIL,        /* 签名校验失败 */
    MTA_AT_RESULT_DEVICE_SEC_DK_INCORRECT,          /* 端口密码错误 */
    MTA_AT_RESULT_DEVICE_SEC_UNLOCK_KEY_INCORRECT,  /* 解锁密码错误 */
    MTA_AT_RESULT_DEVICE_SEC_PH_PHYNUM_LEN_ERROR,   /* 物理号长度错误 */
    MTA_AT_RESULT_DEVICE_SEC_PH_PHYNUM_VALUE_ERROR, /* 物理号码错误 */
    MTA_AT_RESULT_DEVICE_SEC_PH_PHYNUM_TYPE_ERROR,  /* 物理号类型错误 */
    MTA_AT_RESULT_DEVICE_SEC_RSA_ENCRYPT_FAIL,      /* RSA加密失败 */
    MTA_AT_RESULT_DEVICE_SEC_RSA_DECRYPT_FAIL,      /* RSA解密失败 */
    MTA_AT_RESULT_DEVICE_SEC_GET_RAND_NUMBER_FAIL,  /* 获取随机数失败(crypto_rand) */
    MTA_AT_RESULT_DEVICE_SEC_WRITE_HUK_FAIL,        /* HUK写入错误 */
    MTA_AT_RESULT_DEVICE_SEC_FLASH_ERROR,           /* Flash错误 */
    MTA_AT_RESULT_DEVICE_SEC_NV_ERROR,              /* NV读写错误 */
    MTA_AT_RESULT_DEVICE_SEC_OTHER_ERROR,           /* 其它错误 */

    /* 私有命令特有错误码 */
    MTA_AT_RESULT_PRICMD_ERROR_BASE = 0x200000,

    MTA_AT_RESULT_BUTT
};
typedef VOS_UINT32 MTA_AT_ResultUint32;


enum MTA_AT_ResultType {
    MTA_AT_RESULT_TYPE_NULL = 0x00,
    MTA_AT_RESULT_TYPE_LTE  = 0x01,
    MTA_AT_RESULT_TYPE_NR   = 0x02,
    MTA_AT_RESULT_TYPE_DC   = 0x03,
    MTA_AT_RESULT_TYPE_BUTT
};
typedef VOS_UINT8 MTA_AT_ResultTypeUint8;

enum MTA_AT_Cgpsclock {
    MTA_AT_CGPSCLOCK_STOP = 0,
    MTA_AT_CGPSCLOCK_START,
    MTA_AT_CGPSCLOCK_BUTT
};
typedef VOS_UINT32 MTA_AT_CgpsclockUint32;


enum MTA_AT_CposOperateType {
    MTA_AT_CPOS_SEND = 0,
    MTA_AT_CPOS_CANCEL,
    MTA_AT_CPOS_BUTT
};
typedef VOS_UINT32 MTA_AT_CposOperateTypeUint32;


enum AT_MTA_PersCategory {
    AT_MTA_PERS_CATEGORY_NETWORK          = 0x00, /* Category: 锁网 */
    AT_MTA_PERS_CATEGORY_NETWORK_SUBSET   = 0x01, /* Category: 锁子网 */
    AT_MTA_PERS_CATEGORY_SERVICE_PROVIDER = 0x02, /* Category: 锁SP */
    AT_MTA_PERS_CATEGORY_CORPORATE        = 0x03, /* Category: 锁团体 */
    AT_MTA_PERS_CATEGORY_SIM_USIM         = 0x04, /* Category: 锁(U)SIM卡 */

    AT_MTA_PERS_CATEGORY_BUTT
};
typedef VOS_UINT8 AT_MTA_PersCategoryUint8;


enum AT_MTA_RptGeneralCtrlType {
    AT_MTA_RPT_GENERAL_CONTROL_NO_REPORT = 0x00, /* 禁止所有的主动上报 */
    AT_MTA_RPT_GENERAL_CONTROL_REPORT    = 0x01, /* 打开所有的主动上报 */
    AT_MTA_RPT_GENERAL_CONTROL_CUSTOM    = 0x02, /* 按BIT位控制对应命令的主动上报 */

    AT_MTA_RPT_GENERAL_CONTROL_BUTT
};
typedef VOS_UINT8 AT_MTA_RptGeneralCtrlTypeUint8;


enum AT_MTA_RptSetType {
    AT_MTA_SET_MODE_RPT_TYPE = 0, /* Category: 设置模式变化是否主动上报 */
    AT_MTA_SET_SRVST_RPT_TYPE,
    AT_MTA_SET_RSSI_RPT_TYPE,
    AT_MTA_SET_TIME_RPT_TYPE,
    AT_MTA_SET_CTZR_RPT_TYPE,
    AT_MTA_SET_SIMST_RPT_TYPE,
    AT_MTA_SET_CREG_RPT_TYPE,
    AT_MTA_SET_CGREG_RPT_TYPE,
    AT_MTA_SET_CEREG_RPT_TYPE,
    AT_MTA_SET_CSID_RPT_TYPE,
    AT_MTA_SET_CLOCINFO_RPT_TYPE,
    AT_MTA_SET_MTREATTACH_RPT_TYPE,
    AT_MTA_SET_CENFS_RPT_TYPE,
    AT_MTA_SET_C5GREG_RPT_TYPE,

    AT_MTA_SET_LENDC_RPT_TYPE,

    AT_MTA_SET_RRCSTAT_RPT_TYPE,
    AT_MTA_SET_HFREQINFO_RPT_TYPE,

    AT_MTA_SET_RPT_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_RptSetTypeUint32;


enum AT_MTA_RptQryType {
    AT_MTA_QRY_TIME_RPT_TYPE = 0,
    AT_MTA_QRY_CTZR_RPT_TYPE,
    AT_MTA_QRY_CSSN_RPT_TYPE,
    AT_MTA_QRY_CUSD_RPT_TYPE,
    AT_MTA_QRY_RPT_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_RptQryTypeUint32;


enum AT_MTA_CmdRptFlg {
    AT_MTA_CMD_RPT_FLG_OFF = 0, /* AT命令不主动上报 */
    AT_MTA_CMD_RPT_FLG_ON,      /* AT命令主动上报 */
    AT_MTA_CMD_RPT_FLG_BUTT
};
typedef VOS_UINT8 AT_MTA_CmdRptFlgUint8;

enum AT_MTA_ClearFreqFlg {
    AT_MTA_CLEAR_FREQ_FLG_NOT_CSG_HISTORY_FREQ = 0, /* 清除非CSG历史频点 */
    AT_MTA_CLEAR_FREQ_FLG_CSG_HISTORY_FREQ,         /* 清除CSG历史频点 */
    AT_MTA_CLEAR_FREQ_FLG_ALL_FREQ,                 /* 清除所有历史频点 */
    AT_MTA_CLEAR_FREQ_FLG_BUTT
};
typedef VOS_UINT8 AT_MTA_ClearFreqFlgUint8;


enum AT_MTA_CmdRatmode {
    AT_MTA_CMD_RATMODE_GSM = 0,
    AT_MTA_CMD_RATMODE_WCDMA,
    AT_MTA_CMD_RATMODE_LTE,
    AT_MTA_CMD_RATMODE_TD,
    AT_MTA_CMD_RATMODE_CDMA,
    AT_MTA_CMD_RATMODE_NR,
    AT_MTA_CMD_RATMODE_BUTT
};
typedef VOS_UINT8 AT_MTA_CmdRatmodeUint8;


enum AT_MTA_CmdSignaling {
    AT_MTA_CMD_SIGNALING_MODE    = 0, /* 信令模式 */
    AT_MTA_CMD_NONSIGNALING_MODE = 1, /* 非信令模式 */

    AT_MTA_CMD_NONSIGNALING_BUTT
};
typedef VOS_UINT8 AT_MTA_CmdSignalingUint8;


enum AT_MTA_TrxTasCmd {
    AT_MTA_TRX_TAS_CMD_CLOSE_ALGORITHM = 0, /* 关闭算法 */
    AT_MTA_TRX_TAS_CMD_SET_PARA        = 1, /* 设置参数 */
    AT_MTA_TRX_TAS_CMD_OPEN_ALGORITHM  = 2, /* 打开算法 */

    AT_MTA_TRX_TAS_CMD_BUTT
};
typedef VOS_UINT8 AT_MTA_TrxTasCmdUint8;


enum MTA_AT_JamResult {
    MTA_AT_JAM_RESULT_JAM_DISAPPEARED = 0x00, /* 状态正常无干扰 */
    MTA_AT_JAM_RESULT_JAM_DISCOVERED,         /* 干扰存在 */

    MTA_AT_JAM_RESULT_BUTT
};
typedef VOS_UINT32 MTA_AT_JamResultUint32;


enum AT_MTA_FreqlockRatmode {
    AT_MTA_FREQLOCK_RATMODE_GSM = 0x01,
    AT_MTA_FREQLOCK_RATMODE_WCDMA,
    AT_MTA_FREQLOCK_RATMODE_LTE,
    AT_MTA_FREQLOCK_RATMODE_CDMA1X,
    AT_MTA_FREQLOCK_RATMODE_TD,
    AT_MTA_FREQLOCK_RATMODE_WIMAX,
    AT_MTA_FREQLOCK_RATMODE_EVDO,

    AT_MTA_FREQLOCK_RATMODE_BUTT
};
typedef VOS_UINT8 AT_MTA_FreqlockRatmodeUint8;


enum AT_MTA_GsmBand {
    AT_MTA_GSM_BAND_850 = 0x00,
    AT_MTA_GSM_BAND_900,
    AT_MTA_GSM_BAND_1800,
    AT_MTA_GSM_BAND_1900,

    AT_MTA_GSM_BAND_BUTT
};
typedef VOS_UINT16 AT_MTA_GsmBandUint16;


enum AT_MTA_Cfg {
    AT_MTA_CFG_DISABLE = 0,
    AT_MTA_CFG_ENABLE  = 1,
    AT_MTA_CFG_BUTT
};
typedef VOS_UINT8 AT_MTA_CfgUint8;


enum AT_MTA_MbmsServiceStateSet {
    AT_MTA_MBMS_SERVICE_STATE_SET_ACTIVE       = 0,
    AT_MTA_MBMS_SERVICE_STATE_SET_DEACTIVE     = 1,
    AT_MTA_MBMS_SERVICE_STATE_SET_DEACTIVE_ALL = 2,
    AT_MTA_MBMS_SERVICE_STATE_SET_BUTT
};
typedef VOS_UINT8 AT_MTA_MbmsServiceStateSetUint8;


enum AT_MTA_MbmsCastMode {
    AT_MTA_MBMS_CAST_MODE_UNICAST   = 0,
    AT_MTA_MBMS_CAST_MODE_MULTICAST = 1,
    AT_MTA_MBMS_CAST_MODE_BUTT
};
typedef VOS_UINT8 AT_MTA_MbmsCastModeUint8;


enum MTA_AT_EmbmsFuntionalityStatus {
    MTA_AT_EMBMS_FUNTIONALITY_OFF = 0,
    MTA_AT_EMBMS_FUNTIONALITY_ON  = 1,
    MTA_AT_EMBMS_FUNTIONALITY_STATUS_BUTT
};
typedef VOS_UINT8 MTA_AT_EmbmsFuntionalityStatusUint8;


enum MTA_AT_MbmsServiceEvent {
    MTA_AT_MBMS_SERVICE_EVENT_SERVICE_CHANGE    = 0,
    MTA_AT_MBMS_SERVICE_EVENT_NO_SERVICE        = 1,
    MTA_AT_MBMS_SERVICE_EVENT_ONLY_UNICAST      = 2,
    MTA_AT_MBMS_SERVICE_EVENT_SERVICE_AVAILABLE = 3,
    MTA_AT_MBMS_SERVICE_EVENT_BUTT
};
typedef VOS_UINT8 MTA_AT_MbmsServiceEventUint8;


enum AT_MTA_CoexBwType {
    AT_MTA_COEX_BAND_WIDTH_6RB   = 0, /* 宽带1.4M */
    AT_MTA_COEX_BAND_WIDTH_15RB  = 1, /* 宽带3M */
    AT_MTA_COEX_BAND_WIDTH_25RB  = 2, /* 宽带5M */
    AT_MTA_COEX_BAND_WIDTH_50RB  = 3, /* 宽带10M */
    AT_MTA_COEX_BAND_WIDTH_75RB  = 4, /* 宽带15M */
    AT_MTA_COEX_BAND_WIDTH_100RB = 5, /* 宽带20M */
    AT_MTA_COEX_BAND_WIDTH_BUTT
};
typedef VOS_UINT16 AT_MTA_CoexBwTypeUint16;


enum AT_MTA_CoexCfg {
    AT_MTA_COEX_CFG_DISABLE = 0,
    AT_MTA_COEX_CFG_ENABLE  = 1,
    AT_MTA_COEX_CFG_BUTT
};
typedef VOS_UINT16 AT_MTA_CoexCfgUint16;


enum AT_MTA_LteLowPower {
    AT_MTA_LTE_LOW_POWER_NORMAL = 0,
    AT_MTA_LTE_LOW_POWER_LOW    = 1,
    AT_MTA_LTE_LOW_POWER_BUTT
};
typedef VOS_UINT8 AT_MTA_LteLowPowerUint8;


enum AT_MTA_MbmsPriority {
    AT_MTA_MBMS_PRIORITY_UNICAST = 0,
    AT_MTA_MBMS_PRIORITY_MBMS    = 1,
    AT_MTA_MBMS_PRIORITY_BUTT
};
typedef VOS_UINT8 AT_MTA_MbmsPriorityUint8;

enum AT_MTA_UeCenterType {
    AT_MTA_UE_CENTER_VOICE_CENTRIC = 0,
    AT_MTA_UE_CENTER_DATA_CENTRIC  = 1,
    AT_MTA_UE_CENTER_BUTT
};
typedef VOS_UINT32 AT_MTA_UeCenterTypeUint32;


enum AT_MTA_BodySarState {
    AT_MTA_BODY_SAR_OFF = 0, /* Body SAR功能关闭 */
    AT_MTA_BODY_SAR_ON,      /* Body SAR功能开启 */
    AT_MTA_BODY_SAR_STATE_BUTT
};
typedef VOS_UINT16 AT_MTA_BodySarStateUint16;


enum MTA_AT_RefclockStatus {
    MTA_AT_REFCLOCK_UNLOCKED = 0, /* GPS参考时钟频率非锁定状态 */
    MTA_AT_REFCLOCK_LOCKED,       /* GPS参考时钟频率锁定状态 */
    MTA_AT_REFCLOCK_STATUS_BUTT
};
typedef VOS_UINT16 MTA_AT_RefclockStatusUint16;


enum MTA_AT_GphyXpassMode {
    MTA_AT_GPHY_XPASS_MODE_DISABLE = 0, /* 非XPASS模式，类似高铁 */
    MTA_AT_GPHY_XPASS_MODE_ENABLE,      /* XPASS模式，类似高铁 */
    MTA_AT_GPHY_XPASS_MODE_BUTT
};
typedef VOS_UINT16 MTA_AT_GphyXpassModeUint16;


enum MTA_AT_WphyHighwayMode {
    MTA_AT_WPHY_HIGHWAY_MODE_DISABLE = 0, /* W非高铁模式 */
    MTA_AT_WPHY_HIGHWAY_MODE_ENABLE,      /* W高铁模式 */
    MTA_AT_WPHY_HIGHWAY_MODE_BUTT
};
typedef VOS_UINT16 MTA_AT_WphyHighwayModeUint16;


enum MTA_AT_NetmonGsmState {
    MTA_AT_NETMON_GSM_STATE_INIT            = 0,
    MTA_AT_NETMON_GSM_STATTE_WAIT_CELL_INFO = 1,
    MTA_AT_NETMON_GSM_STATTE_WAIT_TA        = 2,
    MTA_AT_NETMON_GSM_STATTE_BUTT
};

typedef VOS_UINT32 MTA_AT_NetmonGsmStateUint32;



enum MTA_AT_NetmonCellInfoRat {
    MTA_AT_NETMON_CELL_INFO_GSM       = 0,
    MTA_AT_NETMON_CELL_INFO_UTRAN_FDD = 1,
    MTA_AT_NETMON_CELL_INFO_UTRAN_TDD = 2,
    MTA_AT_NETMON_CELL_INFO_LTE       = 3,
    MTA_AT_NETMON_CELL_INFO_NR        = 4,
    MTA_AT_NETMON_CELL_INFO_RAT_BUTT
};
typedef VOS_UINT32 MTA_AT_NetmonCellInfoRatUint32;



enum MTA_TAF_CTZR_TYPE {
    MTA_TAF_CTZR_OFF  = 0x00, /* CTZR去使能 */
    MTA_TAF_CTZR_CTZV = 0x01, /* CTZR:CTZV */
    MTA_TAF_CTZR_CTZE = 0x02, /* CTZR:CTZE */
    MTA_TAF_CTZR_BUTT
};
typedef VOS_UINT32 MTA_TAF_CtzrTypeUint32;


enum AT_MTA_AfcClkStatus {
    AT_MTA_AFC_CLK_UNLOCKED = 0, /* AFC UNLOCKED */
    AT_MTA_AFC_CLK_LOCKED,       /* AFC LOCKED */

    AT_MTA_AFC_CLK_STATUS_BUTT
};
typedef VOS_UINT32 AT_MTA_AfcClkStatusUint32;


enum AT_MTA_XcposrCfg {
    AT_MTA_XCPOSR_CFG_DISABLE = 0, /* 不支持清除缓存 */
    AT_MTA_XCPOSR_CFG_ENABLE,      /* 支持清除缓存 */
    AT_MTA_XCPOSR_CFG_BUTT
};
typedef VOS_UINT8 AT_MTA_XcposrCfgUnit8;


enum AT_MTA_FratIgnition {
    AT_MTA_FRAT_IGNITION_STATT_OFF = 0, /* 不支持清除缓存 */
    AT_MTA_FRAT_IGNITION_STATT_ON,      /* 支持清除缓存 */
    AT_MTA_FRAT_IGNITION_STATT_BUTT
};
typedef VOS_UINT8 AT_MTA_FratIgnitionUnit8;


enum MTA_AT_AfcClkUnlockCause {
    MTA_AT_AFC_CLK_UNLOCK_CAUSE_SLEEP,

    MTA_AT_AFC_CLK_UNLOCK_CAUSE_BUTT
};
typedef VOS_UINT16 MTA_AT_AfcClkUnlockCauseUint16;


enum MTA_AT_ModemId {
    MTA_AT_MODEM_ID_0 = 0,
    MTA_AT_MODEM_ID_1,
    MTA_AT_MODEM_ID_2,

    MTA_AT_MODEM_ID_BUTT
};
typedef VOS_UINT8 MTA_AT_ModemIdUint8;


enum MTA_AT_RatMode {
    MTA_AT_RAT_MODE_GSM     = 0,
    MTA_AT_RAT_MODE_WCDMA   = 1,
    MTA_AT_RAT_MODE_LTE     = 2,
    MTA_AT_RAT_MODE_TDSCDMA = 3,
    MTA_AT_RAT_MODE_CDMA_1X = 4,
    MTA_AT_RAT_MODE_HRPD    = 5,
    MTA_AT_RAT_MODE_NR      = 6,
    MTA_AT_RAT_MODE_BUTT
};
typedef VOS_UINT8 MTA_AT_RatModeUint8;


enum AT_MTA_RatModeEnum {
    AT_MTA_RAT_MODE_LTE = 0,
    AT_MTA_RAT_MODE_NR  = 1,
    AT_MTA_RAT_MODE_BUTT
};
typedef VOS_UINT8 AT_MTA_RatModeUint8;


enum MTA_AT_DayLightSavingInd {
    MTA_AT_NO_ADJUST = 0,
    MTA_AT_ONE_HOUR  = 1,
    MTA_AT_TWO_HOUR  = 2,
    MTA_AT_HOUR_BUTT
};
typedef VOS_UINT8 MTA_AT_DayLightSavingIndUint8;


enum MTA_AT_TimeType {
    MTA_AT_LOCAL_TIME     = 1,
    MTA_AT_GPS_TIME       = 2,
    MTA_AT_LOCAL_GPS_TIME = 3,
    MTA_AT_TIME_BUTT
};
typedef VOS_UINT8 MTA_AT_TimeTypeUint8;


enum AT_MTA_ModemCapUpdateType {
    AT_MTA_MODEM_CAP_UPDATE_TYPE_CDMA_MODEM_SWITCH = 0x00U, /* AT^CDMAMODEMSWITCH导致的平台能力更新 */
    AT_MTA_MODEM_CAP_UPDATE_TYPE_ACTIVE_MODEM      = 0x01U, /* AT^ACTIVEMODEM导致的平台能力更新 */

    AT_MTA_MODEM_CAP_UPDATE_TYPE_BUTT
};
typedef VOS_UINT8 AT_MTA_ModemCapUpdateTypeUint8;


enum AT_MTA_RsInfoType {
    AT_MTA_RSRP_TYPE = 0,
    AT_MTA_RSRQ_TYPE = 1,
    AT_MTA_RS_INFO_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_RsInfoTypeUint32;


enum AT_MTA_PseucellNotifyType {
    AT_MTA_PSEUCELL_NOTIFY_TYPE_BY_MESSAGE = 0,
    AT_MTA_PSEUCELL_NOTIFY_TYPE_BUTT
};
typedef VOS_UINT8 AT_MTA_PseucellNotifyTypeUint8;


enum AT_MTA_SysMode {
    AT_MTA_SYS_MODE_GSM     = 1,
    AT_MTA_SYS_MODE_1X      = 2,
    AT_MTA_SYS_MODE_WCDMA   = 3,
    AT_MTA_SYS_MODE_TDSCDMA = 4,
    AT_MTA_SYS_MODE_WIMAX   = 5,
    AT_MTA_SYS_MODE_LTE     = 6,
    AT_MTA_SYS_MODE_HRPD    = 7,
    AT_MTA_SYS_MODE_BUTT
};
typedef VOS_UINT32 AT_MTA_SysModeUint32;


enum AT_MTA_ChrAlarmRlatOp {
    AT_MTA_CHR_ALARM_RLAT_OP_READ  = 0,
    AT_MTA_CHR_ALARM_RLAT_OP_WRITE = 1,

    AT_MTA_CHR_ALARM_RLAT_OP_BUTT
};
typedef VOS_UINT32 AT_MTA_ChrAlarmRlatOpUint32;


enum MTA_AT_BandWidth {
    MTA_AT_BAND_WIDTH_1DOT4M = 0, /* 带宽为1.4MHz */
    MTA_AT_BAND_WIDTH_3M,         /* 带宽为3MHz */
    MTA_AT_BAND_WIDTH_5M,         /* 带宽为5MHz */
    MTA_AT_BAND_WIDTH_10M,        /* 带宽为10MHz */
    MTA_AT_BAND_WIDTH_15M,        /* 带宽为15MHz */
    MTA_AT_BAND_WIDTH_20M,        /* 带宽为20MHz */
    MTA_AT_BAND_WIDTH_BUTT
};
typedef VOS_UINT8 MTA_AT_BandWidthUint8;


enum MTA_AT_FineTimeRat {
    MTA_AT_FINE_TIME_RAT_GSM      = 0,
    MTA_AT_FINE_TIME_RAT_WCDMA    = 1,
    MTA_AT_FINE_TIME_RAT_LTE      = 2,
    MTA_AT_FINE_TIME_RAT_TDS_CDMA = 3,
    MTA_AT_FINE_TIME_RAT_CDMA_1X  = 4,
    MTA_AT_FINE_TIME_RAT_HRPD     = 5,
    MTA_AT_FINE_TIME_RAT_NR       = 6,
    MTA_AT_FINE_TIME_RAT_BUTT
};
typedef VOS_UINT8 MTA_AT_FineTimeRatUint8;

/*
 * 协议表格:
 * ASN.1描述:
 * 结构说明: 当前的协议状态
 */
enum MTA_AT_ProtocolState {
    MTA_AT_PROTOCOL_STATE_IDLE      = 0,
    MTA_AT_PROTOCOL_STATE_CONNECTED = 1,
    MTA_AT_PROTOCOL_STATE_INACTIVE  = 2,

    MTA_AT_PROTOCOL_STATE_BUTT
};
typedef VOS_UINT8 MTA_AT_ProtocolStateUint8;

/*
 * 协议表格:
 * ASN.1描述:
 * 结构说明: FINETIME PARA
 */
enum AT_MTA_FinetimePara {
    AT_MTA_FINETIME_PARA_0 = 0,
    AT_MTA_FINETIME_PARA_1 = 1,
    AT_MTA_FINETIME_PARA_2 = 2,

    AT_MTA_FINETIME_PARA_BUTT
};
typedef VOS_UINT8 AT_MTA_FinetimeParaUint8;


enum AT_MTA_OverHeatingSetEnum {
    AT_MTA_OVERHEATING_SET  = 0,
    AT_MTA_OVERHEATING_QUIT = 1,

    AT_MTA_OVERHEATING_PARA_BUTT
};
typedef VOS_UINT8 AT_MTA_OoverheatingParaUint8;


enum TAF_MTA_NvCarrierOptResult {
    TAF_MTA_NV_CARRIER_OPT_NO_ERROR                 = 0x00000000,
    TAF_MTA_NV_CARRIER_OPT_INVALID_PARA             = 0x10100002, /* 无效的参数 */
    TAF_MTA_NV_CARRIER_OPT_NOT_FIND_FILE            = 0x10100006, /* 文件不存在 */
    TAF_MTA_NV_CARRIER_OPT_NOT_FIND_NV              = 0x10100016, /* 没有发现NV */
    TAF_MTA_NV_CARRIER_OPT_WRITE_NV_TIMEOUT         = 0x10100035, /* 写NV超时,(实际是写入文件系统超时) */
    TAF_MTA_NV_CARRIER_OPT_WRITE_NV_NOT_SUPPORT_ERR = 0x10100037, /* 不支持错误 */
    TAF_MTA_NV_CARRIER_OPT_MODEM_ID_ERROR           = 0x10100039, /* modem id 参数错误 */

    TAF_MTA_NV_CARRIER_OPT_BUTT
};
typedef VOS_UINT32 TAF_MTA_NvCarrierOptResultUint32;


enum AT_MTA_SmsDomain {
    AT_MTA_SMS_DOMAIN_NOT_USE_SMS_OVER_IP = 0, /* *< not to use SMS over ip */
    /* *< prefer to use SMS over ip, CS/PS SMS as secondary */
    AT_MTA_SMS_DOMAIN_PREFER_TO_USE_SMS_OVER_IP = 1,

    AT_MTA_SMS_DOMAIN_BUTT
};
typedef VOS_UINT8 AT_MTA_SmsDomainUint8;


enum MTA_AT_TimeInfoRptOpt {
    MTA_AT_TIME_INFO_RPT_OPT_MMINFO    = 0, /* ^TIME上报 */
    MTA_AT_TIME_INFO_RPT_OPT_SIB16TIME = 1, /* ^SIB16TIME上报 */
    MTA_AT_TIME_INFO_RPT_OPT_BUTT
};
typedef VOS_UINT8 MTA_AT_TimeInfoRptOptUint8;


enum AT_MTA_NvRefreshReason {
    AT_MTA_NV_REFRESH_USIM_DEPENDENT = 0, /* 随卡匹配 */
    AT_MTA_NV_REFRESH_BUTT
};
typedef VOS_UINT8 AT_MTA_NvRefreshReasonUint8;


enum AT_MTA_JamDetectMode {
    AT_MTA_JAM_DETECT_MODE_STOP   = 0, /* 关闭干扰检测 */
    AT_MTA_JAM_DETECT_MODE_START  = 1, /* 启动干扰检测 */
    AT_MTA_JAM_DETECT_MODE_UPDATE = 2, /* 更新干扰检测参数 */
    AT_MTA_JAM_DETECT_MODE_BUTT
};
typedef VOS_UINT8 AT_MTA_JamDetectModeUint8;


enum AT_MTA_DcxoSampleType {
    AT_MTA_DCXO_SAMPLE_H        = 0, /* 高温段 */
    AT_MTA_DCXO_SAMPLE_M        = 1, /* 中温段 */
    AT_MTA_DCXO_SAMPLE_L        = 2, /* 低温段 */
    AT_MTA_DCXO_SAMPLE_INITFREQ = 3, /* 产线校准数据 */
    AT_MTA_DCXO_SAMPLE_BUTT
};
typedef VOS_UINT16 AT_MTA_DcxoSampleTypeUint16;


enum MTA_AT_DcxoSampleResult {
    MTA_AT_DCXO_SAMPLE_RESULT_NO_ERROR       = 0, /* 结果正确 */
    MTA_AT_DCXO_SAMPLE_RESULT_ERROR          = 1, /* 结果错误 */
    MTA_AT_DCXO_SAMPLE_RESULT_PARA_ERROR     = 2, /* 参数错误 */
    MTA_AT_DCXO_SAMPLE_RESULT_OPTION_TIMEOUT = 3, /* 定时器超时 */
    MTA_AT_DCXO_SAMPLE_RESULT_BUTT
};
typedef VOS_UINT16 MTA_AT_DcxoSampleResultUint16;


enum AT_MTA_LowPwrModeRat {
    AT_MTA_LOW_PWR_MODE_RAT_LTE = 0, /* LTE */
    AT_MTA_LOW_PWR_MODE_RAT_NR  = 1, /* NR */

    AT_MTA_LOW_PWR_MODE_RAT_BUTT
};
typedef VOS_UINT8 AT_MTA_LowPwrModeRatUint8;


enum MTA_AT_UlMode {
    MTA_AT_UL_MODE_NULL = 0, /* 上行参数无效 */
    MTA_AT_UL_MODE_UL_ONLY,  /* 上行工作在UL模式 */
    MTA_AT_UL_MODE_SUL_ONLY, /* 上行工作在SUL模式 */
    MTA_AT_UL_MODE_BOTH,     /* 上行工作在并发模式 */
    MTA_AT_UL_MODE_BUTT
};
typedef VOS_UINT32 MTA_AT_UlModeUint32;


enum MTA_AT_IndexTable {
    /* 3GPP TS38.214 5.1.3.1 3张表 */
    MTA_AT_INDEX_TABLE_ONE = 0,
    MTA_AT_INDEX_TABLE_TWO,
    MTA_AT_INDEX_TABLE_THREE,
    /* 3GPP TS38.214 6.1.4.1 2张表 */
    MTA_AT_INDEX_TABLE_FOUR,
    MTA_AT_INDEX_TABLE_FIVE,
    MTA_AT_INDEX_TABLE_BUTT
};
typedef VOS_UINT8 MTA_AT_IndexTableUint8;


enum MTA_AT_NrScsTypeComm {
    MTA_AT_NR_SCS_TYPE_COMM_15  = 0x00,
    MTA_AT_NR_SCS_TYPE_COMM_30  = 0x01,
    MTA_AT_NR_SCS_TYPE_COMM_60  = 0x02,
    MTA_AT_NR_SCS_TYPE_COMM_120 = 0x03,
    MTA_AT_NR_SCS_TYPE_COMM_240 = 0x04,
    MTA_AT_NR_SCS_TYPE_COMM_BUTT
};
typedef VOS_UINT8 MTA_AT_NrScsTypeCommUint8;


enum AT_MTA_NssaiQryType {
    AT_MTA_NSSAI_QRY_DEF_CFG_NSSAI_ONLY    = 0x00, /* 只查询default config nssai */
    AT_MTA_NSSAI_QRY_DEF_CFG_AND_REJ_NSSAI = 0x01, /* 查询default config nssai && reject nssai */
    /* 查询default config nssai && reject nssai && Cfg nssai */
    AT_MTA_NSSAI_QRY_DEF_CFG_AND_REJ_AND_CFG_NSSAI = 0x02,
    /* 查询default config nssai && reject nssai && Cfg nssai && Allowed nssai */
    AT_MTA_NSSAI_QRY_DEF_CFG_AND_REJ_AND_CFG_AND_ALLOWED_NSSAI = 0x03,

    AT_MTA_NSSAI_QRY_PREFER_NSSAI = 0x04, /* 查询prefer nssai */

    AT_MTA_NSSAI_QRY_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_NssaiQryTypeUint32;


enum AT_MTA_NssaiSetType {
    AT_MTA_NSSAI_SET_PREFER_NSSAI  = 0x00, /* 设置prefer nssai信息 */
    AT_MTA_NSSAI_SET_DEF_CFG_NSSAI = 0x01, /* 设置default config nssai */

    AT_MTA_NSSAI_SET_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_NssaiSetTypeUint32;


enum MTA_AT_NetmonMeasRsType {
    MTA_AT_NETMON_MEAS_RS_TYPE_SSB = 0,
    MTA_AT_NETMON_MEAS_RS_TYPE_CSI_RS,
    MTA_AT_NETMON_MEAS_RS_TYPE_BUTT
};
typedef VOS_UINT8 MTA_AT_NetmonMeasRsTypeUint8;


enum MTA_AT_UartTestRslt {
    MTA_AT_UART_TEST_RSLT_SUCCESS      = 0x0000, /* 消息处理正常 */
    MTA_AT_UART_TEST_RSLT_DATA_ERROR   = 0x0001,
    MTA_AT_UART_TEST_RSLT_ICC_NOT_OPEN = 0x0002,
    MTA_AT_UART_TEST_RSLT_TIME_OUT     = 0x0003,

    MTA_AT_UART_TEST_RSLT_BUTT
};
typedef VOS_UINT32 MTA_AT_UartTestRsltUint32;


enum MTA_AT_HsrcellInfoRat {
    MTA_AT_HSRCELLINFO_RAT_GSM     = 0,
    MTA_AT_HSRCELLINFO_RAT_WCDMA   = 1,
    MTA_AT_HSRCELLINFO_RAT_LTE     = 2,
    MTA_AT_HSRCELLINFO_RAT_TD_CDMA = 3,
    MTA_AT_HSRCELLINFO_RAT_1X      = 4,
    MTA_AT_HSRCELLINFO_RAT_NR      = 5,
    MTA_AT_HSRCELLINFO_RAT_UNKNOWN = 6,

    MTA_AT_HSRCELLINFO_RAT_BUTT
};
typedef VOS_UINT8 MTA_AT_HsrcellInfoRatUint8;

/*
 * 5 消息头定义
 * *s
 */


typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgId; /* 消息名 */
    AT_APPCTRL appCtrl;
    VOS_UINT8  content[4]; /* 消息内容 */
} AT_MTA_Msg;


typedef struct {
    MTA_AT_CposOperateTypeUint32 cposOpType;
    VOS_UINT32                   xmlLength;
    VOS_CHAR                     xmlText[MTA_CPOS_XML_MAX_LEN];
} AT_MTA_CposReq;


typedef struct {
    AT_MTA_LowPwrModeRatUint8 rat;
    VOS_UINT8                 rcv[3];
} AT_MTA_LowPwrModeReq;


typedef struct {
    VOS_UINT32 result;
} MTA_AT_LowPwrModeCnf;


typedef struct {
    MTA_AT_ResultUint32 result; /* 命令执行结果 */
} MTA_AT_CposCnf;


typedef struct {
    /* AT向MTA上报的XML码流最大长度为1024，同时还需预留一个字节存放字符串结尾的空字符 */
    VOS_CHAR  xmlText[MTA_CPOSR_XML_MAX_LEN + 1];
    VOS_UINT8 rsv[3];
} MTA_AT_CposrInd;

typedef struct {
    VOS_UINT16 transactionId;
    VOS_UINT8  msgBodyType;
    VOS_UINT8  commonIeValidFlg;
    VOS_UINT8  endFlg;
    VOS_UINT8  id;
    VOS_UINT8  reserved;
    VOS_UINT8  nameLength;
    VOS_UINT8  name[MTA_MAX_EPDU_NAME_LEN]; /* 预留一位保存'\0' */
    VOS_UINT8  total;
    VOS_UINT8  index;
    VOS_UINT16 dataLength;
    VOS_UINT8  data[MTA_MAX_EPDU_BODY_LEN];
} MTA_AT_EpduDataInd;

typedef struct {
    VOS_UINT32 clearFlg;
} MTA_AT_XcposrrptInd;


typedef struct {
    MTA_AT_CgpsclockUint32 gpsClockState; /* RF芯片GPS时钟状态 */
} AT_MTA_CgpsclockReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 命令执行结果 */

    MTA_AT_ModemIdUint8 modemId;
    MTA_AT_RatModeUint8 ratMode;

    VOS_UINT8 reserved[2];
} MTA_AT_CgpsclockCnf;


typedef struct {
    AT_MTA_PersCategoryUint8 category;
    VOS_UINT8                reserved[3];
    VOS_UINT8                password[MTA_SIMLOCK_PASSWORD_LEN];
} AT_MTA_SimlockunlockReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 命令执行结果 */
} MTA_AT_SimlockunlockCnf;


typedef struct {
    VOS_UINT8 ratType;
    VOS_UINT8 reserve[3]; /* 预留后续使用 */
} AT_MTA_QryNmrReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT8           totalIndex;
    VOS_UINT8           currIndex;
    VOS_UINT16          nmrLen;     /* NMR数据长度 */
    VOS_UINT8           nmrData[4]; /* NMR数据首地址 */
} MTA_AT_QryNmrCnf;


typedef struct {
    VOS_UINT8 offsetFlg;  /* 0:灭屏；1:亮屏 */
    VOS_UINT8 reserve[3]; /* 预留后续使用 */
} AT_MTA_ReselOffsetCfgSetNtf;


typedef struct {
    VOS_UINT8  cmd;
    VOS_UINT8  paraNum;
    VOS_UINT16 reserve;
    VOS_UINT32 para[AT_MTA_WRR_AUTOTEST_MAX_PARA_NUM];
} AT_MTA_WrrAutotestQryPara;


typedef struct {
    VOS_UINT8  cmd;
    VOS_UINT8  paraNum;
    VOS_UINT16 reserve;
    VOS_UINT32 para[AT_MTA_GAS_AUTOTEST_MAX_PARA_NUM];
} AT_MTA_GasAutotestQryPara;


typedef struct {
    VOS_UINT32 rsltNum;
    VOS_UINT32 rslt[MTA_AT_WRR_AUTOTEST_MAX_RSULT_NUM];
} MTA_AT_WrrAutotestQryRslt;


typedef struct {
    VOS_UINT32                result;
    MTA_AT_WrrAutotestQryRslt wrrAutoTestRslt;
} MTA_AT_WrrAutotestQryCnf;


typedef struct {
    VOS_UINT32 rsltNum;
    VOS_UINT32 rslt[MTA_AT_GAS_AUTOTEST_MAX_RSULT_NUM];
} MTA_AT_GasAutotestQryRslt;


typedef struct {
    VOS_UINT32                result;
    MTA_AT_GasAutotestQryRslt grrAutoTestRslt;
} MTA_AT_GasAutotestQryCnf;


typedef struct {
    VOS_UINT16 cellFreq;
    VOS_UINT16 primaryScramCode;
    VOS_INT16  cpichRscp;
    VOS_INT16  cpichEcN0;

} MTA_AT_WrrCellinfo;


typedef struct {
    VOS_UINT32         cellNum;
    MTA_AT_WrrCellinfo wCellInfo[MTA_AT_WRR_MAX_NCELL_NUM]; /* 最多支持W 8个邻区的查询 */

} MTA_AT_WrrCellinfoRslt;

typedef struct {
    VOS_UINT32             result;
    MTA_AT_WrrCellinfoRslt wrrCellInfo;
} MTA_AT_WrrCellinfoQryCnf;


typedef struct {
    VOS_UINT16 eventId; /* 对应的事件类型 */
    VOS_UINT16 cellNum;
    VOS_UINT16 primaryScramCode[MTA_AT_WRR_ONE_MEANRPT_MAX_CELL_NUM];
} MTA_AT_WrrMeanrpt;


typedef struct {
    VOS_UINT32        rptNum;
    MTA_AT_WrrMeanrpt meanRptInfo[MTA_AT_WRR_MAX_MEANRPT_NUM];
} MTA_AT_WrrMeanrptRslt;


typedef struct {
    VOS_UINT32            result;
    MTA_AT_WrrMeanrptRslt meanRptRslt;
} MTA_AT_WrrMeanrptQryCnf;


typedef struct {
    VOS_INT16 cpichRscp;
    VOS_INT16 cpichEcNo;
} MTA_AT_AntennaQryCnf;


typedef struct {
    VOS_UINT8  freqLockEnable;
    VOS_UINT8  reserved[1];
    VOS_UINT16 lockedFreq;
} MTA_AT_WrrFreqlockCtrl;


typedef struct {
    VOS_UINT32 result;
} MTA_AT_WrrFreqlockSetCnf;


typedef struct {
    VOS_UINT32             result;
    MTA_AT_WrrFreqlockCtrl freqLockInfo;
} MTA_AT_WrrFreqlockQryCnf;


typedef struct {
    VOS_UINT32 result;
} MTA_AT_WrrRrcVersionSetCnf;


typedef struct {
    VOS_UINT32 result;
    VOS_UINT8  rrcVersion;
    VOS_UINT8  reserved[3];
} MTA_AT_WrrRrcVersionQryCnf;


typedef struct {
    VOS_UINT32 result;
} MTA_AT_WrrCellsrhSetCnf;


typedef struct {
    VOS_UINT32 result;
    VOS_UINT8  cellSearchType;
    VOS_UINT8  reserve[3];
} MTA_AT_WrrCellsrhQryCnf;


typedef struct {
    VOS_INT16  cpichRscp;
    VOS_INT16  cpichEcNo;
    VOS_UINT32 cellId;
    VOS_UINT8  rssi; /* Rssi, GU下使用 */
    VOS_UINT8  reserve[3];

} AT_MTA_Anquery2G3GPara;


typedef struct {
    VOS_INT16 rsrp; /* 范围：(-141,-44), 99为无效 */
    VOS_INT16 rsrq; /* 范围：(-40, -6) , 99为无效 */
    VOS_INT16 rssi; /* Rssi, LTE下使用 */
    VOS_UINT8 reserve[2];
} AT_MTA_Anquery4GPara;


typedef struct {
    VOS_UINT8 serviceSysMode; /* 指示上报模式 */
    VOS_UINT8 reserve[3];
    union {
        AT_MTA_Anquery2G3GPara st2G3GCellSignInfo; /* Rssi, GU下使用 */
        AT_MTA_Anquery4GPara   st4GCellSignInfo;   /* Rssi, LTE下使用 */
    } u;
} AT_MTA_AnqueryPara;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
    AT_MTA_AnqueryPara  antennaInfo;
} MTA_AT_AntennaInfoQryCnf;


typedef struct {
    VOS_INT16 cpichRscp;
    VOS_INT16 cpichEcNo;
} AT_MTA_CsnrPara;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
    AT_MTA_CsnrPara     csnrPara;
} MTA_AT_CsnrQryCnf;

/*
 * 结构说明: CSQLVL查询操作的结果上报结构
 *           包括RSCP 等级和RSCP 的绝对值
 */
typedef struct {
    VOS_UINT16 rscp;  /* RSCP 等级 */
    VOS_UINT16 level; /* RSCP 的绝对值 */
} AT_MTA_CsqlvlPara;


typedef struct {
    VOS_UINT8 rssilv; /* CSQLVLEX查询获取到的信号格数 */
    /* 比特误码率百分比，暂时不支持BER查询，填99 */
    VOS_UINT8 ber;
} AT_MTA_CsqlvlextPara;


typedef struct {
    MTA_AT_ResultUint32  result; /* 操作结果 */
    AT_MTA_CsqlvlPara    csqLvlPara;
    AT_MTA_CsqlvlextPara csqLvlExtPara;
    VOS_UINT8            reserved[2];
} MTA_AT_CsqlvlQryCnf;


typedef struct {
    MTA_AT_ResultUint32      result;            /* 操作结果 */
    AT_MTA_FratIgnitionUnit8 fratIgnitionState; /* IgnitionState */
    VOS_UINT8                reserved[3];
} MTA_AT_FratIgnitionQryCnf;


typedef struct {
    AT_MTA_FratIgnitionUnit8 fratIgnitionState; /* IgnitionState */
    VOS_UINT8                reserved[3];
} AT_MTA_FratIgnitionSetReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 查询操作结果 */
} MTA_AT_FratIgnitionSetCnf;


typedef struct {
    AT_MTA_BodySarStateUint16 state;       /* Body SAR状态 */
    VOS_UINT16                rsv;         /* 保留位 */
    MTA_BodySarPara           bodySarPara; /* Body SAR功率门限参数 */
} AT_MTA_BodySarSetReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
} MTA_AT_ResultCnf;


typedef struct {
    VOS_UINT8 reserved[4]; /* 保留位 */
} AT_MTA_Reserve;


typedef struct {
    /* 禁止或使能+CSSI的补充业务通知 0:不上报;1:上报 */
    VOS_UINT8 cssiRptFlg;
    /* 禁止或使能+CSSU的补充业务通知 0:不上报;1:上报 */
    VOS_UINT8 cssuRptFlg;
} AT_MTA_CssnRptFlg;


typedef struct {
    /* 主动上报模式，0:关闭所有的主动上报；1:打开所有的主动上报；2:根据RptCfg参数决定对应的bit位是否主动上报 */
    AT_MTA_RptGeneralCtrlTypeUint8 curcRptType;
    VOS_UINT8                      reserve[3]; /* 保留字段 */
    /* 64bit主动上报标识 */
    VOS_UINT8 rptCfg[AT_MTA_RPT_CFG_MAX_SIZE];
} AT_MTA_CurcSetNotify;


typedef struct {
    VOS_UINT8 reserve[4];
} AT_MTA_CurcQryReq;


typedef struct {
    AT_MTA_RptSetTypeUint32 reqType; /* 请求类型 */

    union {
        /* mode是否主动上报标识 0:不上报，1:上报 */
        VOS_UINT8 modeRptFlg;
        VOS_UINT8 srvstRptFlg;  /* service status是否主动上报标识 */
        VOS_UINT8 rssiRptFlg;   /* rssi是否主动上报标识 */
        VOS_UINT8 timeRptFlg;   /* time是否主动上报标识 */
        VOS_UINT8 ctzrRptFlg;   /* ctzr是否主动上报标识 */
        VOS_UINT8 dsFlowRptFlg; /* 流量是否主动上报标识 */
        VOS_UINT8 simstRptFlg;  /* sim卡状态是否主动上报标识 */
        VOS_UINT8 cregRptFlg;   /* cs域注册状态是否主动上报标识 */
        VOS_UINT8 cgregRptFlg;  /* ps域注册状态是否主动上报标识 */
        VOS_UINT8 ceregRptFlg;  /* L注册状态是否主动上报标识 */
        VOS_UINT8 csidRptFlg;     /* CSID是否主动上报标识 */
        VOS_UINT8 clocinfoRptFlg; /* CLOCINFO是否主动上报标识 */
        VOS_UINT8 mtReattachRptFlg; /* MTREATTACH是否主动上报标识 */
        /* ^CENFS是否主动上报标识 0:不上报，1:上报 */
        VOS_UINT8 cenfsRptFlg;
        VOS_UINT8 c5gregRptFlg; /* NR注册状态是否主动上报标识 */

        VOS_UINT8 lendcRptFlg;

        VOS_UINT8 rrcStatRptFlg;
        VOS_UINT8 hfreqInfoRptFlg;
    } u;

    VOS_UINT8 reserve[3];

} AT_MTA_UnsolicitedRptSetReq;


typedef struct {
    AT_MTA_RptQryTypeUint32 reqType;
} AT_MTA_UnsolicitedRptQryReq;


typedef struct {
    VOS_UINT8 gameMode;
    VOS_UINT8 rsv[3];
} AT_MTA_CommGameModeSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_GameModeSetCfn;


typedef struct {
    ModemIdUint16           sourceModemId;
    ModemIdUint16           destinationModemId;
    PLATAFORM_RatCapability sourceModemPlatform;
    PLATAFORM_RatCapability destinationModemPlatform;
} AT_MTA_CdmaModemCapSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_CdmaModemCapSetCnf;


typedef struct {
    ModemIdUint16           cdmaModemId; /* cdma能力所在的modem id */
    VOS_UINT8               rsv[2];
    PLATAFORM_RatCapability modem0Platform;
    PLATAFORM_RatCapability modem1Platform;
} AT_MTA_CdmaCapResumeSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_CdmaCapResumeSetCnf;


typedef struct {
    VOS_UINT16 pidIndex;
    VOS_UINT8  rsv[2];
    VOS_UINT32 len; /* 有效字符的个数 */
    VOS_UINT8  date[AT_CMD_COMM_BOOSTER_BS_MAX_LENGTH];
} AT_MTA_CommBoosterSetReq;


typedef struct {
    VOS_UINT16 pidIndex;
    VOS_UINT8  rsv[2];
    VOS_UINT32 len; /* 有效字符的个数 */
    VOS_UINT8  date[AT_CMD_COMM_BOOSTER_BS_MAX_LENGTH];
} AT_MTA_CommBoosterQueryReq;


typedef struct {
    VOS_UINT8  internalResultFlag; /* 内部原因标识 */
    VOS_UINT8  externalResultFlag; /* 外部原因标识 */
    VOS_UINT8  rsv[2];             /* 保留位 */
    VOS_UINT32 result;             /* 结果 */
} MTA_AT_CommBoosterSetCnf;


typedef struct {
    VOS_UINT8  result;
    VOS_UINT8  rsv[3];
    VOS_UINT32 len; /* 有效字符的个数 */
    VOS_UINT8  date[MTA_AT_BOOSTER_QRY_CNF_MAX_NUM];
} MTA_AT_CommBoosterQueryCnf;


typedef struct {
    TAF_MTA_NvCarrierOptResultUint32 result;
} MTA_AT_SetNvloadCnf;


typedef struct {
    VOS_UINT32 pidIndex;
    VOS_UINT32 len;
    /* Booster特性主动上报参数，TLV格式，满足四字节对齐 */
    VOS_UINT8 boosterIndData[MTA_AT_BOOSTER_IND_MAX_NUM];
} MTA_AT_CommBoosterInd;


typedef struct {
    VOS_UINT32 reqType;
} AT_MTA_NvloadSetReq;


typedef struct {
    VOS_UINT16 reqType;
    /* 温区范围(极高温，高温，常温，低温，极低温)，"11111"代表所有温区 */
    VOS_UINT16 tempRange;
    VOS_INT16  ppmOffset;  /* 样本偏移量 */
    VOS_INT16  timeOffset; /* 时间偏移量 */

} AT_MTA_SetSampleReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetSampleCnf;



typedef struct {
    VOS_UINT8 pseudRat; /* 伪基站制式 1.GSM  2.WCDMA  3.LTE */
    /* 查询参数 1.查询伪基站是否支持  2.查询伪基站拦截次数 */
    VOS_UINT8 pseudBtsQryType;
    VOS_UINT8 rsv[2];

} AT_MTA_SetPseudbtsReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* NO_ERROR:成功，其他:失败 */
    /* 查询参数 1.查询伪基站是否支持  2.查询伪基站拦截次数 */
    VOS_UINT8 pseudBtsIdentType;
    VOS_UINT8 rsv[3];

    union {
        VOS_UINT32 pseudBtsIdentTimes; /* 识别次数 */
        VOS_UINT8  pseudBtsIdentCap;   /* 是否支持 */
    } u;
} MTA_AT_PseudBtsSetCnf;



typedef struct {
    /* AT命令中CLFS参数组数，每4个参数为一组，每组内参数之间以逗号为分隔符 */
    VOS_UINT16 clfsGroupNum;
    VOS_UINT16 dataLength;  /* aucClfsData长度 */
    VOS_UINT8  clfsData[4]; /* CLFS模型参数数据 */
} AT_MTA_ParaInfo;



typedef struct {
    VOS_UINT8 activeFlg;                          /* 0:功能关闭，1:功能打开 */
    VOS_UINT8 seq;                                /* 流水号 */
    VOS_UINT8 type;                               /* 0:机器学习GSM伪基站模型参数 */
    VOS_UINT8 versionId[AT_MTA_VERSION_INFO_LEN]; /* 版本号，固定为xx.xx.xxx */
    VOS_UINT8 rsv[3];
    /* 机器学习概率门限参数，范围0~99999。整数99999表示99.999%概率 */
    VOS_UINT32      probaRate;
    AT_MTA_ParaInfo paraInfo;
} AT_MTA_SetSubclfsparamReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SubclfsparamSetCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT8           versionId[AT_MTA_VERSION_INFO_LEN]; /* 版本号 */
    VOS_UINT8           rsv[2];
} MTA_AT_SubclfsparamQryCnf;


typedef struct {
    VOS_UINT8 switchFlag;
    VOS_UINT8 reserve[3];
} AT_MTA_NcellMonitorSetReq;


typedef struct {
    AT_MTA_ClearFreqFlgUint8 mode;
    VOS_UINT8                reserved[3];
} AT_MTA_ClearHistoryFreqReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    /* 主动上报模式，0:关闭所有的主动上报；1:打开所有的主动上报；2:根据ReportCfg参数决定对应的bit位是否主动上报 */
    AT_MTA_RptGeneralCtrlTypeUint8 curcRptType;
    VOS_UINT8                      reserve[3];
    VOS_UINT8                      rptCfg[AT_MTA_RPT_CFG_MAX_SIZE]; /* 主动上报标识 */
} MTA_AT_CurcQryCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_UnsolicitedRptSetCnf;


typedef struct {
    MTA_AT_ResultUint32     result;
    AT_MTA_RptQryTypeUint32 reqType;

    union {
        VOS_UINT8              timeRptFlg; /* time是否主动上报标识 */
        MTA_TAF_CtzrTypeUint32 ctzrRptFlg; /* ctzr是否主动上报标识 */
        AT_MTA_CssnRptFlg      cssnRptFlg; /* cssn是否主动上报标识结构体 */
        VOS_UINT8              cusdRptFlg; /* cusd是否主动上报标识结构体 */
    } u;

} MTA_AT_UnsolicitedRptQryCnf;


typedef struct {
    VOS_UINT8 imei[NV_ITEM_IMEI_SIZE]; /* IMEI号码 */
} MTA_AT_CgsnQryCnf;

typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT8           switchFlag;
    VOS_UINT8           ncellState;
    VOS_UINT8           reserved[2];
} MTA_AT_NcellMonitorQryCnf;


typedef struct {
    VOS_UINT8 ncellState;
    VOS_UINT8 reserved[3];
} MTA_AT_NcellMonitorInd;


typedef struct {
    AT_MTA_CmdRptFlgUint8 rptFlg;       /* 命令上报开关标志 */
    VOS_UINT8             reserved1[3]; /* 保留位 */
} AT_MTA_RefclkfreqSetReq;


typedef struct {
    MTA_AT_ResultUint32         result;
    VOS_UINT32                  freq;         /* GPS参考时钟的频率值，单位Hz */
    VOS_UINT32                  precision;    /* 当前GPS参考时钟的精度，单位ppb */
    MTA_AT_RefclockStatusUint16 status;       /* 时钟频率锁定状态 */
    VOS_UINT8                   reserved1[2]; /* 保留位 */
} MTA_AT_RefclkfreqQryCnf;


typedef struct {
    VOS_UINT16 channelNo; /* 接收端地址 */
    VOS_UINT16 rficReg;
} AT_MTA_RficssirdReq;


typedef struct {
    VOS_UINT32 regValue; /* 回复的寄存器的值 */
} MTA_AT_RficssirdCnf;


typedef struct {
    VOS_UINT32                  freq;         /* GPS参考时钟的频率值，单位Hz */
    VOS_UINT32                  precision;    /* 当前GPS参考时钟的精度，单位ppb */
    MTA_AT_RefclockStatusUint16 status;       /* 时钟频率锁定状态 */
    VOS_UINT8                   reserved1[2]; /* 保留位 */
} MTA_AT_RefclkfreqInd;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
} MTA_AT_HandledectSetCnf;


typedef struct {
    VOS_UINT16          handle; /* 左右手类型 */
    VOS_UINT16          reserved1[1];
    MTA_AT_ResultUint32 result; /* 操作结果 */
} MTA_AT_HandledectQryCnf;


typedef struct {
    VOS_UINT8 cause;        /* PS域迁移原因值 */
    VOS_UINT8 reserved1[3]; /* 保留位 */
} MTA_AT_PsTransferInd;


typedef struct {
    VOS_UINT16 mipiClk;      /* 主动上报MIPICLK值 */
    VOS_UINT8  reserved1[2]; /* 保留位 */
} MTA_AT_RfLcdMipiclkInd;


typedef struct {
    MTA_AT_UartTestRsltUint32 uartTestRslt;
} MTA_AT_UartTestRsltInd;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_UartTestCnf;


typedef struct {
    VOS_UINT16 mipiClk; /* 主动上报MIPICLK值 */
    VOS_UINT16 result;
} MTA_AT_RfLcdMipiclkCnf;


typedef struct {
    VOS_UINT32 version; /* 命令版本号 */
} AT_MTA_EcidSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT8           cellInfoStr[4]; /* 增强型小区信息字符串 */
} MTA_AT_EcidSetCnf;


typedef struct {
    PS_BOOL_ENUM_UINT8 psProtectFlg;
    VOS_UINT8          reserve[3]; /* 预留后续使用 */
} AT_MTA_RrcProtectPsReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 返回给AT的结果 */
} MTA_AT_RrcProtectPsCnf;


typedef struct {
    VOS_UINT8 reserve[4]; /* 预留后续使用 */
} AT_MTA_PhyInitReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 返回给AT的结果 */
} MTA_AT_PhyInitCnf;


typedef struct {
    AT_MTA_CmdRatmodeUint8 ratMode;
    VOS_UINT8              flag;
    VOS_UINT8              reserved[2];
} AT_MTA_SetDpdttestFlagReq;


typedef struct {
    AT_MTA_CmdRatmodeUint8 ratMode;
    VOS_UINT8              reserved[3];
    VOS_UINT32             dpdtValue;
} AT_MTA_SetDpdtValueReq;


typedef struct {
    AT_MTA_CmdRatmodeUint8 ratMode;
    VOS_UINT8              reserved[3];
} AT_MTA_QryDpdtValueReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetDpdttestFlagCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetDpdtValueCnf;


typedef struct {
    VOS_UINT32          dpdtValue;
    MTA_AT_ResultUint32 result;
} MTA_AT_QryDpdtValueCnf;


typedef struct {
    AT_MTA_CmdRatmodeUint8   ratMode;
    AT_MTA_TrxTasCmdUint8    cmd;
    AT_MTA_CmdSignalingUint8 mode;
    VOS_UINT8                reserved;
    VOS_UINT32               trxTasValue;
} AT_MTA_SetTrxTasReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetTrxTasCnf;


typedef struct {
    AT_MTA_CmdRatmodeUint8   ratMode;
    AT_MTA_CmdSignalingUint8 mode;
    VOS_UINT8                reserved[2];
} AT_MTA_QryTrxTasReq;


typedef struct {
    MTA_AT_ResultUint32    result;
    AT_MTA_CmdRatmodeUint8 ratMode;
    VOS_UINT8              reserved[3];
    VOS_UINT32             trxTasValue;
} MTA_AT_QryTrxTasCnf;


typedef struct {
    /* 检测需要达到的频点测量值，取值范围:[0,70] */
    VOS_UINT8 threshold;
    /* 检测需要达到的频点个数，取值范围:[0,255] */
    VOS_UINT8 freqNum;
    VOS_UINT8 reserved[2];
} AT_MTA_GsmJamDetect;


typedef struct {
    /* 测量RSSI时，检测需要达到的频点测量值，取值范围[0,70]，实际用的时候减70使用 */
    VOS_UINT8 rssiSrhThreshold;
    /* 测量RSSI时检测需要达到的频点个数占频点总数（BAND和干扰信号取交集）的百分比，取值范围：[0,100] */
    VOS_UINT8 rssiSrhFreqPercent;
    /* 测量PSCH时检测需要小于或等于的频点测量值，取值范围:[0,65535] */
    VOS_UINT16 pschSrhThreshold;
    /* 测量PSCH时检测需要达到的频点个数占频点总数（BAND和干扰信号取交集）的百分比，取值范围:[0,100] */
    VOS_UINT8 pschSrhFreqPercent;
    VOS_UINT8 reserved[3];
} AT_MTA_WcdmaJamDetect;


typedef struct {
    /* 测量RSSI时，检测需要达到的频点测量值，取值范围[0,70]，实际用的时候减70使用 */
    VOS_INT16 rssiThresh;
    /* 测量PSS Ratio小于该阈值的频点判断为干扰频点，取值范围:[0,100] */
    VOS_UINT16 pssratioThresh;
    /* 测量RSSI时检测需要达到的频点个数占频点总数（BAND和干扰信号取交集）的百分比，取值范围：[0,100] */
    VOS_UINT8 rssiPercent;
    /* 测量PSS Ratio时，干扰频点个数占可疑干扰范围内频点个数的百分比，取值范围:[0,100] */
    VOS_UINT8 pssratioPercent;
    VOS_UINT8 reserved[2];
} AT_MTA_LteJamDetect;


typedef union {
    AT_MTA_GsmJamDetect   gsmPara;
    AT_MTA_WcdmaJamDetect wcdmaPara;
    AT_MTA_LteJamDetect   ltePara;
} AT_MTA_JamDetectUnion;


typedef struct {
    VOS_UINT8              mode; /* 干扰检测功能开关 */
    VOS_UINT8              reserved[2];
    AT_MTA_CmdRatmodeUint8 rat;       /* 接入制式 */
    AT_MTA_JamDetectUnion  unJamPara; /* 干扰检测配置参数 */
} AT_MTA_SetJamDetectReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetJamDetectCnf;


typedef struct {
    VOS_UINT8             gsmJamMode;   /* GSM制式下，干扰检测开关状态 */
    VOS_UINT8             wcdmaJamMode; /* WCDMA制式下，干扰检测开关状态 */
    VOS_UINT8             lteJamMode;   /* LTE制式下，干扰检测开关状态 */
    VOS_UINT8             reserved;
    AT_MTA_GsmJamDetect   gsmPara;
    AT_MTA_WcdmaJamDetect wcdmaPara;
    AT_MTA_LteJamDetect   ltePara;
} MTA_AT_QryJamDetectCnf;


typedef struct {
    AT_MTA_CmdRatmodeUint8 rat;
    VOS_UINT8              reserved[3];
    MTA_AT_JamResultUint32 jamResult;
} MTA_AT_JamDetectInd;


typedef struct {
    VOS_UINT8                   enableFlg; /* 1:锁频功能打开，0:功能关闭 */
    AT_MTA_FreqlockRatmodeUint8 ratMode;
    AT_MTA_GsmBandUint16        band;
    VOS_UINT32                  lockedFreq;
} AT_MTA_SetFreqLockReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetFreqLockCnf;


typedef struct {
    PS_BOOL_ENUM_UINT8   enableFlag;  /* PS_TRUE:锁定，PS_FALSE:去锁定 */
    VOS_UINT8            reserved[3]; /* 保留位 */
    VOS_UINT16           freq;        /* 频点值 */
    AT_MTA_GsmBandUint16 band;        /* GSM频段 */
} AT_MTA_SetGsmFreqlockReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetGsmFreqlockCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    /* PS_TRUE: 执行了锁频命令,PS_FALSE: 没有执行锁频命令 */
    PS_BOOL_ENUM_UINT8   lockFlg;
    VOS_UINT8            reserved[3]; /* 保留位 */
    VOS_UINT16           freq;        /* 绝对频点号 */
    AT_MTA_GsmBandUint16 band;        /* GSM频段 */
} MTA_AT_QryGsmFreqlockCnf;


typedef struct {
    VOS_UINT32 result;
} MTA_AT_DelCellentityCnf;

#if (FEATURE_ON == FEATURE_PROBE_FREQLOCK)

enum AT_FREQLOCK_WType {
    AT_MTA_WCDMA_FREQLOCK_CMD_FREQ_ONLY = 8, /* 对应CWAS锁频类型 */
    AT_MTA_WCDMA_FREQLOCK_CMD_COMBINED  = 9, /* 对应CWAS锁频+扰码类型 */
    AT_MTA_WCDMA_FREQLOCK_CMD_BUTT
};
typedef VOS_UINT8 AT_MTA_WcdmaFreqlockCmdUint8;


enum AT_MTA_M2MFreqlockType {
    AT_MTA_M2M_FREQLOCK_TYPE_ENUM_FREQ_ONLY     = 1, /* 锁频ONLY类型 */
    AT_MTA_M2M_FREQLOCK_TYPE_ENUM_FREQ_COMBINED = 2, /* 锁频+主扰码/扰码/物理小区ID */
    AT_MTA_M2M_FREQLOCK_TYPE_ENUM_BUTT
};
typedef VOS_UINT8 AT_MTA_M2MFreqlockTypeUint8;


typedef struct {
    VOS_UINT16           freq;    /* 频点值 */
    AT_MTA_GsmBandUint16 band;    /* GSM频段 */
    VOS_UINT32           reserve; /* 8字节对齐-代码检视 */
} AT_MTA_M2MGsmFreqlockPara;


typedef struct {
    AT_MTA_M2MFreqlockTypeUint8 freqType;    /* 锁频类型 */
    VOS_UINT8                   reserved[3]; /* 保留位 */
    VOS_UINT16                  freq;        /* 频点值 */
    VOS_UINT16                  psc;         /* WCDMA主扰码 */
} AT_MTA_M2MWcdmaFreqlockPara;


typedef struct {
    AT_MTA_M2MFreqlockTypeUint8 freqType;    /* 锁频类型 */
    VOS_UINT8                   reserved[3]; /* 保留位 */
    VOS_UINT16                  freq;        /* 频点值 */
    VOS_UINT16                  sc;          /* TD-SCDMA扰码 */
} AT_MTA_M2MTdscdmaFreqlockPara;


typedef struct {
    VOS_UINT32                  freq;     /* 频点值 */
    VOS_UINT16                  pci;      /* LTE物理小区ID */
    AT_MTA_M2MFreqlockTypeUint8 freqType; /* 锁频类型 */
    VOS_UINT8                   reserved; /* 保留位 */
} AT_MTA_M2MLteFreqlockPara;


enum AT_FreqlockFlagType {
    AT_MTA_M2M_FREQLOCK_MODE_GSM     = 1, /* GSM制式 */
    AT_MTA_M2M_FREQLOCK_MODE_WCDMA   = 2, /* WCDMA制式 */
    AT_MTA_M2M_FREQLOCK_MODE_TDSCDMA = 3, /* TD-SCDMA制式 */
    AT_MTA_M2M_FREQLOCK_MODE_LTE     = 4, /* LTE制式 */
    AT_MTA_M2M_FREQLOCK_MODE_BUTT
};
typedef VOS_UINT8 AT_MTA_M2MFreqlockModeUint8;


enum AT_MTA_M2MFreqlockFlagType {
    AT_MTA_M2M_FREQLOCK_FLAG_TYPE_OFF = 0, /* 锁频关闭 */
    AT_MTA_M2M_FREQLOCK_FLAG_TYPE_ON  = 1, /* 锁频开启 */
    AT_MTA_M2M_FREQLOCK_FLAG_TYPE_BUTT
};
typedef VOS_UINT8 AT_MTA_M2MFreqlockFlagUint8;


typedef struct {
    AT_MTA_M2MFreqlockFlagUint8   enableFlag;  /* 使能标识 */
    AT_MTA_M2MFreqlockModeUint8   mode;        /* 锁频制式 */
    VOS_UINT8                     reserved[6]; /* 保留 */
    AT_MTA_M2MGsmFreqlockPara     gFreqPara;   /* G模锁频消息结构 */
    AT_MTA_M2MWcdmaFreqlockPara   wFreqPara;   /* W模锁频消息结构 */
    AT_MTA_M2MTdscdmaFreqlockPara tdFreqPara;  /* TD模锁频消息结构 */
    AT_MTA_M2MLteFreqlockPara     lFreqPara;   /* L模锁频消息结构 */
} AT_MTA_SetM2MFreqlockReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetM2MFreqlockCnf;


typedef struct {
    VOS_UINT8                     freqState[MTA_FREQLOCK_MAX_MODE]; /* GUTL模式的锁频状态 */
    AT_MTA_M2MGsmFreqlockPara     gFreqLockInfo;                    /* GSM锁频状态查询结果 */
    AT_MTA_M2MWcdmaFreqlockPara   wFreqLockInfo;                    /* WCDMA锁频状态查询结果 */
    AT_MTA_M2MTdscdmaFreqlockPara tFreqLockInfo;                    /* TD-SCDMA锁频状态查询结果 */
    AT_MTA_M2MLteFreqlockPara     lFreqLockInfo;                    /* LTE锁频状态查询结果 */
} MTA_AT_QryM2MFreqlockCnf;
#endif


typedef struct {
    MTA_AT_GphyXpassModeUint16   gphyXpassMode;
    VOS_UINT16                   resev1;
    MTA_AT_WphyHighwayModeUint16 wphyXpassMode;
    VOS_UINT16                   resev2;
} MTA_AT_XpassInfoInd;

typedef struct {
    VOS_UINT32 ctrlType;
    VOS_UINT32 para1;
    VOS_UINT32 para2;
    VOS_UINT32 para3;
} AT_MTA_SetFemctrlReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetFemctrlCnf;


typedef struct {
    VOS_UINT8 secType;   /* 安全控制类型 */
    VOS_UINT8 secStrFlg; /* 是否携带安全校验密文 */
    VOS_UINT8 reserved[2];
    VOS_UINT8 secString[AT_RSA_CIPHERTEXT_LEN]; /* 安全校验密文 */
} AT_MTA_NvwrsecctrlSetReq;


typedef struct {
    PS_BOOL_ENUM_UINT8 actFrFlag; /* 激活FR标志，0:不激活  1:激活 */
    VOS_UINT8          rsv[3];    /* 保留位 */
} AT_MTA_SetFrReq;


typedef struct {
    MTA_AT_ResultUint32 rslt; /* FR设置及结果 */
} MTA_AT_SetFrCnf;


typedef struct {
    VOS_UINT16 year;
    VOS_UINT8  month;
    VOS_UINT8  day;
    VOS_UINT8  hour;
    VOS_UINT8  minute;
    VOS_UINT8  second;
    VOS_INT8   timeZone;
} MTA_AT_Time;


typedef struct {
    VOS_UINT8   dst;
    VOS_UINT8   ieFlg;
    VOS_INT8    localTimeZone;
    VOS_UINT8   rsv;
    VOS_UINT8   curcRptCfg[8];
    VOS_UINT8   unsolicitedRptCfg[8];
    MTA_AT_Time universalTimeandLocalTimeZone;
} TAF_AT_CommTime;


typedef struct {
    MTA_AT_TimeInfoRptOptUint8 rptOptType;
    VOS_UINT8                  reserve[3];
    TAF_AT_CommTime            commTimeInfo;
} MTA_AT_Sib16TimeUpdate;


enum MTA_AT_AccessStratumRel {
    MTA_AT_ACCESS_STRATUM_REL8       = 0,
    MTA_AT_ACCESS_STRATUM_REL9       = 1,
    MTA_AT_ACCESS_STRATUM_REL10      = 2,
    MTA_AT_ACCESS_STRATUM_REL11      = 3,
    MTA_AT_ACCESS_STRATUM_REL12      = 4,
    MTA_AT_ACCESS_STRATUM_REL13      = 5,
    MTA_AT_ACCESS_STRATUM_REL14      = 6,
    MTA_AT_ACCESS_STRATUM_REL_SPARE1 = 7
};
typedef VOS_UINT8 MTA_AT_AccessStratumRelUint8;


typedef struct {
    MTA_AT_AccessStratumRelUint8 accessStratumRel; /* 指示AT接入层的协议版本 */
    VOS_UINT8                    rsv[3];
} MTA_AT_AccessStratumRelInd;


typedef struct {
    /* 主卡主CC TM1时为TRUE（后续按周期上报）; 其他情况为 其他情况为FALSE(只上报一次，携带内容为全0). */
    VOS_UINT8 validflag;
    VOS_UINT8 rxAntNum; /* UE接收天线数：2收、4收 */
    /* 传输模式 。仅当 ucTmMode=0(对应传输模式1)时，透传的相位相关数据有效 */
    VOS_UINT8 tmMode;
    VOS_UINT8  reserved;
    VOS_UINT32 corrQR1Data0011; /* 透传以下6个寄存器数据。具体含义如下： */
    VOS_UINT32 corrQR1Data01Iq;
    VOS_UINT32 corrQR2Data0011; /* 仅4收时有效； */
    VOS_UINT32 corrQR2Data01Iq; /* 仅4收时有效； */
    VOS_UINT32 corrQR3Data0011; /* 仅4收时有效； */
    VOS_UINT32 corrQR3Data01Iq; /* 仅4收时有效； */
    VOS_INT16 rsrpR0; /* 天线 0 rsrp， 单位 dB */
    VOS_INT16 rsrpR1; /* 天线 1 rsrp， 单位 dB */
    VOS_INT16 rsrpR2; /* 天线 2 rsrp， 单位 dB，仅 4收时有效 */
    VOS_INT16 rsrpR3; /* 天线 3 rsrp， 单位 dB，仅 4收时有效 */
    VOS_INT16 rssiR0; /* 天线 0 rssi， 单位 dB */
    VOS_INT16 rssiR1; /* 天线 1 rssi， 单位 dB */
    VOS_INT16 rssiR2; /* 天线 2 rssi， 单位 dB，仅 4收时有效 */
    VOS_INT16 rssiR3; /* 天线 3 rssi， 单位 dB，仅 4收时有效 */
    VOS_INT16 rsv[8];
} MTA_AT_EccStatusInd;


typedef struct {
    VOS_UINT32 eccEnable;
    VOS_UINT32 rptPeriod;
} AT_MTA_SetEccCfgReq;


typedef MTA_AT_ResultCnf MTA_AT_SET_ECC_CFG_CNF_STRU;

typedef MTA_AT_ResultCnf MTA_AT_LENDC_CNF_STRU;


typedef struct {
    /* 当前小区是否支持endc模式: 0:不支持，1:支持 */
    VOS_UINT8 endcAvaliableFlag;
    /* 是否进入ENDC双连接状态: 0:非ENDC状态，1:ENDC已经建立状态 */
    VOS_UINT8 nrPscellFlag;
    /* PLMN LIST中是否有PLMN支持ENDC模式, true代表有PLMN支持ENDC模式, false代表没有PLMN支持endc模式 */
    VOS_UINT8 endcPlmnAvaliableFlag;
    /* 核心网是否限制endc能力，true代表核心网限制endc能力，false代表核心网没有限制endc能力 */
    VOS_UINT8 endcEpcRestrictedFlag;
} MTA_AT_LendcInfo;


typedef struct {
    MTA_AT_ResultUint32 rslt; /* 结果 */
    /* 开启/关闭^LENDC主动上报，0:关闭  1:开启 */
    VOS_UINT32       reportFlag;
    MTA_AT_LendcInfo lendcInfo;
} MTA_AT_LendcQryCnf;


typedef struct {
    MTA_AT_LendcInfo lendcInfo;
} MTA_AT_LendcInfoInd;

/*
 * 协议表格:
 * ASN.1描述:
 * 结构说明: NRRC锁频类型
 */
enum MTA_AT_FreqlockType {
    MTA_AT_FREQLOCK_TYPE_LOCK_NONE = 0,
    MTA_AT_FREQLOCK_TYPE_LOCK_FREQ = 1,
    MTA_AT_FREQLOCK_TYPE_LOCK_CELL = 2,
    MTA_AT_FREQLOCK_TYPE_LOCK_BAND = 3,

    MTA_AT_FREQLOCK_TYPE_LOCK_BUTT
};
typedef VOS_UINT8 MTA_AT_FreqlockTypeUint8;

/*
 * 协议表格:
 * ASN.1描述:
 * 结构说明: NRRC SCS Type COMM
 */
enum MTA_AT_ScsTypeComm {
    MTA_AT_SCS_TYPE_COMM_15  = 0x00,
    MTA_AT_SCS_TYPE_COMM_30  = 0x01,
    MTA_AT_SCS_TYPE_COMM_60  = 0x02,
    MTA_AT_SCS_TYPE_COMM_120 = 0x03,
    MTA_AT_SCS_TYPE_COMM_240 = 0x04,

    MTA_AT_SCS_TYPE_COMM_BUTT
};
typedef VOS_UINT8 MTA_AT_ScsTypeCommUint8;

typedef MTA_AT_ResultCnf MTA_AT_NRRC_FREQLOCK_SET_CNF_STRU;


typedef struct {
    MTA_AT_FreqlockTypeUint8 freqType;
    MTA_AT_ScsTypeCommUint8  scsType;
    VOS_UINT16               reserved;
    VOS_UINT16               band;
    VOS_UINT16               phyCellId;
    VOS_UINT32               nrArfcn;
} AT_MTA_SetNrfreqlockReq;


typedef struct {
    MTA_AT_ResultUint32      result;
    MTA_AT_FreqlockTypeUint8 freqType;
    MTA_AT_ScsTypeCommUint8  scsType;
    VOS_UINT16               reserved;
    VOS_UINT16               band;
    VOS_UINT16               phyCellId;
    VOS_UINT32               nrArfcn;
} MTA_AT_NrrcFreqlockQryCnf;


typedef struct {
    AT_MTA_CfgUint8 cfg;         /* 0:去使能，1:使能 */
    VOS_UINT8       reserved[3]; /* 保留位 */
} AT_MTA_MbmsServiceOptionSetReq;


typedef struct {
    VOS_UINT32 mcc; /* MCC, 3 bytes */
    VOS_UINT32 mnc; /* MNC, 2 or 3 bytes */
} AT_MTA_PlmnId;


typedef struct {
    VOS_UINT32    mbmsSerId; /* Service ID */
    AT_MTA_PlmnId plmnId;    /* PLMN ID */
} AT_MTA_MbmsTmgi;


typedef struct {
    AT_MTA_MbmsServiceStateSetUint8 stateSet;    /* 状态设置 */
    VOS_UINT8                       reserved[3]; /* 保留位 */
    VOS_UINT32                      areaId;      /* Area ID */
    AT_MTA_MbmsTmgi                 tmgi;        /* TMGI */
} AT_MTA_MbmsServiceStateSetReq;


typedef struct {
    AT_MTA_MbmsCastModeUint8 castMode;    /* 0:单播,1:组播 */
    VOS_UINT8                reserved[3]; /* 保留位 */
} AT_MTA_MbmsPreferenceSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          utc[2]; /* (0..549755813887) */
} MTA_AT_MbmsSib16NetworkTimeQryCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    /* BSSI信号强度,0xFF:表示BSSI信号强度无效 */
    VOS_UINT8 bssiLevel;
    VOS_UINT8 reserved[3]; /* 保留位 */
} MTA_AT_MbmsBssiSignalLevelQryCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          cellId; /* 小区ID */
} MTA_AT_MbmsNetworkInfoQryCnf;


typedef struct {
    MTA_AT_ResultUint32                 result;
    MTA_AT_EmbmsFuntionalityStatusUint8 status;      /* 功能状态 */
    VOS_UINT8                           reserved[3]; /* 保留位 */
} MTA_AT_EmbmsStatusQryCnf;


typedef struct {
    AT_MTA_CfgUint8 cfg;         /* 0:关闭,1:打开 */
    VOS_UINT8       reserved[3]; /* 保留位 */
} AT_MTA_MbmsUnsolicitedCfgSetReq;


typedef struct {
    MTA_AT_MbmsServiceEventUint8 event;       /* 服务事件 */
    VOS_UINT8                    reserved[3]; /* 保留位 */
} MTA_AT_MbmsServiceEventInd;


typedef struct {
    /* 0: Normal;1: Low Power Consumption */
    AT_MTA_LteLowPowerUint8 lteLowPowerFlg;
    VOS_UINT8               reserved[3]; /* 保留位 */
} AT_MTA_LowPowerConsumptionSetReq;


typedef struct {
    VOS_UINT32 freqList[AT_MTA_INTEREST_FREQ_MAX_NUM]; /* 频点列表 */
    /* VOS_FALSE: 单播优先;VOS_TRUE: MBMS优先 */
    AT_MTA_MbmsPriorityUint8 mbmsPriority;
    VOS_UINT8                reserved[3]; /* 保留位 */
} AT_MTA_MbmsInterestlistSetReq;


typedef struct {
    VOS_UINT32      opSessionId : 1;
    VOS_UINT32      spare : 31;
    VOS_UINT32      areaId;    /* Area ID */
    AT_MTA_MbmsTmgi tmgi;      /* TMGI */
    VOS_UINT32      sessionId; /* Session ID */
} MTA_AT_MbmsAvlService;


typedef struct {
    MTA_AT_ResultUint32 result;
    /* 可用服务数,0:表示没有可用服务 */
    VOS_UINT32            avlServiceNum;
    MTA_AT_MbmsAvlService avlServices[AT_MTA_MBMS_AVL_SERVICE_MAX_NUM]; /* 可用服务列表 */
} MTA_AT_MbmsAvlServiceListQryCnf;


typedef struct {
    AT_MTA_CoexBwTypeUint16 coexBwType;
    AT_MTA_CoexCfgUint16    cfg;
    VOS_UINT16              txBegin;
    VOS_UINT16              txEnd;
    VOS_INT16               txPower;
    VOS_UINT16              rxBegin;
    VOS_UINT16              rxEnd;
    VOS_UINT8               reserved[2]; /* 保留位 */
} AT_MTA_CoexPara;


typedef struct {
    VOS_UINT16      coexParaNum;
    VOS_UINT16      coexParaSize;
    AT_MTA_CoexPara coexPara[AT_MTA_ISMCOEX_BANDWIDTH_NUM];
} AT_MTA_LteWifiCoexSetReq;


typedef struct {
    AT_MTA_CoexPara coexPara[AT_MTA_ISMCOEX_BANDWIDTH_NUM];
} MTA_AT_LteWifiCoexQryCnf;


typedef struct {
    VOS_UINT8 meid[AT_MTA_MEID_DATA_LEN];
    VOS_UINT8 rsv[1];
} AT_MTA_MeidSetReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 查询操作结果 */
    VOS_UINT8           efruimid[MTA_AT_EFRUIMID_OCTET_LEN_EIGHT];
    VOS_UINT8           meId[MTA_AT_MEID_NV_DATA_LEN_NEW + 1]; /* Data */
    VOS_UINT8           pEsn[MTA_AT_PESN_NV_DATA_LEN];         /* Data */
    VOS_UINT32          meIdReadRst;
    VOS_UINT32          pEsnReadRst;
} MTA_AT_MeidQryCnf;


typedef struct {
    MTA_AT_ResultUint32 result;      /* 查询操作结果 */
    VOS_UINT8           transMode;   /* 传输模式 */
    VOS_UINT8           reserved[3]; /* 保留位 */
} MTA_AT_TransmodeQryCnf;


typedef struct {
    AT_MTA_UeCenterTypeUint32 ueCenter; /* UE模式 */
} AT_MTA_SetUeCenterReq;

#if (FEATURE_ON == FEATURE_UE_MODE_NR)

enum AT_MTA_NrDcMode {
    AT_MTA_NR_DC_MODE_NONE           = 0x00, /* 不支持辅连接 */
    AT_MTA_NR_DC_MODE_ENDC_ONLY      = 0x01, /* 仅支持ENDC */
    AT_MTA_NR_DC_MODE_NEDC_ONLY      = 0x02, /* 仅支持NEDC */
    AT_MTA_NR_DC_MODE_ENDC_NEDC_BOTH = 0x03, /* ENDC和NEDC都支持 */
    AT_MTA_NR_DC_MODE_BUTT
};
typedef VOS_UINT8 AT_MTA_NrDcModeUint8;

enum AT_MTA_5GcAccessMode {
    AT_MTA_5GC_ACCESS_MODE_NOT_ALLOW  = 0x00, /* 不允许接入5gc */
    AT_MTA_5GC_ACCESS_MODE_NR_ONLY    = 0x01, /* 仅允许NR接入5gc */
    AT_MTA_5GC_ACCESS_MODE_LTE_ONLY   = 0x02, /* 仅允许LTE接入5gc */
    AT_MTA_5GC_ACCESS_MODE_NR_AND_LTE = 0x03, /* 允许LTE和NR都接入5gc */
    AT_MTA_5GC_ACCESS_MODE_BUTT
};
typedef VOS_UINT8 AT_MTA_5GcAccessModeUint8;


typedef struct {
    /* 0:不支持NR接入网以SA工作；1:支持NR接入网以SA工作 */
    VOS_UINT8                 nrSaSupportFlag;
    AT_MTA_NrDcModeUint8      nrDcMode;        /* NR的DC支持模式 */
    AT_MTA_5GcAccessModeUint8 en5gcAccessMode; /* 允许接入5gc的制式 */
    VOS_UINT8                 rsv;
} AT_MTA_Set5GOptionReq;


typedef struct {
    MTA_AT_ResultUint32 rslt;
} MTA_AT_Set5GOptionCnf;


typedef struct {
    MTA_AT_ResultUint32 rslt;
    /* 0:不支持NR接入网以SA工作；1:支持NR接入网以SA工作 */
    VOS_UINT8                 nrSaSupportFlag;
    AT_MTA_NrDcModeUint8      nrDcMode;        /* NR的DC支持模式 */
    AT_MTA_5GcAccessModeUint8 en5gcAccessMode; /* 允许接入5gc的制式 */
    VOS_UINT8                 rsv;
} MTA_AT_Qry5GOptionCnf;
#endif


typedef struct {
    MTA_AT_ResultUint32 result; /* 查询操作结果 */
} MTA_AT_SetUeCenterCnf;


typedef struct {
    MTA_AT_ResultUint32       result;   /* 查询操作结果 */
    AT_MTA_UeCenterTypeUint32 ueCenter; /* UE模式 */
} MTA_AT_QryUeCenterCnf;


typedef struct {
    VOS_UINT32            opTa : 1;
    VOS_UINT32            opSpare : 31;
    VOS_UINT16            ta;
    VOS_UINT8             reserved[2];
    VOS_UINT32            mcc;    /* 移动国家码 */
    VOS_UINT32            mnc;    /* 移动网络码 */
    VOS_UINT32            cellId; /* 小区ID */
    VOS_UINT32            arfcn;  /* 绝对频点号 */
    VOS_UINT16            lac;    /* 位置区码 */
    VOS_INT16             rssi;   /* Receiving signal strength in dbm */
    MTA_RRC_GsmBandUint16 band;   /* GSM频段(0-3) */
    VOS_UINT8             bsic;   /* 小区基站码 */
    /* IDLE态下或者PS数传态下无效,专用态下填充信道质量值，范围[0,7] ,无效值99 */
    VOS_UINT8 rxQuality;
} MTA_AT_NetmonGsmScellInfo;


typedef struct {
    VOS_UINT32                 opCellId : 1;
    VOS_UINT32                 opLac : 1;
    VOS_UINT32                 opSpare : 30;
    VOS_UINT32                 mcc;    /* 移动国家码 */
    VOS_UINT32                 mnc;    /* 移动网络码 */
    VOS_UINT32                 cellId; /* 小区ID */
    VOS_UINT32                 arfcn;  /* 频点 */
    VOS_UINT16                 lac;    /* 位置区码 */
    VOS_UINT8                  reserved[2];
    MTA_NETMON_UtranTypeUint32 cellMeasTypeChoice; /* NETMON频率信息类型:FDD,TDD */
    union {
        MTA_NETMON_UtranScellInfoFdd cellMeasResultsFdd; /* FDD */
        MTA_NETMON_UtranScellInfoTdd cellMeasResultsTdd; /* TDD */
    } u;
} MTA_AT_NetmonUtranScellInfo;


typedef struct {
    VOS_UINT32                mcc;                 /* 移动国家码 */
    VOS_UINT32                mnc;                 /* 移动网络码 */
    VOS_UINT32                cellIdentityHighBit; /* Cell Identity高32位 */
    VOS_UINT32                cellIdentityLowBit;  /* Cell Identity低32位 */
    VOS_UINT32                phyCellId;           /* 物理小区ID */
    VOS_UINT32                arfcn;               /* 频点 */
    VOS_UINT32                tac;
    VOS_INT16                 rsrp;
    VOS_INT16                 rsrq;
    VOS_INT16                 sinr;
    MTA_AT_NrScsTypeCommUint8 nrScsType;
    VOS_UINT8                 reserved;
} MTA_AT_NetmonNrScellInfo;


typedef union {
    MTA_AT_NetmonGsmScellInfo   gsmSCellInfo;   /* GSM服务小区信息 */
    MTA_AT_NetmonUtranScellInfo utranSCellInfo; /* WCDMA服务小区信息 */
    MTA_NETMON_EutranScellInfo  lteSCellInfo;   /* LTE服务小区信息 */
    MTA_AT_NetmonNrScellInfo    nrSCellInfo;    /* NR服务小区信息 */
} MTA_AT_NetMonScellInfo;


typedef struct {
    VOS_UINT32            opBsic : 1;
    VOS_UINT32            opCellId : 1;
    VOS_UINT32            opLac : 1;
    VOS_UINT32            opSpare : 29;
    VOS_UINT32            cellId; /* 小区ID */
    VOS_UINT32            afrcn;  /* 频点 */
    VOS_UINT16            lac;    /* 位置区码 */
    VOS_INT16             rssi;   /* 频点的RSSI */
    MTA_RRC_GsmBandUint16 band;   /* band 0-3 */
    VOS_UINT8             bsic;   /* 小区基站码 */
    VOS_UINT8             reserved;
} MTA_AT_NetmonGsmNcellInfo;


typedef struct {
    VOS_UINT32 arfcn; /* 频点 */
    VOS_UINT16 psc;   /* 主扰码 */
    VOS_INT16  ecN0;  /* ECN0 */
    VOS_INT16  rscp;  /* RSCP */
    VOS_UINT8  reserved[2];
} MTA_AT_NetmonUtranNcellInfoFdd;


typedef struct {
    VOS_UINT32 arfcn;  /* 频点 */
    VOS_UINT16 sc;     /* 扰码 */
    VOS_UINT16 syncId; /* 下行导频码 */
    VOS_INT16  rscp;   /* RSCP */
    VOS_UINT8  reserved[2];
} MTA_AT_NetmonUtranNcellInfoTdd;


typedef struct {
    VOS_UINT32 phyCellId; /* 物理小区ID */
    VOS_UINT32 arfcn;     /* 频点 */
    VOS_INT16  rsrp;      /* RSRP参考信号接收功率 */
    VOS_INT16  rsrq;      /* RSRQ参考信号接收质量 */
    VOS_INT16  sinr;
    VOS_UINT8  reserved[2];
} MTA_AT_NetmonNrNcellInfo;


typedef struct {
    /* GSM 临区个数 */
    VOS_UINT8 gsmNCellCnt;
    /* WCDMA 临区个数 */
    VOS_UINT8 utranNCellCnt;
    /* LTE 临区个数 */
    VOS_UINT8 lteNCellCnt;
    /* NR 临区个数 */
    VOS_UINT8 nrNCellCnt;
    /* GSM 临区数据结构 */
    MTA_AT_NetmonGsmNcellInfo gsmNCellInfo[NETMON_MAX_GSM_NCELL_NUM];
    /* NETMON频率信息类型:FDD,TDD */
    MTA_NETMON_UtranTypeUint32 cellMeasTypeChoice;
    union {
        /* FDD临区数据结构 */
        MTA_AT_NetmonUtranNcellInfoFdd fddNCellInfo[NETMON_MAX_UTRAN_NCELL_NUM];
        /* TDD临区数据结构 */
        MTA_AT_NetmonUtranNcellInfoTdd tddNCellInfo[NETMON_MAX_UTRAN_NCELL_NUM];
    } u;
    /* LTE 临区数据结构 */
    MTA_NETMON_LteNcellInfo lteNCellInfo[NETMON_MAX_LTE_NCELL_NUM];
    /* NR 临区数据结构 */
    MTA_AT_NetmonNrNcellInfo nrNCellInfo[NETMON_MAX_NR_NCELL_NUM];
} MTA_AT_NetmonNcellInfo;


typedef struct {
    VOS_UINT32                   arfcn;
    VOS_UINT32                   phyCellId;
    VOS_INT16                    rsrp;
    VOS_INT16                    rsrq;
    VOS_INT16                    sinr;
    MTA_AT_NetmonMeasRsTypeUint8 measRsType;
    VOS_UINT8                    rsv;
} MTA_AT_NetmonNrCcInfo;


typedef struct {
    VOS_UINT32            componentCarrierNum;                        /* 载波数目 */
    MTA_AT_NetmonNrCcInfo componentCarrierInfo[NETMON_MAX_NR_CC_NUM]; /* 载波信息 */
} MTA_AT_NetmonNrSscellInfo;


typedef struct {
    MTA_AT_ResultUint32 result; /* 查询结果是否成功 */
    /* 0:查询服务小区，1:查询临区，2:查询辅连接的小区 */
    MTA_NetMonCellTypeUint32       cellType;
    MTA_AT_NetmonCellInfoRatUint32 ratType; /* 服务小区的接入技术类型 */
    union {
        MTA_AT_NetmonNcellInfo    nCellInfo; /* 临区信息 */
        MTA_AT_NetMonScellInfo    unSCellInfo;
        MTA_AT_NetmonNrSscellInfo secSrvCellInfo; /* 辅连接小区信息 */
    } u;
} MTA_AT_NetmonCellInfo;


typedef struct {
    VOS_UINT32 reserveValue;
} AT_MTA_QryAfcClkFreqXocoefReq;


typedef struct {
    MTA_AT_ResultUint32 result;

    AT_MTA_AfcClkStatusUint32 status;
    VOS_INT32                 deviation; /* 频偏 */

    /* 温度范围 */
    VOS_INT16 coeffStartTemp;
    VOS_INT16 coeffEndTemp;

    /* 多项式系数 */
    VOS_UINT32 coeffMantissa[AT_MTA_GPS_XO_COEF_NUM]; /* a0,a1,a2,a3尾数 */
    VOS_UINT16 coeffExponent[AT_MTA_GPS_XO_COEF_NUM]; /* a0,a1,a2,a3指数 */

    MTA_AT_ModemIdUint8 modemId;
    MTA_AT_RatModeUint8 ratMode;
    VOS_UINT8           reserved[2];
} MTA_AT_QryAfcClkFreqXocoefCnf;


typedef struct {
    VOS_UINT32 doSysEvent;
} AT_MTA_EvdoSysEventSetReq;


typedef struct {
    VOS_UINT32 doSigMask;
} AT_MTA_EvdoSigMaskSetReq;


typedef struct {
    VOS_UINT32 paraLen;
    VOS_UINT8  content[4];
} MTA_AT_EvdoRevaRlinkInfoInd;


typedef struct {
    VOS_UINT32 paraLen;
    VOS_UINT8  content[4];
} MTA_AT_EvdoSigExeventInd;


typedef struct {
    /* GPS芯片是否支持清除辅助定位信息，上电开机默认为不支持 */
    AT_MTA_XcposrCfgUnit8 xcposrEnableCfg;
    VOS_UINT8             reserved[3];
} AT_MTA_SetXcposrReq;


typedef struct {
    MTA_AT_ResultUint32   result; /* 命令执行结果 */
    AT_MTA_XcposrCfgUnit8 xcposrEnableCfg;
    VOS_UINT8             reserved[3];
} MTA_AT_QryXcposrCnf;


typedef struct {
    /* +XCPOSRRPT命令主动上报控制，上电开机默认为不允许主动上报 */
    VOS_UINT8 xcposrRptFlg;
    VOS_UINT8 reserved[3];
} AT_MTA_SetXcposrrptReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 命令执行结果 */
    VOS_UINT8           xcposrRptFlg;
    VOS_UINT8           reserved[3];
} MTA_AT_QryXcposrrptCnf;


typedef struct {
    VOS_UINT32 sensorStat; /* Sensor Hub 状态 */
} AT_MTA_SetSensorReq;

typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetSensorCnf;


typedef struct {
    VOS_UINT8 screenStat; /* Screen 状态 */
    VOS_UINT8 reserved[3];
} AT_MTA_SetScreenReq;

typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetScreenCnf;


typedef struct {
    AT_MTA_CfgUint8 cfg;         /* 0:关闭,1:打开 */
    VOS_UINT8       reserved[3]; /* 保留位 */
} AT_MTA_SetRxtestmodeReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
} MTA_AT_SetRxtestmodeCnf;

typedef struct {
    VOS_UINT16 cause;
    VOS_UINT16 reserved; /* 保留位 */
} MTA_AT_AfcClkUnlockInd;


typedef struct {
    VOS_UINT8 groupId;  /* 器件号 */
    VOS_UINT8 deviceId; /* 设备号 */
    VOS_UINT8 caseId;   /* 器件最优频率类号 */
    VOS_UINT8 mode;     /* 主动上报方式 */
} MTA_AT_DeviceFreq;


typedef struct {
    VOS_UINT8         rptDeviceNum;                                 /* 要上报的激活器件数目 */
    VOS_UINT8         reserved[3];                                  /* 保留位 */
    MTA_AT_DeviceFreq deviceFreqList[MTA_AT_MAX_BESTFREQ_GROUPNUM]; /* 8组器件最优频率列表 */
} MTA_AT_BestFreqCaseInd;


typedef struct {
    MTA_AT_ResultUint32 result; /* 消息处理结果 */
    VOS_UINT8           activeDeviceNum;
    VOS_UINT8           reserved[3];
    MTA_AT_DeviceFreq   deviceInfoList[MTA_AT_MAX_BESTFREQ_GROUPNUM];
} MTA_AT_BestfreqQryCnf;


typedef struct {
    VOS_UINT32 ratType; /* 0-GSM 1-WCDMA */
} AT_MTA_SlaveSetReq;

typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT16          rfic0DieIdValid; /* 1-存在， 0-不存在 */
    VOS_UINT16          rfic1DieIdValid;
    VOS_UINT16          rfic2DieIdValid;
    VOS_UINT16          rfic3DieIdValid;
    VOS_UINT16          rfic0DieId[MTA_AT_MAX_DIE_ID_LEN];
    VOS_UINT16          rfic1DieId[MTA_AT_MAX_DIE_ID_LEN];
    VOS_UINT16          rfic2DieId[MTA_AT_MAX_DIE_ID_LEN];
    VOS_UINT16          rfic3DieId[MTA_AT_MAX_DIE_ID_LEN];
} MTA_AT_RficDieIdReqCnf;


typedef struct {
    VOS_UINT16 dataValid;
    VOS_UINT16 reserved;
    VOS_UINT8  rffeDieId[MTA_AT_MAX_RFFE_DIE_ID_LEN];
} MTA_AT_RffeDieIdValue;


typedef struct {
    MTA_AT_ResultUint32   result;                                  /* 操作结果 */
    MTA_AT_RffeDieIdValue dieIdData[MTA_AT_MAX_RFFE_DIE_ID_COUNT]; /* DieId数据 */
} MTA_AT_RffeDieIdReqCnf;


typedef struct {
    VOS_UINT16 cmdType;   /* 命令类型 */
    VOS_UINT16 ratBitmap; /* 支持的rat模式，比bitmap */
    VOS_UINT32 para1;     /* 配置参数1 */
    VOS_UINT32 para2;     /* 配置参数2 */
    VOS_UINT32 para3;     /* 配置参数3 */
} AT_MTA_PhyComCfgSetReq;

#if (FEATURE_UE_MODE_NR == FEATURE_ON)

typedef struct {
    VOS_UINT16 cmdType;   /* 命令类型 */
    VOS_UINT16 paraNum;   /* 参数个数 */
    VOS_UINT32 paraList[MTA_NRPHY_MAX_PARA_NUM];     /* 配置参数 */
} AT_MTA_NrphyComCfgSetReq;
#endif

typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_PhyComCfgSetCnf;

#if (FEATURE_UE_MODE_NR == FEATURE_ON)

typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_NrphyComCfgSetCnf;
#endif


typedef struct {
    AT_MTA_CfgUint8 enable;      /* 0关闭主动上报，1打开主动上报 */
    VOS_UINT8       reserved[3]; /* 保留位 */
} AT_MTA_SetCrrconnReq;


typedef struct {
    VOS_UINT8 status0;
    VOS_UINT8 status1;
    VOS_UINT8 status2;
    VOS_UINT8 reserved; /* 保留位 */
} MTA_AT_CrrconnStatusInd;


typedef MTA_AT_ResultCnf MTA_AT_SetCrrconnCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    AT_MTA_CfgUint8     enable;
    VOS_UINT8           status0;
    VOS_UINT8           status1;
    VOS_UINT8           status2;
} MTA_AT_QryCrrconnCnf;


typedef struct {
    VOS_UINT32 enable;    /* 0关闭主动上报，1打开主动上报 */
    VOS_UINT32 threshold; /* LPDCP信息上报阈值 */
} AT_MTA_SetVtrlqualrptReq;


typedef struct {
    VOS_INT16  rsrp; /* RSRP测量结果 */
    VOS_INT16  rsrq; /* RSRQ测量结果 */
    VOS_INT16  rssi; /* RSSI测量结果 */
    VOS_UINT16 bler; /* 误码率 */
} MTA_AT_RlQualityInfoInd;


typedef struct {
    VOS_UINT32 currBuffTime;   /* 最老数据包缓存时长 */
    VOS_UINT32 currBuffPktNum; /* 当前缓存数据包个数 */
    /* MAC上行速率,只包含上行新授权速率，不包含重传授权。单位:bytes/s */
    VOS_UINT32 macUlThrput;
    VOS_UINT32 maxBuffTime; /* 最大缓存时间，单位ms */
} MTA_AT_VideoDiagInfoRpt;


typedef MTA_AT_ResultCnf MTA_AT_SetVtrlqualrptCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT8           dieSn[MTA_PMU_MAX_DIE_SN_LEN];
} MTA_AT_PmuDieSnReqCnf;


typedef struct {
    VOS_UINT32 extendFlag;
    VOS_UINT32 mipiId;
    VOS_UINT32 slaveId;
    VOS_UINT32 regAddr;
    VOS_UINT32 value;
    VOS_UINT32 byteCnt;
} AT_MTA_MipiWrexReq;


typedef struct {
    VOS_UINT32 extendFlag;
    VOS_UINT32 mipiId;
    VOS_UINT32 slaveId;
    VOS_UINT32 regAddr;
    VOS_UINT32 byteCnt;
    VOS_UINT32 speedType;
} AT_MTA_MipiRdexReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_MipiWrexCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          value;
} MTA_AT_MipiRdexCnf;


typedef struct {
    AT_MTA_ModemCapUpdateTypeUint8 modemCapUpdateType; /* MODEM平台能力更新类型 */
    VOS_UINT8                      reserved[3];
} AT_MTA_ModemCapUpdateReq;


typedef struct {
    MTA_AT_ResultUint32 rslt; /* MODEM平台能力更新结果 */
} MTA_AT_ModemCapUpdateCnf;


typedef struct {
    VOS_UINT8 dlCategery;  /* 下行category */
    VOS_UINT8 ulCategery;  /* 上行category */
    VOS_UINT8 reserved[2]; /* 保留位 */
} MTA_AT_LteCategoryInfoInd;

/*
 * H2ASN顶级消息结构定义
 */
typedef struct {
    /* _H2ASN_MsgChoice_Export AT_MTA_MSG_TYPE_ENUM_UINT32 */
    AT_MTA_MsgTypeUint32 msgId;
    AT_APPCTRL           appCtrl;
    VOS_UINT8            msgBlock[4];
    /*
     * _H2ASN_MsgChoice_When_Comment          AT_MTA_MsgTypeUint32
     */
} AT_MTA_MsgData;
/* _H2ASN_Length UINT32 */

typedef struct {
    VOS_MSG_HEADER
    AT_MTA_MsgData msgData;
} AtMtaInterface_Msg;


typedef struct {
    VOS_INT32 sec;
    VOS_INT32 min;
    VOS_INT32 hour;
    VOS_INT32 day;
    VOS_INT32 month;
    VOS_INT32 year;
    VOS_INT32 zone;
} AT_MTA_ModemTime;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetTimeCnf;


typedef struct {
    VOS_UINT32 value;
} AT_MTA_Gnssnty;



typedef struct {
    VOS_UINT8 deviceId; /* 需要设置的干扰器件ID, 0表示全部，1-255分别对应特定器件 */
    VOS_UINT8 mode;     /* 0表示关闭主动上报，1表示开启主动上报 */
    VOS_UINT8 reserved[2];
} AT_MTA_BestfreqSetReq;


enum MTA_AT_RcmTasTestRatmode {
    MTA_AT_RATMODE_GSM   = 0,
    MTA_AT_RATMODE_WCDMA = 1,
    MTA_AT_RATMODE_LTE   = 2,
    MTA_AT_RATMODE_TDS   = 3,
    MTA_AT_RATMODE_1X    = 4,
    MTA_AT_RATMODE_HRPD  = 5,
    MTA_AT_RATMODE_BUTT
};
typedef VOS_UINT32 MTA_AT_TasTestRatmodeUint32;


typedef struct {
    MTA_AT_TasTestRatmodeUint32 ratMode; /* 1-WCDMA   2-TLE */
    VOS_UINT32                  para0;
    VOS_UINT32                  para1;
    VOS_UINT32                  para2;
    VOS_UINT32                  para3;
} AT_MTA_TasTestCfg;


typedef struct {
    VOS_RATMODE_ENUM_UINT32 ratMode; /* 1-WCDMA   2-TLE */
} AT_MTA_TasTestQry;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_TasTestCfgCnf;


typedef struct {
    VOS_UINT16 uhwSrcAntTimeLength;  /* 源(当前)天线时长，单位:ms */
    VOS_UINT16 uhwDestAntTimeLength; /* 目标天线时长，单位:ms */
    /* 源(当前)天线发射功率提升值，单位:0.1dB */
    VOS_INT16 srcAntTxPowerGain;
    /* 目标天线发射功率提升值，单位:0.1dB */
    VOS_INT16 destAntTxPowerGain;
} MTA_AT_TxStateiiLevel;


typedef struct {
    VOS_UINT16            uhwLevelNum;
    VOS_UINT16            uhwRsv;
    MTA_AT_TxStateiiLevel levelItem[MTA_AT_MAX_STATEII_LEVEL_ITEM];
} MTA_AT_TxStateiiLevelTable;


typedef struct {
    MTA_AT_ResultUint32         result;
    MTA_AT_TasTestRatmodeUint32 ratMode;
    VOS_UINT32                  currLevel; /* 当前档位索引 */
    MTA_AT_TxStateiiLevel       levelInfo; /* 当前档位的具体内容 */
    MTA_AT_TxStateiiLevelTable  usedTable; /* 档位表 */
} MTA_AT_TasTestQryCnf;


typedef struct {
    VOS_UINT8 pmicId;
    VOS_UINT8 reserved[3];
} AT_MTA_MultiPmuDieIdSet;


typedef struct {
    AT_MTA_RsInfoTypeUint32 rsInfoType; /* 0:查询RSRP，1:查询RSRQ */
} AT_MTA_RsInfoQryReq;


typedef struct {
    VOS_UINT32 cellId; /* 小区ID */
    VOS_UINT32 earfcn; /* 小区频点 */
    /* 信号接收功率，LRRC乘100后的值，AT上报要带两位小数 */
    VOS_INT32 rsrp;
} MTA_AT_RsrpInfo;


typedef struct {
    VOS_UINT32 cellId; /* 小区ID */
    VOS_UINT32 earfcn; /* 小区频点 */
    /* 信号接收质量，LRRC乘100后的值，AT上报要带两位小数 */
    VOS_INT32 rsrq;
} MTA_AT_RsrqInfo;


typedef struct {
    AT_MTA_RsInfoTypeUint32 rsInfoType; /* 0:查询RSRP，1:查询RSRQ */
    VOS_UINT32              rsInfoNum;
    union {
        MTA_AT_RsrpInfo rsrpInfo[AT_MAX_RS_INFO_NUM]; /* RSRP测量结果 */
        MTA_AT_RsrqInfo rsrqInfo[AT_MAX_RS_INFO_NUM]; /* RSRQ测量结果 */
    } u;
} MTA_AT_RsInfoRslt;


typedef struct {
    MTA_AT_ResultUint32 result;
    MTA_AT_RsInfoRslt   rsInfoRslt;
} MTA_AT_RsInfoQryCnf;


typedef struct {
    VOS_UINT32 errcCapType;
    VOS_UINT32 para1;
    VOS_UINT32 para2;
    VOS_UINT32 para3;
} AT_MTA_ErrccapCfgReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_ErrccapCfgCnf;


typedef struct {
    VOS_UINT32 errcCapType;
} AT_MTA_ErrccapQryReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          errcCapType;
    VOS_UINT32          para1;
    VOS_UINT32          para2;
    VOS_UINT32          para3;
} MTA_AT_ErrccapQryCnf;


typedef struct {
    VOS_UINT32                     mcc;
    VOS_UINT32                     mnc;
    VOS_UINT32                     cellId;
    VOS_UINT32                     lac;
    AT_MTA_SysModeUint32           sysmode;
    AT_MTA_PseucellNotifyTypeUint8 pseucellNotifyType; /* 0:为短信识别；其他:不支持 */
    VOS_UINT8                      reserved[3];
} AT_MTA_PseucellInfoSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_PseucellInfoSetCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_INT16           powerDet;     /* 功率检测结果，0.1dBm精度 */
    VOS_UINT8           reserved1[2]; /* 保留位 */
} MTA_AT_PowerDetQryCnf;


typedef struct {
    VOS_UINT16 readType;
    VOS_UINT16 mipiId;
    VOS_UINT16 slaveId;
    VOS_UINT16 regAddr;
    VOS_UINT16 speedType;
    VOS_UINT16 readBitMask;
    VOS_UINT16 reserved1;
    VOS_UINT16 reserved2;
} AT_MTA_MipiReadReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
    VOS_UINT32          value;
} MTA_AT_MipiReadCnf;


typedef struct {
    VOS_UINT16 writeType;
    VOS_UINT16 mipiId;
    VOS_UINT16 slaveId;
    VOS_UINT16 regAddr;
    VOS_UINT16 mipiData;
    VOS_UINT16 reserved1;
} AT_MTA_PhyMipiWriteReq;


typedef struct {
    MTA_AT_ResultUint32 result; /* 操作结果 */
} MTA_AT_PhyMipiWriteCnf;


typedef struct {
    VOS_UINT32 enableFlag;
} AT_MTA_NoCardModeSetReq;

typedef MTA_AT_ResultCnf MTA_AT_NoCardModeSetCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          enableFlag;
} MTA_AT_NoCardModeQryCnf;


typedef struct {
    AT_MTA_ChrAlarmRlatOpUint32 alarmOp;
    VOS_UINT32                  alarmId;
    VOS_UINT32                  alarmDetail;
} AT_MTA_ChrAlarmRlatCfgSetReq;


typedef struct {
    MTA_AT_ResultUint32         result;
    AT_MTA_ChrAlarmRlatOpUint32 alarmOp;
    VOS_UINT32                  alarmDetail;
} MTA_AT_ChrAlarmRlatCfgSetCnf;


typedef struct {
    VOS_UINT16 transactionId;
    VOS_UINT8  msgBodyType;
    VOS_UINT8  locSource;
    VOS_UINT8  endFlag;
    VOS_UINT8  commonIeValidFlg;
    VOS_UINT8  id;
    VOS_UINT8  locCalcErr;
    VOS_UINT8  nameLength;
    VOS_UINT8  reserved[3];
    VOS_UINT8  name[AT_MTA_EPDU_NAME_LENGTH_MAX];
    VOS_UINT8  totalNum;
    VOS_UINT8  index;
    VOS_UINT16 epduLength;
    VOS_UINT8  epduContent[AT_MTA_EPDU_CONTENT_LENGTH_MAX];
    VOS_UINT8  reserved1[2];
} AT_MTA_EpduSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_EpduSetCnf;


typedef struct {
    VOS_UINT32 freqBegin; /* 单位100khz */
    VOS_UINT32 freqEnd;   /* 单位100khz */
} AT_MTA_FreqRange;


typedef struct {
    VOS_UINT16 bandInd; /* Band:1-255 */
    VOS_UINT8  reserved;
    /* 0-4,0:表示去激活整个Band，频率范围是协议默认范围 */
    VOS_UINT8        freqRangeNum;
    AT_MTA_FreqRange freqRange[AT_MTA_MAX_SUB_BAND_NUM];
} AT_MTA_BandInfo;


typedef struct {
    VOS_UINT8       caA2Flg;  /* CA A2功能开关 */
    VOS_UINT8       caA4Flg;  /* CA A4功能开关 */
    VOS_UINT8       caCqiFlg; /* CA cqi=0功能开关 */
    VOS_UINT8       reserved[3];
    VOS_UINT16      bandNum; /* 0-8,0:表示去使能所有的CA SCell */
    AT_MTA_BandInfo bandInfo[AT_MTA_MAX_BAND_NUM];
} AT_MTA_CaInfo;


typedef struct {
    /* 使能去使能Ca标志，0:去使能  1:使能 */
    VOS_UINT8     caActFlag;
    VOS_UINT8     reserved[3]; /* 保留位 */
    AT_MTA_CaInfo caInfo;
} AT_MTA_CaCfgSetReq;


typedef struct {
    VOS_UINT8             cellIndex; /* 小区索引, 0: PCell, 1~7: SCell */
    MTA_AT_BandWidthUint8 bandWidth; /* SCell占用带宽 */
    VOS_UINT16            bandInd;   /* SCell频段 */

    /* CA上行是否被配置，0:未配置，1:已配置 */
    VOS_UINT8 ulConfigured;
    /* CA下行是否被配置，0:未配置，1:已配置 */
    VOS_UINT8 dlConfigured;
    /* SCell是否被激活，0:去激活，1:激活 */
    VOS_UINT8 actived;
    /* 本SCell是否为LaaScell, 0:不是, 1:是 */
    VOS_UINT8 laaScellFlg;

    VOS_UINT32 earfcn; /* SCell频点 */
} MTA_AT_CaCellInfo;


typedef struct {
    MTA_AT_ResultUint32 result;                           /* 操作结果 */
    VOS_UINT32          totalCellNum;                     /* 上报消息中有效Cell的个数 */
    MTA_AT_CaCellInfo   cellInfo[MTA_AT_CA_MAX_CELL_NUM]; /* 前ulTotalCellNum个元素有效 */
} MTA_AT_CaCellInfoCnf;


typedef struct {
    VOS_UINT32        totalCellNum;                     /* 上报消息中有效Cell的个数 */
    MTA_AT_CaCellInfo cellInfo[MTA_AT_CA_MAX_CELL_NUM]; /* 前ulTotalCellNum个元素有效 */
} MTA_AT_CaCellInfoNtf;


typedef struct {
    VOS_UINT8               fineTimeType;
    VOS_UINT8               forceFlag;
    VOS_UINT16              num;
    MTA_AT_FineTimeRatUint8 ratMode; /* 指定接入模式 */
    VOS_UINT8               reserved[3];
} AT_MTA_FineTimeSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_FineTimeSetCnf;


typedef struct {
    MTA_AT_ResultUint32       result;
    VOS_INT32                 sinr;                                 /* 当前信噪比 */
    VOS_UINT8                 utcTime[MTA_AT_UTC_MAX_LENGTH];       /* (0..549755813887) */
    VOS_UINT8                 utcTimeOffset[MTA_AT_UTC_MAX_LENGTH]; /* (0..549755813887) */
    VOS_INT16                 ta;                                   /* 网侧配置的时间提前量 */
    VOS_INT16                 leapSecond;                           /* 闰秒调整值 */
    VOS_UINT8                 leapSecondValid;                      /* 是否存在闰秒调整 */
    MTA_AT_FineTimeRatUint8   rat;
    MTA_AT_ProtocolStateUint8 state; /* 当前LRRC协议状态 */
    VOS_UINT8                 reserved[3];
} MTA_AT_SibFineTimeInd;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT16          sysFn;
    VOS_UINT8           reserved[2];
} MTA_AT_LppFineTimeInd;


typedef struct {
    MTA_AT_RatModeUint8          overHeatingType; /* 当前接入技术指示LTE、NR */
    AT_MTA_OoverheatingParaUint8 overHeatingFlag; /* 检测到过热，设置参数 */
    VOS_UINT8                    reserved[2];
    VOS_UINT32                   setParamFlag; /* 设置的参数标志位 */
    VOS_UINT32                   param1;       /* 参数1 */
    VOS_UINT32                   param2;       /* 参数2 */
    VOS_UINT32                   param3;       /* 参数3 */
    VOS_UINT32                   param4;       /* 参数4 */
    VOS_UINT32                   param5;       /* 参数5 */
    VOS_UINT32                   param6;       /* 参数6 */
    VOS_UINT32                   param7;       /* 参数7 */
    VOS_UINT32                   param8;       /* 参数8 */
    VOS_UINT32                   param9;       /* 参数9 */
    VOS_UINT32                   param10;      /* 参数10 */
} AT_MTA_OverHeatingSetReq;


typedef struct {
    MTA_AT_RatModeUint8 overHeatingType; /* 当前接入技术指示LTE、NR */
} AT_MTA_OverHeatingQryReq;


typedef struct {
    VOS_UINT32          seq;     /* 流水号 */
    VOS_BOOL            endFlag; /* 是否配置结束的标识 */
    AT_MTA_RatModeUint8 ratMode;
    VOS_UINT8           reserved[3];
    VOS_UINT32          msgLen;
    VOS_UINT8           msg[0];  //lint !e43
} AT_MTA_UeCapParamSet;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_OverHeatingSetCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_UeCapParamSetCnf;


typedef struct {
    MTA_AT_ResultUint32          result;                 /* 结果 */
    AT_MTA_OoverheatingParaUint8 overHeatingSupportFlag; /* 检测到过热，设置参数 */
    VOS_UINT8                    reserved[3];
    VOS_UINT32                   lastReportParamFlag; /* 设置的参数标志位 */
    VOS_UINT32                   param1;              /* 参数1 */
    VOS_UINT32                   param2;              /* 参数2 */
    VOS_UINT32                   param3;              /* 参数3 */
    VOS_UINT32                   param4;              /* 参数4 */
    VOS_UINT32                   param5;              /* 参数5 */
    VOS_UINT32                   param6;              /* 参数6 */
    VOS_UINT32                   param7;              /* 参数7 */
    VOS_UINT32                   param8;              /* 参数8 */
    VOS_UINT32                   param9;              /* 参数9 */
    VOS_UINT32                   param10;             /* 参数10 */
} MTA_AT_OverHeatingQryCnf;


typedef struct {
    VOS_UINT32 msgLen;  /* 由接入层控制不超过2400字节 */
    VOS_UINT8  msg[0];  //lint !e43
} MTA_AT_LrrcUeCapInfoNotify;


typedef struct {
    VOS_UINT32 msgLen;  /* 由接入层控制不超过2400字节 */
    VOS_UINT8  msg[0];  //lint !e43
} MTA_AT_NrrcUeCapInfoNotify;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_CaCellSetCnf;


typedef struct {
    VOS_BOOL blEnableType;
} AT_MTA_CaCellSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_BOOL            blEnableType;
} MTA_AT_CaCellQryCnf;


typedef struct {
    VOS_UINT32 cmdType;
    VOS_UINT32 para1;
    VOS_UINT32 para2;
    VOS_UINT32 para3;
} AT_MTA_L2ComCfgSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_L2ComCfgSetCnf;


typedef struct {
    VOS_UINT32 cmdType;
} AT_MTA_L2ComCfgQryReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          cmdType;
    VOS_UINT32          para1;
    VOS_UINT32          para2;
    VOS_UINT32          para3;
} MTA_AT_L2ComCfgQryCnf;


typedef struct {
    VOS_UINT32 operation;  /* 0:恢复  1:设置 */
    VOS_UINT32 sleepThres; /* 当为设置时有效，表示低功耗状态由active -> sleep的门限，单位:ms */
} AT_MTA_NPdcpSleepThresCfgReq;


typedef struct {
    VOS_UINT32 result; /* 参数配置结果, 0: 成功, 1: 失败 */
    VOS_UINT32 curThres;
} MTA_AT_NPdcpSleepThresCfgCnf;


typedef struct {
    VOS_UINT32 ackType;   /* ACK类型 */
    VOS_UINT32 ackValue1; /* ACK值1 */
    VOS_UINT32 ackValue2; /* ACK值2 */
    VOS_UINT32 ackValue3; /* ACK值3 */
} MTA_AT_PhyCommAckInd;


typedef struct {
    AT_MTA_SmsDomainUint8 smsDomain;
    VOS_UINT8             reserved[3];
} AT_MTA_SmsDomainSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SmsDomainSetCnf;


typedef struct {
    MTA_AT_ResultUint32   result;
    AT_MTA_SmsDomainUint8 smsDomain;
    VOS_UINT8             reserved[3];
} MTA_AT_SmsDomainQryCnf;


typedef struct {
    VOS_UINT32 mcc;
    VOS_UINT32 mnc;
    VOS_UINT8  length;
    VOS_UINT8  epsNetwork[MTA_AT_EPS_NETWORK_BYTE];
    VOS_UINT8  reserved;
} MTA_AT_EpsNetworkInfo;


typedef struct {
    MTA_AT_ResultUint32   result;
    MTA_AT_EpsNetworkInfo epsNetworkInfo;
} MTA_AT_EpsNetworkQryCnf;


typedef struct {
    VOS_UINT32 locationPermitFlag;
} AT_MTA_GpslocsetSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_GpslocsetSetCnf;



typedef struct {
    MTA_AT_ResultUint32 result;
    TIME_ZONE_Time      time;
} MTA_AT_CclkQryCnf;



typedef struct {
    VOS_INT32 tempResult;
} MTA_AT_TempProtectInd;


typedef struct {
    AT_MTA_NvRefreshReasonUint8 reason;
    VOS_UINT8                   reserved[3];
} AT_MTA_NvRefreshSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_NvRefreshSetCnf;


typedef struct {
    VOS_UINT32 pseudBtsType;
} MTA_AT_PseudBtsIdentInd;


typedef struct {
    VOS_UINT16 num; /* 强制脉冲的个数 */
    VOS_UINT8  rsv[2];

} AT_MTA_SetForcesyncReq;


typedef struct {
    VOS_UINT32 result; /* 执行结果 */
} MTA_AT_SetForcesyncCnf;

/*
 * 结构说明: AT向MTA发送LTEPWRDISS请求消息结构体
 */
typedef struct {
    VOS_UINT8 mode;
    VOS_UINT8 reserved;
    VOS_INT16 para;
} AT_MTA_LtepwrdissSetReq;

/*
 * 结构说明: MTA向AT发送LTEPWRDISS请求结果消息结构体
 */
typedef struct {
    MTA_AT_ResultUint32 result;
} AT_MTA_LtepwrdissSetCnf;


typedef struct {
    AT_MTA_DcxoSampleTypeUint16 dcxoSampleType; /* DCXO_SAMPLE样本类型 */
    VOS_UINT8                   rsv[2];
} AT_MTA_DcxoSampeQryReq;

/*
 * 协议表格:
 * ASN.1描述:
 * 结构说明: 存放一组DCXO样本
 */
typedef struct {
    /* 年月，比如2016.4表示值为1604,小于1604表示该样本无效 */
    VOS_INT32 swTime : 24;
    /* 模式，高4bit为modem_id,低4bit为模式，0:G、1:U、2:L、3:TD、4:1X、5:EVDO */
    VOS_INT32 swMode : 8;
    VOS_INT32 swTemp; /* 温度，Boston温度单位0.001度 */
    VOS_INT32 swPpm;  /* 相对频偏，定点化2^31/10^6 */
} MTA_AT_DcxoSample;

/*
 * 协议表格:
 * ASN.1描述:
 * 结构说明:2.2表示普通晶体的初始频偏和温度
 */
typedef struct {
    /* 普通晶体的初始频偏，频偏放大倍数为2^31/10^6 */
    VOS_INT32 swInitFrequency;
    /* 普通晶体的初始温度，温度单位0.001度 */
    VOS_INT32 swInitTemperature;
} MTA_AT_XoInitFrequency;


typedef union {
    MTA_AT_DcxoSample      sample[UCOM_NV_DCXO_SAMPLE_H_MAX_NUM]; /* 不同温段样本数据 */
    MTA_AT_XoInitFrequency dcxoInitPpm;                           /* 拟合曲线系数 */
} MTA_AT_DCXO_SECTION_DATA_UNION_STRU;


typedef struct {
    AT_MTA_DcxoSampleTypeUint16         dcxoSampleType; /* DCXO_SAMPLE样本类型 */
    MTA_AT_DcxoSampleResultUint16       result;
    MTA_AT_DCXO_SECTION_DATA_UNION_STRU sctionData; /* 不同温段和产线校准数据 */
} MTA_AT_DcxoSampeQryCnf;


typedef struct {
    VOS_UINT32 bandCnt;                                /* 个数 */
    VOS_UINT16 bandInfo[AT_MTA_UECAP_MAX_NR_BAND_NUM]; /* Band列表 */
} MTA_NRRC_BandInfo;


typedef struct {
    VOS_UINT32 bandWcdmaI2100 : 1;
    VOS_UINT32 bandWcdmaIi1900 : 1;
    VOS_UINT32 bandWcdmaIii1800 : 1;
    VOS_UINT32 bandWcdmaIv1700 : 1;
    VOS_UINT32 bandWcdmaV850 : 1;
    VOS_UINT32 bandWcdmaVi800 : 1;
    VOS_UINT32 bandWcdmaVii2600 : 1;
    VOS_UINT32 bandWcdmaViii900 : 1;
    VOS_UINT32 bandWcdmaIxJ1700 : 1;
    VOS_UINT32 bandSpare1 : 1;
    VOS_UINT32 bandWcdmaXi1500 : 1;
    VOS_UINT32 bandSpare7 : 7;
    VOS_UINT32 bandWcdmaXix850 : 1;
    VOS_UINT32 bandSpare13 : 13;
} AT_WCDMA_PrefBand;


typedef struct {
    VOS_UINT32 bandGsm450 : 1;
    VOS_UINT32 bandGsm480 : 1;
    VOS_UINT32 bandGsm850 : 1;
    VOS_UINT32 bandGsmP900 : 1;
    VOS_UINT32 bandGsmR900 : 1;
    VOS_UINT32 bandGsmE900 : 1;
    VOS_UINT32 bandGsm1800 : 1;
    VOS_UINT32 bandGsm1900 : 1;
    VOS_UINT32 bandGsm700 : 1;
    VOS_UINT32 bandSpare7 : 23;

} AT_GsmPrefBand;


typedef union {
    VOS_UINT32        band;
    AT_WCDMA_PrefBand bitBand;
} AT_WCDMA_BAND_SET_UN;


typedef union {
    VOS_UINT32     band;
    AT_GsmPrefBand bitBand;
} AT_GSM_BAND_SET_UN;

#if (FEATURE_LTEV == FEATURE_ON)

typedef struct {
    VOS_UINT32 bandNum;
    VOS_UINT32 opBand[MTA_AT_LTEV_BAND_MAX_NUM];
} MTA_AT_LtevBandInfo;
#endif


typedef struct {
    VOS_UINT32           ueGSptBand;
    VOS_UINT32           ueWSptBand;
    VOS_UINT32           allUeBand;
    AT_WCDMA_BAND_SET_UN unWRFSptBand;
    AT_GSM_BAND_SET_UN   unGRFSptBand;
    MTA_NRRC_BandInfo    nrBand;
#if (FEATURE_LTEV == FEATURE_ON)
    MTA_AT_LtevBandInfo  ltevBand;
#endif
} AT_UE_BandCapaSt;


typedef struct {
    MTA_AT_ResultUint32 result;
    AT_UE_BandCapaSt    bandFeature;
} MTA_AT_SfeatureQryCnf;

#if (FEATURE_ON == FEATURE_UE_MODE_NR)

typedef struct {
    VOS_UINT32 nrrcCfgNetMode;
    VOS_UINT32 para[AT_MTA_NRRCCAP_PARA_MAX_NUM];
} AT_MTA_NrrccapCfgSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_NrrccapCfgSetCnf;


typedef struct {
    VOS_UINT32 nrrcCfgNetMode;
} AT_MTA_NrrccapQryReq;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          nrrcCfgNetMode;
    VOS_UINT32          para[AT_MTA_NRRCCAP_PARA_MAX_NUM];
} MTA_AT_NrrccapQryCnf;


typedef struct {
    VOS_UINT8 mode;
    VOS_UINT8 reserved;
    VOS_INT16 para;
} AT_MTA_NrpwrctrlSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_NrpwrctrlSetCnf;


typedef struct {
    VOS_UINT32 saSupportFlg;
} MTA_AT_NrNwSaCapInfoPara;


typedef struct {
    VOS_UINT32 para1;
    VOS_UINT32 para2;
    VOS_UINT32 para3;
    VOS_UINT32 para4;
    VOS_UINT32 para5;
    VOS_UINT32 para6;
    VOS_UINT32 para7;
    VOS_UINT32 para8;
    VOS_UINT32 para9;
    VOS_UINT32 para10;
} MTA_AT_NrNwCapInfoCommPara;


typedef union {
    /* nwCapType=0时对应的网络能力信息 */
    MTA_AT_NrNwSaCapInfoPara   nrSaPara;
    MTA_AT_NrNwCapInfoCommPara commPara;
} MTA_AT_NrNwCapInfoPara;


enum AT_MTA_NwCapInfoType {
    /* 端管标识指示的网络SA能力 */
    AT_MTA_NR_NW_CAP_INFO_SA = 1,
    AT_MTA_NR_NW_CAP_INFO_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_NrNwCapInfoTypeUint32;


enum AT_MTA_NrNwCapOperType {
    AT_MTA_NR_NW_CAP_RPT_CFG_SET = 0,
    AT_MTA_NR_NW_CAP_OPER_TYPE_BUTT
};
typedef VOS_UINT32 AT_MTA_NrNwCapOperTypeUint32;


typedef struct {
    AT_MTA_NrNwCapInfoTypeUint32 type;
    MTA_AT_NrNwCapInfoPara       capInfo;
} MTA_AT_NrNwCapInfoReportInd;


typedef struct {
    AT_MTA_NrNwCapInfoTypeUint32 type;
    VOS_UINT32                   rptFlg;
} AT_MTA_NrNwCapInfoRptCfgSetReq;

/*
 * 网络能力相关操作处理函数指针
 */
typedef VOS_VOID (*TAF_MTA_NrNWCAPOPER_PROC_FUNC_PTR)(AT_MTA_NrNwCapInfoTypeUint32 capType, VOS_UINT32 reserve);


typedef struct {
    AT_MTA_NrNwCapOperTypeUint32 opertype;
    AT_MTA_NrNwCapInfoTypeUint32 capType;
    TAF_MTA_NrNWCAPOPER_PROC_FUNC_PTR operProcFunc;
} MTA_AT_NrNwCapOperProcMapdTbl;

#endif


typedef struct {
    VOS_UINT16 ulMcs[MTA_AT_MAX_CODE_NUMBER];
    VOS_UINT16 dlMcs[MTA_AT_MAX_CODE_NUMBER];
} AT_LTE_McsInfo;


typedef struct {
    MTA_AT_UlModeUint32 ulMode;
    /* NR同一个MCS支持多个调制表，这里表示调制方式 3GPP TS38.214 5.1.3.1 6.1.4.1 */
    MTA_AT_IndexTableUint8 ulMcsTable;
    MTA_AT_IndexTableUint8 sulMcsTable;
    MTA_AT_IndexTableUint8 dlMcsTable;
    VOS_UINT8              rsv;
    VOS_UINT16             ulMcs[MTA_AT_MAX_CODE_NUMBER];
    VOS_UINT16             sulMcs[MTA_AT_MAX_CODE_NUMBER];
    VOS_UINT16             dlMcs[MTA_AT_MAX_CODE_NUMBER];
} AT_NR_McsInfo;


typedef struct {
    MTA_AT_ResultUint32    result;
    MTA_AT_ResultTypeUint8 resultType;
    VOS_UINT8              rsv;
    VOS_UINT16             nrCellNum;
    AT_LTE_McsInfo         lteMcsInfo;
    AT_NR_McsInfo          nrMcsInfo[MTA_AT_MAX_CC_NUMBER];
} MTA_AT_McsQryCnf;


typedef struct {
    VOS_UINT32 ulFreq;
    VOS_UINT32 ulEarfcn;
    VOS_UINT32 ulBandWidth;
    VOS_UINT32 dlFreq;
    VOS_UINT32 dlEarfcn;
    VOS_UINT32 dlBandWidth;
    VOS_UINT16 band;
    VOS_UINT8  rsv[2];
} AT_LTE_HfreqInfo;


typedef struct {
    MTA_AT_UlModeUint32 ulMode;
    VOS_UINT32          ulFreq;
    VOS_UINT32          ulNarfcn;
    VOS_UINT32          ulBandWidth;
    VOS_UINT32          sulFreq;
    VOS_UINT32          sulNarfcn;
    VOS_UINT32          sulBandWidth;
    VOS_UINT32          dlFreq;
    VOS_UINT32          dlNarfcn;
    VOS_UINT32          dlBandWidth;
    VOS_UINT16          band;
    VOS_UINT16          sulBand;
} AT_NR_HfreqInfo;

typedef struct {
    MTA_AT_ResultUint32    result;
    MTA_AT_ResultTypeUint8 resultType;
    VOS_UINT8              isreportFlg;
    VOS_UINT16             nrCellNum;
    AT_LTE_HfreqInfo       lteHfreqInfo;
    AT_NR_HfreqInfo        nrHfreqInfo[MTA_AT_MAX_CC_NUMBER];
} MTA_AT_HfreqinfoQryCnf;


typedef struct {
    MTA_AT_ResultUint32    result;
    MTA_AT_ResultTypeUint8 resultType;
    VOS_UINT8              rsv;
    VOS_UINT16             nrCellNum;
    AT_LTE_HfreqInfo       lteHfreqInfo;
    AT_NR_HfreqInfo        nrHfreqInfo[MTA_AT_MAX_CC_NUMBER];
} MTA_AT_HfreqinfoInd;


typedef struct {
    VOS_INT16 guTxPwr;
    VOS_INT16 puschPwr;
    VOS_INT16 pucchPwr;
    VOS_INT16 srsPwr;
    VOS_INT16 prachPwr;
    VOS_UINT8 rsv[2];
} AT_TXPOWER_Info;


typedef struct {
    MTA_AT_UlModeUint32 ulMode;
    VOS_UINT32          ulFreq; /* 工作频率以及UL发射功率 */
    VOS_INT16           ulPuschPwr;
    VOS_INT16           ulPucchPwr;
    VOS_INT16           ulSrsPwr;
    VOS_INT16           ulPrachPwr;
    VOS_UINT32          sulFreq; /* SUL工作频率以及SUL发射功率 */
    VOS_INT16           sulPuschPwr;
    VOS_INT16           sulPucchPwr;
    VOS_INT16           sulSrsPwr;
    VOS_INT16           sulPrachPwr;
} AT_NTXPOWER_Info;


typedef struct {
    MTA_AT_ResultUint32 result;
    AT_TXPOWER_Info     txpwrInfo;
} MTA_AT_TxpowerQryCnf;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT16          nrCellNum;
    VOS_UINT8           rsv[2];
    AT_NTXPOWER_Info    nrTxPwrInfo[MTA_AT_MAX_CC_NUMBER];
} MTA_AT_NtxpowerQryCnf;


typedef struct {
    AT_MTA_NssaiSetTypeUint32 sNssaiSetType;
    PS_CONFIGURED_Nssai       st5gNssai;
} AT_MTA_5GNssaiSetReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_5GNssaiSetCnf;


typedef struct {
    VOS_UINT32                opPlmn : 1;
    VOS_UINT32                opSpare : 31;
    PS_NR_PlmnId              plmn;
    AT_MTA_NssaiQryTypeUint32 sNssaiQryType;
} AT_MTA_5GNssaiQryReq;


typedef struct {
    PS_NR_PlmnId        plmnId;
    PS_ALLOW_Nssai      allowNssai;
    PS_CONFIGURED_Nssai cfgNssai;
} MTA_AT_5GAllowedAndCfgNssaiWithPlmnId;


typedef struct {
    VOS_UINT32                            plmnNum;
    MTA_AT_5GAllowedAndCfgNssaiWithPlmnId nssai[MTA_AT_MAX_ALLOWED_AND_CFG_NSSAI_PLMN_NUM];
} MTA_AT_5GAllowedAndCfgNssaiQryCnf;


typedef struct {
    MTA_AT_ResultUint32               result;
    AT_MTA_NssaiQryTypeUint32         sNssaiQryType;
    PS_CONFIGURED_Nssai               st5gDefCfgNssai;   /* default config nssai */
    PS_CONFIGURED_Nssai               st5gPreferNssai;   /* prefer nssai */
    PS_REJECTED_NssaiInfo             st5gRejNssai;      /* reject nssai */
    MTA_AT_5GAllowedAndCfgNssaiQryCnf st5gAllowCfgNssai; /* allow以及cfg nssai */
} MTA_AT_5GNssaiQryCnf;


typedef struct {
    VOS_UINT32 unsolicitedRptFlg; /* 设置是否主动上报land info, 0:不主动上报；1:主动上报 */
} AT_MTA_SetLadnRptReq;


typedef struct {
    MTA_AT_ResultUint32 result;
} MTA_AT_SetLadnRptCnf;


typedef struct {
    VOS_UINT32 availFlg; /* DNN是否可用，VOS_TRUE:可用；VOS_FALSE:不可用 */
    PS_APN     dnn;
} MTA_AT_LadnDnn;


typedef struct {
    MTA_AT_ResultUint32 result;
    VOS_UINT32          rptFlg;
    VOS_UINT32          ladnDnnNum;
    MTA_AT_LadnDnn      ladnList[MTA_AT_MAX_LADN_DNN_NUM];
} MTA_AT_QryLadnInfoCnf;


typedef struct {
    VOS_UINT32     ladnDnnNum;
    MTA_AT_LadnDnn ladnList[MTA_AT_MAX_LADN_DNN_NUM];
} MTA_AT_LadnInfoInd;


typedef struct {
    VOS_UINT8                  highSpeedFlg; /* GU直接填0 */
    MTA_AT_HsrcellInfoRatUint8 rat;
    VOS_INT16                  rsrp;
    VOS_UINT32                 cellIdLowBit;  /* 小区ID低4字节 */
    VOS_UINT32                 cellIdHighBit; /* 小区ID高4字节，支持NR Cell Id扩展 */
} MTA_AT_HsrcellInfoInd;


typedef struct {
    VOS_UINT8 ssbId; /* SSBID */
    VOS_UINT8 rsv;
    VOS_INT16 rsrp; /* RSRP测量结果 */
} MTA_AT_NrRsrpMeasRslt;


typedef struct {
    VOS_UINT32            arFcn;     /* arfcn，不支持返回-1 */
    VOS_UINT16            phyCellId; /* 物理层小区ID */
    VOS_UINT8             rsv1[2];
    VOS_UINT64            cgi; /* Cell Global Identity，NSA下没有(返回0xFFFFFFFFFFFFFFFF)，SA下也不一定查到，时序相关 */
    VOS_INT16             rsrp;                                       /* PCELL的RSRP测量结果，如果不存在，填写0x7FFF */
    VOS_INT16             sinr;                                       /* PCELL的SINR测量结果，如果不存在，填写0x7FFF */
    VOS_INT32             ta;                                         /* time advance */
    MTA_AT_NrRsrpMeasRslt pscellMeasRslt[MTA_AT_MAX_NR_SSB_BEAM_NUM]; /* 服务区SSB ID对应的参考信号接收功率 */
} MTA_AT_NrPscellDescription;


typedef struct {
    VOS_UINT32            arFcn;     /* arfcn，邻区小区SSB频点，不支持返回0xFFFFFFFF */
    VOS_UINT16            phyCellId; /* NR邻区物理层小区ID */
    VOS_INT16             rsrp;      /* 邻区参考信号接收功率 */
    VOS_INT16             sinr;      /* 邻区信号干扰噪声比，无效值0x7FFF */
    VOS_UINT8             rsv[2];
    MTA_AT_NrRsrpMeasRslt ncellMeasRslt[MTA_AT_MAX_NR_NCELL_BEAM_RPT_NUM]; /* SSB测量结果=RSRP+SINR */
} MTA_AT_NrNcellDescription;


typedef struct {
    MTA_AT_ResultUint32        result;        /* 查询结果是否成功 */
    VOS_UINT8                  spcellPresent; /* 此标志位不为TRUE，后面pscell和ncell的信息都无效 */
    VOS_UINT8                  ncellPresent;  /* 此标志位不为TRUE，后面ncell的信息都无效 */
    VOS_UINT8                  ncellNum;
    VOS_UINT8                  rsv;
    MTA_AT_NrPscellDescription spcellSsbInfo;
    MTA_AT_NrNcellDescription  ncellSsbInfo[MTA_AT_MAX_NR_NCELL_RPT_NUM]; /* 连接态邻区测量信息 */
} MTA_AT_NrSsbIdQryCnf;
/*****************************************************************************
  10 函数声明
*****************************************************************************/

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
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
