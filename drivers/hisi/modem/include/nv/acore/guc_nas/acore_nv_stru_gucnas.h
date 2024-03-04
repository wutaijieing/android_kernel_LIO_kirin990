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
#define ADS_UL_QUEUE_SCHEDULER_PRI_MAX 9 /* 优先级级别最大数 */
#define AT_WIFI_SSID_LEN_MAX 33          /* WIFI SSID KEY最大长度 */
#define AT_WIFI_KEY_LEN_MAX 27
#define AT_WIFI_WLAUTHMODE_LEN 16     /* 鉴权模式字符串长度 */
#define AT_WIFI_ENCRYPTIONMODES_LEN 5 /* 加密模式字符串长度 */
#define AT_WIFI_WLWPAPSK_LEN 65       /* WPA的密码字符串长度 */
#define AT_WIFI_MAX_SSID_NUM 4        /* 最多支持4组SSID */
#define AT_WIFI_KEY_NUM (AT_WIFI_MAX_SSID_NUM)
#define AT_MAX_ABORT_CMD_STR_LEN 16
#define AT_MAX_ABORT_RSP_STR_LEN 16
#define AT_PRODUCT_NAME_MAX_NUM 29
#define AT_PRODUCT_NAME_LENGHT (AT_PRODUCT_NAME_MAX_NUM + 1)
#define AT_WEBUI_PWD_MAX 16 /* WEB UI 密码最大长度 */
#define AT_AP_XML_RPT_SRV_URL_LEN 127
#define AT_AP_XML_RPT_SRV_URL_STR_LEN (AT_AP_XML_RPT_SRV_URL_LEN + 1)
#define AT_AP_NVIM_XML_RPT_INFO_TYPE_LEN 127
#define AT_AP_NVIM_XML_RPT_INFO_TYPE_STR_LEN (AT_AP_NVIM_XML_RPT_INFO_TYPE_LEN + 1)
#define RNIC_NVIM_NAPI_LB_MAX_LEVEL 16 /* 软中断负载均衡最大档位 */
#define RNIC_NVIM_NAPI_LB_MAX_CPUS 16  /* 软中断负载均衡最大CPU个数 */
#define RNIC_NVIM_MAX_CLUSTER 3        /* CPU最大CLUSTER个数 */
#define RNIC_NVIM_RHC_MAX_LEVEL 16     /* 高速控制最大档位 */

enum AT_UART_LinkType {
    AT_UART_LINK_TYPE_OM = 1, /* OM模式 */
    AT_UART_LINK_TYPE_AT = 2, /* AT模式 */
    AT_UART_LINK_TYPE_BUTT    /* 无效值 */
};
typedef VOS_UINT16 AT_UART_LinkTypeUint16;


enum NAPI_WEIGHT_AdjMode {
    NAPI_WEIGHT_ADJ_STATIC_MODE  = 0x00, /* 静态调整模式 */
    NAPI_WEIGHT_ADJ_DYNAMIC_MODE = 0x01, /* 动态调整模式 */

    NAPI_WEIGHT_ADJ_MODE_BUTT            /* 无效值 */
};
typedef VOS_UINT8 NAPI_WEIGHT_AdjModeUint8;

/*
 * 标    识: NV_ITEM_USB_ENUM_STATUS(21)
 * 结构说明: 单板的USB虚拟设备形态值，可以通过“at^u2diag=”来设置。
 */
typedef struct {
    /*
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT32 status;
    VOS_UINT32 value;    /* 产线配置。 */
    VOS_UINT32 reserve1; /* 保留 */
    VOS_UINT32 reserve2; /* 保留 */
} USB_ENUM_Status;


typedef struct {
    /*
     * Bit[0]：DRX Debug Flag；
     * Bit[1]：USIMM Debug Flag；
     * Bit[2] - Bit[31]：保留。
     */
    VOS_UINT32 commDebugFlag;
} TAF_AT_NvimCommdegbugCfg;

/*
 * 标    识: NV_ITEM_PID_ENABLE_TYPE(2601)
 * 结构说明: 不再使用。
 */
typedef struct {
    VOS_UINT32 pidEnabled; /* pid使能标记；0：不使能；1：使能 */
} NV_PID_EnableType;


typedef struct {
    /*
     * 0：烧片版本。
     * 1：正式版本。
     */
    VOS_UINT32 nvSwVerFlag;
} OM_SW_VerFlag;


typedef struct {
    VOS_INT8 ate5DissdPwd[AT_DISSD_PWD_LEN]; /* 字符串。 */
} TAF_AT_NvimDissdPwd;


typedef struct {
    VOS_INT8 atOpwordPwd[AT_OPWORD_PWD_LEN]; /* 字符串，默认用户已授权。 */
} TAF_AT_NvimDislogPwdNew;


typedef struct {
    /*
     * UART端口默认工作模式。
     * 1：OM；
     * 2：AT。
     */
    AT_UART_LinkTypeUint16  uartLinkType;
    VOS_UINT8                     reserve1; /* 保留 */
    VOS_UINT8                     reserve2; /* 保留 */
} TAF_AT_NvimDefaultLinkOfUart;

