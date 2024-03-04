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

#include "AtCmdCssProc.h"
#include "ATCmdProc.h"
#include "ps_common_def.h"
#include "AtEventReport.h"
#include "securec.h"


#define THIS_FILE_ID PS_FILE_ID_AT_CMD_CSS_PROC_C

/* AT与CSS模块间消息处理函数指针 */
const AT_CSS_MsgProFunc g_atCssMsgTab[] = {
    /* 消息ID */ /* 消息处理函数 */
    { ID_CSS_AT_MCC_INFO_SET_CNF, AT_RcvCssMccInfoSetCnf },
    { ID_CSS_AT_MCC_VERSION_INFO_CNF, AT_RcvCssMccVersionQryCnf },
    { ID_CSS_AT_QUERY_MCC_INFO_NOTIFY, AT_RcvCssMccNotify },

    { ID_CSS_AT_BLACK_CELL_LIST_SET_CNF, AT_RcvCssBlackCellListSetCnf },
    { ID_CSS_AT_BLACK_CELL_LIST_QUERY_CNF, AT_RcvCssBlackCellListQryCnf },
    { ID_CSS_AT_BLACK_CELL_MCC_NOTIFY, AT_RcvCssBlackCellMccNotify },

    { ID_CSS_AT_LINE_INDEX_LIST_SET_CNF, AT_RcvCssLineIndexListSetCnf },
    { ID_CSS_AT_LINE_INDEX_LIST_QUERY_CNF, AT_RcvCssLineIndexListQryCnf },
    { ID_CSS_AT_LINE_DETAIL_SET_CNF, AT_RcvCssLineDetailSetCnf },
    { ID_CSS_AT_LINE_PLMN_NOTIFY, AT_RcvCssLinePlmnNotify },
    { ID_CSS_AT_LINE_INDEX_NOTIFY, AT_RcvCssLineIndexNotify },

};


VOS_VOID AT_ProcCssMsg(struct MsgCB *msg)
{
    CSS_AT_InterfaceMsg *msgTemp = VOS_NULL_PTR;
    VOS_UINT32           i;
    VOS_UINT32           msgCnt;
    VOS_UINT32           msgId;
    VOS_UINT32           rst;

    /* 从g_astAtProcMsgFromImsaTab中获取消息个数 */
    msgCnt  = sizeof(g_atCssMsgTab) / sizeof(AT_CSS_MsgProFunc);
    msgTemp = (CSS_AT_InterfaceMsg *)msg;

    /* 从消息包中获取MSG ID */
    msgId = msgTemp->msgData.msgId;

    /* g_astAtProcMsgFromCssTab查表，进行消息分发 */
    for (i = 0; i < msgCnt; i++) {
        if (g_atCssMsgTab[i].msgId == msgId) {
            rst = g_atCssMsgTab[i].procMsgFunc((struct MsgCB *)msgTemp);

            if (rst == VOS_ERR) {
                AT_ERR_LOG("AT_ProcCssMsg: Msg Proc Err!");
            }

            return;
        }
    }

    /* 没有找到匹配的消息 */
    if (msgCnt == i) {
        AT_ERR_LOG("AT_ProcCssMsg: Msg Id is invalid!");
    }

    return;
}


