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

#ifndef __ACORE_NV_STRU_GUCNAS_H__
#define __ACORE_NV_STRU_GUCNAS_H__

#include "vos.h"
#include "acore_nv_id_gucnas.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(push, 4)

#define AT_MDATE_STRING_LENGTH 20
#define TAF_AT_NVIM_CLIENT_CFG_LEN 128
#define AT_DISSD_PWD_LEN 16
#define AT_OPWORD_PWD_LEN 16
#define AT_FACINFO_INFO1_LENGTH 128
#define AT_FACINFO_INFO2_LENGTH 128
#define AT_FACINFO_STRING_LENGTH ((AT_FACINFO_INFO1_LENGTH + 1) + (AT_FACINFO_INFO2_LENGTH + 1))
#define AT_FACINFO_INFO1_STR_LENGTH (AT_FACINFO_INFO1_LENGTH + 1)
#define AT_FACINFO_INFO2_STR_LENGTH (AT_FACINFO_INFO2_LENGTH + 1)
#define AT_NVIM_SETZ_LEN 16
#define AT_NOTSUPPORT_STR_LEN 16
#define AT_NVIM_RIGHT_PWD_LEN 16
#define ADS_UL_QUEUE_SCHEDULER_PRI_MAX 9 /* ���ȼ���������� */
#define AT_WIFI_SSID_LEN_MAX 33          /* WIFI SSID KEY��󳤶� */
#define AT_WIFI_KEY_LEN_MAX 27
#define AT_WIFI_WLAUTHMODE_LEN 16     /* ��Ȩģʽ�ַ������� */
#define AT_WIFI_ENCRYPTIONMODES_LEN 5 /* ����ģʽ�ַ������� */
#define AT_WIFI_WLWPAPSK_LEN 65       /* WPA�������ַ������� */
#define AT_WIFI_MAX_SSID_NUM 4        /* ���֧��4��SSID */
#define AT_WIFI_KEY_NUM (AT_WIFI_MAX_SSID_NUM)
#define AT_MAX_ABORT_CMD_STR_LEN 16
#define AT_MAX_ABORT_RSP_STR_LEN 16
#define AT_PRODUCT_NAME_MAX_NUM 29
#define AT_PRODUCT_NAME_LENGHT (AT_PRODUCT_NAME_MAX_NUM + 1)
#define AT_WEBUI_PWD_MAX 16 /* WEB UI ������󳤶� */
#define AT_AP_XML_RPT_SRV_URL_LEN 127
#define AT_AP_XML_RPT_SRV_URL_STR_LEN (AT_AP_XML_RPT_SRV_URL_LEN + 1)
#define AT_AP_NVIM_XML_RPT_INFO_TYPE_LEN 127
#define AT_AP_NVIM_XML_RPT_INFO_TYPE_STR_LEN (AT_AP_NVIM_XML_RPT_INFO_TYPE_LEN + 1)
#define RNIC_NVIM_NAPI_LB_MAX_LEVEL 16 /* ���жϸ��ؾ������λ */
#define RNIC_NVIM_NAPI_LB_MAX_CPUS 16  /* ���жϸ��ؾ������CPU���� */
#define RNIC_NVIM_MAX_CLUSTER 3        /* CPU���CLUSTER���� */
#define RNIC_NVIM_RHC_MAX_LEVEL 16     /* ���ٿ������λ */

enum AT_UART_LinkType {
    AT_UART_LINK_TYPE_OM = 1, /* OMģʽ */
    AT_UART_LINK_TYPE_AT = 2, /* ATģʽ */
    AT_UART_LINK_TYPE_BUTT    /* ��Чֵ */
};
typedef VOS_UINT16 AT_UART_LinkTypeUint16;


enum NAPI_WEIGHT_AdjMode {
    NAPI_WEIGHT_ADJ_STATIC_MODE  = 0x00, /* ��̬����ģʽ */
    NAPI_WEIGHT_ADJ_DYNAMIC_MODE = 0x01, /* ��̬����ģʽ */

    NAPI_WEIGHT_ADJ_MODE_BUTT            /* ��Чֵ */
};
typedef VOS_UINT8 NAPI_WEIGHT_AdjModeUint8;

/*
 * ��    ʶ: NV_ITEM_USB_ENUM_STATUS(21)
 * �ṹ˵��: �����USB�����豸��ֵ̬������ͨ����at^u2diag=�������á�
 */
typedef struct {
    /*
     * 0��δ���
     * 1�����
     */
    VOS_UINT32 status;
    VOS_UINT32 value;    /* �������á� */
    VOS_UINT32 reserve1; /* ���� */
    VOS_UINT32 reserve2; /* ���� */
} USB_ENUM_Status;


typedef struct {
    /*
     * Bit[0]��DRX Debug Flag��
     * Bit[1]��USIMM Debug Flag��
     * Bit[2] - Bit[31]��������
     */
    VOS_UINT32 commDebugFlag;
} TAF_AT_NvimCommdegbugCfg;

/*
 * ��    ʶ: NV_ITEM_PID_ENABLE_TYPE(2601)
 * �ṹ˵��: ����ʹ�á�
 */
typedef struct {
    VOS_UINT32 pidEnabled; /* pidʹ�ܱ�ǣ�0����ʹ�ܣ�1��ʹ�� */
} NV_PID_EnableType;


typedef struct {
    /*
     * 0����Ƭ�汾��
     * 1����ʽ�汾��
     */
    VOS_UINT32 nvSwVerFlag;
} OM_SW_VerFlag;


