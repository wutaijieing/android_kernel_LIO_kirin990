/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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
#ifndef _AT_INPUT_PROC_H_
#define _AT_INPUT_PROC_H_

#include "imm_interface.h"
#include "at_mdrv_interface.h"
#include "ATCmdProc.h"
#include "product_config.h"
#include "taf_oam_interface.h"
#include "AtOutputProc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/* Begin - yaochaoqun - 2009-4-2 - for new PNP */
#define SCSI_CMD_LEN 31
/* End - yaochaoqun - 2009-4-2 - for new PNP */

/* 向底软最大重传次数 */
#define AT_NDIS_MAX_RESEND_TIMES 60

/* AT数据初始长度 */
#define AT_INIT_DATA_LEN 0

#define AT_RCV_DATA_MAX_LEN 0xffff
#define AT_MODEM_UL_DATA_BUFF_SIZE 1536
#define AT_MODEM_UL_DATA_BUFF_NUM 16

#define AT_UART_UL_DATA_BUFF_SIZE 1536
#define AT_UART_UL_DATA_BUFF_NUM 16

/* HSIC AT通道，底软向协议栈发送数据的上行缓存规格 */
#define AT_HSIC_UL_DATA_BUFF_SIZE (5 * 1024)
#define AT_HSIC_UL_DATA_BUFF_NUM 2

/* 来电RI管脚电平控制定时器名称 */
#define AT_SET_VOICE_RI_TMR_NAME(tmrName) do { \
    (tmrName) = AT_VOICE_RI_TIMER;         \
    (tmrName) |= AT_INTERNAL_PROCESS_TYPE; \
} while (0)

/* 来电RI管脚电平控制定时器参数 */
#define AT_SET_VOICE_RI_TMR_PARAM(tmrParam, indexNum, callId) ((tmrParam) = ((callId) << 8) | (indexNum))

/* 从来电RI管脚电平控制定时器超时消息中获取CALLID */
#define AT_GET_VOICE_RI_CALLID_FROM_TMR_PARAM(ulTmrParam) ((VOS_UINT8)(((ulTmrParam)&0x0000FF00) >> 8))

/* 从来电RI管脚电平控制定时器超时消息中获取端口ID */
#define AT_GET_VOICE_RI_CLIENTID_FROM_TMR_PARAM(ulTmrParam) ((VOS_UINT8)((ulTmrParam)&0x000000FF))

/* 新短信RI管脚电平控制定时器名称 */
#define AT_SET_SMS_RI_TMR_NAME(tmrName) do { \
    (tmrName) = AT_SMS_RI_TIMER;           \
    (tmrName) |= AT_INTERNAL_PROCESS_TYPE; \
} while (0)

/* 新短信RI管脚电平控制定时器参数 */
#define AT_SET_SMS_RI_TMR_PARAM(tmrParam, indexNum) ((tmrParam) = (indexNum))

/* 从新短信RI管脚电平控制定时器超时消息中获取端口ID */
#define AT_GET_SMS_RI_CLIENTID_FROM_TMR_PARAM(ulTmrParam) ((VOS_UINT8)((ulTmrParam)&0x000000FF))

/* UART端口数据帧格式映射表指针和大小 */
#define AT_UART_GET_FORMAT_TBL_PTR() (g_atUartFormatTab)
#define AT_UART_GET_FORMAT_TBL_SIZE() (AT_ARRAY_SIZE(g_atUartFormatTab))


enum AT_MemSourceType {
    AT_MEM_SOURCE_UDI_UL_BUF, /* MODEM设备buffer内存 */
    AT_MEM_SOURCE_UDI_DL_BUF, /* 使用的DDR内存 */

    AT_MEM_SOURCE_TYPE_BUTT
};
typedef VOS_UINT32 AT_MemSourceTypeUint32;

extern VOS_UINT32 g_atNdisSendSem;


extern VOS_INT OM_RcvDiagCmdFromPC(VOS_UINT8 portNo, VOS_UINT8 *data, VOS_UINT16 uslength);
#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_INT32 Sock_RecvCallbackRegister(VOS_UINT8 portNo, pSockRecv callback);
#endif