/*
 * 标    识: NV_ITEM_BATTERY_ADC(90)
 * 结构说明: 用于保存电池校准参数数据结构
 */
typedef struct {
    VOS_UINT16 minValue; /* 最小值 */
    VOS_UINT16 maxValue; /* 最大值 */
} VBAT_CALIBART_Type;


typedef struct {
    /* 字段0的定制内容，内容长度为128字节，每个字节用十进制表示，第129字节存储结束符。 */
    VOS_UINT8 factInfo1[AT_FACINFO_INFO1_STR_LENGTH];
    /* 字段1的定制内容，内容长度为128字节，每个字节用十进制表示，第258字节存储结束符。 */
    VOS_UINT8 factInfo2[AT_FACINFO_INFO2_STR_LENGTH];
    VOS_UINT8 reserve1; /* 保留 */
    VOS_UINT8 reserve2; /* 保留 */
} TAF_AT_NvimFactoryInfo;


typedef struct {
    /*
     * Yyyy-mm-dd hh-mm-ss格式存储的生产日期。
     * 年份固定为4位，月份，日期和小时，分钟，秒均为2位。不足2位时，左边补0。
     */
    VOS_UINT8 mDate[AT_MDATE_STRING_LENGTH];
} TAF_AT_NvimManufactureDate;


typedef struct {
    VOS_UINT32 smsRiOnInterval;    /* 短信RI高电平持续时间，单位为毫秒(ms)。 */
    VOS_UINT32 smsRiOffInterval;   /* 短信RI低电平持续时间，单位为毫秒(ms)。 */
    VOS_UINT32 voiceRiOnInterval;  /* 语音RI高电平持续时间，单位为毫秒(ms)。 */
    VOS_UINT32 voiceRiOffInterval; /* 语音RI低电平持续时间，单位为毫秒(ms)。 */
    VOS_UINT8  voiceRiCycleTimes;  /* 语音RI波形周期次数。 */
    VOS_UINT8  reserved[3];        /* 保留。 */
} TAF_NV_UartRi;


typedef struct {
    /*
     * 帧格式。取值范围：（1-6）
     * 1：8个数据位2个停止位
     * 2：8个数据位1个校验位1个停止位
     * 3：8个数据位1个停止位
     * 4：7个数据位2个停止位
     * 5：7个数据位1个校验位1个停止位
     * 6：7个数据为1个停止位
     */
    VOS_UINT8 format;
    /*
     * 校验算法。取值范围：（0-3）
     * 0：奇校验
     * 1：偶校验
     * 2：标记校验
     * 3：空校验
     */
    VOS_UINT8 parity;
    VOS_UINT8 reserved[2]; /* 保留 */

} TAF_NV_UartFrame;


typedef struct {
    /*
     * 波特率。取值范围：
     * 0, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800. 921600, 2764800, 4000000
     */
    VOS_UINT32             baudRate;
    TAF_NV_UartFrame frame;    /* UART帧格式。 */
    TAF_NV_UartRi    riConfig; /* UART Ring脚配置。 */
} TAF_NV_UartCfg;


typedef struct {
    /*
     * 短信读取缓存控制。
     * 0：无效；
     * 1：有效。
     */
    VOS_UINT8 enableFlg;
    VOS_UINT8 reserved[3]; /* 保留 */
} TAF_NV_PortBuffCfg;


typedef struct {
    /*
     * CDMAMODEMSWITCH不重启Modem特性是否启用。
     * 0：不启用，即切模后重启Modem；
     * 1：启用，即切模后不重启Modem。
     * 注意：
     * 1. 此特性由终端定制；
     * 2. 若启用特性，将不支持开机状态下发CdmaModemSwitch命令，也不支持在副卡下发CdmaModemSwitch命令。
     *    即在下发CdmaModemSwitch命令进行切模前，AP必须确保主副Modem均处于关机状态，且在主卡下发此命令。
     * 3. 若关闭此特性，切模后需重启Modem。
     */
    VOS_UINT8 enableFlg;
    VOS_UINT8 reversed[7]; /* 保留位，八字节对齐 */
} TAF_NVIM_CdmamodemswitchNotResetCfg;


typedef struct {
    VOS_UINT32 ipv6AddrTestModeCfg; /* 值为0x55AA55AA时，是测试模式，其他值为正常模式。 */
} TAF_NVIM_Ipv6AddrTestModeCfg;


typedef struct {
    /*
     * IPF处理ADS下行数据的模式。
     * 0：中断上下文；
     * 1：线程上下文。
     */
    VOS_UINT8 ipfMode;
    VOS_UINT8 reserved0; /* 保留位 */
    VOS_UINT8 reserved1; /* 保留位 */
    VOS_UINT8 reserved2; /* 保留位 */
} TAF_NV_AdsIpfModeCfg;


typedef struct {
    VOS_UINT8 info[128]; /* 单板制造信息，前16bytes记录单板SN号，其他为保留位。 */
} OM_ManufactureInfo;