typedef struct {
    VOS_INT8 ate5DissdPwd[AT_DISSD_PWD_LEN]; /* �ַ����� */
} TAF_AT_NvimDissdPwd;


typedef struct {
    VOS_INT8 atOpwordPwd[AT_OPWORD_PWD_LEN]; /* �ַ�����Ĭ���û�����Ȩ�� */
} TAF_AT_NvimDislogPwdNew;


typedef struct {
    /*
     * UART�˿�Ĭ�Ϲ���ģʽ��
     * 1��OM��
     * 2��AT��
     */
    AT_UART_LinkTypeUint16  uartLinkType;
    VOS_UINT8                     reserve1; /* ���� */
    VOS_UINT8                     reserve2; /* ���� */
} TAF_AT_NvimDefaultLinkOfUart;

/*
 * ��    ʶ: NV_ITEM_BATTERY_ADC(90)
 * �ṹ˵��: ���ڱ�����У׼�������ݽṹ
 */
typedef struct {
    VOS_UINT16 minValue; /* ��Сֵ */
    VOS_UINT16 maxValue; /* ���ֵ */
} VBAT_CALIBART_Type;


typedef struct {
    /* �ֶ�0�Ķ������ݣ����ݳ���Ϊ128�ֽڣ�ÿ���ֽ���ʮ���Ʊ�ʾ����129�ֽڴ洢�������� */
    VOS_UINT8 factInfo1[AT_FACINFO_INFO1_STR_LENGTH];
    /* �ֶ�1�Ķ������ݣ����ݳ���Ϊ128�ֽڣ�ÿ���ֽ���ʮ���Ʊ�ʾ����258�ֽڴ洢�������� */
    VOS_UINT8 factInfo2[AT_FACINFO_INFO2_STR_LENGTH];
    VOS_UINT8 reserve1; /* ���� */
    VOS_UINT8 reserve2; /* ���� */
} TAF_AT_NvimFactoryInfo;


typedef struct {
    /*
     * Yyyy-mm-dd hh-mm-ss��ʽ�洢���������ڡ�
     * ��ݹ̶�Ϊ4λ���·ݣ����ں�Сʱ�����ӣ����Ϊ2λ������2λʱ����߲�0��
     */
    VOS_UINT8 mDate[AT_MDATE_STRING_LENGTH];
} TAF_AT_NvimManufactureDate;


typedef struct {
    VOS_UINT32 smsRiOnInterval;    /* ����RI�ߵ�ƽ����ʱ�䣬��λΪ����(ms)�� */
    VOS_UINT32 smsRiOffInterval;   /* ����RI�͵�ƽ����ʱ�䣬��λΪ����(ms)�� */
    VOS_UINT32 voiceRiOnInterval;  /* ����RI�ߵ�ƽ����ʱ�䣬��λΪ����(ms)�� */
    VOS_UINT32 voiceRiOffInterval; /* ����RI�͵�ƽ����ʱ�䣬��λΪ����(ms)�� */
    VOS_UINT8  voiceRiCycleTimes;  /* ����RI�������ڴ����� */
    VOS_UINT8  reserved[3];        /* ������ */
} TAF_NV_UartRi;


typedef struct {
    /*
     * ֡��ʽ��ȡֵ��Χ����1-6��
     * 1��8������λ2��ֹͣλ
     * 2��8������λ1��У��λ1��ֹͣλ
     * 3��8������λ1��ֹͣλ
     * 4��7������λ2��ֹͣλ
     * 5��7������λ1��У��λ1��ֹͣλ
     * 6��7������Ϊ1��ֹͣλ
     */
    VOS_UINT8 format;
    /*
     * У���㷨��ȡֵ��Χ����0-3��
     * 0����У��
     * 1��żУ��
     * 2�����У��
     * 3����У��
     */
    VOS_UINT8 parity;
    VOS_UINT8 reserved[2]; /* ���� */

} TAF_NV_UartFrame;


typedef struct {
    /*
     * �����ʡ�ȡֵ��Χ��
     * 0, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800. 921600, 2764800, 4000000
     */
    VOS_UINT32             baudRate;
    TAF_NV_UartFrame frame;    /* UART֡��ʽ�� */
    TAF_NV_UartRi    riConfig; /* UART Ring�����á� */
} TAF_NV_UartCfg;


typedef struct {
    /*
     * ���Ŷ�ȡ������ơ�
     * 0����Ч��
     * 1����Ч��
     */
    VOS_UINT8 enableFlg;
    VOS_UINT8 reserved[3]; /* ���� */
} TAF_NV_PortBuffCfg;


typedef struct {
    /*
     * CDMAMODEMSWITCH������Modem�����Ƿ����á�
     * 0�������ã�����ģ������Modem��
     * 1�����ã�����ģ������Modem��
     * ע�⣺
     * 1. ���������ն˶��ƣ�
     * 2. ���������ԣ�����֧�ֿ���״̬�·�CdmaModemSwitch���Ҳ��֧���ڸ����·�CdmaModemSwitch���
     *    �����·�CdmaModemSwitch���������ģǰ��AP����ȷ������Modem�����ڹػ�״̬�����������·������
     * 3. ���رմ����ԣ���ģ��������Modem��
     */
    VOS_UINT8 enableFlg;
    VOS_UINT8 reversed[7]; /* ����λ�����ֽڶ��� */
} TAF_NVIM_CdmamodemswitchNotResetCfg;


typedef struct {
    VOS_UINT32 ipv6AddrTestModeCfg; /* ֵΪ0x55AA55AAʱ���ǲ���ģʽ������ֵΪ����ģʽ�� */
} TAF_NVIM_Ipv6AddrTestModeCfg;


