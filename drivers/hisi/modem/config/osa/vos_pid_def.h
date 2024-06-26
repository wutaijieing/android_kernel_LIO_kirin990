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



#ifndef _VOS_PID_DEF_H
#define _VOS_PID_DEF_H

#include "product_config.h"
#include "v_typdef.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/* 24到31bit保留，16到23bit是CPU ID，0到15bit是PID */
/* CPU0 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_0_DOPRAEND   (5)

/* macro defined for PID management */
#define BEGIN_CPU_ID_0_PID_DEFINITION() \
    enum VOS_CPU_ID_0_Process_ID \
    { \
        VOS_CPU_ID_0_PID_RESERVED = VOS_PID_CPU_ID_0_DOPRAEND - 1,

#define END_CPU_ID_0_PID_DEFINITION() \
        VOS_CPU_ID_0_PID_BUTT \
    };

/* CPU1 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_1_DOPRAEND   (65536)

/* macro defined for PID management */
#define BEGIN_CPU_ID_1_PID_DEFINITION() \
    enum VOS_CPU_ID_1_Process_ID \
    { \
        VOS_CPU_ID_1_PID_RESERVED = VOS_PID_CPU_ID_1_DOPRAEND - 1,

#define END_CPU_ID_1_PID_DEFINITION() \
        VOS_CPU_ID_1_PID_BUTT \
    };

/* CPU2 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_2_DOPRAEND   (131072)

/* macro defined for PID management */
#define BEGIN_CPU_ID_2_PID_DEFINITION() \
    enum VOS_CPU_ID_2_Process_ID \
    { \
        VOS_CPU_ID_2_PID_RESERVED = VOS_PID_CPU_ID_2_DOPRAEND - 1,

#define END_CPU_ID_2_PID_DEFINITION() \
        VOS_CPU_ID_2_PID_BUTT \
    };

/* CPU3 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_3_DOPRAEND   (196608)

/* macro defined for PID management */
#define BEGIN_CPU_ID_3_PID_DEFINITION() \
    enum VOS_CPU_ID_3_Process_ID \
    { \
        VOS_CPU_ID_3_PID_RESERVED = VOS_PID_CPU_ID_3_DOPRAEND - 1,

#define END_CPU_ID_3_PID_DEFINITION() \
        VOS_CPU_ID_3_PID_BUTT \
    };

/* CPU4 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_4_DOPRAEND   (262144)

/* macro defined for PID management */
#define BEGIN_CPU_ID_4_PID_DEFINITION() \
    enum VOS_CPU_ID_4_Process_ID \
    { \
        VOS_CPU_ID_4_PID_RESERVED = VOS_PID_CPU_ID_4_DOPRAEND - 1,

#define END_CPU_ID_4_PID_DEFINITION() \
        VOS_CPU_ID_4_PID_BUTT \
    };

/* CPU5 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_5_DOPRAEND   (327680)

/* macro defined for PID management */
#define BEGIN_CPU_ID_5_PID_DEFINITION() \
    enum VOS_CPU_ID_5_Process_ID \
    { \
        VOS_CPU_ID_5_PID_RESERVED = VOS_PID_CPU_ID_5_DOPRAEND - 1,

#define END_CPU_ID_5_PID_DEFINITION() \
        VOS_CPU_ID_5_PID_BUTT \
    };

/* CPU6 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_6_DOPRAEND   (393216)

/* macro defined for PID management */
#define BEGIN_CPU_ID_6_PID_DEFINITION() \
    enum VOS_CPU_ID_6_Process_ID \
    { \
        VOS_CPU_ID_6_PID_RESERVED = VOS_PID_CPU_ID_6_DOPRAEND - 1,

#define END_CPU_ID_6_PID_DEFINITION() \
        VOS_CPU_ID_6_PID_BUTT \
    };

/* CPU7 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_7_DOPRAEND   (458752)

/* macro defined for PID management */
#define BEGIN_CPU_ID_7_PID_DEFINITION() \
    enum VOS_CPU_ID_7_Process_ID \
    { \
        VOS_CPU_ID_7_PID_RESERVED = VOS_PID_CPU_ID_7_DOPRAEND - 1,

#define END_CPU_ID_7_PID_DEFINITION() \
        VOS_CPU_ID_7_PID_BUTT \
    };

/* CPU8 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_8_DOPRAEND   (524288)

/* macro defined for PID management */
#define BEGIN_CPU_ID_8_PID_DEFINITION() \
    enum VOS_CPU_ID_8_Process_ID \
    { \
        VOS_CPU_ID_8_PID_RESERVED = VOS_PID_CPU_ID_8_DOPRAEND - 1,

#define END_CPU_ID_8_PID_DEFINITION() \
        VOS_CPU_ID_8_PID_BUTT \
    };

/* CPU9 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_9_DOPRAEND   (589824)

/* macro defined for PID management */
#define BEGIN_CPU_ID_9_PID_DEFINITION() \
    enum VOS_CPU_ID_9_Process_ID \
    { \
        VOS_CPU_ID_9_PID_RESERVED = VOS_PID_CPU_ID_9_DOPRAEND - 1,

#define END_CPU_ID_9_PID_DEFINITION() \
        VOS_CPU_ID_9_PID_BUTT \
    };

/* CPU10 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_10_DOPRAEND   (655360)

/* macro defined for PID management */
#define BEGIN_CPU_ID_10_PID_DEFINITION() \
    enum VOS_CPU_ID_10_Process_ID \
    { \
        VOS_CPU_ID_10_PID_RESERVED = VOS_PID_CPU_ID_10_DOPRAEND - 1,

#define END_CPU_ID_10_PID_DEFINITION() \
        VOS_CPU_ID_10_PID_BUTT \
    };

/* CPU11 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_11_DOPRAEND   (720896)

/* macro defined for PID management */
#define BEGIN_CPU_ID_11_PID_DEFINITION() \
    enum VOS_CPU_ID_11_Process_ID \
    { \
        VOS_CPU_ID_11_PID_RESERVED = VOS_PID_CPU_ID_11_DOPRAEND - 1,

#define END_CPU_ID_11_PID_DEFINITION() \
        VOS_CPU_ID_11_PID_BUTT \
    };

/* CPU12 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_12_DOPRAEND   (786432)

/* macro defined for PID management */
#define BEGIN_CPU_ID_12_PID_DEFINITION() \
    enum VOS_CPU_ID_12_Process_ID \
    { \
        VOS_CPU_ID_12_PID_RESERVED = VOS_PID_CPU_ID_12_DOPRAEND - 1,

#define END_CPU_ID_12_PID_DEFINITION() \
        VOS_CPU_ID_12_PID_BUTT \
    };

/* CPU13 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_13_DOPRAEND   (851968)

/* macro defined for PID management */
#define BEGIN_CPU_ID_13_PID_DEFINITION() \
    enum VOS_CPU_ID_13_Process_ID \
    { \
        VOS_CPU_ID_13_PID_RESERVED = VOS_PID_CPU_ID_13_DOPRAEND - 1,

#define END_CPU_ID_13_PID_DEFINITION() \
        VOS_CPU_ID_13_PID_BUTT \
    };

/* CPU14 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_14_DOPRAEND   (917504)

/* macro defined for PID management */
#define BEGIN_CPU_ID_14_PID_DEFINITION() \
    enum VOS_CPU_ID_14_Process_ID \
    { \
        VOS_CPU_ID_14_PID_RESERVED = VOS_PID_CPU_ID_14_DOPRAEND - 1,

#define END_CPU_ID_14_PID_DEFINITION() \
        VOS_CPU_ID_14_PID_BUTT \
    };

/* CPU15 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_15_DOPRAEND   (983040)

/* macro defined for PID management */
#define BEGIN_CPU_ID_15_PID_DEFINITION() \
    enum VOS_CPU_ID_15_Process_ID \
    { \
        VOS_CPU_ID_15_PID_RESERVED = VOS_PID_CPU_ID_15_DOPRAEND - 1,

#define END_CPU_ID_15_PID_DEFINITION() \
        VOS_CPU_ID_15_PID_BUTT \
    };

/* CPU16 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_16_DOPRAEND   (1048576)

/* macro defined for PID management */
#define BEGIN_CPU_ID_16_PID_DEFINITION() \
    enum VOS_CPU_ID_16_Process_ID \
    { \
        VOS_CPU_ID_16_PID_RESERVED = VOS_PID_CPU_ID_16_DOPRAEND - 1,

#define END_CPU_ID_16_PID_DEFINITION() \
        VOS_CPU_ID_16_PID_BUTT \
    };

/* CPU17 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_17_DOPRAEND   (1114112)

/* macro defined for PID management */
#define BEGIN_CPU_ID_17_PID_DEFINITION() \
    enum VOS_CPU_ID_17_Process_ID \
    { \
        VOS_CPU_ID_17_PID_RESERVED = VOS_PID_CPU_ID_17_DOPRAEND - 1,

#define END_CPU_ID_17_PID_DEFINITION() \
        VOS_CPU_ID_17_PID_BUTT \
    };

/* CPU18 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_18_DOPRAEND   (1179648)

/* macro defined for PID management */
#define BEGIN_CPU_ID_18_PID_DEFINITION() \
    enum VOS_CPU_ID_18_Process_ID \
    { \
        VOS_CPU_ID_18_PID_RESERVED = VOS_PID_CPU_ID_18_DOPRAEND - 1,

#define END_CPU_ID_18_PID_DEFINITION() \
        VOS_CPU_ID_18_PID_BUTT \
    };

/* 注意: 从0x8000开始的PID号已经归外部模块使用 */
/* 具体申明一个PID */
#define DEFINE_PID(pid) pid,

/* 根据PID 得到 CPU ID */
#define VOS_GetCpuId(pid) (((pid) >> 16) & 0x0FF)
#define VOS_GET_CPU_ID(pid) (((pid) >> 16) & 0x0FF)

#if (FEATURE_MULTI_MODEM == FEATURE_ON)

#if (3 == MULTI_MODEM_NUMBER) /* three modems */

enum ModemId {
    MODEM_ID_0 = 0,
    MODEM_ID_1,
    MODEM_ID_2,
    MODEM_ID_BUTT
};

#else /* two modems */

enum ModemId {
    MODEM_ID_0 = 0,
    MODEM_ID_1,
    MODEM_ID_BUTT,
    MODEM_ID_2
};

#endif

#else /* only one */

enum ModemId {
    MODEM_ID_0 = 0,
    MODEM_ID_BUTT,
    MODEM_ID_1,
    MODEM_ID_2
};

#endif
typedef VOS_UINT16 MODEM_ID_ENUM_UINT16;
typedef VOS_UINT16 ModemIdUint16;

