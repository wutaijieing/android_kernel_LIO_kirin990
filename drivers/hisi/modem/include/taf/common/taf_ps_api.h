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
#ifndef _TAF_PS_API_H_
#define _TAF_PS_API_H_

#include "vos.h"
#include "mdrv.h"
#include "ps_tag.h"
#include "taf_type_def.h"
#include "PsTypeDef.h"
#include "taf_api.h"
#include "taf_ps_type_def.h"
#include "nas_comm_packet_ser.h"
#include "imsa_wifi_interface.h"
#include "ps_iface_global_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef WIN32
#pragma warning(disable : 4200) /* zero-sized array in struct/union */
#endif

#pragma pack(4)

#define TAF_MAX_ACCESS_NUM_LEN (32)
#define TAF_MAX_GW_AUTH_USERNAME_LEN (255)
#define TAF_MAX_GW_AUTH_PASSWORD_LEN (255)

/* 所支持最大的PDP参数表项目数 */

#if (FEATURE_ON == FEATURE_LTE)
#define TAF_MAX_CID (31)
#else
#define TAF_MAX_CID (11)
#endif
#define TAF_MAX_CID_NV (11)

#define TAF_INVALID_CID (0xFF)

/* 4 x 3 char dec nums + 3 x '.' + '\0' */
#define TAF_MAX_IPV4_ADDR_STR_LEN (16)

/* 8 x 4 字符(HEX) + 7 x ':' + '\0' */
#define TAF_MAX_IPV6_ADDR_COLON_STR_LEN (40)

/* 32 x 3 字符(DEC) + 31 x '.' + '\0' */
#define TAF_MAX_IPV6_ADDR_DOT_STR_LEN (128)

/* IPV6字符串格式使用的分隔符标记最大个数 */
#define TAF_IPV6_STR_MAX_TOKENS (16)

/* RFC2373规定的IPV6字符串格式使用的分隔符标记最大个数 */
#define TAF_IPV6_STR_RFC2373_TOKENS (8)

/* RFC协议使用的IPV4文本表达方式使用的分隔符 */
#define TAF_IPV4_STR_DELIMITER '.'
/* RFC2373使用的IPV6文本表达方式使用的分隔符 */
#define TAF_IPV6_STR_DELIMITER ':'

#define TAF_IPV6_PREFIX_LEN (8) /* IPv6地址前缀长度 */
#define TAF_IPV6_IID_LEN (8)    /* IPv6地址接口ID长度 */

/* password+auth */
#define TAF_PPP_PAP_REQ_MAX_LEN (100 + 100)

/* code(1B)+id(1B)+length(2B)+challenge_size(1B)+challenge+name */
#define TAF_PPP_CHAP_CHALLENGE_MAX_LEN (1 + 1 + 2 + 1 + 48 + 100)

/* code(1B)+id(1B)+length(2B)+response_size(1B)+response+name */
#define TAF_PPP_CHAP_RESPONSE_MAX_LEN (1 + 1 + 2 + 1 + 100 + 100)

#define TAF_PPP_AUTH_FRAME_BUF_MAX_LEN (256)

#define TAF_PPP_IPCP_FRAME_BUF_MAX_LEN (256)

#define TAF_DEFAULT_DSFLOW_NV_WR_INTERVAL (2) /* DSFLOW流量统计NV写间隔, 单位(min) */

#define TAF_PS_CALL_TYPE_DOWN_NORMAL (0)
#define TAF_PS_CALL_TYPE_UP (1)
#define TAF_PS_CALL_TYPE_DOWN_FORCE (2)

#define TAF_PS_IS_SM_CAUSE_VALID(sm_cause)                                                                  \
    ((((sm_cause) + TAF_PS_CAUSE_SM_NW_SECTION_BEGIN) >= TAF_PS_CAUSE_SM_NW_OPERATOR_DETERMINED_BARRING) && \
     (((sm_cause) + TAF_PS_CAUSE_SM_NW_SECTION_BEGIN) <= TAF_PS_CAUSE_SM_NW_APN_RESTRICTION_INCOMPATIBLE))

#define TAF_PS_CONVERT_SM_CAUSE_TO_PS_CAUSE(sm_cause) ((sm_cause) + TAF_PS_CAUSE_SM_NW_SECTION_BEGIN)

#define TAF_PS_GET_MSG_CONTENT(pstMsg) ((VOS_VOID *)(((TAF_PS_Msg *)(pstMsg))->content))

#define TAF_PS_BUILD_EXCLIENTID(usModemId, usClientId) ((VOS_UINT16)(((usModemId)&0x000F) << 12) | (usClientId))

#define TAF_PS_GET_MODEMID_FROM_EXCLIENTID(usExClientId) ((VOS_UINT16)(((usExClientId)&0xF000) >> 12))

#define TAF_PS_GET_CLIENTID_FROM_EXCLIENTID(usExClientId) ((VOS_UINT16)((usExClientId)&0x0FFF))

/*
 * 下边这些签署值都是以AT命令为标准的取值，也是APP认为的签署取值，不是3GPP
 * 核心网认为的签署值
 */
#define TAF_PS_APPQOS_TRAFFCLASS_SUBSCRIB 4
#define TAF_PS_APPQOS_DELIVERORDER_SUBSCRIB 2
#define TAF_PS_APPQOS_DELIVEReRRSDU_SUBSCRIB 3
#define TAF_PS_APPQOS_MAXSDUSIZE_SUBSCRIB 0
#define TAF_PS_APPQOS_MAXBITUL_SUBSCRIB 0
#define TAF_PS_APPQOS_MAXBITDL_SUBSCRIB 0
#define TAF_PS_APPQOS_RESIDUALBER_SUBSCRIB 0
#define TAF_PS_APPQOS_SDUERRRATIO_SUBSCRIB 0
#define TAF_PS_APPQOS_TRANSDELAY_SUBSCRIB 0
#define TAF_PS_APPQOS_TRAFFHANDPRIOR_SUBSCRIB 0
#define TAF_PS_APPQOS_GUARANTBITUL_SUBSCRIB 0
#define TAF_PS_APPQOS_GUARANTBITDL_SUBSCRIB 0
#define TAF_PS_APPQOS_SRCSTATISTICSDES_SUBSCRIB 0
#define TAF_PS_APPQOS_SIGNALINDICATION_SUBSCRIB 0

#define TAF_PS_PDP_RESELECT_CID (12)
#define TAF_PS_PDP_MAX_CID (20)

#define TAF_PS_PDP_TYPE_CHG_MAX_NUM (7)

#define TAF_PS_APN_DATA_SYS_INFO_MAX_NUM (6)
#define TAF_PS_APN_DATA_SYS_SUBSID_MAX_NUM (2)

#define TAF_PS_APN_DATA_SYS_PROTECT_IN_CELLULAR_MIN_TIME (10)
#define TAF_PS_APN_DATA_SYS_PROTECT_IN_WLAN_MIN_TIME (2)

#define TAF_PS_MSG_CONTENT_NUM (4)

#define TAF_PS_UE_POLICY_MAX_SECTION_LEN (1000)

#define TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN 4

#define TAF_PS_UE_OS_ID_INFO_MAX_LEN 240


enum TAF_PS_MsgId {
    /*
     * 标准命令DSM_CONN模块处理[0x0000, 0x004F]
     */
    /* +CGACT */
    /* _H2ASN_MsgChoice TAF_PS_SetPdpStateReq */
    ID_MSG_TAF_PS_SET_PDP_CONTEXT_STATE_REQ = TAF_PS_MSG_ID_BASE + 0x0001,

    /* +CGCMOD */
    /* _H2ASN_MsgChoice TAF_PS_CallModifyReq */
    ID_MSG_TAF_PS_CALL_MODIFY_REQ = TAF_PS_MSG_ID_BASE + 0x0002,

    /* +CGANS */
    /* _H2ASN_MsgChoice TAF_PS_CallAnswerReq */
    ID_MSG_TAF_PS_CALL_ANSWER_REQ = TAF_PS_MSG_ID_BASE + 0x0003,
    /* _H2ASN_MsgChoice TAF_PS_CallHangupReq */
    ID_MSG_TAF_PS_CALL_HANGUP_REQ = TAF_PS_MSG_ID_BASE + 0x0004,

    /* 以下是老的消息ID不再使用，暂时保留，用于老架构编译 */
    /* +CGQREQ */
    ID_MSG_TAF_PS_SET_GPRS_QOS_INFO_REQ = TAF_PS_MSG_ID_BASE + 0x0007, /* _H2ASN_MsgChoice */
    ID_MSG_TAF_PS_GET_GPRS_QOS_INFO_REQ = TAF_PS_MSG_ID_BASE + 0x0008, /* _H2ASN_MsgChoice */

    /* +CGQMIN */
    ID_MSG_TAF_PS_SET_GPRS_QOS_MIN_INFO_REQ = TAF_PS_MSG_ID_BASE + 0x0009, /* _H2ASN_MsgChoice */
    ID_MSG_TAF_PS_GET_GPRS_QOS_MIN_INFO_REQ = TAF_PS_MSG_ID_BASE + 0x000A, /* _H2ASN_MsgChoice */

    /*
     * 私有命令DSM_CONN处理[0x0050, 0x00CF]
     */
    /* ^NDISCONN/^NDISDUP */
    /* _H2ASN_MsgChoice TAF_PS_CallOrigReq */
    ID_MSG_TAF_PS_CALL_ORIG_REQ = TAF_PS_MSG_ID_BASE + 0x0051,
    /* _H2ASN_MsgChoice TAF_PS_CallEndReq */
    ID_MSG_TAF_PS_CALL_END_REQ = TAF_PS_MSG_ID_BASE + 0x0052,

    /* PPP */
    /* _H2ASN_MsgChoice TAF_PS_PppDialOrigReq */
    ID_MSG_TAF_PS_PPP_DIAL_ORIG_REQ = TAF_PS_MSG_ID_BASE + 0x0053,

    /* 异步接口获取SDF信息 */
    /* _H2ASN_MsgChoice TAF_SDF_ParaQueryInfo */
    ID_MSG_TAF_PS_GET_CID_SDF_REQ = TAF_PS_MSG_ID_BASE + 0x0054,

    /* 异步接口获取CID */
    ID_MSG_TAF_PS_GET_UNUSED_CID_REQ = TAF_PS_MSG_ID_BASE + 0x0055, /* _H2ASN_MsgChoice */

    /* ^IMSCTRLMSG */
    /* _H2ASN_MsgChoice TAF_PS_EpdgCtrlNtf */
    ID_MSG_TAF_PS_EPDG_CTRL_NTF = TAF_PS_MSG_ID_BASE + 0x0056,

    /*
     * 私有命令APS处理[0x00D0, 0x00FF]
     */
    /* ^DSFLOWQRY */
    /* _H2ASN_MsgChoice TAF_PS_GetDsflowInfoReq */
    ID_MSG_TAF_PS_GET_DSFLOW_INFO_REQ = TAF_PS_MSG_ID_BASE + 0x00D1,

    /* ^DSFLOWCLR */
    /* _H2ASN_MsgChoice TAF_PS_ClearDsflowReq */
    ID_MSG_TAF_PS_CLEAR_DSFLOW_REQ = TAF_PS_MSG_ID_BASE + 0x00D2,

    /* ^DSFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_ConfigDsflowRptReq */
    ID_MSG_TAF_PS_CONFIG_DSFLOW_RPT_REQ = TAF_PS_MSG_ID_BASE + 0x00D3,

    /* ^TRIG */
    /* _H2ASN_MsgChoice TAF_PS_TrigGprsDataReq */
    ID_MSG_TAF_PS_TRIG_GPRS_DATA_REQ = TAF_PS_MSG_ID_BASE + 0x00D4,

    /* ^LTECS */
    /* _H2ASN_MsgChoice TAF_PS_LtecsReq */
    ID_MSG_TAF_PS_GET_LTE_CS_REQ = TAF_PS_MSG_ID_BASE + 0x00D5,

    /* ^CEMODE */
    /* _H2ASN_MsgChoice TAF_PS_CemodeReq */
    ID_MSG_TAF_PS_GET_CEMODE_REQ = TAF_PS_MSG_ID_BASE + 0x00D6,

    /* CDMA下，进入DORMANT状态指示 */
    /* _H2ASN_MsgChoice TAF_PS_CallEnterDormantInd */
    ID_MSG_TAF_PS_CALL_ENTER_DORMANT_IND = TAF_PS_MSG_ID_BASE + 0x00D7,

    /* _H2ASN_MsgChoice TAF_PS_HaiChangeInd */
    ID_MSG_TAF_PS_HAI_CHANGE_IND = TAF_PS_MSG_ID_BASE + 0x00D8,

    /* ^APDSFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_SetApdsflowRptCfgReq */
    ID_MSG_TAF_PS_SET_APDSFLOW_RPT_CFG_REQ = TAF_PS_MSG_ID_BASE + 0x00D9,
    /* _H2ASN_MsgChoice TAF_PS_GetApdsflowRptCfgReq */
    ID_MSG_TAF_PS_GET_APDSFLOW_RPT_CFG_REQ = TAF_PS_MSG_ID_BASE + 0x00DA,

    /* _H2ASN_MsgChoice TAF_PS_SetDsflowNvWriteCfgReq */
    ID_MSG_TAF_PS_SET_DSFLOW_NV_WRITE_CFG_REQ = TAF_PS_MSG_ID_BASE + 0x00DB,
    /* _H2ASN_MsgChoice TAF_PS_GetDsflowNvWriteCfgReq */
    ID_MSG_TAF_PS_GET_DSFLOW_NV_WRITE_CFG_REQ = TAF_PS_MSG_ID_BASE + 0x00DC,

    /* _H2ASN_MsgChoice TAF_PS_CdataDialModeReq */
    ID_MSG_TAF_PS_SET_CDMA_DIAL_MODE_REQ = TAF_PS_MSG_ID_BASE + 0x00DD,

    /* ^VTFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_ConfigVtflowRptReq */
    ID_MSG_TAF_PS_CONFIG_VTFLOW_RPT_REQ = TAF_PS_MSG_ID_BASE + 0x00DE,

    /* ^PDPROFMOD */
    /* _H2ASN_MsgChoice TAF_PS_SetProfileInfoReq */
    ID_MSG_TAF_PS_SET_PDPROFMOD_INFO_REQ = TAF_PS_MSG_ID_BASE + 0x00DF,

    /* 以下是老的消息ID不再使用，暂时保留，用于老架构编译 */
    /* _H2ASN_MsgChoice TAF_PS_Set1XCqosPriReq */
    ID_MSG_TAF_PS_SET_CQOS_PRI_REQ = TAF_PS_MSG_ID_BASE + 0x00E0,

    /* ^DATASWITCH */
    /* _H2ASN_MsgChoice TAF_PS_SetDataSwitchReq */
    ID_MSG_TAF_PS_SET_DATA_SWITCH_REQ = TAF_PS_MSG_ID_BASE + 0x00E1,
    /* _H2ASN_MsgChoice TAF_PS_GET_DATA_SWITCH_REQ_STRU */
    ID_MSG_TAF_PS_GET_DATA_SWITCH_REQ = TAF_PS_MSG_ID_BASE + 0x00E2,

    /* ^DATAROAMSWITCH */
    /* _H2ASN_MsgChoice TAF_PS_SetDataRoamSwitchReq */
    ID_MSG_TAF_PS_SET_DATA_ROAM_SWITCH_REQ = TAF_PS_MSG_ID_BASE + 0x00E3,
    /* _H2ASN_MsgChoice TAF_PS_GET_DATA_ROAM_SWITCH_REQ_STRU */
    ID_MSG_TAF_PS_GET_DATA_ROAM_SWITCH_REQ = TAF_PS_MSG_ID_BASE + 0x00E4,

    /* 参数设置相关的消息，取值区间从TAF_PS_APP_PROFILE_ID_BASE开始，由DSM_PROFILE模块处理 */
    /*
     * 标准命令
     */
    /* +CGDCONT */
    /* _H2ASN_MsgChoice TAF_PS_SetPrimPdpContextInfoReq */
    ID_MSG_TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0001,
    /* _H2ASN_MsgChoice TAF_PS_GetPrimPdpContextInfoReq */
    ID_MSG_TAF_PS_GET_PRIM_PDP_CONTEXT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0002,

    /* +CGDSCONT */
    /* _H2ASN_MsgChoice TAF_PS_SetSecPdpContextInfoReq */
    ID_MSG_TAF_PS_SET_SEC_PDP_CONTEXT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0003,
    /* _H2ASN_MsgChoice TAF_PS_GetSecPdpContextInfoReq */
    ID_MSG_TAF_PS_GET_SEC_PDP_CONTEXT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0004,

    /* +CGTFT */
    /* _H2ASN_MsgChoice TAF_PS_SetTftInfoReq */
    ID_MSG_TAF_PS_SET_TFT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0005,
    /* _H2ASN_MsgChoice TAF_PS_GetTftInfoReq */
    ID_MSG_TAF_PS_GET_TFT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0006,

    /* +CGEQREQ */
    /* _H2ASN_MsgChoice TAF_PS_SetUmtsQosInfoReq */
    ID_MSG_TAF_PS_SET_UMTS_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0007,
    /* _H2ASN_MsgChoice TAF_PS_GetUmtsQosInfoReq */
    ID_MSG_TAF_PS_GET_UMTS_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0008,

    /* +CGEQMIN */
    /* _H2ASN_MsgChoice TAF_PS_SetUmtsQosMinInfoReq */
    ID_MSG_TAF_PS_SET_UMTS_QOS_MIN_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0009,
    /* _H2ASN_MsgChoice TAF_PS_GetUmtsQosMinInfoReq */
    ID_MSG_TAF_PS_GET_UMTS_QOS_MIN_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x000A,

    /* +CGEQNEG */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicUmtsQosInfoReq */
    ID_MSG_TAF_PS_GET_DYNAMIC_UMTS_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x000B,

    /* +CGACT */
    /* _H2ASN_MsgChoice TAF_PS_GetPdpStateReq */
    ID_MSG_TAF_PS_GET_PDP_CONTEXT_STATE_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x000C,

    /* +CGPADDR */
    /* _H2ASN_MsgChoice TAF_PS_GetPdpIpAddrInfoReq */
    ID_MSG_TAF_PS_GET_PDP_IP_ADDR_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x000D,

    /* _H2ASN_MsgChoice TAF_PS_GetPdpContextInfoReq */
    ID_MSG_TAF_PS_GET_PDPCONT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x000E,