typedef struct {
    /*
     * IPF����ADS�������ݵ�ģʽ��
     * 0���ж������ģ�
     * 1���߳������ġ�
     */
    VOS_UINT8 ipfMode;
    VOS_UINT8 reserved0; /* ����λ */
    VOS_UINT8 reserved1; /* ����λ */
    VOS_UINT8 reserved2; /* ����λ */
} TAF_NV_AdsIpfModeCfg;


typedef struct {
    VOS_UINT8 info[128]; /* ����������Ϣ��ǰ16bytes��¼����SN�ţ�����Ϊ����λ�� */
} OM_ManufactureInfo;


typedef struct {
    /*
     * ���������Ƿ�ʹ�ܡ�
     * 1��ʹ��
     * 0����ֹ
     */
    VOS_UINT32 enable;
    VOS_UINT32 txWakeTimeout; /* ���ͳ���ʱ�䣬��λ�����롣 */
    VOS_UINT32 rxWakeTimeout; /* ���ճ���ʱ�䣬��λ�����롣 */
    VOS_UINT32 reserved;      /* �����ֽڡ� */
} TAF_NV_AdsWakeLockCfg;


typedef struct {
    /*
     * �Ƿ��ܹ�ͨ��netlink socket���modem log��
     * 0����ʹ��, default:0��
     * 1��ʹ�ܡ�
     */
    VOS_UINT8 printModemLogType;
    VOS_UINT8 reserved0; /* �����ֽ� */
    VOS_UINT8 reserved1; /* �����ֽ� */
    VOS_UINT8 reserved2; /* �����ֽ� */
} TAF_NV_PrintModemLogType;


typedef struct {
    /*
     * �ڴ��������
     * ע��BlkSizeΪ448 bytes���ڴ������СΪ512��BlkSizeΪ1540 bytes���ڴ������С768�顣
     */
    VOS_UINT16 blkNum;
    VOS_UINT16 reserved0; /* �����ֽڡ� */
    VOS_UINT16 reserved1; /* �����ֽڡ� */
    VOS_UINT16 reserved2; /* �����ֽڡ� */

} TAF_NV_AdsMemCfg;


typedef struct {
    /*
     * ��CAHCE�����Ƿ�ʹ�ܡ�
     * 1��ʹ�ܣ�
     * 0����ֹ��
     */
    VOS_UINT32 enable;
    /*
     * memCfg[0]��BlkSizeΪ448 bytes���ڴ����ã�
     * memCfg[1]��BlkSizeΪ1540 bytes���ڴ����ã�
     */
    TAF_NV_AdsMemCfg memCfg[2];
} TAF_NV_AdsMemPoolCfg;


typedef struct {
    /*
     * ���������Ƿ�ʹ�ܡ�
     * 1��ʹ��
     * 0����ֹ
     */
    VOS_UINT32 enabled;
    /* �������ֵ��ȡֵ��Χ[50,100]����λ���ٷֱȡ� */
    VOS_UINT32 errorRateThreshold;  /* �������ֵ��ȡֵ��Χ[50,100]����λ���ٷֱȡ� */
    VOS_UINT32 detectDuration;      /* ���������ʱ�䣬ȡֵ��Χ[1000,10000]����λ�����롣*/
    VOS_UINT32 reserved;            /* �����ֽ� */

} TAF_NV_AdsErrorFeedbackCfg;


typedef struct {
    /*
     * �û��Ƿ����ͨ��AT+CFUN=1,1ֱ�Ӵ���������λ��
     * 0xEF������
     * ������������
     */
    VOS_UINT8 userRebootConfig;
    VOS_UINT8 reserved1; /* �����ֶ� */
    VOS_UINT8 reserved2; /* �����ֶ� */
    VOS_UINT8 reserved3; /* �����ֶ� */
} TAF_NVIM_UserRebootSupport;


typedef struct {
    VOS_UINT32 dlPktNumPerSecLevel1; /* RNIC����ÿ�����б�������λ1 */
    VOS_UINT32 dlPktNumPerSecLevel2; /* RNIC����ÿ�����б�������λ2 */
    VOS_UINT32 dlPktNumPerSecLevel3; /* RNIC����ÿ�����б�������λ3 */
    VOS_UINT32 dlPktNumPerSecLevel4; /* RNIC����ÿ�����б�������λ4 */
    VOS_UINT32 dlPktNumPerSecLevel5; /* RNIC����ÿ�����б�������λ5 */
    VOS_UINT32 reserved0;            /* ����λ */
    VOS_UINT32 reserved1;            /* ����λ */
    VOS_UINT32 reserved2;            /* ����λ */

} RNIC_NAPI_DlPktNumPerSecLevel;


typedef struct {
    VOS_UINT8 napiWeightLevel1;  // RNIC������Ӧÿ�����б�����NAPI Weightֵ��λ1
    VOS_UINT8 napiWeightLevel2;  // RNIC������Ӧÿ�����б�����NAPI Weightֵ��λ2
    VOS_UINT8 napiWeightLevel3;  // RNIC������Ӧÿ�����б�����NAPI Weightֵ��λ3
    VOS_UINT8 napiWeightLevel4;  // RNIC������Ӧÿ�����б�����NAPI Weightֵ��λ4
    VOS_UINT8 napiWeightLevel5;  // RNIC������Ӧÿ�����б�����NAPI Weightֵ��λ5
    VOS_UINT8 reserved0;         // ����λ
    VOS_UINT8 reserved1;         // ����λ
    VOS_UINT8 reserved2;         // ����λ

} RNIC_NAPI_WeightLevel;