typedef struct {
    /*
     * 持锁功能是否使能。
     * 1：使能
     * 0：禁止
     */
    VOS_UINT32 enable;
    VOS_UINT32 txWakeTimeout; /* 发送持锁时间，单位：毫秒。 */
    VOS_UINT32 rxWakeTimeout; /* 接收持锁时间，单位：毫秒。 */
    VOS_UINT32 reserved;      /* 保留字节。 */
} TAF_NV_AdsWakeLockCfg;


typedef struct {
    /*
     * 是否能够通过netlink socket输出modem log。
     * 0：不使能, default:0；
     * 1：使能。
     */
    VOS_UINT8 printModemLogType;
    VOS_UINT8 reserved0; /* 保留字节 */
    VOS_UINT8 reserved1; /* 保留字节 */
    VOS_UINT8 reserved2; /* 保留字节 */
} TAF_NV_PrintModemLogType;


typedef struct {
    /*
     * 内存块数量。
     * 注：BlkSize为448 bytes的内存块数最小为512；BlkSize为1540 bytes的内存块数最小768块。
     */
    VOS_UINT16 blkNum;
    VOS_UINT16 reserved0; /* 保留字节。 */
    VOS_UINT16 reserved1; /* 保留字节。 */
    VOS_UINT16 reserved2; /* 保留字节。 */

} TAF_NV_AdsMemCfg;


typedef struct {
    /*
     * 免CAHCE功能是否使能。
     * 1：使能；
     * 0：禁止。
     */
    VOS_UINT32 enable;
    /*
     * memCfg[0]（BlkSize为448 bytes的内存配置）
     * memCfg[1]（BlkSize为1540 bytes的内存配置）
     */
    TAF_NV_AdsMemCfg memCfg[2];
} TAF_NV_AdsMemPoolCfg;


typedef struct {
    /*
     * 反馈功能是否使能。
     * 1：使能
     * 0：禁止
     */
    VOS_UINT32 enabled;
    /* 错包率阈值，取值范围[50,100]，单位：百分比。 */
    VOS_UINT32 errorRateThreshold;  /* 错包率阈值，取值范围[50,100]，单位：百分比。 */
    VOS_UINT32 detectDuration;      /* 错包检测持续时间，取值范围[1000,10000]，单位：毫秒。*/
    VOS_UINT32 reserved;            /* 保留字节 */

} TAF_NV_AdsErrorFeedbackCfg;


typedef struct {
    /*
     * 用户是否可以通过AT+CFUN=1,1直接触发整机复位。
     * 0xEF：可以
     * 其他：不可以
     */
    VOS_UINT8 userRebootConfig;
    VOS_UINT8 reserved1; /* 保留字段 */
    VOS_UINT8 reserved2; /* 保留字段 */
    VOS_UINT8 reserved3; /* 保留字段 */
} TAF_NVIM_UserRebootSupport;


typedef struct {
    VOS_UINT32 dlPktNumPerSecLevel1; /* RNIC网卡每秒下行报文数档位1 */
    VOS_UINT32 dlPktNumPerSecLevel2; /* RNIC网卡每秒下行报文数档位2 */
    VOS_UINT32 dlPktNumPerSecLevel3; /* RNIC网卡每秒下行报文数档位3 */
    VOS_UINT32 dlPktNumPerSecLevel4; /* RNIC网卡每秒下行报文数档位4 */
    VOS_UINT32 dlPktNumPerSecLevel5; /* RNIC网卡每秒下行报文数档位5 */
    VOS_UINT32 reserved0;            /* 保留位 */
    VOS_UINT32 reserved1;            /* 保留位 */
    VOS_UINT32 reserved2;            /* 保留位 */

} RNIC_NAPI_DlPktNumPerSecLevel;


typedef struct {
    VOS_UINT8 napiWeightLevel1;  // RNIC网卡对应每秒下行报文数NAPI Weight值档位1
    VOS_UINT8 napiWeightLevel2;  // RNIC网卡对应每秒下行报文数NAPI Weight值档位2
    VOS_UINT8 napiWeightLevel3;  // RNIC网卡对应每秒下行报文数NAPI Weight值档位3
    VOS_UINT8 napiWeightLevel4;  // RNIC网卡对应每秒下行报文数NAPI Weight值档位4
    VOS_UINT8 napiWeightLevel5;  // RNIC网卡对应每秒下行报文数NAPI Weight值档位5
    VOS_UINT8 reserved0;         // 保留位
    VOS_UINT8 reserved1;         // 保留位
    VOS_UINT8 reserved2;         // 保留位

} RNIC_NAPI_WeightLevel;


typedef struct {
    RNIC_NAPI_DlPktNumPerSecLevel dlPktNumPerSecLevel; /* RNIC网卡每秒下行报文数档位 */
    RNIC_NAPI_WeightLevel             napiWeightLevel;     /* RNIC网卡对应每秒下行报文数NAPI Weight值档位 */

} RNIC_NAPI_WeightDynamicAdjCfg;


