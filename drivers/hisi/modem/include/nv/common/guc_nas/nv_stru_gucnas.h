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

#ifndef __NV_STRU_GUCNAS_H__
#define __NV_STRU_GUCNAS_H__

#include "vos.h"
#include "nv_id_gucnas.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push, 4)

#define TAF_NVIM_SN_LEN 20
#define TAF_NVIM_MAX_OPER_NAME_SERVICE_PRIO_NUM 4
#define TAF_NV_BLACK_LIST_MAX_NUM 51
#define TAF_NVIM_MAX_USER_SYS_CFG_RAT_NUM 7
#define TAF_MAX_MFR_ID_LEN 31
#define TAF_MAX_MFR_ID_STR_LEN (TAF_MAX_MFR_ID_LEN + 1)
#define TAF_PH_PRODUCT_NAME_LEN 15
#define TAF_PH_PRODUCT_NAME_STR_LEN (TAF_PH_PRODUCT_NAME_LEN + 1)
#define MN_MSG_SRV_PARAM_LEN 8 /* 短信业务参数结构与NV项中存储的长度 */
#define TAF_NVIM_MSG_ACTIVE_MESSAGE_MAX_URL_LEN 160
#define TAF_SVN_DATA_LENGTH 2                     /* SVN有效数据长度 */
#define TAF_PH_NVIM_MAX_GUL_RAT_NUM 3             /* AT^syscfgex中acqorder代表的接入技术个数 */
#define TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM 20 /* 9130扩展IPv6回退处理扩展原因值最大个数 */
#define PLATFORM_MAX_RAT_NUM (7U)                   /* 接入技术最大值 */
#define MTA_BODY_SAR_WBAND_MAX_NUM 5
#define MTA_BODY_SAR_GBAND_MAX_NUM 4
#define TAF_PH_SIMLOCK_PLMN_STR_LEN 8 /* Plmn 号段长度 */
#define TAF_MAX_SIM_LOCK_RANGE_NUM 20

#define TAF_NVIM_MAX_RAT_ORDER_NUM 8

/* WINS可配置NV项的结构体 */
#define WINS_CONFIG_DISABLE 0 /* WINS不使能 */
#define WINS_CONFIG_ENABLE  1  /* WINS使能 */

#define TAF_NVIM_APN_CUSTOM_CHAR_MAX_NUM 16

typedef VOS_UINT32 MMA_QuickStartStaUint32;
/* 对NVID枚举的转定义(PS_NV_ID_ENUM, SYS_NV_ID_ENUM, RF_NV_ID_ENUM) */
typedef VOS_UINT16 NV_IdU16;

/* 结构说明: 上层对接应用类型枚举 */
enum SYSTEM_AppConfigType {
    SYSTEM_APP_MP = 0,  /* 上层应用是Mobile Partner */
    SYSTEM_APP_WEBUI,   /* 上层应用是Web UI */
    SYSTEM_APP_ANDROID, /* 上层应用是Android */
    SYSTEM_APP_BUTT     /* 无效值 */
};
typedef VOS_UINT16 SYSTEM_AppConfigTypeUint16;


enum TAF_NVIM_LcRatCombined {
    TAF_NVIM_LC_RAT_COMBINED_GUL = 0x55, /* 互操作方案GUL */
    TAF_NVIM_LC_RAT_COMBINED_CL  = 0xAA, /* 互操作方案CL */
    TAF_NVIM_LC_RAT_COMBINED_BUTT        /* 无效值 */
};
typedef VOS_UINT8 TAF_NVIM_LcRatCombinedUint8;


enum TAF_NVIM_LcWorkCfg {
    TAF_NVIM_LC_INDEPENT_WORK = 0, /* 独立方案 */
    TAF_NVIM_LC_INTER_WORK    = 1, /* 互通方案 */
    TAF_NVIM_LC_WORK_CFG_BUTT      /* 无效值 */
};
typedef VOS_UINT8 TAF_NVIM_LcWorkCfgUint8;

/* 枚举说明: */
enum TAF_MMA_CfreqLockModeType {
    TAF_MMA_CFREQ_LOCK_MODE_OFF  = 0x00, /* 禁止锁频功能 */
    TAF_MMA_CFREQ_LOCK_MODE_ON   = 0x01, /* 启动锁频功能 */
    TAF_MMA_CFREQ_LOCK_MODE_BUTT = 0x02  /* 无效值 */
};
typedef VOS_UINT8 TAF_MMA_CfreqLockModeTypeUint8;

/* 实际未使用 */

enum TAF_LSMS_ResendFlag {
    TAF_LSMS_RESEND_FLAG_DISABLE = 0, /* 禁止重发 */
    TAF_LSMS_RESEND_FLAG_ENABLE  = 1, /* 使能重发 */

    TAF_LSMS_RESEND_FLAG_BUTT         /* 无效值 */
};
typedef VOS_UINT8 TAF_LSMS_ResendFlagUint8;


enum TAF_NV_ActiveModemMode {
    TAF_NV_ACTIVE_SINGLE_MODEM = 0x00, /* 单Modem模式 */
    TAF_NV_ACTIVE_MULTI_MODEM  = 0x01, /* 多Modem模式 */

    TAF_NV_ACTIVE_MODEM_MODE_BUTT      /* 无效值 */
};
typedef VOS_UINT8 TAF_NV_ActiveModemModeUint8;


enum TAF_NV_GpsChipType {
    TAF_NV_GPS_CHIP_BROADCOM = 0, /* Broadcom类型 */
    TAF_NV_GPS_CHIP_HISI1102 = 1, /* Hisi1102类型 */

