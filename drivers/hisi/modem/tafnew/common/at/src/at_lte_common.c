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

/* ***************************************************************************** */

/* PROJECT   : MSP_CPV700R001C01 */

/* SUBSYSTEM : AT */

/* MODULE    : LTE */

/* OWNER     : */

/* ***************************************************************************** */

/*lint -save -e537 -e958 -e713 -e718 -e746 -e740 -e516 -e830 -e533*/
#include "osm.h"
#include "gen_msg.h"
#include "msp_at.h"
#include "at_lte_common.h"
#include "ATCmdProc.h"
#include "mdrv_rfile.h"

#include <linux/syscalls.h>
#include "mdrv_om.h"
#include "mn_comm_api.h"
#include "securec.h"
/*lint -e767 原因:Log打印*/
#define THIS_FILE_ID MSP_FILE_ID_AT_LTE_COMMON_C
/*lint +e767 */
#define AT_SEND_DATA_BUFF_INDEX 2

/* 发送缓冲区 */

AT_SEND_DataBuffer g_atSendData = {
    {0},
};
VOS_UINT8 *g_atSndCodeAddr = &g_atSendData.buffer[AT_SEND_DATA_BUFF_INDEX];

AT_ERROR_CodeTable g_errCodeTable[] = {
    { APP_SUCCESS, AT_OK },
    { APP_FAILURE, AT_ERROR },
    { APP_ERR_CAPABILITY_ERROR, AT_CME_OPERATION_NOT_SUPPORTED },
    { APP_ERR_USIM_SIM_CARD_NOTEXIST, AT_CME_SIM_NOT_INSERTED },
    { APP_ERR_NEED_PIN1, AT_CME_SIM_PIN_REQUIRED },
    { APP_ERR_NEED_PUK1, AT_CME_SIM_PUK_REQUIRED },
    { APP_ERR_USIM_SIM_INVALIDATION, AT_CME_SIM_FAILURE },
    { APP_ERR_ID_INVALID, AT_CME_INVALID_INDEX },
    { APP_ERR_TIME_OUT, AT_CME_NETWORK_TIMEOUT },
    { APP_ERR_PARA_ERROR, AT_CME_INCORRECT_PARAMETERS },
    { APP_ERR_NULL_PTR, AT_CME_INCORRECT_PARAMETERS },
    { APP_ERR_NUM_VALUE_INVALID, AT_CME_INCORRECT_PARAMETERS },
    { APP_ERR_NUM_LEN_INVALID, AT_CME_INCORRECT_PARAMETERS },

    /* 拨号错误 */

    { APP_ERR_SM_APN_LEN_ILLEGAL, AT_CME_APN_LEN_ILLEGAL },
    { APP_ERR_SM_APN_SYNTACTICAL_ERROR, AT_CME_APN_SYNTACTICAL_ERROR },
    { APP_ERR_SM_SET_APN_BEFORE_SET_AUTH, AT_CME_SET_APN_BEFORE_SET_AUTH },
    { APP_ERR_SM_AUTH_TYPE_ILLEGAL, AT_CME_AUTH_TYPE_ILLEGAL },
    { APP_ERR_SM_USER_NAME_TOO_LONG, AT_CME_USER_NAME_TOO_LONG },
    { APP_ERR_SM_USER_PASSWORD_TOO_LONG, AT_CME_USER_PASSWORD_TOO_LONG },
    { APP_ERR_SM_ACCESS_NUM_TOO_LONG, AT_CME_ACCESS_NUM_TOO_LONG },
    { APP_ERR_SM_CALL_CID_IN_OPERATION, AT_CME_CALL_CID_IN_OPERATION },
    { APP_ERR_SM_BEARER_TYPE_NOT_DEFAULT, AT_CME_BEARER_TYPE_NOT_DEFAULT },
    { APP_ERR_SM_CALL_CID_INVALID, AT_CME_CALL_CID_INVALID },
    { APP_ERR_SM_CALL_CID_ACTIVE, AT_CME_CALL_CID_ACTIVE },
    { APP_ERR_SM_BEARER_TYPE_ILLEGAL, AT_CME_BEARER_TYPE_ILLEGAL },
    { APP_ERR_SM_MUST_EXIST_DEFAULT_TYPE_CID, AT_CME_MUST_EXIST_DEFAULT_TYPE_CID },
    { APP_ERR_SM_PDN_TYPE_ILLEGAL, AT_CME_PDN_TYPE_ILLEGAL },
    { APP_ERR_SM_IPV4_ADDR_ALLOC_TYPE_ILLEGAL, AT_CME_IPV4_ADDR_ALLOC_TYPE_ILLEGAL },
    { APP_ERR_SM_LINK_CID_INVALID, AT_CME_LINK_CID_INVALID },

    /* NV ERROR CODE */

    { ERR_MSP_NV_NOT_SUPPORT_ID, AT_DEVICE_NV_NOT_SUPPORT_ID },
    { ERR_MSP_NV_ERROR_READ, AT_DEVICE_NV_READ_FAILURE },
    { ERR_MSP_NV_NOT_SUPPORT_LENTH, AT_DEVICE_NV_WRITE_FAIL_OVERLEN },
    { ERR_MSP_NV_BAD_BLOCK, AT_DEVICE_NV_WRITE_FAIL_BADFLASH },
    { ERR_MSP_NV_ERROR_WRITE, AT_DEVICE_NV_WRITE_FAIL_UNKNOWN },
    /* 装备 ERROR CODE */

    { ERR_MSP_SUCCESS, AT_OK },
    { ERR_MSP_FAILURE, AT_ERROR },
    { ERR_MSP_NOT_EXIST, AT_DEVICE_NOT_SUPPORT },
    { ERR_MSP_UNKNOWN, AT_DEVICE_ERR_UNKNOWN },
    { ERR_MSP_INVALID_PARAMETER, AT_DEVICE_INVALID_PARAMETERS },
    { ERR_MSP_VCTCXO_OVER_HIGH, AT_DEVICE_VCTCXO_OVER_HIGH },
    { ERR_MSP_UE_MODE_ERR, AT_DEVICE_UE_MODE_ERR },
    { ERR_MSP_NOT_SET_CHAN, AT_DEVICE_NOT_SET_CHAN },
    { ERR_MSP_OPEN_RFTX_ERROR, AT_DEVICE_OPEN_UL_CHAN_ERROR },
    { ERR_MSP_OPEN_RFRX_ERROR, AT_DEVICE_OPEN_DL_CHAN_ERROR },
    { ERR_MSP_OPEN_CHAN_ERROR, AT_DEVICE_OPEN_CHAN_ERROR },
    { ERR_MSP_CLOSE_CHAN_ERROR, AT_DEVICE_CLOSE_CHAN_ERROR },
    { ERR_MSP_NOT_SUPPORT, AT_DEVICE_OPERATION_NOT_SUPPORT },
    { ERR_MSP_INVALID_OP, AT_DEVICE_INVALID_OP },
    { ERR_MSP_ENTER_BAND_ERROR, AT_DEVICE_CHAN_BAND_INVALID },
    { ERR_MSP_SET_CHAN_INFOR_ERROR, AT_DEVICE_SET_CHAN_INFO_FAILURE },
    { ERR_MSP_FRE_CHANI_NOTMATCH, AT_DEVICE_CHAN_BAND_CHAN_NOT_MAP },
    { ERR_MSP_SET_TX_POWER_FAILUE, AT_DEVICE_SET_TX_POWER_FAILURE },
    { ERR_MSP_SET_PA_LEVEL_FAILUE, AT_DEVICE_SET_PA_LEVEL_FAILURE },
    { ERR_MSP_NOT_SET_CURRENT_CHAN, AT_DEVICE_NOT_SET_CURRENT_CHAN },
    { ERR_MSP_NOT_ABLETO_RD_APC, AT_DEVICE_CUR_APC_UNAVAILABLE },
    { ERR_MSP_SET_APC_ERROR, AT_DEVICE_SET_APC_ERR },
    { ERR_MSP_RD_APC_ERROR, AT_DEVICE_RD_APC_ERR },
    { ERR_MSP_SET_LINA_ERROR, AT_DEVICE_SET_LNA_ERR },
    { ERR_MSP_NOT_OPEN_RXRX, AT_DEVICE_NOT_OPEN_DL_CHAN },
    { ERR_MSP_NOT_OPEN_RXTX, AT_DEVICE_NOT_OPEN_UL_CHAN },
    { ERR_MSP_NO_SIGNAL, AT_DEVICE_NO_SIGNAL },
    { ERR_MSP_PHYNUM_LENGTH_ERROR, AT_DEVICE_PHYNUM_LEN_ERR },
    { ERR_MSP_PHYNUM_INVALID, AT_DEVICE_PHYNUM_INVALID },
    { ERR_MSP_PHYNUM_TYPE_ERROR, AT_DEVICE_PHYNUM_TYPE_ERR },
    { ERR_MSP_PLATINFOR_NOTABLE_TORD, AT_DEVICE_PLATFORM_INFO_UNAVAILABLE },
    { ERR_MSP_WRT_NOT_UNLOCK, AT_DEVICE_DATA_LOCK },
    { ERR_MSP_CALLNUM_OVER20, AT_DEVICE_PLMN_OVER_20 },
    { ERR_MSP_MNC_ENTER_OVERFLOW, AT_DEVICE_MNC_NUM_INVALID },
    { ERR_MSP_RD_VER_FAILUE, AT_DEVICE_GET_VER_ERR },
    { ERR_MSP_OPERTION_ERROR, AT_DEVICE_PORTLOCK_ERR },
    { ERR_MSP_PWD_ERR, AT_DEVICE_PWD_ERR },
    { ERR_MSP_TIME_OUT, AT_DEVICE_TIMEOUT_ERR },
    { ERR_MSP_NO_MORE_MEMORY, AT_DEVICE_NOT_ENOUGH_MEMORY },
    { ERR_MSP_SIMM_LOCK, AT_DEVICE_SIMM_LOCK },
    { ERR_MSP_CLOSE_RFTX_ERROR, AT_DEVICE_CLOSE_UL_CHAN_FAILURE },
    { ERR_MSP_CLOSE_RFRX_ERROR, AT_DEVICE_CLOSE_DL_CHAN_FAILURE },
    { ERR_MSP_NV_DATA_INVALID, AT_DEVICE_NV_DATA_INVALID },

};

