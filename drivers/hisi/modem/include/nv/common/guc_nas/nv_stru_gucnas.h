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
#define MN_MSG_SRV_PARAM_LEN 8 /* ����ҵ������ṹ��NV���д洢�ĳ��� */
#define TAF_NVIM_MSG_ACTIVE_MESSAGE_MAX_URL_LEN 160
#define TAF_SVN_DATA_LENGTH 2                     /* SVN��Ч���ݳ��� */
#define TAF_PH_NVIM_MAX_GUL_RAT_NUM 3             /* AT^syscfgex��acqorder����Ľ��뼼������ */
#define TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM 20 /* 9130��չIPv6���˴�����չԭ��ֵ������ */
#define PLATFORM_MAX_RAT_NUM (7U)                   /* ���뼼�����ֵ */
#define MTA_BODY_SAR_WBAND_MAX_NUM 5
#define MTA_BODY_SAR_GBAND_MAX_NUM 4
#define TAF_PH_SIMLOCK_PLMN_STR_LEN 8 /* Plmn �Ŷγ��� */
#define TAF_MAX_SIM_LOCK_RANGE_NUM 20

#define TAF_NVIM_MAX_RAT_ORDER_NUM 8

/* WINS������NV��Ľṹ�� */
#define WINS_CONFIG_DISABLE 0 /* WINS��ʹ�� */
#define WINS_CONFIG_ENABLE  1  /* WINSʹ�� */

#define TAF_NVIM_APN_CUSTOM_CHAR_MAX_NUM 16

typedef VOS_UINT32 MMA_QuickStartStaUint32;
/* ��NVIDö�ٵ�ת����(PS_NV_ID_ENUM, SYS_NV_ID_ENUM, RF_NV_ID_ENUM) */
typedef VOS_UINT16 NV_IdU16;

/* �ṹ˵��: �ϲ�Խ�Ӧ������ö�� */
enum SYSTEM_AppConfigType {
    SYSTEM_APP_MP = 0,  /* �ϲ�Ӧ����Mobile Partner */
    SYSTEM_APP_WEBUI,   /* �ϲ�Ӧ����Web UI */
    SYSTEM_APP_ANDROID, /* �ϲ�Ӧ����Android */
    SYSTEM_APP_BUTT     /* ��Чֵ */
};
typedef VOS_UINT16 SYSTEM_AppConfigTypeUint16;


enum TAF_NVIM_LcRatCombined {
    TAF_NVIM_LC_RAT_COMBINED_GUL = 0x55, /* ����������GUL */
    TAF_NVIM_LC_RAT_COMBINED_CL  = 0xAA, /* ����������CL */
    TAF_NVIM_LC_RAT_COMBINED_BUTT        /* ��Чֵ */
};
typedef VOS_UINT8 TAF_NVIM_LcRatCombinedUint8;


enum TAF_NVIM_LcWorkCfg {
    TAF_NVIM_LC_INDEPENT_WORK = 0, /* �������� */
    TAF_NVIM_LC_INTER_WORK    = 1, /* ��ͨ���� */
    TAF_NVIM_LC_WORK_CFG_BUTT      /* ��Чֵ */
};
typedef VOS_UINT8 TAF_NVIM_LcWorkCfgUint8;

/* ö��˵��: */
enum TAF_MMA_CfreqLockModeType {
    TAF_MMA_CFREQ_LOCK_MODE_OFF  = 0x00, /* ��ֹ��Ƶ���� */
    TAF_MMA_CFREQ_LOCK_MODE_ON   = 0x01, /* ������Ƶ���� */
    TAF_MMA_CFREQ_LOCK_MODE_BUTT = 0x02  /* ��Чֵ */
};
typedef VOS_UINT8 TAF_MMA_CfreqLockModeTypeUint8;

/* ʵ��δʹ�� */

enum TAF_LSMS_ResendFlag {
    TAF_LSMS_RESEND_FLAG_DISABLE = 0, /* ��ֹ�ط� */
    TAF_LSMS_RESEND_FLAG_ENABLE  = 1, /* ʹ���ط� */

    TAF_LSMS_RESEND_FLAG_BUTT         /* ��Чֵ */
};
typedef VOS_UINT8 TAF_LSMS_ResendFlagUint8;


enum TAF_NV_ActiveModemMode {
    TAF_NV_ACTIVE_SINGLE_MODEM = 0x00, /* ��Modemģʽ */
    TAF_NV_ACTIVE_MULTI_MODEM  = 0x01, /* ��Modemģʽ */

    TAF_NV_ACTIVE_MODEM_MODE_BUTT      /* ��Чֵ */
};
typedef VOS_UINT8 TAF_NV_ActiveModemModeUint8;


enum TAF_NV_GpsChipType {
    TAF_NV_GPS_CHIP_BROADCOM = 0, /* Broadcom���� */
    TAF_NV_GPS_CHIP_HISI1102 = 1, /* Hisi1102���� */

    TAF_NV_GPS_CHIP_BUTT          /* ��Чֵ */
};
typedef VOS_UINT8 TAF_NV_GpsChipTypeUint8;

/* ME Storage Function On or Off */
enum MN_MSG_MeStorageStatus {
    MN_MSG_ME_STORAGE_DISABLE = 0x00, /* ME�洢���Ź��ܹر� */
    MN_MSG_ME_STORAGE_ENABLE  = 0x01, /* ME�洢���Ź��ܿ��� */
    MN_MSG_ME_STORAGE_BUTT            /* ��Чֵ */
};
typedef VOS_UINT8 MN_MSG_MeStorageStatusUint8;


enum PLATFORM_RatType {
    PLATFORM_RAT_GSM,   /* GSM���뼼�� */
    PLATFORM_RAT_WCDMA, /* WCDMA���뼼�� */
    PLATFORM_RAT_LTE,   /* LTE���뼼�� */
    PLATFORM_RAT_TDS,   /* TDS���뼼�� */
    PLATFORM_RAT_1X,    /* CDMA-1X���뼼�� */
    PLATFORM_RAT_HRPD,  /* CDMA-EV_DO���뼼�� */

