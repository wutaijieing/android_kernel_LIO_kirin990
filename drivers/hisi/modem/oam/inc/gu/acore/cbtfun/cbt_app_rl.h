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
/* datatype �ֶγ��� */
#define CBT_RL_DATATYPE_LEN (0x1)
#define CBT_HDLC_FCS_LEN (2) /* HDLC֡��FCSδת��ʱ�ĳ���(��λ: Byte) */
#define CBT_HDLC_BUF_MAX_LEN (CBT_MSG_SEGMENT_LEN + CBT_RL_DATATYPE_LEN + CBT_HDLC_FCS_LEN)

/* ֵΪCBT_READ_NV_IND_STRU�ṹ���У�sizeof(errorCode) + sizeof(ulNvId) + sizeof(ulCount) */
#define CBT_READ_NV_HEAD_SIZE (12)
/* ����ΪNVID��ֵ+NVID������ռ�ڴ泤�� */
#define CBT_NV_ITEM_SIZE (4)
#define CBT_IND_RESULT_SIZE (4)
#define CBT_EST_IND_CHIP_ID_SIZE (4)
#define CBT_EST_IND_RSV_LEN (212)

#define CBT_END_FRAME (1)
/* #define SEG_MAX_LEN                         (4*1024) */
/* #define MAXSEGMENT                          (16) */
/* ��������󳤶� */
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
    VOS_UINT16        msgId; /* ��ϢID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT16        c1Value; /* C1ֵ */
    VOS_UINT16        c2Value; /* C2ֵ */
} CBT_SetC1C2ValueReq;

typedef struct {
    VOS_UINT32     result; /* ����ִ�н�� */
    VOS_UINT16     chipId;
    VOS_UINT16     rsv;
    msw_ver_info_s swverInfo;
    VOS_UINT8      reserve[CBT_EST_IND_RSV_LEN];
} CBT_EstablishIndPara;

typedef struct {
    CBT_MsgHead          msgHeader;
    VOS_UINT16           msgId; /* ��ϢID */
    CBT_ComponentModeInfo    compMode;
    VOS_UINT32           msgLength;
    CBT_EstablishIndPara msgData;
} CBT_EstablishInd;

typedef struct {
    CBT_MsgHead       msgHeader;
    VOS_UINT16        msgId; /* ��ϢID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;
    VOS_UINT32        linkType; /* 0��ʾHDLC, 1��ʾusb */
    VOS_UINT32        rsv[4];
} CBT_EstablishReq;

/*
 * ����˵��: A�˷���C�˵Ľ�����Ϣ
 */
typedef struct {
    VOS_MSG_HEADER
    CBT_EstablishReq data;
} CBT_EstablishReqAtoc;

typedef struct {
    VOS_UINT32 count;        /* Ҫ��ȡ��NV����� */
    VOS_UINT16 nvItemId[2]; /* Ҫ��ȡ��NV��ID���飬���и���ΪulCount */
} CBT_ReadNvReq;

typedef struct {
    VOS_UINT32 errorCode;        /* ����ִ�н�� */
    VOS_UINT32 errNvId;          /* ���س��ִ����NVID */
    VOS_UINT32 count;            /* ���ص�NV����� */
    VOS_UINT16 nvItemData[2]; /* ����NVIDֵ��NVID���ݵĳ��ȡ�NVID������ */
} CBT_ReadNvInd;

typedef struct {
    CBT_MsgHead       msgHead;
    VOS_UINT16        msgId; /* ��ϢID */
    CBT_ComponentModeInfo compMode;
    VOS_UINT32        msgLength;

    VOS_UINT32 errorCode; /* ����ִ�н�� */
    VOS_UINT32 errNvId;   /* ���س��ִ����NVID */
    VOS_UINT32 count;     /* ���ص�NV����� */
} CBT_ReadNvCnf;

/*
 * ����˵��: OMRL CBTУ׼ͨ��HDLC����������Ϣ�ṹ(uncache)
 */
typedef struct {
    VOS_UINT8 *buf;     /* ָ�򻺳��������׵�ַ */
    VOS_UINT8 *realBuf; /* ָ�򻺳���ʵ�׵�ַ */
    VOS_UINT32 bufSize; /* ���滺�����ܴ�С */
    VOS_UINT32 rsv;     /* Reserve */
} CBT_HdlcEncodeMemCtrl;

/*
 * ����˵��: ACPU ICCͨ������ʧ�ܵĿ�ά�ɲ���Ϣ��¼�ṹ
 */
typedef struct {
    VOS_UINT32 iccOmSendErrNum;   /* ��ǰ���͵Ĵ����¼ */
    VOS_UINT32 iccOmSendErrLen;   /* ��ǰ���͵Ĵ�������Ϣ���� */
    VOS_UINT32 iccOmSendErrSlice; /* ��ǰ���͵Ĵ�����Ϣ���һ�����ݵ�ʱ�� */
} CBT_AcpuIccSendFail;

/*
 * ����˵��: ACPU ICCͨ���ɹ����͵Ŀ�ά�ɲ���Ϣ��¼�ṹ
 */
typedef struct {
    VOS_UINT32 iccOmSendMsgNum; /* ��ǰ���͵�����Ϣ�� */
    VOS_UINT32 iccOmSendLen;    /* ��ǰ���͵�����Ϣ���� */
    VOS_UINT32 iccOmSendSlice;  /* ��ǰ���͵����һ�����ݵ�ʱ�� */
} CBT_AcpuIccSendSuc;