extern VOS_INT32  AT_AppComEst(VOS_VOID);
extern VOS_UINT32 AT_CheckAppUser(VOS_UINT8 indexNum);
extern VOS_UINT32 AT_CheckModemUser(VOS_UINT8 indexNum);
extern VOS_UINT32 AT_CheckHsUartUser(VOS_UINT8 indexNum);
extern VOS_UINT32 AT_CheckUserType(VOS_UINT8 indexNum, AT_USER_TYPE userType);

extern VOS_VOID   AT_CloseUsbNdis(VOS_VOID);
extern VOS_UINT32 At_CmdStreamPreProc(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len);
extern TAF_UINT32 At_DataStreamPreProc(TAF_UINT8 indexNum, TAF_UINT8 dataMode, TAF_UINT8 *data, TAF_UINT16 len);
extern VOS_VOID   AT_VcomCmdStreamEcho(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len);
extern VOS_VOID   AT_GetAtMsgStruMsgLength(VOS_UINT32 infoLength, VOS_UINT32 *msgLength);
extern VOS_VOID   AT_GetUserTypeFromIndex(VOS_UINT8 indexNum, VOS_UINT8 *userType);
extern VOS_VOID   AT_InitFcMap(VOS_VOID);
extern VOS_VOID   AT_InitUartLink(VOS_UINT8 indexNum);
extern VOS_UINT32 AT_IpDataModeRcvModemMsc(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru);
extern VOS_UINT32 AT_IsConcurrentPorts(VOS_UINT8 indexOne, VOS_UINT8 indexTwo);

extern VOS_VOID   AT_ModemSetCtlStatus(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru);
extern VOS_UINT32 At_OmDataProc(VOS_UINT16 portNo, VOS_UINT8 *data, VOS_UINT16 len);
extern VOS_UINT32 AT_PppDataModeRcvModemMsc(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru);

extern TAF_UINT32 At_SendCmdMsg(TAF_UINT8 indexNum, TAF_UINT8 *data, TAF_UINT16 len, TAF_UINT8 type);
extern VOS_UINT32 AT_SendCtrlDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 length);
extern VOS_UINT32 AT_SendPcuiDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 length);

extern VOS_UINT32 AT_SetModemStatus(VOS_UINT8 indexNum, struct modem_msc_stru *msc);

extern VOS_UINT32 AT_SockEst(VOS_VOID);
extern VOS_VOID   AT_StopFlowCtrl(VOS_UINT8 indexNum);
extern VOS_UINT32 At_UsbCtrEst(VOS_VOID);
extern VOS_UINT32 AT_ModemEst(VOS_VOID);
extern VOS_VOID   AT_ModemeEnableCB(VOS_UINT8 indexNum, VOS_UINT32 enable);

extern VOS_UINT32 At_ModemMscInd(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru);
extern VOS_VOID   AT_ModemRcvMscInd(VOS_UINT16 clientId, struct modem_msc_stru *pstRcvedMsc);
extern VOS_UINT32 AT_ModemStatusPreProc(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru);
extern VOS_UINT32 AT_UsbNdisEst(VOS_VOID);
extern VOS_UINT32 At_UsbPcuiEst(VOS_VOID);
extern VOS_UINT32 At_UsbPcui2Est(VOS_VOID);
extern VOS_UINT32 AT_IsApPort(VOS_UINT8 indexNum);

VOS_VOID AT_NcmRcvConnStatusChg(VOS_UINT16 clientId, enum ncm_ioctrl_connect_stus status);
VOS_VOID AT_NcmConnStatusChgProc(enum ncm_ioctrl_connect_stus status);

VOS_VOID   AT_ModemProcDtrChange(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc);
VOS_UINT32 AT_ModemStartFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2);
VOS_UINT32 AT_ModemStopFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2);

VOS_UINT32 AT_UartSendRawDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 len);
VOS_VOID   AT_UartInitLink(VOS_UINT8 indexNum);
VOS_VOID   AT_UartInitPort(VOS_VOID);