typedef struct {
    RNIC_NAPI_DlPktNumPerSecLevel dlPktNumPerSecLevel; /* RNIC����ÿ�����б�������λ */
    RNIC_NAPI_WeightLevel             napiWeightLevel;     /* RNIC������Ӧÿ�����б�����NAPI Weightֵ��λ */

} RNIC_NAPI_WeightDynamicAdjCfg;


typedef struct {
    /*
     * RNIC�����������ݵ�Linux����Э��ջ�Ľӿ�ģʽ��
     * 0��Net_rx�ӿ�
     * 1��NAPI�ӿ�
     */
    VOS_UINT8 napiEnable;
    /*
     * NAPI Weight����ģʽ��
     * 0����̬ģʽ
     * 1����̬����ģʽ
     */
    NAPI_WEIGHT_AdjModeUint8  napiWeightAdjMode;
    /* RNIC����NAPI��ʽһ��poll��������� */
    VOS_UINT8                       napiPollWeight;
    /*
     * GRO������
     * 0��������
     * 1������
     */
    VOS_UINT8                             groEnable;
    VOS_UINT16                            napiPollQueMaxLen;       /* RNIC Poll����֧�ֵ���󳤶� */
    VOS_UINT8                             reserved1;               /* ����λ */
    VOS_UINT8                             reserved2;               /* ����λ */
    RNIC_NAPI_WeightDynamicAdjCfg napiWeightDynamicAdjCfg; /* Napi Weight��̬�������� */

} TAF_NV_RnicNapiCfg;


typedef struct {
    VOS_UINT32 pps;                                      /* RNIC��������ÿ���հ�������λֵ */
    VOS_UINT8  cpusWeight[RNIC_NVIM_NAPI_LB_MAX_CPUS];  /* ����NAPI���ؾ����CPU Weightֵ��λ */

} RNIC_NAPI_LbLevelCfg;


typedef struct {
    VOS_UINT8                   napiLbEnable;                                 /* NAPI���ؾ��⹦�ܿ��� */
    VOS_UINT8                   napiLbValidLevel;                             /* Napi���ؾ�����Ч��λ */
    VOS_UINT16                  napiLbCpumask;                                /* ����Napi���ؾ����CPU���� */
    VOS_UINT8                   reserved0;                                    /* ����λ */
    VOS_UINT8                   reserved1;                                    /* ����λ */
    VOS_UINT8                   reserved2;                                    /* ����λ */
    VOS_UINT8                   reserved3;                                    /* ����λ */
    VOS_UINT32                  reserved4;                                    /* ����λ */
    VOS_UINT32                  reserved5;                                    /* ����λ */
    RNIC_NAPI_LbLevelCfg napiLbLevelCfg[RNIC_NVIM_NAPI_LB_MAX_LEVEL]; /* Napi���ؾ��⵵λ���� */

} TAF_NV_RnicNapiLbCfg;


typedef struct {
    VOS_UINT32 totalPps;                       /* RNIC��������ÿ���հ�������λֵ */
    VOS_UINT32 nonTcpPps;                      /* RNIC��������ÿ���շ�tcp��������λֵ */
    VOS_UINT32 backlogQueLimitLen;             /* CPU backlog que�޳� */
    VOS_UINT8  congestionCtrl;                 /* ӵ�����ƿ��� */
    VOS_UINT8  rpsBitMask;                     /* RPS CPU���� */
    VOS_UINT8  reserved1;                      /* ����λ */
    VOS_UINT8  isolationDisable;               /* �ر�isolation��ʶ */
    VOS_UINT32 freqReq[RNIC_NVIM_MAX_CLUSTER]; /* �������CLUSTER��Ƶֵ */

} RNIC_RhcLevel;


typedef struct {
    VOS_UINT8           rhcFeature;                        /* ���ո��ٿ������Կ��� */
    VOS_UINT8           reserved1;                         /* ����λ */
    VOS_UINT8           reserved2;                         /* ����λ */
    VOS_UINT8           reserved3;                         /* ����λ */
    VOS_UINT32          reserved4;                         /* ����λ */
    RNIC_RhcLevel levelCfg[RNIC_NVIM_RHC_MAX_LEVEL]; /* ���ո��ٿ��Ƶ�λ���� */

} TAF_NV_RnicRhcCfg;


typedef struct {
    /*
     * ����ÿ���ֽڵ�BIT0-BIT1��Ӧһclient�����������ĸ�Modem Id��
     * 00:��ʾModem0
     * 01:��ʾModem1
     * 10:��ʾModem2��
     * BIT2��Ӧһ��client�Ƿ�����㲥:
     * 0:��ʾ������
     * 1:��ʾ����
     * ���֧��96��ͨ��
     */
    VOS_UINT8 atClientConfig[TAF_AT_NVIM_CLIENT_CFG_LEN];
} TAF_AT_NvimAtClientCfg;


typedef struct {
    VOS_UINT8 gmmInfo[2]; /* Gmm��Ϣ */
    VOS_UINT8 reserve1;    /* ���� */
    VOS_UINT8 reserve2;    /* ���� */
} TAF_AT_NvGmmInfo;


typedef struct {
    /*
     * 0���û�û�����ý��շּ���
     * 1���û����ù����շּ���
     */
    VOS_UINT8 vaild;
    VOS_UINT8 reserved1; /* ������ */
    VOS_UINT8 reserved2; /* ������ */
    VOS_UINT8 reserved3; /* ������ */
} TAF_AT_NvimRxdivConfig;


