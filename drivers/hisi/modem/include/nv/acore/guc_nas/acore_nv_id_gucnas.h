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


#ifndef __ACORE_NV_ID_GUCNAS_H__
#define __ACORE_NV_ID_GUCNAS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/****************************************************************************
  1 其他头文件包含
*****************************************************************************/
/*****************************************************************************
 枚举名    : GUCNAS_ACORE_NV_ID_ENUM
 枚举说明  : GUCNAS组件ACORE的NV项ID枚举
*****************************************************************************/
enum GUCNAS_ACORE_NV_ID_ENUM
{
/*    36 */     NV_ITEM_SW_VERSION_FLAG                  = 36,       /* OM_SW_VerFlag */
/*    90 */     NV_ITEM_BATTERY_ADC                      = 90,       /* VBAT_CALIBART_Type */

/*  2102 */     NV_ITEM_CDMAMODEMSWITCH_NOT_RESET_CFG    = 2102,     /* TAF_NVIM_CdmamodemswitchNotResetCfg  用来配置CDMAMODEMSWITCH不重启特性是否开启，若不开启，设置后需RESET MODEM */

/*  2336 */     NV_ITEM_IPV6_ADDRESS_TEST_MODE_CFG       = 2336,     /* TAF_NVIM_Ipv6AddrTestModeCfg        IPV6地址测试模式配置，0x55AA55AA为测试模式，其他值为正常模式 */
/*  2337 */     NV_ITEM_ADS_IPF_MODE_CFG                 = 2337,     /* TAF_NV_AdsIpfModeCfg                 IPF处理ADS下行数据的模式配置 */

/*  2354 */     NV_ITEM_ADS_WAKE_LOCK_CFG                = 2354,     /* TAF_NV_AdsWakeLockCfg ADS WAKELOCK 配置 */

/*  2361 */     NV_ITEM_PRINT_MODEM_LOG_TYPE             = 2361,     /* TAF_NV_PrintModemLogType             控制是否输出modem log的类型 */

/*  2369 */     NV_ITEM_ADS_MEM_POOL_CFG                 = 2369,     /* TAF_NV_AdsMemPoolCfg */

/*  2391 */     NV_ITEM_USER_REBOOT_SUPPORT_FLG          = 2391,     /* TAF_NVIM_UserRebootSupport            用户是否可以发起整机复位 */

/*  2421 */     NV_ITEM_RNIC_NAPI_CFG                    = 2421,     /* TAF_NV_RnicNapiCfg                    RNIC NAPI GRO配置 */

/*  2437 */     NV_ITEM_AT_CLIENT_CFG                    = 2437,     /* TAF_AT_NvimAtClientCfg               双卡双通控制AT通道归属哪个Modem信息 */

/*  2461 */     NV_ITEM_ADS_PACKET_ERROR_FEEDBACK_CFG    = 2461,     /* TAF_NV_AdsErrorFeedbackCfg           下行错包反馈配置*/

/*  2478 */     NV_ITEM_IMS_SWITCH_RAT_RELATION_CFG      = 2478,     /* TAF_NV_ImsSwitchRatRelationCfg      ^IMSSWITCH命令2/3G开关与4G开关关联配置 */

/*  2550 */     NV_ITEM_RNIC_NAPI_LB_CFG                 = 2550,     /* TAF_NV_RnicNapiLbCfg                  RNIC NAPI Load Balance配置 */

/*  2559 */     NV_ITEM_RNIC_RHC_CFG                     = 2559,     /* TAF_NV_RnicRhcCfg                      下行接收高速控制配置 */

/*  2600 */     NV_ITEM_COMMDEGBUG_CFG                   = 2600,     /* TAF_AT_NvimCommdegbugCfg */
/*  2601 */     NV_ITEM_PID_ENABLE_TYPE                  = 2601,     /* NV_PID_EnableType */
/*  2602 */     NV_ITEM_AT_DISSD_PWD                     = 2602,     /* TAF_AT_NvimDissdPwd */
/*  2603 */     NV_ITEM_AT_DISLOG_PWD_NEW                = 2603,     /* TAF_AT_NvimDislogPwdNew */
/*  2604 */     NV_ITEM_DEFAULT_LINK_OF_UART             = 2604,     /* TAF_AT_NvimDefaultLinkOfUart */
/*  2605 */     NV_ITEM_AT_FACTORY_INFO                  = 2605,     /* TAF_AT_NvimFactoryInfo */
/*  2606 */     NV_ITEM_AT_MANUFACTURE_DATE              = 2606,     /* TAF_AT_NvimManufactureDate */
/*  2607 */     NV_ITEM_UART_CFG                         = 2607,     /* TAF_NV_UartCfg */
/*  2608 */     NV_ITEM_PORT_BUFF_CFG                    = 2608,     /* TAF_NV_PortBuffCfg */

/*  2627 */     NV_ITEM_RXDIV_CONFIG                     = 2627,     /* TAF_AT_NvimRxdivConfig */

/*  2635 */     NV_ITEM_NDIS_DHCP_DEF_LEASE_TIME         = 2635,     /* NDIS_NV_DhcpLeaseHour */
/*  2636 */     NV_ITEM_CREG_CGREG_CI_FOUR_BYTE_RPT      = 2636,     /* NAS_NV_CregCgregCiFourByteRpt */

/*  2640 */     NV_ITEM_ATSETZ_RET_VALUE                 = 2640,     /* TAF_AT_Tz */
/*  2641 */     NV_ITEM_NOT_SUPPORT_RET_VALUE            = 2641,     /* TAF_AT_NotSupportCmdErrorText */
/*  2642 */     NV_ITEM_AT_RIGHT_PASSWORD                = 2642,     /* TAF_AT_NvimRightOpenFlag */

/*  2649 */     NV_ITEM_IPV6_ROUTER_MTU                  = 2649,     /* TAF_NDIS_NvIpv6RouterMtu */
/*  2650 */     NV_ITEM_DIAL_CONNECT_DISPLAY_RATE        = 2650,     /* AT_NVIM_DialConnectDisplayRate */

/*  2655 */     NV_ITEM_ADS_QUEUE_SCHEDULER_PRI          = 2655,     /* ADS_UL_QueueSchedulerPriNv */
/*  2656 */     NV_ITEM_WIFI_INFO                        = 2656,     /* AT_WIFI_Info， XML文件中的结构体名为nv_wifi_Info */

/*  2675 */     NV_ITEM_MUX_SUPPORT_FLG                  = 2675,     /* TAF_AT_NvimMuxSupportFlg */

/*  2677 */     NV_ITEM_MUX_REPORT_CFG                   = 2677,     /* TAF_AT_NvimMuxReportCfg */

/*  2679 */     NV_ITEM_SS_CUSTOMIZE_PARA                = 2679,     /* AT_SS_CustomizePara */
/*  2680 */     NV_ITEM_SHARE_PDP_INFO                   = 2680,     /* TAF_NVIM_SharePdpInfo Share PDP特性控制NV */

/*  2696 */     NV_ITEM_ADS_DYNAMIC_THRESHOLD_CFG        = 2696,     /* ADS_NV_DynamicThreshold */

/*  2713 */     NV_ITEM_AP_RPT_SRV_URL                   = 2713,     /* TAF_AT_NvimApRptSrvUrl */
/*  2714 */     NV_ITEM_AP_XML_INFO_TYPE                 = 2714,     /* TAF_AT_NvimApXmlInfoType */
/*  2715 */     NV_ITEM_AP_XML_RPT_FLAG                  = 2715,     /* TAF_AT_NvimApXmlRptFlg */

/*  8200 */     NV_ITEM_GMM_INFO                          = 8200,     /* TAF_AT_NvGmmInfo */


/*  8340 */     NV_ITEM_REPORT_REG_ACT_FLG               = 8340,     /* TAF_AT_NvimReportRegActFlg */

/*  8514 */     NV_ITEM_IPV6_CAPABILITY                  = 8514,     /* AT_NV_Ipv6Capability */

/*  9041 */     NV_ITEM_SUB_PLATFORM_INFO                  = 9041,     /* NAS_NVIM_Platform */
/*  9070 */     NV_ITEM_EQVER                            = 9070,     /* TAF_AT_EqVer */
/*  9071 */     NV_ITEM_CSVER                            = 9071,     /* TAF_NVIM_CsVer */
/*  9110 */     NV_ITEM_MULTI_WIFI_KEY                   = 9110,     /* TAF_AT_MultiWifiSec 支持多组SSID扩展新增WIKEY NV项 */
/*  9111 */     NV_ITEM_MULTI_WIFI_STATUS_SSID           = 9111,     /* TAF_AT_MultiWifiSsid 支持多组SSID扩展新增SSID NV项 */
/*  9113 */     NV_ITEM_AT_ABORT_CMD_PARA                = 9113,     /* AT_NVIM_AbortCmdPara */
/*  9128 */     NV_ITEM_CIMI_PORT_CFG                    = 9128,     /* TAF_AT_NvimCimiPortCfg */
/*  9269 */     NV_ITEM_DMS_DEBUG_CFG                    = 9269,     /* TAF_NV_PortDebugCfg */

/* 32768 */     NV_ITEM_MANUFACTURE_INFO                  = 32768,    /* OM_ManufactureInfo */

/* 50014 */     NV_ITEM_WIFI_MAC_ADDR                    = 50014,    /* OM_MAC_Addr */
/* 50061 */     NV_ITEM_PPP_DIAL_ERR_CODE                = 50061,    /* NAS_NV_PppDialErrCode */
/* 50091 */     NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE          = 50091,    /* AT_DynamicPidType */
/* 52001 */     NV_ITEM_WEB_ADMIN_PASSWORD               = 52001,    /* TAF_AT_NvimWebAdminPassword */

/*  BUTT */     NV_ITEM_GUCNAS_ACORE_NV_BUTT,
};


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __ACORE_NV_ID_GUCNAS_H__ */

