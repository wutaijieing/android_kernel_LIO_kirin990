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


#ifndef __SI_STK_H__
#define __SI_STK_H__

#include "vos.h"

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "pam_om.h"
#include "usimm_api.h"
#include "nas_stk_interface.h"
#include "ccore_nv_stru_pam.h"
#include "ccore_nv_stru_gunas.h"
#include "imsa_stk_interface.h"
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "pam_app_om.h"
#endif

#ifndef MODEM_FUSION_VERSION
#include "msp_diag_comm.h"
#else
#include "mdrv_diag.h"
#endif

#include "mdrv_nvim.h"

#include "taf_oam_interface.h"
#include "nv_stru_pam.h"
#include "nv_stru_gucnas.h"
#include "at_oam_interface.h"
#include "mn_error_code.h"
#include "taf_mmi_str_parse.h"

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "taf_xsms_stk_interface.h"
#include "taf_xsms_decode.h"
#endif
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "si_acore_api.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

typedef VOS_UINT32 (*TLV2DS)(VOS_UINT8 *pDes, VOS_UINT8 *pSrc);

typedef VOS_UINT32 (*DS2TLV)(VOS_UINT8 Tag, VOS_UINT8 *pDes, VOS_UINT8 *pSrc, VOS_UINT8 len);

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (VOS_OS_VER == VOS_WIN32))
#define STK_GEN_LOG_MODULE(Level) (MDRV_DIAG_GEN_LOG_MODULE(MODEM_ID_0, DIAG_MODE_COMM, Level))

#define STK_KEY_INFO_LOG(string)                                                                             \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s", string)
#define STK_KEY_NORMAL_LOG(string)                                                                             \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s", string)
#define STK_KEY_WARNING_LOG(string)                                                                             \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "WARNING:%s", string)
#define STK_KEY_ERROR_LOG(string)                                                                             \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "ERROR:%s", string)

#define STK_KEY_INFO_LOG1(string, para1)                                                                     \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s,%d", string, para1)
#define STK_KEY_NORMAL_LOG1(string, para1)                                                                     \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s,%d", string, para1)
#define STK_KEY_WARNING_LOG1(string, para1)                                                                     \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "WARNING:%s,%d", string, para1)
#define STK_KEY_ERROR_LOG1(string, para1)                                                                     \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "ERROR:%s,%d", string, para1)

#define STK_INFO_LOG(string)                                                                                 \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s", string)
#define STK_NORMAL_LOG(string)                                                                                 \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s", string)
#define STK_WARNING_LOG(string)                                                                                 \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "WARNING:%s", string)
#define STK_ERROR_LOG(string)                                                                                 \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "ERROR:%s", string)

#define STK_INFO_LOG1(string, para1)                                                                         \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s,%d", string, para1)
#define STK_NORMAL_LOG1(string, para1)                                                                         \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "NORMAL:%s,%d", string, para1)
#define STK_WARNING_LOG1(string, para1)                                                                         \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "WARNING:%s,%d", string, para1)
#define STK_ERROR_LOG1(string, para1)                                                                         \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "ERROR:%s,%d", string, para1)

#define STK_WARNING_NOSLOTID_LOG(string)                                                                        \
    (VOS_VOID)mdrv_diag_log_report(STK_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING), I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                             "WARNING:%s", string)

#elif (OSA_CPU_CCPU == VOS_OSA_CPU)

#define STK_KEY_WARNING_LOG(string) \
    USIMM_LogPrint(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)
#define STK_KEY_ERROR_LOG(string) \
    USIMM_LogPrint(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s", string)

#define STK_KEY_INFO_LOG1(string, para1) \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "Info:%s,%d", string, para1)
#define STK_KEY_NORMAL_LOG1(string, para1)                                                                          \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "Normal:%s,%d", string, \
                    para1)
#define STK_KEY_WARNING_LOG1(string, para1)                                                                           \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s,%d", string, \
                    para1)
#define STK_KEY_ERROR_LOG1(string, para1) \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s,%d", string, para1)

#define STK_INFO_LOG(string) \
    USIMM_LogPrint(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR *)__FUNCTION__)

#define STK_WARNING_NOSLOTID_LOG(string)                                                                         \
    USIMM_LogPrint(USIMM_GetCurrInstanceSlotId(), PS_LOG_LEVEL_WARNING, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, \
                   "Warning:%s", string)

#define STK_NORMAL_LOG(string)                                                                   \
    USIMM_LogPrint(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s", \
                   (VOS_CHAR *)__FUNCTION__)
#define STK_WARNING_LOG(string)                                                                   \
    USIMM_LogPrint(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s", \
                   (VOS_CHAR *)__FUNCTION__)
#define STK_ERROR_LOG(string)                                                                   \
    USIMM_LogPrint(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s", \
                   (VOS_CHAR *)__FUNCTION__)

#define STK_INFO_LOG1(string, para1)                                                                \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s: %d", \
                    (VOS_CHAR *)__FUNCTION__, para1)
#define STK_NORMAL_LOG1(string, para1)                                                                \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s: %d", \
                    (VOS_CHAR *)__FUNCTION__, para1)
#define STK_WARNING_LOG1(string, para1)                                                                \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s: %d", \
                    (VOS_CHAR *)__FUNCTION__, para1)
#define STK_ERROR_LOG1(string, para1)                                                                \
    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_STK_PID, VOS_NULL_PTR, __LINE__, "%s: %d", \
                    (VOS_CHAR *)__FUNCTION__, para1)

#endif

#define SI_SAT_OFFSET(A, b) ((VOS_UINT16)(&(((A *)0)->b)))

#define SI_STK_GET_MCC(num) \
    (100 * ((num % 1000 / 100 + 9) % 10) + 10 * ((num % 100 / 10 + 9) % 10) + (num % 10 + 9) % 10)

#define SI_STK_GET_MNC(num) (10 * ((num % 100 / 10 + 9) % 10) + (num % 10 + 9) % 10)

#define SI_STK_TAGNOTFOUND (0xFFFFFFFF)

#define STK_DATA_TAG_MASK (0x80) /* 按照协议,Data Tag的最高bit可以是1 */

#define STK_POROTECT_TIME_LEN (20) /* 默认20秒超时 */

#define STK_IND_TIME_LEN (5000)

#define STK_DURATION_MIN (0x00)
#define STK_DURATION_SEC (0x01)
#define STK_DURATION_TENTH_SEC (0x02)

#define STK_TIME_MANAGEMENT_START (0x00)
#define STK_TIME_MANAGEMENT_DEACTIVE (0x01)
#define STK_TIME_MANAGEMENT_GET_VALUE (0x02)

#define STK_TIMER_MAX_NUM (9)

/* 返回的TR数据长度，按照结果只有一个字节计算 */
#define STK_TERMINAL_RSP_LEN (12)

#define STK_LOCAL_INFO_2G (9)

#define STK_ENVELOPE_MAX_LEN (256)

#define STK_SMS_MAX_MSG_SEGMENT_NUM (2)

#define STK_TIMER_SECOND_VALUE_MAX (60)
#define STK_TIMER_MINUTE_VALUE_MAX (60)
#define STK_TIMER_HOUR_VALUE_MAX (17)