    /* +CGAUTO */
    /* _H2ASN_MsgChoice TAF_PS_SetAnswerModeInfoReq */
    ID_MSG_TAF_PS_SET_ANSWER_MODE_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x000F,
    /* _H2ASN_MsgChoice TAF_PS_GetAnswerModeInfoReq */
    ID_MSG_TAF_PS_GET_ANSWER_MODE_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0010,

    /* +CGCONTRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicPrimPdpContextInfoReq */
    ID_MSG_TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0011,

    /* +CGSCONTRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicSecPdpContextInfoReq */
    ID_MSG_TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0012,

    /* +CGTFTRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicTftInfoReq */
    ID_MSG_TAF_PS_GET_DYNAMIC_TFT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0013,

    /* +CGEQOS */
    /* _H2ASN_MsgChoice TAF_PS_SetEpsQosInfoReq */
    ID_MSG_TAF_PS_SET_EPS_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0014,
    /* _H2ASN_MsgChoice TAF_PS_GetEpsQosInfoReq */
    ID_MSG_TAF_PS_GET_EPS_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0015,

    /* +CGEQOSRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicEpsQosInfoReq */
    ID_MSG_TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0016,

    /* D */
    /* _H2ASN_MsgChoice TAF_PS_GetDGprsActiveTypeReq */
    ID_MSG_TAF_PS_GET_D_GPRS_ACTIVE_TYPE_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0017,

    /* ^DWINS */
    /* _H2ASN_MsgChoice TAF_PS_ConfigNbnsFunctionReq */
    ID_MSG_TAF_PS_CONFIG_NBNS_FUNCTION_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0018,

    /* ^AUTHDATA */
    /* _H2ASN_MsgChoice TAF_PS_SetAuthdataInfoReq */
    ID_MSG_TAF_PS_SET_AUTHDATA_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0019,
    /* _H2ASN_MsgChoice TAF_PS_GetAuthdataInfoReq */
    ID_MSG_TAF_PS_GET_AUTHDATA_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x001A,

    /* ^DNSQUERY */
    /* _H2ASN_MsgChoice TAF_PS_GetNegotiationDnsReq */
    ID_MSG_TAF_PS_GET_NEGOTIATION_DNS_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x001B,

    /* ^CGDNS */
    /* _H2ASN_MsgChoice TAF_PS_SetPdpDnsInfoReq */
    ID_MSG_TAF_PS_SET_PDP_DNS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x001C,
    /* _H2ASN_MsgChoice TAF_PS_GetPdpDnsInfoReq */
    ID_MSG_TAF_PS_GET_PDP_DNS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x001D,

    /* +CTA */
    /* _H2ASN_MsgChoice TAF_PS_SetCtaInfoReq */
    ID_MSG_TAF_PS_SET_CTA_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x001E,
    /* _H2ASN_MsgChoice TAF_PS_GET_1X_CTA_INFO_REQ_STRU */
    ID_MSG_TAF_PS_GET_CTA_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x001F,

    /* _H2ASN_MsgChoice TAF_PS_GetCgmtuValueReq */
    ID_MSG_TAF_PS_GET_CGMTU_VALUE_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0020,

    /* ^IMSPDPCFG */
    /* _H2ASN_MsgChoice TAF_PS_SetImsPdpCfgReq */
    ID_MSG_TAF_PS_SET_IMS_PDP_CFG_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0021,

    /* ^CDORMTIMER */
    /* _H2ASN_MsgChoice TAF_PS_SET_1X_DORMANT_TIMER_REQ_STRU */
    ID_MSG_TAF_PS_SET_1X_DORM_TIMER_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0022,
    /* _H2ASN_MsgChoice TAF_PS_GET_1X_DORMANT_TIMER_REQ_STRU */
    ID_MSG_TAF_PS_GET_1X_DORM_TIEMR_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0X0023,

    /* _H2ASN_MsgChoice TAF_PS_SetMipModeReq */
    ID_MSG_TAF_PS_SET_MIP_MODE_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x0024,
    /* _H2ASN_MsgChoice TAF_PS_GetMipModeReq */
    ID_MSG_TAF_PS_GET_MIP_MODE_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0X0025,

    /* _H2ASN_MsgChoice TAF_PS_SetApnThrotInfoReq */
    ID_MSG_TAF_PS_SET_APN_THROT_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0X0028,

    /* _H2ASN_MsgChoice TAF_PS_GetLteAttachInfoReq */
    ID_MSG_TAF_PS_GET_LTE_ATTACH_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0X0029,

    /* _H2ASN_MsgChoice TAF_PS_Set5GQosInfoReq */
    ID_MSG_TAF_PS_SET_5G_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x002A,
    /* _H2ASN_MsgChoice TAF_PS_Get5gQosInfoReq */
    ID_MSG_TAF_PS_GET_5G_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x002B,

    /* +C5GQOSRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamic5GQosInfoReq */
    ID_MSG_TAF_PS_GET_DYNAMIC_5G_QOS_INFO_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0x002C,

    ID_MSG_TAF_PS_SET_ROAMING_PDP_TYPE_REQ = TAF_PS_APP_PROFILE_ID_BASE + 0X002D,

    /* 以下是AT与DSM-PCF之间的消息，消息ID范围TAF_PS_APP_PCF_ID_BASE + 0xFF,由DSM_PCF模块处理 */

    /* ^CPOLICYRPT */
    /* _H2ASN_MsgChoice TAF_PS_SetUePolicyRptReq */
    ID_MSG_TAF_PS_SET_UE_POLICY_RPT_REQ = TAF_PS_APP_PCF_ID_BASE + 0x0001,

    /* ^CPOLICYCODE */
    /* _H2ASN_MsgChoice TAF_PS_GetUePolicyReq */
    ID_MSG_TAF_PS_GET_UE_POLICY_REQ = TAF_PS_APP_PCF_ID_BASE + 0x0002,

    /* +CSUEPOLICY */
    /* _H2ASN_MsgChoice TAF_PS_5GUePolicyRsp */
    ID_MSG_TAF_PS_5G_UE_POLICY_INFO_RSP = TAF_PS_APP_PCF_ID_BASE + 0x0003,

    ID_MSG_TAF_PS_BUTT

};
typedef VOS_UINT32 TAF_PS_MsgIdUint32;


enum TAF_PS_EvtId {
    /* PS CALL */
    /* _H2ASN_MsgChoice TAF_PS_CallPdpActivateCnf           */
    ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_CNF = TAF_PS_EVT_ID_BASE + 0x0001,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpActivateRej           */
    ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_REJ = TAF_PS_EVT_ID_BASE + 0x0002,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpActivateInd           */
    ID_EVT_TAF_PS_CALL_PDP_ACTIVATE_IND = TAF_PS_EVT_ID_BASE + 0x0003,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpManageInd             */
    ID_EVT_TAF_PS_CALL_PDP_MANAGE_IND = TAF_PS_EVT_ID_BASE + 0x0004,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpModifyCnf             */
    ID_EVT_TAF_PS_CALL_PDP_MODIFY_CNF = TAF_PS_EVT_ID_BASE + 0x0005,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpModifyRej             */
    ID_EVT_TAF_PS_CALL_PDP_MODIFY_REJ = TAF_PS_EVT_ID_BASE + 0x0006,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpModifyInd             */
    ID_EVT_TAF_PS_CALL_PDP_MODIFY_IND = TAF_PS_EVT_ID_BASE + 0x0007,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpDeactivateCnf         */
    ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_CNF = TAF_PS_EVT_ID_BASE + 0x0008,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpDeactivateInd         */
    ID_EVT_TAF_PS_CALL_PDP_DEACTIVATE_IND = TAF_PS_EVT_ID_BASE + 0x0009,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpPdpDisconnectInd     */
    ID_EVT_TAF_PS_CALL_PDP_DISCONNECT_IND = TAF_PS_EVT_ID_BASE + 0x000A,
    /* _H2ASN_MsgChoice TAF_PS_Ipv6InfoInd                   */
    ID_EVT_TAF_PS_CALL_PDP_IPV6_INFO_IND = TAF_PS_EVT_ID_BASE + 0x000B,
    /* _H2ASN_MsgChoice TAF_PS_CallPdpRabidChangeInd       */
    ID_EVT_TAF_PS_CALL_PDP_RABID_CHANGE_IND = TAF_PS_EVT_ID_BASE + 0x000C,
    /* _H2ASN_MsgChoice TAF_PS_CallHandoverResultNtf */
    ID_EVT_TAF_PS_CALL_HANDOVER_RESULT_NTF = TAF_PS_EVT_ID_BASE + 0x000D,
    /* _H2ASN_MsgChoice TAF_PS_EpdgCtrluNtf */
    ID_EVT_TAF_PS_EPDG_CTRLU_NTF = TAF_PS_EVT_ID_BASE + 0x000E,
    /* _H2ASN_MsgChoice TAF_PS_CnModeChangeInd */
    ID_EVT_TAF_PS_CN_MODE_CHANGE_IND = TAF_PS_EVT_ID_BASE + 0x000F,
    /* _H2ASN_MsgChoice TAF_PS_IpChangeInd */
    ID_EVT_TAF_PS_IP_CHANGE_IND = TAF_PS_EVT_ID_BASE + 0x0010,

    /* _H2ASN_MsgChoice TAF_PS_CallOrigCnf                   */
    ID_EVT_TAF_PS_CALL_ORIG_CNF = TAF_PS_EVT_ID_BASE + 0x0031,
    /* _H2ASN_MsgChoice TAF_PS_CallEndCnf                    */
    ID_EVT_TAF_PS_CALL_END_CNF = TAF_PS_EVT_ID_BASE + 0x0032,
    /* _H2ASN_MsgChoice TAF_PS_CallModifyCnf                 */
    ID_EVT_TAF_PS_CALL_MODIFY_CNF = TAF_PS_EVT_ID_BASE + 0x0033,
    /* _H2ASN_MsgChoice TAF_PS_CallAnswerCnf                 */
    ID_EVT_TAF_PS_CALL_ANSWER_CNF = TAF_PS_EVT_ID_BASE + 0x0034,
    /* _H2ASN_MsgChoice TAF_PS_CallHangupCnf                 */
    ID_EVT_TAF_PS_CALL_HANGUP_CNF = TAF_PS_EVT_ID_BASE + 0x0035,

    /* D */
    /* _H2ASN_MsgChoice TAF_PS_GetDGprsActiveTypeCnf      */
    ID_EVT_TAF_PS_GET_D_GPRS_ACTIVE_TYPE_CNF = TAF_PS_EVT_ID_BASE + 0x0100,

    /* PPP */
    /* _H2ASN_MsgChoice TAF_PS_PppDialOrigCnf               */
    ID_EVT_TAF_PS_PPP_DIAL_ORIG_CNF = TAF_PS_EVT_ID_BASE + 0x0101,

    /* +CGDCONT */
    /* _H2ASN_MsgChoice TAF_PS_SetPrimPdpContextInfoCnf   */
    ID_EVT_TAF_PS_SET_PRIM_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0102,
    /* _H2ASN_MsgChoice TAF_PS_GetPrimPdpContextInfoCnf   */
    ID_EVT_TAF_PS_GET_PRIM_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0103,

    /* +CGDSCONT */
    /* _H2ASN_MsgChoice TAF_PS_SetSecPdpContextInfoCnf    */
    ID_EVT_TAF_PS_SET_SEC_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0104,
    /* _H2ASN_MsgChoice TAF_PS_GetSecPdpContextInfoCnf    */
    ID_EVT_TAF_PS_GET_SEC_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0105,

    /* +CGTFT */
    /* _H2ASN_MsgChoice TAF_PS_SetTftInfoCnf                */
    ID_EVT_TAF_PS_SET_TFT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0106,
    /* _H2ASN_MsgChoice TAF_PS_GetTftInfoCnf                */
    ID_EVT_TAF_PS_GET_TFT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0107,

    /* +CGEQREQ */
    /* _H2ASN_MsgChoice TAF_PS_SetUmtsQosInfoCnf           */
    ID_EVT_TAF_PS_SET_UMTS_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x010C,
    /* _H2ASN_MsgChoice TAF_PS_GetUmtsQosInfoCnf           */
    ID_EVT_TAF_PS_GET_UMTS_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x010D,

    /* +CGEQMIN */
    /* _H2ASN_MsgChoice TAF_PS_SetUmtsQosMinInfoCnf       */
    ID_EVT_TAF_PS_SET_UMTS_QOS_MIN_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x010E,
    /* _H2ASN_MsgChoice TAF_PS_GetUmtsQosMinInfoCnf       */
    ID_EVT_TAF_PS_GET_UMTS_QOS_MIN_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x010F,

    /* +CGEQNEG */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicUmtsQosInfoCnf   */
    ID_EVT_TAF_PS_GET_DYNAMIC_UMTS_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0110,

    /* +CGACT */
    /* _H2ASN_MsgChoice TAF_PS_SetPdpStateCnf       */
    ID_EVT_TAF_PS_SET_PDP_CONTEXT_STATE_CNF = TAF_PS_EVT_ID_BASE + 0x0111,
    /* _H2ASN_MsgChoice TAF_PS_GetPdpStateCnf       */
    ID_EVT_TAF_PS_GET_PDP_CONTEXT_STATE_CNF = TAF_PS_EVT_ID_BASE + 0x0112,

    /* +CGPADDR */
    /* _H2ASN_MsgChoice TAF_PS_GetPdpIpAddrInfoCnf        */
    ID_EVT_TAF_PS_GET_PDP_IP_ADDR_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0113,
    /* _H2ASN_MsgChoice TAF_PS_GetPdpContextInfoCnf        */
    ID_EVT_TAF_PS_GET_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0114,

    /* +CGAUTO */
    /* _H2ASN_MsgChoice TAF_PS_SetAnswerModeInfoCnf        */
    ID_EVT_TAF_PS_SET_ANSWER_MODE_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0115,
    /* _H2ASN_MsgChoice TAF_PS_GetAnswerModeInfoCnf        */
    ID_EVT_TAF_PS_GET_ANSWER_MODE_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0116,

    /* +CGCONTRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicPrimPdpContextInfoCnf */
    ID_EVT_TAF_PS_GET_DYNAMIC_PRIM_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0117,

    /* +CGSCONTRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicSecPdpContextInfoCnf */
    ID_EVT_TAF_PS_GET_DYNAMIC_SEC_PDP_CONTEXT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0118,

    /* +CGTFTRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicTftInfoCnf        */
    ID_EVT_TAF_PS_GET_DYNAMIC_TFT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0119,

    /* +CGEQOS */
    /* _H2ASN_MsgChoice TAF_PS_SetEpsQosInfoCnf            */
    ID_EVT_TAF_PS_SET_EPS_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x011A,
    /* _H2ASN_MsgChoice TAF_PS_GetEpsQosInfoCnf            */
    ID_EVT_TAF_PS_GET_EPS_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x011B,

    /* +CGEQOSRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamicEpsQosInfoCnf    */
    ID_EVT_TAF_PS_GET_DYNAMIC_EPS_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x011C,

    /* ^DSFLOWQRY */
    /* _H2ASN_MsgChoice TAF_PS_GetDsflowInfoCnf             */
    ID_EVT_TAF_PS_GET_DSFLOW_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x011D,

    /* ^DSFLOWCLR */
    /* _H2ASN_MsgChoice TAF_PS_ClearDsflowCnf                */
    ID_EVT_TAF_PS_CLEAR_DSFLOW_CNF = TAF_PS_EVT_ID_BASE + 0x011E,

    /* ^DSFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_ConfigDsflowRptCnf           */
    ID_EVT_TAF_PS_CONFIG_DSFLOW_RPT_CNF = TAF_PS_EVT_ID_BASE + 0x011F,

    /* ^DSFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_ReportDsflowInd               */
    ID_EVT_TAF_PS_REPORT_DSFLOW_IND = TAF_PS_EVT_ID_BASE + 0x0120,

    /* ^CGDNS */
    /* _H2ASN_MsgChoice TAF_PS_SetPdpDnsInfoCnf            */
    ID_EVT_TAF_PS_SET_PDP_DNS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0123,
    /* _H2ASN_MsgChoice TAF_PS_GetPdpDnsInfoCnf            */
    ID_EVT_TAF_PS_GET_PDP_DNS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0124,

    /* ^AUTHDATA */
    /* _H2ASN_MsgChoice TAF_PS_SetAuthDataInfoCnf           */
    ID_EVT_TAF_PS_SET_AUTHDATA_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0127,
    /* _H2ASN_MsgChoice TAF_PS_GetAuthdataInfoCnf           */
    ID_EVT_TAF_PS_GET_AUTHDATA_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0128,

    /* ^DNSQUERY */
    /* _H2ASN_MsgChoice TAF_PS_GetNegotiationDnsCnf         */
    ID_EVT_TAF_PS_GET_NEGOTIATION_DNS_CNF = TAF_PS_EVT_ID_BASE + 0x0129,

    /* ^LTECS */
    /* _H2ASN_MsgChoice TAF_PS_LtecsCnf                       */
    ID_EVT_TAF_PS_LTECS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x012A,

    /* ^PDPROFMOD */
    /* _H2ASN_MsgChoice TAF_PS_SetPdpProfInfoCnf           */
    ID_EVT_TAF_PS_SET_PDP_PROF_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x012B,

    /* +CEMODE */
    /* _H2ASN_MsgChoice TAF_PS_CemodeCnf                      */
    ID_EVT_TAF_PS_CEMODE_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x012C,

    /* 异步接口获取SDF信息 */
    /* _H2ASN_MsgChoice TAF_PS_SdfInfoCnf                    */
    ID_EVT_TAF_PS_GET_CID_SDF_CNF = TAF_PS_EVT_ID_BASE + 0x012D,

    /* 此消息不再使用，暂时保留，为兼容老架构编译 */
    /* ^CQOSPRI */
    /* _H2ASN_MsgChoice TAF_PS_SetCqosPriCnf                */
    ID_EVT_TAF_PS_SET_CQOS_PRI_CNF = TAF_PS_EVT_ID_BASE + 0x012E,

    /* ^APDSFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_SetApdsflowRptCfgCnf */
    ID_EVT_TAF_PS_SET_APDSFLOW_RPT_CFG_CNF = TAF_PS_EVT_ID_BASE + 0x012F,
    /* _H2ASN_MsgChoice TAF_PS_GetApdsflowRptCfgCnf */
    ID_EVT_TAF_PS_GET_APDSFLOW_RPT_CFG_CNF = TAF_PS_EVT_ID_BASE + 0x0130,
    /* _H2ASN_MsgChoice TAF_PS_ApdsflowReportInd */
    ID_EVT_TAF_PS_APDSFLOW_REPORT_IND = TAF_PS_EVT_ID_BASE + 0x0131,

