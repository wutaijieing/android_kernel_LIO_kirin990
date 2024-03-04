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

#ifndef __CBTRL_H__
#define __CBTRL_H__

#include "vos.h"
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#ifdef MODEM_FUSION_VERSION
#include "mdrv_diag.h"
#else
#include "msp_diag_comm.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* OM<->APP : NON SIGALING Transparent Message. */
#define APP_OM_NON_SIG_BT_TRAN_REQ (0x1601)
#define OM_APP_NON_SIG_BT_TRAN_CNF (0x1602)

/* OM<->APP : Get the WCDMA SYNC Status. */
#define APP_OM_W_SYNC_STATUS_REQ (0x1603)
#define OM_APP_W_SYNC_STATUS_CNF (0x1604)

/* OM<->APP : Get the GMSK/EDGE DownLink Status. */
#define APP_OM_GE_DL_STATUS_REQ (0x1605)
#define OM_APP_GE_DL_STATUS_CNF (0x1606)

/* OM<->APP : Get the EDGE BLER. */
#define APP_OM_EDGE_BLER_REQ (0x1607)
#define OM_APP_EDGE_BLER_CNF (0x1608)

/* OM<->APP : Get the EDGE BLER. */
#define APP_OM_W_BER_DATA_REQ (0x1609)
#define OM_APP_W_BER_DATA_CNF (0x160a)
#define OM_APP_W_BER_DATA_IND (0x160b)

#define APP_OM_LISTMODE_BT_GETMSG_REQ (0x1611)
#define OM_APP_LISTMODE_BT_GETMSG_CNF (0x1612)

#define APP_OM_LISTMODE_BT_TRAN_REQ (0x1613)
#define OM_APP_LISTMODE_BT_TRAN_CNF (0x1614)

#define APP_OM_LISTMODE_BT_TEST_REQ (0x1615)
#define OM_APP_LISTMODE_BT_TEST_CNF (0x1616)
#define OM_APP_LISTMODE_BT_BER_IND (0x1617)

#define OM_APP_LISTMODE_BT_RSSI_IND (0x1618)

#define APP_OM_BT_W_TX_SET_POWER_REQ (0x1619)
#define OM_APP_BT_W_TX_SET_POWER_CNF (0x1620)

#define APP_OM_BT_WG_RSSI_REQ (0x1621)
#define OM_APP_BT_WG_RSSI_CNF (0x1622)

#define APP_OM_G_BER_DATA_REQ (0x1624)
#define OM_APP_G_BER_DATA_CNF (0x1625)
#define OM_APP_G_BER_DATA_IND (0x1626)

#define APP_OM_NON_SIG_BT_C_SYNC_REQ (0x1701)
#define OM_APP_NON_SIG_BT_C_SYNC_CNF (0x1702)
#define APP_OM_NON_SIG_BT_C_FER_REQ (0x1703)
#define OM_APP_NON_SIG_BT_C_FER_CNF (0x1704)

#define APP_OM_BT_C_TX_SET_POWER_REQ (0x1705)
#define OM_APP_BT_C_TX_SET_POWER_CNF (0x1706)

#define APP_OM_SWITCH_RF_ANT_REQ 0x0068
#define OM_APP_SWITCH_RF_ANT_CNF 0x0068

#if (VOS_OSA_CPU == OSA_CPU_CCPU)
#define CBT_PRINT_SECFUN_RESULT(ret) CBT_PrintSecResult(ret, THIS_FILE_ID, __LINE__)
#endif

#define CBT_STATE_IDLE 0   /* IDLE态，不能正常与工具进行通信 */
#define CBT_STATE_ACTIVE 1 /* ACTIVE态，能够正常工作 */

#define CBT_MSG_ID_COMP_LENGTH (4U)

#define CBT_MSG_HEADER_LENGTH (16U)
#define CBT_MSG_IDLEN_LENGTH (8U)
#define CBT_MSG_HEAD_EX_LENGTH ((CBT_MSG_HEADER_LENGTH) + (CBT_MSG_IDLEN_LENGTH))

