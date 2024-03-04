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

#include "ATCmdProc.h"

#include "ppp_interface.h"
#include "AtUsbComItf.h"
#include "AtInputProc.h"
#include "AtTafAgentInterface.h"
#include "TafAgentInterface.h"
#include "AtCmdMsgProc.h"
#include "AtDataProc.h"
#include "imm_interface.h"
#include "at_mdrv_interface.h"
#include "AtMntn.h"
#include "AcpuReset.h"

#include "AtInternalMsg.h"
#include "securec.h"
#include "mn_comm_api.h"
#include "dms_debug.h"
#include "dms_file_node_i.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_AT_INPUTPROC_C
#define AT_MSG_CUSTOMIZED_ITEM_LEN 4

VOS_UINT32 g_atUsbDebugFlag = VOS_FALSE;

/* AT/DIAG通道的链路索引 */
VOS_UINT8 g_omDiagIndex = AT_MAX_CLIENT_NUM;

/* AT帧结构与DRV 值之间的对应关系 */
AT_UartFormatParam g_atUartFormatTab[] = {
    /* auto detect (not support) */

    { AT_UART_FORMAT_8DATA_2STOP, AT_UART_DATA_LEN_8_BIT,
      AT_UART_STOP_LEN_2_BIT, AT_UART_PARITY_LEN_0_BIT },

    { AT_UART_FORMAT_8DATA_1PARITY_1STOP, AT_UART_DATA_LEN_8_BIT,
      AT_UART_STOP_LEN_1_BIT, AT_UART_PARITY_LEN_1_BIT },

    { AT_UART_FORMAT_8DATA_1STOP, AT_UART_DATA_LEN_8_BIT,
      AT_UART_STOP_LEN_1_BIT, AT_UART_PARITY_LEN_0_BIT },

    { AT_UART_FORMAT_7DATA_2STOP, AT_UART_DATA_LEN_7_BIT,
      AT_UART_STOP_LEN_2_BIT, AT_UART_PARITY_LEN_0_BIT },

    { AT_UART_FORMAT_7DATA_1PARITY_1STOP, AT_UART_DATA_LEN_7_BIT,
      AT_UART_STOP_LEN_1_BIT, AT_UART_PARITY_LEN_1_BIT },

    { AT_UART_FORMAT_7DATA_1STOP, AT_UART_DATA_LEN_7_BIT,
      AT_UART_STOP_LEN_1_BIT, AT_UART_PARITY_LEN_0_BIT }
};

STATIC const struct DMS_ClientCBTbl g_AtClientCBTbl[] = {
    [DMS_CLIENT_TYPE_COM] = {
        {
            .RcvData = AT_ComRcvData,
        },
        .RcvConnStatusChgInd = AT_NcmRcvConnStatusChg, /* 该接口当前仅给ndis ctrl端口使用 */
        .RcvSwitchGwModeInd  = AT_AppUsbSwitchGwMode,  /* 该接口仅给appvcom0口使用 */
    },

    [DMS_CLIENT_TYPE_MODEM] = {
        {
            .RcvPrivData = AT_ModemRcvPrivData,
        },
        .RcvConnectInd = AT_ModemRcvConnectInd,
        .RcvMscInd     = AT_ModemRcvMscInd,
    },

#if (FEATURE_AT_HSUART == FEATURE_ON)
    [DMS_CLIENT_TYPE_HSUART] = {
        {
            .RcvPrivData = AT_HsuartRcvPrivData,
        },
        .RcvMscInd         = AT_HsuartRcvMscInd,
        .RcvEscapeSequence = AT_HsuartRcvEscapeSequence,
        .RcvWaterMarkInd   = AT_HsuartRcvWaterMarkInd,
    },
#endif

    [DMS_CLIENT_TYPE_NDIS_DATA] = {
        .RcvConnectInd = AT_NcmDataRcvConnectInd,
    },
};


VOS_UINT32 AT_GetUsbDebugFlag(VOS_VOID)
{
    return g_atUsbDebugFlag;
}


AT_ClientManage* AT_GetClientManage(AT_ClientIdUint16 clientId)
{
    if (clientId >= AT_CLIENT_ID_BUTT) {
        AT_WARN_LOG("AT_GetClientManage: client id is more than AT_CLIENT_ID_BUTT!");
        return VOS_NULL_PTR;
    }

    return &(g_atClientTab[clientId]);
}


VOS_INT32 AT_ComRcvData(VOS_UINT16 clientId, VOS_UINT8 *data, VOS_UINT32 length)
{
    AT_ClientManage *atClientManage = VOS_NULL_PTR;
    VOS_UINT32       ret;

    /* AT命令长度处理函数统一用的VOS_UINT16，超过支持范围的长度打印异常返回 */
    if (length > AT_RCV_DATA_MAX_LEN) {
        AT_WARN_LOG("AT_ComRcvData: length is more than 0xffff!");
        return VOS_ERR;
    }

    atClientManage = AT_GetClientManage(clientId);

    if (atClientManage == VOS_NULL_PTR) {
        AT_WARN_LOG1("AT_ComRcvData: get client fail! ", clientId);
        return VOS_ERR;
    }

    if (atClientManage->used == AT_CLIENT_NULL) {
        AT_WARN_LOG("AT_ComRcvData: APP client is unused!");
        return VOS_ERR;
    }

    if (AT_GetUsbDebugFlag() == VOS_TRUE) {
        PS_PRINTF_INFO("AT_ComRcvData clientId = %d, length = %d, data = %s\n", clientId, length, data);
    }

    AT_MntnIncRcvStreamCnt(clientId);

    if (atClientManage->mode == AT_CMD_MODE) {
        ret = At_CmdStreamPreProc((VOS_UINT8)clientId, data, (VOS_UINT16)length);
    } else {
        ret = At_DataStreamPreProc((VOS_UINT8)clientId, atClientManage->dataMode, data, (VOS_UINT16)length);
    }

    if (ret == AT_SUCCESS) {
        return VOS_OK;
    } else {
        return VOS_ERR;
    }
}


VOS_BOOL AT_IsEscapeSequence(VOS_UINT8 *data, VOS_UINT16 len)
{
    if (len == 3) {
        if ((data[0] == '+') && (data[1] == '+') && (data[2] == '+')) {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_VOID AT_ModemCmdDataPreProc(AT_ClientManage *atClientManage, IMM_Zc *immZc)
{
    VOS_UINT8 *dataTemp = VOS_NULL_PTR;
    VOS_UINT16 len;

    dataTemp = immZc->data;
    len      = (VOS_UINT16)immZc->len;

    /* 若Modem通道已经切入命令态，但此时仍然收到PPP帧，则直接丢弃 */
    if ((len > 0) && (dataTemp[0] == 0x7e) && (dataTemp[len - 1] == 0x7e)) {
        /* 释放BSP内存 */
        DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
        return;
    }

    if (At_CmdStreamPreProc((VOS_UINT8)atClientManage->clientId, dataTemp, len) != AT_SUCCESS) {
        AT_WARN_LOG("AT_ModemRcvPrivData: At_CmdStreamPreProc fail!");
    }

    /* 释放BSP内存 */
    DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
    return;
}


VOS_INT32 AT_ModemRcvPrivData(VOS_UINT16 clientId, VOS_VOID *data)
{
    AT_ClientManage      *atClientManage = VOS_NULL_PTR;
    IMM_Zc               *immZc          = VOS_NULL_PTR;
    VOS_UINT8            *dataTemp       = VOS_NULL_PTR;
    struct modem_msc_stru mscStru;
    VOS_UINT16            len;

    atClientManage = AT_GetClientManage(clientId);

    if (atClientManage == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_ModemRcvPrivData: get client manage fail!");
        return VOS_ERR;
    }

    immZc = (IMM_Zc *)data;

    /* 检查index和Dlci是否正确 */
    if (clientId != AT_CLIENT_ID_MODEM) {
        AT_ERR_LOG1("AT_ModemRcvPrivData: uncorret clientid %d!", clientId);
        /* 释放内存 */
        DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
        return VOS_ERR;
    }

    /* 从pstData(IMM_Zc类型)中取出数据内容和长度，分别保存在pData和usLen中 */
    dataTemp = immZc->data;
    len      = (VOS_UINT16)immZc->len;

    if (atClientManage->mode == AT_CMD_MODE) {
        AT_ModemCmdDataPreProc(atClientManage, immZc);
        return VOS_OK;
    }

    /* 根据modem口的状态进行分发 */
    switch (atClientManage->dataMode) {
        case AT_PPP_DATA_MODE:
            /*
             * (AT2D17549)规避MAC 10.6.2系统拨号挂断失败问题
             * 若接收数据为"+++"，则模拟拉底DTR信号的处理方式
             */
            if (AT_IsEscapeSequence(dataTemp, len) == VOS_TRUE) {
                /* 模拟拉底DTR信号 */
                memset_s(&mscStru, (VOS_SIZE_T)sizeof(mscStru), 0x00, (VOS_SIZE_T)sizeof(mscStru));
                mscStru.op_dtr = 1;
                mscStru.uc_dtr = 0;
                At_ModemMscInd((VOS_UINT8)clientId, &mscStru);
                break;
            }
            /* PPP负责释放上行内存 */
            PPP_PullPacketEvent(atClientManage->pppId, immZc);
            return VOS_OK;

        case AT_IP_DATA_MODE:
            if (AT_IsEscapeSequence(dataTemp, len) == VOS_TRUE) {
                /* 模拟拉底DTR信号 */
                memset_s(&mscStru, (VOS_SIZE_T)sizeof(mscStru), 0x00, (VOS_SIZE_T)sizeof(mscStru));
                mscStru.op_dtr = 1;
                mscStru.uc_dtr = 0;
                At_ModemMscInd((VOS_UINT8)clientId, &mscStru);
                break;
            }
            /* PPP负责释放上行内存 */
            PPP_PullRawDataEvent(atClientManage->pppId, immZc);
            return VOS_OK;

        default:
            AT_WARN_LOG("AT_ModemRcvPrivData: DataMode Wrong!");
            break;
    }

    /* 释放内存 */
    DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
    return VOS_OK;
}


VOS_VOID AT_ModemRcvConnectInd(VOS_UINT16 clientId, VOS_UINT8 connState)
{
    if (clientId != AT_CLIENT_ID_MODEM) {
        AT_ERR_LOG1("AT_ModemRcvConnectInd: uncorret clientid %d!", clientId);
        return;
    }

    /* 端口断开 */
    if (connState == DMS_DISCONNECT) {
        /* modem端口处理 */
        AT_ModemeEnableCB((VOS_UINT8)clientId, VOS_FALSE);
        AT_DeRegModemPsDataFCPoint((VOS_UINT8)clientId, AT_GET_RABID_FROM_EXRABID(g_atClientTab[clientId].exPsRabId));

        /* 从记录表中删除USB相关的client ID */
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_PCUI);
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_CTRL);
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_NDIS);
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_MODEM);
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_PCUI2);
    }
}


VOS_VOID AT_NcmDataRcvConnectInd(VOS_UINT16 clientId, VOS_UINT8 connState)
{
    /* ndis data通道clientid默认为0 */
    if (clientId != 0) {
        AT_ERR_LOG1("AT_NcmDataRcvConnectInd: uncorret clientid %d!", clientId);
        return;
    }

    /* 端口断开 */
    if (connState == DMS_DISCONNECT) {
        /* ndis端口处理 */
        AT_NcmConnStatusChgProc(NCM_IOCTL_STUS_BREAK);
    }
}