    TAF_NV_GPS_CHIP_BUTT          /* 无效值 */
};
typedef VOS_UINT8 TAF_NV_GpsChipTypeUint8;

/* ME Storage Function On or Off */
enum MN_MSG_MeStorageStatus {
    MN_MSG_ME_STORAGE_DISABLE = 0x00, /* ME存储短信功能关闭 */
    MN_MSG_ME_STORAGE_ENABLE  = 0x01, /* ME存储短信功能开启 */
    MN_MSG_ME_STORAGE_BUTT            /* 无效值 */
};
typedef VOS_UINT8 MN_MSG_MeStorageStatusUint8;


enum PLATFORM_RatType {
    PLATFORM_RAT_GSM,   /* GSM接入技术 */
    PLATFORM_RAT_WCDMA, /* WCDMA接入技术 */
    PLATFORM_RAT_LTE,   /* LTE接入技术 */
    PLATFORM_RAT_TDS,   /* TDS接入技术 */
    PLATFORM_RAT_1X,    /* CDMA-1X接入技术 */
    PLATFORM_RAT_HRPD,  /* CDMA-EV_DO接入技术 */

    PLATFORM_RAT_NR,    /* NR接入技术 */
    PLATFORM_RAT_BUTT   /* 无效值 */
};
typedef VOS_UINT16 PLATFORM_RatTypeUint16;


enum MTA_WcdmaBand {
    MTA_WCDMA_I_2100 = 0x0001,  /* 频段I-2100 */
    MTA_WCDMA_II_1900,          /* 频段II-1900 */
    MTA_WCDMA_III_1800,         /* 频段III-1800 */
    MTA_WCDMA_IV_1700,          /* 频段IV-1700 */
    MTA_WCDMA_V_850,            /* 频段V-850 */
    MTA_WCDMA_VI_800,           /* 频段VI-800 */
    MTA_WCDMA_VII_2600,         /* 频段VII-2600 */
    MTA_WCDMA_VIII_900,         /* 频段VIII-900 */
    MTA_WCDMA_IX_J1700,         /* 频段IX-J1700 */
    MTA_WCDMA_XI_1500 = 0x000B, /* 频段XI-1500 */
    MTA_WCDMA_XIX_850 = 0x0013, /* 频段XIX-850 */
    /*
     * 以下频段暂不支持
     * MTA_WCDMA_X,
     * MTA_WCDMA_XII,
     * MTA_WCDMA_XIII,
     * MTA_WCDMA_XIV,
     * MTA_WCDMA_XV,
     * MTA_WCDMA_XVI,
     * MTA_WCDMA_XVII,
     * MTA_WCDMA_XVIII,
     */
    MTA_WCDMA_BAND_BUTT
};
typedef VOS_UINT16 MTA_WcdmaBandUint16;


enum TAF_NVIM_RatMode {
    TAF_NVIM_RAT_MODE_GSM = 0x01, /* GSM接入模式 */
    TAF_NVIM_RAT_MODE_WCDMA,      /* WCDMA接入模式 */
    TAF_NVIM_RAT_MODE_LTE,        /* LTE接入模式 */
    TAF_NVIM_RAT_MODE_CDMA1X,     /* CDMA1X接入模式 */
    TAF_NVIM_RAT_MODE_TDSCDMA,    /* TDSCDMA接入模式 */
    TAF_NVIM_RAT_MODE_WIMAX,      /* WIMAX接入模式 */
    TAF_NVIM_RAT_MODE_EVDO,       /* EVDO接入模式 */

    TAF_NVIM_RAT_MODE_BUTT        /* 无效值 */
};
typedef VOS_UINT8 TAF_NVIM_RatModeUint8;


enum TAF_NVIM_GSM_BAND_ENUM {
    TAF_NVIM_GSM_BAND_850 = 0, /* 频段850 */
    TAF_NVIM_GSM_BAND_900,     /* 频段900 */
    TAF_NVIM_GSM_BAND_1800,    /* 频段1800 */
    TAF_NVIM_GSM_BAND_1900,    /* 频段1900 */

    TAF_NVIM_GSM_BAND_BUTT     /* 无效值 */
};
typedef VOS_UINT16 TAF_NVIM_GsmBandUint16;


enum NV_MsMode {
    NV_MS_MODE_CS_ONLY, /* 仅支持CS域 */
    NV_MS_MODE_PS_ONLY, /* 仅支持PS域 */
    NV_MS_MODE_CS_PS,   /* CS和PS都支持 */

    NV_MS_MODE_ANY, /* ANY,相当于仅支持CS域 */

    NV_MS_MODE_BUTT
};
typedef VOS_UINT8 NV_MsModeUint8;


enum AT_CvhuMode {
    CVHU_MODE_0,   /* 下发ATH可以挂断通话 */
    CVHU_MODE_1,   /* 下发ATH只是返回OK，不会影响通话 */
    CVHU_MODE_BUTT /* 其他值按照ATH可以挂断通话处理 */
};
typedef VOS_UINT8 AT_CvhuModeUint8;


enum CCWAI_Mode {
    CCWAI_MODE_DISABLE = 0,
    CCWAI_MODE_ENABLE  = 1,

    CCWAI_MODE_BUTT
};
typedef VOS_UINT8 CCWAI_ModeUint8;


enum CCWA_CtrlMode {
    CCWA_CTRL_BY_NW = 0,
    CCWA_CTRL_BY_UE = 1,

    CCWA_CTRL_MODE_BUTT
};
typedef VOS_UINT8 CCWA_CtrlModeUint8;


typedef struct {
    VOS_UINT8 serialNumber[TAF_NVIM_SN_LEN]; /* SN码流 */
} TAF_NVIM_SerialNum;