#define CBT_MSG_SEGMENT_LEN (4U * 1024U)
#define CBT_MSG_CONTEXT_MAX_LENGTH (CBT_MSG_SEGMENT_LEN - CBT_MSG_HEADER_LENGTH)

#define CBT_CCPU_TO_ACPU_SEND_DATA_REQ (0x030d)

#define CBT_GET_COMPMODE_COMPID(COMPMODE) ((COMPMODE >> 12) & 0xF)
#define CBT_GET_COMPMODE_MODE(COMPMODE) ((COMPMODE >> 8) & 0xF)

#define CBT_MODEM_SSID_COMBINE(modem, ssid) (((modem)&0x0F) | ((ssid << 4) & 0xF0))

#define CBT_POWER_10TH_UNIT (10)
#define CBT_RSSI_8TH_UNIT (8)
#define CBT_POWER_10DBM_THRESHOLD (100)
#define CBT_GSM_BAND_CHAN_COMPOSE(usBand, usChan) (((usBand) << 12) | (0xFFF & (usChan)))
#define CBT_RSSI_TO_POWER(x) (((x)*CBT_POWER_10TH_UNIT) / CBT_RSSI_8TH_UNIT)
#define CBT_CALL_GSM_RX_LEVEL_PARA (316)
#define CBT_WCDMA_FREQ_MEASURE_PARA (65535)

#define CBT_WRITE_NV_HEAD_SIZE (8)
#define CBT_WRITE_NV_FLASH_HEAD_SIZE (8)

#define OM_BER_DATA_MAX_SIZE (1024)
#define CBT_NV_TO_FLASH_HEAD_SIZE (4)
#define OM_STATE_IDLE 0   /* IDLE态，OM不能正常与工具进行通信 */
#define OM_STATE_ACTIVE 1 /* ACTIVE态，OM能够正常工作 */

/* OM<->APP : Just for LMT. */
#define APP_OM_WRITE_NV_REQ 0x8023
#define OM_APP_WRITE_NV_CNF 0x8024

/* OM<->APP : Just for LMT. */
#define APP_OM_HANDLE_LMT_REQ 0x80a1
#define OM_APP_HANDLE_LMT_CNF 0x80a2

/* OM<->APP : Just for LMT. */
#define APP_OM_ACTIVE_PHY_REQ 0x80a3
#define OM_APP_ACTIVE_PHY_CNF 0x80a4

/* OM<->APP :查询PA的属性 */
#define APP_OM_PA_ATTRIBUTE_REQ 0x80c1
#define OM_APP_PA_ATTRIBUTE_IND 0x80c2

/* OM<->APP :通过物理通道查询PA的温度 */
#define APP_OM_PA_TEMP_PHY_CHAN_REQ 0x80c7
#define OM_APP_PA_TEMP_PHY_CHAN_IND 0x80c8

/* 设置从信令下切换到非信令下 */
#define APP_OM_SET_FTM_REQ 0x80e5
#define OM_APP_SET_FTM_CNF 0x80e6

/* OM<->APP :LMT查询DSDA支持状态 */
#define APP_OM_QUERY_MODEM_NUM_REQ 0x8211
#define OM_APP_QUERY_MODEM_NUM_CNF 0x8212

#define APP_OM_AT_TMODE_REQ 0x0065
#define OM_APP_AT_TMODE_CNF 0x0065

#define APP_OM_AT_BANDSW_REQ 0x0066
#define OM_APP_AT_BANDSW_CNF 0x0066

#define APP_OM_AT_FCHAN_REQ 0x0067
#define OM_APP_AT_FCHAN_CNF 0x0067

/* OM<->APP : Restablish link */
#define APP_OM_ESTABLISH_REQ 0x80b1
#define OM_APP_ESTABLISH_CNF 0x80b2