    PLATFORM_RAT_NR,    /* NR���뼼�� */
    PLATFORM_RAT_BUTT   /* ��Чֵ */
};
typedef VOS_UINT16 PLATFORM_RatTypeUint16;


enum MTA_WcdmaBand {
    MTA_WCDMA_I_2100 = 0x0001,  /* Ƶ��I-2100 */
    MTA_WCDMA_II_1900,          /* Ƶ��II-1900 */
    MTA_WCDMA_III_1800,         /* Ƶ��III-1800 */
    MTA_WCDMA_IV_1700,          /* Ƶ��IV-1700 */
    MTA_WCDMA_V_850,            /* Ƶ��V-850 */
    MTA_WCDMA_VI_800,           /* Ƶ��VI-800 */
    MTA_WCDMA_VII_2600,         /* Ƶ��VII-2600 */
    MTA_WCDMA_VIII_900,         /* Ƶ��VIII-900 */
    MTA_WCDMA_IX_J1700,         /* Ƶ��IX-J1700 */
    MTA_WCDMA_XI_1500 = 0x000B, /* Ƶ��XI-1500 */
    MTA_WCDMA_XIX_850 = 0x0013, /* Ƶ��XIX-850 */
    /*
     * ����Ƶ���ݲ�֧��
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
    TAF_NVIM_RAT_MODE_GSM = 0x01, /* GSM����ģʽ */
    TAF_NVIM_RAT_MODE_WCDMA,      /* WCDMA����ģʽ */
    TAF_NVIM_RAT_MODE_LTE,        /* LTE����ģʽ */
    TAF_NVIM_RAT_MODE_CDMA1X,     /* CDMA1X����ģʽ */
    TAF_NVIM_RAT_MODE_TDSCDMA,    /* TDSCDMA����ģʽ */
    TAF_NVIM_RAT_MODE_WIMAX,      /* WIMAX����ģʽ */
    TAF_NVIM_RAT_MODE_EVDO,       /* EVDO����ģʽ */

    TAF_NVIM_RAT_MODE_BUTT        /* ��Чֵ */
};
typedef VOS_UINT8 TAF_NVIM_RatModeUint8;


enum TAF_NVIM_GSM_BAND_ENUM {
    TAF_NVIM_GSM_BAND_850 = 0, /* Ƶ��850 */
    TAF_NVIM_GSM_BAND_900,     /* Ƶ��900 */
    TAF_NVIM_GSM_BAND_1800,    /* Ƶ��1800 */
    TAF_NVIM_GSM_BAND_1900,    /* Ƶ��1900 */

    TAF_NVIM_GSM_BAND_BUTT     /* ��Чֵ */
};
typedef VOS_UINT16 TAF_NVIM_GsmBandUint16;


enum NV_MsMode {
    NV_MS_MODE_CS_ONLY, /* ��֧��CS�� */
    NV_MS_MODE_PS_ONLY, /* ��֧��PS�� */
    NV_MS_MODE_CS_PS,   /* CS��PS��֧�� */

    NV_MS_MODE_ANY, /* ANY,�൱�ڽ�֧��CS�� */

    NV_MS_MODE_BUTT
};
typedef VOS_UINT8 NV_MsModeUint8;


enum AT_CvhuMode {
    CVHU_MODE_0,   /* �·�ATH���ԹҶ�ͨ�� */
    CVHU_MODE_1,   /* �·�ATHֻ�Ƿ���OK������Ӱ��ͨ�� */
    CVHU_MODE_BUTT /* ����ֵ����ATH���ԹҶ�ͨ������ */
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
    VOS_UINT8 serialNumber[TAF_NVIM_SN_LEN]; /* SN���� */
} TAF_NVIM_SerialNum;


typedef struct {
    /*
     * SVLTE���ԵĿ����͹رա�
     * 1��֧��SVLTE��
     * 0����֧��SVLTE��
     */
    VOS_UINT8 svlteSupportFlag;
    VOS_UINT8 reserved[3]; /* ���� */
} SVLTE_SupportFlag;


typedef struct {
    /*
     * ȡLC���Թ����Ƿ�ʹ��
     * 1��ʹ�ܣ�
     * 0����ʹ�ܡ�
     * CDMA���ʱlcEnableFlgĬ����false
     */
    VOS_UINT8                           ucLCEnableFlg;
    TAF_NVIM_LcRatCombinedUint8 enRatCombined; /* L-C��������������ģʽ */
    TAF_NVIM_LcWorkCfgUint8     lcWorkCfg;   /* L-C�������������� */
    VOS_UINT8                           reserved[1];/* ���� */
} TAF_NV_LC_CTRL_PARA_STRU;


typedef struct {
    /*
     * 0���ϲ�Ӧ��ΪMobile Partner��Stick��̬Ĭ��ֵ����
     * 1���ϲ�Ӧ��ΪWeb UI��E5��Hilink��̬Ĭ��ֵ����
     * 2���ϲ�Ӧ��ΪAndroid��AP-Modem��̬Ĭ��ֵ����
     */
    SYSTEM_AppConfigTypeUint16         sysAppConfigType;
    VOS_UINT8                          reserve1; /* ���� */
    VOS_UINT8                          reserve2; /* ���� */
} NAS_NVIM_SystemAppConfig;


