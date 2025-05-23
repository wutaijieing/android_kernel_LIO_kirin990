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



#ifndef __CBT_APPRL_H__
#define __CBT_APPRL_H__

#include "cbt_rl.h"
#include "mdrv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern VOS_UINT32 g_cbtAcpuDbgFlag;

#define CBT_MSG_FIRST_BYTE (0x07)
#define BIT_LEFTMOVE_N(num) (0x01 << (num))
#define CBT_ACPU_VCOM_CB BIT_LEFTMOVE_N(0)
#define CBT_ACPU_DISPATCH_MSG BIT_LEFTMOVE_N(1)
#define CBT_ACPU_USB_CB BIT_LEFTMOVE_N(2)
/* datatype 字段长度 */
#define CBT_RL_DATATYPE_LEN (0x1)
#define CBT_HDLC_FCS_LEN (2) /* HDLC帧中FCS未转义时的长度(单位: Byte) */
#define CBT_HDLC_BUF_MAX_LEN (CBT_MSG_SEGMENT_LEN + CBT_RL_DATATYPE_LEN + CBT_HDLC_FCS_LEN)

/* 值为CBT_READ_NV_IND_STRU结构体中，sizeof(errorCode) + sizeof(ulNvId) + sizeof(ulCount) */
#define CBT_READ_NV_HEAD_SIZE (12)
/* 长度为NVID的值+NVID长度所占内存长度 */
#define CBT_NV_ITEM_SIZE (4)
#define CBT_IND_RESULT_SIZE (4)
#define CBT_EST_IND_CHIP_ID_SIZE (4)
#define CBT_EST_IND_RSV_LEN (212)

#define CBT_END_FRAME (1)
/* #define SEG_MAX_LEN                         (4*1024) */
/* #define MAXSEGMENT                          (16) */
/* 组包后的最大长度 */
#define CBT_TOTAL_MSG_MAX_LEN (20 * 1024) /* (SEG_MAX_LEN + (SEG_MAX_LEN-CBT_MSG_HEADER_LENGTH)*(MAXSEGMENT-1)) */

#define OM_CBT_SEND_DATA_REQ (0x030d)

#define CBT_ACPU_DEBUG_TRACE(data, dataLen, enable)                                            \
    if (VOS_FALSE != (g_cbtAcpuDbgFlag & enable)) {                                                 \
        VOS_UINT32 omDbgIndex;                                                                      \
        mdrv_debug("<%s> Data Len=%d\n", __FUNCTION__, dataLen);                                    \
        for (omDbgIndex = 0; (omDbgIndex < dataLen) && (omDbgIndex < 5000); omDbgIndex++) { \
            mdrv_debug("%02x \n", *((VOS_UINT8 *)data + omDbgIndex));                            \
        }                                                                                             \
        mdrv_debug("<%s>\n", __FUNCTION__);                                                           \
    }

/* OM<->APP : Restablish link */
#define APP_OM_ESTABLISH_REQ 0x80b1
#define OM_APP_ESTABLISH_CNF 0x80b2

/* OM<->APP : Release link */
#define APP_OM_RELEASE_REQ 0x80b3
#define OM_APP_RELEASE_CNF 0x80b4

#define APP_OM_READ_NV_REQ 0x8021

#define OM_APP_READ_NV_IND 0x8022

#define APP_OM_NV_BACKUP_REQ 0x8025
#define OM_APP_NV_BACKUP_CNF 0x8026

/* OM<->APP : */
#define APP_OM_C1C2_NV_WRITE_REQ 0x8030
#define OM_APP_C1C2_NV_WRITE_CNF 0x8031

enum APP_FUNC_ProcErr {
    APP_FUNC_PROC_ERR_ALLOC_MEM = 0x1001,
    APP_FUNC_PROC_ERR_GET_DRV_VER,
    APP_FUNC_PROC_ERR_ALLOC_MSG,
    APP_FUNC_PROC_ERR_MEM_CPYS,
    APP_FUNC_PROC_ERR_SEND_MSG,

    APP_FUNC_PROC_ERR_ALLOC_BUTT
};
typedef VOS_UINT32 APP_FUNC_ProcErrUint32;

typedef struct {
    CBT_MsgHead       msgHead;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT16        c1Value; /* C1值 */
    VOS_UINT16        c2Value; /* C2值 */
} CBT_SetC1C2ValueReq;