#define STK_PROVIDE_LOCAL_INFO_NAA (0x00)
#define STK_PROVIDE_LOCAL_INFO_IMEI (0x01)
#define STK_PROVIDE_LOCAL_INFO_NMR (0x02)
#define STK_PROVIDE_LOCAL_INFO_TIME (0x03)
#define STK_PROVIDE_LOCAL_INFO_LANGUAGE (0x04)
#define STK_PROVIDE_LOCAL_INFO_RES_GSM (0x05)
#define STK_PROVIDE_LOCAL_INFO_ACCESS_TECH (0x06)
#define STK_PROVIDE_LOCAL_INFO_ESN (0x07)
#define STK_PROVIDE_LOCAL_INFO_IMEISV (0x08)
#define STK_PROVIDE_LOCAL_INFO_SEARCH_MODE (0x09)
#define STK_PROVIDE_LOCAL_INFO_BATTERY_STATE (0x0A)
#define STK_PROVIDE_LOCAL_INFO_MEID (0x0B)
#define STK_PROVIDE_LOCAL_INFO_RES_3GPP (0x0C)

#define STK_IMEI_LEN (14)
#define STK_IMEISV_LEN (16)
#define STK_IMEI_TYPE (0x0A)
#define STK_IMEISV_TYPE (0x03)

#define STK_SEND_SMS_OP_ID (0x00)
#define STK_SEND_SS_OP_ID (0x01)
#define STK_SEND_USSD_OP_ID (0x02)
#define STK_SETUP_CALL_OP_ID (0x03)
#define STK_SETUP_CALL_DTMF_OP_ID (0x04)
#define STK_SEND_DTMF_OP_ID (0x05)

#define STK_NV_ENABLED (0x0101)

/* profile 特别bit */
#define SI_STK_LOCALINFO_NETINFO (30)
#define SI_STK_LOCALINFO_NMR (31)
#define SI_STK_GET_READER_STATUS (51)
#define SI_STK_GET_READER_ID (52)
#define SI_STK_TIMER_MM_START (56)
#define SI_STK_TIMER_MM_STOP (57)
#define SI_STK_LOCALINFO_TIME (58)
#define SI_STK_CALL_CONTROL (63)
#define SI_STK_LOCALINFO_LANGUAGE (67)
#define SI_STK_LOCALINFO_TA (68)
#define SI_STK_LOCALINFO_ACCESS (71)

#define STK_OPID_VALUE STK_SEND_SMS_OP_ID

/* NAS支持最大呼叫数 */
#define STK_CALL_MAX_NUM (7)

/* 定义parse字符 */
#define STK_PAUSE_CHAR (0x0c)

/* 挂断所有呼叫的ID */
#define STK_ALL_CALL_ID (0)

/* DURATION定时器默认时长，等待NAS回复CONNECT消息 */
#define STK_SETUP_CALL_DURATION_DEFAULT (60)

/* 发送DTMF时暂停的时间长度，单位为MS */
#define STK_SEND_DTMF_PAUSE_TIME (3000)

#define STK_SSD_OR_USSD_DCS_LEN 420

/* STK重试呼叫的最大次数 */
#define STK_RECALL_TIME_MAX (3)

#define STK_BURSTDTMF_LEN (150)

#define STK_ONLY_TAG_LEN_SIZE (2)

#define STK_RESEND_TIMER_START(Pid, TimerId, TimerName, Length) \
    (VOS_VOID)VOS_StopRelTimer(TimerId);                        \
    (VOS_VOID)VOS_StartRelTimer(TimerId, Pid, Length * 1000, TimerName, 0, VOS_RELTIMER_NOLOOP, VOS_TIMER_PRECISION_5)

#define STK_RESEND_TIMER_STOP(TimerId) VOS_StopRelTimer(TimerId)

#define STK_BCD2HEX(x) ((VOS_UINT8)((VOS_UINT8)(((x)&0xF0) >> 4) + (VOS_UINT8)(((x)&0x0F) * 0x0A)))

#define STK_HEX2BCD(x) \
    (((x) < 10) ? (VOS_UINT8)((x) << 4) : ((VOS_UINT8)((VOS_UINT8)(((x) / 10) & 0x0F) + (VOS_UINT8)(((x) % 10) << 4))))

#define STK_ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

/* 检查TAF回复的PS_USIM_ENVELOPE_CNF_STRU消息中，ucDataType的合法性 */
#define STK_IS_TAF_ENVELOPE_CNF_DATATYPE_VALID(dataType)                                  \
    ((SI_STK_ENVELOPE_PPDOWN == dataType) || (SI_STK_ENVELOPE_CBDOWN == dataType) ||    \
     (SI_STK_ENVELOPE_CALLCRTL == dataType) || (SI_STK_ENVELOPE_SMSCRTL == dataType) || \
     (SI_STK_ENVELOPE_USSDDOWN == dataType))

enum SI_STK_TimerName {
    STK_PROTECT_TIMER_NAME             = 0,
    STK_TIMER_MANAGEMENT_TIMER1        = 1,
    STK_TIMER_MANAGEMENT_TIMER2        = 2,
    STK_TIMER_MANAGEMENT_TIMER3        = 3,
    STK_TIMER_MANAGEMENT_TIMER4        = 4,
    STK_TIMER_MANAGEMENT_TIMER5        = 5,
    STK_TIMER_MANAGEMENT_TIMER6        = 6,
    STK_TIMER_MANAGEMENT_TIMER7        = 7,
    STK_TIMER_MANAGEMENT_TIMER8        = 8,
    STK_IND_TIMER_NAME                 = 9,
    STK_REFRESH_TIMER_NAME             = 10,
    STK_GETTA_TIMER_NAME               = 11,
    STK_SETUP_CALL_DURATION_TIMER_NAME = 12,
    STK_DTMF_PAUSE_TIMER_NAME          = 13,
    STK_NMR_TIMER_NAME                 = 14,
    STK_LOCIEVENT_TIMER_NAME           = 15,
    STK_ACCEVENT_TIMER_NAME            = 16,
    STK_ENVELOP_PPWD_TIMER_NAME        = 17,

    SI_STK_TIMERNAME_BUTT
};
typedef VOS_UINT32 SI_STK_TimerNameUint32;

enum SI_STK_CmdProcStatus {
    SI_STK_NORMAL_CNF  = 0, /* 正常回复 */
    SI_STK_WAITING_CNF = 1, /* 等待回复 */
    SI_STK_TIMEOUT_CNF = 2, /* 超时回复 */
    SI_STK_BUTT
};
typedef VOS_UINT32 SI_STK_CmdProcStatusUint32;

enum SI_STK_TagType {
    SI_TAG_TYPE_NULL = 0, /* 数据结构不确定 */
    SI_TAG_TYPE_LV   = 1, /* LV 数据结构 */
    SI_TAG_TYPE_TLV  = 2, /* TLV数据结构 */
    SI_TAG_TYPE_V    = 3, /* V 数据结构 */
    SI_TAG_BUTT
};
typedef VOS_UINT8 SI_STK_TagTypeUint8;

enum SI_DecodeResult {
    SI_STK_OK          = COMMAND_PERFORMED_SUCCESSFULLY,
    SI_STK_CMDEND      = PROACTIVE_UICC_SESSION_TERMINATED_BY_THE_USER,
    SI_STK_HANDLEERROR = TERMINAL_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
    SI_STK_NOTSUPPORT  = COMMAND_BEYOND_TERMINALS_CAPABILITIES,
    SI_STK_TYPE_ERROR  = COMMAND_TYPE_NOT_UNDERSTOOD_BY_TERMINAL,
    SI_STK_DATA_ERROR  = COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL,
    SI_STK_OK_WAITRP   = 0x40,
    SI_STK_NORP        = 0x41,
    SI_DECODE_RESULT_BUTT
};

enum SI_CodedataType {
    SI_CODE_TR_DATA       = 0x00,
    SI_CODE_EVENT_DATA    = 0x01,
    SI_CODE_ENVELOPE_DATA = 0x02,
    SI_CODE_DATA_BUTT
};
typedef VOS_UINT32 SI_CodedataTypeUint32;