#define FTM_DUMP_FILE_NAME_LENGTH 50
#define FTM_DUMP_FILE_MAX_NUM 4
#define FTM_DUMP_SIZE_12K 0x3000

#define FTM_DUMP_LOG_PATH MODEM_LOG_ROOT "/DrvLog/Ftm/"
#define FTM_DUMP_HEAD "ftm_"

/*lint -e959 */
/*lint -e958 */

struct linux_dirent {
    unsigned long  ino;
    unsigned long  off;
    unsigned short reclen;
    char           name[1];
};
/*lint +e959 */
/*lint +e958 */

/* ***************************************************************************** */

/* 函数名称: atChgErrorCode */

/* 功能描述: 把其他模块返回的错误码转换成AT的错误码 */

/*  */

/* 参数说明: */

/*   usTafErrorCode [in] 错误码 */

/*  */

/* 返 回 值: */

/*    返回AT的错误码 */

/*  */

/* 调用要求: TODO: ... */

/* 调用举例: TODO: ... */


/* ***************************************************************************** */

static VOS_UINT32 ChgErrorCode(VOS_UINT32 tafErrorCode)
{
    VOS_UINT32 rtn    = AT_CME_UNKNOWN;
    VOS_UINT32 i      = 0;
    VOS_UINT32 errNum = sizeof(g_errCodeTable) / sizeof(AT_ERROR_CodeTable);

    for (i = 0; i < errNum; i++) {
        if (tafErrorCode == g_errCodeTable[i].srcError) {
            rtn = g_errCodeTable[i].atError;

            return rtn;
        }
    }

    return AT_ERROR;
}