VOS_VOID AT_AppUsbSwitchGwMode(VOS_UINT16 clientId)
{
    VOS_UINT8          *systemAppConfig = VOS_NULL_PTR;
    AT_PS_DataChanlCfg *chanCfg         = VOS_NULL_PTR;
    TAF_IFACE_Deactive  ifaceDown;
    TAF_Ctrl            ctrl;
    VOS_UINT32          i;

    if (clientId != AT_CLIENT_ID_APP) {
        AT_ERR_LOG1("AT_AppUsbSwitchGwMode clientid uncorrect, %d.\r", clientId);
        return;
    }

    g_hiLinkMode    = AT_HILINK_GATEWAY_MODE;
    systemAppConfig = AT_GetSystemAppConfigAddr();

    if (*systemAppConfig == SYSTEM_APP_WEBUI) {
        for (i = 1; i <= TAF_MAX_CID; i++) {
            chanCfg = AT_PS_GetDataChanlCfg(g_atClientTab[AT_CLIENT_ID_APP].clientId, (VOS_UINT8)i);

            if ((chanCfg->used == VOS_FALSE) || (chanCfg->portIndex >= AT_CLIENT_ID_BUTT)) {
                continue;
            }

            if ((chanCfg->ifaceId >= PS_IFACE_ID_RMNET0) && (chanCfg->ifaceId <= PS_IFACE_ID_RMNET2)) {
                memset_s(&ifaceDown, sizeof(ifaceDown), 0x00, sizeof(ifaceDown));
                memset_s(&ctrl, sizeof(ctrl), 0x00, sizeof(ctrl));

                ifaceDown.ifaceId  = (VOS_UINT8)chanCfg->ifaceId;
                ifaceDown.cause    = TAF_PS_CALL_END_CAUSE_NORMAL;
                ifaceDown.userType = TAF_IFACE_USER_TYPE_APP;

                /* 构造控制结构体 */
                (VOS_VOID)AT_PS_BuildIfaceCtrl(WUEPS_PID_AT, g_atClientTab[chanCfg->portIndex].clientId, 0, &ctrl);
                (VOS_VOID)TAF_IFACE_Down(&ctrl, &ifaceDown);
            }
        }
    }

    return;
}


VOS_VOID AT_GetAtMsgStruMsgLength(VOS_UINT32 infoLength, VOS_UINT32 *msgLength)
{
    if (infoLength > AT_MSG_CUSTOMIZED_ITEM_LEN) {
        *msgLength = (sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH) + (infoLength - AT_MSG_CUSTOMIZED_ITEM_LEN);
    } else {
        *msgLength = sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH;
    }

    return;
}


VOS_VOID AT_GetUserTypeFromIndex(VOS_UINT8 indexNum, VOS_UINT8 *userType)
{
    if (indexNum < AT_MAX_CLIENT_NUM) {
        *userType = g_atClientTab[indexNum].userType;
    } else {
        *userType = AT_BUTT_USER;
    }

    return;
}


VOS_VOID AT_VcomCmdStreamEcho(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* E5形态无需回显 */
    /* AGPS通道无需回显 */
    if ((*systemAppConfig != SYSTEM_APP_WEBUI)
#if (FEATURE_VCOM_EXT == FEATURE_ON)
        && (indexNum != AT_CLIENT_ID_APP9) && (indexNum != AT_CLIENT_ID_APP12) && (indexNum != AT_CLIENT_ID_APP24)
#endif
    ) {
        AT_MntnIncSndStreamCnt(indexNum);
        APP_VCOM_SEND((VOS_UINT8)g_atClientTab[indexNum].portNo, data, len);
    }

    return;
}


VOS_VOID AT_SetAts3Value(VOS_UINT8 value)
{
    if (g_atUsbDebugFlag == VOS_TRUE) {
        g_atS3 = value;
    }

    return;
}