    /* _H2ASN_MsgChoice TAF_PS_SetDsflowNvWriteCfgCnf */
    ID_EVT_TAF_PS_SET_DSFLOW_NV_WRITE_CFG_CNF = TAF_PS_EVT_ID_BASE + 0x0132,
    /* _H2ASN_MsgChoice TAF_PS_GetDsflowNvWriteCfgCnf */
    ID_EVT_TAF_PS_GET_DSFLOW_NV_WRITE_CFG_CNF = TAF_PS_EVT_ID_BASE + 0x0133,

    /* +CTA */
    /* _H2ASN_MsgChoice TAF_PS_SetCtaInfoCnf */
    ID_EVT_TAF_PS_SET_CTA_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0134,
    /* _H2ASN_MsgChoice TAF_PS_GetCtaInfoCnf */
    ID_EVT_TAF_PS_GET_CTA_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0135,

    /* +CRM */
    /* _H2ASN_MsgChoice TAF_PS_CdataDialModeCnf                */
    ID_EVT_TAF_PS_SET_CDMA_DIAL_MODE_CNF = TAF_PS_EVT_ID_BASE + 0x0136,

    /* _H2ASN_MsgChoice TAF_PS_GetCgmtuValueCnf                */
    ID_EVT_TAF_PS_GET_CGMTU_VALUE_CNF = TAF_PS_EVT_ID_BASE + 0x0137,
    /* _H2ASN_MsgChoice TAF_PS_CgmtuValueChgInd                */
    ID_EVT_TAF_PS_CGMTU_VALUE_CHG_IND = TAF_PS_EVT_ID_BASE + 0x0138,

    /* ^LIMITPDPACT */
    /* _H2ASN_MsgChoice TAF_PS_CallLimitPdpActInd */
    ID_EVT_TAF_PS_CALL_LIMIT_PDP_ACT_IND = TAF_PS_EVT_ID_BASE + 0x0139,

    /* _H2ASN_MsgChoice TAF_PS_SetImsPdpCfgCnf */
    ID_EVT_TAF_PS_SET_IMS_PDP_CFG_CNF = TAF_PS_EVT_ID_BASE + 0x013A,

    /* ^VTFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_ReportVtflowInd               */
    ID_EVT_TAF_PS_REPORT_VTFLOW_IND = TAF_PS_EVT_ID_BASE + 0x013B,

    /* ^VTFLOWRPT */
    /* _H2ASN_MsgChoice TAF_PS_ConfigVtflowRptCnf           */
    ID_EVT_TAF_PS_CONFIG_VTFLOW_RPT_CNF = TAF_PS_EVT_ID_BASE + 0x013C,

    /* _H2ASN_MsgChoice TAF_PS_Set1xDormTimerCnf */
    ID_EVT_TAF_PS_SET_1X_DORM_TIMER_CNF = TAF_PS_EVT_ID_BASE + 0x013D,
    /* _H2ASN_MsgChoice TAF_PS_Get1XDormTimerCnf */
    ID_EVT_TAF_PS_GET_1X_DORM_TIMER_CNF = TAF_PS_EVT_ID_BASE + 0x013E,

    /* _H2ASN_MsgChoice TAF_PS_SetMipModeCnf */
    ID_EVT_TAF_PS_SET_MIP_MODE_CNF = TAF_PS_EVT_ID_BASE + 0x013F,
    /* _H2ASN_MsgChoice TAF_PS_GetMipModeCnf */
    ID_EVT_TAF_PS_GET_MIP_MODE_CNF = TAF_PS_EVT_ID_BASE + 0x0140,

    /* _H2ASN_MsgChoice TAF_PS_ReportPcoInfoInd */
    ID_EVT_TAF_PS_REPORT_PCO_INFO_IND = TAF_PS_EVT_ID_BASE + 0x0145,

    /* ^DATASWITCH */
    /* _H2ASN_MsgChoice TAF_PS_SET_DATA_SWITCH_CNF_STRU */
    ID_EVT_TAF_PS_SET_DATA_SWITCH_CNF = TAF_PS_EVT_ID_BASE + 0x0146,
    /* _H2ASN_MsgChoice TAF_PS_GetDataSwitchCnf */
    ID_EVT_TAF_PS_GET_DATA_SWITCH_CNF = TAF_PS_EVT_ID_BASE + 0x0147,

    /* ^DATAROAMSWITCH */
    /* _H2ASN_MsgChoice TAF_PS_SET_DATA_ROAM_SWITCH_CNF_STRU */
    ID_EVT_TAF_PS_SET_DATA_ROAM_SWITCH_CNF = TAF_PS_EVT_ID_BASE + 0x0148,
    /* _H2ASN_MsgChoice TAF_PS_GET_DATA_ROAM_SWITCH_REQ_STRU */
    ID_EVT_TAF_PS_GET_DATA_ROAM_SWITCH_CNF = TAF_PS_EVT_ID_BASE + 0x0149,

    /* _H2ASN_MsgChoice TAF_PS_SetApnThrotInfoCnf */
    ID_EVT_TAF_PS_SET_APN_THROT_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x014A,

    ID_MSG_TAF_PS_GET_LTE_ATTACH_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x014D,

    /* _H2ASN_MsgChoice TAF_PS_Set5gQosInfoCnf            */
    ID_EVT_TAF_PS_SET_5G_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x014E,
    /* _H2ASN_MsgChoice TAF_PS_Get5GQosInfoCnf            */
    ID_EVT_TAF_PS_GET_5G_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x014F,

    /* +C5GQOSRDP */
    /* _H2ASN_MsgChoice TAF_PS_GetDynamic5GQosInfoCnf */
    ID_EVT_TAF_PS_GET_DYNAMIC_5G_QOS_INFO_CNF = TAF_PS_EVT_ID_BASE + 0x0150,

    /* _H2ASN_MsgChoice TAF_PS_SetRoamingPdpTypeCnf */
    ID_EVT_TAF_PS_SET_ROAMING_PDP_TYPE_CNF = TAF_PS_EVT_ID_BASE + 0x0151,

    /* ^CPOLICYRPT */
    /* _H2ASN_MsgChoice TAF_PS_UePolicyRptInd */
    ID_EVT_TAF_PS_UE_POLICY_RPT_IND = TAF_PS_EVT_ID_BASE + 0x0152,
    /* _H2ASN_MsgChoice TAF_PS_SetUePolicyRptCnf */
    ID_EVT_TAF_PS_SET_UE_POLICY_RPT_CNF = TAF_PS_EVT_ID_BASE + 0x0153,

    /* ^CPOLICYCODE */
    /* _H2ASN_MsgChoice TAF_PS_GetUePolicyCnf */
    ID_EVT_TAF_PS_GET_UE_POLICY_CNF = TAF_PS_EVT_ID_BASE + 0x0154,

    /* TAF_PS_UePolicyRspCheckRsltInd */
    ID_EVT_TAF_PS_UE_POLICY_RSP_CHECK_RSLT_IND = TAF_PS_EVT_ID_BASE + 0x0155,

    /* 其它事件 */
    /* DSM->IMSA通知SRVCC CANCEL */
    /* _H2ASN_MsgChoice TAF_SRVCC_CancelNotifyInd */
    ID_EVT_TAF_PS_SRVCC_CANCEL_NOTIFY_IND = TAF_PS_EVT_ID_BASE + 0x0200,

    ID_EVT_TAF_PS_BUTT
};
typedef VOS_UINT32 TAF_PS_EvtIdUint32;

/*
 * Description:
 */
enum TAF_PS_CdataBearStatus {
    TAF_PS_CDATA_BEAR_STATUS_INACTIVE = 0x00,
    TAF_PS_CDATA_BEAR_STATUS_IDLE     = 0x01,
    TAF_PS_CDATA_BEAR_STATUS_ACTIVE   = 0x02,
    TAF_PS_CDATA_BEAR_STATUS_SUSPEND  = 0x03,
    TAF_PS_CDATA_BEAR_STATUS_BUTT     = 0x04
};
typedef VOS_UINT8 TAF_PS_CdataBearStatusUint8;

/* 删除TAF_AUTH_TYPE_ENUM_UINT8 */


enum TAF_PDP_ActiveStatus {
    TAF_PDP_INACTIVE = 0x00, /* PDP未激活 */
    TAF_PDP_ACTIVE   = 0x01, /* PDP已激活 */

    TAF_PDP_ACTIVE_BUTT = 0xFF
};
typedef VOS_UINT8 TAF_PDP_ActiveStatusUint8;


enum TAF_GPRS_ActiveType {
    TAF_IP_ACTIVE_TE_PPP_MT_PPP_TYPE         = 0x00,
    TAF_PPP_ACTIVE_TE_PPP_MT_NOT_PPP_TYPE    = 0x01,
    TAF_IP_ACTIVE_TE_NOT_PPP_MT_NOT_PPP_TYPE = 0x02,

    TAF_IP_ACTIVE_TE_BUTT
};
typedef VOS_UINT8 TAF_GPRS_ActiveTypeUint8;


enum TAF_L2P_Type {
    TAF_L2P_PPP  = 0x00,
    TAF_L2P_NULL = 0x01,

    TAF_L2P_BUTT
};
typedef VOS_UINT8 TAF_L2P_TypeUint8;

/* 删除TAF_UMTS_QOS_TRAFFIC_CLASS_ENUM_UINT8 */


enum TAF_DSFLOW_ClearMode {
    TAF_DSFLOW_CLEAR_TOTAL = 1,  /* 清零所有RABID的流量 */
    TAF_DSFLOW_CLEAR_WITH_RABID, /* 清零指定RABID的流量 */

    TAF_DSFLOW_CLEAR_BUTT
};
typedef VOS_UINT8 TAF_DSFLOW_ClearModeUint8;

#if (FEATURE_ON == FEATURE_IMS)
/*
 * 结构说明: DSM->IMSA通知SRVCC CANECL
 */
enum TAF_SRVCC_CancelNotify {
    TAF_SRVCC_CANCEL_NOTIFY_HO_CANCELLED = 0,

    TAF_SRVCC_CANCEL_NOTIFY_IND_BUTT
};
typedef VOS_UINT32 TAF_SRVCC_CancelNotifyUint32;

#endif

/* 删除TAF_PS_CDATA_1X_QOS_NON_ASSURED_PRI_ENUM_UINT8 */


enum TAF_PS_MipMode {
    TAF_PS_MIP_MODE_SIP_ONLY = 0x00, /* Simple IP ONLY模式 */
    /* Mobile IP Prefered with SIP fallback 模式 */
    TAF_PS_MIP_MODE_MIP_PREFERED = 0x01,
    TAF_PS_MIP_MODE_MIP_ONLY     = 0x02, /* Mobile IP ONLY模式  */
    TAF_PS_MIP_MODE_BUTT
};
typedef VOS_UINT8 TAF_PS_MipModeUint8;

/* 以下枚举定义是为了保证老架构能编译通过 */
/*
 *  Description: User Priority Adjustment Values
 * Refer to  C.S0017-012-A_v2.0_060522 2.2.8 Table 15
 */
enum TAF_PS_Cdata1XQosNonAssuredPri {
    TAF_PS_CDATA_1X_QOS_DESIRED_NON_ASSURED_PRI_MIN     = 0x00, /* *< min value */
    TAF_PS_CDATA_1X_QOS_DESIRED_NON_ASSURED_PRI_DEFAULT = 0x0D, /* *< default value */
    TAF_PS_CDATA_1X_QOS_DESIRED_NON_ASSURED_PRI_MAX     = 0x0F, /* *< max value */
    TAF_PS_CDATA_1X_QOS_NON_ASSURED_PRI_BUTT            = 0x10
};
typedef VOS_UINT8 TAF_PS_Cdata1XQosNonAssuredPriUint8;


enum TAF_PS_Ipv6InfoResult {
    TAF_PS_IPV6_INFO_RESULT_SUCCESS      = 0x00, /* IPV6地址分配/刷新成功 */
    TAF_PS_IPV6_INFO_RESULT_ALLOC_FAIL   = 0x01, /* IPV6地址分配失败 */
    TAF_PS_IPV6_INFO_RESULT_REFRESH_FAIL = 0x02, /* IPV6地址刷新失败 */

    TAF_PS_IPV6_INFO_RESULT_BUTT
};
typedef VOS_UINT8 TAF_PS_Ipv6InfoResultUint8;


enum TAF_PS_RoamingLteChgIpType {
    TAF_PS_ROAMING_LTE_CHG_IP_TYPE_NO_CHANGE      = 0x00,
    TAF_PS_ROAMING_LTE_CHG_IP_TYPE_IPV4V6_TO_IPV4 = 0x01,
    TAF_PS_ROAMING_LTE_CHG_IP_TYPE_IPV4V6_TO_IPV6 = 0x02,

    TAF_PS_ROAMING_LTE_CHG_IP_TYPE_BUTT
};

typedef VOS_UINT8 TAF_PS_RoamingLteChgIpTypeUint8;


enum TAF_PS_RegResul {
    TAF_PS_REG_RESULT_SUCCESS = 0, /* PS域注册成功 */
    TAF_PS_REG_RESULT_FAILURE = 1, /* PS域注册失败 */
    TAF_PS_REG_RESULT_BUTT
};
typedef VOS_UINT32 TAF_PS_RegResulUint32;


enum TAF_PS_RegType {
    TAF_PS_REG_TYPE_ATTACH = 0, /* PS域注册类型是ATTACH */
    TAF_PS_REG_TYPE_RAU    = 1, /* PS域注册类型是RAU */
    TAF_PS_REG_TYPE_TAU    = 2, /* PS域注册类型是TAU */
    TAF_PS_REG_TYPE_BUTT
};
typedef VOS_UINT32 TAF_PS_RegTypeUint32;


enum TAF_PS_PdnOperateType {
    TAF_PS_PDN_OPERATE_TYPE_DEACTIVE = 0,
    TAF_PS_PDN_OPERATE_TYPE_ACTIVE   = 1,
    TAF_PS_PDN_OPERATE_TYPE_MODIFY   = 2,
    TAF_PS_PDN_OPERATE_TYPE_BUTT
};
typedef VOS_UINT8 TAF_PS_PdnOperateTypeUint8;


enum TAF_PS_PolicyMsgType {
    TAF_PS_UE_POLICY_TYPE_COMPLETE       = 0,
    TAF_PS_UE_POLICY_TYPE_COMMAND_REJECT = 1,
    TAF_PS_UE_POLICY_TYPE_STATE_IND      = 2,
    TAF_PS_UE_POLICY_TYPE_BUTT
};
typedef VOS_UINT8 TAF_PS_PolicyMsgTypeUint8;


enum TAF_PS_IpChangeOperType {
    TAF_PS_IP_CHANGE_OPER_ADD    = 0,
    TAF_PS_IP_CHANGE_OPER_DELETE = 1,
    TAF_PS_IP_CHANGE_OPER_CHANGE = 2,
    TAF_PS_IP_CHANGE_OPER_BUTT
};
typedef VOS_UINT8 TAF_PS_IpChangeOperTypeUint8;


enum TAF_PS_UePolicyRspCheckRslt {
    TAF_PS_UE_POLICY_RSP_CHECK_RSLT_SUCC = 0,
    TAF_PS_UE_POLICY_RSP_CHECK_RSLT_FAIL = 1,

    TAF_PS_UE_POLICY_RSP_CHECK_RSLT_BUTT
};
typedef VOS_UINT8 TAF_PS_UePolicyRspCheckRsltUint8;


typedef struct {
    MSG_Header header;
    VOS_UINT32 evtId;
    VOS_UINT8  content[4];

} TAF_PS_Evt;


typedef struct {
    VOS_UINT32 reserved;
    VOS_UINT32 linkTime; /* DS连接时间 */

    union {
        VOS_UINT32 sendFluxArray[2]; /* DS发送流量 */
        VOS_UINT64 sendFlux;         /* DS发送流量 */
    };
    union {
        VOS_UINT32 recvFluxArray[2]; /* DS接收流量 */
        VOS_UINT64 recvFlux;         /* DS接收流量 */
    };

} TAF_DSFLOW_Info;


typedef struct {
    VOS_UINT32 sendRate;    /* 当前发送速率 */
    VOS_UINT32 recvRate;    /* 当前接收速率 */
    VOS_UINT32 qosSendRate; /* QOS协商发送速率 */
    VOS_UINT32 qosRecvRate; /* QOS协商接收速率 */

    TAF_DSFLOW_Info flowInfo; /* 当前连接流量信息 */

} TAF_DSFLOW_Report;


typedef TAF_DSFLOW_Info TAF_VTFLOW_Report;


typedef struct {
    TAF_DSFLOW_Info currentFlowInfo; /* 当前连接流量信息 */
    TAF_DSFLOW_Info totalFlowInfo;   /* 累计流量信息 */

} TAF_DSFLOW_QueryInfo;


typedef struct {
    /* 目前仅支持是否鉴权，以后扩充为支持哪种鉴权算法 */
    TAF_PDP_AuthTypeUint8 auth;

    VOS_UINT8 reserved[3];

    /* aucUserName[0]放置用户名长度 */
    VOS_UINT8 userName[TAF_MAX_AUTHDATA_USERNAME_LEN + 1];

    /* aucPassword[0]放置密码长度 */
    VOS_UINT8 password[TAF_MAX_AUTHDATA_PASSWORD_LEN + 1];

} TAF_PDP_Auth;


typedef struct {
    VOS_UINT8           cid;
    VOS_UINT8           reserved[3];
    TAF_PDP_PrimContext priPdpInfo;

} TAF_PRI_PdpQueryInfo;


typedef struct {
    VOS_UINT8  cid;
    VOS_UINT8  pfNum; /* pf个数 */
    VOS_UINT8  reserved[2];
    TAF_PDP_Pf pfInfo[TAF_MAX_SDF_PF_NUM]; /* pf表 */

} TAF_TFT_QureyInfo;


typedef struct {
    VOS_UINT8    cid;
    VOS_UINT8    reserved[3];
    TAF_UMTS_Qos qosInfo;

} TAF_UMTS_QosQueryInfo;


typedef struct {
    VOS_UINT8   cid;
    VOS_UINT8   reserved[3];
    TAF_PDP_Dns dnsInfo;

} TAF_DNS_QueryInfo;


typedef struct {
    VOS_UINT8    cid;
    VOS_UINT8    reserved[3];
    TAF_PDP_Auth authInfo;

} TAF_AUTH_QueryInfo;