typedef struct {
    /*
     * ��Ƶ�����Ƿ�����
     * 1������
     * 0��������
     */
    TAF_MMA_CfreqLockModeTypeUint8 freqLockMode;
    VOS_UINT8                               reserve[3];    /* ���� */
    VOS_UINT16                              sid;           /* ϵͳ�� */
    VOS_UINT16                              nid;           /* ����� */
    VOS_UINT16                              cdmaBandClass; /* cdma 1XƵ�� */
    VOS_UINT16                              cdmaFreq;      /* cdma 1XƵ�� */
    VOS_UINT16                              cdmaPn;        /* cdma 1X��PNֵ */
    VOS_UINT16                              evdoBandClass; /* EVDOƵ�� */
    VOS_UINT16                              evdoFreq;      /* EVDOƵ�� */
    VOS_UINT16                              evdoPn;        /* EVDO��PNֵ */
} TAF_NVIM_CfreqLockCfg;


typedef struct {
    VOS_UINT8 internationalRoamEmcNotSelIms; /* �������ν�������ѡ��Ims��־ */
    /*
     * ������ʧ�ܺ��Ƿ����־��
     * 1:������ʧ�ܺ���
     * 0:������ʧ�ܺ�����ڵ�ǰ����������
     */
    VOS_UINT8 notReSelDomainAfterEmcFailFlg;
    VOS_UINT8 reserved2; /* ����λ */
    VOS_UINT8 reserved3; /* ����λ */
    VOS_UINT8 reserved4; /* ����λ */
    VOS_UINT8 reserved5; /* ����λ */
    VOS_UINT8 reserved6; /* ����λ */
    VOS_UINT8 reserved7; /* ����λ */
} TAF_NVIM_SpmEmcCustomCfg;


typedef struct {
    VOS_UINT8 multiModeEmcSupportFlag; /* 0����֧�֣�1��֧�� */
    VOS_UINT8 reserved1;               /* ����λ */
    VOS_UINT8 reserved2;               /* ����λ */
    VOS_UINT8 reserved3;               /* ����λ */
} TAF_NVIM_MultiModeEmcCfg;

/* ʵ��δʹ�� */

typedef struct {
    /*
     * IMS����3GPP2���Ź���ʹ�ܱ�־��
     * 0�����ܽ�ֹ
     * 1�����ܿ���
     */
    VOS_UINT8 lteSmsEnable;
    /*
     * IMS����3GPP2�����ط�����ʹ��־��
     * 0����ֹ�ط�
     * 1�������ط�
     */
    TAF_LSMS_ResendFlagUint8 resendFlag;
    VOS_UINT8                       resendMax; /* IMS����3GPP2�����ط������� */
    /*
     * IMS����3GPP2�����ط�ʱ����
     * ��λ���롣
     */
    VOS_UINT8 resendInterval;
} TAF_NVIM_LteSmsCfg;


typedef struct {
    /*
     * 0�����Modem��
     * 1����Modem��
     */
    TAF_NV_ActiveModemModeUint8         activeModem;
    VOS_UINT8                           reserve[3]; /* ����λ */
} TAF_NV_DsdsActiveModemMode;


typedef struct {
    VOS_UINT8 operNameServicePrioNum; /* �������ò�ѯ��Ӫ������ʱ֧�ֵ��ļ�������ȡֵ��Χ[0-4] */

    /*
     * 1����֧��PNN��cosp��^eonsucs2��ѯʱ���PNN�ļ�����ƥ�����Ӫ�����ƣ�
     * 2����֧��CPHS��cops��^ eonsucs2��ѯʱ���CPHS�ļ�����ƥ�����Ӫ������
     * 3����֧��MM INFO,cops��^eonsucs2��ѯʱ��mm/gmm/emm information�в���ƥ�����Ӫ������;
     * 4����֧��SPN,cops��^eonsucs2��ѯʱ���SPN�ļ�����ƥ�����Ӫ������
     */
    VOS_UINT8 operNameSerivcePrio[TAF_NVIM_MAX_OPER_NAME_SERVICE_PRIO_NUM];
    VOS_UINT8 reserved1; /* ���� */
    VOS_UINT8 reserved2; /* ���� */
    VOS_UINT8 reserved3; /* ���� */
    /*
     * ����PLMN�Ƚ��Ƿ�֧��ͨ���
     * 0����֧��
     * 1��֧��
     */
    VOS_UINT8 plmnCompareSupportWildCardFlag;
    VOS_UINT8 wildCard;                       /* ͨ���,ȡֵa-f,�ɴ���0-9�������� */
    VOS_UINT8 reserved4;                      /* ���� */
    VOS_UINT8 reserved5;                      /* ���� */
} TAF_NVIM_EnhancedOperNameServiceCfg;


typedef struct {
    /*
     * NV��ȫ����ģʽ��
     * 0�������ޣ�
     * 1��ȫ�����ƣ�
     * 2�����޺�������
     */
    VOS_UINT8 secType;
    /*
     * ����ֵ����������NV ID�ĸ�����ȡֵ��Χ0~51��
     * ����secTypeֵΪ2ʱ��Ч��
     */
    VOS_UINT8 blackListNum;
    /*
     * ����ֵ���飬����NV ID��������
     * ����secTypeֵΪ2ʱ��Ч����Ч������blackListNumָʾ��
     */
    VOS_UINT16 blackList[TAF_NV_BLACK_LIST_MAX_NUM];
} TAF_NV_NvwrSecCtrl;


typedef struct {
    /*
     * �Ƿ�ΪC�˵�����λ
     * 0������C�˵�����λ,�����ϵ�
     * 1����C�˵�����λ
     */
    VOS_UINT8 ccpuResetFlag;
    VOS_UINT8 reserved1; /* �����ֶ� */
    VOS_UINT8 reserved2; /* �����ֶ� */
    VOS_UINT8 reserved3; /* �����ֶ� */
} TAF_NVIM_CcpuResetRecord;


typedef struct {
    /*
     * ��˽���˹����Ƿ�ʹ�ܱ�־��
     * 0����ʹ��
     * 1��ʹ��
     */
    VOS_UINT8 filterEnableFlg;
    VOS_UINT8 reserved[3]; /* �����ֶ�; */
} NAS_NV_PrivacyFilterCfg;