/*
 * ����˵��: OMģ���ά�ɲ���Ϣ��¼�ṹ
 */
typedef struct {
    VOS_UINT32 dataLen; /* ���ջ������ݳ��� */
    VOS_UINT32 num;     /* ���ջ������ݴ��� */
} CBT_MaintenanceInfo;

/*
 * ����˵��: OMģ�����������Ϣ��¼�ṹ
 */
typedef struct {
    VOS_UINT32 rlsAp2Modem;   /* AP��Modem�л� */
    VOS_UINT32 rlsModem2Ap;   /* Modem��AP�л� */
    VOS_UINT32 rlsDrvSuspend; /* DRV���� */
    VOS_UINT32 rlsPortClose;  /* �˿ڹر� */
    VOS_UINT32 rlsPortSwitch; /* �˿��л� */
    VOS_UINT32 slice;         /* ���һ���յ�������ʱ�� */
} CBT_AcpuPcUeReleaseInfo;

/*
 * ����˵��: CBTģ���쳣���տ�ά�ɲ���Ϣ��¼�ṹ
 */
typedef struct {
    CBT_MaintenanceInfo lostData;      /* ��¼���������� */
    VOS_UINT32          frameDecapErr; /* ��¼������֡����Ĵ��� */

    VOS_UINT16 packetLenErr;   /* ��¼����������С��OM_USB_FRAME_LEN����Ĵ��� */
    VOS_UINT16 lenTooShortErr; /* ��¼�������ݳ���С�ڷֶ�ͷ�Ĵ������ */
    VOS_UINT16 segLenErr;      /* ��¼�����γ��ȴ������ */
    VOS_UINT16 segNumErr;      /* ��¼��������Ŀ������� */

    VOS_UINT16 datatypeErr;   /* ��¼����datatype�ֶδ������ */
    VOS_UINT16 msgTooLongErr; /* ��¼�����������Ϣ����8K�Ĵ������ */
    VOS_UINT16 cpuIdErr;      /* ��¼����CPU id������� */
    VOS_UINT16 noMemErr;      /* ��¼�����ռ䲻��Ĵ������ */

    VOS_UINT16 dataHeadErr;   /* ��¼��������֡ͷ��������� */
    VOS_UINT16 msgLenErr;     /* ��¼������Ϣ���ȴ������ */
    VOS_UINT16 msgSnErr;      /* ��¼������Ϣ��Ŵ������ */
    VOS_UINT16 transIdErr;    /* ��¼����TransId ��Ŵ������ */
    VOS_UINT16 packetLostErr; /* ��¼��������ʧ�Ĵ������ */
    VOS_UINT16 timeStampErr;  /* ��¼������ʱ����Ĵ������ */

    VOS_UINT16 linkDataLenErr; /* ��¼��·��Ϣ���ȴ���Ĵ��� */
    VOS_UINT16 linkStatusErr;  /* ��¼��·�ر�ʱ�յ���Ϣ���Ĵ��� */
    VOS_UINT16 allocMsg;       /* ��¼��Ϣ����ʧ�ܵĴ��� */
    VOS_UINT16 rcv;

    CBT_AcpuIccSendFail  iccSendFailInfo; /* ��¼ACPU�ɹ����͵�ICC���� */
} CBT_AcpuPcUeFail;

/*
 * ����˵��: CBTģ���������տ�ά�ɲ���Ϣ��¼�ṹ
 */
typedef struct {
    CBT_MaintenanceInfo     totalData; /* ��¼���յ��������� */
    CBT_MaintenanceInfo     hdlcDecapData;
    CBT_MaintenanceInfo     linkData;       /* ��¼�յ�����·���� */
    CBT_MaintenanceInfo     ccpuData;       /* ��¼����CCPU������ */
    CBT_MaintenanceInfo     acpuData;       /* ��¼����ACPU������ */
    CBT_AcpuIccSendSuc      iccSendSucInfo; /* ��¼ACPU�ɹ����͵�ICC���� */
    CBT_MaintenanceInfo     rlsData;        /* ��¼���߷��͵�Release���� */
    CBT_AcpuPcUeReleaseInfo rlsInfo;        /* ��¼Release��Ϣ */
} CBT_AcpuPcUeSuc;

/*
 * ����˵��: CBTRLģ����������Ҫ����Ϣ�ṹ
 */
typedef struct {
    VOS_UINT8  totalSegNum;   /* �ܵķֶ��� */
    VOS_UINT8  expectedSegSn; /* ��������������õ�����Ϣ����� */
    VOS_UINT16 rsv;

    VOS_UINT32 transId;
    VOS_UINT32 timeStampL;
    VOS_UINT16 timeStampH;
    VOS_UINT16 rsv2;

    VOS_UINT32 totalMsgLen; /* ����������������Ϣ�ܵĳ��� */
    VOS_UINT32 moveLen;     /* ���������ÿ����һ��Ϣ�����ƫ�� */
    MsgBlock * wholeMsg;    /* ָ�����������������Ϣ�� */
} CBT_MsgCombineInfo;

/*
 * ����˵��: CBTRLģ��ͨ��������Ϣ�ṹ
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