typedef struct {
    /*
     * RNIC网卡下行数据到Linux网络协议栈的接口模式：
     * 0：Net_rx接口
     * 1：NAPI接口
     */
    VOS_UINT8 napiEnable;
    /*
     * NAPI Weight调整模式：
     * 0：静态模式
     * 1：动态调整模式
     */
    NAPI_WEIGHT_AdjModeUint8  napiWeightAdjMode;
    /* RNIC网卡NAPI方式一次poll的最大报文数 */
    VOS_UINT8                       napiPollWeight;
    /*
     * GRO开启：
     * 0：不开启
     * 1：开启
     */
    VOS_UINT8                             groEnable;
    VOS_UINT16                            napiPollQueMaxLen;       /* RNIC Poll队列支持的最大长度 */
    VOS_UINT8                             reserved1;               /* 保留位 */
    VOS_UINT8                             reserved2;               /* 保留位 */
    RNIC_NAPI_WeightDynamicAdjCfg napiWeightDynamicAdjCfg; /* Napi Weight动态调整配置 */

} TAF_NV_RnicNapiCfg;


typedef struct {
    VOS_UINT32 pps;                                      /* RNIC网卡下行每秒收包个数档位值 */
    VOS_UINT8  cpusWeight[RNIC_NVIM_NAPI_LB_MAX_CPUS];  /* 参与NAPI负载均衡的CPU Weight值档位 */

} RNIC_NAPI_LbLevelCfg;


typedef struct {
    VOS_UINT8                   napiLbEnable;                                 /* NAPI负载均衡功能开关 */
    VOS_UINT8                   napiLbValidLevel;                             /* Napi负载均衡有效档位 */
    VOS_UINT16                  napiLbCpumask;                                /* 参与Napi负载均衡的CPU掩码 */
    VOS_UINT8                   reserved0;                                    /* 保留位 */
    VOS_UINT8                   reserved1;                                    /* 保留位 */
    VOS_UINT8                   reserved2;                                    /* 保留位 */
    VOS_UINT8                   reserved3;                                    /* 保留位 */
    VOS_UINT32                  reserved4;                                    /* 保留位 */
    VOS_UINT32                  reserved5;                                    /* 保留位 */
    RNIC_NAPI_LbLevelCfg napiLbLevelCfg[RNIC_NVIM_NAPI_LB_MAX_LEVEL]; /* Napi负载均衡档位配置 */

} TAF_NV_RnicNapiLbCfg;


typedef struct {
    VOS_UINT32 totalPps;                       /* RNIC网卡下行每秒收包个数档位值 */
    VOS_UINT32 nonTcpPps;                      /* RNIC网卡下行每秒收非tcp包个数档位值 */
    VOS_UINT32 backlogQueLimitLen;             /* CPU backlog que限长 */
    VOS_UINT8  congestionCtrl;                 /* 拥塞控制开关 */
    VOS_UINT8  rpsBitMask;                     /* RPS CPU掩码 */
    VOS_UINT8  reserved1;                      /* 保留位 */
    VOS_UINT8  isolationDisable;               /* 关闭isolation标识 */
    VOS_UINT32 freqReq[RNIC_NVIM_MAX_CLUSTER]; /* 请求调整CLUSTER主频值 */

} RNIC_RhcLevel;


typedef struct {
    VOS_UINT8           rhcFeature;                        /* 接收高速控制特性开关 */
    VOS_UINT8           reserved1;                         /* 保留位 */
    VOS_UINT8           reserved2;                         /* 保留位 */
    VOS_UINT8           reserved3;                         /* 保留位 */
    VOS_UINT32          reserved4;                         /* 保留位 */
    RNIC_RhcLevel levelCfg[RNIC_NVIM_RHC_MAX_LEVEL]; /* 接收高速控制档位配置 */

} TAF_NV_RnicRhcCfg;


typedef struct {
    /*
     * 其中每个字节的BIT0-BIT1对应一client归属归属于哪个Modem Id。
     * 00:表示Modem0
     * 01:表示Modem1
     * 10:表示Modem2。
     * BIT2对应一个client是否允许广播:
     * 0:表示不允许
     * 1:表示允许。
     * 最多支持96个通道
     */
    VOS_UINT8 atClientConfig[TAF_AT_NVIM_CLIENT_CFG_LEN];
} TAF_AT_NvimAtClientCfg;


typedef struct {
    VOS_UINT8 gmmInfo[2]; /* Gmm信息 */
    VOS_UINT8 reserve1;    /* 保留 */
    VOS_UINT8 reserve2;    /* 保留 */
} TAF_AT_NvGmmInfo;


typedef struct {
    /*
     * 0：用户没有设置接收分集；
     * 1：用户设置过接收分集。
     */
    VOS_UINT8 vaild;
    VOS_UINT8 reserved1; /* 保留。 */
    VOS_UINT8 reserved2; /* 保留。 */
    VOS_UINT8 reserved3; /* 保留。 */
} TAF_AT_NvimRxdivConfig;