VOS_BOOL AT_IsClientAllowEcho(VOS_UINT8 clientId)
{
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* UART NDIS 通道无需回显 */
    if ((clientId == AT_CLIENT_ID_UART) || (clientId == AT_CLIENT_ID_NDIS)) {
        return VOS_FALSE;
    }

    /* HSUART通道无需回显 */
#if (FEATURE_AT_HSUART == FEATURE_ON)
    if (clientId == AT_CLIENT_ID_HSUART) {
        return VOS_FALSE;
    }
#endif

    /* AGPS通道无需回显 */
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    if ((clientId == AT_CLIENT_ID_APP9) || (clientId == AT_CLIENT_ID_APP12) || (clientId == AT_CLIENT_ID_APP24)) {
        return VOS_FALSE;
    }
#endif

    /* E5形态无需回显 */
    if (*systemAppConfig == SYSTEM_APP_WEBUI) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID AT_CmdStreamEcho(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT32 ret;
    VOS_UINT16 echoLen;

    /* 判断pData码流的结尾是否为<CR><LF>形式，代码中2为回车换行两个字符长度 */
    if ((len > 2) && (g_atS3 == data[len - 2]) && (g_atS4 == data[len - 1])) {
        /* 删去结尾的<LF>字符 */
        echoLen = len - 1;
    } else {
        echoLen = len;
    }

    if (AT_IsClientAllowEcho(indexNum) == VOS_TRUE) {
        ret = DMS_PORT_SendData(g_atClientTab[indexNum].portNo, data, echoLen);
    } else {
        AT_ERR_LOG1("AT_CmdStreamEcho:WARNING: not allow echo, ucIndex:", indexNum);
        return;
    }

    if (ret == VOS_OK) {
        AT_MntnIncSndStreamCnt(indexNum);
    } else {
        AT_ERR_LOG1("AT_CmdStreamEcho:WARNING: Abnormal UserType,ucIndex:", indexNum);
    }

    return;
}


VOS_UINT32 At_CmdStreamPreProc(VOS_UINT8 indexNum, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT8 *head = TAF_NULL_PTR;
    errno_t    memResult;
    VOS_UINT16 count = 0;
    VOS_UINT16 total = 0;

    head = data;

    if (g_atUsbDebugFlag == VOS_TRUE) {
        PS_PRINTF_INFO("<At_CmdStreamPreProc> AtEType = %d, UserType = %d, g_atS3 = %d\n", g_atEType,
                       g_atClientTab[indexNum].userType, g_atS3);
    }

    /* 处理通道回显 */
    if (g_atEType == AT_E_ECHO_CMD) {
        AT_CmdStreamEcho(indexNum, data, len);
    }

    /*
     * MAC系统上的MP后台问题:AT+CMGS=**<CR><^z><Z>(或AT+CMGW=**<CR><^z><Z>)
     * 为了规避该问题，需要在接收到如上形式的码流后，
     * 需要将命令后的无效字符<^z><Z>删去
     */
    AT_DiscardInvalidCharForSms(data, &len);

    /* 解析到如下字符才将码流以消息方式发送到AT的消息队列中: <CR>/<ctrl-z>/<ESC> */
    while (count++ < len) {
        if (At_CheckSplitChar((*((data + count) - 1)))) {
            if (g_atDataBuff[indexNum].buffLen > 0) {
                if ((g_atDataBuff[indexNum].buffLen + count) >= AT_COM_BUFF_LEN) {
                    g_atDataBuff[indexNum].buffLen = 0;
                    AT_WARN_LOG("At_CmdStreamPreProc: buff is full at split char!");
                    return AT_FAILURE;
                }
                memResult = memcpy_s(&g_atDataBuff[indexNum].dataBuff[g_atDataBuff[indexNum].buffLen],
                                     AT_COM_BUFF_LEN - g_atDataBuff[indexNum].buffLen, head, count);
                TAF_MEM_CHK_RTN_VAL(memResult, AT_COM_BUFF_LEN - g_atDataBuff[indexNum].buffLen, count);
                At_SendCmdMsg(indexNum, g_atDataBuff[indexNum].dataBuff,
                              (TAF_UINT16)(g_atDataBuff[indexNum].buffLen + count), AT_NORMAL_TYPE_MSG);
                head                           = data + count;
                total                          = count;
                g_atDataBuff[indexNum].buffLen = 0;
            } else {
                At_SendCmdMsg(indexNum, head, count - total, AT_NORMAL_TYPE_MSG);
                head  = data + count;
                total = count;
            }
        }
    }

    if (total < len) {
        if ((g_atDataBuff[indexNum].buffLen + (len - total)) >= AT_COM_BUFF_LEN) {
            g_atDataBuff[indexNum].buffLen = 0;
            AT_WARN_LOG("At_CmdStreamPreProc: buff is full after split char!");
            return AT_FAILURE;
        }
        memResult = memcpy_s(&g_atDataBuff[indexNum].dataBuff[g_atDataBuff[indexNum].buffLen],
                             AT_COM_BUFF_LEN - g_atDataBuff[indexNum].buffLen, head, (TAF_UINT32)(len - total));
        TAF_MEM_CHK_RTN_VAL(memResult, AT_COM_BUFF_LEN - g_atDataBuff[indexNum].buffLen, (TAF_UINT32)(len - total));
        g_atDataBuff[indexNum].buffLen += (VOS_UINT16)((data - head) + len);
    }

    return AT_SUCCESS;
}


VOS_VOID AT_StopFlowCtrl(VOS_UINT8 indexNum)
{
    switch (g_atClientTab[indexNum].userType) {
        case AT_MODEM_USER:
            AT_MntnTraceStopFlowCtrl(indexNum, AT_FC_DEVICE_TYPE_MODEM);
            AT_CtrlCTS(indexNum, AT_IO_LEVEL_HIGH);
            break;

        case AT_HSUART_USER:
            AT_MntnTraceStopFlowCtrl(indexNum, AT_FC_DEVICE_TYPE_HSUART);
            AT_CtrlCTS(indexNum, AT_IO_LEVEL_HIGH);
            break;

        default:
            break;
    }

    return;
}


LOCAL VOS_UINT32 At_Om2AtSwitch(VOS_UINT8 *data, VOS_UINT16 len)
{
    /* 判断是否为+++的码流 */
    if ((len == 3) && (data[0] == '+') && (data[1] == '+') && (data[2] == '+')) {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_UINT32 At_OmDataProc(VOS_UINT16 portNo, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT32 rst;

    /* OM只处理UART PCUI CTRL口的数据 */
    switch (portNo) {
        case DMS_PORT_UART:
            if (g_atPortDataRcvFuncTab[AT_UART_PORT] == VOS_NULL_PTR) {
                AT_ERR_LOG("At_OmDataProc: Uart port proc func is NULL!");
                return VOS_ERR;
            }

            rst = g_atPortDataRcvFuncTab[AT_UART_PORT](data, (VOS_UINT32)len);
            break;

        case DMS_PORT_PCUI:
            /* 适配+++的码流后，将OM口切换到AT口 */
            if (At_Om2AtSwitch(data, len) == VOS_OK) {
                rst = AT_ResumeOm2AtMode();
                return rst;
            }

            if (g_atPortDataRcvFuncTab[AT_PCUI_PORT] == VOS_NULL_PTR) {
                AT_ERR_LOG("At_OmDataProc: PCUI proc func is NULL!");
                return VOS_ERR;
            }

            rst = g_atPortDataRcvFuncTab[AT_PCUI_PORT](data, (VOS_UINT32)len);
            break;

        case DMS_PORT_CTRL:
            if (g_atPortDataRcvFuncTab[AT_CTRL_PORT] == VOS_NULL_PTR) {
                AT_ERR_LOG("At_OmDataProc: CTRL proc func is NULL!");
                return VOS_ERR;
            }

            rst = g_atPortDataRcvFuncTab[AT_CTRL_PORT](data, (VOS_UINT32)len);
            break;

        case DMS_PORT_PCUI2:
            if (g_atPortDataRcvFuncTab[AT_PCUI2_PORT] == VOS_NULL_PTR) {
                AT_ERR_LOG("At_OmDataProc: PCUI2 proc func is NULL!");
                return VOS_ERR;
            }

            rst = g_atPortDataRcvFuncTab[AT_PCUI2_PORT](data, (VOS_UINT32)len);
            break;
#if (FEATURE_AT_HSUART == FEATURE_ON)
        case DMS_PORT_HSUART:
            if (g_atPortDataRcvFuncTab[AT_HSUART_PORT] == VOS_NULL_PTR) {
                AT_ERR_LOG("At_OmDataProc: HSUART proc func is NULL!");
                return VOS_ERR;
            }

            rst = g_atPortDataRcvFuncTab[AT_HSUART_PORT](data, (VOS_UINT32)len);
            break;
#endif
        default:
            AT_WARN_LOG("At_OmDataProc: don't proc data of this port!");
            return VOS_ERR;
    }

    return rst;
}


TAF_UINT32 At_DataStreamPreProc(TAF_UINT8 indexNum, TAF_UINT8 dataMode, TAF_UINT8 *data, TAF_UINT16 len)
{
    AT_LOG1("At_DataStreamPreProc ucIndex:", indexNum);
    AT_LOG1("At_DataStreamPreProc usLen:", len);
    AT_LOG1("At_DataStreamPreProc DataMode:", dataMode);

    switch (dataMode) {
        /* 调用OM提供的各端口接收数据函数发送数据，不需要再区分数据模式 */
        case AT_DIAG_DATA_MODE:
        case AT_OM_DATA_MODE:
            At_OmDataProc(g_atClientTab[indexNum].portNo, data, len);
            break;

        default:
            AT_WARN_LOG("At_DataStreamPreProc DataMode Wrong!");
            break;
    }
    return AT_SUCCESS;
}


VOS_UINT32 AT_PppDataModeRcvModemMsc(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru)
{
    /*
     * 1.判断(g_atClientTab[ucIndex].CmdCurrentOpt != AT_CMD_PS_DATA_CALL_END_SET)
     *  的目的:若正常断开，则之前已经执行了PDP DEACTIVE流程，此时若再拉低DTR，则
     *  无需再执行该异常流程
     * 2.该分支的处理场景:若出于流控状态下,用户发起PPP断开，此时PPP报文无法交互，
     *   只能在最后拉低DTR信号的时候，执行PPP断开操作
     */
    if (mscStru->op_dtr && (mscStru->uc_dtr == 0)) {
        if ((g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_PS_DATA_CALL_END_SET) ||
            (g_atClientTab[indexNum].cmdCurrentOpt == AT_CMD_WAIT_PPP_PROTOCOL_REL_SET)) {
            return AT_SUCCESS;
        }

        /*
         * 若处于流控状态下，则PPP断开的协商报文是无法传到UE侧的，
         * UE只能在DTR信号拉低的时候,执行PPP拨号断开操作
         */
        if ((g_atClientTab[indexNum].modemStatus & IO_CTRL_CTS) == 0) {
            AT_StopFlowCtrl((TAF_UINT8)indexNum);
        }

        /* 向PPP发送释放PPP操作 */
        PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_REL_PPP_REQ);

        /* 向PPP发送HDLC去使能操作 */
        PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_HDLC_DISABLE);

        /* 停止Modem口的AT定时器以及AT链路的当前操作指示 */
        AT_STOP_TIMER_CMD_READY(indexNum);

        /* EVENT - RCV Down DTR to Disconnect PPP in Abnormal procedure(PDP type:IP) ;index */
        AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DTE_DOWN_DTR_RELEASE_PPP_IP_TYPE, &indexNum, sizeof(indexNum));

        if (TAF_PS_CallEnd(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                           g_atClientTab[indexNum].cid) == VOS_OK) {
            /* 开定时器 */
            if (At_StartTimer(AT_SET_PARA_TIME, indexNum) != AT_SUCCESS) {
                AT_ERR_LOG("At_UsbModemStatusPreProc:ERROR:Start Timer");
                return AT_FAILURE;
            }

            /* 设置当前操作类型 */
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PS_DATA_CALL_END_SET;
        } else {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_IpDataModeRcvModemMsc(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru)
{
    if (mscStru->op_dtr && (mscStru->uc_dtr == 0)) {
        /* 若原先开启了流控，则需停止流控 */
        if ((g_atClientTab[indexNum].modemStatus & IO_CTRL_CTS) == 0) {
            AT_StopFlowCtrl((TAF_UINT8)indexNum);
        }

        PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_REL_PPP_RAW_REQ);

        /* 向PPP发送HDLC去使能操作 */
        PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_HDLC_DISABLE);

        /* 停止Modem口的AT定时器以及AT链路的当前操作指示 */
        AT_STOP_TIMER_CMD_READY(indexNum);
        ;

        /* EVENT - RCV Down DTR to Disconnect PPP in Abnormal procedure(PDP type:PPP) ;index */
        AT_EventReport(WUEPS_PID_AT, NAS_OM_EVENT_DTE_DOWN_DTR_RELEASE_PPP_PPP_TYPE, &indexNum, sizeof(indexNum));

        if (TAF_PS_CallEnd(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                           g_atClientTab[indexNum].cid) == VOS_OK) {
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_PS_DATA_CALL_END_SET;
            return AT_SUCCESS;
        } else {
            return AT_FAILURE;
        }
    }

    return AT_SUCCESS;
}


VOS_VOID AT_ModemProcDtrChange(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc)
{
    if (dceMsc->uc_dtr == 1) {
        /* 拉高DSR、CTS信号 */
        AT_CtrlDSR(indexNum, AT_IO_LEVEL_HIGH);
        AT_StopFlowCtrl(indexNum);
    } else {
        /*
         * 参考Q实现，DSR信号在上电后一直保持拉高状态，即使收到DTR也不拉低DSR；
         * 同时，PC在正常流程中一般不会拉低DTR信号，在异常流程中会将之拉低，
         * 所以UE在收到DTR拉低 的时候，需要将DCD拉低
         */

        AT_CtrlDCD(indexNum, AT_IO_LEVEL_LOW);
    }
}


VOS_UINT32 AT_ModemStartFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2)
{
    VOS_UINT8 indexNum;

    for (indexNum = 0; indexNum < AT_MAX_CLIENT_NUM; indexNum++) {
        if ((g_atClientTab[indexNum].userType == AT_MODEM_USER) && (g_atClientTab[indexNum].mode == AT_DATA_MODE)) {
            if ((g_atClientTab[indexNum].dataMode == AT_PPP_DATA_MODE) ||
                (g_atClientTab[indexNum].dataMode == AT_IP_DATA_MODE)) {
                AT_MntnTraceStartFlowCtrl(indexNum, AT_FC_DEVICE_TYPE_MODEM);
                AT_CtrlCTS(indexNum, AT_IO_LEVEL_LOW);
            }
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_ModemStopFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2)
{
    VOS_UINT8 indexNum;

    for (indexNum = 0; indexNum < AT_MAX_CLIENT_NUM; indexNum++) {
        if ((g_atClientTab[indexNum].userType == AT_MODEM_USER) && (g_atClientTab[indexNum].mode == AT_DATA_MODE)) {
            if ((g_atClientTab[indexNum].dataMode == AT_PPP_DATA_MODE) ||
                (g_atClientTab[indexNum].dataMode == AT_IP_DATA_MODE)) {
                AT_MntnTraceStopFlowCtrl(indexNum, AT_FC_DEVICE_TYPE_MODEM);
                AT_CtrlCTS(indexNum, AT_IO_LEVEL_HIGH);
            }
        }
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_ModemStatusPreProc(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru)
{
    NAS_OM_EventIdUint16 eventId;

#if (FEATURE_AT_HSUART == FEATURE_ON)
    VOS_UINT32 ret;
    ret = VOS_TRUE;
#endif

    if (mscStru == VOS_NULL_PTR) {
        return AT_FAILURE;
    }

    if (mscStru->op_dtr) {
        eventId = (mscStru->uc_dtr != 0) ? NAS_OM_EVENT_DTE_UP_DTR : NAS_OM_EVENT_DTE_DOWN_DTR;

        AT_EventReport(WUEPS_PID_AT, eventId, &indexNum, sizeof(VOS_UINT8));

        if (AT_CheckModemUser(indexNum) == VOS_TRUE) {
            AT_ModemProcDtrChange(indexNum, mscStru);
        }

#if (FEATURE_AT_HSUART == FEATURE_ON)
        if (AT_CheckHsUartUser(indexNum) == VOS_TRUE) {
            ret = AT_HSUART_ProcDtrChange(indexNum, mscStru);
            if (ret == VOS_FALSE) {
                return AT_SUCCESS;
            }
        }
#endif
    }

    /* 数传模式响应MSC处理 */
    if ((g_atClientTab[indexNum].mode == AT_DATA_MODE) || (g_atClientTab[indexNum].mode == AT_ONLINE_CMD_MODE)) {
        switch (g_atClientTab[indexNum].dataMode) {
            case AT_PPP_DATA_MODE:
                return AT_PppDataModeRcvModemMsc(indexNum, mscStru);

            case AT_IP_DATA_MODE:
                return AT_IpDataModeRcvModemMsc(indexNum, mscStru);

            default:
                AT_WARN_LOG("At_UsbModemStatusPreProc: DataMode Wrong!");
                break;
        }
    }

    return AT_SUCCESS;
}


VOS_VOID AT_ModemSetCtlStatus(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru)
{
    if (mscStru == TAF_NULL_PTR) {
        return;
    }

    /* 更新dsr信号 */
    if (mscStru->op_dsr) {
        if (mscStru->uc_dsr == 1) {
            g_atClientTab[indexNum].modemStatus |= IO_CTRL_DSR;
        } else {
            g_atClientTab[indexNum].modemStatus &= ~IO_CTRL_DSR;
        }
    }

    /* 更新CTS信号 */
    if (mscStru->op_cts) {
        if (mscStru->uc_cts == 1) {
            g_atClientTab[indexNum].modemStatus |= IO_CTRL_CTS;
        } else {
            g_atClientTab[indexNum].modemStatus &= ~IO_CTRL_CTS;
        }
    }

    /* 更新RI信号 */
    if (mscStru->op_ri) {
        if (mscStru->uc_ri == 1) {
            g_atClientTab[indexNum].modemStatus |= IO_CTRL_RI;
        } else {
            g_atClientTab[indexNum].modemStatus &= ~IO_CTRL_RI;
        }
    }

    /* 更新DCD信号 */
    if (mscStru->op_dcd) {
        if (mscStru->uc_dcd == 1) {
            g_atClientTab[indexNum].modemStatus |= IO_CTRL_DCD;
        } else {
            g_atClientTab[indexNum].modemStatus &= ~IO_CTRL_DCD;
        }
    }

    /* 更新FC信号 */
    if (mscStru->op_fc) {
        if (mscStru->uc_fc == 1) {
            g_atClientTab[indexNum].modemStatus |= IO_CTRL_FC;
        } else {
            g_atClientTab[indexNum].modemStatus &= ~IO_CTRL_FC;
        }
    }
}


VOS_UINT32 AT_SetModemStatus(VOS_UINT8 indexNum, struct modem_msc_stru *msc)
{
    VOS_UINT32 result;

    if (msc == VOS_NULL_PTR) {
        return AT_FAILURE;
    }

    if (indexNum >= AT_CLIENT_ID_BUTT) {
        return AT_FAILURE;
    }

    /* 更新本地管脚信号 */
    AT_ModemSetCtlStatus(indexNum, msc);

    /* 输出管脚信号可维可测 */
    AT_MntnTraceOutputMsc(indexNum, msc);

    /* 写入管脚信号参数 */
    switch (g_atClientTab[indexNum].userType) {
#if (FEATURE_AT_HSUART == FEATURE_ON)
        case AT_HSUART_USER:
#endif
        case AT_MODEM_USER:
            result = DMS_PORT_SendMscCmd(g_atClientTab[indexNum].portNo, msc);
            break;

        default:
            result = AT_SUCCESS;
            break;
    }

    return result;
}

/* 删除At_SetModemStatusForFC函数, 功能和At_SetModemStatus重复 */


VOS_UINT32 AT_ModemEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_MODEM;

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_MODEM, ID_AT_MNTN_RESULT_MODEM);
    /* 清空对应表项 */
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    /* 填写用户表项 */
    g_atClientTab[indexNum].clientId = AT_CLIENT_ID_MODEM;
    g_atClientTab[indexNum].portNo   = DMS_PORT_MODEM;
    g_atClientTab[indexNum].userType = AT_MODEM_USER;
    g_atClientTab[indexNum].used     = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    clientInfo.clientId    = AT_CLIENT_ID_MODEM;
    clientInfo.clientType  = DMS_CLIENT_TYPE_MODEM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_MODEM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_MODEM, &clientInfo);

    AT_NORM_LOG2("AT_ModemEst ucIndex and reg ret is ", indexNum, ret);

    return ret;
}


VOS_UINT32 At_ModemMscInd(VOS_UINT8 indexNum, struct modem_msc_stru *mscStru)
{
    AT_PPP_ModemMscInd *msg = VOS_NULL_PTR;
    VOS_UINT32          length;
    errno_t             memResult;
    VOS_UINT_PTR        tmpAddr;

    length = (sizeof(AT_PPP_ModemMscInd) - VOS_MSG_HEAD_LENGTH) + (sizeof(struct modem_msc_stru) - 4);
    /*lint -save -e830*/
    msg = (AT_PPP_ModemMscInd *)PS_ALLOC_MSG(PS_PID_APP_PPP, length);
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        /* 打印出错信息---申请消息包失败: */
        AT_WARN_LOG("At_ModemMscInd: Alloc AT_PPP_MODEM_MSC_IND_MSG_STRU msg fail!");
        return AT_FAILURE;
    }

    /* 填写消息头: */
    TAF_CfgMsgHdr((MsgBlock *)msg, PS_PID_APP_PPP, WUEPS_PID_AT, length);

    msg->msgHeader.msgName = AT_PPP_MODEM_MSC_IND_MSG;

    /* 填写消息体 */
    msg->idx = indexNum;

    /* 填写管脚数据 */
    tmpAddr = (VOS_UINT_PTR)(msg->MscInd);

    memResult = memcpy_s((VOS_VOID *)tmpAddr, sizeof(struct modem_msc_stru), (VOS_UINT8 *)mscStru,
                         sizeof(struct modem_msc_stru));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(struct modem_msc_stru), sizeof(struct modem_msc_stru));

    /* 发送消息 */
    if (PS_SEND_MSG(PS_PID_APP_PPP, msg) != VOS_OK) {
        /* 打印警告信息---发送消息失败: */
        AT_WARN_LOG("At_ModemMscInd:WARNING:SEND AT_PPP_MODEM_MSC_IND_MSG_STRU msg FAIL!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_SendZcDataToModem(VOS_UINT16 pppId, IMM_Zc *dataBuf)
{
    VOS_UINT32 result;
    VOS_UINT8  indexNum;

    indexNum = g_atPppIndexTab[pppId];

    if ((g_atClientTab[indexNum].mode == AT_CMD_MODE) || (g_atClientTab[indexNum].mode == AT_ONLINE_CMD_MODE)) {
        IMM_ZcFree(dataBuf);
        return AT_FAILURE;
    }

    switch (g_atClientTab[indexNum].userType) {
#if (FEATURE_AT_HSUART == FEATURE_ON)
        case AT_HSUART_USER:
#endif
        case AT_MODEM_USER:
            result = DMS_PORT_SendPrivData(g_atClientTab[indexNum].portNo, dataBuf);
            break;

        default:
            IMM_ZcFree(dataBuf);
            result = AT_FAILURE;
            break;
    }

    return result;
}


VOS_VOID AT_ModemRcvMscInd(VOS_UINT16 clientId, struct modem_msc_stru *rcvedMsc)
{
    VOS_UINT8 indexNum;

    if (clientId != AT_CLIENT_ID_MODEM) {
        AT_WARN_LOG1("AT_ModemRcvMscInd, WARNING, clientid abnormal %d!", clientId);
        return;
    }
    if (rcvedMsc == VOS_NULL_PTR) {
        AT_WARN_LOG("AT_ModemRcvMscInd, WARNING, Receive NULL pointer MSC info!");
        return;
    }

    /* MODEM索引号 */
    indexNum = (VOS_UINT8)clientId;

    /* 输入管脚信号可维可测 */
    AT_MntnTraceInputMsc(indexNum, (struct modem_msc_stru *)rcvedMsc);

    At_ModemMscInd(indexNum, (struct modem_msc_stru *)rcvedMsc);

    return;
}


VOS_VOID AT_SetUsbDebugFlag(VOS_UINT32 flag)
{
    g_atUsbDebugFlag = flag;
}


VOS_UINT32 At_UsbPcuiEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_PCUI;

    /* 清空对应表项 */
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_PCUI, ID_AT_MNTN_RESULT_PCUI);

    /* 填写用户表项 */
    g_atClientTab[indexNum].clientId = AT_CLIENT_ID_PCUI;
    g_atClientTab[indexNum].portNo   = DMS_PORT_PCUI;
    g_atClientTab[indexNum].userType = AT_USBCOM_USER;
    g_atClientTab[indexNum].used     = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    clientInfo.clientId    = AT_CLIENT_ID_PCUI;
    clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_PCUI, &clientInfo);

    AT_WARN_LOG2("At_UsbPcuiEst ucIndex and reg ret is ", indexNum, ret);

    return VOS_OK;
}