typedef struct {
    /*
     * ����^XCPOSRRPT���������ϱ������Ƿ�򿪣�
     * 0���رգ�
     * 1��������
     * ע������NV����^XCPOSRRPT����ͬʱ���ܵ�^XCPOSR�����^XCPOSRRPT�����������
     */
    VOS_UINT8 xcposrRptNvCfg;
    VOS_UINT8 cposrDefault;     /* �����ϵ��+CPOSR�����ϱ������Ĭ��ֵ */
    VOS_UINT8 xcposrDefault;    /* �����ϵ��^XCPOSR�����Ĭ��ֵ */
    VOS_UINT8 xcposrRptDefault; /* �����ϵ��^XCPOSRRPT�����Ĭ��ֵ */
} TAF_NVIM_XcposrrptCfg;


typedef struct {
    VOS_UINT8 ratOrderNum; /* SYS CFG����ʱratOrder����Ч���� */
    /*
     * Rat����
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
     * GPSоƬ���ͣ�
     * 0��Broadcom
     * 1��Hisi1102
     */
    TAF_NV_GpsChipTypeUint8 gpsChipType;
    /*
     * SCPLL������ܷ������ͣ�
     * 0���رգ��ο�ʱ����������ʵ������ϱ���
     * 1��ʱ��������Modem1ʱ���滻ģʽΪCDMA��
     */
    VOS_UINT8 pllStubType;
    VOS_UINT8 reserve2; /* ���� */
    VOS_UINT8 reserve3; /* ���� */
} TAF_NVIM_GpsCustCfg;


typedef struct {
    /*
     * ��ȡ���ٿ�����־��
     * 0x00000000�������
     * 0x01000101�����
     */
    MMA_QuickStartStaUint32 quickStartSta;
} NAS_NVIM_FollowonOpenspeedFlag;


typedef struct {
    VOS_UINT8 mfrId[TAF_MAX_MFR_ID_STR_LEN]; /* UE���������ƣ����һ���ֽ�Ϊ��������\0���� */
} TAF_PH_FmrId;


typedef struct {
    VOS_UINT8 rsv[15];   /* ������ */
    VOS_UINT8 s0TimerLen; /* �Զ�Ӧ��ȴ�ʱ�䣨��λ��s���� */
} TAF_CCA_TelePara;


typedef struct {
    VOS_UINT8 productName[TAF_PH_PRODUCT_NAME_STR_LEN]; /* �����Ʒ���ƣ����һ���ֽ�Ϊ��������\0���� */
} TAF_PH_ProductName;


typedef struct {
    /*
     * 1��accessMode[0]
     * WCDMA��ģ��ֵΪ0x00��
     * WCDMA/GSM˫ģ��ֵΪ0x01��
     * GSM��ģ��ֵΪ0x02��
     * 2��accessMode[1]
     * �Զ�ѡ�����磬ֵΪ0x00��
     * ˫ģ������ѡ��GSMģʽ��ֵΪ0x01��
     * ˫ģ������ѡ��WCDMAģʽ��ֵΪ0x02��
     */
    VOS_UINT8 accessMode[2];
    VOS_UINT8 reserve1; /* Ԥ��λ */
    VOS_UINT8 reserve2; /* Ԥ��λ */
} NAS_MMA_NvimAccessMode;


typedef struct {
    /*
     * 0��ֻ֧��CS���ֻ���
     * 1��ֻ֧��PS���ֻ���
     * 2��֧��CS/PS���ֻ���
     * 3��NV_MS_MODE_ANY����ͬNV_MS_MODE_CS
     */
    VOS_UINT8 msClass;
    VOS_UINT8 reserved; /* �����������ֽڶ��롣 */
} NAS_NVIM_MsClass;


typedef struct {
    /*
     * 1��smsServicePara[0]
     *    ���Ž��մ洢�ϱ���ʽ��0��������1��modem�洢���Ų��ϱ���AP��
     *                          2��modem���洢ֱ���ϱ���AP��3��modem�������ϱ���AP������ȷ�ϡ�
     * 2��smsServicePara[1]
     *    ����smsServicePara[6]Ϊ1ʱ��Ч��
     *    0���޴洢���ʣ�
     *    1��SM��SIM�����洢��
     *    2��ME��Flash���洢��
     * 3��smsServicePara[2]������״̬����洢�ϱ���ʽ��ȡֵͬindex 0��
     * 4��smsServicePara[3]������״̬������մ洢���ʣ�ȡֵͬindex 1��
     * 5��smsServicePara[4]���㲥���Ž��մ洢���ʣ�ȡֵͬindex 1��
     * 6��smsServicePara[5]
     *    Ӧ�õĶ��Ŵ洢״̬�����ݲ�Ʒ��̬���ơ�
     *    0��Ӧ�õĶ��Ŵ洢�ռ��������ã�
     *    1��Ӧ�õĶ��Ŵ洢�ռ������á�
     * 7��smsServicePara[6]
     *    smsServicePara[1]�д洢�Ľ��ն��Ŵ洢���������Ƿ���Ч��
     *    0����Ч���ϵ翪��ʱ���Ž��մ洢����������SM��
     *    1����Ч���ϵ翪��ʱ���Ž��մ洢�������ʹ�smsServicePara[1]��ȡ��
     * 8��smsServicePara[7]
     *    �����������
     *    0���Զ��������ŷ�����
     *    1��ֻ��+CGSMS����ָ�������з��Ͷ��š�
     */
    VOS_UINT8 smsServicePara[MN_MSG_SRV_PARAM_LEN];
} TAF_NVIM_SmsServicePara;