enum SI_STK_DataTag {
    COMMAND_DETAILS_TAG            = 0x01,
    DEVICE_IDENTITY_TAG            = 0x02,
    RESULT_TAG                     = 0x03,
    DURATION_TAG                   = 0x04,
    ALPHA_IDENTIFIER_TAG           = 0x05,
    ADDRESS_TAG                    = 0x06,
    CAP_CFG_PARA_TAG               = 0x07,
    SUBADDRESS_TAG                 = 0x08,
    SS_STRING_TAG                  = 0x09,
    USSD_STRING_TAG                = 0x0A,
    SMS_TPDU_TAG                   = 0x0B,
    CELL_BROADCAST_PAGE_TAG        = 0x0C,
    TEXT_STRING_TAG                = 0x0D,
    TONE_TAG                       = 0x0E,
    ITEM_TAG                       = 0x0F,
    ITEM_IDENTIFIER_TAG            = 0x10,
    RESPONSE_LENGTH_TAG            = 0x11,
    FILE_LIST_TAG                  = 0x12,
    LOCATION_INFORMATION_TAG       = 0x13,
    IMEI_TAG                       = 0x14,
    HELP_REQUEST_TAG               = 0x15,
    NET_MEASUREMENT_RESULTS_TAG    = 0x16,
    DEFAULT_TEXT_TAG               = 0x17,
    MS_NEXTACTION_INDICATOR_TAG    = 0x18,
    EVENT_LIST_TAG                 = 0x19,
    CAUSE_TAG                      = 0x1A,
    LOCATION_STATUS_TAG            = 0x1B,
    TRANSACTION_IDENTIFIER_TAG     = 0x1C,
    BCCH_CHANNEL_LIST_TAG          = 0x1D,
    ICON_IDENTIFIER_TAG            = 0x1E,
    ITEM_ICONID_LIST_TAG           = 0x1F,
    CARD_READER_STATUS_TAG         = 0x20,
    CARD_ATR_TAG                   = 0x21,
    C_APDU_TAG                     = 0x22,
    R_APDU_TAG                     = 0x23,
    TIMER_IDENTIFIER_TAG           = 0x24,
    TIMER_VALUE_TAG                = 0x25,
    DATETIME_AND_TIMEZONE_TAG      = 0x26,
    CALL_CONTROL_REQACTION_TAG     = 0x27,
    AT_COMMAND_TAG                 = 0x28,
    AT_RESPONSE_TAG                = 0x29,
    BC_REPEAT_INDICATOR_TAG        = 0x2A,
    IMMEDIATE_RESPONSE_TAG         = 0x2B,
    DTMF_STRING_TAG                = 0x2C,
    LANGUAGE_TAG                   = 0x2D,
    TIMING_ADVANCE_TAG             = 0x2E,
    AID_TAG                        = 0x2F,
    BROWSER_IDENTITY_TAG           = 0x30,
    URL_TAG                        = 0x31,
    BEARER_TAG                     = 0x32,
    PROVISIONING_REFFILE_TAG       = 0x33,
    BROWSER_TERMINATION_CAUSE_TAG  = 0x34,
    BEARER_DESCRIPTION_TAG         = 0x35,
    CHANNEL_DATA_TAG               = 0x36,
    CHANNEL_DATA_LENGTH_TAG        = 0x37,
    CHANNEL_STATUS_TAG             = 0x38,
    BUFFER_SIZE_TAG                = 0x39,
    CARD_READER_IDENTIFIER_TAG     = 0x3A, /* 该TAG和 Refresh enforcement policy 共用 */
    RFU_3B                         = 0x3B,
    UICC_TERMINAL_TRAN_LEVEL_TAG   = 0x3C,
    RFU_3D                         = 0x3D,
    OTHER_ADDR_TAG                 = 0x3E,
    ACCESS_TECHNOLOGY_TAG          = 0x3F,
    DISPLAY_PARAMETERS_TAG         = 0x40,
    SERVICE_RECORD_TAG             = 0x41,
    DEVICE_FILTER_TAG              = 0x42,
    SERVICE_SEARCH_TAG             = 0x43,
    ATTRIBUTE_INFORMATION_TAG      = 0x44,
    SERVICE_AVAILABILITY_TAG       = 0x45,
    ESN_TAG                        = 0x46,
    NETWORK_ACCESS_NAME_TAG        = 0x47,
    CDMA_SMS_TPDU                  = 0x48,
    REMOTE_ENTITY_ADDRESS_TAG      = 0x49,
    I_WLAN_ID_TAG                  = 0x4A,
    I_WLAN_ACCESS_STATUS_TAG       = 0x4B,
    RFU_4C                         = 0x4C,
    RFU_4D                         = 0x4D,
    RFU_4E                         = 0x4E,
    RFU_4F                         = 0x4F,
    TEXT_ATTRIBUTE_TAG             = 0x50,
    ITEM_TEXT_ATTRIBUTE_LIST_TAG   = 0x51,
    PDP_CONTEXT_ACTIVATION_PAR_TAG = 0x52,
    RFU_53                         = 0x53,
    RFU_54                         = 0x54,
    CSG_CELL_SELEC_STATUS_TAG      = 0x55,
    CSG_ID_TAG                     = 0x56,
    HNB_NAME_TAG                   = 0x57,
    RFU_58                         = 0x58,
    RFU_59                         = 0x59,
    RFU_5A                         = 0x5A,
    RFU_5B                         = 0x5B,
    RFU_5C                         = 0x5C,
    RFU_5D                         = 0x5D,
    RFU_5E                         = 0x5E,
    RFU_5F                         = 0x5F,
    RFU_60                         = 0x60,
    CDMA_ECC_TAG                   = 0x61,
    IMEISV_TAG                     = 0x62,
    BATTERY_STATE_TAG              = 0x63,
    BROWSING_STATUS_TAG            = 0x64,
    NETWORK_SEARCH_MODE_TAG        = 0x65,
    FRAME_LAYOUT_TAG               = 0x66,
    FRAMES_INFORMATION_TAG         = 0x67,
    FRAME_IDENTIFIER_TAG           = 0x68,
    UTRAN_MEASUREMENT_TAG          = 0x69,
    MMS_REFERENCE_TAG              = 0x6A,
    MMS_IDENTIFIER_TAG             = 0x6B,
    MMS_TRANSFER_STATUS_TAG        = 0x6C,
    MEID_TAG                       = 0x6D,
    MMS_CONTENT_ID_TAG             = 0x6E,
    MMS_NOTIFICATION_TAG           = 0x6F,
    LAST_ENVELOPE_TAG              = 0x70,
    RFU_62                         = 0x71,
    PLMNWACT_LIST_TAG              = 0x72,
    ROUTING_AREA_INFO_TAG          = 0x73,
    ATTACH_TYPE_TAG                = 0x74,
    REJETION_CAUSE_CODE_TAG        = 0x75,
    GEOGRAPH_LOCAL_PARA_TAG        = 0x76,
    GAD_SHAPES_TAG                 = 0x77,
    NMEA_SENTENCE_TAG              = 0x78,
    PLMN_LIST_TAG                  = 0x79,
    RFU_7A                         = 0x7A,
    RFU_7B                         = 0x7B,
    EPSPDN_ACTIVE_PARA_TAG         = 0x7C,
    TRACKING_AREA_ID_TAG           = 0x7D,
    CSG_ID_LIST_TAG                = 0x7E,
    SI_STK_TAG_BUTT
};