typedef struct {
    /*
     * 0�����ϱ����ֻ�������̬Ĭ��ֵ����
     * 1���ϱ����ֻ���̬��Ĭ��ֵ����
     */
    VOS_UINT16 reportRegActFlg;
    VOS_UINT8  reserve1; /* ���� */
    VOS_UINT8  reserve2; /* ���� */
} TAF_AT_NvimReportRegActFlg;

/*
 * ��    ʶ: NV_ITEM_NDIS_DHCP_DEF_LEASE_TIME(2635)
 * �ṹ˵��: ��NV�ѷ�����
 */
typedef struct {
    VOS_UINT32 dhcpLeaseHour; /* Range:[0x1,0x2250] */
} NDIS_NV_DhcpLeaseHour;


typedef struct {
    /*
     * ��NV���Ƿ񼤻
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 status;
    /*
     * CREG/CGREG��<CI>���ϱ��ֽ���
     * 0��<CI>����2�ֽڷ�ʽ�ϱ���
     * 1��<CI>����4�ֽڷ�ʽ�ϱ���
     */
    VOS_UINT8 ciBytesRpt;
    VOS_UINT8 reserve1;                  /* ���� */
    VOS_UINT8 reserve2;                  /* ���� */
} NAS_NV_CregCgregCiFourByteRpt; /* VDF����: CREG/CGREG����<CI>���Ƿ���4�ֽ��ϱ���NV����ƽṹ�� */


typedef struct {
    VOS_INT8 tz[AT_NVIM_SETZ_LEN]; /* �ַ����� */
} TAF_AT_Tz;


typedef struct {
    VOS_INT8 errorText[AT_NOTSUPPORT_STR_LEN]; /* �ַ����� */
} TAF_AT_NotSupportCmdErrorText;


typedef struct {
    /*
     * 0��PCUI�ڴ򿪣�
     * 1��PCUI�ڹرա�
     */
    VOS_UINT32 rightOpenFlg;
    VOS_INT8   password[AT_NVIM_RIGHT_PWD_LEN]; /* ����PCUIͨ���������ַ����� */
} TAF_AT_NvimRightOpenFlag;


typedef struct {
    /* IPV6 MTU ��Ϣ, ��λΪ�ֽ�  */
    VOS_UINT32 ipv6RouterMtu;
} TAF_NDIS_NvIpv6RouterMtu;

/*
 * ��    ʶ: NV_ITEM_IPV6_CAPABILITY(8514)
 * �ṹ˵��: ���ڶ�ȡ��ȡIPV6���������Ϣ, ĿǰIPV6����ֻ��֧�ֵ�IPV4V6_OVER_ONE_PDP
 */
typedef struct {
    /*
     * ��NV�Ƿ���Ч��־��
     * 0����Ч��
     * 1����Ч��
     */
    VOS_UINT8 status;
    /*
     * 1��IPv4 only��
     * 2��IPv6 only��
     * 4��IPV4V6 support enabled over one IPV4V6 context (fallbacking on 2 single address PDP contexts if necessary)��
     * 8��IPV4V6 support enabled over 2 single address PDP contexts���ݲ�֧�֣���
     */
    VOS_UINT8 ipv6Capablity;
    VOS_UINT8 reversed[2]; /* ������ */
} AT_NV_Ipv6Capability;


typedef struct {
    VOS_UINT8 gsmConnectRate;   /* GSM�������ʣ���λ��MB/s�� */
    VOS_UINT8 gprsConnectRate;  /* Gprs�������ʣ���λ��MB/s�� */
    VOS_UINT8 edgeConnectRate;  /* Edge�������ʣ���λ��MB/s�� */
    VOS_UINT8 wcdmaConnectRate; /* Wcdma�������ʣ���λ��MB/s�� */
    VOS_UINT8 dpaConnectRate;   /* DPA�������ʣ���λ��MB/s�� */
    VOS_UINT8 reserve1;         /* ���� */
    VOS_UINT8 reserve2;         /* ���� */
    VOS_UINT8 reserve3;         /* ���� */
} AT_NVIM_DialConnectDisplayRate;


typedef struct {
    /*
     * �Ƿ�ʹ��ADS_Queue_Scheduler_Pri���ԡ�
     * 0��δʹ�ܣ���ʾ���������ȼ�,�ȼ����ȴ���
     * 1���������ȼ��㷨��
     */
    VOS_UINT32 status;
    VOS_UINT16 priWeightedNum[ADS_UL_QUEUE_SCHEDULER_PRI_MAX]; /* ADS���ж������ȼ��ļ�Ȩֵ�� */
    VOS_UINT8  rsv[2];                                         /* ����λ */
} ADS_UL_QueueSchedulerPriNv;