typedef struct {
    VOS_UINT8        cid;
    VOS_UINT8        reserved[3];
    TAF_PDP_Authdata authDataInfo;

} TAF_AUTHDATA_QueryInfo;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 opLinkdRabId : 1;
    VOS_UINT32 opPdpAddr : 1;
    VOS_UINT32 opApn : 1;
    VOS_UINT32 opEmergencyInd : 1;
    VOS_UINT32 opImCnSignalFlg : 1;
    VOS_UINT32 opCause : 1;
    VOS_UINT32 opUmtsQos : 1;
    VOS_UINT32 opEpsQos : 1;
    VOS_UINT32 opTft : 1;
    VOS_UINT32 opIpv4Mtu : 1;
    /* 用于VERRIZON定制需求中,指示FF00H是否可用。 1:可用 0:不可用 */
    VOS_UINT32 opCustomPco : 1;
    /* 用于定制需求中,把EPDG地址上报给APS时,指示EPDG地址信息是否存在。 1:存在 0:不存在 */
    VOS_UINT32 opEpdgInfo : 1;
    VOS_UINT32 opLinkedQfi : 1;
    VOS_UINT32 opNrQos : 1;
    VOS_UINT32 opRabId : 1;
    VOS_UINT32 opQfi : 1;
    VOS_UINT32 opSpare : 16;

    VOS_UINT8                cid;
    VOS_UINT8                rabId;
    VOS_UINT8                linkdRabId;
    TAF_PDP_EmcIndUint8      emergencyInd;
    TAF_PDP_ImCnSigFlagUint8 imCnSignalFlg;

    VOS_UINT8            pduSessionId;
    VOS_UINT8            qfi;
    VOS_UINT8            linkedQfi;
    TAF_PS_CauseUint32   cause;
    TAF_PS_RatTypeUint32 cnRatType; /* 核心网制式LTE/NR/G/W/1X/HRPD */

    TAF_PDP_Addr pdpAddr;
    TAF_PDP_Apn  apn;
    TAF_UMTS_Qos umtsQos;
    TAF_EPS_Qos  epsQos;
    PS_NR_Qos    nrQos;

    TAF_PDP_Dns  dns;
    TAF_PDP_Nbns nbns;

    TAF_PDP_Ipv4PcscfList ipv4PcscfList;

    TAF_PDP_Gateway gateWay;
    TAF_PDP_Ipv6Dns ipv6Dns;

    TAF_PDP_Ipv6PcscfList ipv6PcscfList;

    TAF_PDP_Tft tft;

    VOS_UINT16       ipv4Mtu;
    PS_SSC_ModeUint8 sscMode;
    VOS_UINT8        qosRuleNum;

    TAF_PS_CustomPcoInfo customPcoInfo;

    TAF_EPDG_Info epdgInfo;

    TAF_PS_DomainTypeUint8 psCallDomain;
    PS_IFACE_IdUint8       ifaceId;
    VOS_UINT8              reserved[2];

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    /* TODO:为了减少非NR版本中消息大小，暂时用宏控制 */
    PS_NR_QosRuleInfo qosRule[PS_MAX_QOS_RULE_NUM_IN_QOS_FLOW]; /* QoS Rule List */

    PS_EPS_MapQosFlowList mapQosFlowList;
    PS_EPS_MapQosRuleList mapQosRuleList;
#endif
} TAF_PS_CallPdpActivateCnf;


typedef TAF_PS_CallPdpActivateCnf TAF_PS_CallPdpActivateInd;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 opCauseEx : 1;
    VOS_UINT32 opBackOffTimer : 1;
    VOS_UINT32 opAllowedSscMode : 1;
    VOS_UINT32 opSpare : 29;

    VOS_UINT8         cid;
    TAF_PDP_TypeUint8 pdpType;
    union {
        struct {
            VOS_UINT8 sscMode1Allowed : 1; /* VOS_TRUE :allowed，VOS_FALSE :not allowed */
            VOS_UINT8 sscMode2Allowed : 1; /* VOS_TRUE :allowed，VOS_FALSE :not allowed */
            VOS_UINT8 sscMode3Allowed : 1; /* VOS_TRUE :allowed，VOS_FALSE :not allowed */
            VOS_UINT8 spare : 5;
        };
        VOS_UINT8 allowedSscMode; /* allowed SSC MODE */
    };
    VOS_UINT8          resv;
    TAF_PS_CauseUint32 cause;

    /*
     * enCauseEx存在条件:
     * enPdpType为IPv4/IPv6/PPP.
     * enCauseEx可能的取值:
     * TAF_PS_CAUSE_SM_NW_PDP_TYPE_IPV4_ONLY_ALLOWED
     * TAF_PS_CAUSE_SM_NW_PDP_TYPE_IPV6_ONLY_ALLOWED
     * TAF_PS_CAUSE_SM_NW_SINGLE_ADDR_BEARERS_ONLY_ALLOWED
     * enCauseEx如何使用:
     * 用户发起双栈业务建立请求(IPv4v6), TAF上报了REJECT事件, 如果enPdpType
     * 为单栈(IPv4/IPv6), 用户需要根据enCauseEx, 决定是否继续发起业务请求.
     * enCauseEx使用事例:
     * 用户发起双栈业务建立请求(IPv4v6), 网侧建立IPv4类型承载, 同时携带原因值
     * TAF_PS_CAUSE_SM_NW_SINGLE_ADDR_BEARERS_ONLY_ALLOWED, 由于该承载不满足
     * 最小QOS, 被内部去激活, TAF上报REJECT事件, IP类型为IPv4, enCause为
     * QOS_NOT_ACCEPT, enCauseEx为TAF_PS_CAUSE_SM_NW_SINGLE_ADDR_BEARERS_ONLY_ALLOWED,
     * 用户需要尝试发起IPv6业务建立请求.
     */
    TAF_PS_CauseUint32 causeEx;
    VOS_UINT32         backOffTimer; /* 单位s */
} TAF_PS_CallPdpActivateRej;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 opApn : 1;
    VOS_UINT32 opPdpAddr : 1;
    VOS_UINT32 opLinkdRabId : 1;
    VOS_UINT32 opEmergencyInd : 1;
    VOS_UINT32 opImCnSignalFlg : 1;
    VOS_UINT32 opSpare : 27;

    VOS_UINT8                cid;
    VOS_UINT8                rabId;
    VOS_UINT8                linkdRabId;
    TAF_PDP_EmcIndUint8      emergencyInd;
    TAF_PDP_ImCnSigFlagUint8 imCnSignalFlg;
    VOS_UINT8                reserved[3];

    TAF_PDP_Addr pdpAddr;
    TAF_PDP_Apn  apn;

} TAF_PS_CallPdpManageInd;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 opUmtsQos : 1;
    VOS_UINT32 opEpsQos : 1;
    VOS_UINT32 opNrQos : 1;
    VOS_UINT32 opTft : 1;
    VOS_UINT32 opIpv4Mtu : 1;
    VOS_UINT32 opRabId : 1;
    VOS_UINT32 opQfi : 1;
    VOS_UINT32 opSpare : 25;

    VOS_UINT8            cid;
    VOS_UINT8            rabId;
    VOS_UINT8            qfi;
    VOS_UINT8            pduSessionId;
    TAF_PS_RatTypeUint32 cnRatType; /* 核心网制式LTE/NR/G/W/1X/HRPD */

    TAF_UMTS_Qos umtsQos;
    TAF_EPS_Qos  epsQos;
    PS_NR_Qos    nrQos;

    TAF_PDP_Dns  dns;
    TAF_PDP_Nbns nbns;

    TAF_PDP_Ipv4PcscfList ipv4PcscfList;

    TAF_PDP_Ipv6Dns ipv6Dns;

    TAF_PDP_Ipv6PcscfList ipv6PcscfList;

    TAF_PDP_Tft tft;

    VOS_UINT16 ipv4Mtu;
    VOS_UINT8  qosRuleNum;
    VOS_UINT8  reserved1[1];

#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    /* TODO:为了减少非NR版本中消息大小，暂时用宏控制 */
    PS_NR_QosRuleInfo qosRule[PS_MAX_QOS_RULE_NUM_IN_QOS_FLOW]; /* QoS Rule List */

    PS_EPS_MapQosFlowList mapQosFlowList;
    PS_EPS_MapQosRuleList mapQosRuleList;
#endif

} TAF_PS_CallPdpModifyCnf;


typedef TAF_PS_CallPdpModifyCnf TAF_PS_CallPdpModifyInd;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8          cid;
    VOS_UINT8          rabId;
    VOS_UINT8          pduSessionId;
    VOS_UINT8          qfi;
    TAF_PS_CauseUint32 cause;

} TAF_PS_CallPdpModifyRej;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 opBackOffTimer : 1;
    VOS_UINT32 opSpare : 31;

    VOS_UINT8         cid;
    VOS_UINT8         rabId;
    TAF_PDP_TypeUint8 pdpType;

    VOS_UINT8          pduSessionId;
    VOS_UINT8          qfi;
    PS_IFACE_IdUint8   ifaceId;
    VOS_UINT8          primFlag;
    VOS_UINT8          reserved;
    TAF_PS_CauseUint32 cause;
    VOS_UINT32         backOffTimer; /* 单位s */
} TAF_PS_CallPdpDeactivateCnf;


typedef TAF_PS_CallPdpDeactivateCnf TAF_PS_CallPdpDeactivateInd;


typedef struct {
    TAF_Ctrl ctrl;

} TAF_PS_CallPdpPdpDisconnectInd;


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 limitFlg; /* 1:禁止PDP激活 0:允许PDP激活 */
    VOS_UINT8 reserve[3];
    /* PDP激活被拒原因值，仅在FLG为1时有效；FLG为0时，CAUSE值固定为0 */
    TAF_PS_CauseUint32 cause;
} TAF_PS_CallLimitPdpActInd;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
} TAF_PS_CommonCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_CommonReq;


typedef struct {
    VOS_UINT32 opPdpType : 1;        /* PdpType */
    VOS_UINT32 opApn : 1;            /* aucApn */
    VOS_UINT32 opPdpAddr : 1;        /* aucPdpAddr */
    VOS_UINT32 opPdpDcomp : 1;       /* PdpDcomp */
    VOS_UINT32 opPdpHcomp : 1;       /* PdpHcomp */
    VOS_UINT32 opIpv4AddrAlloc : 1;  /* Ipv4AddrAlloc */
    VOS_UINT32 opEmergencyInd : 1;   /* Emergency Indication */
    VOS_UINT32 opPcscfDiscovery : 1; /* P-CSCF discovery */
    VOS_UINT32 opImCnSignalFlg : 1;  /* IM CN Signalling Flag */
    VOS_UINT32 opNasSigPrioInd : 1;  /* Nas Signaling Low pri Ind */
    VOS_UINT32 opSscMode : 1;
    VOS_UINT32 opSNssai : 1;
    VOS_UINT32 opPrefAccessType : 1;
    VOS_UINT32 opRQosInd : 1;
    VOS_UINT32 opMh6Pdu : 1;
    VOS_UINT32 opAlwaysOnInd : 1;
    VOS_UINT32 opSpare : 16;

    VOS_UINT8 defined; /* 0:undefined, 1:defined */

    VOS_UINT8 cid;

    /* 1 IP Internet Protocol (IETF STD 5) */
    /* 2 IPV6 Internet Protocol, version 6 (IETF RFC 2460) */
    /* 3 IPV4V6 Virtual <PDP_type> introduced to handle dual IP stack UE capability. (See 3GPP TS 24.301 [83]) */
    TAF_PDP_TypeUint8       pdpType;
    TAF_PS_AlwaysOnIndUint8 alwaysOnInd;

    /* Access Point Name */
    VOS_UINT8 apn[TAF_MAX_APN_LEN + 1];

    /* the MT in the address space applicable to the PDP. */
    TAF_PDP_Addr pdpAddr;

    /* 0 - off  (default if value is omitted) */
    /* 1 - on (manufacturer preferred compression) */
    /* 2 - V.42bis */
    /* 3 - V.44 */
    TAF_PDP_DCompUint8 pdpDcomp;

    /* 0 - off (default if value is omitted) */
    /* 1 - on (manufacturer preferred compression) */
    /* 2 - RFC1144 (applicable for SNDCP only) */
    /* 3 - RFC2507 */
    /* 4 - RFC3095 (applicable for PDCP only) */
    TAF_PDP_HCompUint8 pdpHcomp;

    /* 0 - IPv4 Address Allocation through NAS Signaling (default if omitted) */
    /* 1 - IPv4 Address Allocated through DHCP */
    TAF_PDP_Ipv4AddrAllocTypeUint8 ipv4AddrAlloc;

    /* 0 - PDP context is not for emergency bearer services */
    /* 1 - PDP context is for emergency bearer services */
    TAF_PDP_EmcIndUint8 emergencyFlg;

    /* 0 - Preference of P-CSCF address discovery not influenced by +CGDCONT */
    /* 1 - Preference of P-CSCF address discovery through NAS Signalling */
    /* 2 - Preference of P-CSCF address discovery through DHCP */
    TAF_PDP_PcscfDiscoveryUint8 pcscfDiscovery;

    /* 0 - UE indicates that the PDP context is not for IM CN subsystem-related signalling only */
    /* 1 - UE indicates that the PDP context is for IM CN subsystem-related signalling only */
    TAF_PDP_ImCnSigFlagUint8 imCnSignalFlg;

    /*
     * 0 - Indicates that this PDP context is to be activated with the value for the low priority indicator
     *     configured in the MT.
     * 1 - Indicates that this PDP context is to be activated with the value for the low priority indicator
     *     set to "MS is not configured for NAS signalling low priority".
     */
    TAF_PDP_NasSigPrioIndUint8 nasSigPrioInd;

    TAF_PS_SscModeUint8            sscMode;
    TAF_PS_PrefAccessTypeUint8     prefAccessType;
    TAF_PS_ReflectQosIndUint8      rQosInd;
    TAF_PS_Ipv6MultiHomingIndUint8 mh6Pdu;
    VOS_UINT8                      resv;
    /* S-NSSAI */
    PS_S_NSSAI_STRU sNssai;

} TAF_PDP_PrimContextExt;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PDP_PrimContextExt pdpContextInfo;
} TAF_PS_SetPrimPdpContextInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetPrimPdpContextInfoCnf;


typedef struct {
    VOS_UINT8 cid;
    VOS_UINT8 imsFlag;
    VOS_UINT8 reserved[2];
} TAF_IMS_PdpCfg;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_IMS_PdpCfg imsPdpCfg;
} TAF_PS_SetImsPdpCfgReq;

typedef TAF_PS_CommonCnf TAF_PS_SetImsPdpCfgCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8 dormantTimer;
    VOS_UINT8 reserved[3];
} TAF_PS_Set1XDormTimerReq;

typedef TAF_PS_CommonCnf TAF_PS_Set1xDormTimerCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_Get1XDormTimerReq;


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 socmDormTiVal;
    VOS_UINT8 userCfgDormTival;
    VOS_UINT8 reserved[2];
} TAF_PS_Get1XDormTimerCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetPrimPdpContextInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32   cause;
    VOS_UINT32           cidNum;
    TAF_PRI_PdpQueryInfo pdpContextQueryInfo[0]; //lint !e43
} TAF_PS_GetPrimPdpContextInfoCnf;

/*
 * 描述: 针对命令 +CGDSCONT
 * ID: ID_MSG_TAF_PS_SET_SEC_PDP_CONTEXT_INFO_REQ
 *     ID_MSG_TAF_PS_SET_SEC_PDP_CONTEXT_INFO_CNF
 *     ID_MSG_TAF_PS_GET_SEC_PDP_CONTEXT_INFO_REQ
 *     ID_MSG_TAF_PS_GET_SEC_PDP_CONTEXT_INFO_CNF
 * REQ: TAF_PS_SetSecPdpContextInfoReq; TAF_PS_GetSecPdpContextInfoReq
 * CNF: TAF_PS_SetSecPdpContextInfoCnf; TAF_PS_GetSecPdpContextInfoCnf
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opLinkdCid : 1;
    VOS_UINT32 opPdpDcomp : 1;
    VOS_UINT32 opPdpHcomp : 1;
    VOS_UINT32 opImCnSignalFlg : 1;
    VOS_UINT32 opSpare : 28;

    VOS_UINT8          defined;  /* 0:undefined, 1:defined */
    VOS_UINT8          cid;      /* dedicated EPS Bearer context */
    VOS_UINT8          linkdCid; /* Default EPS Bearer context */
    TAF_PDP_DCompUint8 pdpDcomp; /* no used in LTE */
    TAF_PDP_HCompUint8 pdpHcomp; /* no used in LTE */
    /* 0 - UE indicates that the PDP context is not for IM CN subsystem-related signalling only */
    /* 1 - UE indicates that the PDP context is for IM CN subsystem-related signalling only */
    TAF_PDP_ImCnSigFlagUint8 imCnSignalFlg;
    VOS_UINT8                reserved[2];

} TAF_PDP_SecContextExt;


typedef struct {
    VOS_UINT8          cid;      /* [1，11] */
    VOS_UINT8          linkdCid; /* [1，11] */
    TAF_PDP_DCompUint8 pdpDcomp; /* 数据压缩算法 */
    TAF_PDP_HCompUint8 pdpHcomp; /* 头压缩算法 */

    TAF_PDP_ImCnSigFlagUint8 imCnSignalFlg;
    VOS_UINT8                reserved[3];

} TAF_PDP_SecContext;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PDP_SecContextExt pdpContextInfo;

} TAF_PS_SetSecPdpContextInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetSecPdpContextInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetSecPdpContextInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_PDP_SecContext pdpContextQueryInfo[0]; //lint !e43
} TAF_PS_GetSecPdpContextInfoCnf;


typedef struct {
    VOS_UINT8 cid[TAF_MAX_CID + 1];
} TAF_CID_List;


typedef struct {
    VOS_UINT8              cid;
    VOS_UINT8              state;
    TAF_PS_BearerTypeUint8 bearerType;
    VOS_UINT8              reserved[1];
} TAF_CID_State;


typedef struct {
    VOS_UINT8 state; /* 0 - deactivated,1 - activated */
    VOS_UINT8 reserved[3];

    VOS_UINT8 cid[TAF_MAX_CID + 1];
} TAF_CID_ListState;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_CID_ListState cidListStateInfo;
} TAF_PS_SetPdpStateReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
} TAF_PS_SetPdpStateCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetPdpStateReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_CID_State      cidStateInfo[0]; //lint !e43
} TAF_PS_GetPdpStateCnf;