typedef struct {
    /*
     * 0��NV��رգ�
     * 1��NV��򿪡�
     */
    VOS_UINT8 roamFeatureFlg;
    /*
     * 0�����ڹ������δ򿪣�
     * 1���������δ򿪹������ιرգ�
     * 2���������ιرչ������δ򿪣�
     * 3���������ιرչ������ιرգ�
     * 4�����β��䡣
     */
    VOS_UINT8 roamCapability;
    VOS_UINT8 reserve1; /* ���� */
    VOS_UINT8 reserve2; /* ���� */
} NAS_NVIM_RoamCfgInfo;

/* NV_ITEM_CUSTOMIZE_SIM_LOCKPLMN_INFO 8267 */

typedef struct {
    VOS_UINT8 mncNum; /* �ַ��ͣ�Ĭ��0xaa */
    /*
     * �ַ���0������9�����Ժ������δ�������ε�λ��ȫ����Ϊ0x0A�����������˺���ε�δ�ﵽ15λ��λ��ȫ����Ϊ0x0F��
     * NV�е�ʵ��ֵ��Ӧ15λIMSI�Ŷλ�0x0A����0x0F��
     */
    VOS_UINT8 rangeBegin[TAF_PH_SIMLOCK_PLMN_STR_LEN];
    /*
     * �ַ���0������9�����Ժ������δ�������ε�λ��ȫ����Ϊ0x0A�����������˺���ε�δ�ﵽ15λ��λ��ȫ����Ϊ0x0F��
     * NV�е�ʵ��ֵ��Ӧ15λIMSI�Ŷλ�0x0A����0x0F��
     */
    VOS_UINT8 rangeEnd[TAF_PH_SIMLOCK_PLMN_STR_LEN];
} TAF_CUSTOM_SimLockPlmnRange;


typedef struct {
    VOS_UINT32                          status; /* NV���Ƿ񼤻��־��0�����1���� */
    /* ��¼SIM LOCKҪ���PLMN��Ϣ */
    TAF_CUSTOM_SimLockPlmnRange simLockPlmnRange[TAF_MAX_SIM_LOCK_RANGE_NUM];
} TAF_CUSTOM_SimLockPlmnInfo;

/* NV_ITEM_CARDLOCK_STATUS 8268 */

typedef struct {
    /*
     * 0��δ���
     * 1�����
     */
    VOS_UINT32 status;
    /*
     * 0��CARDLOCK_CARD_UNLOCKCODE_NEEDED�����ݿ���������ʹ�ã�Ŀǰ��ʱû��ʹ�á�
     * 1��CARDLOCK_CARD_UNLOCKED�����ݿ�û�б�������������Ҫ����<unlock_code>��
     * 2��CARDLOCK_CARD_LOCKED�����ݿ��Ѿ�����������������ʹ�ã�����ǺϷ���IMSI���ϱ�����̨��Ҳ�����ֵ��
     * 3��CARDLOCK_MAX�����ݿ��Ѿ������������ܹ�ʹ�÷Ƿ���SIM����
     */
    VOS_UINT32 cardlockStatus;
    VOS_UINT32 remainUnlockTimes; /* ʣ���������������Ϊ������0��4294967295��2��32�η���1�� */
} TAF_NVIM_CustomCardLockStatus;

/* NV_ITEM_CUSTOMIZE_SIM_LOCK_MAX_TIMES 8269 */

typedef struct {
    /*
     * 0��δ���
     * 1�����
     */
    VOS_UINT32 status;
    VOS_UINT32 lockMaxTimes; /* ����������������Ϊ������0��10 */
} TAF_CUSTOM_SimLockMaxTimes;


typedef struct {
    /*
     * 0��δ���
     * 1�����
     */
    VOS_UINT32 status;
    /*
     * �Ƿ�ʹ����ͨGSM��CDMA����ҵ��
     * 0����ֹ��
     * 1��ʹ�ܡ�
     */
    VOS_UINT32 customizeService;
} NAS_NVIM_CustomizeService;


typedef struct {
    /*
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 ucStatus;
    /*
     * WINSʹ�ܱ��
     * 0����ֹ��
     * 1��ʹ�ܡ�
     */
    VOS_UINT8 ucWins;
    VOS_UINT8 reserve1; /* ���� */
    VOS_UINT8 reserve2; /* ���� */
} WINS_Config;


typedef struct {
    /*
     * ����״̬��
     * 0��δ���
     * 1�����
     * -1����֧�֡�
     */
    VOS_INT8 status;
    /*
     * URL�ַ����ı��뷽ʽ��
     * 1��8bit���룻
     * 2��UCS2���룻
     */
    VOS_UINT8  encodeType;
    VOS_UINT8  reserved1;                                     /* ���� */
    VOS_UINT8  reserved2;                                     /* ���� */
    VOS_UINT32 length;                                        /* URL�ַ����ĳ��ȡ� */
    VOS_UINT8  data[TAF_NVIM_MSG_ACTIVE_MESSAGE_MAX_URL_LEN]; /* URL�ַ���������Ŀǰ֧�ֵ���󳤶�Ϊ160�ֽڡ� */
} TAF_AT_NvimSmsActiveMessage;


typedef struct {
    /*
     * ��NV�Ƿ���Ч��
     * 0����Ч��
     * 1����Ч��
     */
    VOS_UINT8 activeFlag;
    VOS_UINT8 svn[TAF_SVN_DATA_LENGTH]; /* IMEI SVN�ڶ������֣�ȡֵ��Χ0~9�� */
    VOS_UINT8 reserve[1];               /* �����ֽڡ� */
} TAF_SVN_Data;


typedef struct {
    /*
     * �Ƿ�֧��PC Voice���ܡ�
     * 0����֧�֣�������̬Ĭ��ֵ����
     * 1��֧�֣�STICK��HILINK��̬Ĭ��ֵ����
     */
    VOS_UINT16 pcVoiceSupportFlag;
    VOS_UINT8  reserve1; /* ���� */
    VOS_UINT8  reserve2; /* ���� */
} APP_VC_NvimPcVoiceSupportFalg;