/* ***************************************************************************** */

/* 函数名称: CmdErrProc */

/* 功能描述: 错误码转换及AT回复上报 */

/*  */

/* 参数说明: */

/*   ucClientId [in] 客户端ID */

/*   ulErrCode [in]  错误码 */

/*   usBufLen [in]   回复信息 */

/*   pucBuf [in]     回复信息长度 */

/*  */

/* 返 回 值: */

/*    无 */

/*  */

/* 调用要求: TODO: ... */

/* 调用举例: TODO: ... */


/* ***************************************************************************** */

VOS_VOID CmdErrProc(VOS_UINT8 clientId, VOS_UINT32 errCode, VOS_UINT16 bufLen, VOS_UINT8 *buf)
{
    errno_t    memResult;
    VOS_UINT32 result       = 0;
    g_atSendDataBuff.bufLen = bufLen;
    if ((buf != VOS_NULL_PTR) && (bufLen != 0)) {
        /* 改成下面的以匹配函数 At_FormatResultData */
        memResult = memcpy_s(g_atSndCodeAddress, AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET,
                             buf, bufLen);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET, bufLen);
    }

    if (errCode != ERR_MSP_SUCCESS) {
        result = ChgErrorCode(errCode);

        At_FormatResultData(clientId, result);
    } else {
        At_FormatResultData(clientId, AT_OK);
    }

    return;
}

/* ***************************************************************************** */

/* 函数名称: atSendFtmDataMsg */

/* 功能描述: AT模块给FTM 模块发送消息 */

/*  */