/* OM<->APP : Release link */
#define APP_OM_RELEASE_REQ 0x80b3
#define OM_APP_RELEASE_CNF 0x80b4

#define OM_TRANS_PRIMID 0x5001
#define TRANS_MSG_ID 0x6001

/* OM<->APP :设置C核NV存储到Flash */
#define APP_OM_NV_TO_FLASH_REQ 0x802B
#define OM_APP_NV_TO_FLASH_CNF 0x802C

/* OM<->APP :设置C核一次性写NV存储Flash */
#define APP_OM_NV_WRITE_FLASH_REQ 0x802D
#define OM_APP_NV_WRITE_FLASH_CNF 0x802E

/* GUCCBT->NRNOSIG发送IND，NR核融合后该消息删除 */
#define ID_GUCCBT_NRNOSIG_IND 0x0F0F

/* 非CT场景下的写nv消息id */
#define TOOL_UE_WRITE_NV_REQ                       0x9023
#define UE_TOOL_WRITE_NV_CNF                       0x9024
#define TOOL_UE_NV_WRITE_FLASH_REQ                 0x902D
#define UE_TOOL_NV_WRITE_FLASH_CNF                 0x902E

/* RTTAgent TMode Defines */
#define RTTAGENT_CBT_TMODE1 1    // LTE 主模 CT 校准TDS
#define RTTAGENT_CBT_TMODE13 13  // LTE 主模 CT
#define RTTAGENT_CBT_TMODE14 14  // LTE 主模 BT
#define RTTAGENT_CBT_TMODE15 15  // LTE 从模 CT
#define RTTAGENT_CBT_TMODE16 16  // LTE 从模 BT
#define RTTAGENT_CBT_TMODE17 17  // TDS 主模
#define RTTAGENT_CBT_TMODE18 18  // TDS 主模
#define RTTAGENT_CBT_TMODE20 20  // TDS 从模

/* Query PA attribute */
enum {
    OM_W_PA_TEMP = 1,
    OM_G_PA_TEMP,
    OM_W_PLL_LOCK,
    OM_G_PLL_LOCK,
    OM_W_HKADC,
    OM_G_HKADC,
    OM_W_BBP_PLL_LOCK,
    OM_G_BBP_PLL_LOCK,
    OM_DSP_PLL_LOCK,
    OM_ARM_PLL_LOCK,
    OM_SIM_TEMP,
    OM_TCM_STATUS,
    OM_SDMMC_STATUS,
    OM_BATTER_VOLT,
    OM_BATTER_TEMP,
    OM_OLED_TEMP,
    OM_DCXO_TEMP,
    OM_DCXO_TEMP_LT,
    OM_OLED_BUTT
};

typedef enum {
    RFIC_CHANNEL_FLAG_MODEM          = 0,
    RFIC_CHANNEL_FLAG_CHANNEL_NO     = 1,
    RFIC_CHANNEL_FLAG_BUSSINESS_TYPE = 2,
    RFIC_CHANNEL_FLAG_BUTT
} OM_RficChannelFlag;
typedef VOS_UINT8 OM_RficChannelFlagUint8;

enum OM_WSnycStatus {
    OM_W_SYNC_STATUS_SYNC = 0,
    OM_W_SYNC_STATUS_OUT_OF_SYNC,
    OM_W_SYNC_STATUS_UNKNOWN,
    OM_W_SYNC_STATUS_BUTT
};

typedef VOS_UINT16 OM_WSnycStatusUint16;

enum OM_GeSnycStatus {
    OM_GE_SYNC_STATUS_NORMAL,
    OM_GE_SYNC_STATUS_DISNORMAL,
    OM_GE_SYNC_STATUS_UNKNOWN,
    OM_GE_SYNC_STATUS_BUTT
};