VOS_UINT32 AT_RcvCssMccInfoSetCnf(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_MccInfoSetCnf *mccInfoSetCnf = VOS_NULL_PTR;
    VOS_UINT8             indexNum;
    VOS_UINT32            result;
    VOS_UINT32            retVal;

    /* 初始化消息变量 */
    indexNum      = 0;
    retVal        = VOS_ERR;
    result        = AT_ERROR;
    mccInfoSetCnf = (CSS_AT_MccInfoSetCnf *)msg;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(mccInfoSetCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssMccInfoSetCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssMccInfoSetCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_MCCFREQ_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_MCCFREQ_SET) {
        AT_WARN_LOG("AT_RcvCssMccInfoSetCnf: WARNING:Not AT_CMD_MCCFREQ_SET!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    /* 判断查询操作是否成功 */
    if (mccInfoSetCnf->result == VOS_OK) {
        result = AT_OK;
        retVal = VOS_OK;
    }

    g_atSendDataBuff.bufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(indexNum, result);

    return retVal;
}


VOS_UINT32 AT_RcvCssMccVersionQryCnf(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_MccVersionInfoCnf *mccVersionInfoCnf                   = VOS_NULL_PTR;
    VOS_UINT8                 versionId[MCC_INFO_VERSION_LEN + 1] = {0};
    VOS_UINT32                result;
    errno_t                   memResult;
    VOS_UINT8                 indexNum;

    /* 初始化消息变量 */
    indexNum          = 0;
    mccVersionInfoCnf = (CSS_AT_MccVersionInfoCnf *)msg;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(mccVersionInfoCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssMccVersionQryCnf: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssMccVersionQryCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_MCCFREQ_QRY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_MCCFREQ_QRY) {
        AT_WARN_LOG("AT_RcvCssMccVersionQryCnf: WARNING:Not AT_CMD_MCCFREQ_QRY!");
        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    /* 构造Version String */
    memResult = memcpy_s(versionId, sizeof(versionId), mccVersionInfoCnf->versionId, MCC_INFO_VERSION_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(versionId), MCC_INFO_VERSION_LEN);

    /* 判断查询操作是否成功 */
    g_atSendDataBuff.bufLen =
        (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress, "%s: %s",
                               g_parseContext[indexNum].cmdElement->cmdName, (VOS_CHAR *)versionId);

    result = AT_OK;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssMccNotify(struct MsgCB *msg)
{
    /* 定义局部变量 */
    VOS_UINT8                  indexNum;
    VOS_UINT8                  i;
    VOS_UINT8                  mccStr[AT_CSS_MAX_MCC_ID_NUM * AT_MCC_PLUS_COMMA_LENGTH] = {0};
    VOS_UINT8                  versionId[MCC_INFO_VERSION_LEN + 1]                      = {0};
    CSS_AT_QueryMccInfoNotify *cssMccNty                                                = VOS_NULL_PTR;
    errno_t                    memResult;

    /* 初始化消息变量 */
    indexNum  = 0;
    cssMccNty = (CSS_AT_QueryMccInfoNotify *)msg;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssMccNty->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssMccNotify: WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    /* MCC个数不对 */
    if ((cssMccNty->mccNum == 0) || (cssMccNty->mccNum > AT_CSS_MAX_MCC_ID_NUM)) {
        AT_WARN_LOG("AT_RcvCssMccNotify: WARNING:INVALID MCC NUM!");
        return VOS_ERR;
    }

    /* 构造上报给Ril的MCC字符串 */
    for (i = 0; i < cssMccNty->mccNum; i++) {
        mccStr[AT_MCC_PLUS_COMMA_LENGTH * i]     = (cssMccNty->mccId[i].mcc[0] & 0x0f) + '0';
        mccStr[AT_MCC_PLUS_COMMA_LENGTH * i + 1] = ((cssMccNty->mccId[i].mcc[0] & 0xf0) >> 4) + '0';
        mccStr[AT_MCC_PLUS_COMMA_LENGTH * i + 2] = (cssMccNty->mccId[i].mcc[1] & 0x0f) + '0';
        mccStr[AT_MCC_PLUS_COMMA_LENGTH * i + 3] = ',';
    }

    /* 字符串结束符 */
    mccStr[(VOS_UINT8)(cssMccNty->mccNum) * AT_MCC_PLUS_COMMA_LENGTH - 1] = 0;

    /* 构造Version String */
    memResult = memcpy_s(versionId, sizeof(versionId), cssMccNty->versionId, MCC_INFO_VERSION_LEN);
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(versionId), MCC_INFO_VERSION_LEN);

    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s%s %s,%s%s", g_atCrLf,
                                                     g_atStringTab[AT_STRING_MCC].text, (VOS_CHAR *)versionId,
                                                     (VOS_CHAR *)mccStr, g_atCrLf);

    At_SendResultData(indexNum, g_atSndCodeAddress, g_atSendDataBuff.bufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssBlackCellListSetCnf(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_BlackCellListSetCnf *cssSetCnf = VOS_NULL_PTR;
    VOS_UINT32                  result;
    VOS_UINT8                   indexNum;

    /* 初始化消息变量 */
    cssSetCnf = (CSS_AT_BlackCellListSetCnf *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssSetCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssBlackCellListSetCnf: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssBlackCellListSetCnf: WARNING:AT_BROADCAST_INDEX!");

        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CLOUDBLACKLIST_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_BLACKCELLLIST_SET) {
        AT_WARN_LOG("AT_RcvCssBlackCellListSetCnf: WARNING:Not AT_CMD_BLACKCELLLIST_SET!");

        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    result = AT_ERROR;

    if (cssSetCnf->result == VOS_OK) {
        result = AT_OK;
    }

    g_atSendDataBuff.bufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssBlackCellListQryCnf(struct MsgCB *msg)
{
    CSS_AT_BlackCellListQueryCnf *cssQryCnf = VOS_NULL_PTR;
    VOS_UINT8                     versionId[AT_CSS_BLACK_CELL_LIST_VERSION_LEN + 1];
    VOS_UINT32                    mcc;
    VOS_UINT32                    loop;
    errno_t                       memResult;
    VOS_UINT16                    length;
    VOS_UINT8                     indexNum;

    /* 初始化消息变量 */
    cssQryCnf = (CSS_AT_BlackCellListQueryCnf *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssQryCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssBlackCellListQryCnf: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssBlackCellListQryCnf: WARNING:AT_BROADCAST_INDEX!");

        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CLOUDBLACKLIST_QRY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_BLACKCELLLIST_QRY) {
        AT_WARN_LOG("AT_RcvCssBlackCellListQryCnf: WARNING:Not AT_CMD_BLACKCELLLIST_QRY!");

        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    /* MCC个数不对 */
    if (cssQryCnf->mccInfo.mccNum > AT_CSS_MAX_MCC_ID_NUM) {
        AT_WARN_LOG("AT_RcvCssBlackCellListQryCnf: WARNING:INVALID MCC NUM!");

        At_FormatResultData(indexNum, AT_ERROR);

        return VOS_ERR;
    }

    /* 构造上报给Ril的Version */
    memset_s(versionId, sizeof(versionId), 0, sizeof(versionId));
    memResult = memcpy_s(versionId, sizeof(versionId), cssQryCnf->versionId, sizeof(cssQryCnf->versionId));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(versionId), sizeof(cssQryCnf->versionId));

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: \"%s\"",
                                     g_parseContext[indexNum].cmdElement->cmdName, versionId);

    /* 构造上报给Ril的MCC字符串 */
    for (loop = 0; loop < cssQryCnf->mccInfo.mccNum; loop++) {
        mcc = 0;

        AT_ConvertNasMccToBcdType(cssQryCnf->mccInfo.mcc[loop], &mcc);

        length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, ",%x%x%x", (mcc & 0x0f00) >> 8,
                                         (mcc & 0xf0) >> 4, (mcc & 0x0f));
    }

    g_atSendDataBuff.bufLen = length;

    At_FormatResultData(indexNum, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssBlackCellMccNotify(struct MsgCB *msg)
{
    errno_t memResult;
    /* 定义局部变量 */
    CSS_AT_BlackCellMccNotify *cssNty = VOS_NULL_PTR;
    VOS_UINT8                  versionId[AT_CSS_BLACK_CELL_LIST_VERSION_LEN + 1];
    VOS_UINT32                 mcc;
    VOS_UINT32                 loop;
    VOS_UINT16                 length;
    VOS_UINT8                  indexNum;

    /* 初始化消息变量 */
    cssNty = (CSS_AT_BlackCellMccNotify *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssNty->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssBlackCellMccNotify: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    /* MCC个数不对 */
    if ((cssNty->mccInfo.mccNum == 0) || (cssNty->mccInfo.mccNum > AT_CSS_MAX_MCC_ID_NUM)) {
        AT_WARN_LOG("AT_RcvCssBlackCellMccNotify: WARNING:INVALID MCC NUM!");

        return VOS_ERR;
    }

    /* 构造上报给Ril的Version */
    memset_s(versionId, sizeof(versionId), 0, sizeof(versionId));
    memResult = memcpy_s(versionId, sizeof(versionId), cssNty->versionId, sizeof(cssNty->versionId));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(versionId), sizeof(cssNty->versionId));

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s%s\"%s\"", g_atCrLf,
                                     g_atStringTab[AT_STRING_BLACK_CELL_MCC].text, versionId);

    /* 构造上报给Ril的MCC字符串 */
    for (loop = 0; loop < cssNty->mccInfo.mccNum; loop++) {
        mcc = 0;

        AT_ConvertNasMccToBcdType(cssNty->mccInfo.mcc[loop], &mcc);

        length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, ",%x%x%x", (mcc & 0x0f00) >> 8,
                                         (mcc & 0xf0) >> 4, (mcc & 0x0f));
    }

    length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                     (TAF_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

    g_atSendDataBuff.bufLen = length;

    At_SendResultData(indexNum, g_atSndCodeAddress, g_atSendDataBuff.bufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssLineIndexListSetCnf(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_LineIndexListSetCnf *cssSetCnf = VOS_NULL_PTR;
    VOS_UINT32                  result;
    VOS_UINT8                   indexNum;

    /* 初始化消息变量 */
    cssSetCnf = (CSS_AT_LineIndexListSetCnf *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssSetCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssLineIndexListSetCnf: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssLineIndexListSetCnf: WARNING:AT_BROADCAST_INDEX!");

        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_LINEINDEXLIST_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_LINEINDEXLIST_SET) {
        AT_WARN_LOG("AT_RcvCssLineIndexListSetCnf: WARNING:Not AT_CMD_LINEINDEXLIST_SET!");

        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    result = AT_ERROR;

    if (cssSetCnf->result == VOS_OK) {
        result = AT_OK;
    }

    g_atSendDataBuff.bufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssLineIndexListQryCnf(struct MsgCB *msg)
{
    CSS_AT_LineIndexListQueryCnf *cssQryCnf = VOS_NULL_PTR;
    VOS_UINT8                     versionId[AT_CSS_CLOUD_LINE_VERSION_LEN + 1];
    VOS_UINT32                    mcc;
    VOS_UINT32                    mnc;
    errno_t                       memResult;
    VOS_UINT16                    length;
    VOS_UINT8                     indexNum;

    /* 初始化消息变量 */
    cssQryCnf = (CSS_AT_LineIndexListQueryCnf *)msg;

    indexNum = 0;
    mcc      = 0xFFFFFFFF;
    mnc      = 0xFFFFFFFF;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssQryCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssLineIndexListQryCnf: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssLineIndexListQryCnf: WARNING:AT_BROADCAST_INDEX!");

        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_CLOUDBLACKLIST_QRY */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_LINEINDEXLIST_QRY) {
        AT_WARN_LOG("AT_RcvCssLineIndexListQryCnf: WARNING:Not AT_CMD_LINEINDEXLIST_QRY!");

        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    length = 0;
    /* 构造result  */
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                     (TAF_CHAR *)g_atSndCodeAddress + length, "%s: %d",
                                     g_parseContext[indexNum].cmdElement->cmdName, cssQryCnf->result);

    if (cssQryCnf->result == VOS_TRUE) {
        AT_ConvertNasMccToBcdType(cssQryCnf->rplmnInfo.mcc, &mcc);
        AT_ConvertNasMncToBcdType(cssQryCnf->rplmnInfo.mnc, &mnc);

        /* 构造上报给Ril的RPLMN */
        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, ",\"%x%x%x", (mcc & 0x0f00) >> 8,
                                         (mcc & 0xf0) >> 4, (mcc & 0x0f));

        if ((mnc & 0x0f00) == 0x0f00) {
            length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                             (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x\",", (mnc & 0xf0) >> 4,
                                             (mnc & 0x0f));
        } else {
            length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                             (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x\",", (mnc & 0x0f00) >> 8,
                                             (mnc & 0xf0) >> 4, (mnc & 0x0f));
        }

        /* 构造上报给Ril的Version */
        memset_s(versionId, sizeof(versionId), 0, sizeof(versionId));
        memResult = memcpy_s(versionId, sizeof(versionId), cssQryCnf->versionId, sizeof(cssQryCnf->versionId));
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(versionId), sizeof(cssQryCnf->versionId));

        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                         (VOS_CHAR *)g_atSndCodeAddress + length, "\"%s\"", versionId);
    }

    g_atSendDataBuff.bufLen = length;

    At_FormatResultData(indexNum, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssLineDetailSetCnf(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_LineDetailSetCnf *cssSetCnf = VOS_NULL_PTR;
    VOS_UINT32               result;
    VOS_UINT8                indexNum;

    /* 初始化消息变量 */
    cssSetCnf = (CSS_AT_LineDetailSetCnf *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssSetCnf->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssLineIndexListSetCnf: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(indexNum)) {
        AT_WARN_LOG("AT_RcvCssLineIndexListSetCnf: WARNING:AT_BROADCAST_INDEX!");

        return VOS_ERR;
    }

    /* 判断当前操作类型是否为AT_CMD_LINEDETAIL_SET */
    if (g_atClientTab[indexNum].cmdCurrentOpt != AT_CMD_LINEDETAIL_SET) {
        AT_WARN_LOG("AT_RcvCssLineIndexListSetCnf: WARNING:Not AT_CMD_LINEDETAIL_SET!");

        return VOS_ERR;
    }

    /* 复位AT状态 */
    AT_STOP_TIMER_CMD_READY(indexNum);

    result = AT_ERROR;

    if (cssSetCnf->result == VOS_OK) {
        result = AT_OK;
    }

    g_atSendDataBuff.bufLen = 0;

    /* 调用At_FormatResultData发送命令结果 */
    At_FormatResultData(indexNum, result);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssLinePlmnNotify(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_LinePlmnNotify *cssNty = VOS_NULL_PTR;
    VOS_UINT32             mcc    = 0xFFFFFFFF;
    VOS_UINT32             mnc    = 0xFFFFFFFF;
    VOS_UINT16             length;
    VOS_UINT8              indexNum;

    /* 初始化消息变量 */
    cssNty = (CSS_AT_LinePlmnNotify *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssNty->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssLinePlmnNotify: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    AT_ConvertNasMccToBcdType(cssNty->rplmnInfo.mcc, &mcc);
    AT_ConvertNasMncToBcdType(cssNty->rplmnInfo.mnc, &mnc);

    length = 0;

    /* 构造上报给Ril的RPLMN */
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                     (TAF_CHAR *)g_atSndCodeAddress + length, "%s%s\"%x%x%x", g_atCrLf,
                                     g_atStringTab[AT_STRING_LINE_RPLMN].text, (mcc & 0x0f00) >> 8, (mcc & 0xf0) >> 4,
                                     (mcc & 0x0f));

    if ((mnc & 0x0f00) == 0x0f00) {
        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x\"", (mnc & 0xf0) >> 4,
                                         (mnc & 0x0f));
    } else {
        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x\"", (mnc & 0x0f00) >> 8,
                                         (mnc & 0xf0) >> 4, (mnc & 0x0f));
    }

    length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                     (TAF_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

    g_atSendDataBuff.bufLen = length;

    At_SendResultData(indexNum, g_atSndCodeAddress, g_atSendDataBuff.bufLen);

    return VOS_OK;
}


VOS_UINT32 AT_RcvCssLineIndexNotify(struct MsgCB *msg)
{
    /* 定义局部变量 */
    CSS_AT_LineIndexNotify *cssNty = VOS_NULL_PTR;
    VOS_UINT32              mcc    = 0xFFFFFFFF;
    VOS_UINT32              mnc    = 0xFFFFFFFF;
    VOS_UINT16              length;
    VOS_UINT8               indexNum;
    VOS_UINT16              loop;

    /* 初始化消息变量 */
    cssNty = (CSS_AT_LineIndexNotify *)msg;

    indexNum = 0;

    /* 通过ClientId获取ucIndex */
    if (At_ClientIdToUserId(cssNty->clientId, &indexNum) == AT_FAILURE) {
        AT_WARN_LOG("AT_RcvCssLineIndexNotify: WARNING:AT INDEX NOT FOUND!");

        return VOS_ERR;
    }

    if (cssNty->lineNum == 0) {
        AT_WARN_LOG("AT_RcvCssLineIndexNotify: WARNING:line num is 0!");

        return VOS_ERR;
    }

    AT_ConvertNasMccToBcdType(cssNty->plmnId.mcc, &mcc);
    AT_ConvertNasMncToBcdType(cssNty->plmnId.mnc, &mnc);

    length = 0;

    /* 构造上报给Ril的RPLMN */
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                     (TAF_CHAR *)g_atSndCodeAddress + length, "%s%s\"%x%x%x", g_atCrLf,
                                     g_atStringTab[AT_STRING_LINE_INDEX].text, (mcc & 0x0f00) >> 8, (mcc & 0xf0) >> 4,
                                     (mcc & 0x0f));

    if ((mnc & 0x0f00) == 0x0f00) {
        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x\"", (mnc & 0xf0) >> 4,
                                         (mnc & 0x0f));
    } else {
        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, "%x%x%x\"", (mnc & 0x0f00) >> 8,
                                         (mnc & 0xf0) >> 4, (mnc & 0x0f));
    }

    /* CSS带的usLineNum不会大于AT_CSS_PLMN_MAX_LINE_NUM，如果大于，就按AT_CSS_TACLAC_MAX_LINE_NUM个数处理 */
    for (loop = 0; ((loop < cssNty->lineNum) && (loop < AT_CSS_TACLAC_MAX_LINE_NUM)); loop++) {
        length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length, ",%d", cssNty->lineIndexList[loop]);
    }

    length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                     (TAF_CHAR *)g_atSndCodeAddress + length, "%s", g_atCrLf);

    g_atSendDataBuff.bufLen = length;

    At_SendResultData(indexNum, g_atSndCodeAddress, g_atSendDataBuff.bufLen);

    return VOS_OK;
}