typedef struct {
    VOS_UINT32     result; /* 返回执行结果 */
    VOS_UINT16     chipId;
    VOS_UINT16     rsv;
    msw_ver_info_s swverInfo;
    VOS_UINT8      reserve[CBT_EST_IND_RSV_LEN];
} CBT_EstablishIndPara;

typedef struct {
    CBT_MsgHead          msgHeader;
    VOS_UINT16           msgId; /* 消息ID */
    CBT_ComponentModeInfo    compMode;
    VOS_UINT32           msgLength;
    CBT_EstablishIndPara msgData;
} CBT_EstablishInd;

typedef struct {
    CBT_MsgHead       msgHeader;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT32        linkType; /* 0表示HDLC, 1表示usb */
    VOS_UINT32        rsv[4];
} CBT_EstablishReq;

/*
 * 功能说明: A核发给C核的建链消息
 */
typedef struct {
    VOS_MSG_HEADER
    CBT_EstablishReq data;
} CBT_EstablishReqAtoc;

typedef struct {
    VOS_UINT32 count;        /* 要读取的NV项个数 */
    VOS_UINT16 nvItemId[2]; /* 要读取的NV项ID数组，其中个数为ulCount */
} CBT_ReadNvReq;

typedef struct {
    VOS_UINT32 errorCode;        /* 返回执行结果 */
    VOS_UINT32 errNvId;          /* 返回出现错误的NVID */
    VOS_UINT32 count;            /* 返回的NV项个数 */
    VOS_UINT16 nvItemData[2]; /* 包括NVID值、NVID内容的长度、NVID的内容 */
} CBT_ReadNvInd;

typedef struct {
    CBT_MsgHead       msgHead;
    VOS_UINT16        msgId; /* 消息ID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;

    VOS_UINT32 errorCode; /* 返回执行结果 */
    VOS_UINT32 errNvId;   /* 返回出现错误的NVID */
    VOS_UINT32 count;     /* 返回的NV项个数 */
} CBT_ReadNvCnf;

/*
 * 功能说明: OMRL CBT校准通道HDLC解析控制信息结构(uncache)
 */
typedef struct {
    VOS_UINT8 *buf;     /* 指向缓冲区虚拟首地址 */
    VOS_UINT8 *realBuf; /* 指向缓冲区实首地址 */
    VOS_UINT32 bufSize; /* 保存缓冲区总大小 */
    VOS_UINT32 rsv;     /* Reserve */
} CBT_HdlcEncodeMemCtrl;

/*
 * 功能说明: ACPU ICC通道发送失败的可维可测信息记录结构
 */
typedef struct {
    VOS_UINT32 iccOmSendErrNum;   /* 当前发送的错误记录 */
    VOS_UINT32 iccOmSendErrLen;   /* 当前发送的错误总消息长度 */
    VOS_UINT32 iccOmSendErrSlice; /* 当前发送的错误消息最后一条数据的时间 */
} CBT_AcpuIccSendFail;

/*
 * 功能说明: ACPU ICC通道成功发送的可维可测信息记录结构
 */
typedef struct {
    VOS_UINT32 iccOmSendMsgNum; /* 当前发送的总消息数 */
    VOS_UINT32 iccOmSendLen;    /* 当前发送的总消息长度 */
    VOS_UINT32 iccOmSendSlice;  /* 当前发送的最后一条数据的时间 */
} CBT_AcpuIccSendSuc;

/*
 * 功能说明: OM模块可维可测信息记录结构
 */
typedef struct {
    VOS_UINT32 dataLen; /* 接收或发送数据长度 */
    VOS_UINT32 num;     /* 接收或发送数据次数 */
} CBT_MaintenanceInfo;

/*
 * 功能说明: OM模块断链操作信息记录结构
 */
typedef struct {
    VOS_UINT32 rlsAp2Modem;   /* AP到Modem切换 */
    VOS_UINT32 rlsModem2Ap;   /* Modem到AP切换 */
    VOS_UINT32 rlsDrvSuspend; /* DRV挂起 */
    VOS_UINT32 rlsPortClose;  /* 端口关闭 */
    VOS_UINT32 rlsPortSwitch; /* 端口切换 */
    VOS_UINT32 slice;         /* 最后一次收到断链的时间 */
} CBT_AcpuPcUeReleaseInfo;

/*
 * 功能说明: CBT模块异常接收可维可测信息记录结构
 */