enum CBT_CSyncStatus {
    CBT_C_SYNC_STATUS_SYNC = 0,
    CBT_C_SYNC_STATUS_OUT_OF_SYNC,
    CBT_C_SYNC_STATUS_UNKNOWN,
    CBT_C_SYNC_STATUS_BUTT
};

enum LMT_RatMode {
    LMT_RAT_WCDMA = 0,
    LMT_RAT_GSM,
    LMT_RAT_CDMA,
    LMT_RAT_TDS,
    LMT_RAT_LTE,
    LMT_RAT_NR,
    LMT_RAT_BUTT
};

/* ComponentType使用 */
typedef enum {
    CBT_MODE_LTE = 0,
    CBT_MODE_TDS,
    CBT_MODE_GSM,
    CBT_MODE_UMTS,
    CBT_MODE_CDMA,
    CBT_MODE_NR,
    CBT_MODE_LTEV2X = 7,
    CBT_MODE_BUTT,
    CBT_MODE_COMM   = 0xf,
} CBT_ComponentMode;

typedef enum {
    CBT_MT_REQ = 1,
    CBT_MT_CNF,
    CBT_MT_IND,
    CBT_MT_BUTT

} CBT_MsgType;

typedef enum {
    CBT_SSID_NOAPPOINT = 0,
    CBT_SSID_APP_CPU,
    CBT_SSID_MODEM_CPU,
    CBT_SSID_LTE_DSP,
    CBT_SSID_LTE_BBP,
    CBT_SSID_GU_DSP,
    CBT_SSID_HIFI,
    CBT_SSID_TDS_DSP,
    CBT_SSID_TDS_BBP,
    CBT_SSID_MCU,
    CBT_SSID_GPU,
    CBT_SSID_GU_BBP,
    CBT_SSID_IOM3,
    CBT_SSID_ISP,
    CBT_SSID_X_DSP,
    CBT_SSID_RESERVE,
    CBT_SSID_BUTT
} CBT_Ssid;

typedef enum {
    CBT_COMP_FUNC = 1,
    CBT_COMP_NOSIG,
    CBT_COMP_PHY,
    CBT_COMP_PS,
    CBT_COMP_TRANS,
    CBT_COMP_CAL,
    CBT_COMP_BUTT
} CBT_ComponetId;

typedef enum {
    CBT_TEST_MODE_INVALID = 0xFFFFFFFF,
    CBT_TEST_MODE_CT      = 0,
    CBT_TEST_MODE_BT      = 1,
    CBT_TEST_MODE_BUTT
} CBT_TestMode;

/*
 * 结构说明: 定义包含modem和ssid的数据结构
 */
typedef struct {
    VOS_UINT8 modem : 3; /* modem0: 0  modem1: 1 */
    VOS_UINT8 resv : 1;
    VOS_UINT8 ssid : 4;
} CBT_ModemSsid;

/*
 * 结构说明: 定义包含消息类型和消息分段信息的数据结构
 */
typedef struct {
    VOS_UINT8 msgType : 2;   /* 消息类型，REQ: 1   CNF: 2   IND: 3 */
    VOS_UINT8 fragIndex : 4; /* 消息分段的片段索引 */
    VOS_UINT8 eof : 1;       /* 分段结束标识，0分段未结束，1分段结束 */
    VOS_UINT8 fragFlag : 1;  /* 是否分段标识，0不分段，1分段 */
} CBT_FragmentInfo;

/*
 * 结构说明: 定义包含时间戳的数据结构
 */
typedef struct {
    VOS_UINT32 timestampL;
    VOS_UINT16 timestampH;
    VOS_UINT16 rsv;
} CBT_TimeStamp;

/*
 * 结构说明: 定义包含组件和模的数据结构
 */
typedef struct {
    VOS_UINT8 rsv;
    VOS_UINT8 mode : 4; /* 区分GUTLC模 */
    VOS_UINT8 compId : 4;
} CBT_ComponentModeInfo;

/*
 * 结构说明: 定义OM消息头的数据结构
 */