typedef struct {
    /*
     * SVLTE特性的开启和关闭。
     * 1：支持SVLTE；
     * 0：不支持SVLTE。
     */
    VOS_UINT8 svlteSupportFlag;
    VOS_UINT8 reserved[3]; /* 保留 */
} SVLTE_SupportFlag;


typedef struct {
    /*
     * 取LC特性功能是否使能
     * 1：使能；
     * 0：不使能。
     * CDMA宏打开时lcEnableFlg默认填false
     */
    VOS_UINT8                           ucLCEnableFlg;
    TAF_NVIM_LcRatCombinedUint8 enRatCombined; /* L-C互操作方案接入模式 */
    TAF_NVIM_LcWorkCfgUint8     lcWorkCfg;   /* L-C互操作方案配置 */
    VOS_UINT8                           reserved[1];/* 保留 */
} TAF_NV_LC_CTRL_PARA_STRU;


typedef struct {
    /*
     * 0：上层应用为Mobile Partner（Stick形态默认值）；
     * 1：上层应用为Web UI（E5、Hilink形态默认值）；
     * 2：上层应用为Android（AP-Modem形态默认值）。
     */
    SYSTEM_AppConfigTypeUint16         sysAppConfigType;
    VOS_UINT8                          reserve1; /* 保留 */
    VOS_UINT8                          reserve2; /* 保留 */
} NAS_NVIM_SystemAppConfig;


typedef struct {
    /*
     * 锁频功能是否启用
     * 1：启用
     * 0：不启用
     */
    TAF_MMA_CfreqLockModeTypeUint8 freqLockMode;
    VOS_UINT8                               reserve[3];    /* 保留 */
    VOS_UINT16                              sid;           /* 系统号 */
    VOS_UINT16                              nid;           /* 网络号 */
    VOS_UINT16                              cdmaBandClass; /* cdma 1X频段 */
    VOS_UINT16                              cdmaFreq;      /* cdma 1X频点 */
    VOS_UINT16                              cdmaPn;        /* cdma 1X的PN值 */
    VOS_UINT16                              evdoBandClass; /* EVDO频段 */
    VOS_UINT16                              evdoFreq;      /* EVDO频点 */
    VOS_UINT16                              evdoPn;        /* EVDO的PN值 */
} TAF_NVIM_CfreqLockCfg;


typedef struct {
    VOS_UINT8 internationalRoamEmcNotSelIms; /* 国际漫游紧急呼不选择Ims标志 */
    /*
     * 紧急呼失败后是否换域标志，
     * 1:紧急呼失败后换域
     * 0:紧急呼失败后继续在当前域搜网尝试
     */
    VOS_UINT8 notReSelDomainAfterEmcFailFlg;
    VOS_UINT8 reserved2; /* 保留位 */
    VOS_UINT8 reserved3; /* 保留位 */
    VOS_UINT8 reserved4; /* 保留位 */
    VOS_UINT8 reserved5; /* 保留位 */
    VOS_UINT8 reserved6; /* 保留位 */
    VOS_UINT8 reserved7; /* 保留位 */
} TAF_NVIM_SpmEmcCustomCfg;


typedef struct {
    VOS_UINT8 multiModeEmcSupportFlag; /* 0：不支持；1：支持 */
    VOS_UINT8 reserved1;               /* 保留位 */
    VOS_UINT8 reserved2;               /* 保留位 */
    VOS_UINT8 reserved3;               /* 保留位 */
} TAF_NVIM_MultiModeEmcCfg;

/* 实际未使用 */

typedef struct {
    /*
     * IMS发送3GPP2短信功能使能标志：
     * 0：功能禁止
     * 1：功能开启
     */
    VOS_UINT8 lteSmsEnable;
    /*
     * IMS发送3GPP2短信重发功能使标志：
     * 0：禁止重发
     * 1：允许重发
     */
    TAF_LSMS_ResendFlagUint8 resendFlag;
    VOS_UINT8                       resendMax; /* IMS发送3GPP2短信重发最大次数 */
    /*
     * IMS发送3GPP2短信重发时间间隔
     * 单位：秒。
     */
    VOS_UINT8 resendInterval;
} TAF_NVIM_LteSmsCfg;


typedef struct {
    /*
     * 0：单活动Modem；
     * 1：多活动Modem。
     */
    TAF_NV_ActiveModemModeUint8         activeModem;
    VOS_UINT8                           reserve[3]; /* 保留位 */
} TAF_NV_DsdsActiveModemMode;


typedef struct {
    VOS_UINT8 operNameServicePrioNum; /* 用于配置查询运营商名称时支持的文件总数，取值范围[0-4] */

    /*
     * 1代表支持PNN，cosp或^eonsucs2查询时检查PNN文件查找匹配的运营商名称；
     * 2代表支持CPHS：cops或^ eonsucs2查询时检查CPHS文件查找匹配的运营商名；
     * 3代表支持MM INFO,cops或^eonsucs2查询时从mm/gmm/emm information中查找匹配的运营商名称;
     * 4代表支持SPN,cops或^eonsucs2查询时检查SPN文件查找匹配的运营商名称
     */
    VOS_UINT8 operNameSerivcePrio[TAF_NVIM_MAX_OPER_NAME_SERVICE_PRIO_NUM];
    VOS_UINT8 reserved1; /* 保留 */
    VOS_UINT8 reserved2; /* 保留 */
    VOS_UINT8 reserved3; /* 保留 */
    /*
     * 配置PLMN比较是否支持通配符
     * 0：不支持
     * 1：支持
     */
    VOS_UINT8 plmnCompareSupportWildCardFlag;
    VOS_UINT8 wildCard;                       /* 通配符,取值a-f,可代表0-9任意数字 */
    VOS_UINT8 reserved4;                      /* 保留 */
    VOS_UINT8 reserved5;                      /* 保留 */
} TAF_NVIM_EnhancedOperNameServiceCfg;