VOS_UINT32 At_UsbCtrEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_CTRL;

    /* 清空对应表项 */
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_CTRL, ID_AT_MNTN_RESULT_CTRL);

    /* 填写用户表项 */
    g_atClientTab[indexNum].clientId = AT_CLIENT_ID_CTRL;
    g_atClientTab[indexNum].portNo   = DMS_PORT_CTRL;
    g_atClientTab[indexNum].userType = AT_CTR_USER;
    g_atClientTab[indexNum].used     = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    clientInfo.clientId    = AT_CLIENT_ID_CTRL;
    clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_CTRL, &clientInfo);

    AT_WARN_LOG2("At_UsbCtrEst ucIndex and reg ret is ", indexNum, ret);

    return VOS_OK;
}


VOS_UINT32 At_UsbPcui2Est(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_PCUI2;

    /* 清空对应表项 */
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_PCUI2, ID_AT_MNTN_RESULT_PCUI2);

    /* 填写用户表项 */
    g_atClientTab[indexNum].clientId = AT_CLIENT_ID_PCUI2;
    g_atClientTab[indexNum].portNo   = DMS_PORT_PCUI2;
    g_atClientTab[indexNum].userType = AT_PCUI2_USER;
    g_atClientTab[indexNum].used     = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    clientInfo.clientId    = AT_CLIENT_ID_PCUI2;
    clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_PCUI2, &clientInfo);

    AT_WARN_LOG2("At_UsbPcui2Est ucIndex and reg ret is ", indexNum, ret);

    return VOS_OK;
}


VOS_UINT32 AT_UartSendRawDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 len)
{
    VOS_UINT32 result;

    result = DMS_PORT_SendDataSync(DMS_PORT_UART, virAddr, (VOS_UINT16)len);

    if (result != VOS_OK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID AT_UartInitLink(VOS_UINT8 indexNum)
{
    TAF_AT_NvimDefaultLinkOfUart defaultLinkType;

    defaultLinkType.uartLinkType = AT_UART_LINK_TYPE_BUTT;

    /* 清空对应表项 */
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));

    g_atClientTab[indexNum].portNo   = DMS_PORT_UART;
    g_atClientTab[indexNum].userType = AT_UART_USER;
    g_atClientTab[indexNum].used     = AT_CLIENT_USED;

    /* 读取UART端口默认工作模式NV项 */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_DEFAULT_LINK_OF_UART, &defaultLinkType.uartLinkType,
                          sizeof(defaultLinkType.uartLinkType)) != NV_OK) {
        /* NV项读取失败，将UART端口的工作模式设置为OM模式 */
        AT_ERR_LOG("AT_UART_InitLink:Read NV failed!");

        /* 记录AT/OM通道所对应的索引号 */
        g_atOmIndex = indexNum;

        /* 切换至OM数传模式 */
        At_SetMode(indexNum, AT_DATA_MODE, AT_OM_DATA_MODE);
        g_atClientTab[indexNum].dataState = AT_DATA_START_STATE;

        AT_AddUsedClientId2Tab(AT_CLIENT_ID_UART);

    } else {
        /* NV读取成功，检查UART端口的默认工作模式 */
        if (defaultLinkType.uartLinkType != AT_UART_LINK_TYPE_AT) {
            AT_NORM_LOG("AT_UART_InitLink:DEFAULT UART LINK TYPE is OM!");

            /* 记录AT/OM通道所对应的索引号 */
            g_atOmIndex = indexNum;

            /* 切换至OM数传模式 */
            At_SetMode(indexNum, AT_DATA_MODE, AT_OM_DATA_MODE);
            g_atClientTab[indexNum].dataState = AT_DATA_START_STATE;

            AT_AddUsedClientId2Tab(AT_CLIENT_ID_UART);

        } else {
            /* 填写用户表项 */
            g_atClientTab[indexNum].clientId = AT_CLIENT_ID_UART;

            /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
            g_atClientTab[indexNum].mode          = AT_CMD_MODE;
            g_atClientTab[indexNum].indMode       = AT_IND_MODE;
            g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
            g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
            g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
            g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;
        }
    }

    return;
}


VOS_VOID AT_UartInitPort(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_UART;

    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_UART, ID_AT_MNTN_RESULT_UART);

    AT_UartInitLink(indexNum);

    clientInfo.clientId    = AT_CLIENT_ID_UART;
    clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_UART, &clientInfo);

    AT_WARN_LOG2("AT_UART_InitPort ucIndex and reg ret is ", indexNum, ret);
}
#if (FEATURE_AT_HSUART == FEATURE_ON)

VOS_INT32 AT_HsuartCmdDataPreProc(AT_ClientManage *atClientManage, IMM_Zc *immZc)
{
    VOS_UINT8 *data = VOS_NULL_PTR;
    VOS_UINT16 len;

    data = immZc->data;
    len  = (VOS_UINT16)immZc->len;

    /* 若UART通道已经切入命令态，但此时仍然收到PPP帧或者OM数据，则直接丢弃 */
    if ((len > 0) && (data[0] == 0x7e) && (data[len - 1] == 0x7e)) {
        DMS_HSUART_DBG_UL_INVALID_CMD_DATA_NUM(1);
        DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
        return VOS_ERR;
    }

    DMS_HSUART_DBG_UL_VALID_CMD_DATA_NUM(1);

    /* 若是AT命令，则分发送给 AT */
    if (At_CmdStreamPreProc((VOS_UINT8)atClientManage->clientId, data, len) != AT_SUCCESS) {
        AT_WARN_LOG("AT_HsuartRcvPrivData: At_CmdStreamPreProc fail!");
    }

    DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
    return VOS_OK;
}


VOS_INT32 AT_HsuartRcvPrivData(VOS_UINT16 clientId, VOS_VOID *inputData)
{
    AT_ClientManage *atClientManage = VOS_NULL_PTR;
    IMM_Zc          *immZc          = VOS_NULL_PTR;
    VOS_UINT8       *data           = VOS_NULL_PTR;
    VOS_UINT16       len;

    atClientManage = AT_GetClientManage(clientId);

    if (atClientManage == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_HsuartRcvPrivData: get client manage fail!");
        return VOS_ERR;
    }

    immZc = (IMM_Zc *)inputData;

    if (clientId != AT_CLIENT_ID_HSUART) {
        AT_ERR_LOG1("AT_HsuartRcvPrivData: uncorret clientid %d!", clientId);
        DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
        return VOS_ERR;
    }

    /* 从pstData(IMM_Zc类型)中取出数据内容和长度，分别保存在pData和usLen中 */
    data = immZc->data;
    len  = (VOS_UINT16)immZc->len;

    /* 如果当前处于命令模式 或者是 online_command模式 */
    if ((atClientManage->mode == AT_CMD_MODE) || (atClientManage->mode == AT_ONLINE_CMD_MODE)) {
        return AT_HsuartCmdDataPreProc(atClientManage, immZc);
    }

    /* 根据UART口的状态进行分发 */
    switch (atClientManage->dataMode) {
        /* 处理PPP data数据 */
        case AT_PPP_DATA_MODE:

            /* PPP负责释放上行内存 */
            PPP_PullPacketEvent(atClientManage->pppId, immZc);
            DMS_HSUART_DBG_UL_IP_DATA_NUM(1);
            return VOS_OK;

        /* 处理IP data 数据 */
        case AT_IP_DATA_MODE:

            /* PPP负责释放上行内存 */
            PPP_PullRawDataEvent(atClientManage->pppId, immZc);
            DMS_HSUART_DBG_UL_PPP_DATA_NUM(1);
            return VOS_OK;

        /* 处理OM数据 */
        case AT_DIAG_DATA_MODE:
        case AT_OM_DATA_MODE:
            At_OmDataProc(atClientManage->portNo, data, len);
            DMS_HSUART_DBG_UL_OM_DATA_NUM(1);
            break;

        default:
            AT_WARN_LOG("AT_HsuartRcvPrivData: DataMode Wrong!");
            DMS_HSUART_DBG_UL_INVALID_DATA_NUM(1);
            break;
    }

    DMS_PORT_FreeUlDataBuff(atClientManage->portNo, (VOS_VOID *)immZc);
    return VOS_OK;
}