/*
 * 功能说明: 接入模式定义
 * 注意；双卡双待的枚举值定义与协议栈和DRV协商为GULT;
 * 需要和DrvInterface.h中的PWC_COMM_MODE_E保持一致
 */
enum VOS_RatMode {
    VOS_RATMODE_GSM   = 0,
    VOS_RATMODE_WCDMA = 1,
    VOS_RATMODE_LTE   = 2,
    VOS_RATMODE_TDS   = 3,
    VOS_RATMODE_1X    = 4,
    VOS_RATMODE_HRPD  = 5,
#if (FEATURE_ON == FEATURE_UE_MODE_NR)
    VOS_RATMODE_NR = 6,
#endif
    VOS_RATMODE_BUTT
};
typedef VOS_UINT32 VOS_RATMODE_ENUM_UINT32;
typedef VOS_UINT32 VOS_RatModeUint32;

/*
 * 功能说明: CPU ID 的定义 0 是CCPU 1 是ACPU 2 是NRCPU 3 是Hifi,Med
 */
enum VOS_CpuId {
    VOS_CPU_ID_CCPU = 0,
    VOS_CPU_ID_ACPU,
    VOS_CPU_ID_NRCPU,
    VOS_CPU_ID_MEDDSP,
    VOS_CPU_ID_MCU,
    VOS_CPU_ID_BBE16,
    VOS_CPU_ID_RFDSP0,
    VOS_CPU_ID_RFDSP1,
    VOS_CPU_ID_NPHY_HL1C,
    VOS_CPU_ID_NPHY_LL1D0,
    VOS_CPU_ID_NPHY_LL1D1,
    VOS_CPU_ID_NPHY_LL1D2,
    VOS_CPU_ID_NPHY_LL1D3,
    VOS_CPU_ID_NPHY_LL1U0,
    VOS_CPU_ID_NPHY_LL1U1,
    VOS_CPU_ID_NPHY_SDR0,
    VOS_CPU_ID_L2_HAC,
    VOS_CPU_ID_NPHY_FRM,
    VOS_CPU_ID_LTEV,

    VOS_CPU_ID_BUTT
};

BEGIN_CPU_ID_0_PID_DEFINITION()

/* 卡1的PID定义 */
DEFINE_PID(I1_UEPS_PID_XSMS)            /* = 5 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_UEPS_PID_MSCC)            /* = 6 */
DEFINE_PID(I1_WUEPS_PID_CSIMA)          /* = 7 */
DEFINE_PID(I1_WUEPS_PID_AWAKE)          /* = 8 */
DEFINE_PID(I1_DSP_PID_WAKE)             /* = 9 */
DEFINE_PID(I1_UEPS_PID_XPDS)            /* = 10 */
DEFINE_PID(I1_WUEPS_PID_WRR)            /* = 11 */
DEFINE_PID(I1_WUEPS_PID_WCOM)           /* = 12 */
DEFINE_PID(I1_WUEPS_PID_BMC)            /* = 13 */
DEFINE_PID(I1_WUEPS_PID_PDCP)           /* = 14 */
DEFINE_PID(I1_WUEPS_PID_RLC)            /* = 15 */
DEFINE_PID(I1_WUEPS_PID_MAC)            /* = 16 */
DEFINE_PID(I1_WUEPS_PID_RLCDL)          /* = 17 */
DEFINE_PID(I1_WUEPS_PID_MACDL)          /* = 18 */
DEFINE_PID(I1_WUEPS_PID_USIM = 20)      /* = 20 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_MAPS_STK_PID)             /* = 21 */
DEFINE_PID(I1_MAPS_PIH_PID)             /* = 22 */
DEFINE_PID(I1_MAPS_PB_PID)              /* = 23 */
DEFINE_PID(I1_UEPS_PID_GRM)             /* = 24 */
DEFINE_PID(I1_UEPS_PID_DL)              /* = 25 */
DEFINE_PID(I1_UEPS_PID_LL)              /* = 26 */
DEFINE_PID(I1_UEPS_PID_SN)              /* = 27 */
DEFINE_PID(I1_UEPS_PID_GAS)             /* = 28 */
DEFINE_PID(I1_WUEPS_PID_MM)             /* = 29 */
DEFINE_PID(I1_WUEPS_PID_MMC)            /* = 30 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_WUEPS_PID_GMM)            /* = 31 */
DEFINE_PID(I1_WUEPS_PID_MMA)            /* = 32 */
DEFINE_PID(I1_WUEPS_PID_CC)             /* = 33 */
DEFINE_PID(I1_WUEPS_PID_SS)             /* = 34 */
DEFINE_PID(I1_WUEPS_PID_TC)             /* = 35 */
DEFINE_PID(I1_WUEPS_PID_SMS)            /* = 36 */
DEFINE_PID(I1_WUEPS_PID_RABM)           /* = 37 */
DEFINE_PID(I1_WUEPS_PID_SM)             /* = 38 */
DEFINE_PID(I1_WUEPS_PID_ADMIN)          /* = 39 */
DEFINE_PID(I1_WUEPS_PID_TAF)            /* = 40 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_WUEPS_PID_VC)             /* = 41 */
DEFINE_PID(I1_WUEPS_PID_DRV_AGENT)      /* = 42 */
DEFINE_PID(I1_UEPS_PID_MTA)             /* = 43 */
DEFINE_PID(I1_DSP_PID_GPHY)             /* = 44 */
DEFINE_PID(I1_DSP_PID_SLEEP)            /* = 45 */
DEFINE_PID(I1_DSP_PID_IDLE)             /* = 46 */
DEFINE_PID(I1_DSP_PID_APM)              /* = 47 */
DEFINE_PID(I1_WUEPS_PID_SLEEP)          /* = 48 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_PID(CPROC_PID_RM = 49)           /* = 49 */
#endif
DEFINE_PID(MSPS_PID_1X_RLAC = 50)       /* = 50 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(MSPS_PID_1X_RMAC)            /* = 51 */
DEFINE_PID(MSPS_PID_1X_RRLP)            /* = 52 */
DEFINE_PID(MSPS_PID_1X_RTESTSO)         /* = 53 */
DEFINE_PID(MSPS_PID_1X_FTESTSO)         /* = 54 */
DEFINE_PID(MSPS_PID_1X_FLAC)            /* = 55 */
DEFINE_PID(MSPS_PID_1X_FMAC)            /* = 56 */
DEFINE_PID(MSPS_PID_1X_FRLP)            /* = 57 */
DEFINE_PID(CTTF_PID_HRPD_RSLP)          /* = 58 */
DEFINE_PID(CTTF_PID_HRPD_RPA)           /* = 59 */
DEFINE_PID(CTTF_PID_HRPD_RSPS)          /* = 60 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(CTTF_PID_HRPD_RMAC)          /* = 61 */
DEFINE_PID(CTTF_PID_HRPD_FSLP)          /* = 62 */
DEFINE_PID(CTTF_PID_HRPD_FPA)           /* = 63 */
DEFINE_PID(CTTF_PID_HRPD_FSPS)          /* = 64 */
DEFINE_PID(CTTF_PID_HRPD_FMAC)          /* = 65 */
DEFINE_PID(CTTF_PID_HRPD_SIG)           /* = 66 */
DEFINE_PID(MSPS_PID_PPPC)               /* = 67 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_PID(CPROC_PID_HRPD_CM = 68)      /* = 68 */
DEFINE_PID(CPROC_PID_HRPD_SM = 69)      /* = 69 */
#endif
DEFINE_PID(DSP_PID_PROCSTUB = 70)       /* = 70 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_PID(CPROC_PID_1X_CM  = 71)       /* = 71 */
DEFINE_PID(CPROC_PID_1X_SM  = 72)       /* = 72 */
DEFINE_PID(CPROC_PID_1X_DSP = 73)       /* = 73 */
DEFINE_PID(CPROC_PID_EVDO   = 74)       /* = 74 */
DEFINE_PID(UEPS_PID_1XCASM  = 75)       /* = 75 */
DEFINE_PID(UEPS_PID_1XCMEAS = 76)       /* = 76 */
DEFINE_PID(UEPS_PID_1XCSRCH = 77)       /* = 77 */
DEFINE_PID(UEPS_PID_HALMP   = 78)       /* = 78 */
DEFINE_PID(UEPS_PID_HRUP    = 79)       /* = 79 */
DEFINE_PID(UEPS_PID_HSCP    = 80)       /* = 80 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(UEPS_PID_HSP     = 81)       /* = 81 */
#endif
DEFINE_PID(UEPS_PID_XREG    = 82)       /* = 82 */
DEFINE_PID(UEPS_PID_XSD)                /* = 83 */
DEFINE_PID(UEPS_PID_XCC)                /* = 84 */
DEFINE_PID(UEPS_PID_HSD)                /* = 85 */
DEFINE_PID(UEPS_PID_HLU)                /* = 86 */
DEFINE_PID(UEPS_PID_HSM)                /* = 87 */
DEFINE_PID(UEPS_PID_EHSM)               /* = 88 */
DEFINE_PID(I0_UEPS_PID_XSMS)            /* = 89 */
DEFINE_PID(I0_UEPS_PID_XPDS)            /* = 90 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(UEPS_PID_HRM)                /* = 91 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_PID(CDMA_PID_ED     = 92)        /* = 92  CDMA 编解码使用 */
#endif
DEFINE_PID(MSPS_PID_MIP    = 93)        /* = 93 */