typedef struct {
    VOS_UINT32 opbSupport : 1;
    VOS_UINT32 opgSupport : 1;
    VOS_UINT32 opnSupport : 1;
    VOS_UINT32 opSpare : 29;
    VOS_UINT8  bHighChannel; /* 802.11b��ʽ֧������ŵ��ţ�ȡֵ��Χ0~14 */
    VOS_UINT8  bLowChannel;  /* 802.11b��ʽ֧����С�ŵ��ţ�ȡֵ��Χ0~14 */
    VOS_UINT8  rsv1[2];     /* Ԥ������δʹ�� */
    /*
     * 802.11b��ʽ�µ��������ʣ�ȡֵ��Χ0~65535��
     * bPower[0]��PAģʽ�������ʣ�
     * bPower[1]��NO PAģʽ�������ʡ�
     */
    VOS_UINT16 bPower[2];
    VOS_UINT8  gHighChannel; /* 802.11g��ʽ֧������ŵ��ţ�ȡֵ��Χ0~14 */
    VOS_UINT8  gLowChannel;  /* 802.11g��ʽ֧����С�ŵ��ţ�ȡֵ��Χ0~14 */
    VOS_UINT8  rsv2[2];     /* Ԥ������δʹ�� */
    /*
     * 802.11g��ʽ�µ��������ʣ�ȡֵ��Χ0~65535��
     * gPower[0]��PAģʽ�������ʣ�
     * gPower[1]��NO PAģʽ�������ʡ�
     */
    VOS_UINT16 gPower[2];   /* Ԥ������δʹ�� */
    VOS_UINT8  nHighChannel; /* 802.11n��ʽ֧������ŵ��ţ�ȡֵ��Χ0~14 */
    VOS_UINT8  nLowChannel;  /* 802.11n��ʽ֧����С�ŵ��ţ�ȡֵ��Χ0~14 */
    VOS_UINT8  rsv3[2];     /* Ԥ������δʹ�� */
    /*
     * 802.11n��ʽ�µ��������ʣ�ȡֵ��Χ0~65535��
     * nPower[0]��PAģʽ�������ʣ�
     * nPower[1]��NO PAģʽ�������ʡ�
     */
    VOS_UINT16 nPower[2];
} AT_WIFI_Info;

/*
 * ��    ʶ: NV_ITEM_WIFI_INFO(2656)
 * �ṹ˵��: ���ڴ洢WIFI֧��ģʽ���ŵ��š��������ʵ���Ϣ��
 */
typedef struct {
    /*
     * BIT 0�����Ƿ�֧��802.11b��ʽ��
     * 0����֧�֣�
     * 1��֧�֡�
     * BIT 1�����Ƿ�֧��802.11g��ʽ��
     * 0����֧�֣�
     * 1��֧�֡�
     * BIT 2�����Ƿ�֧��802.11n��ʽ��
     * 0����֧�֣�
     * 1��֧�֡�
     * BIT 4~BIT 31Ԥ������δʹ�á�
     */
    VOS_UINT32 opSupport;
    VOS_UINT8  bHighChannel; /* 802.11b��ʽ֧������ŵ��ţ�ȡֵ��Χ0~14�� */
    VOS_UINT8  bLowChannel;  /* 802.11b��ʽ֧����С�ŵ��ţ�ȡֵ��Χ0~14�� */
    VOS_UINT8  rsv1[2];     /* Ԥ������δʹ�á� */
    /*
     * 802.11b��ʽ�µ��������ʣ�ȡֵ��Χ0~65535��
     * bPower[0]��PAģʽ�������ʣ�
     * bPower[1]��NO PAģʽ�������ʡ�
     */
    VOS_UINT16 bPower[2];
    VOS_UINT8  gHighsChannel; /* 802.11g��ʽ֧������ŵ��ţ�ȡֵ��Χ0~14�� */
    VOS_UINT8  gLowChannel;   /* 802.11g��ʽ֧����С�ŵ��ţ�ȡֵ��Χ0~14�� */
    VOS_UINT8  rsv2[2];      /* Ԥ������δʹ�á� */
    /*
     * 802.11g��ʽ�µ��������ʣ�ȡֵ��Χ0~65535��
     * gPower[0]��PAģʽ�������ʣ�
     * gPower[1]��NO PAģʽ�������ʡ�
     */
    VOS_UINT16 gPower[2];
    VOS_UINT8  nHighsChannel; /* 802.11n��ʽ֧������ŵ��ţ�ȡֵ��Χ0~14�� */
    VOS_UINT8  nLowChannel;   /* 802.11n��ʽ֧����С�ŵ��ţ�ȡֵ��Χ0~14�� */
    VOS_UINT8  rsv3[2];     /* Ԥ������δʹ�á� */
    /*
     * 802.11n��ʽ�µ��������ʣ�ȡֵ��Χ0~65535��
     * nPower[0]��PAģʽ�������ʣ�
     * nPower[1]��NO PAģʽ�������ʡ�
     */
    VOS_UINT16 nPower[2];
} NV_WIFI_Info;


typedef struct {
    VOS_UINT16 platform; /* ��ƽ̨��Ϣ��ȡֵ��Χ0~65535�� */
    VOS_UINT8  reserve1; /* ���� */
    VOS_UINT8  reserve2; /* ���� */
} NAS_NVIM_Platform;


typedef struct {
    VOS_UINT16 eqver;    /* װ����һ��AT����İ汾�ţ�ȡֵ��Χ0~65535�� */
    VOS_UINT8  reserve1; /* ���� */
    VOS_UINT8  reserve2; /* ���� */
} TAF_AT_EqVer;


typedef struct {
    /*
     * ���ݿ����ư汾�ţ�ȡֵ��Χ0~65535��
     * Ĭ��ֵ�ɲ�Ʒ�߶��ơ�
     */
    VOS_UINT16 csver;
    VOS_UINT8  reserve1; /* ����λ */
    VOS_UINT8  reserve2; /* ����λ */
} TAF_NVIM_CsVer;


typedef struct {
    /*
     * �Ƿ�֧��MUX���ԡ�
     * 0����֧�֣�HILINKĬ��ֵ����
     * 1��֧�֡�
     */
    VOS_UINT8 muxSupportFlg;
    VOS_UINT8 reserved1; /* ����λ�� */
    VOS_UINT8 reserved2; /* ����λ�� */
    VOS_UINT8 reserved3; /* ����λ�� */
} TAF_AT_NvimMuxSupportFlg;