/*
 * 描述: 针对命令 +CGTFT
 * ID: ID_MSG_L4A_SET_CGTFT_REQ
 *     ID_MSG_L4A_SET_CGTFT_CNF
 *     ID_MSG_L4A_READ_CGTFT_REQ
 *     ID_MSG_L4A_READ_CGTFT_CNF
 * REQ: L4A_SET_CGTFT_REQ_STRU;L4A_READ_CGTFT_REQ_STRU;
 * CNF: L4A_SET_CGTFT_CNF_STRU;L4A_READ_CGTFT_CNF_STRU;
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opPktFilterId : 1;   /* ucPacketFilterId */
    VOS_UINT32 opPrecedence : 1;    /* ucPrecedence */
    VOS_UINT32 opSrcIp : 1;         /* aucSourceIpAddr,Mask */
    VOS_UINT32 opProtocolId : 1;    /* ucProtocolId */
    VOS_UINT32 opDestPortRange : 1; /* destination port range */
    VOS_UINT32 opSrcPortRange : 1;  /* source port range */
    VOS_UINT32 opSpi : 1;           /* ipsec security parameter indexNum */
    VOS_UINT32 opTosMask : 1;       /* type of service */
    VOS_UINT32 opFlowLable : 1;     /* ulFlowLable */
    VOS_UINT32 opDirection : 1;     /* Direction */
    VOS_UINT32 opNwPktFilterId : 1; /* NWPacketFltId */
    VOS_UINT32 opLocalIpv4AddrAndMask : 1;
    VOS_UINT32 opLocalIpv6AddrAndMask : 1;
    VOS_UINT32 opQri : 1;
    VOS_UINT32 opSpare : 18;

    VOS_UINT8 cid;

    VOS_UINT8 defined;

    VOS_UINT8 packetFilterId; /* Packet Filter Id,value range from 1 to 16 */

    /* evaluation precedence indexNum,The value range is from 0 to 255 */
    VOS_UINT8 precedence;

    VOS_UINT8 protocolId; /* protocol number,value range from 0 to 255 */

    VOS_UINT8 reserved[3];

    /* source address and subnet mask */
    TAF_PDP_Addr sourceIpaddr;
    TAF_PDP_Addr sourceIpMask;

    /* destination port range */
    VOS_UINT16 lowDestPort;  /* value range from 0 to 65535 */
    VOS_UINT16 highDestPort; /* value range from 0 to 65535 */

    /* source port range */
    VOS_UINT16 lowSourcePort;  /* value range from 0 to 65535 */
    VOS_UINT16 highSourcePort; /* value range from 0 to 65535 */

    /* ipsec security parameter indexNum */
    VOS_UINT32 secuParaIndex;

    /* only for ipv6 */
    /* value range is from 00000 to FFFFF */
    VOS_UINT32 flowLable;

    /* type of service */
    VOS_UINT8 typeOfService;     /* value range from 0 to 255 */
    VOS_UINT8 typeOfServiceMask; /* value range from 0 to 255 */

    /* 0 - Pre-Release 7 TFT filter (see 3GPP TS 24.008 [8], table 10.5.162) */
    /* 1 - Uplink */
    /* 2 - Downlink */
    /* 3 - Birectional (Up & Downlink) (default if omitted) */
    VOS_UINT8 direction;

    /* only for CGTFTRDP */
    VOS_UINT8 nwPktFilterId; /* value range from 1 to 16 */

    /* local address and subnet mask */
    VOS_UINT8 localIpv4Addr[TAF_IPV4_ADDR_LEN];
    VOS_UINT8 localIpv4Mask[TAF_IPV4_ADDR_LEN];
    VOS_UINT8 localIpv6Addr[TAF_IPV6_ADDR_LEN];
    VOS_UINT8 localIpv6Prefix;
    VOS_UINT8 qri;
    VOS_UINT8 reserved2[2];
} TAF_TFT_Ext;

typedef struct {
    VOS_UINT32  cid;
    VOS_UINT32  pfNum;
    TAF_TFT_Ext tftInfo[TAF_MAX_SDF_PF_NUM];
} TAF_PF_Tft;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_TFT_Ext tftInfo;
} TAF_PS_SetTftInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetTftInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetTftInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_TFT_QureyInfo  tftQueryInfo[0]; //lint !e43
} TAF_PS_GetTftInfoCnf;

/*
 * 描述: 针对命令 +CGEQREQ
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opTrafficClass : 1;
    VOS_UINT32 opDeliverOrder : 1;
    VOS_UINT32 opDeliverErrSdu : 1;
    VOS_UINT32 opMaxSduSize : 1;
    VOS_UINT32 opMaxBitUl : 1;
    VOS_UINT32 opMaxBitDl : 1;
    VOS_UINT32 opResidualBer : 1;
    VOS_UINT32 opSduErrRatio : 1;
    VOS_UINT32 opTransDelay : 1;
    VOS_UINT32 opTraffHandlePrior : 1;
    VOS_UINT32 opGtdBitUl : 1;
    VOS_UINT32 opGtdBitDl : 1;
    VOS_UINT32 opSrcStatisticsDes : 1;
    VOS_UINT32 opSignalIndication : 1;
    VOS_UINT32 opSpare : 18;

    VOS_UINT8 defined;

    VOS_UINT8 cid;

    VOS_UINT8 trafficClass; /*
                             * 0 : Conversational class
                             * 1 : Streaming class
                             * 2 : Interactive class
                             * 3 : Background class
                             * 4 : Subscribed value
                             */
    VOS_UINT8 sduErrRatio;  /*
                             * 0 : subscribed value
                             * 1 : 1*10-2
                             * 2 : 7*10-3
                             * 3 : 1*10-3
                             * 4 : 1*10-4
                             * 5 : 1*10-5
                             * 6 : 1*10-6
                             * 7 : 1*10-1
                             */
    VOS_UINT16 maxSduSize;  /*
                             * 0 : Subscribed value
                             * 1~1520: 1 ~1520 octets
                             */

    VOS_UINT8 deliverOrder; /*
                             * 0 : Without delivery order
                             * 1 : With delivery order
                             * 2 : Subscribed value
                             */

    VOS_UINT8 deliverErrSdu; /*
                              * 0 : Erroneous SDUs are not delivered
                              * 1 : Erroneous SDUs are delivered
                              * 2 : No detect
                              * 3 : subscribed value
                              */

    VOS_UINT32 maxBitUl;        /*
                                 * 0 : Subscribed value
                                 * 1~256000: 1kbps ~ 256000kbps
                                 */
    VOS_UINT32 maxBitDl;        /* 同ulMaxBitUl */
    VOS_UINT32 gtdBitUl;        /* 同ulMaxBitUl */
    VOS_UINT32 gtdBitDl;        /* 同ulMaxBitUl */
    VOS_UINT8  residualBer;     /*
                                 * 0 : subscribed value
                                 * 1 : 5*10-2
                                 * 2 : 1*10-2
                                 * 3 : 5*10-3
                                 * 4 : 4*10-3
                                 * 5 : 1*10-3
                                 * 6 : 1*10-4
                                 * 7 : 1*10-5
                                 * 8 : 1*10-6
                                 * 9 : 6*10-8
                                 */
    VOS_UINT8 traffHandlePrior; /*
                                 * 0 : Subscribed value
                                 * 1 : Priority level 1
                                 * 2 : Priority level 2
                                 * 3 : Priority level 3
                                 */
    VOS_UINT16 transDelay;      /*
                                 * 0 : Subscribed value
                                 * 1~4100 : 1ms~4100ms
                                 */

    VOS_UINT8 srcStatisticsDes; /*
                                 * 0: Characteristics of SDUs is unknown
                                 * 1: Characteristics of SDUs corresponds to a speech source
                                 */

    VOS_UINT8 signalIndication; /*
                                 * 0: PDP context is not optimized for signalling
                                 * 1: PDP context is optimized for signalling
                                 */

    VOS_UINT8 reserved[2];

} TAF_UMTS_QosExt;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_UMTS_QosExt umtsQosInfo;
} TAF_PS_SetUmtsQosInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetUmtsQosInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetUmtsQosInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32    cause;
    VOS_UINT32            cidNum;
    TAF_UMTS_QosQueryInfo umtsQosQueryInfo[0]; //lint !e43
} TAF_PS_GetUmtsQosInfoCnf;

/*
 * 描述: 针对命令 +CGEQMIN
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    TAF_Ctrl ctrl;

    TAF_UMTS_QosExt umtsQosMinInfo;
} TAF_PS_SetUmtsQosMinInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetUmtsQosMinInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetUmtsQosMinInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32    cause;
    VOS_UINT32            cidNum;
    TAF_UMTS_QosQueryInfo umtsQosQueryInfo[0]; //lint !e43
} TAF_PS_GetUmtsQosMinInfoCnf;

/*
 * 描述: 针对命令 +CGEQNEG
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    TAF_Ctrl ctrl;

    TAF_CID_List cidListInfo;
} TAF_PS_GetDynamicUmtsQosInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32    cause;
    VOS_UINT32            cidNum;
    TAF_UMTS_QosQueryInfo umtsQosQueryInfo[0]; //lint !e43
} TAF_PS_GetDynamicUmtsQosInfoCnf;

/*
 * 描述: 针对命令 +CGMOD
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */

typedef struct {
    TAF_Ctrl ctrl;

    TAF_CID_List cidListInfo;
} TAF_PS_CallModifyReq;

typedef TAF_PS_CommonCnf TAF_PS_CallModifyCnf;

/*
 * 描述: 针对命令 +CGANS
 * ID: ID_MSG_L4A_SET_CGANS_RSP
 *     ID_MSG_L4A_SET_CGANS_CNF
 *     ID_MSG_L4A_CGANS_IND
 * RSP: L4A_SET_CGANS_RSP_STRU
 * CNF: L4A_SET_CGANS_CNF_STRU
 * IND: L4A_CGANS_IND_STRU
 * 说明: L4A_CGANS_IND_STRU 是网络发起事件,后续可能不会使用该原语
 */
typedef struct {
    VOS_UINT8 l2P[20];
    VOS_UINT8 cid;
    /* 如果是^CGANS命令，为VOS_TRUE。否则为VOS_FALSE */
    VOS_UINT8 ansExtFlg;
    VOS_UINT8 reserved[2];
} TAF_PS_Answer;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_Answer ansInfo;
} TAF_PS_CallAnswerReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;

    VOS_UINT8 cid;

    VOS_UINT8 reserved[3];
} TAF_PS_CallAnswerCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_CallHangupReq;

typedef TAF_PS_CommonCnf TAF_PS_CallHangupCnf;

/*
 * 描述 : 针对命令 +CGPADDR
 * ID   :
 * REQ  :
 * CNF  :
 * IND  :
 * 说明 : ...
 */
/*
 * 协议表格:
 * 结构说明:
 */

typedef struct {
    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];

    /* the MT in the address space applicable to the PDP */
    TAF_PDP_Addr pdpAddr;
} TAF_PDP_AddrQueryInfo;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_CID_List cidListInfo;
} TAF_PS_GetPdpIpAddrInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32    cause;
    VOS_UINT32            cidNum;
    TAF_PDP_AddrQueryInfo pdpAddrQueryInfo[0]; //lint !e43
} TAF_PS_GetPdpIpAddrInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    VOS_UINT32         cid[TAF_MAX_CID + 1];
} TAF_PS_GetPdpContextInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetPdpContextInfoReq;

/*
 * 描述: 针对命令 +CGAUTO
 * ID: ID_MSG_L4A_SET_CGAUTO_REQ
 *     ID_MSG_L4A_SET_CGAUTO_CNF
 *     ID_MSG_L4A_READ_CGAUTO_REQ
 *     ID_MSG_L4A_READ_CGAUTO_CNF
 * REQ: L4A_SET_CGAUTO_REQ_STRU; L4A_READ_CGAUTO_REQ_STRU
 * CNF: L4A_SET_CGAUTO_CNF_STRU; L4A_READ_CGAUTO_CNF_STRU
 * IND:
 * 说明: ...
 */
typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 ansMode;
} TAF_PS_SetAnswerModeInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetAnswerModeInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetAnswerModeInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         ansMode;
} TAF_PS_GetAnswerModeInfoCnf;

/*
 * 描述: 针对命令 +CGCONTRDP
 * ID: ID_MSG_L4A_SET_CGCONTRDP_REQ
 *     ID_MSG_L4A_SET_CGCONTRDP_CNF
 * REQ: L4A_SET_CGCONTRDP_REQ_STRU
 * CNF: L4A_SET_CGCONTRDP_CNF_STRU
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opBearerId : 1;      /* BearerId */
    VOS_UINT32 opApn : 1;           /* aucApn */
    VOS_UINT32 opIpAddr : 1;        /* aucIpaddr */
    VOS_UINT32 opSubMask : 1;       /* aucSubnetMask */
    VOS_UINT32 opGwAddr : 1;        /* aucGWAddr */
    VOS_UINT32 opDnsPrimAddr : 1;   /* aucDNSPrimAddr */
    VOS_UINT32 opDnsSecAddr : 1;    /* aucDNSSecAddr */
    VOS_UINT32 opPcscfPrimAddr : 1; /* aucPCSCFPrimAddr */
    VOS_UINT32 opPcscfSecAddr : 1;  /* aucPCSCFSecAddr */
    VOS_UINT32 opImCnSignalFlg : 1;
    VOS_UINT32 opIpv4Mtu : 1; /* usIpv4Mtu */
    VOS_UINT32 opQfi : 1;     /* ucQfi */
    VOS_UINT32 opSNssai : 1;  /* aucSNssai */
    VOS_UINT32 opRqTimer : 1; /* ulRqTimer */
    VOS_UINT32 opSpare : 18;

    VOS_UINT8 primayCid; /* default EPS bearer context */
    /* a numeric parameter which identifies the bearer */
    VOS_UINT8 bearerId;

    /* the IM CN subsystem-related signalling flag */
    TAF_PDP_ImCnSigFlagUint8       imCnSignalFlg;
    VOS_UINT8                      pduSessionId;
    VOS_UINT8                      qfi;
    TAF_PS_SscModeUint8            sscMode;
    PS_PDU_SessionAlwaysOnIndUint8 alwaysOnInd;
    VOS_UINT8                      reserved[1];
    TAF_PS_AccessTypeUint16        accessType;
    VOS_UINT16                     ipv4Mtu;
    VOS_UINT32                     rqTimer;

    /* Access Pointer Name */
    VOS_UINT8 apn[TAF_MAX_APN_LEN + 1];

    /* the IP Address of the MT */
    TAF_PDP_Addr pdpAddr;

    /* the subnet mask for the IP Address of the MT */
    TAF_PDP_Addr subnetMask;

    /* the Gateway Address of the MT */
    TAF_PDP_Addr gwAddr;

    /* the IP Address of the primary DNS Server */
    TAF_PDP_Addr dnsPrimAddr;

    /* the IP address of the secondary DNS Server */
    TAF_PDP_Addr dnsSecAddr;

    /* the IP Address of the primary P-CSCF Server */
    TAF_PDP_Addr pcscfPrimAddr;

    /* the IP Address of the secondary P-CSCF Server */
    TAF_PDP_Addr pcscfSecAddr;

    /* S-NSSAI */
    PS_S_NSSAI_STRU sNssai;

} TAF_PDP_DynamicPrimExt;

typedef struct {
    TAF_Ctrl ctrl;

    /* 0xff-if the parameter <cid> is omitted */
    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];
} TAF_PS_GetDynamicPrimPdpContextInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32     cause;
    VOS_UINT32             cidNum;
    TAF_PDP_DynamicPrimExt pdpContxtInfo[0]; //lint !e43
} TAF_PS_GetDynamicPrimPdpContextInfoCnf;

/*
 * 描述: 针对命令 +CGSCONTRDP
 * ID: ID_MSG_L4A_SET_CGSCONTRDP_REQ
 *     ID_MSG_L4A_SET_CGSCONTRDP_CNF
 * REQ: L4A_SET_CGSCONTRDP_REQ_STRU
 * CNF: L4A_SET_CGSCONTRDP_CNF_STRU
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opQfi : 1;
    VOS_UINT32 opSpare : 31;

    VOS_UINT8 cid;

    /* default EPS bearer context */
    VOS_UINT8 primaryCid;

    /* a numeric parameter which identifies the bearer */
    VOS_UINT8 bearerId;

    /* 这个参数是全局分配的PDU Session ID */
    VOS_UINT8 pduSessionId;
    VOS_UINT8 qfi;
    VOS_UINT8 reserved[3];
} TAF_PDP_DynamicSecExt;

typedef struct {
    TAF_Ctrl ctrl;

    /* 0xff-if the parameter <cid> is omitted */
    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];
} TAF_PS_GetDynamicSecPdpContextInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32    cause;
    VOS_UINT32            cidNum;
    TAF_PDP_DynamicSecExt pdpContxtInfo[0]; //lint !e43
} TAF_PS_GetDynamicSecPdpContextInfoCnf;

/*
 * 描述: 针对命令 +CGTFTRDP
 * ID: ID_MSG_L4A_SET_CGTFTRDP_REQ
 *     ID_MSG_L4A_SET_CGTFTRDP_CNF
 * REQ: L4A_SET_CGTFTRDP_REQ_STRU
 * CNF: L4A_SET_CGTFTRDP_CNF_STRU
 * IND:
 * 说明: ...
 */
typedef struct {
    TAF_Ctrl ctrl;

    /* 0xff-if the parameter <cid> is omitted */
    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];
} TAF_PS_GetDynamicTftInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_PF_Tft         pfTftInfo[0]; //lint !e43

} TAF_PS_GetDynamicTftInfoCnf;

/*
 * 描述: 针对命令 +CGEQOS
 * ID: ID_MSG_L4A_SET_CGEQOS_REQ
 *     ID_MSG_L4A_SET_CGEQOS_CNF
 *     ID_MSG_L4A_READ_CGEQOS_REQ
 *     ID_MSG_L4A_READ_CGEQOS_CNF
 * REQ: L4A_SET_CGEQOS_REQ_STRU L4A_READ_CGEQOS_REQ_STRU
 * CNF: L4A_SET_CGEQOS_CNF_STRU L4A_READ_CGEQOS_CNF_STRU
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opQci : 1;   /* QCI */
    VOS_UINT32 opDlgbr : 1; /* DLGBR */
    VOS_UINT32 opUlgbr : 1; /* ULGBR */
    VOS_UINT32 opDlmbr : 1; /* DLMBR */
    VOS_UINT32 opUlmbr : 1; /* ULMBR */
    VOS_UINT32 opSpare : 27;

    VOS_UINT8 cid;

    /* [1 - 4]value range for guranteed bit rate Traffic Flows */
    /* [5 - 9]value range for non-guarenteed bit rate Traffic Flows */
    VOS_UINT8 qci;

    VOS_UINT8 reserved[2];

    /* DL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32 dlgbr;

    /* UL GBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32 ulgbr;

    /* DL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32 dlmbr;

    /* UL MBR in case of GBR QCI, The value is in kbit/s */
    VOS_UINT32 ulmbr;
} TAF_EPS_QosExt;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_EPS_QosExt epsQosInfo;
} TAF_PS_SetEpsQosInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetEpsQosInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetEpsQosInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_EPS_QosExt     epsQosInfo[0]; //lint !e43
} TAF_PS_GetEpsQosInfoCnf;