enum SI_STK_Lock {
    SI_STK_PROUNLOCK = 0x00,
    SI_STK_PROLOCK   = 0x01,
    SI_STK_LOCK_BUTT
};
typedef VOS_UINT32 SI_STK_LockUint32;

enum SI_STK_Timerstate {
    SI_STK_TIMERSTOP = 0x00,
    SI_STK_TIMERRUN  = 0x01,
    SI_STK_TIMERBUTT
};
typedef VOS_UINT32 SI_STK_TimerstateUint32;

enum SI_STK_StrChangemode {
    SI_STK_STR_CHANGEBIG   = 0x00,
    SI_STK_STR_CHANGESMALL = 0x01,
    SI_STK_STR_CHANGE_BUTT
};
typedef VOS_UINT32 SI_STK_StrChangemodeUint32;

/* 沃达丰Dual IMSI 切换定制需求 */
enum SI_STK_ImsichgState {
    SI_STK_IMSICHG_NULL        = 0x00,
    SI_STK_IMSICHG_SELECT_ITEM = 0x01,
    SI_STK_IMSICHG_REFRESH     = 0x02,
    SI_STK_IMSICHG_STATE_BUTT
};
typedef VOS_UINT32 SI_STK_ImsichgStateUint32;

/*
 * 枚举说明: STK模块记录的DTMF SEND状态
 */
enum SI_STK_DtmfSend {
    SI_STK_DTMF_SEND_OK     = 0, /* 当前发送数据成功 */
    SI_STK_DTMF_SEND_ERR    = 1, /* 当前发送数据失败 */
    SI_STK_DTMF_SEND_FINISH = 2, /* 当前发送数据完成 */
    SI_STK_DTMF_SEND_REJECT = 3, /* 当前发送数据被拒 */
    SI_STK_DTMF_SEND_BUTT,
};
typedef VOS_UINT32 SI_STK_DtmfSendUint32;

/*
 * 枚举说明: STK模块记录的当前呼叫状态
 */
enum SI_STK_CallIdStatus {
    SI_STK_CALL_ID_STATUS_IDLE = 0x5A, /* 当前处于空闲状态 */
    SI_STK_CALL_ID_STATUS_BUSY = 0xA5, /* 当前处于呼叫状态(包括DTMF发送过程中) */
    SI_STK_CALL_ID_STATUS_BUTT,
};
typedef VOS_UINT8 SI_STK_CallIdStatusUint8;

/*
 * 枚举说明: STK模块记录的当前呼叫状态
 */
enum SI_STK_CallStatusType {
    SI_STK_CALL_STATUS_WAIT_IDLE        = 0, /* 当前处于空闲状态,当前无SETUP CALL主动命令，不处理NAS消息 */
    SI_STK_CALL_STATUS_WAIT_CONNECT     = 1, /* 当前处于等待CONNECT消息 */
    SI_STK_CALL_STATUS_WAIT_DISCONNECT  = 2, /* 当前处于等待DISCONNECT消息 */
    SI_STK_CALL_STATUS_WAIT_HOLD        = 3, /* 当前处于HOLD消息 */
    SI_STK_CALL_STATUS_SEND_DTMF        = 4, /* 当前处于发送DTMF过程中 */
    SI_STK_CALL_STATUS_WAIT_DURTIMEROUT = 5, /* 当前处于等待duration超时过程中 */
    SI_STK_CALL_STATUS_BUTT,
};
typedef VOS_UINT32 SI_STK_CallStatusUint32;

/*
 * 枚举说明: SETUP CALL主动命令的CQ值
 */
enum SI_STK_SetupCallCommandQualifier {
    SI_STK_SETUP_CALL_ONLY_NOT_BUSY_ON_ANOTHER_CALL         = 0,
    SI_STK_SETUP_CALL_ONLY_NOT_BUSY_ON_ANOTHER_CALL_REDIAL  = 1,
    SI_STK_SETUP_CALL_PUTTING_ALL_OTHER_CALL_ON_HOLD        = 2,
    SI_STK_SETUP_CALL_PUTTING_ALL_OTHER_CALL_ON_HOLD_REDIAL = 3,
    SI_STK_SETUP_CALL_DISCONNECT_ALL_OTHER_CALL             = 4,
    SI_STK_SETUP_CALL_DISCONNECT_ALL_OTHER_CALL_REDIAL      = 5,
    SI_STK_SETUP_CALL_BUTT,
};
typedef VOS_UINT8 SI_STK_SetupCallCommandQualifierUint8;


enum SI_STK_SearchMode {
    SI_STK_SEARCH_MODE_AUTO   = TAF_PH_PLMN_SEL_MODE_AUTO,
    SI_STK_SEARCH_MODE_MANUAL = TAF_PH_PLMN_SEL_MODE_MANUAL,
    SI_STK_SEARCH_MODE_BUTT
};


enum SI_STK_HookMsgName {
    STK_TAF_CS_SERVICE_STATUS = 0xFF00, /* CS服务状态勾包 */
    STK_TAF_CBS_DCS           = 0xFF01, /* USSD DCS编码钩包 */

};
typedef VOS_UINT32 SI_STK_HookMsgNameUint32;


enum SI_STK_TextStringDcs {
    SI_STK_TEXT_STRING_DCS_7BIT = 0x00,
    SI_STK_TEXT_STRING_DCS_8BIT = 0x04,
    SI_STK_TEXT_STRING_DCS_UCS  = 0x08,
    SI_STK_TEXT_STRING_DCS_BUTT
};
typedef VOS_UINT8 SI_STK_TextStringDcsUint8;

/* Envelop 重发事件对应枚举，需要重发场景才能扩展 */
enum SI_STK_EnvelopeResentFlag {
    SI_STK_ENVELOPE_SENT_ONETIME = 0,
    SI_STK_ENVELOPE_LOCI_RESEND  = 1, /* 本地事件需要重发 */
    SI_STK_ENVELOPE_ACC_RESEND   = 2, /* 接入技术改变需要重发 */
    SI_STK_ENVELOPE_PPDW_RESEND  = 3, /* SMS-PP Download遇到卡busy需要重发 */

    SI_STK_ENVELOPE_RESENT_BUTT
};
typedef VOS_UINT32 SI_STK_EnvelopeResentFlagUint32;

enum SI_STK_CallstateFlag {
    SI_STK_CALLSTATE_OFF = 0x0,
    SI_STK_CALLSTATE_ON  = 0xAA,
    SI_STK_CALLSTATE_BUTT
};
typedef VOS_UINT8 SI_STK_CallstateFlagUint8;

typedef struct {
    VOS_UINT32 len;
    VOS_UINT8 *value;
} SI_SAT_Lv;

typedef struct {
    VOS_UINT8  tag;
    VOS_UINT8  len;
    VOS_UINT8  rsv[2];
    VOS_UINT8 *value;
} SI_SAT_Tlv;

typedef struct {
    VOS_UINT32 opInfo;
    VOS_UINT8  decode[500];
} SI_SAT_Commdata;

typedef struct {
    VOS_UINT32                senderPID;
    SI_STK_ReqmsgUint32  stkEventType;
    SI_STK_LockUint32    stkLock;
    VOS_UINT16                clientID;
    VOS_UINT8                 opID;
    VOS_UINT8                 ucReserved;
} SI_STK_ReqUint;

typedef struct {
    VOS_UINT8               usatTag;
    VOS_UINT8               reserved[3];
    SI_STK_CommandDetails   cmdDetail;
    SI_STK_DeviceIdentities cmdDevice;
    SI_STK_SetUpMenu        setUpMenu;
} SI_SAT_SetUpMenuData;