DEFINE_PID(PC_PID_TOOLRFU = 99)         /* = 99  工具已使用 */
DEFINE_PID(PC_PID_TOOL = 100)           /* = 100 工具已使用 */
/* 新增PID 添加到reserved位置，并将reserved删除 */
DEFINE_PID(I0_WUEPS_PID_USIM)           /* = 101 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_MAPS_STK_PID)             /* = 102 */
DEFINE_PID(I0_MAPS_PIH_PID)             /* = 103 */
DEFINE_PID(I0_MAPS_PB_PID)              /* = 104 */
DEFINE_PID(I1_PS_PID_LPP)               /* = 105 */
DEFINE_PID(NVIM_WRITE_DATA_PID)         /* = 106 */
DEFINE_PID(PID_RESERVED107)             /* RESERVED = 107 */
DEFINE_PID(I1_PS_PID_HPA)               /* = 108 */
DEFINE_PID(I1_PS_PID_ERRC)              /* = 109 */
DEFINE_PID(WUEPS_PID_REG)               /* = 110 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(UEPS_PID_BASTET)             /* = 111 */
DEFINE_PID(TLPHY_PID_RTTAGENT)          /* = 112 */
DEFINE_PID(MSP_PID_DIAG_AGENT)          /* = 113 */
DEFINE_PID(I0_WUEPS_PID_AWAKE)          /* = 114 */
DEFINE_PID(I0_MSP_L4_L4A_PID)           /* = 115 */
DEFINE_PID(I1_MSP_L4_L4A_PID)           /* = 116 */
DEFINE_PID(I0_MSP_SYS_FTM_PID)          /* = 117 */
DEFINE_PID(I0_UEPS_PID_MSCC)            /* = 118 */
DEFINE_PID(I0_MSP_LDSP_PID)             /* = 119 */
DEFINE_PID(I1_MSP_LDSP_PID)             /* = 120 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_MSP_SYS_FTM_PID)          /* = 121 */
DEFINE_PID(I1_PS_PID_ERMM)              /* = 122 */
DEFINE_PID(I1_PS_PID_MM)                /* = 123 */
DEFINE_PID(UEPS_PID_SHPA)               /* = 124 */
DEFINE_PID(I1_PS_PID_ESM)               /* = 125 */
DEFINE_PID(I1_PS_PID_RABM)              /* = 126 */
DEFINE_PID(I1_PS_PID_TC)                /* = 127 */
DEFINE_PID(I0_UEPS_PID_GAS)             /* = 128 */
DEFINE_PID(I0_UEPS_PID_DL)              /* = 129 */
DEFINE_PID(I0_UEPS_PID_GRM)             /* = 130 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_UEPS_PID_LL)              /* = 131 */
DEFINE_PID(I0_UEPS_PID_SN)              /* = 132 */
DEFINE_PID(I0_WUEPS_PID_WRR)            /* = 133 */
DEFINE_PID(I0_WUEPS_PID_WCOM)           /* = 134 */
DEFINE_PID(UEPS_PID_CST)                /* = 135 */
DEFINE_PID(I0_WUEPS_PID_BMC)            /* = 136 */
DEFINE_PID(I0_WUEPS_PID_PDCP)           /* = 137 */
DEFINE_PID(I0_WUEPS_PID_RLC)            /* = 138 */
DEFINE_PID(I0_WUEPS_PID_MAC)            /* = 139 */
DEFINE_PID(I0_WUEPS_PID_RABM)           /* = 140 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_WUEPS_PID_MMC)            /* = 141 */
DEFINE_PID(I0_WUEPS_PID_MM)             /* = 142 */
DEFINE_PID(I0_WUEPS_PID_GMM)            /* = 143 */
DEFINE_PID(I0_WUEPS_PID_CC)             /* = 144 */
DEFINE_PID(I0_WUEPS_PID_SM)             /* = 145 */
DEFINE_PID(I0_WUEPS_PID_SMS)            /* = 146 */
DEFINE_PID(I0_WUEPS_PID_SS)             /* = 147 */
DEFINE_PID(I0_WUEPS_PID_TC)             /* = 148 */
DEFINE_PID(WUEPS_PID_SMT)               /* = 149 */
DEFINE_PID(I0_WUEPS_PID_ADMIN)          /* = 150 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(WUEPS_PID_MUX)               /* = 151 */
DEFINE_PID(I0_WUEPS_PID_TAF)            /* = 152 */
DEFINE_PID(WUEPS_PID_PPP)               /* = 153 */
DEFINE_PID(WUEAPP_PID_MUX)              /* = 154 */
DEFINE_PID(I0_PS_PID_NAS_COMM)          /* = 155 */
DEFINE_PID(I1_PS_PID_NAS_COMM)          /* = 156 */
DEFINE_PID(I0_WUEPS_PID_MMA)            /* = 157 */
DEFINE_PID(I0_WUEPS_PID_SLEEP)          /* = 158 */
DEFINE_PID(WUEPS_PID_CBS)               /* = 159 */
DEFINE_PID(WUEPS_PID_RESERVED_160)      /* RESERVED = 160 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_WUEPS_PID_LCS)            /* = 161 */
DEFINE_PID(I2_WUEPS_PID_LCS)            /* = 162 */
DEFINE_PID(WUEPS_PID_APCID1)            /* = 163 */
DEFINE_PID(WUEPS_PID_SPY)               /* = 164 */
DEFINE_PID(I0_WUEPS_PID_VC)             /* = 165 */
DEFINE_PID(I0_WUEPS_PID_LCS)            /* = 166 */
DEFINE_PID(WUEPS_PID_APCID2)            /* = 167 */
DEFINE_PID(WUEPS_PID_APCID3)            /* = 168 */
DEFINE_PID(I0_PS_PID_LPP)               /* = 169 */
DEFINE_PID(PS_PID_CSS)                  /* = 170 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_PS_PID_ERRC)              /* = 171 */
DEFINE_PID(I0_PS_PID_ERMM)              /* = 172 */
DEFINE_PID(I0_PS_PID_MM)                /* = 173 */
DEFINE_PID(I0_PS_PID_ESM)               /* = 174 */
DEFINE_PID(I0_PS_PID_RABM)              /* = 175 */
DEFINE_PID(I0_PS_PID_TC)                /* = 176 */
DEFINE_PID(I0_PS_PID_IP)                /* = 177 */
DEFINE_PID(I0_PS_PID_OM)                /* = 178 */
DEFINE_PID(I0_PS_PID_MAC_UL)            /* = 179 */
DEFINE_PID(I0_PS_PID_RLC_UL)            /* = 180 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_PS_PID_MAC_DL)            /* = 181 */
DEFINE_PID(I0_PS_PID_RLC_DL)            /* = 182 */
DEFINE_PID(I0_PS_PID_PDCP_UL)           /* = 183 */
DEFINE_PID(I0_PS_PID_PDCP_DL)           /* = 184 */
DEFINE_PID(I0_PS_PID_HPA)               /* = 185 */
DEFINE_PID(PS_PID_MUX)                  /* = 186 */
DEFINE_PID(PS_PID_MUXOM)                /* = 187 */
DEFINE_PID(I1_PS_PID_IP)                /* = 188 */
DEFINE_PID(UEPS_PID_FLOWCTRL_C)         /* = 189 */
DEFINE_PID(I0_WUEPS_PID_MACDL)          /* = 190 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_WUEPS_PID_RLCDL)          /* = 191 */
DEFINE_PID(WUEPS_PID_NOSIG)             /* = 192 */
DEFINE_PID(UEPS_PID_DICC_C)             /* = 193 */
DEFINE_PID(I0_WUEPS_PID_DRV_AGENT)      /* = 194 */
DEFINE_PID(I0_PS_PID_PTT)               /* = 195 */
DEFINE_PID(I1_PS_PID_OM)                /* = 196 */
DEFINE_PID(UEPS_PID_CDS)                /* = 197 */
DEFINE_PID(UEPS_PID_NDCLIENT)           /* = 198 */
DEFINE_PID(WUEPS_PID_SAR)               /* = 199 */
DEFINE_PID(UEPS_PID_DHCP)               /* = 200 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_DSP_PID_APM)              /* = 201 */
DEFINE_PID(DSP_PID_UPA)                 /* = 202 */
DEFINE_PID(I0_DSP_PID_WPHY)             /* = 203 */
DEFINE_PID(I1_PS_PID_MAC_UL)            /* = 204 */
DEFINE_PID(I0_DSP_PID_GPHY)             /* = 205 */
DEFINE_PID(I0_DSP_PID_WAKE)             /* = 206 */
DEFINE_PID(I0_DSP_PID_SLEEP)            /* = 207 */
DEFINE_PID(I0_DSP_PID_IDLE)             /* = 208 */
DEFINE_PID(I0_UEPS_PID_MTA)             /* = 209 */
DEFINE_PID(WUEPS_PID_NVIM_FLUSH)        /* = 210 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(HL1_PID_SNDCMD)              /* = 211 */
DEFINE_PID(MSPS_PID_1X_FRESERVE)        /* = 212 */
DEFINE_PID(MSPS_PID_1X_RRESERVE)        /* = 213 */
DEFINE_PID(TPS_PID_RLC)                 /* = 214 */
DEFINE_PID(TPS_PID_MAC)                 /* = 215 */
DEFINE_PID(TPS_PID_PDC)                 /* = 216 */
DEFINE_PID(TPS_PID_RRC)                 /* = 217 */
DEFINE_PID(MSP_PID_BBP_AGENT)           /* = 218 */
DEFINE_PID(UEPS_PID_MTC)                /* = 219 */
DEFINE_PID(WUEPS_PID_CBPCA)             /* = 220 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_WUEPS_PID_CSIMA)          /* = 221 */
DEFINE_PID(I0_PS_PID_IMSA)              /* = 222 */
DEFINE_PID(I0_PS_PID_IMSVA)             /* = 223 */
DEFINE_PID(UEPS_PID_IPS_CCORE)          /* = 224 */
DEFINE_PID(UEPS_PID_RRM)                /* = 225 */
DEFINE_PID(WUEPS_PID_CMMCA)             /* = 226 */
DEFINE_PID(WUEPS_PID_OM_BBP_MASTER)     /* = 227 */
DEFINE_PID(DSP_PID_RCM)                 /* = 228 */
DEFINE_PID(DSP_PID_STARTUP)             /* = 229 */
DEFINE_PID(MSP_PID_DRX)                 /* = 230 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(VOLTE_PID_DIAG)              /* = 231 */
DEFINE_PID(I2_WUEPS_PID_MM)             /* = 232 */
DEFINE_PID(I2_WUEPS_PID_MMC)            /* = 233 */
DEFINE_PID(I2_WUEPS_PID_GMM)            /* = 234 */
DEFINE_PID(I2_WUEPS_PID_MMA)            /* = 235 */
DEFINE_PID(I2_WUEPS_PID_CC)             /* = 236 */
DEFINE_PID(I2_WUEPS_PID_SS)             /* = 237 */
DEFINE_PID(I2_WUEPS_PID_TC)             /* = 238 */
DEFINE_PID(I2_WUEPS_PID_SMS)            /* = 239 */
DEFINE_PID(I2_WUEPS_PID_RABM)           /* = 240 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I2_WUEPS_PID_SM)             /* = 241 */
DEFINE_PID(I2_WUEPS_PID_ADMIN)          /* = 242 */
DEFINE_PID(I2_WUEPS_PID_TAF)            /* = 243 */
DEFINE_PID(I2_WUEPS_PID_VC)             /* = 244 */
DEFINE_PID(I2_WUEPS_PID_DRV_AGENT)      /* = 245 */
DEFINE_PID(I2_UEPS_PID_MTA)             /* = 246 */
DEFINE_PID(I2_UEPS_PID_MSCC)            /* = 247 */
DEFINE_PID(I2_UEPS_PID_XSMS)            /* = 248 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_PID(CPROC_PID_1X_CTAS  = 249)    /* = 249 */
#endif
DEFINE_PID(I2_PS_PID_NAS_COMM = 250)    /* = 250 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(CCPU_PID_CBT)                /* = 251 */
DEFINE_PID(CCPU_PID_PAM_OM)             /* = 252 */
DEFINE_PID(DSP_PID_TLPHY)               /* = 253 */
DEFINE_PID(I2_WUEPS_PID_USIM)           /* = 254 */
DEFINE_PID(I2_MAPS_STK_PID)             /* = 255 */
DEFINE_PID(I2_MAPS_PIH_PID)             /* = 256 */
DEFINE_PID(I2_MAPS_PB_PID)              /* = 257 */
DEFINE_PID(I2_UEPS_PID_GRM)             /* = 258 */
DEFINE_PID(I2_UEPS_PID_DL)              /* = 259 */
DEFINE_PID(I2_UEPS_PID_LL)              /* = 260 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I2_UEPS_PID_SN)              /* = 261 */
DEFINE_PID(I2_UEPS_PID_GAS)             /* = 262 */
DEFINE_PID(I2_DSP_PID_GPHY)             /* = 263 */
DEFINE_PID(I2_DSP_PID_SLEEP)            /* = 264 */
DEFINE_PID(I2_DSP_PID_IDLE)             /* = 265 */
DEFINE_PID(I2_DSP_PID_APM)              /* = 266 */
DEFINE_PID(I2_WUEPS_PID_SLEEP)          /* = 267 */
DEFINE_PID(I2_WUEPS_PID_CSIMA)          /* = 268 */
DEFINE_PID(I2_WUEPS_PID_AWAKE)          /* = 269 */
DEFINE_PID(I2_DSP_PID_WAKE)             /* = 270 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I2_UEPS_PID_XPDS)            /* = 271 */
DEFINE_PID(I1_DSP_PID_WPHY)             /* = 272 */
DEFINE_PID(PID_UPHY_CHR_AGENT)          /* = 273 */
DEFINE_PID(I0_DSP_PID_RTTSTUB)          /* = 274 */
DEFINE_PID(I0_DSP_PID_RTTMATCH)         /* = 275 */
DEFINE_PID(I0_DSP_PID_RTTPREEMPT)       /* = 276 */
DEFINE_PID(WUEPS_PID_EDA)               /* = 277 */
DEFINE_PID(I1_PS_PID_RLC_UL)            /* = 278 */
DEFINE_PID(I1_PS_PID_MAC_DL)            /* = 279 */
DEFINE_PID(I1_PS_PID_RLC_DL)            /* = 280 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I1_PS_PID_PDCP_UL)           /* = 281 */
DEFINE_PID(I1_PS_PID_PDCP_DL)           /* = 282 */
DEFINE_PID(I1_PS_PID_PTT)               /* = 283 */
DEFINE_PID(I1_PS_PID_IMSA)              /* = 284 */
DEFINE_PID(I1_PS_PID_IMSVA)             /* = 285 */
DEFINE_PID(I0_DSP_PID_RTTONLINE)        /* = 286 */
#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_PID(DSP_PID_CSDR_1X_CM_AGENT = 287)    /* = 287 */
DEFINE_PID(DSP_PID_CSDR_1X_SM_AGENT = 288)    /* = 288 */
DEFINE_PID(DSP_PID_CSDR_HRPD_CM_AGENT = 289)  /* = 289 */
DEFINE_PID(DSP_PID_CSDR_HRPD_SM_AGENT = 290)  /* = 290 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
#endif
DEFINE_PID(DSP_PID_GSDR_HIGHPRI_AGENT = 291)  /* = 291 */
DEFINE_PID(LINKH_PID_LINKH_ARM)         /* = 292 */
DEFINE_PID(MSP_PID_CHR_MDM)             /* = 293 */
DEFINE_PID(I1_DSP_PID_TLPHY)            /* = 294 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(DSP_PID_RFM)                 /* = 295 */
DEFINE_PID(I0_UEPS_PID_REGM)            /* = 296 */
DEFINE_PID(I1_UEPS_PID_REGM)            /* = 297 */
DEFINE_PID(I2_UEPS_PID_REGM)            /* = 298 */
DEFINE_PID(I0_DSP_PID_GPHYLOW)          /* = 299 */
DEFINE_PID(I1_DSP_PID_GPHYLOW)          /* = 300 */
DEFINE_PID(I2_DSP_PID_GPHYLOW)          /* = 301 */
DEFINE_PID(DSP_PID_UPHY_MNTN)           /* = 302 */
DEFINE_PID(DSP_PID_BBA_CAL)             /* = 303 */
DEFINE_PID(PHY_PID_PROCESS)             /* = 304 */
DEFINE_PID(PHY_PID_STARTUP1)            /* = 305,时域版本. */
DEFINE_PID(UEPS_PID_NRRC_0)             /* = 306 */
DEFINE_PID(UEPS_PID_NRRM_0)             /* = 307,时域版本. */
DEFINE_PID(I0_UEPS_PID_DSM)             /* = 308 */
DEFINE_PID(I1_UEPS_PID_DSM)             /* = 309 */
DEFINE_PID(I2_UEPS_PID_DSM)             /* = 310 */
DEFINE_PID(I0_UEPS_PID_NPDCP_LTE_UL_RT) /* = 311 */
DEFINE_PID(I0_UEPS_PID_NPDCP_LTE_DL_RT) /* = 312 */
DEFINE_PID(I1_DSP_PID_RTTSTUB)          /* = 313 */
DEFINE_PID(I1_DSP_PID_RTTMATCH)         /* = 314 */
DEFINE_PID(I1_DSP_PID_RTTPREEMPT)       /* = 315 */
DEFINE_PID(I1_DSP_PID_RTTONLINE)        /* = 316 */
DEFINE_PID(UEPS_PID_PSUPMEM)            /* = 317 */
DEFINE_PID(I0_UEPS_PID_CCM)             /* = 318 */
DEFINE_PID(I1_UEPS_PID_CCM)             /* = 319 */
DEFINE_PID(I2_UEPS_PID_CCM)             /* = 320 */ /* 提醒! 新增多实例PID请加到g_psCommPidModemIdMapTbl, 新增CDMA的PID请加到g_psCommPidForCdmaTbl */
DEFINE_PID(I0_PS_PID_VERRC)             /* = 321 */ /* 新增LTE-V PID  */
DEFINE_PID(I0_PS_PID_VSYNC)             /* = 322 */
DEFINE_PID(I0_PS_PID_VMAC_UL)           /* = 323 */
DEFINE_PID(I0_PS_PID_VRLC_UL)           /* = 324 */
DEFINE_PID(I0_PS_PID_VPDCP_UL)          /* = 325 */
DEFINE_PID(I0_PS_PID_VMAC_DL)           /* = 326 */
DEFINE_PID(I0_PS_PID_VRLC_DL)           /* = 327 */
DEFINE_PID(I0_PS_PID_VRBM_DL)           /* = 328 */
DEFINE_PID(I0_PS_PID_VPDCP_DL)          /* = 329 */
DEFINE_PID(I0_PS_PID_VHPA)              /* = 330 */ /* 新增LTE-V PID  */
DEFINE_PID(I0_MAPS_EMAT_PID)            /* = 331 */ /* eSIM适配新增 */
DEFINE_PID(I1_MAPS_EMAT_PID)            /* = 332 */ /* eSIM适配新增 */
DEFINE_PID(I2_MAPS_EMAT_PID)            /* = 333 */ /* eSIM适配新增 */
DEFINE_PID(I0_UEPS_PID_GUCALL)          /* = 334 */
DEFINE_PID(I1_UEPS_PID_GUCALL)          /* = 335 */
DEFINE_PID(I2_UEPS_PID_GUCALL)          /* = 336 */
DEFINE_PID(I0_UEPS_PID_XCALL)           /* = 337 */
DEFINE_PID(I1_UEPS_PID_XCALL)           /* = 338 */
DEFINE_PID(I2_UEPS_PID_XCALL)           /* = 339 */
DEFINE_PID(DSP_PID_BBA_RFM_LPC)         /* = 340 */
DEFINE_PID(CCPU_PID_PAM_MFG)            /* = 341 */
DEFINE_PID(I0_PS_PID_VNAS)              /* = 342 */ /* 新增LTE-V PID */
DEFINE_PID(DSP_PID_BBA_CALC)            /* = 343 */ /* 新增 BBA CAL计算模块 */
DEFINE_PID(I0_PS_PID_VTC)               /* = 344 */