typedef struct {
    /*
     * ME�洢���Ź���ʹ�ܱ�־��
     * 0����ʹ�ܣ�
     * 1��ʹ�ܡ�
     */
    MN_MSG_MeStorageStatusUint8 meStorageStatus;
    VOS_UINT8                           reserve[1]; /* ������ */
    /*
     * ME�洢������������MeStorageStatusʹ��ʱ��Ч��ȡֵ��Χ0~500����
     * ȡֵ=0��ME�洢�����޿��ÿռ䣻
     * ȡֵ>500��Э��ջǿ�ƽ�ME�洢�����޸�Ϊ500��
     */
    VOS_UINT16 meStorageNum;
} MN_MSG_MeStorageParm;


typedef struct {
    VOS_UINT8 ratOrderNum; /* ��ǰ�趨�Ľ��뼼��������ȡֵ��Χ1~3�� */
    /*
     * ��ǰϵͳ֧�ֵĽ��뼼���������ȼ��б�
     * 0��GSM��
     * 1��WCDMA��
     * 2��LTE��
     * 3��1X��
     * 4��HRPD��
     */
    VOS_UINT8 ratOrder[TAF_PH_NVIM_MAX_GUL_RAT_NUM];
} TAF_PH_NvimRatOrder;


typedef struct {
    VOS_UINT8 ratOrderNum; /* ��ǰ�趨֧�ֵĽ�����ʽ������ȡֵ��Χ1~4; */
    /*
     * ��ǰ֧�ֵĽ�����ʽ��
     * 0��GSM��
     * 1��WCDMA��
     * 2��LTE��
     * 3��1X��
     * 4��HRPD��
     * 5��NR;
     */
    VOS_UINT8 ratOrder[TAF_NVIM_MAX_RAT_ORDER_NUM];
    VOS_UINT8 reserved1; /* ���� */
    VOS_UINT8 reserved2; /* ���� */
    VOS_UINT8 reserved3; /* ���� */

} TAF_NVIM_RatOrderEx;


typedef struct {
    /*
     * LTE�������α�ǡ�
     * 0��LTE��ֹ���Σ�
     * 1��LTE�������Ρ�
     */
    VOS_UINT8 lteRoamAllowedFlg;
    VOS_UINT8 reserve[1];             /* ������ */
    VOS_UINT8 roamEnabledMccList[20]; /* �������εĹ������б� �� */
} NAS_MMC_NvimLteInternationalRoamCfg;


typedef struct {
    /*
     * NV��״̬��
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 status;
    /*
     * �Ƿ񼤻���������ܡ�
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 actFlg;
    VOS_UINT8 reserve1; /* ���� */
    VOS_UINT8 reserve2; /* ���� */
} NAS_PREVENT_TestImsiReg;


typedef struct {
    /*
     * ���Ž������̶��Ƽ����־��
     * 0��δ���
     * 1���Ѽ��
     */
    VOS_UINT8 actFlag;
    /*
     * ���Ž������̶������͡�
     * 0��δ���ƣ�
     * 1��DCM���ƣ�
     */
    VOS_UINT8 enMtCustomize;
    /*
     * ����Download���������У��Ƿ���ʱ����stk���š�
     * 0������ʱ
     * 1����ʱ
     */
    VOS_UINT8 mtCustomize[2];
} MN_MSG_MtCustomizeInfo;


typedef struct {
    /*
     * �źŸı���������
     * 0���ź������ı����޽����ʹ��Ĭ��ֵ��GASĬ��ֵ��3dB��WASĬ��ֵ��NV9067���ã�
     * 1���ź������ı䳬��1dB��������������ϱ��ź�������
     * 2���ź������ı䳬��2dB��������������ϱ��ź�������
     * 3���ź������ı䳬��3dB��������������ϱ��ź�������
     * 4���ź������ı䳬��4dB��������������ϱ��ź�������
     * 5���ź������ı䳬��5dB��������������ϱ��ź�������
     */
    VOS_UINT8 signThreshold;
    /*
     * 0���ϱ���չ�ź�������ʱ�����ƣ�
     * 1-20�������ϱ��ź���������С���ʱ�䣬��λ��s��
     */
    VOS_UINT8 minRptTimerInterval;
    VOS_UINT8 rerved1; /* ������ֵΪ0 */
    VOS_UINT8 rerved2; /* ������ֵΪ0 */
} NAS_NVIM_CellSignReportCfg;


typedef struct {
    /*
     * �Ƿ�ʹ�ܲ�Ʒ�߶��Ƶ�PDP����ԭ��ֵ��
     * 1��ʹ�ܣ�
     * 0����ֹ��
     */
    VOS_UINT32 activeFlag;
    VOS_UINT8  smCause[TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM]; /* ��Ʒ�߶��Ƶ�PDP����ԭ��ֵ�� */
} TAF_NV_Ipv6FallbackExtCause;


typedef struct {
    /*
     * ��NV�Ƿ���Ч��
     * 0����NV����Ч��
     * 1����NV��Ч��
     */
    VOS_UINT8 nvimValid;
    /*
     * ��ucNvimValidָʾ��NV��Чʱ�����ղ���AP��RP ACKʱ�������յ��Ķ����Ƿ���Ҫ���ش洢��
     * 0������Ҫ��
     * 1����Ҫ��
     * ������Ϊ1ʱ�������յ���MT���Ž��б��ش洢�������ϱ���AP�����ر��ϱ���AP��ͨ������CNMI�Ĳ���<mode>��<ds>�޸�Ϊ0��
     */
    VOS_UINT8 smsClosePathFlg;
} TAF_NVIM_SmsClosePathCfg;