typedef struct {
    VOS_UINT8        sid; /* 固定为 7 */
    CBT_ModemSsid    modemSsid;
    VOS_UINT8        sessionId; /* 固定为 1 */
    CBT_FragmentInfo msgSegment;
    VOS_UINT32       transId;
    CBT_TimeStamp    timeStamp; /* 时间戳信息 */
} CBT_MsgHead;

/*
 * 结构说明: 工具下发数据包扩展结构
 */
typedef struct {
    CBT_MsgHead       msgHeader;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT8         para[4]; /* 消息内容 */
} CBT_UniformMsg;

/*
 * 结构说明: 工具下发数据包扩展结构
 */
typedef struct {
    VOS_MSG_HEADER
    CBT_UniformMsg pcMsgData;
} CBT_UniformMsgWithHeader;

/*
 * 结构说明: 工具下发数据包扩展结构
 */
typedef struct {
    CBT_MsgHead msgHeader;
    VOS_UINT16  msgId; /* 消息ID */
    VOS_UINT16  compMode;
    VOS_UINT32  msgLength;
    VOS_UINT32  receiverPid; /* 接收消息的模块PID */
    VOS_UINT8   para[4];     /* 消息内容 */
} CBT_TransMsg;

/*
 * 结构说明: 发送给CBT模块的非透传消息数据包结构
 */
typedef struct {
    VOS_UINT16 primId;
    VOS_UINT16 reserve;
    VOS_UINT8  para[4]; /* 消息内容 */
} CBT_HookMsg;

/*
 * 结构说明: 透传给CBT模块的数据包结构
 */
typedef struct {
    VOS_UINT16 transPrimId;
    VOS_UINT16 rsv;
    VOS_UINT16 msgId; /* 指示当前消息类型 */
    VOS_UINT16 compMode;
    VOS_UINT8  para[4]; /* 消息内容 */
} CBT_RcvTransMsg;

/*
 * 结构说明: PHY层或者Agent代理层发送给CBT模块的数据包结构
 */
typedef struct {
    VOS_MSG_HEADER
    VOS_UINT16 msgId; /* 指示当前消息类型 */
    VOS_UINT16 compMode;
    VOS_UINT8  para[4]; /* 消息内容 */
} CBT_SendCommMsg;

/*
 * 结构说明: PHY层或者Agent代理层发送给CBT模块的数据包结构
 */
typedef struct {
    VOS_UINT16 msgId;    /* 指示当前消息类型 */
    VOS_UINT16 compMode; /* 目前未使用 */
    VOS_UINT8  para[4];  /* 消息内容 */
} PS_TO_CbtMsg;

/*
 * 结构说明: 工具下发数据包扩展结构
 */
typedef struct {
    VOS_MSG_HEADER
    VOS_UINT16 primId; /* 消息ID */
    VOS_UINT16 len;
    VOS_UINT8  data[4];
} CBT_CtoaMsg;

/*
 * 结构说明: 写NV请求
 */
typedef struct {
    VOS_UINT32 count;            /* 要写入的NV项个数 */
    VOS_UINT16 nvItemData[2]; /* 包括NVID值、NVID内容的长度、NVID的内容 */
} CBT_WriteNvReq;

/*
 * 结构说明: 写NV结果
 */
typedef struct {
    CBT_MsgHead       msgHead;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT32        errorCode; /* 返回执行结果 */
    VOS_UINT32        errNvId;   /* 返回出现错误的NVID */
} CBT_WriteNvCnf;

/*
 * 结构说明: 写NV刷flash请求
 */
typedef struct {
    VOS_UINT32 count;            /* 要写入的NV项个数 */
    VOS_UINT16 nvItemData[2]; /* 包括NVID值、NVID内容的长度、NVID的内容 */
} CBT_WriteNvFlashReq;

/*
 * 结构说明: 写NV刷flash结果
 */