/*
 * 上面是单板侧的PID
 * 下面是PC侧的PID
 */
/* CCPU PID最大不能超过65535,使用65536以及65536以上的PID会造成单板异常复位 */
#if (VOS_WIN32 == VOS_OS_VER) /*lint !e553*/
DEFINE_PID(WUEPS_PID_ITTSTUB1)
DEFINE_PID(WUEPS_PID_ITTSTUB2)
DEFINE_PID(WUEPS_PID_WTTFTEST)
DEFINE_PID(WUEPS_PID_OAMTEST)
DEFINE_PID(WUEPS_PID_XMLDECODE)
#endif
#ifdef HPA_ITT
DEFINE_PID(MAPS_STUB_RCVMSG_PID)
DEFINE_PID(MAPS_STUB_SENDMSG_PID)
DEFINE_PID(MAPS_STUB_GTRMSG_PROC_PID)
#endif

END_CPU_ID_0_PID_DEFINITION()

/* ACPU PID start */
BEGIN_CPU_ID_1_PID_DEFINITION()
DEFINE_PID(MSP_APP_DS_MUX_PID)          /* = 65536 */
DEFINE_PID(PS_PID_APP_NDIS)             /* = 65537 */
DEFINE_PID(PS_PID_APP_PPP)              /* = 65538 */
DEFINE_PID(PID_RESERVED_35)             /* RESERVED  65539 */
DEFINE_PID(MSP_APP_DS_DIAG_PID)         /* = 65540 */
DEFINE_PID(ACPU_PID_PCSC)               /* = 65541 */
DEFINE_PID(PID_RESERVED_37)             /* RESERVED  65542 */
DEFINE_PID(ACPU_PID_PCVOICE)            /* = 65543 */
DEFINE_PID(PID_RESERVED_36)             /* RESERVED  65544 */
DEFINE_PID(ACPU_PID_PB)                 /* = 65545 */
DEFINE_PID(PID_RESERVED_38)             /* RESERVED  65546 */
DEFINE_PID(UEPS_PID_DICC_A)             /* = 65547 */
DEFINE_PID(ACPU_PID_RNIC)               /* = 65548 */
DEFINE_PID(WUEPS_PID_AT)                /* = 65549 */
DEFINE_PID(ACPU_PID_NFEXT)              /* = 65550 */
DEFINE_PID(ACPU_PID_ADS_UL)             /* = 65551 */
DEFINE_PID(ACPU_PID_ADS_DL)             /* = 65552 */
DEFINE_PID(PID_RESERVED_42)             /* RESERVED  65553 */
DEFINE_PID(ACPU_PID_TAFAGENT)           /* = 65554 */
DEFINE_PID(UEPS_PID_FLOWCTRL_A)         /* = 65555 */
DEFINE_PID(ACPU_PID_CPULOAD)            /* = 65556 */
DEFINE_PID(PID_RESERVED_39)             /* RESERVED  65557 */
DEFINE_PID(UEPS_PID_NDSERVER)           /* = 65558 */
DEFINE_PID(PID_RESERVED_40)             /* RESERVED  65559 */
DEFINE_PID(PID_RESERVED_41)             /* RESERVED  65560 */
DEFINE_PID(DSP_PID_VOICE_RT_ACPU)       /* = 65561 */
DEFINE_PID(DSP_PID_VOICE_ACPU)          /* = 65562 */
DEFINE_PID(MSP_PID_DIAG_FW)             /* = 65563 */
DEFINE_PID(MSP_PID_DIAG_APP_AGENT)      /* = 65564 */
DEFINE_PID(ACPU_PID_CBT)                /* = 65565 */
DEFINE_PID(ACPU_PID_PAM_OM)             /* = 65566 */
DEFINE_PID(ACPU_PID_BASTET_COMM)        /* = 65567 */
DEFINE_PID(ACPU_PID_VOICE_PROXY)        /* = 65568 */
DEFINE_PID(ACPU_PID_ALSA_AGENT)         /* = 65569 */
DEFINE_PID(MSP_PID_CHR)                 /* = 65570 */
DEFINE_PID(ACPU_PID_VOICE_AGENT)        /* = 65571 */
DEFINE_PID(ACPU_PID_PC5_CHQEST)         /* = 65572 */
DEFINE_PID(ACPU_PID_HIFI_MANAGE)        /* = 65573 */
END_CPU_ID_1_PID_DEFINITION() /* ACPU PID end */