VOS_UINT32 AT_HSUART_IsBaudRateValid(AT_UartBaudrateUint32 baudRate)
{
    VOS_UINT32 ret = VOS_FALSE;

    /*
     * ARM   --- 0,300,600,1200,2400,4800,9600,19200,38400,57600,115200,
     * 230400,460800,921600,2764800,4000000
     */
    switch (baudRate) {
        case AT_UART_BAUDRATE_0:
        case AT_UART_BAUDRATE_300:
        case AT_UART_BAUDRATE_600:
        case AT_UART_BAUDRATE_1200:
        case AT_UART_BAUDRATE_2400:
        case AT_UART_BAUDRATE_4800:
        case AT_UART_BAUDRATE_9600:
        case AT_UART_BAUDRATE_19200:
        case AT_UART_BAUDRATE_38400:
        case AT_UART_BAUDRATE_57600:
        case AT_UART_BAUDRATE_115200:
        case AT_UART_BAUDRATE_230400:
        case AT_UART_BAUDRATE_460800:
        case AT_UART_BAUDRATE_921600:
        case AT_UART_BAUDRATE_2764800:
        case AT_UART_BAUDRATE_4000000:
            ret = VOS_TRUE;
            break;

        default:
            ret = VOS_FALSE;
            break;
    }

    return ret;
}


VOS_UINT32 AT_HSUART_IsFormatValid(AT_UartFormatUint8 format)
{
    VOS_UINT32 ret = VOS_FALSE;

    switch (format) {
        case AT_UART_FORMAT_8DATA_2STOP:
        case AT_UART_FORMAT_8DATA_1PARITY_1STOP:
        case AT_UART_FORMAT_8DATA_1STOP:
        case AT_UART_FORMAT_7DATA_2STOP:
        case AT_UART_FORMAT_7DATA_1PARITY_1STOP:
        case AT_UART_FORMAT_7DATA_1STOP:
            ret = VOS_TRUE;
            break;

        default:
            ret = VOS_FALSE;
            break;
    }

    return ret;
}


VOS_UINT32 AT_HSUART_IsParityValid(AT_UartParityUint8 parity)
{
    VOS_UINT32 ret = VOS_FALSE;

    /*
     * 注: 不同版本选用的UART IP不同, 校验规格差异如下
     * V3R3            --- ODD, EVEN, MARK, SPACE
     * V7R11(or later) --- ODD, EVEN
     */
    switch (parity) {
        case AT_UART_PARITY_ODD:
        case AT_UART_PARITY_EVEN:
            ret = VOS_TRUE;
            break;

        default:
            ret = VOS_FALSE;
            break;
    }

    return ret;
}