typedef struct {
    /*
     * NV安全控制模式：
     * 0：不受限；
     * 1：全部限制；
     * 2：受限黑名单。
     */
    VOS_UINT8 secType;
    /*
     * 整型值，黑名单中NV ID的个数，取值范围0~51。
     * 仅在secType值为2时生效。
     */
    VOS_UINT8 blackListNum;
    /*
     * 整型值数组，受限NV ID黑名单。
     * 仅在secType值为2时生效，有效个数由blackListNum指示。
     */
    VOS_UINT16 blackList[TAF_NV_BLACK_LIST_MAX_NUM];
} TAF_NV_NvwrSecCtrl;


typedef struct {
    /*
     * 是否为C核单独复位
     * 0：不是C核单独复位,而是上电
     * 1：是C核单独复位
     */
    VOS_UINT8 ccpuResetFlag;
    VOS_UINT8 reserved1; /* 保留字段 */
    VOS_UINT8 reserved2; /* 保留字段 */
    VOS_UINT8 reserved3; /* 保留字段 */
} TAF_NVIM_CcpuResetRecord;


typedef struct {
    /*
     * 隐私过滤功能是否使能标志：
     * 0：不使能
     * 1：使能
     */
    VOS_UINT8 filterEnableFlg;
    VOS_UINT8 reserved[3]; /* 保留字段; */
} NAS_NV_PrivacyFilterCfg;


typedef struct {
    /*
     * 控制^XCPOSRRPT命令主动上报功能是否打开：
     * 0：关闭；
     * 1：开启。
     * 注：除此NV项外^XCPOSRRPT命令同时还受到^XCPOSR命令和^XCPOSRRPT设置命令控制
     */
    VOS_UINT8 xcposrRptNvCfg;
    VOS_UINT8 cposrDefault;     /* 单板上电后+CPOSR主动上报命令的默认值 */
    VOS_UINT8 xcposrDefault;    /* 单板上电后^XCPOSR命令的默认值 */
    VOS_UINT8 xcposrRptDefault; /* 单板上电后^XCPOSRRPT命令的默认值 */
} TAF_NVIM_XcposrrptCfg;


typedef struct {
    VOS_UINT8 ratOrderNum; /* SYS CFG配置时ratOrder中有效个数 */
    /*
     * Rat类型
     * 0: GSM
     * 1:WCDMA
     * 2:LTE
     * 3:1X
     * 4:HRPD
     * 5:NR
     */
    VOS_UINT8 ratOrder[TAF_NVIM_MAX_USER_SYS_CFG_RAT_NUM];
} TAF_NVIM_MultimodeRatCfg;


typedef struct {
    /*
     * GPS芯片类型：
     * 0：Broadcom
     * 1：Hisi1102
     */
    TAF_NV_GpsChipTypeUint8 gpsChipType;
    /*
     * SCPLL调整规避方案类型：
     * 0：关闭，参考时钟锁定按照实际情况上报；
     * 1：时钟锁定在Modem1时，替换模式为CDMA。
     */
    VOS_UINT8 pllStubType;
    VOS_UINT8 reserve2; /* 保留 */
    VOS_UINT8 reserve3; /* 保留 */
} TAF_NVIM_GpsCustCfg;


typedef struct {
    /*
     * 读取快速开机标志。
     * 0x00000000：不激活；
     * 0x01000101：激活。
     */
    MMA_QuickStartStaUint32 quickStartSta;
} NAS_NVIM_FollowonOpenspeedFlag;


typedef struct {
    VOS_UINT8 mfrId[TAF_MAX_MFR_ID_STR_LEN]; /* UE制造商名称，最后一个字节为结束符‘\0’。 */
} TAF_PH_FmrId;


typedef struct {
    VOS_UINT8 rsv[15];   /* 保留。 */
    VOS_UINT8 s0TimerLen; /* 自动应答等待时间（单位：s）。 */
} TAF_CCA_TelePara;


typedef struct {
    VOS_UINT8 productName[TAF_PH_PRODUCT_NAME_STR_LEN]; /* 保存产品名称，最后一个字节为结束符’\0’。 */
} TAF_PH_ProductName;


typedef struct {
    /*
     * 1、accessMode[0]
     * WCDMA单模，值为0x00。
     * WCDMA/GSM双模，值为0x01。
     * GSM单模，值为0x02。
     * 2、accessMode[1]
     * 自动选择网络，值为0x00。
     * 双模下优先选择GSM模式，值为0x01。
     * 双模下优先选择WCDMA模式，值为0x02。
     */
    VOS_UINT8 accessMode[2];
    VOS_UINT8 reserve1; /* 预留位 */
    VOS_UINT8 reserve2; /* 预留位 */
} NAS_MMA_NvimAccessMode;


typedef struct {
    /*
     * 0：只支持CS域手机；
     * 1：只支持PS域手机；
     * 2：支持CS/PS域手机；
     * 3：NV_MS_MODE_ANY，等同NV_MS_MODE_CS
     */
    VOS_UINT8 msClass;
    VOS_UINT8 reserved; /* 保留，用于字节对齐。 */
} NAS_NVIM_MsClass;