/* NR CPU 的PID 定义 */
BEGIN_CPU_ID_2_PID_DEFINITION()
DEFINE_PID(UEPS_PID_NRRC_2)              /* = 131072 */
DEFINE_PID(UEPS_PID_NRRM_2)              /* = 131073 */
DEFINE_PID(NPHY_PID_STARTUP)             /* = 131074 */
DEFINE_PID(NRCPU_PID_PAM_OM)             /* = 131075 */
DEFINE_PID(UEPS_PID_L2MA)                /* = 131076 */
DEFINE_PID(I0_UEPS_PID_NRMM)             /* = 131077 */
DEFINE_PID(I0_UEPS_PID_SDAP_UL)          /* = 131078 */
DEFINE_PID(I0_UEPS_PID_NPDCP_UL)         /* = 131079 */
DEFINE_PID(I0_UEPS_PID_NRLC_UL_NRT)      /* = 131080 */
DEFINE_PID(I0_UEPS_PID_NPDCP_DL_NRT)     /* = 131081 */
DEFINE_PID(I0_UEPS_PID_NRLC_DL_NRT)      /* = 131082 */
DEFINE_PID(I0_UEPS_PID_SDAP_DL)          /* = 131083 */
DEFINE_PID(UPHY_PID_HL1C_L1C_L_2)        /* = 131084 */
DEFINE_PID(MSP_PID_DIAG_NRM_AGENT)       /* = 131085 */
DEFINE_PID(I0_UEPS_PID_NRSM)             /* = 131086 */
DEFINE_PID(NRCBT_PID_NOSIG)              /* = 131087 */
DEFINE_PID(NRCBT_PID_CBT)                /* = 131088 */
DEFINE_PID(I0_UEPS_PID_PCF)              /* = 131089 */
DEFINE_PID(I0_UEPS_PID_NREAP)            /* = 131090 */
DEFINE_PID(NPHY_PID_RCMAGENT)            /* = 131091 */
DEFINE_PID(NRAGENT_PID_AGENT)            /* = 131092 */
DEFINE_PID(DSP_PID_BBA_RFM_AGENT_LPC)    /* = 131093 */
DEFINE_PID(I0_PS_PID_NRTHROT)            /* = 131094 */
DEFINE_PID(MSP_PID_CHR_NR)               /* = 131095 */
DEFINE_PID(DSP_PID_RTTSTUB_NR)           /* = 131096 */
END_CPU_ID_2_PID_DEFINITION()

BEGIN_CPU_ID_3_PID_DEFINITION()
DEFINE_PID(DSP_PID_VOICE_RESERVED)
DEFINE_PID(DSP_PID_VOICE_RT_HIFI)
DEFINE_PID(DSP_PID_VOICE_HIFI)
DEFINE_PID(DSP_PID_AUDIO_RT)
DEFINE_PID(DSP_PID_AUDIO)
DEFINE_PID(DSP_PID_HIFI_OM)
DEFINE_PID(DSP_PID_VOICEPP)
DEFINE_PID(DSP_PID_VOICE_LOW)
END_CPU_ID_3_PID_DEFINITION()

#if (FEATURE_VOICE_UP == FEATURE_ON)
/* 语音软件上移时, 使用ACPU的PID */
#define DSP_PID_VOICE_RT DSP_PID_VOICE_RT_ACPU
#define DSP_PID_VOICE DSP_PID_VOICE_ACPU
#else
/* 语音软件不上移时, 使用HIFI的PID */
#define DSP_PID_VOICE_RT DSP_PID_VOICE_RT_HIFI
#define DSP_PID_VOICE DSP_PID_VOICE_HIFI
#endif

/* MED HiFi的PID 定义，为保证其它组件编译通过，暂时保留MED_RT等PID */
#define DSP_PID_MED_RT DSP_PID_VOICE_RT
#define DSP_PID_MED DSP_PID_VOICE
#define DSP_PID_MED_OM DSP_PID_HIFI_OM

/* MCU的PID 定义 */
BEGIN_CPU_ID_4_PID_DEFINITION()
DEFINE_PID(ACPU_PID_MCU_OM)
END_CPU_ID_4_PID_DEFINITION()

/* BBE16的PID */
BEGIN_CPU_ID_5_PID_DEFINITION()
DEFINE_PID(DSP_PID_TDPHY_RESERVED)
DEFINE_PID(DSP_PID_TDPHY)
DEFINE_PID(DSP_PID_CSDR_1X_RNCHIP)
DEFINE_PID(DSP_PID_CSDR_COMM_PRIMRCV)
DEFINE_PID(DSP_PID_CSDR_1X_QTR_PCG)
DEFINE_PID(DSP_PID_CSDR_1X_MEASURE)
DEFINE_PID(DSP_PID_CSDR_HRPD_CM)
DEFINE_PID(DSP_PID_CSDR_HRPD_SM)
DEFINE_PID(DSP_PID_CSDR_COMM_DYN_LOAD)
DEFINE_PID(DSP_PID_GSDR_COMM_DYN_LOAD)
DEFINE_PID(DSP_PID_GSDR_HIGHPRI)
DEFINE_PID(DSP_PID_GSDR_SECONDPRI)
DEFINE_PID(DSP_PID_GSDR_THIRDPRI)
DEFINE_PID(DSP_PID_GSDR_FOURTHPRI)
DEFINE_PID(DSP_PID_CSDR_COMM_LPC)
DEFINE_PID(DSP_PID_LPHY_VIRTUAL_PID) /* 提醒! 新增LPHY虚拟的PID，主要用于EASYRF公共平台区分邮箱分发对象 */
DEFINE_PID(DSP_PID_GSDR_LOWPRI)
DEFINE_PID(DSP_PID_RFHAL_CANCEL)
DEFINE_PID(DSP_PID_BBA_PMCTRL_CLDSP)
END_CPU_ID_5_PID_DEFINITION()

BEGIN_CPU_ID_6_PID_DEFINITION()
DEFINE_PID(RFDSP_PID_TRXCH_0)
DEFINE_PID(RFDSP_PID_ATE_0)
DEFINE_PID(RFDSP_PID_CAL_0)
DEFINE_PID(RFDSP_PID_DEBUG_0)
DEFINE_PID(RFDSP_PID_TRXCL_0)
DEFINE_PID(RFDSP_PID_PUB_0)
DEFINE_PID(RFDSP_PID_TRXCM_0)
END_CPU_ID_6_PID_DEFINITION()

BEGIN_CPU_ID_7_PID_DEFINITION()
DEFINE_PID(RFDSP_PID_TRXCH_1)
DEFINE_PID(RFDSP_PID_ATE_1)
DEFINE_PID(RFDSP_PID_CAL_1)
DEFINE_PID(RFDSP_PID_DEBUG_1)
DEFINE_PID(RFDSP_PID_TRXCL_1)
DEFINE_PID(RFDSP_PID_PUB_1)
DEFINE_PID(RFDSP_PID_TRXCM_1)
END_CPU_ID_7_PID_DEFINITION()

/* NPHY新增 */
BEGIN_CPU_ID_8_PID_DEFINITION()
DEFINE_PID(UPHY_PID_HL1C_OM)
DEFINE_PID(UPHY_PID_HL1C_L1C_H)
DEFINE_PID(UPHY_PID_HL1C_L1C_L_8)
DEFINE_PID(UPHY_PID_HL1C_L1D_H)
DEFINE_PID(UPHY_PID_HL1C_L1D_L)
DEFINE_PID(UPHY_PID_HL1C_L1U_H)
DEFINE_PID(UPHY_PID_HL1C_L1U_L)
DEFINE_PID(UPHY_PID_HL1C_SDR_H)
DEFINE_PID(UPHY_PID_HL1C_SDR_L)
DEFINE_PID(NPHY_PID_HL1C_SLOT_PROC_0)
DEFINE_PID(NPHY_PID_HL1C_LL1U0_MBX_H)
DEFINE_PID(NPHY_PID_HL1C_LL1U0_MBX_L)
DEFINE_PID(NPHY_PID_HL1C_UL_DCI_PROC_0)
DEFINE_PID(NPHY_PID_HL1C_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_PID_HL1C_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_PID_HL1C_UL_INT_0)
DEFINE_PID(NPHY_PID_HL1C_STUB_0)
DEFINE_PID(UPHY_PID_HL1C_BBA)
DEFINE_PID(NPHY_PID_HL1C_LPC_0)
DEFINE_PID(UPHY_PID_HL1C_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_HL1C_FRM_BBA_H)
DEFINE_PID(UPHY_PID_HL1C_FRM_BBA_L)
END_CPU_ID_8_PID_DEFINITION()