typedef struct {
    CBT_MsgHead       msgHead;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT32        errorCode; /* 返回执行结果 */
    VOS_UINT32        errNvId;   /* 返回出现错误的NVID */
} CBT_WriteNvFlashCnf;

/*
 * 结构说明: 存储NV到FLASH命令响应包结构
 */
typedef struct {
    CBT_MsgHead       msgHead;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT32        errorCode; /* 返回执行结果 */
} CBT_NvToFlashCnf;

typedef VOS_VOID (*CBT_FUN)(const CBT_UniformMsg *cbtMsg, VOS_UINT16 returnPrimId);

typedef struct {
    CBT_FUN    cbtFun;       /* Reserves the pointer of function handles current msg. */
    VOS_UINT16 primId;       /* Indicates current msg type. */
    VOS_UINT16 returnPrimId; /* Indicates return msg type. */
} CBT_MsgFun;

/*
 * 结构说明: Listmode相关的数据结构
 */
typedef struct {
    CBT_ModemSsid     modemSsid; /* Modem */
    VOS_UINT8         reserve;   /* 保留字 */
    CBT_ComponentModeInfo compMode;  /* 组件ID */

    VOS_UINT32 band;
    VOS_UINT16 txChan;
    VOS_UINT16 rxChan;
    VOS_UINT16 pcl;
    VOS_UINT16 rsv1;

    VOS_UINT16 wSyncStatus;
    VOS_UINT16 cSyncStatus;
    VOS_UINT16 cReportFlag;
    VOS_UINT16 geReportFlag;

    VOS_UINT16 recvPrimId;
    VOS_UINT16 enableReport;
    VOS_UINT16 listModeSegIndex; /* list mode segment index, start by 1 */
    VOS_UINT16 rsv2;
    VOS_UINT32 handOverStartTime;   /* Handover 开始时间: ms */
    VOS_UINT16 enableRssiReport;    /* RSSI上报使能 */
    VOS_UINT16 offSetFrames;        /* Handover后延时触发dsp测量上报RSSI，单位: Frames */
    VOS_UINT16 rssiReportFrames;    /* RSSI上报帧数 */
    VOS_UINT16 trigRssiTimer;       /* 触发dsp测量上报RSSI的计数器 */
    VOS_BOOL   cbtCallSuc;         /* CBT listmode call 结果 */
    VOS_UINT32 cbtCallSucStartTime; /* CBT call 搜到信号的起始时间 */
    VOS_UINT32 cbtCallTimeOut;      /* 仪器触发CBT call 超时时间，单位ms ，0 无须触发 */
    VOS_INT16  cbtCallRxLevel;      /* CBT call 时的下行信号强度，单位0.1dBm */
    VOS_INT16  cbtCallTxPower;      /* CBT call 时的上行信号强度，单位0.1dBm */

    VOS_UINT16 totalFrameNum;      /* 总帧数 */
    VOS_UINT16 curFrameNum;        /* 上报的BER当前帧数 */
    VOS_UINT16 reportFrameNum;     /* 上报的BER总帧数 */
    VOS_UINT16 curReportFrameNum;  /* 上报的BER总帧数 */
    VOS_UINT32 frameStatistic;     /* listmode 测试帧计数 */
    VOS_UINT32 listmodeFrameTotal; /* listmode 已测试帧数 */

    VOS_UINT16 txTotalFrameNum; /* 上报的BER总帧数 */
    VOS_UINT16 txCurFrameNum;   /* 上报的BER当前帧数 */

    VOS_SEM cbtTotalFrameSem;
    VOS_SEM cbtTxSegmentFrameSem;
    VOS_SEM cbtSyncSem;
    VOS_SEM cbtGeDlQualitySem;

#if (FEATURE_ON == FEATURE_GSM_WHOLE_SDR)
    VOS_SEM cbtEdgeBlerSem;
#endif
} CBT_LisiModeCtrl;

/*
 * 结构说明: GSM上报下行质量消息结构
 */