typedef struct {
    /*
     * 1、smsServicePara[0]
     *    短信接收存储上报方式：0：丢弃；1：modem存储短信并上报给AP；
     *                          2：modem不存储直接上报给AP；3：modem将短信上报给AP并进行确认。
     * 2、smsServicePara[1]
     *    仅当smsServicePara[6]为1时有效。
     *    0：无存储介质；
     *    1：SM（SIM卡）存储；
     *    2：ME（Flash）存储。
     * 3、smsServicePara[2]：短信状态报告存储上报方式：取值同index 0。
     * 4、smsServicePara[3]：短信状态报告接收存储介质：取值同index 1。
     * 5、smsServicePara[4]：广播短信接收存储介质：取值同index 1。
     * 6、smsServicePara[5]
     *    应用的短信存储状态，根据产品形态定制。
     *    0：应用的短信存储空间满不可用；
     *    1：应用的短信存储空间满可用。
     * 7、smsServicePara[6]
     *    smsServicePara[1]中存储的接收短信存储介质类型是否有效。
     *    0：无效，上电开机时短信接收存储介质类型是SM；
     *    1：有效，上电开机时短信接收存储介质类型从smsServicePara[1]获取。
     * 8、smsServicePara[7]
     *    发送域定制类别。
     *    0：自动调整短信发送域；
     *    1：只在+CGSMS命令指定的域中发送短信。
     */
    VOS_UINT8 smsServicePara[MN_MSG_SRV_PARAM_LEN];
} TAF_NVIM_SmsServicePara;


typedef struct {
    /*
     * 0：NV项关闭；
     * 1：NV项打开。
     */
    VOS_UINT8 roamFeatureFlg;
    /*
     * 0：国内国际漫游打开；
     * 1：国内漫游打开国际漫游关闭；
     * 2：国内漫游关闭国际漫游打开；
     * 3：国内漫游关闭国际漫游关闭；
     * 4：漫游不变。
     */
    VOS_UINT8 roamCapability;
    VOS_UINT8 reserve1; /* 保留 */
    VOS_UINT8 reserve2; /* 保留 */
} NAS_NVIM_RoamCfgInfo;

/* NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO 8267 */

typedef struct {
    VOS_UINT8 mncNum; /* 字符型：默认0xaa */
    /*
     * 字符“0”～“9”，对号码段中未填入号码段的位置全部置为0x0A，而对填入了号码段但未达到15位的位置全部置为0x0F，
     * NV中的实际值对应15位IMSI号段或“0x0A”或“0x0F”
     */
    VOS_UINT8 rangeBegin[TAF_PH_SIMLOCK_PLMN_STR_LEN];
    /*
     * 字符“0”～“9”，对号码段中未填入号码段的位置全部置为0x0A，而对填入了号码段但未达到15位的位置全部置为0x0F，
     * NV中的实际值对应15位IMSI号段或“0x0A”或“0x0F”
     */
    VOS_UINT8 rangeEnd[TAF_PH_SIMLOCK_PLMN_STR_LEN];
} TAF_CUSTOM_SimLockPlmnRange;


typedef struct {
    VOS_UINT32                          status; /* NV项是否激活标志，0不激活，1激活 */
    /* 记录SIM LOCK要求的PLMN信息 */
    TAF_CUSTOM_SimLockPlmnRange simLockPlmnRange[TAF_MAX_SIM_LOCK_RANGE_NUM];
} TAF_CUSTOM_SimLockPlmnInfo;

/* NV_ITEM_CARDLOCK_STATUS 8268 */

typedef struct {
    /*
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT32 status;
    /*
     * 0：CARDLOCK_CARD_UNLOCKCODE_NEEDED，数据卡可以正常使用，目前暂时没有使用。
     * 1：CARDLOCK_CARD_UNLOCKED，数据卡没有被锁定，但是需要输入<unlock_code>。
     * 2：CARDLOCK_CARD_LOCKED，数据卡已经被解锁，可以正常使用，如果是合法的IMSI，上报给后台的也是这个值。
     * 3：CARDLOCK_MAX，数据卡已经被锁定，不能够使用非法的SIM卡。
     */
    VOS_UINT32 cardlockStatus;
    VOS_UINT32 remainUnlockTimes; /* 剩余解锁次数，必须为整数：0～4294967295（2的32次方减1） */
} TAF_NVIM_CustomCardLockStatus;

/* NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES 8269 */

typedef struct {
    /*
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT32 status;
    VOS_UINT32 lockMaxTimes; /* 最大解锁次数，必须为整数：0～10 */
} TAF_CUSTOM_SimLockMaxTimes;


typedef struct {
    /*
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT32 status;
    /*
     * 是否使能普通GSM和CDMA语音业务。
     * 0：禁止；
     * 1：使能。
     */
    VOS_UINT32 customizeService;
} NAS_NVIM_CustomizeService;


typedef struct {
    /*
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 ucStatus;
    /*
     * WINS使能标记
     * 0：禁止；
     * 1：使能。
     */
    VOS_UINT8 ucWins;
    VOS_UINT8 reserve1; /* 保留 */
    VOS_UINT8 reserve2; /* 保留 */
} WINS_Config;


typedef struct {
    /*
     * 激活状态：
     * 0：未激活；
     * 1：激活；
     * -1：不支持。
     */
    VOS_INT8 status;
    /*
     * URL字符串的编码方式：
     * 1：8bit编码；
     * 2：UCS2编码；
     */
    VOS_UINT8  encodeType;
    VOS_UINT8  reserved1;                                     /* 保留 */
    VOS_UINT8  reserved2;                                     /* 保留 */
    VOS_UINT32 length;                                        /* URL字符串的长度。 */
    VOS_UINT8  data[TAF_NVIM_MSG_ACTIVE_MESSAGE_MAX_URL_LEN]; /* URL字符串码流，目前支持的最大长度为160字节。 */
} TAF_AT_NvimSmsActiveMessage;