typedef struct {
    VOS_UINT16 ratNum; /* ���뼼���ĸ����� */
    /*
     * ���뼼����Rat���͡�
     * 0��GSM��
     * 1��WCDMA��
     * 2��LTE��
     * 3��TDS��
     * 4��1X��
     * 5��HRPD��
     * 6��NR��
     */
    PLATFORM_RatTypeUint16 ratList[PLATFORM_MAX_RAT_NUM];
} PLATAFORM_RatCapability;


typedef struct {
    VOS_INT16 gprsPower; /* GSMƵ�ε�GPRS���书������ֵ����λΪdbm��ȡֵ��Χ15~33 */
    VOS_INT16 edgePower; /* GSMƵ�ε�EDGE���书������ֵ����λΪdbm��ȡֵ��Χ15~33 */
} MTA_BodySarGPara;


typedef struct {
    MTA_WcdmaBandUint16        band; /* WCDMAƵ�Σ�BandֵΪ1~9��11��19�� */
    VOS_INT16                  power; /* ��ӦWCDMAƵ�ε�����书������ֵ����λΪdbm��ȡֵ��Χ17~24�� */
} MTA_BodySarWPara;


typedef struct {
    /*
     * GSM ���ù������޵�BAND���룬�����Ϊ16���ƣ�ÿ��bit��Ӧһ��Ƶ�Σ�BitֵΪ1��ʾ�����ã�Ϊ0��ʾδ���á�
     * Bit0��GSM850(GPRS)��
     * Bit1��GSM900(GPRS)��
     * Bit2��GSM1800(GPRS)��
     * Bit3��GSM1900(GPRS)��
     * Bit16��GSM850(EDGE)��
     * Bit17��GSM900(EDGE)��
     * Bit18��GSM1800(EDGE)��
     * Bit19��GSM1900(EDGE)��
     */
    VOS_UINT32               gBandMask;
    VOS_UINT16               wBandNum;      /* WCDMA���ù������޵�BAND���������֧��5��Ƶ�Ρ� */
    VOS_UINT16               reserved1[1]; /* ������ */
    MTA_BodySarGPara gBandPara[MTA_BODY_SAR_GBAND_MAX_NUM]; /* GSMƵ�ζ�Ӧ��GPRS/EDGE��������ֵ�� */
    MTA_BodySarWPara wBandPara[MTA_BODY_SAR_WBAND_MAX_NUM]; /* WCDMA��Ӧ��Ƶ�μ���������ֵ�� */
} MTA_BodySarPara;


typedef struct {
    /*
     * �����������¶ȱ���������־λ��
     * 1��ʹ�ܣ�
     * 0����ֹ��
     */
    VOS_UINT8 eCallNotifySupport;
    /*
     * �±�״̬�ϱ�ʹ�ܱ�־λ��
     * 1��ʹ�ܣ�
     * 0����ֹ��
     */
    VOS_UINT8 spyStatusIndSupport;
    VOS_UINT8 reserved[2]; /* ������ */
} TAF_TempProtectConfig;


typedef struct {
    /*
     * ������ѡ��
     * 0����ʹ��IMS���ţ�
     * 1������ʹ��IMS����,CS/PS���ű�ѡ��
     */
    VOS_UINT8 smsDomain;
    VOS_UINT8 reserved[3]; /* �����ֶ� */
} TAF_NVIM_SmsDomain;


typedef struct {
    /*
     * Jam Detect���ܹ���ģʽ��
     * 0���ر�
     * 1����
     */
    VOS_UINT8 mode;
    /*
     * Jam Detectʹ�õķ�����Ŀǰֻ֧�ַ���2��
     * 1������1
     * 2������2
     */
    VOS_UINT8 method;
    VOS_UINT8 freqNum;            /* �����Ҫ�ﵽ��Ƶ�������ȡֵ��Χ[0-255]�� */
    VOS_UINT8 threshold;          /* �����Ҫ�ﵽ��Ƶ�����ֵ��ȡֵ��Χ[0-70]�� */
    VOS_UINT8 jamDetectingTmrLen; /* ǿ�ź�Ƶ������������޺󣬼��ͬ������Ķ�ʱ��ʱ������λ���롣 */
    VOS_UINT8 jamDetectedTmrLen;  /* ��⵽���ź󣬵ȴ�����������ʱ�䣬��λ���롣 */
    /*
     * �����������Ƿ���Ҫ�����ϱ���
     * 0����
     * 1����
     */
    VOS_UINT8 fastReportFlag;
    VOS_UINT8 rxlevThresholdOffset; /* Gģ������������ƫ��ֵ�� */
} NV_NAS_JamDetectCfg;


typedef struct {
    /*
     * ��Ƶ���ܿ��ء�
     * 0���رգ�
     * 1���򿪡�
     */
    VOS_UINT8                     enableFlg;
    TAF_NVIM_RatModeUint8  ratMode;    /* ����ģʽ�� */
    TAF_NVIM_GsmBandUint16 band;       /* GSMƵ�Ρ� */
    VOS_UINT32                    lockedFreq; /* ��ƵƵ�㡣 */
} TAF_NVIM_FreqLockCfg;

/*
 * ��    ʶ: NV_ITEM_TRAFFIC_CLASS_TYPE(50009)
 * �ṹ˵��: ���ڶ���PDP����������QoS��Traffic Class��ֵ
 */
typedef struct {
    /*
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 status;
    /*
     * Traffic Class��ֵ,������
     * 0��Subscribed value��
     * 1��Conversational class��
     * 2��Streaming Class��
     * 3��Interactive Class��
     * 4��Background Class��
     */
    VOS_UINT8 trafficClass;
    VOS_UINT8 reserve1; /* ����λ */
    VOS_UINT8 reserve2; /* ����λ */
} AT_TrafficClassCustomize;