VOS_UINT32 AT_HSUART_ValidateFlowCtrlParam(AT_UartFcDceByDteUint8 fcDceByDte, AT_UartFcDteByDceUint8 fcDteByDce)
{
    /*
     * 注: 不同版本选用的UART IP不同, 流控规格差异如下
     * V3R3            --- 硬件流控支持上下行单独开启或关闭
     * V7R11(or later) --- 硬件流控支持上下行同时开启或关闭
     */
    if (fcDceByDte != fcDteByDce) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 AT_HSUART_ValidateCharFrameParam(AT_UartFormatUint8 format, AT_UartParityUint8 parity)
{
    /* 检查格式类型是否支持 */
    if (AT_HSUART_IsFormatValid(format) == VOS_FALSE) {
        return VOS_FALSE;
    }

    /* 检查检验方式是否支持 */
    if (AT_HSUART_IsParityValid(parity) == VOS_FALSE) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


AT_UartFormatParam* AT_HSUART_GetFormatParam(AT_UartFormatUint8 format)
{
    AT_UartFormatParam *formatTblPtr = VOS_NULL_PTR;
    AT_UartFormatParam *formatParam  = VOS_NULL_PTR;
    VOS_UINT32          cnt;

    formatTblPtr = AT_UART_GET_FORMAT_TBL_PTR();

    for (cnt = 0; cnt < AT_UART_GET_FORMAT_TBL_SIZE(); cnt++) {
        if (format == formatTblPtr[cnt].format) {
            formatParam = &formatTblPtr[cnt];
        }
    }

    return formatParam;
}


VOS_UINT32 AT_HSUART_GetUdiValueByDataLen(AT_UartDataLenUint8 dataLen, VOS_UINT32 *udiValue)
{
    /* 指针非空检测 */
    if (udiValue == VOS_NULL_PTR) {
        return VOS_ERR;
    }

    /* 映射底软UDI VALUE */
    switch (dataLen) {
        case AT_UART_DATA_LEN_5_BIT:
            *udiValue = WLEN_5_BITS;
            break;

        case AT_UART_DATA_LEN_6_BIT:
            *udiValue = WLEN_6_BITS;
            break;

        case AT_UART_DATA_LEN_7_BIT:
            *udiValue = WLEN_7_BITS;
            break;

        case AT_UART_DATA_LEN_8_BIT:
            *udiValue = WLEN_8_BITS;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_GetUdiValueByStopLen(AT_UartStopLenUint8 stopLen, VOS_UINT32 *udiValue)
{
    /* 指针非空检测 */
    if (udiValue == VOS_NULL_PTR) {
        return VOS_ERR;
    }

    /* 映射底软UDI VALUE */
    switch (stopLen) {
        case AT_UART_STOP_LEN_1_BIT:
            *udiValue = STP2_OFF;
            break;

        case AT_UART_STOP_LEN_2_BIT:
            *udiValue = STP2_ON;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_GetUdiValueByParity(AT_UartParityUint8 parity, VOS_UINT32 *udiValue)
{
    /* 指针非空检测 */
    if (udiValue == VOS_NULL_PTR) {
        return VOS_ERR;
    }

    /* 映射底软UDI VALUE */
    switch (parity) {
        case AT_UART_PARITY_ODD:
            *udiValue = PARITY_CHECK_ODD;
            break;

        case AT_UART_PARITY_EVEN:
            *udiValue = PARITY_CHECK_EVEN;
            break;

        case AT_UART_PARITY_MARK:
            *udiValue = PARITY_CHECK_MARK;
            break;

        case AT_UART_PARITY_SPACE:
            *udiValue = PARITY_CHECK_SPACE;
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_ConfigFlowCtrl(VOS_UINT8 indexNum, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag)
{
    if (DMS_PORT_ConfigFlowCtrl(DMS_PORT_HSUART, rtsFlag, ctsFlag) != VOS_OK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_ConfigCharFrame(VOS_UINT8 indexNum, AT_UartFormatUint8 format, AT_UartParityUint8 parity)
{
    AT_UartFormatParam *formatParam = VOS_NULL_PTR;
    VOS_UINT32          udiDataLenth;
    VOS_UINT32          udiStpLenth;
    VOS_UINT32          udiParity;
    VOS_UINT32          result;

    /* 参数初始化 */
    udiDataLenth = WLEN_8_BITS;
    udiStpLenth  = STP2_OFF;
    udiParity    = PARITY_NO_CHECK;

    /* 将设置的帧格式和校验方法转换为DRV接口格式 */
    formatParam = AT_HSUART_GetFormatParam(format);
    if (formatParam == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_HSUART_ConfigCharFrame: Format is invalid!");
        return VOS_ERR;
    }

    result = AT_HSUART_GetUdiValueByDataLen(formatParam->dataBitLength, &udiDataLenth);
    if (result != VOS_OK) {
        AT_ERR_LOG("AT_HSUART_ConfigCharFrame: Data bit length is invalid!");
        return VOS_ERR;
    }

    result = AT_HSUART_GetUdiValueByStopLen(formatParam->stopBitLength, &udiStpLenth);
    if (result != VOS_OK) {
        AT_ERR_LOG("AT_HSUART_ConfigCharFrame: Stop bit length is invalid!");
        return VOS_ERR;
    }

    if (formatParam->parityBitLength == AT_UART_PARITY_LEN_1_BIT) {
        result = AT_HSUART_GetUdiValueByParity(parity, &udiParity);
        if (result != VOS_OK) {
            AT_ERR_LOG("AT_HSUART_ConfigCharFrame: Parity bit length is invalid!");
            return VOS_ERR;
        }
    }

    /* 调用DRV函数设置UART帧格式参数（包括串口数据位长度/停止位长度/校验位） */
    if (DMS_PORT_SetCharFrame(DMS_PORT_HSUART, udiDataLenth, udiStpLenth, udiParity) != VOS_OK) {
        AT_ERR_LOG("AT_HSUART_ConfigCharFrame: Set CharFrame failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_ConfigBaudRate(VOS_UINT8 indexNum, AT_UartBaudrateUint32 baudRate)
{
    /* 调用DRV函数设置串口的波特率 */
    if (DMS_PORT_SetBaud(DMS_PORT_HSUART, baudRate) != VOS_OK) {
        AT_ERR_LOG("AT_HSUART_ConfigBaudRate: Set Baud failed!");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_ProcDtrChange(VOS_UINT8 indexNum, struct modem_msc_stru *dceMsc)
{
    AT_UartLineCtrl *lineCtrl = VOS_NULL_PTR;
    VOS_UINT32       ret;

    lineCtrl = AT_GetUartLineCtrlInfo();
    ret      = VOS_TRUE;

    if (dceMsc->uc_dtr == 1) {
        /* 判断&S[<value>] */
        if (lineCtrl->dsrMode == AT_UART_DSR_MODE_ALWAYS_ON) {
            AT_CtrlDSR(indexNum, AT_IO_LEVEL_HIGH);
        }

        /* 判断&C[<value>] */
        if (lineCtrl->dcdMode == AT_UART_DCD_MODE_ALWAYS_ON) {
            AT_CtrlDCD(indexNum, AT_IO_LEVEL_HIGH);
        }

        /* 停止流控 */
        AT_StopFlowCtrl(indexNum);
    } else {
        ret = AT_HSUART_ProcDtrCtrlMode();
    }

    return ret;
}


VOS_UINT32 AT_HSUART_ProcDtrCtrlMode(VOS_VOID)
{
    AT_UartLineCtrl *lineCtrl = VOS_NULL_PTR;
    VOS_UINT32       ret;
    VOS_UINT8        indexNum;

    lineCtrl = AT_GetUartLineCtrlInfo();
    indexNum = AT_CLIENT_ID_HSUART;

    switch (lineCtrl->dtrMode) {
        case AT_UART_DTR_MODE_IGNORE:
            ret = VOS_FALSE;
            break;

        case AT_UART_DTR_MODE_SWITCH_CMD_MODE:
            /* 目前只支持PPP和IP模式下切换为ONLINE-COMMAND模式 */
            if ((g_atClientTab[indexNum].mode == AT_DATA_MODE) &&
                ((g_atClientTab[indexNum].dataMode == AT_PPP_DATA_MODE) ||
                 (g_atClientTab[indexNum].dataMode == AT_IP_DATA_MODE))) {
                if (lineCtrl->dsrMode == AT_UART_DSR_MODE_CONNECT_ON) {
                    AT_CtrlDSR(indexNum, AT_IO_LEVEL_LOW);
                }

                At_SetMode(indexNum, AT_ONLINE_CMD_MODE, AT_NORMAL_MODE);
                At_FormatResultData(indexNum, AT_OK);
            }
            ret = VOS_FALSE;
            break;

        case AT_UART_DTR_MODE_HANGUP_CALL:
            ret = VOS_TRUE;
            break;

        default:
            ret = VOS_FALSE;
            break;
    }

    return ret;
}


VOS_UINT32 AT_HSUART_StartFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2)
{
    VOS_UINT8 indexNum;

    indexNum = AT_CLIENT_ID_HSUART;

    if (g_atClientTab[indexNum].mode == AT_DATA_MODE) {
        if ((g_atClientTab[indexNum].dataMode == AT_PPP_DATA_MODE) ||
            (g_atClientTab[indexNum].dataMode == AT_IP_DATA_MODE)) {
            AT_MntnTraceStartFlowCtrl(indexNum, AT_FC_DEVICE_TYPE_HSUART);
            AT_CtrlCTS(indexNum, AT_IO_LEVEL_LOW);
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_StopFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2)
{
    VOS_UINT8 indexNum;

    indexNum = AT_CLIENT_ID_HSUART;

    if (g_atClientTab[indexNum].mode == AT_DATA_MODE) {
        if ((g_atClientTab[indexNum].dataMode == AT_PPP_DATA_MODE) ||
            (g_atClientTab[indexNum].dataMode == AT_IP_DATA_MODE)) {
            AT_MntnTraceStopFlowCtrl(indexNum, AT_FC_DEVICE_TYPE_HSUART);
            AT_CtrlCTS(indexNum, AT_IO_LEVEL_HIGH);
        }
    }

    return VOS_OK;
}


VOS_UINT32 AT_HSUART_SendRawDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 len)
{
    VOS_UINT32 result;
    VOS_UINT8  indexNum;

    indexNum = AT_CLIENT_ID_HSUART;

    result = DMS_PORT_SendDataSync(g_atClientTab[indexNum].portNo, virAddr, (VOS_UINT16)len);
    if (result != VOS_OK) {
        AT_ERR_LOG("AT_HSUART_SendRawDataFromOm: Send data failed!\r\n");
        DMS_HSUART_DBG_DL_WRITE_SYNC_FAIL_NUM(1);
        return VOS_ERR;
    }

    DMS_HSUART_DBG_DL_WRITE_SYNC_SUCC_NUM(1);

    return VOS_OK;
}


VOS_VOID AT_HsuartRcvMscInd(VOS_UINT16 clientId, struct modem_msc_stru *dceMsc)
{
    VOS_UINT8 indexNum;

    if (clientId != AT_CLIENT_ID_HSUART) {
        AT_WARN_LOG1("AT_HsuartRcvMscInd, WARNING, clientid abnormal %d!", clientId);
        return;
    }

    /* 入参检查 */
    if (dceMsc == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_HsuartRcvMscInd: pstDceMsc is NULL!");
        return;
    }

    /* 输入管脚信号可维可测 */
    indexNum = (VOS_UINT8)clientId;

    /* 将管脚信息发送出去 */
    At_ModemMscInd(indexNum, dceMsc);

    return;
}


VOS_VOID AT_HsuartRcvEscapeSequence(VOS_UINT16 clientId)
{
    AT_Msg *msg = VOS_NULL_PTR;
    errno_t memResult;

    if (clientId != AT_CLIENT_ID_HSUART) {
        AT_ERR_LOG1("AT_HsuartRcvEscapeSequence: clientid abnormal %d!", clientId);
        return;
    }

    /* 构造消息 */
    msg = (AT_Msg *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT, sizeof(AT_Msg));

    if (msg == VOS_NULL_PTR) {
        AT_ERR_LOG("AT_HsuartRcvEscapeSequence: Alloc message failed!");
        return;
    }

    /* 初始化消息 */
    memResult = memset_s((VOS_CHAR *)msg + VOS_MSG_HEAD_LENGTH, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH, 0x00,
                         sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);

    /* 填写消息头 */
    TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, WUEPS_PID_AT, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);
    msg->msgId = ID_AT_SWITCH_CMD_MODE;

    /* 填写消息内容 */
    msg->type     = AT_SWITCH_CMD_MODE_MSG;
    msg->indexNum = (VOS_UINT8)clientId;

    /* 发送消息 */
    if (PS_SEND_MSG(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_ERR_LOG("AT_HsuartRcvEscapeSequence: Send message failed!");
    }

    return;
}


VOS_VOID AT_HsuartRcvWaterMarkInd(VOS_UINT16 clientId, water_level level)
{
    AT_UartCtx *uartCtx = VOS_NULL_PTR;
    AT_Msg     *msg     = VOS_NULL_PTR;
    errno_t     memResult;

    if (clientId != AT_CLIENT_ID_HSUART) {
        AT_ERR_LOG1("AT_HsuartRcvWaterMarkInd: clientid abnormal %d!", clientId);
        return;
    }
    uartCtx = AT_GetUartCtxAddr();

    /*
     * (1) 更新TX高水线标识
     * (2) 如果TX达到低水线, 发送低水线内部消息, 处理相关流程
     */

    uartCtx->txWmHighFlg = (level == HIGH_LEVEL) ? VOS_TRUE : VOS_FALSE;

    if (level == LOW_LEVEL) {
        /* 申请OSA消息 */
        msg = (AT_Msg *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_Msg));
        if (msg == VOS_NULL_PTR) {
            AT_ERR_LOG("AT_HsuartRcvEscapeSequence: Alloc message failed!");
            return;
        }

        /* 清空消息内容 */
        memResult = memset_s(AT_GET_MSG_ENTITY(msg), AT_GET_MSG_LENGTH(msg), 0x00, AT_GET_MSG_LENGTH(msg));
        TAF_MEM_CHK_RTN_VAL(memResult, AT_GET_MSG_LENGTH(msg), AT_GET_MSG_LENGTH(msg));

        /* 填写消息头 */
        AT_CFG_INTRA_MSG_HDR(msg, ID_AT_WATER_LOW_CMD);

        /* 填写消息内容 */
        msg->type     = AT_WATER_LOW_MSG;
        msg->indexNum = (VOS_UINT8)clientId;

        /* 发送消息 */
        AT_SEND_MSG(msg);
    }

    return;
}


VOS_VOID AT_HSUART_InitLink(VOS_UINT8 indexNum)
{
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));

    g_atClientTab[indexNum].clientId      = AT_CLIENT_ID_HSUART;
    g_atClientTab[indexNum].used          = AT_CLIENT_USED;
    g_atClientTab[indexNum].userType      = AT_HSUART_USER;
    g_atClientTab[indexNum].portNo        = DMS_PORT_HSUART;
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    return;
}


VOS_UINT32 AT_HSUART_InitPort(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_HSUART;

    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    /* 配置HSUART端口消息映射 */
    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_HSUART, ID_AT_MNTN_RESULT_HSUART);

    /* 初始化UART链路 */
    AT_HSUART_InitLink(indexNum);

    clientInfo.clientId    = AT_CLIENT_ID_HSUART;
    clientInfo.clientType  = DMS_CLIENT_TYPE_HSUART;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_HSUART];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_HSUART, &clientInfo);

    AT_WARN_LOG2("AT_HSUART_InitPort ucIndex and reg ret is ", indexNum, ret);

    return ret;
}


VOS_VOID AT_HSUART_ConfigDefaultPara(VOS_VOID)
{
    AT_UartPhyCfg *phyCfg = VOS_NULL_PTR;
    phyCfg                = AT_GetUartPhyCfgInfo();

    /* 配置UART波特率 */
    AT_HSUART_ConfigBaudRate(AT_CLIENT_ID_HSUART, phyCfg->baudRate);

    /* 配置UART帧格式 */
    AT_HSUART_ConfigCharFrame(AT_CLIENT_ID_HSUART, phyCfg->frame.format, phyCfg->frame.parity);
}

#endif


VOS_VOID AT_CtrlDCD(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel)
{
    struct modem_msc_stru dceMsc;
    NAS_OM_EventIdUint16  eventId;

    memset_s(&dceMsc, sizeof(dceMsc), 0x00, sizeof(struct modem_msc_stru));

    dceMsc.op_dcd = VOS_TRUE;
    dceMsc.uc_dcd = ioLevel;

    AT_SetModemStatus(indexNum, &dceMsc);

    eventId = (ioLevel == AT_IO_LEVEL_HIGH) ? NAS_OM_EVENT_DCE_UP_DCD : NAS_OM_EVENT_DCE_DOWN_DCD;

    AT_EventReport(WUEPS_PID_AT, eventId, &indexNum, sizeof(VOS_UINT8));

    return;
}


VOS_VOID AT_CtrlDSR(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel)
{
    struct modem_msc_stru dceMsc;
    NAS_OM_EventIdUint16  eventId;

    memset_s(&dceMsc, sizeof(dceMsc), 0x00, sizeof(struct modem_msc_stru));

    dceMsc.op_dsr = VOS_TRUE;
    dceMsc.uc_dsr = ioLevel;

    AT_SetModemStatus(indexNum, &dceMsc);

    eventId = (ioLevel == AT_IO_LEVEL_HIGH) ? NAS_OM_EVENT_DCE_UP_DSR : NAS_OM_EVENT_DCE_DOWN_DSR;

    AT_EventReport(WUEPS_PID_AT, eventId, &indexNum, sizeof(VOS_UINT8));

    return;
}


VOS_VOID AT_CtrlCTS(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel)
{
    struct modem_msc_stru dceMsc;
    NAS_OM_EventIdUint16  eventId;

    memset_s(&dceMsc, sizeof(dceMsc), 0x00, sizeof(struct modem_msc_stru));

    dceMsc.op_cts = VOS_TRUE;
    dceMsc.uc_cts = ioLevel;

    AT_SetModemStatus(indexNum, &dceMsc);

    eventId = (ioLevel == AT_IO_LEVEL_HIGH) ? NAS_OM_EVENT_DCE_UP_CTS : NAS_OM_EVENT_DCE_DOWN_CTS;

    AT_EventReport(WUEPS_PID_AT, eventId, &indexNum, sizeof(VOS_UINT8));

    return;
}


VOS_VOID AT_CtrlRI(VOS_UINT8 indexNum, AT_IoLevelUint8 ioLevel)
{
    struct modem_msc_stru dceMsc;
    NAS_OM_EventIdUint16  eventId;

    memset_s(&dceMsc, sizeof(dceMsc), 0x00, sizeof(struct modem_msc_stru));

    dceMsc.op_ri = VOS_TRUE;
    dceMsc.uc_ri = ioLevel;

    AT_SetModemStatus(indexNum, &dceMsc);

    eventId = (ioLevel == AT_IO_LEVEL_HIGH) ? NAS_OM_EVENT_DCE_UP_RI : NAS_OM_EVENT_DCE_DOWN_RI;

    AT_EventReport(WUEPS_PID_AT, eventId, &indexNum, sizeof(VOS_UINT8));

    return;
}


AT_IoLevelUint8 AT_GetIoLevel(VOS_UINT8 indexNum, VOS_UINT8 ioCtrl)
{
    if ((g_atClientTab[indexNum].modemStatus & ioCtrl) == 0) {
        return AT_IO_LEVEL_LOW;
    }

    return AT_IO_LEVEL_HIGH;
}


VOS_INT32 AT_AppComEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;
    VOS_UINT8             loop;

    for (loop = 0; loop < AT_VCOM_AT_CHANNEL_MAX; loop++) {
        indexNum = AT_CLIENT_ID_APP + loop;

        /* 清空对应表项 */
        memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
        memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

        AT_ConfigTraceMsg(indexNum, (ID_AT_CMD_APP + loop), (ID_AT_MNTN_RESULT_APP + loop));

        g_atClientTab[indexNum].clientId = AT_CLIENT_ID_APP + loop;

        /* 填写用户表项 */
        g_atClientTab[indexNum].portNo        = DMS_PORT_APP + loop;
        g_atClientTab[indexNum].userType      = AT_APP_USER;
        g_atClientTab[indexNum].used          = AT_CLIENT_USED;
        g_atClientTab[indexNum].mode          = AT_CMD_MODE;
        g_atClientTab[indexNum].indMode       = AT_IND_MODE;
        g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
        g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
        g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

        clientInfo.clientId    = indexNum;
        clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
        clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

        ret = DMS_PORT_RegClientInfo(g_atClientTab[indexNum].portNo, &clientInfo);
        AT_WARN_LOG2("AT_AppComEst ucIndex and reg ret is ", indexNum, ret);
    }
    return VOS_OK;
}


VOS_UINT32 AT_SockEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_SOCK;

    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_SOCK, ID_AT_MNTN_RESULT_SOCK);

    g_atClientTab[indexNum].clientId      = AT_CLIENT_ID_SOCK;
    g_atClientTab[indexNum].portNo        = DMS_PORT_SOCK;
    g_atClientTab[indexNum].userType      = AT_SOCK_USER;
    g_atClientTab[indexNum].used          = AT_CLIENT_USED;
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    clientInfo.clientId    = AT_CLIENT_ID_SOCK;
    clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_SOCK, &clientInfo);

    AT_WARN_LOG2("AT_SockEst ucIndex and reg ret is ", indexNum, ret);

    return ret;
}


VOS_VOID AT_NcmConnStatusChgProc(enum ncm_ioctrl_connect_stus status)
{
    AT_Msg *msg = VOS_NULL_PTR;
    errno_t memResult;

    /*
     * 发送网卡断开内部消息, 处理相关流程
     */

    if (status == NCM_IOCTL_STUS_BREAK) {
        /* 申请OSA消息 */
        /*lint -save -e516 */
        msg = (AT_Msg *)AT_ALLOC_MSG_WITH_HDR(sizeof(AT_Msg));
        /*lint -restore */
        if (msg == VOS_NULL_PTR) {
            AT_ERR_LOG("AT_NcmRcvConnStatusChg: Alloc message failed!");
            return;
        }

        /* 清空消息内容 */
        memResult = memset_s(AT_GET_MSG_ENTITY(msg), AT_GET_MSG_LENGTH(msg), 0x00, AT_GET_MSG_LENGTH(msg));
        TAF_MEM_CHK_RTN_VAL(memResult, AT_GET_MSG_LENGTH(msg), AT_GET_MSG_LENGTH(msg));

        /* 填写消息头 */
        AT_CFG_INTRA_MSG_HDR(msg, ID_AT_NCM_CONN_STATUS_CMD);

        /* 填写消息内容 */
        msg->type     = AT_NCM_CONN_STATUS_MSG;
        msg->indexNum = AT_CLIENT_ID_NDIS;

        /* 发送消息 */
        AT_SEND_MSG(msg);
    }

    return;
}


VOS_VOID AT_NcmRcvConnStatusChg(VOS_UINT16 clientId, enum ncm_ioctrl_connect_stus status)
{
    if (clientId != AT_CLIENT_ID_NDIS) {
        AT_ERR_LOG1("AT_NcmRcvConnStatusChg: clientid abnormal, %d\n", clientId);
        return;
    }

    AT_NcmConnStatusChgProc(status);
}



VOS_UINT32 AT_UsbNdisEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;
    VOS_UINT8             indexNum;

    indexNum = AT_CLIENT_ID_NDIS;

    /* 清空对应表项 */
    memset_s(&g_atClientTab[indexNum], sizeof(AT_ClientManage), 0x00, sizeof(AT_ClientManage));
    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    AT_ConfigTraceMsg(indexNum, ID_AT_CMD_NDIS, ID_AT_MNTN_RESULT_NDIS);

    /* 填写用户表项 */
    g_atClientTab[indexNum].clientId = AT_CLIENT_ID_NDIS;
    g_atClientTab[indexNum].portNo   = DMS_PORT_NCM_CTRL;
    g_atClientTab[indexNum].userType = AT_NDIS_USER;
    g_atClientTab[indexNum].used     = AT_CLIENT_USED;

    /* 以下可以不用填写，前面PS_MEMSET已经初始化，只为可靠起见 */
    g_atClientTab[indexNum].mode          = AT_CMD_MODE;
    g_atClientTab[indexNum].indMode       = AT_IND_MODE;
    g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
    g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
    g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;

    clientInfo.clientId    = AT_CLIENT_ID_NDIS;
    clientInfo.clientType  = DMS_CLIENT_TYPE_COM;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_COM];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_NCM_CTRL, &clientInfo);

    AT_WARN_LOG2("AT_UsbNdisEst ucIndex and reg ret is ", indexNum, ret);

    return VOS_OK;
}


VOS_UINT32 AT_UsbNdisDataEst(VOS_VOID)
{
    struct DMS_ClientInfo clientInfo;
    VOS_UINT32            ret;

    memset_s(&clientInfo, sizeof(struct DMS_ClientInfo), 0x00, sizeof(struct DMS_ClientInfo));

    /* NDIS data 通道不参与at命令处理，所以client id为0 */
    clientInfo.clientId    = 0;
    clientInfo.clientType  = DMS_CLIENT_TYPE_NDIS_DATA;
    clientInfo.clientCBTbl = &g_AtClientCBTbl[DMS_CLIENT_TYPE_NDIS_DATA];

    ret = DMS_PORT_RegClientInfo(DMS_PORT_NCM_DATA, &clientInfo);

    AT_WARN_LOG1("AT_UsbNdisDataEst reg ret is ", ret);

    return ret;
}


VOS_UINT32 AT_CheckPcuiCtrlConcurrent(VOS_UINT8 indexOne, VOS_UINT8 indexTwo)
{
    VOS_UINT32 userFlg1;
    VOS_UINT32 userFlg2;

    userFlg1 = AT_CheckUserType(indexOne, AT_USBCOM_USER);
    userFlg1 |= AT_CheckUserType(indexOne, AT_CTR_USER);
    userFlg1 |= AT_CheckUserType(indexOne, AT_PCUI2_USER);

    if (userFlg1 != VOS_TRUE) {
        return VOS_FALSE;
    }

    userFlg2 = AT_CheckUserType(indexTwo, AT_USBCOM_USER);
    userFlg2 |= AT_CheckUserType(indexTwo, AT_CTR_USER);
    userFlg2 |= AT_CheckUserType(indexTwo, AT_PCUI2_USER);

    if (userFlg2 != VOS_TRUE) {
        return VOS_FALSE;
    }

    if (g_atClientTab[indexOne].userType != g_atClientTab[indexTwo].userType) {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 AT_IsConcurrentPorts(VOS_UINT8 indexOne, VOS_UINT8 indexTwo)
{
    VOS_UINT32 appUserFlg1;
    VOS_UINT32 appUserFlg2;

    /* 同一个通道不支持并发，由外层函数保证 */
    appUserFlg1 = AT_CheckAppUser(indexOne);
    appUserFlg2 = AT_CheckAppUser(indexTwo);

    /* 通道1是APP通道,通道2也是APP通道 */
    if (appUserFlg1 == VOS_TRUE) {
        if (appUserFlg2 == VOS_TRUE) {
            return VOS_TRUE;
        }
    }

    /* PCUI和CTRL口并发判断，仅供测试用 */
    if (AT_GetPcuiCtrlConcurrentFlag() == VOS_TRUE) {
        if (AT_CheckPcuiCtrlConcurrent(indexOne, indexTwo) == VOS_TRUE) {
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}


VOS_UINT32 AT_CheckAppUser(VOS_UINT8 indexNum)
{
    if (g_atClientTab[indexNum].userType != AT_APP_USER) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 AT_CheckHsUartUser(VOS_UINT8 indexNum)
{
#if (FEATURE_AT_HSUART == FEATURE_ON)
    if (g_atClientTab[indexNum].userType == AT_HSUART_USER) {
        return VOS_TRUE;
    }
#endif

    return VOS_FALSE;
}


VOS_UINT32 AT_CheckModemUser(VOS_UINT8 indexNum)
{
    if (g_atClientTab[indexNum].userType != AT_MODEM_USER) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_UINT32 AT_CheckUserType(VOS_UINT8 indexNum, AT_USER_TYPE userType)
{
    if (userType != g_atClientTab[indexNum].userType) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID AT_InitFcMap(VOS_VOID)
{
    VOS_UINT8 loop;

    /* 初始化g_fcIdMaptoFcPri */
    for (loop = 0; loop < FC_ID_BUTT; loop++) {
        g_fcIdMaptoFcPri[loop].used      = VOS_FALSE;
        g_fcIdMaptoFcPri[loop].fcPri     = FC_PRI_BUTT;
        g_fcIdMaptoFcPri[loop].rabIdMask = 0;
        g_fcIdMaptoFcPri[loop].modemId   = MODEM_ID_BUTT;
    }
}


VOS_UINT32 AT_SendPcuiDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 length)
{
    if (At_SendData(AT_CLIENT_ID_PCUI, g_atClientTab[AT_CLIENT_ID_PCUI].dataMode, virAddr, (VOS_UINT16)length) !=
        AT_SUCCESS) {
        return VOS_ERR;
    } else {
        return VOS_OK;
    }
}


VOS_UINT32 AT_SendCtrlDataFromOm(VOS_UINT8 *virAddr, VOS_UINT8 *phyAddr, VOS_UINT32 length)
{
    if (At_SendData(AT_CLIENT_ID_CTRL, g_atClientTab[AT_CLIENT_ID_CTRL].dataMode, virAddr, (VOS_UINT16)length) !=
        AT_SUCCESS) {
        return VOS_ERR;
    } else {
        return VOS_OK;
    }
}


CBTCPM_SEND_FUNC AT_QuerySndFunc(AT_PHY_PortUint32 phyPort)
{
    switch (phyPort) {
        case AT_UART_PORT:
            return AT_UartSendRawDataFromOm;

        case AT_PCUI_PORT:
            return AT_SendPcuiDataFromOm;

        case AT_CTRL_PORT:
            return AT_SendCtrlDataFromOm;

#if (FEATURE_AT_HSUART == FEATURE_ON)
        case AT_HSUART_PORT:
            return AT_HSUART_SendRawDataFromOm;
#endif

        default:
            AT_WARN_LOG("AT_QuerySndFunc: don't proc data of this port!");
            return VOS_NULL_PTR;
    }
}


TAF_UINT32 At_SendCmdMsg(TAF_UINT8 indexNum, TAF_UINT8 *data, TAF_UINT16 len, TAF_UINT8 type)
{
    AT_Msg           *msg           = TAF_NULL_PTR;
    AT_CmdMsgNumCtrl *msgNumCtrlCtx = VOS_NULL_PTR;
    VOS_UINT_PTR      tmpAddr;
    VOS_UINT32        length;
    VOS_ULONG         lockLevel;
    errno_t           memResult;
    ModemIdUint16 modemId;
    msgNumCtrlCtx = AT_GetMsgNumCtrlCtxAddr();

    if (data == VOS_NULL_PTR) {
        AT_WARN_LOG("At_SendCmdMsg :pData is null ptr!");
        return AT_FAILURE;
    }

    if (len == 0) {
        AT_WARN_LOG("At_SendCmdMsg ulLength = 0");
        return AT_FAILURE;
    }

    if (len > AT_COM_BUFF_LEN) {
        AT_WARN_LOG("At_SendCmdMsg ulLength > AT_COM_BUFF_LEN");
        return AT_FAILURE;
    }

    /* 增加自定义的ITEM，共4个字节 */
    AT_GetAtMsgStruMsgLength(len, &length);
    msg = (AT_Msg *)PS_ALLOC_MSG(WUEPS_PID_AT, length);
    if (msg == TAF_NULL_PTR) {
        AT_ERR_LOG("At_SendCmdMsg:ERROR:Alloc Msg");
        return AT_FAILURE;
    }

    TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, WUEPS_PID_AT, length);

    if (type == AT_NORMAL_TYPE_MSG) {
        if (msgNumCtrlCtx->msgCount > AT_MAX_MSG_NUM) {
            /*lint -save -e516 */
            /* 释放分配的内存空间 */
            PS_FREE_MSG(WUEPS_PID_AT, msg);
            /*lint -restore */

            return AT_FAILURE;
        }

        /*lint -e571*/
        VOS_SpinLockIntLock(&(msgNumCtrlCtx->spinLock), lockLevel);
        /*lint +e571*/

        msgNumCtrlCtx->msgCount++;

        VOS_SpinUnlockIntUnlock(&(msgNumCtrlCtx->spinLock), lockLevel);
    }

    if (type == AT_COMBIN_BLOCK_MSG) {
        msg->msgId = ID_AT_COMBIN_BLOCK_CMD;
    } else {
        msg->msgId = AT_GetCmdMsgID(indexNum);
    }

    msg->type     = type;     /* 类型 */
    msg->indexNum = indexNum; /* 索引 */
    msg->len      = len;      /* 长度 */

    modemId = MODEM_ID_0;
    if (AT_GetModemIdFromClient(indexNum, &modemId) != VOS_OK) {
        modemId = MODEM_ID_0;
    }

    msg->modemId = (VOS_UINT8)modemId;
    /* 版本信息 */
    msg->versionId    = 0xAA;
    msg->filterAtType = (VOS_UINT8)g_logPrivacyAtCmd;

    memset_s(msg->value, sizeof(msg->value), 0x00, sizeof(msg->value));

    AT_GetUserTypeFromIndex(indexNum, &msg->userType);

    /* 填写新消息内容 */
    tmpAddr   = (VOS_UINT_PTR)(msg->value);
    memResult = memcpy_s((VOS_UINT8 *)tmpAddr, len, data, len);
    TAF_MEM_CHK_RTN_VAL(memResult, len, len);

    /* 发送消息到AT_PID; */
    if (PS_SEND_MSG(WUEPS_PID_AT, msg) != 0) {
        AT_ERR_LOG("At_SendCmdMsg:ERROR:VOS_SendMsg");

        /* 由于消息发送失败时，会触发整机复位，故此处不做ulMsgCount--操作 */

        return AT_FAILURE;
    }
    return AT_SUCCESS;
}


VOS_UINT32 AT_IsApPort(VOS_UINT8 indexNum)
{
    VOS_UINT32 vcomUserFlg;
    VOS_UINT8 *systemAppConfig = VOS_NULL_PTR;

    if (g_atDebugInfo.unCheckApPortFlg == 0) {
        /* 初始化 */
        systemAppConfig = AT_GetSystemAppConfigAddr();
        vcomUserFlg     = AT_CheckAppUser(indexNum);

        if (*systemAppConfig == SYSTEM_APP_ANDROID) {
            /* 如果是手机形态，需要判断HSIC端口，MUX端口，VCOM端口 */
            if (vcomUserFlg == VOS_FALSE) {
                return VOS_FALSE;
            }
        } else {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_INT AT_ProcCCpuResetBefore(VOS_VOID)
{
    AT_Msg *msg = VOS_NULL_PTR;
    errno_t memResult;

    AT_PR_LOGI("enter, %u", VOS_GetSlice());

    /* 设置处于复位前的标志 */
    AT_SetResetFlag(VOS_TRUE);

    dms_reset_modem_status();

    /* 清除TAFAGENT所有的信号量 */
    TAF_AGENT_ClearAllSem();

    /* 构造消息 */
    /*lint -save -e516 */
    msg = (AT_Msg *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT, sizeof(AT_Msg));
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        AT_PR_LOGE("alloc msg fail, %u", VOS_GetSlice());
        return VOS_ERROR;
    }

    /* 初始化消息 */
    memResult = memset_s((VOS_CHAR *)msg + VOS_MSG_HEAD_LENGTH,
                         (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH), 0x00,
                         (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH),
                        (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, WUEPS_PID_AT, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);
    msg->type = ID_CCPU_AT_RESET_START_IND;

    msg->msgId = ID_AT_COMM_CCPU_RESET_START;

    /* 发消息 */
    if (PS_SEND_MSG(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_PR_LOGE("send msg fail, %u", VOS_GetSlice());
        return VOS_ERROR;
    }

    /* 等待回复信号量初始为锁状态，等待消息处理完后信号量解锁。 */
    if (VOS_SmP(AT_GetResetSem(), AT_RESET_TIMEOUT_LEN) != VOS_OK) {
        AT_PR_LOGE("VOS_SmP fail, %u", VOS_GetSlice());
        AT_DBG_LOCK_BINARY_SEM_FAIL_NUM(1);

        return VOS_ERROR;
    }

    /* 记录复位前的次数 */
    AT_DBG_SAVE_CCPU_RESET_BEFORE_NUM(1);

    AT_PR_LOGI("succ, %u", VOS_GetSlice());

    return VOS_OK;
}


VOS_INT AT_ProcCCpuResetAfter(VOS_VOID)
{
    AT_Msg *msg = VOS_NULL_PTR;
    errno_t memResult;

    AT_PR_LOGI("enter, %u", VOS_GetSlice());

    /* 构造消息 */
    /*lint -save -e516 */
    msg = (AT_Msg *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT, sizeof(AT_Msg));
    /*lint -restore */
    if (msg == VOS_NULL_PTR) {
        AT_PR_LOGE("alloc msg fail, %u", VOS_GetSlice());
        return VOS_ERROR;
    }

    /* 初始化消息 */
    memResult = memset_s((VOS_CHAR *)msg + VOS_MSG_HEAD_LENGTH,
                         (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH), 0x00,
                         (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));
    TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH),
                        (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));

    /* 填写消息头 */
    TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, WUEPS_PID_AT, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);
    msg->type = ID_CCPU_AT_RESET_END_IND;

    msg->msgId = ID_AT_COMM_CCPU_RESET_END;

    /* 发消息 */
    if (PS_SEND_MSG(WUEPS_PID_AT, msg) != VOS_OK) {
        AT_PR_LOGE("send msg fail, %u", VOS_GetSlice());
        return VOS_ERROR;
    }

    /* 记录复位后的次数 */
    AT_DBG_SAVE_CCPU_RESET_AFTER_NUM(1);

    AT_PR_LOGI("succ, %u", VOS_GetSlice());

    return VOS_OK;
}


VOS_INT AT_CCpuResetCallback(drv_reset_cb_moment_e param, VOS_INT iUserData)
{
    /* 复位前 */
    if (param == MDRV_RESET_CB_BEFORE) {
        return AT_ProcCCpuResetBefore();
    }
    /* 复位后 */
    else if (param == MDRV_RESET_CB_AFTER) {
        return AT_ProcCCpuResetAfter();
    } else {
        return VOS_ERROR;
    }
}


VOS_INT AT_HifiResetCallback(enum DRV_RESET_CALLCBFUN_MOMENT param, VOS_INT iUserData)
{
    AT_Msg *msg = VOS_NULL_PTR;
    errno_t memResult;

    /* 参数为0表示复位前调用 */
    if (param == DRV_RESET_CALLCBFUN_RESET_BEFORE) {
        AT_PR_LOGI("before reset enter, %u", VOS_GetSlice());
        /* 构造消息 */
        /*lint -save -e516 */
        msg = (AT_Msg *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT, sizeof(AT_Msg));
        /*lint -restore */
        if (msg == VOS_NULL_PTR) {
            AT_PR_LOGE("before reset alloc msg fail, %u", VOS_GetSlice());
            return VOS_ERROR;
        }

        /* 初始化消息 */
        memResult = memset_s((VOS_CHAR *)msg + VOS_MSG_HEAD_LENGTH, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH),
                             0x00, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));
        TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH),
                            (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));

        /* 填写消息头 */
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, WUEPS_PID_AT, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);
        msg->type = ID_HIFI_AT_RESET_START_IND;

        msg->msgId = ID_AT_COMM_HIFI_RESET_START;

        /* 发消息 */
        if (PS_SEND_MSG(WUEPS_PID_AT, msg) != VOS_OK) {
            AT_PR_LOGE("after reset alloc msg fail, %u", VOS_GetSlice());
            return VOS_ERROR;
        }

        return VOS_OK;
    }
    /* 复位后 */
    else if (param == DRV_RESET_CALLCBFUN_RESET_AFTER) {
        AT_PR_LOGI("after reset enter, %u", VOS_GetSlice());
        /* 构造消息 */
        /*lint -save -e516 */
        msg = (AT_Msg *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT, sizeof(AT_Msg));
        /*lint -restore */
        if (msg == VOS_NULL_PTR) {
            AT_PR_LOGE("after reset alloc msg fail, %u", VOS_GetSlice());
            return VOS_ERROR;
        }

        /* 初始化消息 */
        memResult = memset_s((VOS_CHAR *)msg + VOS_MSG_HEAD_LENGTH, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH),
                             0x00, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));
        TAF_MEM_CHK_RTN_VAL(memResult, (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH),
                            (VOS_SIZE_T)(sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH));

        /* 填写消息头 */
        TAF_CfgMsgHdr((MsgBlock *)msg, WUEPS_PID_AT, WUEPS_PID_AT, sizeof(AT_Msg) - VOS_MSG_HEAD_LENGTH);
        msg->type = ID_HIFI_AT_RESET_END_IND;

        msg->msgId = ID_AT_COMM_HIFI_RESET_END;

        /* 发消息 */
        if (PS_SEND_MSG(WUEPS_PID_AT, msg) != VOS_OK) {
            AT_PR_LOGE("after reset send msg fail, %u", VOS_GetSlice());
            return VOS_ERROR;
        }
        return VOS_OK;
    } else {
        return VOS_ERROR;
    }
}


VOS_VOID AT_ModemeEnableCB(VOS_UINT8 indexNum, VOS_UINT32 enable)
{
    /*
     * 设备默认处于生效状态，有数据就通过读回调接收，
     * 　 设备失效时，根据当前状态，通知PPP，如处于数传态，
     *    则通知AT去激活PDP.
     */
    if (enable == PS_FALSE) {
        if (g_atClientTab[indexNum].dataMode == AT_PPP_DATA_MODE) {
            PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_REL_PPP_REQ);

            /* 若原先开启了流控，则需停止流控 */
            if ((g_atClientTab[indexNum].modemStatus & IO_CTRL_CTS) == 0) {
                AT_StopFlowCtrl(indexNum);
            }

            /* 断开拨号 */
            if (TAF_PS_CallEnd(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               g_atClientTab[indexNum].cid) != VOS_OK) {
                AT_ERR_LOG("AT_ModemeEnableCB: TAF_PS_CallEnd failed in <AT_PPP_DATA_MODE>!");
                return;
            }
        } else if (g_atClientTab[indexNum].dataMode == AT_IP_DATA_MODE) {
            PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_REL_PPP_RAW_REQ);

            /* 若原先开启了流控，则需停止流控 */
            if ((g_atClientTab[indexNum].modemStatus & IO_CTRL_CTS) == 0) {
                AT_StopFlowCtrl(indexNum);
            }

            /* 断开拨号 */
            if (TAF_PS_CallEnd(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0,
                               g_atClientTab[indexNum].cid) != VOS_OK) {
                AT_ERR_LOG("AT_ModemeEnableCB: TAF_PS_CallEnd failed in <AT_IP_DATA_MODE>!");
                return;
            }
        } else {
            /* 空的else分支，避免PCLINT报错 */
        }

        /* 向PPP发送HDLC去使能操作 */
        PPP_RcvAtCtrlOperEvent(g_atClientTab[indexNum].pppId, PPP_AT_CTRL_HDLC_DISABLE);

        /* 停止定时器 */
        AT_StopRelTimer(indexNum, &g_atClientTab[indexNum].hTimer);

        /*
         * 管脚信号修改后，At_ModemRelInd函数只可能在USB被拔出的时候调用，
         * 为了达到Modem口always-on的目的，此时需要将该AT链路的状态迁入
         * 到正常的命令状态:
         */
        g_atClientTab[indexNum].mode          = AT_CMD_MODE;
        g_atClientTab[indexNum].indMode       = AT_IND_MODE;
        g_atClientTab[indexNum].dataMode      = AT_DATA_BUTT_MODE;
        g_atClientTab[indexNum].dataState     = AT_DATA_STOP_STATE;
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CURRENT_OPT_BUTT;
        g_parseContext[indexNum].clientStatus = AT_FW_CLIENT_STATUS_READY;
    }
}