typedef struct {
    /*
     * 此NV是否有效。
     * 0：无效；
     * 1：有效。
     */
    VOS_UINT8 activeFlag;
    VOS_UINT8 svn[TAF_SVN_DATA_LENGTH]; /* IMEI SVN第二个数字，取值范围0~9。 */
    VOS_UINT8 reserve[1];               /* 保留字节。 */
} TAF_SVN_Data;


typedef struct {
    /*
     * 是否支持PC Voice功能。
     * 0：不支持（其他形态默认值）；
     * 1：支持（STICK，HILINK形态默认值）。
     */
    VOS_UINT16 pcVoiceSupportFlag;
    VOS_UINT8  reserve1; /* 保留 */
    VOS_UINT8  reserve2; /* 保留 */
} APP_VC_NvimPcVoiceSupportFalg;


typedef struct {
    /*
     * ME存储短信功能使能标志。
     * 0：不使能；
     * 1：使能。
     */
    MN_MSG_MeStorageStatusUint8 meStorageStatus;
    VOS_UINT8                           reserve[1]; /* 保留。 */
    /*
     * ME存储短信容量，在MeStorageStatus使能时有效，取值范围0~500条。
     * 取值=0，ME存储介质无可用空间；
     * 取值>500，协议栈强制将ME存储容量修改为500。
     */
    VOS_UINT16 meStorageNum;
} MN_MSG_MeStorageParm;


typedef struct {
    VOS_UINT8 ratOrderNum; /* 当前设定的接入技术个数，取值范围1~3。 */
    /*
     * 当前系统支持的接入技术及其优先级列表。
     * 0：GSM；
     * 1：WCDMA；
     * 2：LTE；
     * 3：1X；
     * 4：HRPD。
     */
    VOS_UINT8 ratOrder[TAF_PH_NVIM_MAX_GUL_RAT_NUM];
} TAF_PH_NvimRatOrder;


typedef struct {
    VOS_UINT8 ratOrderNum; /* 当前设定支持的接入制式个数，取值范围1~4; */
    /*
     * 当前支持的接入制式：
     * 0：GSM；
     * 1：WCDMA；
     * 2：LTE；
     * 3：1X；
     * 4：HRPD；
     * 5：NR;
     */
    VOS_UINT8 ratOrder[TAF_NVIM_MAX_RAT_ORDER_NUM];
    VOS_UINT8 reserved1; /* 保留 */
    VOS_UINT8 reserved2; /* 保留 */
    VOS_UINT8 reserved3; /* 保留 */

} TAF_NVIM_RatOrderEx;


typedef struct {
    /*
     * LTE允许漫游标记。
     * 0：LTE禁止漫游；
     * 1：LTE允许漫游。
     */
    VOS_UINT8 lteRoamAllowedFlg;
    VOS_UINT8 reserve[1];             /* 保留项 */
    VOS_UINT8 roamEnabledMccList[20]; /* 允许漫游的国家码列表 。 */
} NAS_MMC_NvimLteInternationalRoamCfg;


typedef struct {
    /*
     * NV项状态。
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 status;
    /*
     * 是否激活防卡贴功能。
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 actFlg;
    VOS_UINT8 reserve1; /* 保留 */
    VOS_UINT8 reserve2; /* 保留 */
} NAS_PREVENT_TestImsiReg;


typedef struct {
    /*
     * 短信接收流程定制激活标志。
     * 0：未激活；
     * 1：已激活。
     */
    VOS_UINT8 actFlag;
    /*
     * 短信接收流程定制类型。
     * 0：未定制；
     * 1：DCM定制；
     */
    VOS_UINT8 enMtCustomize;
    /*
     * 接收Download短信流程中，是否延时发送stk短信。
     * 0：不延时
     * 1：延时
     */
    VOS_UINT8 mtCustomize[2];
} MN_MSG_MtCustomizeInfo;


typedef struct {
    /*
     * 信号改变门限配置
     * 0：信号质量改变门限接入层使用默认值。GAS默认值是3dB，WAS默认值由NV9067配置；
     * 1：信号质量改变超过1dB，接入层需主动上报信号质量；
     * 2：信号质量改变超过2dB，接入层需主动上报信号质量；
     * 3：信号质量改变超过3dB，接入层需主动上报信号质量；
     * 4：信号质量改变超过4dB，接入层需主动上报信号质量；
     * 5：信号质量改变超过5dB，接入层需主动上报信号质量。
     */
    VOS_UINT8 signThreshold;
    /*
     * 0：上报扩展信号质量无时间限制；
     * 1-20：两次上报信号质量的最小间隔时间，单位：s。
     */
    VOS_UINT8 minRptTimerInterval;
    VOS_UINT8 rerved1; /* 保留，值为0 */
    VOS_UINT8 rerved2; /* 保留，值为0 */
} NAS_NVIM_CellSignReportCfg;


typedef struct {
    /*
     * 是否使能产品线定制的PDP回退原因值。
     * 1：使能；
     * 0：禁止；
     */
    VOS_UINT32 activeFlag;
    VOS_UINT8  smCause[TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM]; /* 产品线定制的PDP回退原因值。 */
} TAF_NV_Ipv6FallbackExtCause;