/*
 * 描述: 针对命令 +CGEQOSRDP
 * ID: ID_MSG_L4A_SET_CGEQOSRDP_REQ
 *     ID_MSG_L4A_SET_CGEQOSRDP_CNF
 * REQ: L4A_SET_CGEQOSRDP_REQ_STRU
 * CNF: L4A_SET_CGEQOSRDP_CNF_STRU
 * IND:
 * 说明: ...
 */
typedef struct {
    TAF_Ctrl ctrl;

    /* 0xff-if the parameter <cid> is omitted */
    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];
} TAF_PS_GetDynamicEpsQosInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_EPS_QosExt     epsQosInfo[0]; //lint !e43
} TAF_PS_GetDynamicEpsQosInfoCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8 reserve[4];
} TAF_PS_GetDsflowInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32   cause;
    TAF_DSFLOW_QueryInfo queryInfo;
} TAF_PS_GetDsflowInfoCnf;

/*
 * 描述 : 针对命令 ^DSFLOWCLR
 * ID   :
 * REQ  :
 * CNF  :
 * IND  :
 * 说明 : ...
 */
/*
 * 结构说明: 流量清除配置
 */
typedef struct {
    TAF_DSFLOW_ClearModeUint8 clearMode;
    VOS_UINT8                 nsapi;
    VOS_UINT8                 reserved[2];
} TAF_DSFLOW_ClearConfig;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_DSFLOW_ClearConfig clearConfigInfo;
} TAF_PS_ClearDsflowReq;


typedef TAF_PS_CommonCnf TAF_PS_ClearDsflowCnf;

/*
 * 描述 : 针对命令 ^DSFLOWRPT
 * ID   :
 * REQ  :
 * CNF  :
 * IND  :
 * 说明 : ...
 */

typedef struct {
    VOS_UINT32 rptEnabled;  /* 流量上报开启标记   */
    VOS_UINT32 timerLength; /* 流量上报定时器时长 */
} TAF_DSFLOW_ReportConfig;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_DSFLOW_ReportConfig reportConfigInfo;
} TAF_PS_ConfigDsflowRptReq;


typedef TAF_PS_CommonCnf TAF_PS_ConfigDsflowRptCnf;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_DSFLOW_Report reportInfo;
} TAF_PS_ReportDsflowInd;


typedef struct {
    VOS_UINT8 rptEnabled; /* VT 流量上报开启标记   */
    VOS_UINT8 reserved[3];
} TAF_VTFLOW_ReportConfig;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_VTFLOW_ReportConfig reportConfigInfo;
} TAF_PS_ConfigVtflowRptReq;


typedef TAF_PS_CommonCnf TAF_PS_ConfigVtflowRptCnf;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_VTFLOW_Report reportInfo;
} TAF_PS_ReportVtflowInd;

/*
 * 描述 : 针对命令 ^APDSFLOWRPT
 * ID   :
 * REQ  :
 * CNF  :
 * IND  :
 * 说明 : ...
 */

typedef struct {
    VOS_UINT32 rptEnabled;    /* 流量上报开启标记   */
    VOS_UINT32 fluxThreshold; /* 流量上报门限, 单位KByte */
} TAF_APDSFLOW_RptCfg;

/*
 * 结构说明: 流量信息上报结构定义
 */
typedef struct {
    VOS_UINT32 sendRate; /* 当前发送速率 */
    VOS_UINT32 recvRate; /* 当前接收速率 */

    TAF_DSFLOW_Info currentFlowInfo; /* 当前连接流量信息 */
    TAF_DSFLOW_Info totalFlowInfo;   /* 累计流量信息 */

} TAF_APDSFLOW_Report;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_APDSFLOW_RptCfg rptCfg;
} TAF_PS_SetApdsflowRptCfgReq;


typedef TAF_PS_CommonCnf TAF_PS_SetApdsflowRptCfgCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetApdsflowRptCfgReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32  cause;
    TAF_APDSFLOW_RptCfg rptCfg;
} TAF_PS_GetApdsflowRptCfgCnf;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_APDSFLOW_Report reportInfo;
} TAF_PS_ApdsflowReportInd;


typedef struct {
    VOS_UINT8 enabled;     /* 流量写NV开启标记   */
    VOS_UINT8 interval;    /* 流量写NV周期, 单位min */
    VOS_UINT8 reserved[2]; /* 保留字节 */
} TAF_DSFLOW_NvWriteCfg;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_DSFLOW_NvWriteCfg nvWriteCfg;
} TAF_PS_SetDsflowNvWriteCfgReq;


typedef TAF_PS_CommonCnf TAF_PS_SetDsflowNvWriteCfgCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetDsflowNvWriteCfgReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32    cause;
    TAF_DSFLOW_NvWriteCfg nvWriteCfg;
} TAF_PS_GetDsflowNvWriteCfgCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8 pktCdataInactivityTmrLen;
    VOS_UINT8 reserved[3];
} TAF_PS_SetCtaInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

} TAF_PS_GetCtaInfoReq;


typedef struct {
    TAF_Ctrl   ctrl;
    VOS_UINT32 rslt;

} TAF_PS_SetCtaInfoCnf;


typedef struct {
    TAF_Ctrl   ctrl;
    VOS_UINT32 rslt;

    VOS_UINT8 pktCdataInactivityTmrLen;
    VOS_UINT8 reserved[3];

} TAF_PS_GetCtaInfoCnf;



typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetCgmtuValueReq;


typedef struct {
    TAF_Ctrl   ctrl;
    VOS_UINT32 mtuValue;
} TAF_PS_GetCgmtuValueCnf;


typedef struct {
    TAF_Ctrl   ctrl;
    VOS_UINT32 mtuValue;
} TAF_PS_CgmtuValueChgInd;

/*
 * 描述: 针对命令 ^CGAUTH
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opAuth : 1;
    VOS_UINT32 opUserName : 1;
    VOS_UINT32 opPassword : 1;
    VOS_UINT32 opSpare : 29;

    VOS_UINT8 cid;
    VOS_UINT8 defined; /* 0:undefined, 1:defined */
    VOS_UINT8 auth;    /* 目前仅支持是否鉴权，以后扩充为支持哪种鉴权算法 */
    VOS_UINT8 reserved[1];
    VOS_UINT8 userName[TAF_MAX_AUTHDATA_USERNAME_LEN + 1];
    VOS_UINT8 passWord[TAF_MAX_AUTHDATA_PASSWORD_LEN + 1];
} TAF_PDP_AuthExt;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PDP_AuthExt pdpAuthInfo;
} TAF_PS_SetPdpAuthInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetPdpAuthInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetPdpAuthInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_AUTH_QueryInfo pdpAuthQueryInfo[0]; //lint !e43
} TAF_PS_GetPdpAuthInfoCnf;

/*
 * 描述: 针对命令 ^CGDNS
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT32 opPrimDnsAddr : 1;
    VOS_UINT32 opSecDnsAddr : 1;
    VOS_UINT32 opSpare : 30;

    VOS_UINT8 cid;     /* [1，11] 如果仅包含此参数，表示删除该Cid的DNS */
    VOS_UINT8 defined; /* 0:undefined, 1:defined */
    VOS_UINT8 reserved[2];
    VOS_UINT8 primDnsAddr[TAF_IPV4_ADDR_LEN];
    VOS_UINT8 secDnsAddr[TAF_IPV4_ADDR_LEN];
} TAF_PDP_DnsExt;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PDP_DnsExt pdpDnsInfo;
} TAF_PS_SetPdpDnsInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_SetPdpDnsInfoCnf;

typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetPdpDnsInfoReq;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_DNS_QueryInfo  pdpDnsQueryInfo[0]; //lint !e43
} TAF_PS_GetPdpDnsInfoCnf;

/*
 * 描述: 针对命令 ^TRIG
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    VOS_UINT8  mode;
    VOS_UINT8  nsapi;
    VOS_UINT8  times;
    VOS_UINT8  reserved[1];
    VOS_UINT32 length;
    VOS_UINT32 millisecond;
} TAF_GPRS_Data;

typedef struct {
    TAF_Ctrl ctrl;

    TAF_GPRS_Data gprsDataInfo;
} TAF_PS_TrigGprsDataReq;

typedef TAF_PS_CommonCnf TAF_PS_TrigGprsDataCnf;

/*
 * 描述: 针对命令 ^DWINS
 * ID:
 * REQ:
 * CNF:
 * IND:
 * 说明: ...
 */
typedef struct {
    TAF_Ctrl ctrl;

    /* 0:disabled, 1:enabled */
    VOS_UINT32 enabled;
} TAF_PS_ConfigNbnsFunctionReq;

typedef TAF_PS_CommonCnf TAF_PS_ConfigNbnsFunctionCnf;

/*
 * 结构说明: 拨号参数
 */
typedef struct {
    VOS_UINT32 opAuthType : 1;
    VOS_UINT32 opApn : 1;
    VOS_UINT32 opIpAddr : 1;
    VOS_UINT32 opUserName : 1;
    VOS_UINT32 opPassWord : 1;
    VOS_UINT32 opReqType : 1;
    VOS_UINT32 opPdpDcomp : 1;
    VOS_UINT32 opPdpHcomp : 1;
    VOS_UINT32 opIpv4AddrAlloc : 1;
    VOS_UINT32 opEmergencyInd : 1;
    VOS_UINT32 opPcscfDiscovery : 1;
    VOS_UINT32 opImCnSignalFlg : 1;
    VOS_UINT32 opDialRatType : 1;
    VOS_UINT32 opSpare : 19;

    /* connect id [1, 20] */
    VOS_UINT8 cid;

    /* 0 - no auth */
    /* 1 - PAP */
    /* 2 - CHAP */
    /* 3 - PAP OR CHAP */
    TAF_PDP_AuthTypeUint8 authType;

    /* 1 - IPv4 */
    /* 2 - IPv6 */
    /* 3 - IPv4v6 */
    /* 4 - PPP */
    TAF_PDP_TypeUint8 pdpType;

    /* 1 - initial request */
    /* 2 - Handover */
    TAF_PDP_RequestTypeUint8 pdpRequestType;

    /* access point name string with '\0' end */
    VOS_UINT8 apn[TAF_MAX_APN_LEN + 1];

    /* staitc address if needed */
    TAF_PDP_Addr pdpAddr;

    /* user name string with '\0' end */
    VOS_UINT8 userName[TAF_MAX_AUTHDATA_USERNAME_LEN + 1];

    /* user name string with '\0' end */
    VOS_UINT8 passWord[TAF_MAX_AUTHDATA_PASSWORD_LEN + 1];

    /* 0 - off (default if value is omitted) */
    /* 1 - on (manufacturer preferred compression) */
    /* 2 - V.42bis */
    /* 3 - V.44 */
    TAF_PDP_DCompUint8 pdpDcomp;

    /* 0 - off (default if value is omitted) */
    /* 1 - on (manufacturer preferred compression) */
    /* 2 - RFC1144 (applicable for SNDCP only) */
    /* 3 - RFC2507 */
    /* 4 - RFC3095 (applicable for PDCP only) */
    TAF_PDP_HCompUint8 pdpHcomp;

    /* 0 - IPv4 Address Allocation through NAS Signaling (default if omitted) */
    /* 1 - IPv4 Address Allocated through DHCP */
    TAF_PDP_Ipv4AddrAllocTypeUint8 ipv4AddrAlloc;

    /* 0 - PDP context is not for emergency bearer services */
    /* 1 - PDP context is for emergency bearer services */
    TAF_PDP_EmcIndUint8 emergencyInd;

    /* 0 - Preference of P-CSCF address discovery not influenced by +CGDCONT */
    /* 1 - Preference of P-CSCF address discovery through NAS Signalling */
    /* 2 - Preference of P-CSCF address discovery through DHCP */
    TAF_PDP_PcscfDiscoveryUint8 pcscfDiscovery;

    /* 0 - UE indicates that the PDP context is not for IM CN subsystem-related signalling only */
    /* 1 - UE indicates that the PDP context is for IM CN subsystem-related signalling only */
    TAF_PDP_ImCnSigFlagUint8 imCnSignalFlg;

    VOS_UINT8 bitRatType;
    VOS_UINT8 rmnetId;

    VOS_UINT8 isTakeOverFallback; /* DSM CONN是否接管回退流程 */
    VOS_UINT8 isTakeOverEpdgCall; /* DSM CONN是否接管EPDG呼叫流程 */
    VOS_UINT8 reserved[2];
} TAF_PS_DialPara;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_DialPara dialParaInfo;
} TAF_PS_CallOrigReq;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8          cid;
    VOS_UINT8          reserved[3];
    TAF_PS_CauseUint32 cause;
} TAF_PS_CallOrigCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8                cid;
    TAF_PS_CallEndCauseUint8 cause;
    VOS_UINT8                reserved[2];
} TAF_PS_CallEndReq;

/*
 * 结构说明: ID_MSG_TAF_PS_CALL_END_CNF消息结构
 *           断开呼叫正常 - enCause为TAF_PS_CAUSE_SUCCESS
 *           断开呼叫异常 - enCause为TAF_PS_CAUSE_CID_INVALID
 *                          指定的CID没有对应的呼叫实体激活
 */
typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8          cid;
    VOS_UINT8          reserved[3];
    TAF_PS_CauseUint32 cause;

} TAF_PS_CallEndCnf;

/*
 * 描述 : 针对命令 ^AUTHDATA
 * ID   : ID_MSG_TAF_PS_SET_AUTHDATA_INFO_REQ
 *      : ID_MSG_TAF_PS_SET_AUTHDATA_INFO_CNF
 *      : ID_MSG_TAF_PS_GET_AUTHDATA_INFO_REQ
 *      : ID_MSG_TAF_PS_GET_AUTHDATA_INFO_CNF
 * REQ  : TAF_PS_SetAuthdataInfoReq; TAF_PS_GetAuthdataInfoReq
 * CNF  : TAF_PS_SetAuthDataInfoCnf; TAF_PS_GetAuthdataInfoCnf
 * 说明 : ...
 */

typedef struct {
    VOS_UINT32 opAuthType : 1;
    VOS_UINT32 opPlmn : 1;
    VOS_UINT32 opPassWord : 1;
    VOS_UINT32 opUserName : 1;
    VOS_UINT32 opSpare : 28;

    VOS_UINT8             defined; /* 0:undefined, 1:defined */
    VOS_UINT8             cid;
    TAF_PDP_AuthTypeUint8 authType;
    VOS_UINT8             reserved1[1];
    VOS_UINT8             plmn[TAF_MAX_AUTHDATA_PLMN_LEN + 1];
    VOS_UINT8             reserved2[1];
    VOS_UINT8             passWord[TAF_MAX_AUTHDATA_USERNAME_LEN + 1];
    VOS_UINT8             userName[TAF_MAX_AUTHDATA_PASSWORD_LEN + 1];
} TAF_AUTHDATA_Ext;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_AUTHDATA_Ext authDataInfo;
} TAF_PS_SetAuthdataInfoReq;


typedef TAF_PS_CommonCnf TAF_PS_SetAuthDataInfoCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetAuthdataInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32     cause;
    VOS_UINT32             cidNum;
    TAF_AUTHDATA_QueryInfo authDataQueryInfo[0]; //lint !e43
} TAF_PS_GetAuthdataInfoCnf;

/*
 * 描述 : 针对命令 D
 * ID   : ID_MSG_TAF_PS_GET_D_GPRS_ACTIVE_TYPE_REQ
 *      : ID_EVT_TAF_PS_GET_D_GPRS_ACTIVE_TYPE_CNF
 *      :
 *      :
 * REQ  : TAF_PS_GetDGprsActiveTypeReq
 * CNF  : TAF_PS_GetDGprsActiveTypeCnf
 * 说明 : ...
 */


typedef struct {
    VOS_UINT32 opL2p : 1;
    VOS_UINT32 opSpare : 31;

    VOS_UINT8         cid;
    TAF_L2P_TypeUint8 l2p;
    VOS_UINT8         reserved[2];
} TAF_ATD_Para;


typedef struct {
    VOS_UINT8                cid;
    TAF_GPRS_ActiveTypeUint8 activeType;
    VOS_UINT8                reserved[2];
} TAF_CID_GprsActiveType;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_ATD_Para atdPara;
} TAF_PS_GetDGprsActiveTypeReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32     cause;
    TAF_CID_GprsActiveType cidGprsActiveType;
} TAF_PS_GetDGprsActiveTypeCnf;

/*
 * 结构说明:
 */
typedef struct {
    /* request长度: 24.008要求在[3,253]字节 */
    VOS_UINT16 papReqLen;
    VOS_UINT8  reserve[2];                         /* 对齐保留 */
    VOS_UINT8  papReqBuf[TAF_PPP_PAP_REQ_MAX_LEN]; /* request */
} TAF_PPP_AuthPapContent;

/*
 * 结构说明:
 */
typedef struct {
    /* challenge长度: 24.008要求在[3,253]字节 */
    VOS_UINT16 chapChallengeLen;
    /* response长度: 24.008要求在[3,253]字节 */
    VOS_UINT16 chapResponseLen;
    VOS_UINT8  chapChallengeBuf[TAF_PPP_CHAP_CHALLENGE_MAX_LEN]; /* challenge,153B */
    VOS_UINT8  chapResponseBuf[TAF_PPP_CHAP_RESPONSE_MAX_LEN];   /* response,205B */
    VOS_UINT8  reserve[2];                                       /* 对齐保留 */
} TAF_PPP_AuthChapContent;

/*
 * 结构说明:
 */
typedef struct {
    TAF_PDP_AuthTypeUint8 authType;
    VOS_UINT8             reserve[3];

    union {
        TAF_PPP_AuthPapContent  papContent;
        TAF_PPP_AuthChapContent chapContent;
    } authContent;

} TAF_PPP_ReqAuthConfigInfo;

/*
 * 结构说明:
 */