typedef struct {
    CBT_MsgHead header;
    VOS_UINT16  primId; /* Indicates current msg type. */
    VOS_UINT16  toolId; /* Not used now. */
    VOS_UINT32  msgLength;
    VOS_UINT16  berValue[2];
} CBT_GeDlStatus;
/*
 * 结构说明: Ber上报消息结构
 */
typedef struct {
    CBT_MsgHead header;
    VOS_UINT16  primId; /* Indicates current msg type */
    VOS_UINT16  toolId; /* Not used now */
    VOS_UINT32  msgLength;

    VOS_UINT16 totalFrameNum;
    VOS_UINT16 curFrameNum;
    VOS_UINT16 band;
    VOS_UINT16 rxChan;
    VOS_UINT16 crcData;
    VOS_UINT16 listModeSegIndex; /* list mode segment index, start by 1 */
    VOS_UINT32 dataLen;          /* 数据长度,单位:字节, 在数值上是4的整数倍,范围[0..WTTFPHY_MAX_PHY_DATA_REQ_LEN-1] */
    VOS_UINT8  data[OM_BER_DATA_MAX_SIZE]; /* 上行数据块 */
} CBT_ListModeBerInd;

/*
 * 结构说明: C模 Fer上报消息结构
 */
typedef struct {
    CBT_MsgHead header;
    VOS_UINT16  primId; /* Indicates current msg type */
    VOS_UINT16  toolId; /* Not used now */
    VOS_UINT32  msgLength;

    VOS_UINT32 errorCode;
    VOS_UINT16 totalFrameNum;
    VOS_UINT16 badFrameNum;
    VOS_UINT32 totalBitsNum;
    VOS_UINT32 errorBitsNum;
} CBT_CFerDataCnf;

/*
 * 结构说明: G模 Ber上报消息结构
 */
typedef struct {
    CBT_MsgHead header;
    VOS_UINT16  primId; /* Indicates current msg type. */
    VOS_UINT16  toolId; /* Not used now. */
    VOS_UINT32  msgLength;

    VOS_UINT16 totalFrameNum;
    VOS_UINT16 curFrameNum;
    VOS_UINT16 crcData;
    VOS_UINT16 dataLen; /* 数据长度,单位:字节, 在数值上是4的整数倍, 范围[0..WTTFPHY_MAX_PHY_DATA_REQ_LEN-1] */
    VOS_UINT8  data[OM_BER_DATA_MAX_SIZE]; /* 上行数据块 */
} CBT_GBerDataInd;

/*
 * 结构说明: Ber上报消息结构
 */
typedef struct {
    VOS_UINT16 gpioNo;
    VOS_UINT16 gpioLevel;
} CBT_LtSwitchRfAnt;

/*
 * 结构说明: listmode Rssi上报消息结构
 */
typedef struct {
    CBT_MsgHead header;
    VOS_UINT16  primId; /* Indicates current msg type */
    VOS_UINT16  toolId; /* Not used now */
    VOS_UINT32  msgLength;

    VOS_UINT16 band;
    VOS_UINT16 rxChan;
    VOS_UINT16 listModeSegIndex; /* list mode segment index, start by 1 */
    VOS_UINT16 rsv;
    VOS_INT32  rxLevel; /* 上报的RX level 0.125dBm */
    VOS_INT32  rscp;    /* 上报的Rscp 0.125dBm */
} CBT_ListModeRssiInd;

/*
 * 结构说明: 慢速综测Rssi上报消息结构
 */
typedef struct {
    CBT_MsgHead header;
    VOS_UINT16  primId; /* Indicates current msg type */
    VOS_UINT16  toolId; /* Not used now */
    VOS_UINT32  msgLength;

    VOS_UINT32 errorCode;
    VOS_INT32  rxLevel; /* 上报的RX level 0.125dBm */
} CBT_NoSigBtRssiCnf;