BEGIN_CPU_ID_9_PID_DEFINITION()
DEFINE_PID(UPHY_PID_LL1D0_OM)
DEFINE_PID(UPHY_PID_LL1D0_L1C_H)
DEFINE_PID(UPHY_PID_LL1D0_L1C_L)
DEFINE_PID(UPHY_PID_LL1D0_L1D_H)
DEFINE_PID(UPHY_PID_LL1D0_L1D_L)
DEFINE_PID(UPHY_PID_LL1D0_L1U_H)
DEFINE_PID(UPHY_PID_LL1D0_L1U_L)
DEFINE_PID(UPHY_PID_LL1D0_SDR_H)
DEFINE_PID(UPHY_PID_LL1D0_SDR_L)
DEFINE_PID(NPHY_PID_LL1D0_SLOT_PROC_0)
DEFINE_PID(NPHY_PID_LL1D0_LL1U0_MBX_H)
DEFINE_PID(NPHY_PID_LL1D0_LL1U0_MBX_L)
DEFINE_PID(NPHY_PID_LL1D0_UL_DCI_PROC_0)
DEFINE_PID(NPHY_PID_LL1D0_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_PID_LL1D0_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_PID_LL1D0_UL_INT_0)
DEFINE_PID(NPHY_PID_LL1D0_STUB_0)
DEFINE_PID(UPHY_PID_LL1D0_BBA)
DEFINE_PID(NPHY_PID_LL1D0_LPC_0)
DEFINE_PID(UPHY_PID_LL1D0_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_LL1D0_FRM_BBA_H)
DEFINE_PID(UPHY_PID_LL1D0_FRM_BBA_L)
END_CPU_ID_9_PID_DEFINITION()

BEGIN_CPU_ID_10_PID_DEFINITION()
DEFINE_PID(UPHY_PID_LL1D1_OM)
DEFINE_PID(UPHY_PID_LL1D1_L1C_H)
DEFINE_PID(UPHY_PID_LL1D1_L1C_L)
DEFINE_PID(UPHY_PID_LL1D1_L1D_H)
DEFINE_PID(UPHY_PID_LL1D1_L1D_L)
DEFINE_PID(UPHY_PID_LL1D1_L1U_H)
DEFINE_PID(UPHY_PID_LL1D1_L1U_L)
DEFINE_PID(UPHY_PID_LL1D1_SDR_H)
DEFINE_PID(UPHY_PID_LL1D1_SDR_L)
DEFINE_PID(NPHY_PID_LL1D1_SLOT_PROC_0)
DEFINE_PID(NPHY_PID_LL1D1_LL1U0_MBX_H)
DEFINE_PID(NPHY_PID_LL1D1_LL1U0_MBX_L)
DEFINE_PID(NPHY_PID_LL1D1_UL_DCI_PROC_0)
DEFINE_PID(NPHY_PID_LL1D1_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_PID_LL1D1_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_PID_LL1D1_UL_INT_0)
DEFINE_PID(NPHY_PID_LL1D1_STUB_0)
DEFINE_PID(UPHY_PID_LL1D1_BBA)
DEFINE_PID(NPHY_PID_LL1D1_LPC_0)
DEFINE_PID(UPHY_PID_LL1D1_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_LL1D1_FRM_BBA_H)
DEFINE_PID(UPHY_PID_LL1D1_FRM_BBA_L)
END_CPU_ID_10_PID_DEFINITION()

BEGIN_CPU_ID_11_PID_DEFINITION()
DEFINE_PID(UPHY_PID_LL1D2_OM)
DEFINE_PID(UPHY_PID_LL1D2_L1C_H)
DEFINE_PID(UPHY_PID_LL1D2_L1C_L)
DEFINE_PID(UPHY_PID_LL1D2_L1D_H)
DEFINE_PID(UPHY_PID_LL1D2_L1D_L)
DEFINE_PID(UPHY_PID_LL1D2_L1U_H)
DEFINE_PID(UPHY_PID_LL1D2_L1U_L)
DEFINE_PID(UPHY_PID_LL1D2_SDR_H)
DEFINE_PID(UPHY_PID_LL1D2_SDR_L)
DEFINE_PID(NPHY_PID_LL1D2_SLOT_PROC_0)
DEFINE_PID(NPHY_PID_LL1D2_LL1U0_MBX_H)
DEFINE_PID(NPHY_PID_LL1D2_LL1U0_MBX_L)
DEFINE_PID(NPHY_PID_LL1D2_UL_DCI_PROC_0)
DEFINE_PID(NPHY_PID_LL1D2_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_PID_LL1D2_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_PID_LL1D2_UL_INT_0)
DEFINE_PID(NPHY_PID_LL1D2_STUB_0)
DEFINE_PID(UPHY_PID_LL1D2_BBA)
DEFINE_PID(NPHY_PID_LL1D2_LPC_0)
DEFINE_PID(UPHY_PID_LL1D2_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_LL1D2_FRM_BBA_H)
DEFINE_PID(UPHY_PID_LL1D2_FRM_BBA_L)
END_CPU_ID_11_PID_DEFINITION()

BEGIN_CPU_ID_12_PID_DEFINITION()
DEFINE_PID(UPHY_PID_LL1D3_OM)
DEFINE_PID(UPHY_PID_LL1D3_L1C_H)
DEFINE_PID(UPHY_PID_LL1D3_L1C_L)
DEFINE_PID(UPHY_PID_LL1D3_L1D_H)
DEFINE_PID(UPHY_PID_LL1D3_L1D_L)
DEFINE_PID(UPHY_PID_LL1D3_L1U_H)
DEFINE_PID(UPHY_PID_LL1D3_L1U_L)
DEFINE_PID(UPHY_PID_LL1D3_SDR_H)
DEFINE_PID(UPHY_PID_LL1D3_SDR_L)
DEFINE_PID(NPHY_PID_LL1D3_SLOT_PROC_0)
DEFINE_PID(NPHY_PID_LL1D3_LL1U0_MBX_H)
DEFINE_PID(NPHY_PID_LL1D3_LL1U0_MBX_L)
DEFINE_PID(NPHY_PID_LL1D3_UL_DCI_PROC_0)
DEFINE_PID(NPHY_PID_LL1D3_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_PID_LL1D3_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_PID_LL1D3_UL_INT_0)
DEFINE_PID(NPHY_PID_LL1D3_STUB_0)
DEFINE_PID(UPHY_PID_LL1D3_BBA)
DEFINE_PID(NPHY_PID_LL1D3_LPC_0)
DEFINE_PID(UPHY_PID_LL1D3_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_LL1D3_FRM_BBA_H)
DEFINE_PID(UPHY_PID_LL1D3_FRM_BBA_L)
END_CPU_ID_12_PID_DEFINITION()

BEGIN_CPU_ID_13_PID_DEFINITION()
DEFINE_PID(UPHY_PID_LL1U0_OM)
DEFINE_PID(UPHY_PID_LL1U0_L1C_H)
DEFINE_PID(UPHY_PID_LL1U0_L1C_L)
DEFINE_PID(UPHY_PID_LL1U0_L1D_H)
DEFINE_PID(UPHY_PID_LL1U0_L1D_L)
DEFINE_PID(UPHY_PID_LL1U0_L1U_H)
DEFINE_PID(UPHY_PID_LL1U0_L1U_L)
DEFINE_PID(UPHY_PID_LL1U0_SDR_H)
DEFINE_PID(UPHY_PID_LL1U0_SDR_L)
DEFINE_PID(NPHY_UL_PID_SLOT_PROC_0)
DEFINE_PID(NPHY_UL_PID_LL1U0_MBX_H)
DEFINE_PID(NPHY_UL_PID_LL1U0_MBX_L)
DEFINE_PID(NPHY_UL_PID_UL_DCI_PROC_0)
DEFINE_PID(NPHY_UL_PID_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_UL_PID_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_UL_PID_UL_INT_0)
DEFINE_PID(NPHY_UL_PID_STUB_0)
DEFINE_PID(UPHY_PID_LL1U0_BBA)
DEFINE_PID(NPHY_UL_PID_LPC_0)
DEFINE_PID(UPHY_PID_LL1U0_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_LL1U0_FRM_BBA_H)
DEFINE_PID(UPHY_PID_LL1U0_FRM_BBA_L)
END_CPU_ID_13_PID_DEFINITION()

BEGIN_CPU_ID_14_PID_DEFINITION()
DEFINE_PID(UPHY_PID_LL1U1_OM)
DEFINE_PID(UPHY_PID_LL1U1_L1C_H)
DEFINE_PID(UPHY_PID_LL1U1_L1C_L)
DEFINE_PID(UPHY_PID_LL1U1_L1D_H)
DEFINE_PID(UPHY_PID_LL1U1_L1D_L)
DEFINE_PID(UPHY_PID_LL1U1_L1U_H)
DEFINE_PID(UPHY_PID_LL1U1_L1U_L)
DEFINE_PID(UPHY_PID_LL1U1_SDR_H)
DEFINE_PID(UPHY_PID_LL1U1_SDR_L)
DEFINE_PID(NPHY_UL_PID_SLOT_PROC_1)
DEFINE_PID(NPHY_UL_PID_LL1U1_MBX_H)
DEFINE_PID(NPHY_UL_PID_LL1U1_MBX_L)
DEFINE_PID(NPHY_UL_PID_UL_DCI_PROC_1)
DEFINE_PID(NPHY_UL_PID_DL_DCI_AGGR_1)
DEFINE_PID(NPHY_UL_PID_UL_UCI_RSLT_COLLECT_1)
DEFINE_PID(NPHY_UL_PID_UL_INT_1)
DEFINE_PID(NPHY_UL_PID_STUB_1)
DEFINE_PID(UPHY_PID_LL1U1_BBA)
DEFINE_PID(NPHY_UL_PID_LPC_1)
DEFINE_PID(UPHY_PID_LL1U1_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_LL1U1_FRM_BBA_H)
DEFINE_PID(UPHY_PID_LL1U1_FRM_BBA_L)
END_CPU_ID_14_PID_DEFINITION()