typedef struct {
    /*
     * 0：不上报（手机以外形态默认值）；
     * 1：上报（手机形态的默认值）。
     */
    VOS_UINT16 reportRegActFlg;
    VOS_UINT8  reserve1; /* 保留 */
    VOS_UINT8  reserve2; /* 保留 */
} TAF_AT_NvimReportRegActFlg;

/*
 * 标    识: NV_ITEM_NDIS_DHCP_DEF_LEASE_TIME(2635)
 * 结构说明: 此NV已废弃。
 */
typedef struct {
    VOS_UINT32 dhcpLeaseHour; /* Range:[0x1,0x2250] */
} NDIS_NV_DhcpLeaseHour;


typedef struct {
    /*
     * 此NV项是否激活。
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 status;
    /*
     * CREG/CGREG的<CI>域上报字节数
     * 0：<CI>域以2字节方式上报；
     * 1：<CI>域以4字节方式上报。
     */
    VOS_UINT8 ciBytesRpt;
    VOS_UINT8 reserve1;                  /* 保留 */
    VOS_UINT8 reserve2;                  /* 保留 */
} NAS_NV_CregCgregCiFourByteRpt; /* VDF需求: CREG/CGREG命令<CI>域是否以4字节上报的NV项控制结构体 */


typedef struct {
    VOS_INT8 tz[AT_NVIM_SETZ_LEN]; /* 字符串。 */
} TAF_AT_Tz;


typedef struct {
    VOS_INT8 errorText[AT_NOTSUPPORT_STR_LEN]; /* 字符串。 */
} TAF_AT_NotSupportCmdErrorText;


typedef struct {
    /*
     * 0：PCUI口打开；
     * 1：PCUI口关闭。
     */
    VOS_UINT32 rightOpenFlg;
    VOS_INT8   password[AT_NVIM_RIGHT_PWD_LEN]; /* 开启PCUI通道的密码字符串。 */
} TAF_AT_NvimRightOpenFlag;


typedef struct {
    /* IPV6 MTU 信息, 单位为字节  */
    VOS_UINT32 ipv6RouterMtu;
} TAF_NDIS_NvIpv6RouterMtu;

/*
 * 标    识: NV_ITEM_IPV6_CAPABILITY(8514)
 * 结构说明: 用于读取读取IPV6能力相关信息, 目前IPV6能力只能支持到IPV4V6_OVER_ONE_PDP
 */
typedef struct {
    /*
     * 此NV是否有效标志。
     * 0：无效；
     * 1：有效。
     */
    VOS_UINT8 status;
    /*
     * 1：IPv4 only；
     * 2：IPv6 only；
     * 4：IPV4V6 support enabled over one IPV4V6 context (fallbacking on 2 single address PDP contexts if necessary)；
     * 8：IPV4V6 support enabled over 2 single address PDP contexts（暂不支持）。
     */
    VOS_UINT8 ipv6Capablity;
    VOS_UINT8 reversed[2]; /* 保留。 */
} AT_NV_Ipv6Capability;


typedef struct {
    VOS_UINT8 gsmConnectRate;   /* GSM连接速率，单位：MB/s。 */
    VOS_UINT8 gprsConnectRate;  /* Gprs连接速率，单位：MB/s。 */
    VOS_UINT8 edgeConnectRate;  /* Edge连接速率，单位：MB/s。 */
    VOS_UINT8 wcdmaConnectRate; /* Wcdma连接速率，单位：MB/s。 */
    VOS_UINT8 dpaConnectRate;   /* DPA连接速率，单位：MB/s。 */
    VOS_UINT8 reserve1;         /* 保留 */
    VOS_UINT8 reserve2;         /* 保留 */
    VOS_UINT8 reserve3;         /* 保留 */
} AT_NVIM_DialConnectDisplayRate;


typedef struct {
    /*
     * 是否使能ADS_Queue_Scheduler_Pri特性。
     * 0：未使能，表示不采用优先级,先激活先处理；
     * 1：采用优先级算法。
     */
    VOS_UINT32 status;
    VOS_UINT16 priWeightedNum[ADS_UL_QUEUE_SCHEDULER_PRI_MAX]; /* ADS上行队列优先级的加权值。 */
    VOS_UINT8  rsv[2];                                         /* 保留位 */
} ADS_UL_QueueSchedulerPriNv;