typedef struct {
    VOS_UINT8 wifiAuthmode[AT_WIFI_WLAUTHMODE_LEN];                   /* ��Ȩģʽ�� */
    VOS_UINT8 wifiBasicencryptionmodes[AT_WIFI_ENCRYPTIONMODES_LEN];  /* ��������ģʽ�� */
    VOS_UINT8 wifiWpaencryptionmodes[AT_WIFI_ENCRYPTIONMODES_LEN];    /* WPA����ģʽ�� */
    VOS_UINT8 wifiWepKey1[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY1�� */
    VOS_UINT8 wifiWepKey2[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY2�� */
    VOS_UINT8 wifiWepKey3[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY3�� */
    VOS_UINT8 wifiWepKey4[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY4�� */
    VOS_UINT8 wifiWepKeyIndex[AT_WIFI_MAX_SSID_NUM]; /* ʹ�õ�WIFI KEY INDEX�����磬1����ʹ��wlKeys1 */
    VOS_UINT8 wifiWpapsk[AT_WIFI_MAX_SSID_NUM][AT_WIFI_WLWPAPSK_LEN]; /* WPA�����롣 */
    VOS_UINT8 wifiWpsenbl; /* WPS�Ƿ�ʹ�ܡ�0����ʹ�ܣ�1��ʹ�� */
    VOS_UINT8 wifiWpscfg;  /* �Ƿ�����Ĵ����ı�enrollee�Ĳ�����0: ������(Ĭ��); 1:���� */
} TAF_AT_MultiWifiSec;


typedef struct {
    /* �������ʾ����WIFI SSID��ÿ����󳤶�Ϊ33�� */
    VOS_UINT8 wifiSsid[AT_WIFI_MAX_SSID_NUM][AT_WIFI_SSID_LEN_MAX];
    VOS_UINT8 reserved[84]; /* ���� */
} TAF_AT_MultiWifiSsid;


typedef struct {
    /*
     * ��ϲ�����Ч��־��
     * 1����Ч
     * 0����Ч
     */
    VOS_UINT8 abortEnableFlg;
    VOS_UINT8 reserve1; /* ������ֵΪ0�� */
    VOS_UINT8 reserve2; /* ������ֵΪ0�� */
    VOS_UINT8 reserve3; /* ������ֵΪ0�� */
    /* ����������������AT����������ʾ�����ϵ�ǰ��ִ�е���������ַ����������š� */
    VOS_UINT8 abortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN];
    /* �������ķ��ؽ����AT���ش˽����ʾ��ϲ�����ɡ������ַ����������� */
    VOS_UINT8 abortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN];
} AT_NVIM_AbortCmdPara;


typedef struct {
    VOS_UINT32 muxReportCfg; /* MUX �˿�״̬��ֵ0��ʾ����MUX�˿ھ��ܹ������ϱ�AT��� */
} TAF_AT_NvimMuxReportCfg;


typedef struct {
    /*
     * �����Ƿ�����ĳ����ĳЩAT�˿�ʹ��AT+CIMI�����ѯIMSI��
     * BitλΪ0������
     * BitλΪ1��������
     */
    VOS_UINT32 cimiPortCfg;
} TAF_AT_NvimCimiPortCfg;


typedef struct {
    /*
     * NV����־
     * 0��δ���
     * 1�����
     */
    VOS_UINT8 status;
    /*
     * SS��׼����Ʋ���
     * BIT0������+CCWA��ѯ�����Ƿ��������ҵ��״̬��
     * 0���������
     * 1�������
     * BIT1������+CLCK��ѯ�����������ҵ��״̬��
     * 0���������
     * 1�������
     * ����BITλ������
     */
    VOS_UINT8 ssCmdCustomize;
    VOS_UINT8 reserved1[2]; /* ���� */
} AT_SS_CustomizePara;


typedef struct {
    /*
     * �Ƿ�ʹ��Share PDP���ԡ�
     * 1��ʹ�ܣ�
     * 0����ֹ��
     */
    VOS_UINT8 enableFlag;
    VOS_UINT8 reserved; /* ������ */
    /*
     * �ϻ�ʱ�䣬��λΪ��(s)��
     * ��Share PDP����ʹ�ܵ�����£�����Ϊ0�����ϻ�������Ϊ��0ֵ������ϻ����ڡ�
     */
    VOS_UINT16 agingTimeLen;
} TAF_NVIM_SharePdpInfo;


typedef struct {
    VOS_UINT32 waterLevel1; /* ˮ�߽��1 */
    VOS_UINT32 waterLevel2; /* ˮ�߽��2 */
    VOS_UINT32 waterLevel3; /* ˮ�߽��3 */
    VOS_UINT32 waterLevel4; /* ˮ�߽��4,Ԥ�� */
} ADS_UL_WaterMarkLevel;


typedef struct {
    VOS_UINT32 threshold1; /* �ް�����1 */
    VOS_UINT32 threshold2; /* �ް�����2 */
    VOS_UINT32 threshold3; /* �ް�����3 */
    VOS_UINT32 threshold4; /* �ް�����4 */
} ADS_UL_ThresholdLevel;


typedef struct {
    VOS_UINT32                   activeFlag;       /* ʹ�ܱ�ʶ��0��ȥʹ�ܣ�1��ʹ�� */
    VOS_UINT32                   protectTmrExpCnt; /* ������ʱ����ʱ����ʱ������λ���롣 */
    ADS_UL_WaterMarkLevel waterMarkLevel;   /* ���ݰ�ˮ�߽�� */
    ADS_UL_ThresholdLevel  thresholdLevel;   /* ��̬������ޡ� */
    VOS_UINT32                   reserved[6];     /* �����ֶ� */
} ADS_NV_DynamicThreshold;


typedef struct {
    /*
     * VCOM log�Ĵ�ӡ���𣬷�ΪINFO��NORM��ERR��DEBUG,���У�
     * INFO: 1
     * NORMAL:2
     * ERR:4
     * DEBUG:7
     */
    VOS_UINT32 appVcomDebugLevel;
    /*
     * APP VCOM �˿����룬ÿ1bit��ֵ����һ���˿��Ƿ��ӡ
     * 0: ����ӡ
     * 1: ��ӡ
     */
    VOS_UINT32 appVcomPortIdMask1; /* VCOM�˿�ID���� */
    VOS_UINT32 appVcomPortIdMask2; /* VCOM�˿�ID���� */

    /*
     * DMS log�Ĵ�ӡ���𣬷�ΪINFO��WARNING��ERROR��DEBUG,���У�
     * INFO:    1
     * WARNING: 2
     * ERROR:   4
     * DEBUG:   7
     */
    VOS_UINT32 dmsDebugLevel;
    /*
     * DMS �˿����룬ÿ1bit��ֵ����һ���˿��Ƿ��ӡ
     * 0: ����ӡ
     * 1: ��ӡ
     */
    VOS_UINT32 dmsPortIdMask1; /* ��0��31��bit�ֱ��ӦDMS_PortId�еĵ�0����31���˿� */
    VOS_UINT32 dmsPortIdMask2; /* ��0��31��bit�ֱ��ӦDMS_PortId�еĵ�32����63���˿� */
} TAF_NV_PortDebugCfg;


typedef struct {
    VOS_UINT8 macAddr[32]; /* MAC��ַ�� */
} OM_MAC_Addr;


typedef struct {
    /*
     * NV�Ƿ񼤻��ʶ
     * *0��NV��δ���
     *  1��NV���Ѽ��
     */
    VOS_UINT32 nvStatus;
    VOS_UINT8  productId[AT_PRODUCT_NAME_LENGHT]; /* ��Ʒ���ƣ�ASIC���뷽ʽ����\0�������� */
    VOS_UINT8  reserve1;                           /* ���� */
    VOS_UINT8  reserve2;                           /* ���� */
} TAF_AT_ProductId;


typedef struct {
    /*
     * NV�Ƿ���Ч��ʶ
     * 0����Ч��
     * 1����Ч��
     */
    VOS_UINT8 status;
    /*
     * ����ʧ�ܴ������ϱ���־��
     * 0�����ϱ���ʹ��ϵͳĬ�ϵĲ��Ŵ�����ʾ��ʹ��AT^GLASTERR=1��ѯ����ERROR��
     * 1���ϱ���ʹ��AT^GLASTERR=1��ѯ���β���ʧ�ܵĴ����롣
     */
    VOS_UINT8 errCodeRpt;
} NAS_NV_PppDialErrCode; /* ����TIM���Ŵ�����NV��ṹ�� */

/*
 * ��    ʶ: NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE(50091)
 * �ṹ˵��: ���ڻ�ȡ��ǰ�豸�Ķ˿�״̬��
 */
typedef struct {
    /*
     * ��NV���Ƿ񼤻
     * 0��δ���
     * 1�����
     */
    VOS_UINT32 nvStatus;
    VOS_UINT8  firstPortStyle[17];  /* �豸�л�ǰ�˿���̬, ȡֵ��Χ0x00~0xFF�� */
    VOS_UINT8  rewindPortStyle[17]; /* �豸�л���˿���̬, ȡֵ��Χ0x00~0xFF�� */
    VOS_UINT8  reserved[22];        /* ����λ */
} AT_DynamicPidType;


typedef struct {
    VOS_UINT8 webPwd[AT_WEBUI_PWD_MAX]; /* �ַ������ͣ�WEBUI��¼���롣 */
} TAF_AT_NvimWebAdminPassword;


typedef struct {
    /* ����XML reporting��������URL�����һ���ֽ�Ϊ��������\0���� */
    VOS_UINT8 apRptSrvUrl[AT_AP_XML_RPT_SRV_URL_STR_LEN];
} TAF_AT_NvimApRptSrvUrl;


typedef struct {
    /* ����XML reporting����Ϣ���ͣ����һ���ֽ�Ϊ��������\0���� */
    VOS_UINT8 apXmlInfoType[AT_AP_NVIM_XML_RPT_INFO_TYPE_STR_LEN];
} TAF_AT_NvimApXmlInfoType;


typedef struct {
    /*
     * 0���ر�XML reporting���ԣ�
     * 1����XML reporting���ԡ�
     */
    VOS_UINT8 apXmlRptFlg;
    VOS_UINT8 reserve[3]; /* ����λ��δʹ�� */
} TAF_AT_NvimApXmlRptFlg;


typedef struct {
    /*
     * ����ֵ�����Ƶ�^IMSSWITCH����δЯ��<utran_enable>����ʱ���˲���ֵ�Ƿ���<lte_enable>������ȡֵ��Χ0~1��
     * 0����
     * 1����
     */
    VOS_UINT8 utranRelationLteFlg;
    /*
     * ����ֵ�����Ƶ�^IMSSWITCH����δЯ��<gsm_enable>����ʱ���˲���ֵ�Ƿ���<lte_enable>������ȡֵ��Χ0~1��
     * 0����
     * 1����
     */
    VOS_UINT8 gsmRelationLteFlg;
    VOS_UINT8 reserved1; /* Ԥ��λ�� */
    VOS_UINT8 reserved2; /* Ԥ��λ�� */
} TAF_NV_ImsSwitchRatRelationCfg;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __ACORE_NV_STRU_GUCNAS_H__ */