typedef struct {
    VOS_UINT8  opId;
    VOS_UINT8  callId;
    VOS_UINT8  rsv[2];
    VOS_UINT32 dtmfLen;
    VOS_UINT32 curDtmfPtr;
    SI_CHAR    dtfm[512];
} SI_SAT_DtmfData;

typedef struct {
    VOS_UINT16 cmdType;
    VOS_UINT16 bitNum;
} SI_STK_CmdProfileCfg;

typedef struct {
    VOS_UINT8             dataTag;  /* 用于存放Tag的值 */
    SI_STK_TagTypeUint8 tagType;  /* 用于指示Tag的编解码类型 */
    VOS_UINT8             struLen;  /* 用于表明数据结构的大小 */
    VOS_UINT8             valueLen; /* 用于指明当前V型数据实际大小 */
} SI_STK_Datadecode;

typedef struct {
    VOS_UINT32 cmdType;  /* 存放STK命令的类型 */
    VOS_UINT32 tagLen;   /* 存放编解码的列表长度 */
    VOS_UINT8 *tagList; /* 存放编解码列表地址 */
} SI_STK_Taglist;

/*
 * 结构说明: STK模块呼叫状态表结构
 */
typedef struct {
    MN_CALL_ID_T                     callId;        /* 呼叫ID */
    SI_STK_CallIdStatusUint8  status;      /* 呼叫状态 */
    TAF_CALL_VoiceDomainUint8  voiceDomain; /* VOICE DOMAIN */
    VOS_UINT8                        rsv;
} SI_STK_CallStatus;

/*
 * 结构说明: STK模块SETUP CALL中DTMF控制结构
 */
typedef struct {
    VOS_UINT8 *dtmfCharacter;       /* DTMF字符串 */
    VOS_UINT32 dtmfCharacterRemain;  /* 待发送的DTMF字符串 */
    VOS_UINT32 dtmfCharacterCurrSnd; /* 当前发送DTMF字符 */
} SI_STK_SetupCallDtmfInfo;

/*
 * 结构说明: STK模块呼叫控制结构
 */
typedef struct {
    SI_STK_CallStatusUint32  callStatus;                            /* SETUP CALL主动命令处理状态 */
    SI_STK_CallStatus        stkCallStatusTable[STK_CALL_MAX_NUM]; /* STK模块呼叫状态表 */
    MN_CALL_ID_T                   setupCallId;                           /* SETUP CALL发起呼叫的ID，由NAS分配 */
    VOS_UINT8                      recallTime;                            /* 重新呼叫的次数 */
    VOS_UINT8                      rcv[2];
} SI_STK_SetupCallCtrl;

/*
 * 结构说明: L+C共SIM卡功能保存EVENT LIST
 */
typedef struct {
    VOS_UINT32 proactiveCmdLen;
    VOS_UINT8 *cmd;
} SI_STK_CbpSetupEventList;

typedef VOS_UINT32 (*pfSATCmdDecode)(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmd,
                                     VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType, SI_SAT_Commdata *result);

typedef VOS_UINT32 (*pfSATCmdPro)(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *event);

typedef struct {
    SI_STK_CmdTypeUint32 cmdType;   /* 处理消息类型 */
    pfSATCmdDecode  decodeFun; /* 对应解码函数 */
    pfSATCmdPro     procFun;   /* 对应处理函数 */
} SI_STK_CmdhandleList;

typedef struct {
    SI_STK_TimerstateUint32  timerState;
    HTIMER                        timer;
    VOS_UINT8                     hour;
    VOS_UINT8                     min;
    VOS_UINT8                     sec;
    VOS_UINT8                     reserve;
} SI_STK_Timer;

typedef struct {
    SI_STK_CommandDetails   cmdDetail;
    SI_STK_DeviceIdentities cmdDevice;
} SI_STK_ProactiveCmdInfo;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32      msgName;
    SI_ClientIdTypteUint16  clientId;
    VOS_UINT8       opId;
    VOS_UINT8       rsv;
    SI_STK_CmdTypeUint32 satType;
    VOS_UINT32      datalen;
    VOS_UINT8       data[8];
} SI_STK_Req;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgName;
    VOS_UINT8  pwd[4];
} SI_STK_SsPwdReq;

/* 沃达丰Dual IMSI 切换定制需求 */
typedef struct {
    SI_STK_ImsichgStateUint32        imsiChgState; /* 沃达丰IMSI切换定制需求状态记录 */
    VOS_UINT32                       curImsiSign;  /* 取值0,1，与两个IMSI相对应 */
    VOS_UINT8                        oldImsi[9];  /* 切换前的IMSI */
    VOS_UINT8                        oldItemId;    /* 上一次选择的ITEM ID */
    VOS_UINT8                        tryFlag;      /* 记录是否已尝试过一次切换 */
    VOS_UINT8                        cycleFlag;    /* 标记是否在状态机中轮转 */
} SI_STK_ImsichgCtrl;

typedef VOS_UINT32 (*STKIMSIPROC)(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

/* IMSI 切换定制需求状态机结构 */
typedef struct {
    SI_STK_ImsichgStateUint32  imsiChgState; /* 沃达丰IMSI切换定制需求状态记录 */
    STKIMSIPROC                      iMSIChgProc;   /* 沃达丰IMSI切换定制需求对应状态处理函数 */
} SI_STK_ImsichgProc;

/* 缓存下发的Envelope命令 */
typedef struct {
    VOS_UINT32 cmdLen;                        /* Envelope命令长度 */
    VOS_UINT8  cmdData[SI_STK_DATA_MAX_LEN]; /* Envelope命令内容 */
} SI_STK_EnvelopeCmd;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgName;
    VOS_UINT8  data[4];
} SI_STK_AtcnfMsg;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32 msgName;
    VOS_UINT16 clientId;
    VOS_UINT8  opId;
    VOS_UINT8  ucValue;
    VOS_UINT8  data[4];
} SI_STK_TafSendmsg;

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32            msgId;
    MN_MSG_EventUint32  eventType;
    VOS_UINT8             event[4];
} SI_STK_TafcnfMsg;

/* 内部事件记录结构声明 */
typedef struct {
    VOS_UINT32 opMTCall : 1;
    VOS_UINT32 opCallConnect : 1;
    VOS_UINT32 opCallDisconnet : 1;
    VOS_UINT32 opLociStatus : 1;
    VOS_UINT32 opAccChange : 1;
    VOS_UINT32 opNetSearchChg : 1;
    VOS_UINT32 opNetRej : 1;
    VOS_UINT32 opIMSRegistration : 1;
    VOS_UINT32 opReserved : 24;
} SI_STK_EventState;

typedef struct {
    MSG_Header msgHeader;
    VOS_UINT32      csServiceStatus;
} STK_CS_Service;


typedef struct {
    VOS_UINT8              inputDcs;
    VOS_UINT8              rsv;
    VOS_UINT16             stringLen;
    MN_MSG_CbdcsCode       cbDcs;
    VOS_UINT8              string[4];
} STK_CBS_Dcs;
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
typedef struct {
    NAS_STK_UpdateTypeUint8        origType; /* MM子层传递给STK的原始type */
    SI_STK_UpdateAttachTypeUint8  destType; /* 根据31111 8.92  Update/Attach Type，原始type所映射到的目的type */
    VOS_UINT16                           usRsv;
} STK_UPDATA_AttachTypeTransfer;
#endif

typedef struct {
    HTIMER     stkResendTimer;
    VOS_UINT32 stkResendCounter;
    VOS_UINT32 sendPara; /* 重试完成后，消息发送给对应PID */
    VOS_UINT32 dataLen;
    VOS_UINT8  data[STK_ENVELOPE_MAX_LEN];
} STK_EVENTDATA_Resend;