typedef struct {
    VOS_UINT16 ipcpLen;                              /* Ipcp帧长度 */
    VOS_UINT8  reserve[2];                           /* 对齐保留 */
    VOS_UINT8  ipcp[TAF_PPP_IPCP_FRAME_BUF_MAX_LEN]; /* Ipcp帧 */
} TAF_PPP_ReqIpcpConfigInfo;

/*
 * 结构说明:
 */
typedef struct {
    TAF_PPP_ReqAuthConfigInfo auth;
    TAF_PPP_ReqIpcpConfigInfo ipcp;
} TAF_PPP_ReqConfigInfo;

/*
 * 描述 : 针对PPP拨号
 * ID   : ID_MSG_TAF_PS_PPP_DIAL_ORIG_REQ
 *      : ID_EVT_TAF_PS_PPP_DIAL_ORIG_CNF
 *      :
 *      :
 * REQ  : TAF_PS_PppDialOrigReq
 * CNF  : TAF_PS_PppDialOrigCnf
 * 说明 : ...
 */

typedef struct {
    VOS_UINT32 opPppConfigInfo : 1;
    VOS_UINT32 opSpare : 31;

    VOS_UINT8 cid;
    VOS_UINT8 rmnetId; /* 网卡ID */

    VOS_UINT8             reserved[2];
    TAF_PPP_ReqConfigInfo pppReqConfigInfo;
} TAF_PPP_DialPara;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PPP_DialPara pppDialParaInfo;
} TAF_PS_PppDialOrigReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
} TAF_PS_PppDialOrigCnf;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_Ipv6InfoResultUint8 ipv6Rst;
    VOS_UINT8                  cid;
    VOS_UINT8                  qfi;
    VOS_UINT8                  rabId;
    VOS_UINT8                  pduSessionId;
    VOS_UINT8                  reserved[3];

    TAF_PDP_Ipv6RaInfo ipv6RaInfo;

} TAF_PS_Ipv6InfoInd;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];
} TAF_PS_GetNegotiationDnsReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    TAF_DNS_QueryInfo  negotiationDns;
} TAF_PS_GetNegotiationDnsCnf;

#if (FEATURE_ON == FEATURE_LTE)


typedef struct {
    VOS_UINT8 sg;
    VOS_UINT8 ims;
    VOS_UINT8 csfb;
    VOS_UINT8 vcc;
    VOS_UINT8 voLga;
    VOS_UINT8 reserved[3];
} TAF_PH_Ltecs;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_LtecsReq;


typedef struct {
    TAF_Ctrl           ctrl;
    TAF_PS_CauseUint32 cause;
    TAF_PH_Ltecs       lteCs;
} TAF_PS_LtecsCnf;



typedef struct {
    VOS_UINT32 currentUeMode;      /* 当前UE模式 */
    VOS_UINT32 supportModeCnt;     /* UE能够支持的模式的个数 */
    VOS_UINT32 supportModeList[4]; /* UE能够支持的模式 */
} TAF_PH_Cemode;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_CemodeReq;


typedef struct {
    TAF_Ctrl           ctrl;
    TAF_PS_CauseUint32 cause;
    TAF_PH_Cemode      cemode;
} TAF_PS_CemodeCnf;


#endif

/*
 * 结构说明: 网关鉴权参数结构体
 */
typedef struct {
    TAF_PDP_AuthTypeUint8 authType;
    VOS_UINT8             accNumLen;
    VOS_UINT8             reserved[2];
    VOS_UINT8             auAccessNum[TAF_MAX_ACCESS_NUM_LEN];
    VOS_UINT8             userNameLen;
    VOS_UINT8             userName[TAF_MAX_AUTHDATA_USERNAME_LEN];
    VOS_UINT8             pwdLen;
    VOS_UINT8             pwd[TAF_MAX_AUTHDATA_PASSWORD_LEN];

} TAF_GW_Auth;

/*
 * 结构说明: SDF参数结构
 */
typedef struct {
    VOS_UINT32 opLinkdCid : 1;
    VOS_UINT32 opPdnType : 1;
    VOS_UINT32 opApn : 1;
    VOS_UINT32 opUmtsQos : 1;
    VOS_UINT32 opEpsQos : 1;
    VOS_UINT32 opGwAuthInfo : 1;
    VOS_UINT32 opEmergencyInd : 1;
    VOS_UINT32 opIpv4AddrAllocType : 1;
    VOS_UINT32 opPcscfDiscovery : 1;
    VOS_UINT32 opImCnSignalFlg : 1;
    VOS_UINT32 opImsSuppFlg : 1;
    VOS_UINT32 opRoamPdnTypeFlag : 1;
    VOS_UINT32 opSpare : 20;

    VOS_UINT8                      cid;
    VOS_UINT8                      linkdCid;
    TAF_PDP_TypeUint8              pdnType;
    TAF_PDP_TypeUint8              roamPdnType;
    TAF_PDP_Ipv4AddrAllocTypeUint8 ipv4AddrAllocType;

    TAF_PDP_EmcIndUint8         emergencyInd;
    TAF_PDP_PcscfDiscoveryUint8 pcscfDiscovery;
    TAF_PDP_ImCnSigFlagUint8    imCnSignalFlg;
    VOS_UINT8                   pfNum;
    VOS_UINT8              imsSuppFlg;
    VOS_UINT8              nasSigPrioInd;
    TAF_PS_BearerTypeUint8 bearType;

    TAF_UMTS_Qos umtsQosInfo;
    TAF_EPS_Qos  epsQosInfo;
    TAF_PDP_Apn  apnInfo;
    TAF_GW_Auth  gwAuthInfo;
    TAF_PDP_Pf   cntxtTftInfo[TAF_MAX_SDF_PF_NUM];

} TAF_SDF_Para;

/*
 * 结构说明: SDF配置参数查询结构体
 */
typedef struct {
    VOS_UINT32   sdfNum;
    TAF_SDF_Para sdfPara[TAF_MAX_CID_NV];

} TAF_SDF_ParaQueryInfo;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_SdfInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_SDF_ParaQueryInfo sdfInfo;
} TAF_PS_SdfInfoCnf;


typedef struct {
    VOS_UINT8 qosFlag; /* 0：Qos表无效，1：Qos表有效 */
    /* 0:定义了最小可接受Qos,1:没有最小可接受Qos */
    VOS_UINT8 minQosFlag;
    VOS_UINT8 reserved[2];

    TAF_UMTS_Qos qos;    /* Qos表 */
    TAF_UMTS_Qos minQos; /* MINQos表 */
} TAF_PS_PdpQosQueryPara;


typedef struct {
    TAF_UMTS_QosExt qos;    /* Qos表 */
    TAF_UMTS_QosExt minQos; /* MINQos表 */
} TAF_PS_PdpQosSetPara;

#if (FEATURE_ON == FEATURE_IMS)
/*
 * 结构说明: APS->IMSA通知SRVCC CANECL
 */
typedef struct {
    TAF_Ctrl                     ctrl;
    TAF_SRVCC_CancelNotifyUint32 notificationIndicator;
} TAF_SRVCC_CancelNotifyInd;
#endif

/* 删除TAF_PS_SET_1X_CQOS_PRI_REQ_STRU  、TAF_PS_SET_CQOS_PRI_CNF_STRU */


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 cid;
    VOS_UINT8 rabId;
    VOS_UINT8 reserved[2];
} TAF_PS_CallDormantInd;

/*
 * Description:
 */
typedef struct {
    MSG_Header msgHeader; /* _H2ASN_Skip */
    VOS_UINT8  pdpId;
    VOS_UINT8  reserved[3];
} TAF_PS_CallEnterDormantInd;

/*
 * Description:
 */
typedef struct {
    MSG_Header msgHeader; /* _H2ASN_Skip */
    VOS_UINT8  pdpId;
    VOS_UINT8  reserved[3];
} TAF_PS_HaiChangeInd;

/*
 * Description:
 */
typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CdataDialModeUint32 dialMode;
} TAF_PS_CdataDialModeReq;

typedef TAF_PS_CommonCnf TAF_PS_CdataDialModeCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8         cid;
    VOS_UINT8         newRabId;
    VOS_UINT8         oldRabId;
    TAF_PDP_TypeUint8 pdpType;

} TAF_PS_CallPdpRabidChangeInd;



typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_MipModeUint8 mipMode;
    VOS_UINT8           resv[3];
} TAF_PS_SetMipModeReq;


typedef struct {
    TAF_Ctrl           ctrl;
    TAF_PS_CauseUint32 cause;

} TAF_PS_SetMipModeCnf;


typedef struct {
    TAF_Ctrl ctrl;

} TAF_PS_GetMipModeReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_MipModeUint8 mipMode;
    VOS_UINT8           resv[3];

} TAF_PS_GetMipModeCnf;

/*
 * H2ASN顶级消息结构定义
 */
typedef struct {
    /* _H2ASN_MsgChoice_Export TAF_PS_MSG_ID_ENUM_UINT32 */
    TAF_PS_MsgIdUint32 msgId;
    VOS_UINT8          msgBlock[4];
    /*
     * _H2ASN_MsgChoice_When_Comment          TAF_PS_MsgIdUint32
     */
} TAF_PS_MsgReq;
/* _H2ASN_Length UINT32 */

typedef struct {
    VOS_MSG_HEADER
    TAF_PS_MsgReq msgReq;
} TafApsApi_Msg;

typedef struct {
    /* _H2ASN_MsgChoice_Export TAF_PS_EVT_ID_ENUM_UINT32 */
    TAF_PS_EvtIdUint32 evtId;
    VOS_UINT8          evtBlock[4];
    /*
     * _H2ASN_MsgChoice_When_Comment          TAF_PS_EvtIdUint32
     */
} TAF_PS_EvtCnf;
/* _H2ASN_Length UINT32 */

typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32    msgName;
    TAF_PS_EvtCnf evtCnf;
} TafApsApi_Evt;

/* 以下数据结构，新架构代码已经不使用，当前先保留，保证老代码还是OK的 */

typedef struct {
    VOS_UINT32 opPdpType : 1;  /* PdpType */
    VOS_UINT32 opApn : 1;      /* aucApn */
    VOS_UINT32 opUsername : 1; /* AuthUsername */
    VOS_UINT32 opPassword : 1; /* AuthPassword */
    VOS_UINT32 opAuthType : 1; /* AuthType */
    VOS_UINT32 opSpare : 27;

    VOS_UINT8 defined; /* 0:undefined, 1:defined */

    VOS_UINT8 cid;

    /* 1 IP Internet Protocol (IETF STD 5) */
    /* 2 IPV6 Internet Protocol, version 6 (IETF RFC 2460) */
    /* 3 IPV4V6 Virtual <PDP_type> introduced to handle dual IP stack UE capability. (See 3GPP TS 24.301 [83]) */
    TAF_PDP_TypeUint8 pdpType;

    VOS_UINT8 reserved1[1];

    /* Access Point Name */
    VOS_UINT8 apn[TAF_MAX_APN_LEN + 1];

    VOS_UINT8 passWord[TAF_MAX_AUTHDATA_PASSWORD_LEN + 1];
    VOS_UINT8 userName[TAF_MAX_AUTHDATA_USERNAME_LEN + 1];
    VOS_UINT8 authType;
    VOS_UINT8 reserved2[3];

} TAF_PDP_ProfileExt;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PDP_ProfileExt pdpProfInfo;
} TAF_PS_SetProfileInfoReq;


typedef struct {
    TAF_Ctrl                            ctrl;
    TAF_PS_Cdata1XQosNonAssuredPriUint8 pri;
    VOS_UINT8                           reserved[3];
} TAF_PS_Set1XCqosPriReq;

typedef TAF_PS_CommonCnf TAF_PS_SetPdpProfInfoCnf;


typedef struct {
    TAF_Ctrl   ctrl;
    VOS_UINT32 rslt;
} TAF_PS_SetCqosPriCnf;


typedef struct {
    TAF_Ctrl          ctrl;
    VOS_UINT8         cid;
    TAF_PDP_TypeUint8 pdpType;
    TAF_PS_PdnOperateTypeUint8 operateType;
    VOS_UINT8                  reserved[1];
    TAF_PS_CustomPcoInfo customPcoInfo;
} TAF_PS_ReportPcoInfoInd;


typedef struct {
    VOS_UINT8 roamingNwNotSupportDualStackFlg;
    VOS_UINT8 imsPdnSupportIpType;    /* 1:(IPV4 Only), 2:(IPV6 ONly) */
    VOS_UINT8 nonImsPdnSupportIpType; /* 1:(IPV4 Only), 2:(IPV6 ONly) */
    VOS_UINT8 resv;
} TAF_APS_RoamingNwNotSupportDualStackCfg;


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 dataSwitch;
    VOS_UINT8 reserved1;
    VOS_UINT8 reserved2;
    VOS_UINT8 reserved3;
} TAF_PS_SetDataSwitchReq;


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 dataRoamSwitch;
    VOS_UINT8 reserved1;
    VOS_UINT8 reserved2;
    VOS_UINT8 reserved3;
} TAF_PS_SetDataRoamSwitchReq;


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 dataSwitch;
    VOS_UINT8 reserved1;
    VOS_UINT8 reserved2;
    VOS_UINT8 reserved3;
} TAF_PS_GetDataSwitchCnf;


typedef struct {
    TAF_Ctrl  ctrl;
    VOS_UINT8 dataRoamSwitch;
    VOS_UINT8 reserved1;
    VOS_UINT8 reserved2;
    VOS_UINT8 reserved3;
} TAF_PS_GetDataRoamSwitchCnf;

typedef TAF_PS_CommonReq TAF_PS_GET_DATA_SWITCH_REQ_STRU;
typedef TAF_PS_CommonReq TAF_PS_GET_DATA_ROAM_SWITCH_REQ_STRU;
typedef TAF_PS_CommonCnf TAF_PS_SET_DATA_SWITCH_CNF_STRU;
typedef TAF_PS_CommonCnf TAF_PS_SET_DATA_ROAM_SWITCH_CNF_STRU;


typedef struct {
    VOS_UINT8  cid;
    VOS_UINT8  apnClassType;
    VOS_UINT8  reserved[2];
    VOS_UINT32 waitTime;
    VOS_UINT32 pdnMaxConnTime;
    VOS_UINT32 pdnMaxConnCount;
} TAF_PS_ApnThrotInfo;


typedef struct {
    TAF_Ctrl            ctrl;
    TAF_PS_ApnThrotInfo apnThrotInfo;
} TAF_PS_SetApnThrotInfoReq;


typedef TAF_PS_CommonCnf TAF_PS_SetApnThrotInfoCnf;


typedef struct {
    TAF_Ctrl ctrl;
} TAF_PS_GetLteAttachInfoReq;


typedef struct {
    /* IPV4:1, IPV6:2, IPV4V6:3, PPP:4 */
    TAF_PDP_TypeUint8 pdpType;     /* IP TYPE */
    VOS_UINT8         reserved[3]; /* 保留 */
    TAF_PDP_Apn       apn;         /* APN 信息 */
} TAF_LTE_AttachQueryInfo;


typedef struct {
    TAF_Ctrl                ctrl;
    TAF_PS_CauseUint32      cause;           /* 错误码存储 */
    TAF_LTE_AttachQueryInfo lteAttQueryInfo; /* LTEAttEsm查询信息 */
} TAF_PS_GetLteAttachInfoCnf;


typedef struct {
    /* [1 - 4]value range for guranteed bit rate Qos Flows */
    /* 65,66,75  value for guranteed bit rate Qos Flows */
    /* [5 - 9]value range for non-guarenteed bit rate Qos Flows */
    /* 69,70,79,80 value for non-guarenteed bit rate Qos Flows */
    VOS_UINT8 uc5Qi;

    VOS_UINT8 reserved[3];

    /* DL GFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 dlgfbr;

    /* UL GFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 ulgfbr;

    /* DL MFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 dlmfbr;

    /* UL MFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 ulmfbr;
} TAF_PS_5GQos;


typedef struct {
    VOS_UINT32 op5Qi : 1;
    VOS_UINT32 opDlgfbr : 1;
    VOS_UINT32 opUlgfbr : 1;
    VOS_UINT32 opDlmfbr : 1;
    VOS_UINT32 opUlmfbr : 1;
    VOS_UINT32 opSpare : 27;

    VOS_UINT8 cid;
    VOS_UINT8 defined;
    VOS_UINT8 rsv[1];

    /* [1 - 4]value range for guranteed bit rate Qos Flows */
    /* 65,66,75  value for guranteed bit rate Qos Flows */
    /* [5 - 9]value range for non-guarenteed bit rate Qos Flows */
    /* 69,70,79,80 value for non-guarenteed bit rate Qos Flows */
    VOS_UINT8 uc5Qi;

    /* DL GFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 dlgfbr;

    /* UL GFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 ulgfbr;

    /* DL MFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 dlmfbr;

    /* UL MFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 ulmfbr;
} TAF_5G_QosExt;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_5G_QosExt st5QosInfo;
} TAF_PS_Set5GQosInfoReq;

typedef TAF_PS_CommonCnf TAF_PS_Set5gQosInfoCnf;


typedef TAF_PS_CommonReq TAF_PS_Get5gQosInfoReq;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32 cause;
    VOS_UINT32         cidNum;
    TAF_5G_QosExt      ast5gQosInfo[0]; //lint !e43
} TAF_PS_Get5GQosInfoCnf;


typedef struct {
    TAF_Ctrl ctrl;

    /* 0xff-if the parameter <cid> is omitted */
    VOS_UINT8 cid;
    VOS_UINT8 reserved[3];
} TAF_PS_GetDynamic5GQosInfoReq;


typedef struct {
    VOS_UINT32 opDlgfbr : 1;
    VOS_UINT32 opUlgfbr : 1;
    VOS_UINT32 opDlmfbr : 1;
    VOS_UINT32 opUlmfbr : 1;
    VOS_UINT32 opAveragWindow : 1;
    VOS_UINT32 opSambr : 1;
    VOS_UINT32 opSpare : 26;

    VOS_UINT8 cid;

    /* [1 - 4]value range for guranteed bit rate Qos Flows */
    /* 65,66,75  value for guranteed bit rate Qos Flows */
    /* [5 - 9]value range for non-guarenteed bit rate Qos Flows */
    /* 69,70,79,80 value for non-guarenteed bit rate Qos Flows */
    VOS_UINT8 uc5Qi;

    VOS_UINT16 averagWindow; /* Averaging Window */

    /* DL GFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 dlgfbr;

    /* UL GFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 ulgfbr;

    /* DL MFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 dlmfbr;

    /* UL MFBR in case of GBR 5QI, The value is in kbit/s */
    VOS_UINT32 ulmfbr;

    PS_PDU_SessionAmbr ambr; /* Session AMBR */
} TAF_PS_5GDynamicQosExt;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_CauseUint32     cause;
    VOS_UINT32             cidNum;
    TAF_PS_5GDynamicQosExt ast5gQosInfo[0]; //lint !e43
} TAF_PS_GetDynamic5GQosInfoCnf;