typedef struct {
    VOS_UINT32 opbSupport : 1;
    VOS_UINT32 opgSupport : 1;
    VOS_UINT32 opnSupport : 1;
    VOS_UINT32 opSpare : 29;
    VOS_UINT8  bHighChannel; /* 802.11b制式支持最大信道号，取值范围0~14 */
    VOS_UINT8  bLowChannel;  /* 802.11b制式支持最小信道号，取值范围0~14 */
    VOS_UINT8  rsv1[2];     /* 预留，暂未使用 */
    /*
     * 802.11b制式下的期望功率，取值范围0~65535。
     * bPower[0]：PA模式期望功率；
     * bPower[1]：NO PA模式期望功率。
     */
    VOS_UINT16 bPower[2];
    VOS_UINT8  gHighChannel; /* 802.11g制式支持最大信道号，取值范围0~14 */
    VOS_UINT8  gLowChannel;  /* 802.11g制式支持最小信道号，取值范围0~14 */
    VOS_UINT8  rsv2[2];     /* 预留，暂未使用 */
    /*
     * 802.11g制式下的期望功率，取值范围0~65535。
     * gPower[0]：PA模式期望功率；
     * gPower[1]：NO PA模式期望功率。
     */
    VOS_UINT16 gPower[2];   /* 预留，暂未使用 */
    VOS_UINT8  nHighChannel; /* 802.11n制式支持最大信道号，取值范围0~14 */
    VOS_UINT8  nLowChannel;  /* 802.11n制式支持最小信道号，取值范围0~14 */
    VOS_UINT8  rsv3[2];     /* 预留，暂未使用 */
    /*
     * 802.11n制式下的期望功率，取值范围0~65535。
     * nPower[0]：PA模式期望功率；
     * nPower[1]：NO PA模式期望功率。
     */
    VOS_UINT16 nPower[2];
} AT_WIFI_Info;

/*
 * 标    识: NV_ITEM_WIFI_INFO(2656)
 * 结构说明: 用于存储WIFI支持模式、信道号、期望功率等信息。
 */
typedef struct {
    /*
     * BIT 0单板是否支持802.11b制式。
     * 0：不支持；
     * 1：支持。
     * BIT 1单板是否支持802.11g制式。
     * 0：不支持；
     * 1：支持。
     * BIT 2单板是否支持802.11n制式。
     * 0：不支持；
     * 1：支持。
     * BIT 4~BIT 31预留，暂未使用。
     */
    VOS_UINT32 opSupport;
    VOS_UINT8  bHighChannel; /* 802.11b制式支持最大信道号，取值范围0~14。 */
    VOS_UINT8  bLowChannel;  /* 802.11b制式支持最小信道号，取值范围0~14。 */
    VOS_UINT8  rsv1[2];     /* 预留，暂未使用。 */
    /*
     * 802.11b制式下的期望功率，取值范围0~65535。
     * bPower[0]：PA模式期望功率；
     * bPower[1]：NO PA模式期望功率。
     */
    VOS_UINT16 bPower[2];
    VOS_UINT8  gHighsChannel; /* 802.11g制式支持最大信道号，取值范围0~14。 */
    VOS_UINT8  gLowChannel;   /* 802.11g制式支持最小信道号，取值范围0~14。 */
    VOS_UINT8  rsv2[2];      /* 预留，暂未使用。 */
    /*
     * 802.11g制式下的期望功率，取值范围0~65535。
     * gPower[0]：PA模式期望功率；
     * gPower[1]：NO PA模式期望功率。
     */
    VOS_UINT16 gPower[2];
    VOS_UINT8  nHighsChannel; /* 802.11n制式支持最大信道号，取值范围0~14。 */
    VOS_UINT8  nLowChannel;   /* 802.11n制式支持最小信道号，取值范围0~14。 */
    VOS_UINT8  rsv3[2];     /* 预留，暂未使用。 */
    /*
     * 802.11n制式下的期望功率，取值范围0~65535。
     * nPower[0]：PA模式期望功率；
     * nPower[1]：NO PA模式期望功率。
     */
    VOS_UINT16 nPower[2];
} NV_WIFI_Info;


typedef struct {
    VOS_UINT16 platform; /* 子平台信息，取值范围0~65535。 */
    VOS_UINT8  reserve1; /* 保留 */
    VOS_UINT8  reserve2; /* 保留 */
} NAS_NVIM_Platform;


typedef struct {
    VOS_UINT16 eqver;    /* 装备归一化AT命令的版本号，取值范围0~65535。 */
    VOS_UINT8  reserve1; /* 保留 */
    VOS_UINT8  reserve2; /* 保留 */
} TAF_AT_EqVer;


typedef struct {
    /*
     * 数据卡定制版本号，取值范围0~65535。
     * 默认值由产品线定制。
     */
    VOS_UINT16 csver;
    VOS_UINT8  reserve1; /* 保留位 */
    VOS_UINT8  reserve2; /* 保留位 */
} TAF_NVIM_CsVer;


typedef struct {
    /*
     * 是否支持MUX特性。
     * 0：不支持（HILINK默认值）；
     * 1：支持。
     */
    VOS_UINT8 muxSupportFlg;
    VOS_UINT8 reserved1; /* 保留位。 */
    VOS_UINT8 reserved2; /* 保留位。 */
    VOS_UINT8 reserved3; /* 保留位。 */
} TAF_AT_NvimMuxSupportFlg;