typedef struct {
    /*
     * 此NV是否生效。
     * 0：此NV不生效；
     * 1：此NV生效。
     */
    VOS_UINT8 nvimValid;
    /*
     * 当ucNvimValid指示此NV生效时，当收不到AP的RP ACK时，后续收到的短信是否需要本地存储。
     * 0：不需要；
     * 1：需要。
     * 当设置为1时，后续收到的MT短信进行本地存储，不会上报给AP，即关闭上报给AP的通道，把CNMI的参数<mode>和<ds>修改为0。
     */
    VOS_UINT8 smsClosePathFlg;
} TAF_NVIM_SmsClosePathCfg;


typedef struct {
    VOS_UINT16 ratNum; /* 接入技术的个数。 */
    /*
     * 接入技术，Rat类型。
     * 0：GSM；
     * 1：WCDMA；
     * 2：LTE；
     * 3：TDS；
     * 4：1X；
     * 5：HRPD；
     * 6：NR。
     */
    PLATFORM_RatTypeUint16 ratList[PLATFORM_MAX_RAT_NUM];
} PLATAFORM_RatCapability;


typedef struct {
    VOS_INT16 gprsPower; /* GSM频段的GPRS发射功率门限值，单位为dbm，取值范围15~33 */
    VOS_INT16 edgePower; /* GSM频段的EDGE发射功率门限值，单位为dbm，取值范围15~33 */
} MTA_BodySarGPara;


typedef struct {
    MTA_WcdmaBandUint16        band; /* WCDMA频段，Band值为1~9、11、19。 */
    VOS_INT16                  power; /* 对应WCDMA频段的最大发射功率门限值，单位为dbm，取值范围17~24。 */
} MTA_BodySarWPara;


typedef struct {
    /*
     * GSM 配置功率门限的BAND掩码，其参数为16进制，每个bit对应一个频段，Bit值为1表示已配置，为0表示未配置。
     * Bit0：GSM850(GPRS)；
     * Bit1：GSM900(GPRS)；
     * Bit2：GSM1800(GPRS)；
     * Bit3：GSM1900(GPRS)；
     * Bit16：GSM850(EDGE)；
     * Bit17：GSM900(EDGE)；
     * Bit18：GSM1800(EDGE)；
     * Bit19：GSM1900(EDGE)。
     */
    VOS_UINT32               gBandMask;
    VOS_UINT16               wBandNum;      /* WCDMA配置功率门限的BAND个数，最多支持5个频段。 */
    VOS_UINT16               reserved1[1]; /* 保留。 */
    MTA_BodySarGPara gBandPara[MTA_BODY_SAR_GBAND_MAX_NUM]; /* GSM频段对应的GPRS/EDGE功率门限值。 */
    MTA_BodySarWPara wBandPara[MTA_BODY_SAR_WBAND_MAX_NUM]; /* WCDMA对应的频段及功率门限值。 */
} MTA_BodySarPara;


typedef struct {
    /*
     * 紧急呼叫与温度保护交互标志位。
     * 1：使能；
     * 0：禁止。
     */
    VOS_UINT8 eCallNotifySupport;
    /*
     * 温保状态上报使能标志位。
     * 1：使能；
     * 0：禁止。
     */
    VOS_UINT8 spyStatusIndSupport;
    VOS_UINT8 reserved[2]; /* 保留项 */
} TAF_TempProtectConfig;


typedef struct {
    /*
     * 短信优选域。
     * 0：不使用IMS短信；
     * 1：优先使用IMS短信,CS/PS短信备选。
     */
    VOS_UINT8 smsDomain;
    VOS_UINT8 reserved[3]; /* 保留字段 */
} TAF_NVIM_SmsDomain;


typedef struct {
    /*
     * Jam Detect功能工作模式。
     * 0：关闭
     * 1：打开
     */
    VOS_UINT8 mode;
    /*
     * Jam Detect使用的方案。目前只支持方案2。
     * 1：方案1
     * 2：方案2
     */
    VOS_UINT8 method;
    VOS_UINT8 freqNum;            /* 检测需要达到的频点个数，取值范围[0-255]。 */
    VOS_UINT8 threshold;          /* 检测需要达到的频点测量值，取值范围[0-70]。 */
    VOS_UINT8 jamDetectingTmrLen; /* 强信号频点个数满足门限后，检查同步结果的定时器时长，单位：秒。 */
    VOS_UINT8 jamDetectedTmrLen;  /* 检测到干扰后，等待搜网结果的最长时间，单位：秒。 */
    /*
     * 搜网过程中是否需要快速上报。
     * 0：否
     * 1：是
     */
    VOS_UINT8 fastReportFlag;
    VOS_UINT8 rxlevThresholdOffset; /* G模噪音门限配置偏移值。 */
} NV_NAS_JamDetectCfg;


typedef struct {
    /*
     * 锁频功能开关。
     * 0：关闭；
     * 1：打开。
     */
    VOS_UINT8                     enableFlg;
    TAF_NVIM_RatModeUint8  ratMode;    /* 接入模式。 */
    TAF_NVIM_GsmBandUint16 band;       /* GSM频段。 */
    VOS_UINT32                    lockedFreq; /* 锁频频点。 */
} TAF_NVIM_FreqLockCfg;

/*
 * 标    识: NV_ITEM_TRAFFIC_CLASS_TYPE(50009)
 * 结构说明: 用于定制PDP激活请求中QoS中Traffic Class的值
 */
typedef struct {
    /*
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 status;
    /*
     * Traffic Class的值,整数。
     * 0：Subscribed value；
     * 1：Conversational class；
     * 2：Streaming Class；
     * 3：Interactive Class；
     * 4：Background Class。
     */
    VOS_UINT8 trafficClass;
    VOS_UINT8 reserve1; /* 保留位 */
    VOS_UINT8 reserve2; /* 保留位 */
} AT_TrafficClassCustomize;