typedef struct {
    VOS_UINT8                 pti;
    TAF_PS_PolicyMsgTypeUint8 uePolicyMsgType;
    VOS_UINT8                 ueOsIdLen;
    VOS_UINT8                 classMarkLen;
    VOS_UINT8                 classMark[TAF_PS_UE_POLICY_CLASSMARK_MAX_LEN];
    VOS_UINT8                 ueOsIdInfo[TAF_PS_UE_OS_ID_INFO_MAX_LEN];
    VOS_UINT32                infoLen;
    VOS_UINT8                 content[0]; //lint !e43
} TAF_PS_5GUePolicyInfo;


typedef struct {
    TAF_Ctrl              ctrl;
    TAF_PS_5GUePolicyInfo uePolicyInfo;
} TAF_PS_5GUePolicyRsp;


typedef WIFI_IMSA_ImsPdnActivateCnf TAF_PS_EpdgActCnfInfo;


typedef WIFI_IMSA_ImsPdnDeactivateCnf TAF_PS_EpdgDeactCnfInfo;


typedef WIFI_IMSA_ImsPdnDeactivateInd TAF_PS_EpdgDeactIndInfo;


typedef struct {
    VOS_UINT32 opActCnf : 1;
    VOS_UINT32 opDeActCnf : 1;
    VOS_UINT32 opDeActInd : 1;
    VOS_UINT32 opSpare : 29;

    union {
        TAF_PS_EpdgActCnfInfo   actCnfInfo;
        TAF_PS_EpdgDeactCnfInfo deActCnfInfo;
        TAF_PS_EpdgDeactIndInfo deActIndInfo;
    };
} TAF_PS_EpdgCtrl;


typedef struct {
    TAF_Ctrl        ctrl;
    TAF_PS_EpdgCtrl epdgCtrl;
} TAF_PS_EpdgCtrlNtf;


typedef IMSA_WIFI_ImsPdnActivateReq TAF_PS_EpdgActReqInfo;


typedef IMSA_WIFI_ImsPdnDeactivateReq TAF_PS_EpdgDeactReqInfo;


typedef struct {
    VOS_UINT32 opActReq : 1;
    VOS_UINT32 opDeActReq : 1;
    VOS_UINT32 opSpare : 30;

    union {
        TAF_PS_EpdgActReqInfo   actReqInfo;
        TAF_PS_EpdgDeactReqInfo deActReqInfo;
    };
} TAF_PS_EpdgCtrlu;


typedef struct {
    TAF_Ctrl         ctrl;
    TAF_PS_EpdgCtrlu epdgCtrlu;
} TAF_PS_EpdgCtrluNtf;


typedef struct {
    TAF_PDP_TypeUint8     pdpType;
    VOS_UINT8             ipv4RabId;
    VOS_UINT8             ipv4PduSessionId;
    VOS_UINT8             ipv6RabId;
    VOS_UINT8             ipv6PduSessionId;
    VOS_UINT8             reserved[1];
    VOS_UINT16            ipv4Mtu;
    VOS_UINT8             ipv4Addr[TAF_IPV4_ADDR_LEN];
    VOS_UINT8             ipv6Addr[TAF_IPV6_ADDR_LEN];
    TAF_PDP_Dns           ipv4Dns;
    TAF_PDP_Ipv6Dns       ipv6Dns;
    TAF_PDP_Ipv4PcscfList ipv4PcscfList;
    TAF_PDP_Ipv6PcscfList ipv6PcscfList;
} TAF_PS_CallHandoverPdpInfo;


typedef struct {
    TAF_Ctrl                   ctrl;
    VOS_UINT8                  cid;
    TAF_PS_HandoverResultUint8 result;
    TAF_PS_DomainTypeUint8     currentDomain;
    VOS_UINT8                  reserved[1];
    TAF_PS_CallHandoverPdpInfo pdpInfo;
} TAF_PS_CallHandoverResultNtf;


typedef struct {
    TAF_Ctrl               ctrl;
    VOS_UINT8              pduSessionId;
    TAF_PS_DomainTypeUint8 currentDomain;
    VOS_UINT8              reserved[2];
    TAF_PS_RatTypeUint32   cnRatType; /* 核心网制式LTE/NR/G/W/1X/HRPD */
} TAF_PS_CnModeChangeInd;


typedef struct {
    VOS_UINT8         defined; /* 0:undefined, 1:defined */
    VOS_UINT8         cid;
    TAF_PDP_TypeUint8 roamingPdpType;
    VOS_UINT8         rsv[5];
} TAF_PS_RoamingPdpTypeInfo;


typedef struct {
    TAF_Ctrl ctrl;

    TAF_PS_RoamingPdpTypeInfo pdpTypeInfo;
} TAF_PS_SetRoamingPdpTypeReq;


typedef TAF_PS_CommonCnf TAF_PS_SetRoamingPdpTypeCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8 enable;
    VOS_UINT8 reserved[3];
} TAF_PS_SetUePolicyRptReq;

typedef TAF_PS_CommonCnf TAF_PS_SetUePolicyRptCnf;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT16                    totalLength;
    VOS_UINT16                    sectionNum;
    PS_URSP_ProtocalVersionUint32 protocalVer;
} TAF_PS_UePolicyRptInd;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT8 index;
    VOS_UINT8 reserved[3];
} TAF_PS_GetUePolicyReq;


typedef struct {
    TAF_Ctrl           ctrl;
    TAF_PS_CauseUint32 cause;
    VOS_UINT8          index;
    VOS_UINT8          reserved;
    VOS_UINT16         length;
    VOS_UINT8          content[0]; //lint !e43
} TAF_PS_GetUePolicyCnf;


typedef struct {
    TAF_Ctrl                         ctrl;
    TAF_PS_UePolicyRspCheckRsltUint8 rslt;
    VOS_UINT8                        reserved[3];
} TAF_PS_UePolicyRspCheckRsltInd;


typedef struct {
    TAF_Ctrl ctrl;

    VOS_UINT32 opIpv4Mtu : 1;
    VOS_UINT32 opSpare : 31;

    VOS_UINT8                    cid;
    PS_IFACE_IdUint8             ifaceId;
    TAF_PS_IpChangeOperTypeUint8 operateType;
    TAF_PS_DomainTypeUint8       domainType;
    VOS_UINT8                    ipv4RabId;
    VOS_UINT8                    ipv6RabId;
    VOS_UINT8                    ipv4PduSessionId;
    VOS_UINT8                    ipv6PduSessionId;
    VOS_UINT16                   ipv4Mtu;
    VOS_UINT8                    reserved[2];

    TAF_PDP_Addr          pdpAddr;
    TAF_PDP_Dns           ipv4Dns;
    TAF_PDP_Ipv6Dns       ipv6Dns;
    TAF_PDP_Ipv4PcscfList ipv4PcscfList;
    TAF_PDP_Ipv6PcscfList ipv6PcscfList;
    TAF_PDP_Ipv6RaInfo    ipv6RaInfo;
} TAF_PS_IpChangeInd;

/*
 * 功能描述: 设置Primary PDP上下文信息
 */
VOS_UINT32 TAF_PS_SetPrimPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                        TAF_PDP_PrimContextExt *pdpContextInfo);

/*
 * 功能描述: 获取Primary PDP上下文信息
 */
VOS_UINT32 TAF_PS_GetPrimPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 获取PDP上下文信息
 */
VOS_UINT32 TAF_PS_GetPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);
/*
 * 功能描述: 设置Secondary PDP上下文信息
 */
VOS_UINT32 TAF_PS_SetSecPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                       TAF_PDP_SecContextExt *pdpContextInfo);

/*
 * 功能描述: 获取Secondary PDP上下文信息
 */
VOS_UINT32 TAF_PS_GetSecPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 设置PDP TFT参数
 */
VOS_UINT32 TAF_PS_SetTftInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_TFT_Ext *tftInfo);

/*
 * 功能描述: 获取PDP TFT参数
 */
VOS_UINT32 TAF_PS_GetTftInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 设置UMTS QOS参数信息
 */
VOS_UINT32 TAF_PS_SetUmtsQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                 TAF_UMTS_QosExt *umtsQosInfo);

/*
 * 功能描述: 获取UMTS QOS参数信息
 */
VOS_UINT32 TAF_PS_GetUmtsQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 设置UMTS MIN QOS参数信息
 */
VOS_UINT32 TAF_PS_SetUmtsQosMinInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                    TAF_UMTS_QosExt *umtsQosMinInfo);

/*
 * 功能描述: 获取UMTS MIN QOS参数信息
 */
VOS_UINT32 TAF_PS_GetUmtsQosMinInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 获取动态UMTS QOS参数信息
 */
VOS_UINT32 TAF_PS_GetDynamicUmtsQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                        TAF_CID_List *cidListInfo);

/*
 * 功能描述: 设置指定CID(表)对应的PDP的状态
 */
VOS_UINT32 TAF_PS_SetPdpContextState(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                     TAF_CID_ListState *cidListStateInfo);

/*
 * 功能描述: 获取所有已定义CID对应的PDP的状态
 */
VOS_UINT32 TAF_PS_GetPdpContextState(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 修改PS CALL参数
 */
VOS_UINT32 TAF_PS_CallModify(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_CID_List *cidListInfo);

/*
 * 功能描述: 应答PS CALL
 */
VOS_UINT32 TAF_PS_CallAnswer(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_PS_Answer *ansInfo);

/*
 * 功能描述: 挂断PS CALL
 */
VOS_UINT32 TAF_PS_CallHangup(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 发起PS CALL
 */
VOS_UINT32 TAF_PS_CallOrig(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_PS_DialPara *dialParaInfo);

/*
 * 功能描述: 结束PS CALL
 */
VOS_UINT32 TAF_PS_CallEnd(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid);


VOS_UINT32 TAF_PS_CallEndEx(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid,
                            TAF_PS_CallEndCauseUint8 cause);

/*
 * 功能描述: 获取指定CID(表)的PDP IP地址信息
 */
VOS_UINT32 TAF_PS_GetPdpIpAddrInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                   TAF_CID_List *cidListInfo);

/*
 * 功能描述: 设置PS域呼叫应答模式信息
 */
VOS_UINT32 TAF_PS_SetAnsModeInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT32 ansMode);

/*
 * 功能描述: 获取PS域呼叫应答模式信息
 */
VOS_UINT32 TAF_PS_GetAnsModeInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 获取指定的已定义CID的动态Primary PDP上下文信息
 */
VOS_UINT32 TAF_PS_GetDynamicPrimPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                               VOS_UINT8 cid);

/*
 * 功能描述: 获取指定的已定义CID的动态Secondary PDP上下文信息
 */
VOS_UINT32 TAF_PS_GetDynamicSecPdpContextInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                              VOS_UINT8 cid);

/*
 * 功能描述: 获取指定的已定义CID的动态TFT信息
 */
VOS_UINT32 TAF_PS_GetDynamicTftInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid);

/*
 * 功能描述: 设置指定CID的EPS QOS参数信息
 */
VOS_UINT32 TAF_PS_SetEpsQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_EPS_QosExt *epsQosInfo);

/*
 * 功能描述: 获取EPS QOS参数信息
 */
VOS_UINT32 TAF_PS_GetEpsQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 获取指定的已定义CID的动态EPS QOS参数
 */
VOS_UINT32 TAF_PS_GetDynamicEpsQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid);

/*
 * 功能描述: 获取数据流量信息
 */
VOS_UINT32 TAF_PS_GetDsFlowInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 清除数据流量信息
 */
VOS_UINT32 TAF_PS_ClearDsFlowInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                  TAF_DSFLOW_ClearConfig *clearConfigInfo);

/*
 * 功能描述: 配置流量上报模式
 */
VOS_UINT32 TAF_PS_ConfigDsFlowRpt(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                  TAF_DSFLOW_ReportConfig *reportConfigInfo);

/*
 * 功能描述: 配置VT流量上报模式
 */
VOS_UINT32 TAF_PS_ConfigVTFlowRpt(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                  TAF_VTFLOW_ReportConfig *reportConfigInfo);

/*
 * 功能描述: 设置PDP DNS信息
 */
VOS_UINT32 TAF_PS_SetPdpDnsInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_PDP_DnsExt *pdpDnsInfo);

/*
 * 功能描述: 获取PDP DNS信息
 */
VOS_UINT32 TAF_PS_GetPdpDnsInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 发送上行GPRS数据
 */
VOS_UINT32 TAF_PS_TrigGprsData(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_GPRS_Data *gprsDataInfo);

/*
 * 功能描述: 配置NBNS功能
 */
VOS_UINT32 TAF_PS_ConfigNbnsFunction(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT32 enabled);

/*
 * 功能描述: 获取NDIS状态信息
 */
VOS_UINT32 TAF_PS_GetNdisStateInfo(VOS_UINT32 moduleId, VOS_UINT16 clientId, VOS_UINT8 opId);

/*
 * 功能描述: 设置鉴权参数信息(NDIS)
 */
VOS_UINT32 TAF_PS_SetAuthDataInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                  TAF_AUTHDATA_Ext *authDataInfo);

/*
 * 功能描述: 获取鉴权参数信息(NDIS)
 */
VOS_UINT32 TAF_PS_GetAuthDataInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 获取D命令GPRS类型(PPP拨号)
 */
VOS_UINT32 TAF_PS_GetGprsActiveType(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_ATD_Para *atdPara);

/*
 * 功能描述: 发起PPP拨号
 */
VOS_UINT32 TAF_PS_PppDialOrig(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid,
                              TAF_PPP_ReqConfigInfo *pppReqConfigInfo);

#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT32 TAF_PS_GetLteCsInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_GetCemodeInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_SetPdpProfInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                 TAF_PDP_ProfileExt *pdpProfInfo);
#endif

/*
 * 功能描述: 获取所有NV项中的SDF配置信息, 支持同步和异步两种方式
 *           输出参数非空 --- 同步方式(目前仅支持C核)
 *           输出参数为空 --- 异步方式
 */
VOS_UINT32 TAF_PS_GetCidSdfParaInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                    TAF_SDF_ParaQueryInfo *sdfQueryInfo);

/*
 * 功能描述: 获取空闲未激活的CID
 *           输出参数非空 --- 同步方式(目前仅支持C核)
 *           输出参数为空 --- 异步方式
 */
VOS_UINT32 TAF_PS_GetUnusedCid(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 *puCid);

/*
 * 功能描述: 获取指定CID的DNS信息
 */
VOS_UINT32 TAF_PS_GetDynamicDnsInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid);

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
VOS_UINT32 TAF_PS_SetCqosPriInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, TAF_PS_Cdata1XQosNonAssuredPriUint8 pri);
#endif

/*
 * 功能描述: 设置AP流量上报参数
 */
VOS_UINT32 TAF_PS_SetApDsFlowRptCfg(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                    TAF_APDSFLOW_RptCfg *rptCfg);

/*
 * 功能描述: 获取AP流量上报参数
 */
VOS_UINT32 TAF_PS_GetApDsFlowRptCfg(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 设置流量写NV配置
 */
VOS_UINT32 TAF_PS_SetDsFlowNvWriteCfg(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                      TAF_DSFLOW_NvWriteCfg *nvWriteCfg);

/*
 * 功能描述: 获取流量写NV配置
 */
VOS_UINT32 TAF_PS_GetDsFlowNvWriteCfg(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_SetCdataDialModeInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, TAF_PS_CdataDialModeUint32 dialMode);

VOS_UINT32 TAF_PS_SetImsPdpCfg(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_IMS_PdpCfg *imsPdpCfg);

VOS_UINT32 TAF_PS_GetCtaInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_SetCtaInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 timeLen);

VOS_UINT32 TAF_PS_SetCdmaDormantTimer(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                      VOS_UINT8 dormantTimer);

VOS_UINT32 TAF_PS_ProcCdmaDormTimerQryReq(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);


TAF_PS_CdataBearStatusUint8 TAF_PS_GetCdataBearStatus(VOS_UINT8 pdpId);

TAF_PS_CdataBearStatusUint8 TAF_PS_GetPppStatus(VOS_VOID);

VOS_UINT32 TAF_PS_SetMipMode(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_PS_MipModeUint8 mipMode);

VOS_UINT32 TAF_PS_GetMipMode(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_GetModuleIdByCid(VOS_UINT8 cid, ModemIdUint16 modemId);

VOS_UINT32 TAF_PS_SetApnThrotInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                  TAF_PS_ApnThrotInfo *apnThrotInfo);

VOS_UINT8 TAF_PS_FindCidForDial(VOS_UINT32 appPid);

VOS_UINT32 TAF_PS_Set_DataSwitchStatus(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                       VOS_UINT8 dataSwitch);

VOS_UINT32 TAF_PS_Set_DataRoamSwitchStatus(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                           VOS_UINT8 dataRoamSwitch);

VOS_UINT32 TAF_PS_Get_DataSwitchStatus(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_Get_DataRoamSwitchStatus(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

/*
 * 功能描述: 获取LTE 注册承载的PDN上下文信息
 */
VOS_UINT32 TAF_PS_GetLteAttchInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_Set5QosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, TAF_5G_QosExt *pst5gQosInfo);
VOS_UINT32 TAF_PS_Get5gQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId);

VOS_UINT32 TAF_PS_GetDynamic5gQosInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 cid);

/*
 * 功能描述: 调用DSM IFACE接口返回WLAN控制结果信息
 */
VOS_UINT32 TAF_PS_EpdgCtrlMsg(const TAF_Ctrl *ctrl, const TAF_PS_EpdgCtrl *epdgCtrl);

VOS_UINT32 TAF_PS_SetRoamPdpType(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                 const TAF_PS_RoamingPdpTypeInfo *pdpTypeInfo);

VOS_UINT32 TAF_PS_SetUePolicyRpt(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 enable);

VOS_UINT32 TAF_PS_GetUePolicyInfo(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId, VOS_UINT8 indexNum);

VOS_UINT32 TAF_PS_SetCsUePolicy(VOS_UINT32 moduleId, VOS_UINT16 exClientId, VOS_UINT8 opId,
                                TAF_PS_5GUePolicyInfo *pst5gUePolicy);

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