typedef struct {
    VOS_UINT8 wifiAuthmode[AT_WIFI_WLAUTHMODE_LEN];                   /* 鉴权模式。 */
    VOS_UINT8 wifiBasicencryptionmodes[AT_WIFI_ENCRYPTIONMODES_LEN];  /* 基本加密模式。 */
    VOS_UINT8 wifiWpaencryptionmodes[AT_WIFI_ENCRYPTIONMODES_LEN];    /* WPA加密模式。 */
    VOS_UINT8 wifiWepKey1[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY1。 */
    VOS_UINT8 wifiWepKey2[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY2。 */
    VOS_UINT8 wifiWepKey3[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY3。 */
    VOS_UINT8 wifiWepKey4[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX]; /* WIFI KEY4。 */
    VOS_UINT8 wifiWepKeyIndex[AT_WIFI_MAX_SSID_NUM]; /* 使用的WIFI KEY INDEX。例如，1代表使用wlKeys1 */
    VOS_UINT8 wifiWpapsk[AT_WIFI_MAX_SSID_NUM][AT_WIFI_WLWPAPSK_LEN]; /* WPA的密码。 */
    VOS_UINT8 wifiWpsenbl; /* WPS是否使能。0：不使能；1：使能 */
    VOS_UINT8 wifiWpscfg;  /* 是否允许寄存器改变enrollee的参数。0: 不允许(默认); 1:允许 */
} TAF_AT_MultiWifiSec;


typedef struct {
    /* 该数组表示四组WIFI SSID，每组最大长度为33。 */
    VOS_UINT8 wifiSsid[AT_WIFI_MAX_SSID_NUM][AT_WIFI_SSID_LEN_MAX];
    VOS_UINT8 reserved[84]; /* 保留 */
} TAF_AT_MultiWifiSsid;


typedef struct {
    /*
     * 打断参数有效标志。
     * 1：有效
     * 0：无效
     */
    VOS_UINT8 abortEnableFlg;
    VOS_UINT8 reserve1; /* 保留，值为0。 */
    VOS_UINT8 reserve2; /* 保留，值为0。 */
    VOS_UINT8 reserve3; /* 保留，值为0。 */
    /* 打断命令的命令名，AT输入此命令表示请求打断当前正执行的命令。需有字符串结束符号。 */
    VOS_UINT8 abortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN];
    /* 打断命令的返回结果，AT返回此结果表示打断操作完成。需有字符串结束符。 */
    VOS_UINT8 abortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN];
} AT_NVIM_AbortCmdPara;


typedef struct {
    VOS_UINT32 muxReportCfg; /* MUX 端口状态，值0表示所有MUX端口均能够主动上报AT命令。 */
} TAF_AT_NvimMuxReportCfg;


typedef struct {
    /*
     * 控制是否允许某个或某些AT端口使用AT+CIMI命令查询IMSI。
     * Bit位为0：允许。
     * Bit位为1：不允许。
     */
    VOS_UINT32 cimiPortCfg;
} TAF_AT_NvimCimiPortCfg;


typedef struct {
    /*
     * NV项激活标志
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT8 status;
    /*
     * SS标准命令定制参数
     * BIT0：控制+CCWA查询命令是否输出完整业务状态。
     * 0：不输出；
     * 1：输出；
     * BIT1：控制+CLCK查询命令输出完整业务状态。
     * 0：不输出；
     * 1：输出；
     * 其他BIT位保留。
     */
    VOS_UINT8 ssCmdCustomize;
    VOS_UINT8 reserved1[2]; /* 保留 */
} AT_SS_CustomizePara;


typedef struct {
    /*
     * 是否使能Share PDP特性。
     * 1：使能；
     * 0：禁止。
     */
    VOS_UINT8 enableFlag;
    VOS_UINT8 reserved; /* 保留。 */
    /*
     * 老化时间，单位为秒(s)。
     * 在Share PDP特性使能的情况下，设置为0代表不老化，设置为非0值则代表老化周期。
     */
    VOS_UINT16 agingTimeLen;
} TAF_NVIM_SharePdpInfo;


typedef struct {
    VOS_UINT32 waterLevel1; /* 水线界别1 */
    VOS_UINT32 waterLevel2; /* 水线界别2 */
    VOS_UINT32 waterLevel3; /* 水线界别3 */
    VOS_UINT32 waterLevel4; /* 水线界别4,预留 */
} ADS_UL_WaterMarkLevel;


typedef struct {
    VOS_UINT32 threshold1; /* 赞包门限1 */
    VOS_UINT32 threshold2; /* 赞包门限2 */
    VOS_UINT32 threshold3; /* 赞包门限3 */
    VOS_UINT32 threshold4; /* 赞包门限4 */
} ADS_UL_ThresholdLevel;


typedef struct {
    VOS_UINT32                   activeFlag;       /* 使能标识。0：去使能，1：使能 */
    VOS_UINT32                   protectTmrExpCnt; /* 保护定时器超时计数时长，单位：秒。 */
    ADS_UL_WaterMarkLevel waterMarkLevel;   /* 数据包水线界别。 */
    ADS_UL_ThresholdLevel  thresholdLevel;   /* 动态组包门限。 */
    VOS_UINT32                   reserved[6];     /* 保留字段 */
} ADS_NV_DynamicThreshold;


typedef struct {
    /*
     * VCOM log的打印级别，分为INFO、NORM、ERR和DEBUG,其中：
     * INFO: 1
     * NORMAL:2
     * ERR:4
     * DEBUG:7
     */
    VOS_UINT32 appVcomDebugLevel;
    /*
     * APP VCOM 端口掩码，每1bit的值代表一个端口是否打印
     * 0: 不打印
     * 1: 打印
     */
    VOS_UINT32 appVcomPortIdMask1; /* VCOM端口ID掩码 */
    VOS_UINT32 appVcomPortIdMask2; /* VCOM端口ID掩码 */

    /*
     * DMS log的打印级别，分为INFO、WARNING、ERROR和DEBUG,其中：
     * INFO:    1
     * WARNING: 2
     * ERROR:   4
     * DEBUG:   7
     */
    VOS_UINT32 dmsDebugLevel;
    /*
     * DMS 端口掩码，每1bit的值代表一个端口是否打印
     * 0: 不打印
     * 1: 打印
     */
    VOS_UINT32 dmsPortIdMask1; /* 第0到31个bit分别对应DMS_PortId中的第0到第31个端口 */
    VOS_UINT32 dmsPortIdMask2; /* 第0到31个bit分别对应DMS_PortId中的第32到第63个端口 */
} TAF_NV_PortDebugCfg;


typedef struct {
    VOS_UINT8 macAddr[32]; /* MAC地址。 */
} OM_MAC_Addr;


typedef struct {
    /*
     * NV是否激活标识
     * *0：NV项未激活；
     *  1：NV项已激活。
     */
    VOS_UINT32 nvStatus;
    VOS_UINT8  productId[AT_PRODUCT_NAME_LENGHT]; /* 产品名称，ASIC编码方式，“\0”结束。 */
    VOS_UINT8  reserve1;                           /* 保留 */
    VOS_UINT8  reserve2;                           /* 保留 */
} TAF_AT_ProductId;


typedef struct {
    /*
     * NV是否有效标识
     * 0：有效；
     * 1：无效。
     */
    VOS_UINT8 status;
    /*
     * 拨号失败错误码上报标志。
     * 0：不上报，使用系统默认的拨号错误提示，使用AT^GLASTERR=1查询返回ERROR。
     * 1：上报，使用AT^GLASTERR=1查询本次拨号失败的错误码。
     */
    VOS_UINT8 errCodeRpt;
} NAS_NV_PppDialErrCode; /* 巴西TIM拨号错误码NV项结构体 */

/*
 * 标    识: NV_ITEM_HUAWEI_DYNAMIC_PID_TYPE(50091)
 * 结构说明: 用于获取当前设备的端口状态。
 */
typedef struct {
    /*
     * 本NV项是否激活。
     * 0：未激活；
     * 1：激活。
     */
    VOS_UINT32 nvStatus;
    VOS_UINT8  firstPortStyle[17];  /* 设备切换前端口形态, 取值范围0x00~0xFF。 */
    VOS_UINT8  rewindPortStyle[17]; /* 设备切换后端口形态, 取值范围0x00~0xFF。 */
    VOS_UINT8  reserved[22];        /* 保留位 */
} AT_DynamicPidType;


typedef struct {
    VOS_UINT8 webPwd[AT_WEBUI_PWD_MAX]; /* 字符串类型，WEBUI登录密码。 */
} TAF_AT_NvimWebAdminPassword;


typedef struct {
    /* 保存XML reporting服务器的URL，最后一个字节为结束符’\0’。 */
    VOS_UINT8 apRptSrvUrl[AT_AP_XML_RPT_SRV_URL_STR_LEN];
} TAF_AT_NvimApRptSrvUrl;


typedef struct {
    /* 保存XML reporting的信息类型，最后一个字节为结束符’\0’。 */
    VOS_UINT8 apXmlInfoType[AT_AP_NVIM_XML_RPT_INFO_TYPE_STR_LEN];
} TAF_AT_NvimApXmlInfoType;


typedef struct {
    /*
     * 0：关闭XML reporting特性；
     * 1：打开XML reporting特性。
     */
    VOS_UINT8 apXmlRptFlg;
    VOS_UINT8 reserve[3]; /* 保留位，未使用 */
} TAF_AT_NvimApXmlRptFlg;


typedef struct {
    /*
     * 整型值，控制当^IMSSWITCH命令未携带<utran_enable>参数时，此参数值是否与<lte_enable>关联，取值范围0~1。
     * 0：否
     * 1：是
     */
    VOS_UINT8 utranRelationLteFlg;
    /*
     * 整型值，控制当^IMSSWITCH命令未携带<gsm_enable>参数时，此参数值是否与<lte_enable>关联，取值范围0~1。
     * 0：否
     * 1：是
     */
    VOS_UINT8 gsmRelationLteFlg;
    VOS_UINT8 reserved1; /* 预留位。 */
    VOS_UINT8 reserved2; /* 预留位。 */
} TAF_NV_ImsSwitchRatRelationCfg;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __ACORE_NV_STRU_GUCNAS_H__ */