BEGIN_CPU_ID_15_PID_DEFINITION()
DEFINE_PID(UPHY_PID_SDR0_OM)
DEFINE_PID(UPHY_PID_SDR0_L1C_H)
DEFINE_PID(UPHY_PID_SDR0_L1C_L)
DEFINE_PID(UPHY_PID_SDR0_L1D_H)
DEFINE_PID(UPHY_PID_SDR0_L1D_L)
DEFINE_PID(UPHY_PID_SDR0_L1U_H)
DEFINE_PID(UPHY_PID_SDR0_L1U_L)
DEFINE_PID(UPHY_PID_SDR0_SDR_H)
DEFINE_PID(UPHY_PID_SDR0_SDR_L)
DEFINE_PID(NPHY_PID_SDR0_SLOT_PROC_0)
DEFINE_PID(NPHY_PID_SDR0_LL1U0_MBX_H)
DEFINE_PID(NPHY_PID_SDR0_LL1U0_MBX_L)
DEFINE_PID(NPHY_PID_SDR0_UL_DCI_PROC_0)
DEFINE_PID(NPHY_PID_SDR0_DL_DCI_AGGR_0)
DEFINE_PID(NPHY_PID_SDR0_UL_UCI_RSLT_COLLECT_0)
DEFINE_PID(NPHY_PID_SDR0_UL_INT_0)
DEFINE_PID(NPHY_PID_SDR0_STUB_0)
DEFINE_PID(UPHY_PID_SDR0_BBA)
DEFINE_PID(NPHY_PID_SDR0_LPC_0)
DEFINE_PID(UPHY_PID_SDR0_BBA_PMCTRL)
DEFINE_PID(UPHY_PID_SDR0_FRM_BBA_H)
DEFINE_PID(UPHY_PID_SDR0_FRM_BBA_L)
END_CPU_ID_15_PID_DEFINITION()

BEGIN_CPU_ID_16_PID_DEFINITION()
DEFINE_PID(I0_UEPS_PID_NRLC_UL_RT)
DEFINE_PID(I0_UEPS_PID_NMAC_UL_RT)
DEFINE_PID(I0_UEPS_PID_NMAC_UL_NRT)
DEFINE_PID(I0_UEPS_PID_NPDCP_DL_RT)
DEFINE_PID(I0_UEPS_PID_NRLC_DL_RT)
DEFINE_PID(I0_UEPS_PID_NMAC_DL)
DEFINE_PID(MSP_PID_DIAG_HAC_AGENT)
DEFINE_PID(I0_UEPS_PID_NMACMSG_DISPATCH)
END_CPU_ID_16_PID_DEFINITION()

BEGIN_CPU_ID_17_PID_DEFINITION()
DEFINE_PID(UPHY_PID_FRM_BBA_H)
DEFINE_PID(UPHY_PID_FRM_BBA_L)
DEFINE_PID(UPHY_PID_FRM_BBA_PMCTRL)
END_CPU_ID_17_PID_DEFINITION()

BEGIN_CPU_ID_18_PID_DEFINITION()
DEFINE_PID(DSP_PID_LTEV_VIRTUAL_PID)
END_CPU_ID_18_PID_DEFINITION()


#if defined (INSTANCE_1)
#define WUEPS_PID_USIM  I1_WUEPS_PID_USIM
#define MAPS_STK_PID    I1_MAPS_STK_PID
#define MAPS_PIH_PID    I1_MAPS_PIH_PID
#define MAPS_PB_PID     I1_MAPS_PB_PID
#define MAPS_EMAT_PID   I1_MAPS_EMAT_PID
#define WUEPS_PID_SLEEP I1_WUEPS_PID_SLEEP
#define WUEPS_PID_AWAKE I1_WUEPS_PID_AWAKE
#define UEPS_PID_DL     I1_UEPS_PID_DL
#define UEPS_PID_LL     I1_UEPS_PID_LL
#define UEPS_PID_SN     I1_UEPS_PID_SN
#define UEPS_PID_GRM    I1_UEPS_PID_GRM
#define WUEPS_PID_BMC   I1_WUEPS_PID_BMC
#define WUEPS_PID_PDCP  I1_WUEPS_PID_PDCP
#define WUEPS_PID_RLC   I1_WUEPS_PID_RLC
#define WUEPS_PID_MAC   I1_WUEPS_PID_MAC
#define WUEPS_PID_RLCDL I1_WUEPS_PID_RLCDL
#define WUEPS_PID_MACDL I1_WUEPS_PID_MACDL
#define DSP_PID_GPHY    I1_DSP_PID_GPHY
#define DSP_PID_GPHYLOW I1_DSP_PID_GPHYLOW
#define DSP_PID_SLEEP   I1_DSP_PID_SLEEP
#define DSP_PID_WAKE    I1_DSP_PID_WAKE
#define DSP_PID_IDLE    I1_DSP_PID_IDLE
#define DSP_PID_APM     I1_DSP_PID_APM
#define DSP_PID_WPHY    I1_DSP_PID_WPHY
#define UEPS_PID_GAS    I1_UEPS_PID_GAS
#define WUEPS_PID_MM    I1_WUEPS_PID_MM
#define WUEPS_PID_MMC   I1_WUEPS_PID_MMC
#define UEPS_PID_REGM   I1_UEPS_PID_REGM
#define WUEPS_PID_GMM   I1_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I1_WUEPS_PID_MMA
#define UEPS_PID_DSM    I1_UEPS_PID_DSM
#define WUEPS_PID_CC    I1_WUEPS_PID_CC
#define WUEPS_PID_SS    I1_WUEPS_PID_SS
#define WUEPS_PID_TC    I1_WUEPS_PID_TC
#define WUEPS_PID_SMS   I1_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I1_WUEPS_PID_RABM
#define WUEPS_PID_SM    I1_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I1_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I1_WUEPS_PID_TAF
#define WUEPS_PID_VC    I1_WUEPS_PID_VC
#define UEPS_PID_MTA    I1_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I1_WUEPS_PID_DRV_AGENT
#define UEPS_PID_XSMS   I1_UEPS_PID_XSMS
#define UEPS_PID_CCM    I1_UEPS_PID_CCM
#define UEPS_PID_GUCALL I1_UEPS_PID_GUCALL
#define UEPS_PID_XCALL  I1_UEPS_PID_XCALL
#define UEPS_PID_MSCC   I1_UEPS_PID_MSCC
#define WUEPS_PID_CSIMA I1_WUEPS_PID_CSIMA
#define UEPS_PID_XPDS   I1_UEPS_PID_XPDS
#define WUEPS_PID_LCS   I1_WUEPS_PID_LCS
#define PS_PID_NAS_COMM   I1_PS_PID_NAS_COMM
#define WUEPS_PID_WRR   I1_WUEPS_PID_WRR
#define WUEPS_PID_WCOM  I1_WUEPS_PID_WCOM

#define MSP_L4_L4A_PID  I1_MSP_L4_L4A_PID
#define MSP_LDSP_PID    I1_MSP_LDSP_PID
#define MSP_SYS_FTM_PID I1_MSP_SYS_FTM_PID

#define PS_PID_ERRC     I1_PS_PID_ERRC
#define PS_PID_ERMM     I1_PS_PID_ERMM
#define PS_PID_OM       I1_PS_PID_OM
#define PS_PID_HPA      I1_PS_PID_HPA
#define PS_PID_MAC_UL   I1_PS_PID_MAC_UL
#define PS_PID_MAC_DL   I1_PS_PID_MAC_DL
#define PS_PID_RLC_UL   I1_PS_PID_RLC_UL
#define PS_PID_RLC_DL   I1_PS_PID_RLC_DL
#define PS_PID_PDCP_UL  I1_PS_PID_PDCP_UL
#define PS_PID_PDCP_DL  I1_PS_PID_PDCP_DL
#define PS_PID_LPP      I1_PS_PID_LPP
#define PS_PID_MM       I1_PS_PID_MM
#define PS_PID_ESM      I1_PS_PID_ESM
#define PS_PID_RABM     I1_PS_PID_RABM
#define PS_PID_TC       I1_PS_PID_TC
#define PS_PID_IP       I1_PS_PID_IP
#define PS_PID_IMSA     I1_PS_PID_IMSA
#define PS_PID_IMSVA    I1_PS_PID_IMSVA
#define PS_PID_PTT      I1_PS_PID_PTT

#define DSP_PID_RTTSTUB     I1_DSP_PID_RTTSTUB
#define DSP_PID_RTTMATCH    I1_DSP_PID_RTTMATCH
#define DSP_PID_RTTPREEMPT  I1_DSP_PID_RTTPREEMPT
#define DSP_PID_RTTONLINE   I1_DSP_PID_RTTONLINE
#elif defined (INSTANCE_2)
#define WUEPS_PID_USIM  I2_WUEPS_PID_USIM
#define MAPS_STK_PID    I2_MAPS_STK_PID
#define MAPS_PIH_PID    I2_MAPS_PIH_PID
#define MAPS_PB_PID     I2_MAPS_PB_PID
#define MAPS_EMAT_PID   I2_MAPS_EMAT_PID
#define WUEPS_PID_SLEEP I2_WUEPS_PID_SLEEP
#define WUEPS_PID_AWAKE I2_WUEPS_PID_AWAKE
#define UEPS_PID_DL     I2_UEPS_PID_DL
#define UEPS_PID_LL     I2_UEPS_PID_LL
#define UEPS_PID_SN     I2_UEPS_PID_SN
#define UEPS_PID_GRM    I2_UEPS_PID_GRM
#define DSP_PID_GPHY    I2_DSP_PID_GPHY
#define DSP_PID_GPHYLOW I2_DSP_PID_GPHYLOW
#define DSP_PID_SLEEP   I2_DSP_PID_SLEEP
#define DSP_PID_WAKE    I2_DSP_PID_WAKE
#define DSP_PID_IDLE    I2_DSP_PID_IDLE
#define DSP_PID_APM     I2_DSP_PID_APM
#define UEPS_PID_GAS    I2_UEPS_PID_GAS
#define WUEPS_PID_MM    I2_WUEPS_PID_MM
#define WUEPS_PID_MMC   I2_WUEPS_PID_MMC
#define UEPS_PID_REGM   I2_UEPS_PID_REGM
#define WUEPS_PID_GMM   I2_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I2_WUEPS_PID_MMA
#define UEPS_PID_DSM    I2_UEPS_PID_DSM
#define WUEPS_PID_CC    I2_WUEPS_PID_CC
#define WUEPS_PID_SS    I2_WUEPS_PID_SS
#define WUEPS_PID_TC    I2_WUEPS_PID_TC
#define WUEPS_PID_SMS   I2_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I2_WUEPS_PID_RABM
#define WUEPS_PID_SM    I2_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I2_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I2_WUEPS_PID_TAF
#define WUEPS_PID_VC    I2_WUEPS_PID_VC
#define UEPS_PID_MTA    I2_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I2_WUEPS_PID_DRV_AGENT
#define UEPS_PID_MSCC   I2_UEPS_PID_MSCC
#define UEPS_PID_XSMS   I2_UEPS_PID_XSMS
#define UEPS_PID_CCM    I2_UEPS_PID_CCM
#define UEPS_PID_GUCALL I2_UEPS_PID_GUCALL
#define UEPS_PID_XCALL  I2_UEPS_PID_XCALL
#define WUEPS_PID_CSIMA I2_WUEPS_PID_CSIMA
#define UEPS_PID_XPDS   I2_UEPS_PID_XPDS
#define WUEPS_PID_LCS   I2_WUEPS_PID_LCS
#define PS_PID_NAS_COMM   I2_PS_PID_NAS_COMM
#define WUEPS_PID_WRR   I0_WUEPS_PID_WRR
#define WUEPS_PID_WCOM  I0_WUEPS_PID_WCOM
#define DSP_PID_WPHY    I0_DSP_PID_WPHY
#define WUEPS_PID_BMC   I0_WUEPS_PID_BMC
#define WUEPS_PID_PDCP  I0_WUEPS_PID_PDCP
#define WUEPS_PID_RLC   I0_WUEPS_PID_RLC
#define WUEPS_PID_MAC   I0_WUEPS_PID_MAC
#define WUEPS_PID_RLCDL I0_WUEPS_PID_RLCDL
#define WUEPS_PID_MACDL I0_WUEPS_PID_MACDL