/*
 * 结构说明: STK模块保存USIM卡信息数据结构
 */
typedef struct {
    USIMM_PhyCardTypeUint32    phyType;
    USIMM_CardTypeUint32       cardType;
    USIMM_CardAppServicUint32  svcStatus;
    VOS_BOOL                   bIsSupprot5GCard;
} SI_STK_Cardinfo;

typedef struct {
    VOS_UINT32 reqFlag;
    VOS_UINT32 dataLen;
    VOS_UINT8  data[SI_STK_DATA_MAX_LEN];
} SI_STK_Flaglvdata;

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
typedef struct {
    VOS_UINT32                     isPressNeed;
    VOS_UINT32                     smsNum;
    VOS_UINT16                     userDataStartByte;
    VOS_UINT16                     userDataLen;
    TAF_XSMS_BdMsgUserData userData;
} SI_STK_CdmaSmsCtrlPara;
#endif
#endif

typedef struct {
    SI_STK_CallstateFlagUint8 csCallState;
    SI_STK_CallstateFlagUint8 psCallState;
    VOS_UINT16                  usRsv;
} SI_STK_CallState;

typedef struct {
    VOS_UINT32 mcc;
    VOS_UINT32 mnc;
} STK_PLMN_Id;

/*
 * 结构说明: STK模块维护的系统信息字段结构
 */
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
typedef struct {
    SI_STK_AccessTechUint8  curRat;    /* 接入技术 */
    SI_STK_SearchModeUint8  srchMode;  /* 搜网模式 */
    NAS_STK_UtranModeUint8  utranMode; /* TDD或FDD模式 */
    VOS_UINT8                     rsv;
    VOS_UINT32                    cellIdentityHighBit;
    VOS_UINT32                    cellIdentityLowBit;
    STK_PLMN_Id              curPlmnId; /* PLMN */
    VOS_UINT32                    lac;       /* 对应G/U模的LAC 以及L/NR模的TAC */
} SI_STK_SysInfo;
#endif
/*
 * 结构说明: STK模块维护的NAS信息字段结构
 */
#if (OSA_CPU_CCPU == VOS_OSA_CPU)

typedef struct {
    VOS_UINT32 eventMsgFlag;
    /* 使用STK自定义的服务状态，不使用NAS消息中的值，在更新时需要转换 */
    SI_STK_ServiceStatusUint32  serviceStatus;
    SI_STK_ServiceStatusUint32  csServiceStatus;
    SI_STK_SysInfo              sysInfo;
} SI_STK_NasInfo;

typedef struct {
    /* 使用STK自定义的服务状态，不使用NAS消息中的值，在更新时需要转换 */
    SI_STK_ServiceStatusUint32  serviceStatus;
    MMA_STK_1XSysInfo          sysInfo;
} SI_STK_CdmanasSysinfo;
#endif


typedef VOS_VOID (*PFSISTKPIDMSGPROC)(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);


typedef struct {
    VOS_UINT32        msgPid;
    PFSISTKPIDMSGPROC procFunc; /* 处理函数 */
} SI_STK_PIDMSGPROC_FUNC;


typedef VOS_VOID (*PFSISTKCSCALLBACKPROC)(SI_PIH_CardSlotUint32 slotId, MN_CALL_EventUint32 eventType,
                                          MN_CALL_Info *pstEvent);


typedef struct {
    MN_CALL_EventUint32  eventType;
    PFSISTKCSCALLBACKPROC  procFunc; /* 处理函数 */
} SI_STK_CSCALLBACKPROC_FUNC;


typedef VOS_UINT32 (*PFSISTKPROVIDELOCALPROC)(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *pCmdData);


typedef struct {
    VOS_UINT32              commandQua;
    PFSISTKPROVIDELOCALPROC procFunc; /* 处理函数 */
} SI_STK_PROVIDELOCALPROC_FUNC;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
/* STKComm.c */
extern VOS_UINT32 SI_STK_StrStr(VOS_UINT8 *str1, VOS_UINT8 *str2, VOS_UINT32 strlen1, VOS_UINT32 strlen2);

extern VOS_UINT32 SI_STKFindTag(VOS_UINT8 tag, VOS_UINT8 *data, VOS_UINT32 spareBuffSize, VOS_UINT32 dataLen,
                                VOS_UINT32 findNum);

extern VOS_VOID SI_STKDecodeTagList(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                    VOS_UINT32 spareBuffSize, VOS_UINT8 *tagList, VOS_UINT32 listLen,
                                    SI_SAT_Commdata *dec);

extern VOS_VOID SI_STK_InitEnvelope(VOS_UINT8 *cmdData, VOS_UINT32 cmdDataLen,
                                    SI_STK_DeviceIdentities *diInfo, VOS_UINT8 cmdType);

extern VOS_UINT32 SI_STKCommCodeData(SI_PIH_CardSlotUint32 slotId, USIMM_U8LvData *cmdData,
                                     SI_CodedataTypeUint32 dataType, VOS_UINT32 cmdType,
                                     SI_SAT_Commdata *src);

extern VOS_UINT32 SI_STKTagDataFree(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 tag, VOS_VOID *data);

extern VOS_UINT32 SI_STKCommDecodeData(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                       VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType, SI_SAT_Commdata *dec);

extern VOS_UINT32 SI_STK_SendTerminalResponseReqMsg(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 len,
                                                    VOS_UINT8 *rsp, USIMM_TrPara32  para);

extern VOS_UINT32 SI_STK_SendResetReqMsg(SI_PIH_CardSlotUint32    slotId,
                                         USIMM_StkCommandDetails *cMDDetail,
                                         USIMM_ResetInfo          *rstInfo);

extern VOS_VOID SI_STKSimpleResponseData(SI_PIH_CardSlotUint32 slotId, SI_STK_CommandDetails *cMDInfo,
                                         VOS_UINT32 dataLen, VOS_UINT8 *data);

extern VOS_VOID SI_STK_InitEventDown(VOS_UINT8 *cmdData, VOS_UINT32 cmdDataLen,
                                     SI_STK_DeviceIdentities *diInfo, VOS_UINT8 eventType);

extern SI_VOID SI_STK_EventDownload(SI_PIH_CardSlotUint32 slotId, const VOS_VOID *eventData,
                                    VOS_UINT32 eventLen, SI_STK_EventTypeUint32 eventDownTag, VOS_UINT32 sDId);

extern VOS_VOID SI_STKSimpleResponse(SI_PIH_CardSlotUint32 slotId, SI_STK_CommandDetails *cMDInfo,
                                     VOS_UINT8 result);

extern VOS_VOID SI_STKCommDataFree(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 cmdType,
                                   SI_SAT_Commdata *data);

extern VOS_UINT32 SI_STKDecodeTagData(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                      VOS_UINT32 cmdDataLen, VOS_UINT8 tag, VOS_VOID *dec);

extern VOS_UINT32 SI_STK_CheckProfileCfg(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 cmdType,
                                         SI_STK_CmdProfileCfg *cfgTbl, VOS_UINT32 tblSize);

extern VOS_VOID SI_STK_SaveEnvelopeCmd(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 cmdLen,
                                       VOS_UINT8 *cmdData);

extern VOS_UINT32 SI_STK_SendEnvelopeReqMsg(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 sendPara,
                                            USIMM_CardAppUint32  appType, VOS_UINT8 len,
                                            VOS_UINT8 *envelope);