typedef struct {
    /*
     * NV�����־λ
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 status;
    /*
     * USSD��Ϣ���䷽��, Ĭ��Ϊ͸��ģʽ��
     * 0��USSD��͸��������������֧�ֱ���룩��
     * 1��USSD͸�������������岻����룬ֻ��͸�����ɺ�̨������룩
     */
    VOS_UINT8 ussdTransMode;
    VOS_UINT8 reserve1; /* ����λ */
    VOS_UINT8 reserve2; /* ����λ */
} TAF_UssdNvimTransMode;


typedef struct {
    /*
     * ��NV���Ƿ񼤻
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 actFlg;
    /*
     * 0��Vodafone Class0���Ŷ��ƣ�
     * 1��Italy TIM Class0���Ŷ��ƣ���H3G Class0 ���Ŷ��ƣ���
     * 2��3GPPЭ��Ĵ���ʽ��
     * 3������VIVO���ƣ�CLASS0���������ֱ���ϱ����ݣ����ݶ���Э�����ͣ�ͨ��CSMS�����޸ģ�ȷ���Ƿ���Ӧ��ȷ�϶��Ž��գ���
     */
    VOS_UINT8 class0Tailor;
} MN_MSG_NvimClass0Tailor;


typedef struct {
    /*
     * ���ú��еȴ��Ŀ���ģʽ��
     * ccwaCtrlModeΪ0������еȴ���3gpp������ƣ�
     * ccwaCtrlModeΪ1������еȴ���UE���ƣ�����VOLTE�����硣
     * ����VoLTE�������ϣ�AP����CCWA֧��ʱ��IMS��û�к����罻����VoLTE�ĵ绰��CCWA��UE���ƣ���
     * ֧��VOLTE���նˣ�����VOLTE������û�д򿪣������·�CCWAI�����ú��еȴ���
     */
    VOS_UINT8 ccwaCtrlMode;
    /*
     * ���еȴ�����ģʽ
     * 0��disable
     * 1��enable
     */
    VOS_UINT8 ccwaiMode;
    VOS_UINT8 reserved1; /* Ԥ�� */
    VOS_UINT8 reserved2; /* Ԥ�� */
} TAF_CALL_NvimCcwaCtrlMode;


typedef struct {
    /*
     * CVHUģʽ����,�����Ƿ�֧��ATH�Ҷ�ͨ����
     * 0���·�ATH���ԹҶ�ͨ����
     * 1���·�ATHֻ�Ƿ���OK����Ӱ��ͨ����
     */
    AT_CvhuModeUint8        cvhuMode;
    VOS_UINT8               reserved1; /* Ԥ��λ */
    VOS_UINT8               reserved2; /* Ԥ��λ */
    VOS_UINT8               reserved3; /* Ԥ��λ */
} NAS_NVIM_CustomCallCfg;


typedef struct {
    /*
     * USIMM��SIMSQ���������ϱ����ܿ��ء�
     * 0���رա�
     * 1���򿪡�
     */
    VOS_UINT8 simsqEnable;
    VOS_UINT8 reserved1; /* Ԥ��λ */
    VOS_UINT8 reserved2; /* Ԥ��λ */
    VOS_UINT8 reserved3; /* Ԥ��λ */
} NAS_NVIM_CustomUsimmCfg;


typedef struct {
    VOS_UINT8 ucCustomCharNum; /* �����ַ�����������������16���ַ��� */
    /*
     * APNNI���ֿ�ͷ��β�ַ������ʹ���
     * 0�������
     * 1�����
     */
    VOS_UINT8 ucProtocolStringCheckFlag;
    VOS_UINT8 ucReserved1; /* ����λ */
    VOS_UINT8 ucReserved2; /* ����λ */
    VOS_UINT8 ucReserved3; /* ����λ */
    VOS_UINT8 ucReserved4; /* ����λ */
    VOS_UINT8 ucReserved5; /* ����λ */
    VOS_UINT8 ucReserved6; /* ����λ */
    /* �����ַ����飬��������ASCII���г������ַ���0-31��������б�ܣ�47,92���Լ�ɾ�����ţ�127��������ַ��� */
    VOS_UINT8 aucCustomChar[TAF_NVIM_APN_CUSTOM_CHAR_MAX_NUM];
} TAF_NVIM_ApnCustomFormatCfg;


typedef struct {
    /*
     * LTE��eCall������ѡģʽ
     * 0��ƽ̨Ĭ��ʵ�֡�
     * 1��LTE��eCall����ǿ��CSFB��CS����С�
     * 2��LTE��eCall��������ͨ���еĺ�����ѡ�����һ�¡���VoLTE�µ���eCall�ӿ�ʵ�����������ͨ���л�����ͨ�������С�
     * 3����VoLTE�µ���eCall�ӿڷ���3GPP R14��eCall over IMSҪ�󡣣�������
     */
    VOS_UINT8 ecallDomainMode;
    /*
     * eCallǿ��ģʽ
     * 0��Autoģʽ��UE����SIM�ļ�����������ģʽ������3GPPЭ��TS31.102��
     *    �����EFUST��eCall data (Service n��89) ��FDN (Service n��2) ����������EFEST��FDN��Service n��1��������
     *    ����eCall onlyģʽ���У�
     *    �����EFUST��eCall data (Service n��89) ��SDN (Service n��4) ����������eCall and normalģʽ���У����������
     *    ��normalģʽ���У�
     * 1��Force eCall Onlyģʽ�������ǲ����������SIM����UE����eCall onlyģʽ�����У�eCall onlyģʽ�²�����ע�����磬
     *    ����eCall inactive״̬��
     * 2��Force eCall and normalģʽ�������ǲ����������SIM����UE����eCall and normalģʽ�����У�eCall and normal
     *    ģʽ����ע�����磻
     */
    VOS_UINT8 ecallForceMode;
    VOS_UINT8 reserved1; /* ���� */
    VOS_UINT8 reserved2; /* ���� */
} TAF_NVIM_CustomEcallCfg;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __NV_STRU_GUCNAS_H__ */