#if (FEATURE_AT_HSUART == FEATURE_ON)
VOS_UINT32 AT_HSUART_IsBaudRateValid(AT_UartBaudrateUint32 baudRate);
VOS_UINT32 AT_HSUART_IsFormatValid(AT_UartFormatUint8 format);
VOS_UINT32 AT_HSUART_IsParityValid(AT_UartParityUint8 parity);
VOS_UINT32 AT_HSUART_ValidateFlowCtrlParam(AT_UartFcDceByDteUint8 fcDceByDte, AT_UartFcDteByDceUint8 fcDteByDce);
VOS_UINT32 AT_HSUART_ValidateCharFrameParam(AT_UartFormatUint8 format, AT_UartParityUint8 parity);
AT_UartFormatParam* AT_HSUART_GetFormatParam(AT_UartFormatUint8 format);
VOS_UINT32          AT_HSUART_GetUdiValueByDataLen(AT_UartDataLenUint8 dataLen, VOS_UINT32 *udiValue);
VOS_UINT32          AT_HSUART_GetUdiValueByStopLen(AT_UartStopLenUint8 stopLen, VOS_UINT32 *udiValue);
VOS_UINT32          AT_HSUART_GetUdiValueByParity(AT_UartParityUint8 parity, VOS_UINT32 *udiValue);

VOS_UINT32 AT_HSUART_ProcDtrChange(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru);
VOS_UINT32 AT_HSUART_ProcDtrCtrlMode(VOS_VOID);

VOS_UINT32 AT_HSUART_StartFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2);
VOS_UINT32 AT_HSUART_StopFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2);

VOS_UINT32 AT_HSUART_SendRawDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 len);

VOS_VOID AT_HsuartRcvMscInd(VOS_UINT16 clientId, struct modem_msc_stru *dceMsc);
VOS_VOID AT_HsuartRcvEscapeSequence(VOS_UINT16 clientId);
VOS_VOID AT_HsuartRcvWaterMarkInd(VOS_UINT16 clientId, water_level level);
VOS_UINT32 AT_HSUART_InitPort(VOS_VOID);
VOS_UINT32 AT_HSUART_ConfigFlowCtrl(VOS_UINT8 indexNum, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag);

VOS_UINT32 AT_HSUART_ConfigCharFrame(VOS_UINT8 indexNum, AT_UartFormatUint8 format, AT_UartParityUint8 parity);

VOS_UINT32 AT_HSUART_ConfigBaudRate(VOS_UINT8 indexNum, AT_UartBaudrateUint32 baudRate);

VOS_VOID AT_HSUART_InitLink(VOS_UINT8 indexNum);
VOS_VOID AT_HSUART_ConfigDefaultPara(VOS_VOID);
#endif

AT_IoLevelUint8 AT_GetIoLevel(VOS_UINT8 indexNum, VOS_UINT8 ioCtrl);
VOS_VOID        AT_CtrlDCD(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel);
VOS_VOID        AT_CtrlDSR(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel);
VOS_VOID        AT_CtrlCTS(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel);
VOS_VOID        AT_CtrlRI(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel);


typedef struct {
    VOS_MSG_HEADER                    /* _H2ASN_Skip */
    AT_InterMsgIdUint32   msgId;      /* 消息类型 */ /* _H2ASN_Skip */
    VOS_UINT32            portId;     /* 端口ID */
    struct modem_msc_stru dceMscInfo; /* 管脚信号信息 */

} AT_MntnMsc;

/*
 * 功能描述: 管脚信号输入可维可测
 */
VOS_VOID AT_MntnTraceInputMsc(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc);

/*
 * 功能描述: 管脚信号输出可维可测
 */
VOS_VOID AT_MntnTraceOutputMsc(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc);
VOS_UINT32 AT_UsbNdisDataEst(VOS_VOID);
VOS_INT32  AT_ComRcvData(VOS_UINT16 clientId, VOS_UINT8 *data, VOS_UINT32 length);
VOS_INT32  AT_ModemRcvPrivData(VOS_UINT16 clientId, VOS_VOID *data);
VOS_INT32  AT_HsuartRcvPrivData(VOS_UINT16 clientId, VOS_VOID *data);
VOS_VOID   AT_ModemRcvConnectInd(VOS_UINT16 clientId, VOS_UINT8 connState);
VOS_VOID   AT_NcmDataRcvConnectInd(VOS_UINT16 clientId, VOS_UINT8 connState);
VOS_VOID   AT_AppUsbSwitchGwMode(VOS_UINT16 clientId);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of AtInputProc */