extern VOS_VOID SI_STK_ClearEnvelopeCmd(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STKCheckCardState(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STKErrorProc(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID SI_STK_InitTResponse(VOS_UINT8 *tRdata, VOS_UINT32 dataLen, SI_STK_CommandDetails *cMDInfo,
                                     SI_STK_Result *result);

extern VOS_VOID SI_STKSendTRWithData(SI_PIH_CardSlotUint32 slotId, SI_STK_CommandDetails *cMDInfo,
                                     VOS_UINT32 resultLen, VOS_UINT8 *result, VOS_UINT32 dataLen,
                                     VOS_UINT8 *data);

extern VOS_VOID SI_STK_SsStr2ASCII(const VOS_UINT8 *src, VOS_UINT8 *dest, VOS_UINT32 length);

extern VOS_VOID SI_STK_Num2BCD(const VOS_UINT8 *src, VOS_UINT8 *dest, VOS_UINT32 length);

extern VOS_UINT32 SI_STKGetBitFromBuf(VOS_UINT8 *dataBuf, VOS_UINT32 bitNo, VOS_UINT32 bufLen);

extern VOS_UINT32 SI_STK_SendStatusReqMsg(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STK_SendNvRefreshReqMsg(SI_PIH_CardSlotUint32 slotId);

/* STKGobal.c */
extern VOS_VOID STK_UpdateCsSvcStatus(SI_PIH_CardSlotUint32      slotId,
                                      NAS_STK_ServiceStatusUint8  csServiceStatus);

extern VOS_VOID STK_GetSysInfo(SI_PIH_CardSlotUint32 slotId, SI_STK_SysInfo *sysInfo);

extern VOS_UINT32 STK_GetSvcStatus(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT8 STK_GetCurRat(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT8 STK_GetSearchMode(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID STK_UpdateSvcStatus(SI_PIH_CardSlotUint32      slotId,
                                    NAS_STK_ServiceStatusUint8  serviceStatus);

extern VOS_VOID STK_UpdateCurRat(SI_PIH_CardSlotUint32 slotId, TAF_MMA_RatTypeUint8 rat);

extern VOS_VOID STK_UpdateSysInfo(SI_PIH_CardSlotUint32 slotId, NAS_STK_SysInfo *sysInfo);

extern VOS_UINT32 STK_GetCsSvcStatus(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID STK_ResetGlobalValue(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID STK_ProfileInit(SI_PIH_CardSlotUint32 slotId, USIMM_StkCfg *sTKProfileContent,
                                USIMM_PhyCardTypeUint32  phyType);

extern VOS_VOID STK_GetProfileInfo(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 **ppucProfile,
                                   VOS_UINT8 *profileLen);

extern VOS_VOID STK_SetCallCleanGobal(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID STK_UpdateSearchMode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 searchMode);

extern VOS_VOID SI_STK_InitNVConfig(SI_PIH_CardSlotUint32 slotId);

extern VOS_BOOL SI_STK_CheckDualSimCtrlNvDataValid(SI_PIH_CardSlotUint32 slotId,
                                                   SI_STK_ImsichgMatch   *iMSIMatch);

extern VOS_VOID STK_InitGobal(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID STK_ClearPauseCharGobal(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID STK_CallStateMachineSet(SI_PIH_CardSlotUint32   slotId,
                                        SI_STK_CallStatusUint32  callStatus);

/* STKPro.c */
extern VOS_UINT32 SI_STK_RefreshProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STKNoNeedDecode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                     VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType, SI_SAT_Commdata *result);

extern VOS_UINT32 SI_STK_MoreTimeProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_PollIntervalProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_PollingOFFProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SetUpEeventListSpecialProc(SI_PIH_CardSlotUint32 slotId,
                                                    SI_STK_DataInfo       *cmdData);

extern VOS_UINT32 SI_STKSetUpCall_Decode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                         VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType,
                                         SI_SAT_Commdata *result);

extern VOS_UINT32 SI_STK_SetCallSpecialProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendSSProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendUSSDProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendSMSProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendDTMFProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_LaunchBrowser_Decode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                              VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType,
                                              SI_SAT_Commdata *result);

extern VOS_UINT32 SI_STKDataIndCallback(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_DisplayTextProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SelectItem_Decode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                           VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType,
                                           SI_SAT_Commdata *result);

extern VOS_BOOL SI_STK_CheckSupportAP(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STK_SetUpMenu_Decode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                          VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType,
                                          SI_SAT_Commdata *result);

extern VOS_UINT32 SI_STK_SetUpMenuProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_ProvideLocIProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_TimerManageProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SetUpIdleTextProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STKSetFrame_Decode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                        VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType,
                                        SI_SAT_Commdata *result);

extern VOS_UINT32 SI_STKCommDecode(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *cmdData,
                                   VOS_UINT32 spareBuffSize, VOS_UINT32 cmdType, SI_SAT_Commdata *result);

extern VOS_BOOL SI_STK_CheckCardStatus(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID SI_STK_TRCnfMsgProc(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

extern VOS_VOID SI_STK_RcvSmsEnvelopeCnfDispatchHandle(SI_PIH_CardSlotUint32 slotId,
                                                       USIMM_STKEnvelopeCnf *usimmMsg);

extern VOS_UINT32 SI_STK_SetUpEeventListProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

/* STKSpecialProc.c */
extern VOS_VOID SI_STK_LocationStatusEventDownload(SI_PIH_CardSlotUint32 slotId);

extern VOS_BOOL STK_IsCallInService(SI_PIH_CardSlotUint32 slotId);

extern VOS_BOOL SI_STK_ProvideLocalInfo_NAA(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *trData,
                                            VOS_UINT32 trDataBuffSize, VOS_UINT32 *trDataLen,
                                            SI_STK_NasInfo *mmaInfo);

extern VOS_VOID SI_STK_TransferUpdateAttachType(VOS_UINT8 *type, NAS_STK_UpdateTypeUint8 nasType);

extern VOS_UINT32 MN_MSG_DecodeCbsDcs(VOS_UINT8 dcs, VOS_UINT8 *content, VOS_UINT32 contentLength,
                                      MN_MSG_CbdcsCode *dcsInfo);

extern VOS_VOID SI_STK_SendChangePollTimerLenMsg(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 timerLen);

extern VOS_UINT32 SI_STK_SendSSSpecialProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendDTMFSpecialProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendSMSSpecialProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_UINT32 SI_STK_SendUSSDSpecialProc(SI_PIH_CardSlotUint32 slotId, SI_STK_DataInfo *cmdData);

extern VOS_VOID SI_STK_EventResendClean(SI_PIH_CardSlotUint32       slotId,
                                        SI_STK_EnvelopeResentFlagUint32 dataType);

extern VOS_UINT32 SI_STK_CheckMsgPID(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg, VOS_UINT32 pid);

VOS_VOID SI_STK_EventResendSave(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 sendPara,
                                SI_STK_EnvelopeResentFlagUint32 dataType, VOS_UINT32 eventLen,
                                VOS_UINT8 *data);

extern VOS_UINT32 SI_STK_DisconnectStkCall(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STK_SendStartDtmfMsg(SI_PIH_CardSlotUint32 slotId, VOS_CHAR dtmf, VOS_UINT8 opId,
                                          VOS_UINT8 callId);

extern VOS_VOID SI_STK_ResendTimerMsgHandle(SI_PIH_CardSlotUint32 slotId, REL_TimerMsgBlock *msg);

extern VOS_VOID SI_STK_IMSIChgBegin(SI_PIH_CardSlotUint32 slotId, SI_STK_Req *sTKReqMsg);

extern VOS_VOID SI_STK_SetUpCallAllow(SI_PIH_CardSlotUint32        slotId,
                                      SI_STK_SetupCallConfirmUint32  action);

extern VOS_VOID SI_STK_NetworkRejectionEventDownload(SI_PIH_CardSlotUint32          slotId,
                                                     NAS_STK_NetworkRejectionEvent *msg);

extern VOS_UINT32 SI_STK_SVLTECheckCsDomainAndMsgPID(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg,
                                                     VOS_UINT32 expectPid);
extern VOS_VOID   SI_STK_UpdateLocationInfo(SI_PIH_CardSlotUint32        slotId,
                                            NAS_STK_LocStatusEventInfo *locStatusEvent);

extern VOS_VOID SI_STK_LociInfoIndMsgProc(SI_PIH_CardSlotUint32    slotId,
                                          NAS_STK_LocationInfoInd *lociInfo);

extern VOS_VOID SI_STK_NetworkSearchModeChangeEventDownload(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID SI_STK_TAFMsgProc(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

extern VOS_VOID SI_STK_IMSIChgProc(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

extern VOS_VOID SI_STK_UssdDcsHook(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 inputDcs, VOS_UINT8 *ussdString,
                                   VOS_UINT16 ringLen, MN_MSG_CbdcsCode *cbDcs);

extern VOS_UINT32 TAF_STD_UnPack7Bit(const VOS_UINT8 *orgChar, VOS_UINT32 len, VOS_UINT8 fillBit,
                                     VOS_UINT8 *unPackedChar);

extern VOS_UINT32 TAF_STD_Pack7Bit(const VOS_UINT8 *orgChar, VOS_UINT32 len, VOS_UINT8 fillBit,
                                   VOS_UINT8 *packedChar, VOS_UINT32 *len00000);

extern VOS_VOID SI_STK_SetUpCallStopDurationTimer(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 flg);

extern SI_STK_DtmfSendUint32 SI_STK_SendStopDtmfMsg(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 ucopID);

extern SI_STK_DtmfSendUint32 SI_STK_SendDtmfToTAF(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 opId,
                                                         VOS_UINT8 callId);

extern VOS_VOID SI_STK_SetupCallStatusTable(SI_PIH_CardSlotUint32 slotId, MN_CALL_EventUint32 eventType,
                                            MN_CALL_Info *event);

extern VOS_VOID SI_STK_SuspendCnfMsgProc(SI_PIH_CardSlotUint32 slotId, MN_CALL_Info *event);

extern VOS_UINT32 SI_STK_SetUpCallOnlyNotBusyOnCall(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID SI_STK_BcdStrToAscii(VOS_UINT8 bcdNumLen, VOS_UINT8 *bcdNum, VOS_UINT8 *asciiNum,
                                     VOS_UINT32 *len, SI_STK_StrChangemodeUint32  changeMode);

extern VOS_UINT32 SI_STK_EncodeUssdString(SI_PIH_CardSlotUint32 slotId,
                                          SI_STK_UssdString     *srcUssdString,
                                          TAF_SS_UssdString     *destUssdString);

extern VOS_VOID SI_STK_SetUpCallStartDurationTimer(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 flg,
                                                   SI_STK_Duration *duration);

extern VOS_UINT32 SI_STK_SetUpCallFunc(SI_PIH_CardSlotUint32 slotId, VOS_BOOL bNeedDurTimer);

/* STKAtPrintf.c */
extern VOS_VOID At_STKCallBackFunc(SI_PIH_CardSlotUint32 slotId, SI_STK_EventInfo *event);

extern VOS_VOID STK_InitSndCodeAddr(SI_PIH_CardSlotUint32 slotId);

extern SI_STK_DataInfo *SI_STK_GetDataInfoAddr(SI_PIH_CardSlotUint32 slotId);

extern SI_STK_ReqUint *SI_STK_GetSTKCtrlAddr(SI_PIH_CardSlotUint32 slotId);

extern SI_SAT_SetUpMenuData *SI_STK_GetSetUpMenuDataAddr(SI_PIH_CardSlotUint32 slotId);

extern SI_STK_ImsichgCtrl *SI_STK_GetIMSIChgCtrlAddr(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STK_IMSCodeData(VOS_UINT8 *cmdData, VOS_UINT32 bufOffs, VOS_UINT32 bufLen,
                                     IMSA_STK_ImsRegistrationEventDownload *msg);

extern VOS_UINT32 SI_STK_GetIMSRegistrationDataLen(const VOS_UINT8 *dataBuf);

#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
extern VOS_VOID SI_STK_CdmaLocStatusEventDownload(SI_PIH_CardSlotUint32 slotId);

extern VOS_UINT32 SI_STKGetCdmaEcc(SI_PIH_CardSlotUint32 slotId, SI_STK_Address *addr);

extern VOS_VOID SI_STK_RcvXsmsEnvelopeHandle(SI_PIH_CardSlotUint32 slotId, USIMM_STKEnvelopeCnf *msg);

extern VOS_VOID SI_STK_XsmsRcvSendSmsCnf(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

extern VOS_VOID SI_STK_XsmsRcvSendSmsRsltInd(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

extern VOS_VOID SI_STK_XsmsRcvSmsPPDownLoadReq(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);

extern VOS_VOID SI_STK_InitXsmsGlobal(SI_PIH_CardSlotUint32 slotId);

extern VOS_VOID SI_STK_SendXsmsReq(SI_PIH_CardSlotUint32 slotId, TAF_XSMS_Message *message,
                                   VOS_UINT16 isUserAckMsg);

extern VOS_VOID SI_STK_SendXsmsPPDownLoadCnf(SI_PIH_CardSlotUint32 slotId, VOS_UINT32 envelopeRslt,
                                             VOS_UINT32 rspCode, VOS_UINT32 dataLen, VOS_UINT8 *data);

extern VOS_VOID SI_STK_ServiceInfoInd(SI_PIH_CardSlotUint32 slotId, MMA_STK_ServiceInfoInd *msg);

extern VOS_BOOL SI_STK_CheckCdmaCallID(SI_PIH_CardSlotUint32 slotId);

extern SI_STK_DtmfSendUint32 SI_STK_SendXDtmfToTAF(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 opID,
                                                          VOS_UINT8 callID);

extern VOS_VOID SI_STK_SendBurstDTMFCnfProc(SI_PIH_CardSlotUint32           slotId,
                                            TAF_CALL_EvtSendBurstDtmfCnf *event);

extern VOS_VOID SI_STK_SendBurstDTMFResultProc(SI_PIH_CardSlotUint32            slotId,
                                               TAF_CALL_EvtSendBurstDtmfRslt *event);

extern VOS_UINT32 SI_STK_SendCdmaSMSHandle(SI_PIH_CardSlotUint32    slotId,
                                           SI_STK_SendShortMessage *sendSMS,
                                           SI_STK_CommandDetails    *cmdDetail);

extern VOS_UINT32 SI_STK_CdmaProvideLocalInfo(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *trData,
                                              VOS_UINT32 trDataLen);

extern VOS_UINT32 SI_STK_ProvideESN(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *trData, VOS_UINT32 trDataLen);

extern VOS_UINT32 SI_STK_ProvideMEID(SI_PIH_CardSlotUint32 slotId, VOS_UINT8 *trData, VOS_UINT32 trDataLen);

extern VOS_UINT32 SI_STK_CheckCdmaSmsPara(TAF_XSMS_TransportMessage *tLMsg,
                                          SI_STK_CommandDetails     *cmdDetail,
                                          SI_STK_CdmaSmsCtrlPara  *cdmaSmsPara);
#endif
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