#define MSP_L4_L4A_PID  I0_MSP_L4_L4A_PID
#define MSP_LDSP_PID    I0_MSP_LDSP_PID
#define MSP_SYS_FTM_PID I0_MSP_SYS_FTM_PID

#define PS_PID_ERRC     I0_PS_PID_ERRC
#define PS_PID_ERMM     I0_PS_PID_ERMM
#define PS_PID_OM       I0_PS_PID_OM
#define PS_PID_HPA      I0_PS_PID_HPA
#define PS_PID_MAC_UL   I0_PS_PID_MAC_UL
#define PS_PID_MAC_DL   I0_PS_PID_MAC_DL
#define PS_PID_RLC_UL   I0_PS_PID_RLC_UL
#define PS_PID_RLC_DL   I0_PS_PID_RLC_DL
#define PS_PID_PDCP_UL  I0_PS_PID_PDCP_UL
#define PS_PID_PDCP_DL  I0_PS_PID_PDCP_DL
#define PS_PID_LPP      I0_PS_PID_LPP
#define PS_PID_MM       I0_PS_PID_MM
#define PS_PID_ESM      I0_PS_PID_ESM
#define PS_PID_RABM     I0_PS_PID_RABM
#define PS_PID_TC       I0_PS_PID_TC
#define PS_PID_IP       I0_PS_PID_IP
#define PS_PID_IMSA     I0_PS_PID_IMSA
#define PS_PID_IMSVA    I0_PS_PID_IMSVA
#define PS_PID_PTT      I0_PS_PID_PTT

#else
#define WUEPS_PID_USIM  I0_WUEPS_PID_USIM
#define MAPS_STK_PID    I0_MAPS_STK_PID
#define MAPS_PIH_PID    I0_MAPS_PIH_PID
#define MAPS_PB_PID     I0_MAPS_PB_PID
#define MAPS_EMAT_PID   I0_MAPS_EMAT_PID
#define WUEPS_PID_SLEEP I0_WUEPS_PID_SLEEP
#define WUEPS_PID_AWAKE I0_WUEPS_PID_AWAKE
#define UEPS_PID_GAS    I0_UEPS_PID_GAS
#define UEPS_PID_DL     I0_UEPS_PID_DL
#define UEPS_PID_LL     I0_UEPS_PID_LL
#define UEPS_PID_SN     I0_UEPS_PID_SN
#define UEPS_PID_GRM    I0_UEPS_PID_GRM
#define WUEPS_PID_BMC   I0_WUEPS_PID_BMC
#define WUEPS_PID_PDCP  I0_WUEPS_PID_PDCP
#define WUEPS_PID_RLC   I0_WUEPS_PID_RLC
#define WUEPS_PID_MAC   I0_WUEPS_PID_MAC
#define WUEPS_PID_RLCDL I0_WUEPS_PID_RLCDL
#define WUEPS_PID_MACDL I0_WUEPS_PID_MACDL
#define DSP_PID_GPHY    I0_DSP_PID_GPHY
#define DSP_PID_GPHYLOW I0_DSP_PID_GPHYLOW
#define DSP_PID_SLEEP   I0_DSP_PID_SLEEP
#define DSP_PID_WAKE    I0_DSP_PID_WAKE
#define DSP_PID_IDLE    I0_DSP_PID_IDLE
#define DSP_PID_APM     I0_DSP_PID_APM
#define DSP_PID_WPHY    I0_DSP_PID_WPHY
#define WUEPS_PID_MM    I0_WUEPS_PID_MM
#define WUEPS_PID_MMC   I0_WUEPS_PID_MMC
#define UEPS_PID_REGM   I0_UEPS_PID_REGM
#define WUEPS_PID_GMM   I0_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I0_WUEPS_PID_MMA
#define UEPS_PID_DSM    I0_UEPS_PID_DSM
#define WUEPS_PID_CC    I0_WUEPS_PID_CC
#define WUEPS_PID_SS    I0_WUEPS_PID_SS
#define WUEPS_PID_TC    I0_WUEPS_PID_TC
#define WUEPS_PID_SMS   I0_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I0_WUEPS_PID_RABM
#define WUEPS_PID_SM    I0_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I0_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I0_WUEPS_PID_TAF
#define WUEPS_PID_VC    I0_WUEPS_PID_VC
#define UEPS_PID_MTA    I0_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I0_WUEPS_PID_DRV_AGENT
#define UEPS_PID_XSMS   I0_UEPS_PID_XSMS
#define UEPS_PID_CCM    I0_UEPS_PID_CCM
#define UEPS_PID_GUCALL I0_UEPS_PID_GUCALL
#define UEPS_PID_XCALL  I0_UEPS_PID_XCALL

#define UEPS_PID_MSCC   I0_UEPS_PID_MSCC
#define WUEPS_PID_CSIMA I0_WUEPS_PID_CSIMA

#define UEPS_PID_XPDS   I0_UEPS_PID_XPDS
#define WUEPS_PID_LCS   I0_WUEPS_PID_LCS
#define PS_PID_NAS_COMM   I0_PS_PID_NAS_COMM
#define WUEPS_PID_WRR   I0_WUEPS_PID_WRR
#define WUEPS_PID_WCOM  I0_WUEPS_PID_WCOM

#define MSP_L4_L4A_PID  I0_MSP_L4_L4A_PID
#define MSP_LDSP_PID    I0_MSP_LDSP_PID
#define MSP_SYS_FTM_PID I0_MSP_SYS_FTM_PID

#ifdef FEATURE_NPHY_START
#define PHY_PID_STARTUP PHY_PID_STARTUP1
#else
#define PHY_PID_STARTUP NPHY_PID_STARTUP
#endif

#define PS_PID_ERRC     I0_PS_PID_ERRC
#define PS_PID_ERMM     I0_PS_PID_ERMM
#define PS_PID_OM       I0_PS_PID_OM
#define PS_PID_HPA      I0_PS_PID_HPA
#define PS_PID_MAC_UL   I0_PS_PID_MAC_UL
#define PS_PID_MAC_DL   I0_PS_PID_MAC_DL
#define PS_PID_RLC_UL   I0_PS_PID_RLC_UL
#define PS_PID_RLC_DL   I0_PS_PID_RLC_DL
#define PS_PID_PDCP_UL  I0_PS_PID_PDCP_UL
#define PS_PID_PDCP_DL  I0_PS_PID_PDCP_DL
#define PS_PID_LPP      I0_PS_PID_LPP
#define PS_PID_MM       I0_PS_PID_MM
#define PS_PID_ESM      I0_PS_PID_ESM
#define PS_PID_RABM     I0_PS_PID_RABM
#define PS_PID_TC       I0_PS_PID_TC
#define PS_PID_IP       I0_PS_PID_IP
#define PS_PID_IMSA     I0_PS_PID_IMSA
#define PS_PID_IMSVA    I0_PS_PID_IMSVA
#define PS_PID_PTT      I0_PS_PID_PTT
#define UEPS_PID_NRMM   I0_UEPS_PID_NRMM
#define UEPS_PID_NRSM   I0_UEPS_PID_NRSM
#define UEPS_PID_PCF    I0_UEPS_PID_PCF
#define UEPS_PID_NREAP  I0_UEPS_PID_NREAP
#define PS_PID_NRTHROT  I0_PS_PID_NRTHROT

#define DSP_PID_RTTSTUB     I0_DSP_PID_RTTSTUB
#define DSP_PID_RTTMATCH    I0_DSP_PID_RTTMATCH
#define DSP_PID_RTTPREEMPT  I0_DSP_PID_RTTPREEMPT
#define DSP_PID_RTTONLINE   I0_DSP_PID_RTTONLINE
#endif

#define UPHY_PID_CSDR_1X_CM         DSP_PID_CSDR_1X_CM_AGENT
#define UPHY_PID_CSDR_1X_SM         DSP_PID_CSDR_1X_SM_AGENT
#define UPHY_PID_CSDR_HRPD_CM       DSP_PID_CSDR_HRPD_CM_AGENT
#define UPHY_PID_CSDR_HRPD_SM       DSP_PID_CSDR_HRPD_SM_AGENT
#define UPHY_PID_GSDR_HIGHPRI       DSP_PID_GSDR_HIGHPRI_AGENT

#if (FEATURE_ON == FEATURE_SUPPORT_5G_RTT_TIME_DOMAIN_BBIT)
#define UEPS_PID_NRRC               UEPS_PID_NRRC_0
#define UEPS_PID_NRRM               UEPS_PID_NRRM_0
#else
#define UEPS_PID_NRRC               UEPS_PID_NRRC_2
#define UEPS_PID_NRRM               UEPS_PID_NRRM_2
#endif

#if (FEATURE_ON == FEATURE_NPHY_STUB_ESL)
#define UPHY_PID_HL1C_L1C_L         UPHY_PID_HL1C_L1C_L_2
#else
#define UPHY_PID_HL1C_L1C_L         UPHY_PID_HL1C_L1C_L_8
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_PID_DEF_H */