/*
 * 结构说明: CBT模块转发给RTT模块的消息
 */
typedef struct {
    CBT_MsgHead       msgHeader;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT16        tMode; /* TMode */
    VOS_UINT16        rsv;
} CBT_RttAgentMsg;

/*
 * 结构说明: 加载LMT的部分参数
 */
typedef struct {
    VOS_UINT32              modem;
    VOS_UINT16              sysMode;
    VOS_BOOL                masterMode;
    VOS_RatModeUint32 msgMode;
    VOS_UINT32              receiverPid;
} CBT_LmtMsgInfo;

typedef VOS_UINT32 (*RTTAgentFunc)(VOS_VOID *pMsg);

/*
 * 结构说明: NR主模BT业务下，usBusiness标识
 */
typedef struct {
    VOS_MSG_HEADER
    VOS_UINT16 msgId;
    VOS_UINT16 business;
} GUCCBT_NrnosigIndMsg;

typedef struct {
    VOS_UINT8 rsv;
    /* 当ucComponentType是OM/MSP, 则无效 0x0: LTE, 0x1: TDS,0x2: GSM, 0x3: UMTS, 0x4: X, 0xF: 与模无关 */
    VOS_UINT8 componentType : 4;
    VOS_UINT8 componentId : 4; /* OM/MSP:0x1, nosig:0x2, PHY:0x3 */
} CBT_ComponentId;

/*
 * 结构说明: NrNosig给GUCCBT回复消息暂定
 */
typedef struct {
    VOS_MSG_HEADER
    VOS_UINT16      msgId; /* 指示当前消息类型 */
    CBT_ComponentId componentId;
    VOS_UINT32      result;
} NRNOSIG_GuccbtMsg;

/* 查询PA属性结构体 */
typedef struct {
    VOS_UINT16 queryType;
    VOS_UINT16 rsv;
    VOS_INT32  queryResult;
} OM_PaItem;

typedef struct {
    VOS_UINT32 result;
    OM_PaItem  paItem[1];
} OM_AppPaAttribute;

typedef struct {
    VOS_UINT16              sysMode; /* rat制式，0xffff表示制式不变，只用于刷新nv */
    VOS_UINT8               nvEnable : 1;
    VOS_UINT8               rficChannel : 4;     /* RFIC通道 */
    OM_RficChannelFlagUint8 rficChannelFlag : 3; /* RFIC通道使能标志, 0 不使能, 1 表示物理通道, 2 表示通道的业务类型 */
    VOS_UINT8               ftmMode : 1;
    VOS_UINT8               rsv2 : 7;
} OM_LmtMsg;

typedef struct {
    VOS_UINT16 sysMode;
    VOS_UINT16 rsv;
} OM_ActivePhy;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
VOS_VOID CBT_PrintSecResult(errno_t ret, VOS_UINT32 fileId, VOS_UINT32 line);

VOS_VOID CBT_SendContentChannel(CBT_ModemSsid modemSsid, CBT_ComponentModeInfo compMode, VOS_UINT16 returnPrimId,
                                CBT_UniformMsg *cbtToPcMsg);

VOS_VOID CBT_SendResultChannel(CBT_ModemSsid modemSsid, CBT_ComponentModeInfo compMode, VOS_UINT16 returnPrimId,
                               VOS_UINT32 result);

VOS_VOID OM_NoSigCtrlInit(VOS_UINT8 modemId);

VOS_VOID CBT_ListModeCtrlInit(VOS_UINT8 modemId);

VOS_VOID CBT_ResetMsgHead(CBT_UniformMsg *cbtToPcMsg);

VOS_UINT32 CBT_SendData(CBT_UniformMsg *msg, VOS_UINT16 msgLen);

VOS_UINT32 CBT_GreenChannel(CBT_ModemSsid modemSsid, CBT_ComponentModeInfo compMode, VOS_UINT16 primId, VOS_UINT8 *data,
                            VOS_UINT16 len);

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