typedef struct {
    /*
     * NV激活标志位
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 status;
    /*
     * USSD消息传输方案, 默认为透传模式。
     * 0：USSD非透传方案（即单板支持编解码）；
     * 1：USSD透传方案（即单板不编解码，只是透传，由后台来编解码）
     */
    VOS_UINT8 ussdTransMode;
    VOS_UINT8 reserve1; /* 保留位 */
    VOS_UINT8 reserve2; /* 保留位 */
} TAF_UssdNvimTransMode;


typedef struct {
    /*
     * 本NV项是否激活。
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 actFlg;
    /*
     * 0：Vodafone Class0短信定制；
     * 1：Italy TIM Class0短信定制（即H3G Class0 短信定制）；
     * 2：3GPP协议的处理方式；
     * 3：巴西VIVO定制（CLASS0类短信总是直接上报内容，根据短信协议类型（通过CSMS命令修改）确定是否由应用确认短信接收）。
     */
    VOS_UINT8 class0Tailor;
} MN_MSG_NvimClass0Tailor;


typedef struct {
    /*
     * 设置呼叫等待的控制模式；
     * ccwaCtrlMode为0，则呼叫等待由3gpp网络控制；
     * ccwaCtrlMode为1，则呼叫等待由UE控制，用于VOLTE的网络。
     * （在VoLTE的网络上，AP配置CCWA支持时，IMS并没有和网络交互，VoLTE的电话的CCWA由UE控制）。
     * 支持VOLTE的终端，不管VOLTE开关有没有打开，都是下发CCWAI来设置呼叫等待。
     */
    VOS_UINT8 ccwaCtrlMode;
    /*
     * 呼叫等待设置模式
     * 0：disable
     * 1：enable
     */
    VOS_UINT8 ccwaiMode;
    VOS_UINT8 reserved1; /* 预留 */
    VOS_UINT8 reserved2; /* 预留 */
} TAF_CALL_NvimCcwaCtrlMode;


typedef struct {
    /*
     * CVHU模式设置,控制是否支持ATH挂断通话。
     * 0：下发ATH可以挂断通话。
     * 1：下发ATH只是返回OK，不影响通话。
     */
    AT_CvhuModeUint8        cvhuMode;
    VOS_UINT8               reserved1; /* 预留位 */
    VOS_UINT8               reserved2; /* 预留位 */
    VOS_UINT8               reserved3; /* 预留位 */
} NAS_NVIM_CustomCallCfg;


typedef struct {
    /*
     * USIMM、SIMSQ命令主动上报功能开关。
     * 0：关闭。
     * 1：打开。
     */
    VOS_UINT8 simsqEnable;
    VOS_UINT8 reserved1; /* 预留位 */
    VOS_UINT8 reserved2; /* 预留位 */
    VOS_UINT8 reserved3; /* 预留位 */
} NAS_NVIM_CustomUsimmCfg;


typedef struct {
    VOS_UINT8 ucCustomCharNum; /* 定制字符个数，最多可以设置16个字符。 */
    /*
     * APNNI部分开头结尾字符串检查使能项。
     * 0：不检查
     * 1：检查
     */
    VOS_UINT8 ucProtocolStringCheckFlag;
    VOS_UINT8 ucReserved1; /* 保留位 */
    VOS_UINT8 ucReserved2; /* 保留位 */
    VOS_UINT8 ucReserved3; /* 保留位 */
    VOS_UINT8 ucReserved4; /* 保留位 */
    VOS_UINT8 ucReserved5; /* 保留位 */
    VOS_UINT8 ucReserved6; /* 保留位 */
    /* 定制字符数组，可以设置ASCII表中除控制字符（0-31），正反斜杠（47,92）以及删除符号（127）以外的字符。 */
    VOS_UINT8 aucCustomChar[TAF_NVIM_APN_CUSTOM_CHAR_MAX_NUM];
} TAF_NVIM_ApnCustomFormatCfg;


typedef struct {
    /*
     * LTE下eCall呼叫域选模式
     * 0：平台默认实现。
     * 1：LTE下eCall呼叫强制CSFB到CS域进行。
     * 2：LTE下eCall呼叫与普通呼叫的呼叫域选择规则一致。在VoLTE下调用eCall接口实际请求的是普通呼叫或者普通紧急呼叫。
     * 3：在VoLTE下调用eCall接口符合3GPP R14对eCall over IMS要求。（保留）
     */
    VOS_UINT8 ecallDomainMode;
    /*
     * eCall强制模式
     * 0：Auto模式，UE根据SIM文件来决定运行模式，根据3GPP协议TS31.102，
     *    如果在EFUST中eCall data (Service n°89) 和FDN (Service n°2) 开启，并且EFEST中FDN（Service n°1）开启，
     *    则以eCall only模式运行，
     *    如果在EFUST中eCall data (Service n°89) 和SDN (Service n°4) 开启，则以eCall and normal模式运行，其他情况按
     *    照normal模式运行；
     * 1：Force eCall Only模式，无论是插入的是哪种SIM卡，UE都以eCall only模式来运行；eCall only模式下不主动注册网络，
     *    处于eCall inactive状态；
     * 2：Force eCall and normal模式，无论是插入的是哪种SIM卡，UE都以eCall and normal模式来运行；eCall and normal
     *    模式正常注册网络；
     */
    VOS_UINT8 ecallForceMode;
    VOS_UINT8 reserved1; /* 保留 */
    VOS_UINT8 reserved2; /* 保留 */
} TAF_NVIM_CustomEcallCfg;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __NV_STRU_GUCNAS_H__ */

