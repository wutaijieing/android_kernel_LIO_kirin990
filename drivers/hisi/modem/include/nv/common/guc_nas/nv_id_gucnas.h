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



#ifndef __NV_ID_GUCNAS_H__
#define __NV_ID_GUCNAS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/****************************************************************************
  1 其他头文件包含
*****************************************************************************/
/*****************************************************************************
 枚举名    : GUCNAS_NV_ID_ENUM
 枚举说明  : GUCNAS组件公共的NV项ID枚举定义
*****************************************************************************/
enum GUCNAS_NV_ID_ENUM
{
/*     6 */     NV_ITEM_SERIAL_NUM                       = 6,        /* TAF_NVIM_SerialNum */
/*    21 */     NV_ITEM_USB_ENUM_STATUS                  = 21,       /* USB_ENUM_Status */
/*    60 */     NV_ITEM_SVLTE_FLAG                       = 60,       /* SVLTE_SupportFlag */
/*    70 */     NV_ITEM_LC_CTRL_PARA                     = 70,       /* TAF_NV_LC_CTRL_PARA_STRU */
/*   121 */     NV_ITEM_SYSTEM_APP_CONFIG                = 121,      /* NAS_NVIM_SystemAppConfig */

/*  2003 */     NV_ITEM_CFREQ_LOCK_CFG                    = 2003,     /* TAF_NVIM_CfreqLockCfg */
/*  2095 */     NV_ITEM_LTE_SMS_CFG                      = 2095,     /* TAF_NVIM_LteSmsCfg */
/*  2300 */     NV_ITEM_DSDS_ACTIVE_MODEM_MODE           = 2300,     /* TAF_NV_DsdsActiveModemMode */
/*  2316 */     NV_ITEM_ENHANCED_OPERATOR_NAME_SRV_CFG   = 2316,     /* TAF_NVIM_EnhancedOperNameServiceCfg */
/*  2321 */     NV_ITEM_NVWR_SEC_CTRL                    = 2321,     /* TAF_NV_NvwrSecCtrl    NVWR命令安全控制 */
/*  2331 */     NV_ITEM_CCPU_RESET_RECORD                = 2331,     /* TAF_NVIM_CcpuResetRecord  C核单独复位记录 */
/*  2340 */     NV_ITEM_CCWA_CTRL_MODE                   = 2340,     /* TAF_CALL_NvimCcwaCtrlMode */
/*  2352 */     NV_ITEM_PRIVACY_LOG_FILTER_CFG           = 2352,     /* NAS_NV_PrivacyFilterCfg */
/*  2371 */     NV_ITEM_XCPOSRRPT_CFG                    = 2371,     /* TAF_NVIM_XcposrrptCfg 是否上报清除GPS缓存的辅助定位信息 */
/*  2387 */     NV_ITEM_USER_SYS_CFG_RAT_INFO            = 2387,     /* TAF_NVIM_MultimodeRatCfg 用户SYS CFG RAT配置 */
/*  2396 */     NV_ITEM_GPS_CUST_CFG                     = 2396,     /* TAF_NVIM_GpsCustCfg 查询GPS芯片类型 */

/* 2512 */      NV_ITEM_RAT_PRIO_LIST_EX                 = 2512,     /* TAF_NVIM_RatOrderEx */

/*  2525 */     NV_ITEM_CUSTOM_CALL_CFG                  = 2525,     /* NAS_NVIM_CustomCallCfg */

/*  2526 */     NV_ITEM_CUSTOM_USIMM_CFG                 = 2526,     /* NAS_NVIM_CustomUsimmCfg */

/*  2553 */     NV_ITEM_CUSTOM_ECALL_CFG                 = 2553,     /* TAF_NVIM_CustomEcallCfg */

/*  2557 */     NV_ITEM_APN_CUSTOM_FORMAT_CFG            = 2557,     /* TAF_NVIM_ApnCustomFormatCfg */

/*  2611 */     NV_ITEM_PRODUCT_NAME                      = 2611,     /* TAF_PH_ProductName */
/*  2615 */     NV_ITEM_MMA_ACCESS_MODE                   = 2615,     /* NAS_MMA_NvimAccessMode */
/*  2626 */     NV_ITEM_WINS_CONFIG                      = 2626,     /* WINS_Config */
/*  2630 */     NV_ITEM_SMS_ACTIVE_MESSAGE_PARA           = 2630,     /* TAF_AT_NvimSmsActiveMessage */
/*  2644 */     NV_ITEM_PREVENT_TEST_IMSI_REG            = 2644,     /* NAS_PREVENT_TestImsiReg */

/*  2695 */     NV_ITEM_JAM_DETECT_CFG                   = 2695,     /* NV_NAS_JamDetectCfg */
/*  2698 */     NV_ITEM_FREQ_LOCK_CFG                    = 2698,     /* TAF_NVIM_FreqLockCfg */
/*  2705 */     NV_ITEM_PRODUCT_ID                       = 2705,     /* TAF_AT_ProductId */
/*  2707 */     NV_ITEM_CUST_USSD_MODE                   = 2707,     /* TAF_UssdNvimTransMode */

/*  6656 */     NV_ITEM_FOLLOW_ON_OPEN_SPEED_FLAG          = 6656,     /* NAS_NVIM_FollowonOpenspeedFlag */

/*  8203 */     NV_ITEM_FMRID                            = 8203,     /* TAF_PH_FmrId */
/*  8230 */     NV_ITEM_CCA_TELE_PARA                     = 8230,     /* TAF_CCA_TelePara */
/*  8233 */     NV_ITEM_MMA_MS_CLASS                      = 8233,     /* NAS_NVIM_MsClass */
/*  8237 */     NV_ITEM_SMS_SERVICE_PARA                 = 8237,     /* TAF_NVIM_SmsServicePara */
/*  8266 */     NV_ITEM_ROAM_CAPA                        = 8266,     /* NAS_NVIM_RoamCfgInfo */
/*  8267 */     NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO         = 8267,     /* TAF_CUSTOM_SimLockPlmnInfo */
/*  8268 */     NV_ITEM_CARDLOCK_STATUS                   = 8268,     /* TAF_CUSTOM_CARDLOCK_STATUS_STRU */
/*  8269 */     NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES         = 8269,     /* TAF_CUSTOM_SimLockMaxTimes */
/*  8271 */     NV_ITEM_CUSTOMIZE_SERVICE                 = 8271,     /* NAS_NVIM_CustomizeService */
/*  8337 */     NV_ITEM_IMEI_SVN                         = 8337,     /* TAF_SVN_Data */
/*  8471 */     NV_ITEM_PCVOICE_SUPPORT_FLG              = 8471,     /* APP_VC_NvimPcVoiceSupportFalg */
/*  8501 */     NV_ITEM_SMS_ME_STORAGE_INFO              = 8501,     /* MN_MSG_MeStorageParm */
/*  9007 */     NV_ITEM_RAT_PRIO_LIST                    = 9007,     /* TAF_PH_NvimRatOrder */
/*  9052 */     NV_ITEM_LTE_INTERNATION_ROAM_CONFIG      = 9052,     /* NAS_MMC_NvimLteInternationalRoamCfg */
/*  9118 */     NV_ITEM_SMS_MT_CUSTOMIZE_INFO            = 9118,     /* MN_MSG_MtCustomizeInfo */
/*  9123 */     NV_ITEM_CELL_SIGN_REPORT_CFG             = 9123,     /* NAS_NVIM_CellSignReportCfg */
/*  9130 */     NV_ITEM_IPV6_BACKPROC_EXT_CAUSE          = 9130,     /* TAF_NV_Ipv6FallbackExtCause */
/*  9136 */     NV_ITEM_SMS_CLOSE_PATH                   = 9136,     /* TAF_NVIM_SmsClosePathCfg */
/*  9203 */     NV_ITEM_PLATFORM_RAT_CAP                 = 9203,     /* PLATAFORM_RatCapability */
/*  9206 */     NV_ITEM_BODY_SAR_PARA                    = 9208,     /* MTA_BodySarPara */
/*  9212 */     NV_ITEM_TEMP_PROTECT_CONFIG              = 9212,     /* TAF_TempProtectConfig */
/*  9237 */     NV_ITEM_SMS_DOMAIN                       = 9237,     /* TAF_NVIM_SmsDomain */
/* 50009 */     NV_ITEM_TRAFFIC_CLASS_TYPE               = 50009,    /* AT_TrafficClassCustomize */
/* 50056 */     NV_ITEM_SMS_CLASS0_TAILOR                = 50056,    /* MN_MSG_NvimClass0Tailor */

/*  BUTT */     NV_ITEM_GUCNAS_NV_BUTT,
};


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __NV_ID_GUCNAS_H__ */