/* 参数说明: */

/*   TaskId [in] 接收PID */

/*   MsgId  [in] 消息ID */

/*   ulClientId [in] 端口号 */

/*   pData  [in] 数据起始 */

/*   uLen   [in] 数据长度 */

/* 返 回 值: */

/*    ERR_MSP_SUCCESS成功 */

/*    非ERR_MSP_SUCCESS失败 */


/* ***************************************************************************** */

VOS_UINT32 atSendFtmDataMsg(VOS_UINT32 taskId, VOS_UINT32 msgId, VOS_UINT32 clientId, const VOS_UINT8 *data,
                            VOS_UINT32 uLen)
{
    errno_t        memResult;
    VOS_UINT32     u32Ret   = ERR_MSP_UNKNOWN;
    AT_FW_DataMsg *msgBlock = NULL;

    msgBlock = (AT_FW_DataMsg *)VOS_AllocMsg(WUEPS_PID_AT, (sizeof(AT_FW_DataMsg) + uLen - VOS_MSG_HEAD_LENGTH));

    if (msgBlock != VOS_NULL_PTR) {
        TAF_CfgMsgHdr((MsgBlock *)msgBlock, WUEPS_PID_AT, AT_GetDestPid((MN_CLIENT_ID_T)clientId, taskId),
                      sizeof(AT_FW_DataMsg) + uLen - VOS_MSG_HEAD_LENGTH);

        msgBlock->msgId    = msgId;
        msgBlock->clientId = clientId;
        msgBlock->len      = uLen;

        if ((data != VOS_NULL_PTR) && (uLen != 0)) {
            memResult = memcpy_s(msgBlock->context, uLen, data, uLen);
            TAF_MEM_CHK_RTN_VAL(memResult, uLen, uLen);
        }

        u32Ret = VOS_SendMsg(VOS_GET_SENDER_ID(msgBlock), msgBlock);

        if (u32Ret == VOS_OK) {
            u32Ret = ERR_MSP_SUCCESS;
        }
    }

    return u32Ret;
}


VOS_UINT32 atSendL4aDataMsg(MN_CLIENT_ID_T clientId, VOS_UINT32 taskId, VOS_UINT32 msgId, const VOS_UINT8 *data,
                            VOS_UINT32 uLen)
{
    errno_t       memResult;
    VOS_UINT32    u32Ret   = 0;
    L4A_COMM_Req *msgBlock = VOS_NULL_PTR;

    msgBlock = (L4A_COMM_Req *)VOS_AllocMsg(WUEPS_PID_AT, (uLen - VOS_MSG_HEAD_LENGTH));

    if (msgBlock != VOS_NULL_PTR) {
        memResult = memcpy_s(msgBlock, uLen, data, uLen);
        TAF_MEM_CHK_RTN_VAL(memResult, uLen, uLen);
        VOS_SET_SENDER_ID(msgBlock, WUEPS_PID_AT);
        VOS_SET_RECEIVER_ID(msgBlock, AT_GetDestPid(clientId, taskId));
        VOS_SET_MSG_LEN(msgBlock, (uLen - VOS_MSG_HEAD_LENGTH));
        msgBlock->msgId = msgId;

        u32Ret = VOS_SendMsg(VOS_GET_SENDER_ID(msgBlock), msgBlock);
    }

    return u32Ret;
}

/* 定义了LTE与TDS私有AT命令 */

AT_ParCmdElement g_atTlCmdTbl[] = {
    { AT_CMD_LWCLASH,
      VOS_NULL_PTR, AT_SET_PARA_TIME, AT_QryLwclashPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^LWCLASH", VOS_NULL_PTR },

    { AT_CMD_RADVER,
      AT_SetRadverPara, AT_SET_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^RADVER", (VOS_UINT8 *)"(0-3),(4-16)" },

    { AT_CMD_LCACELL,
      VOS_NULL_PTR, AT_NOT_SET_TIME, AT_QryLcacellPara, AT_QRY_PARA_TIME, VOS_NULL_PTR, AT_NOT_SET_TIME,
      VOS_NULL_PTR, AT_NOT_SET_TIME,
      AT_CME_INCORRECT_PARAMETERS, CMD_TBL_NO_LIMITED,
      (VOS_UINT8 *)"^LCACELL", VOS_NULL_PTR }
};

/*lint -restore*/


VOS_UINT32 At_RegisterTLCmdTable(VOS_VOID)
{
    return AT_RegisterCmdTable(g_atTlCmdTbl, sizeof(g_atTlCmdTbl) / sizeof(g_atTlCmdTbl[0]));
}