typedef struct {
    CBT_MaintenanceInfo lostData;      /* 记录丢掉的数据 */
    VOS_UINT32          frameDecapErr; /* 记录发生解帧错误的次数 */

    VOS_UINT16 packetLenErr;   /* 记录发生包长度小于OM_USB_FRAME_LEN错误的次数 */
    VOS_UINT16 lenTooShortErr; /* 记录发生数据长度小于分段头的错误次数 */
    VOS_UINT16 segLenErr;      /* 记录发生段长度错误次数 */
    VOS_UINT16 segNumErr;      /* 记录发生段数目错误次数 */

    VOS_UINT16 datatypeErr;   /* 记录发生datatype字段错误次数 */
    VOS_UINT16 msgTooLongErr; /* 记录发生组包后消息大于8K的错误次数 */
    VOS_UINT16 cpuIdErr;      /* 记录发生CPU id错误次数 */
    VOS_UINT16 noMemErr;      /* 记录发生空间不足的错误次数 */

    VOS_UINT16 dataHeadErr;   /* 记录发生数据帧头部错误次数 */
    VOS_UINT16 msgLenErr;     /* 记录发生消息长度错误次数 */
    VOS_UINT16 msgSnErr;      /* 记录发生消息序号错误次数 */
    VOS_UINT16 transIdErr;    /* 记录发生TransId 序号错误次数 */
    VOS_UINT16 packetLostErr; /* 记录发生包丢失的错误次数 */
    VOS_UINT16 timeStampErr;  /* 记录发生包时间戳的错误次数 */

    VOS_UINT16 linkDataLenErr; /* 记录链路消息长度错误的次数 */
    VOS_UINT16 linkStatusErr;  /* 记录链路关闭时收到消息包的次数 */
    VOS_UINT16 allocMsg;       /* 记录消息分配失败的次数 */
    VOS_UINT16 rcv;

    CBT_AcpuIccSendFail  iccSendFailInfo; /* 记录ACPU成功发送的ICC数据 */
} CBT_AcpuPcUeFail;

/*
 * 功能说明: CBT模块正常接收可维可测信息记录结构
 */
typedef struct {
    CBT_MaintenanceInfo     totalData; /* 记录接收到的总数据 */
    CBT_MaintenanceInfo     hdlcDecapData;
    CBT_MaintenanceInfo     linkData;       /* 记录收到的链路数据 */
    CBT_MaintenanceInfo     ccpuData;       /* 记录发给CCPU的数据 */
    CBT_MaintenanceInfo     acpuData;       /* 记录发给ACPU的数据 */
    CBT_AcpuIccSendSuc      iccSendSucInfo; /* 记录ACPU成功发送的ICC数据 */
    CBT_MaintenanceInfo     rlsData;        /* 记录工具发送的Release数据 */
    CBT_AcpuPcUeReleaseInfo rlsInfo;        /* 记录Release信息 */
} CBT_AcpuPcUeSuc;

/*
 * 功能说明: CBTRL模块完成组包需要的信息结构
 */
typedef struct {
    VOS_UINT8  totalSegNum;   /* 总的分段数 */
    VOS_UINT8  expectedSegSn; /* 组包函数中期望得到的消息包序号 */
    VOS_UINT16 rsv;

    VOS_UINT32 transId;
    VOS_UINT32 timeStampL;
    VOS_UINT16 timeStampH;
    VOS_UINT16 rsv2;

    VOS_UINT32 totalMsgLen; /* 组包函数中组包后消息总的长度 */
    VOS_UINT32 moveLen;     /* 组包函数中每放入一消息包后的偏移 */
    MsgBlock * wholeMsg;    /* 指向组包函数中完整消息包 */
} CBT_MsgCombineInfo;

/*
 * 功能说明: CBTRL模块通道控制信息结构
 */
typedef struct {
    VOS_UINT32         cbtSwitchOnOff;
    CBT_MsgCombineInfo msgCombineInfo;

    CBT_AcpuPcUeFail pcToUeErrRecord;
    CBT_AcpuPcUeSuc  pcToUeSucRecord;
} CBT_RcvChanCtrlInfo;

VOS_UINT32 CBT_AcpuInit(VOS_VOID);
VOS_UINT32 CBT_AcpuSendData(CBT_UniformMsg *msg, VOS_UINT16 msgLen);

VOS_VOID CBT_AcpuSendResultChannel(CBT_ModemSsid modemSsid, CBT_ComponentModeInfo compMode, VOS_UINT16 returnPrimId,
                                   APP_FUNC_ProcErrUint32 result);
VOS_VOID CBT_AcpuSendContentChannel(CBT_ModemSsid modemSsid, CBT_ComponentModeInfo compMode, VOS_UINT16 returnPrimId,
                                    CBT_UniformMsg *cbtToPcMsg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
