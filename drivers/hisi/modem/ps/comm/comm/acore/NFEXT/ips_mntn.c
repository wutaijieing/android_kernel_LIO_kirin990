/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019.All rights reserved.
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

#include "ips_mntn.h"
#include "securec.h"
#include "v_typdef.h"
#include "nv_stru_gucnas.h"
#include "ttf_oam_interface.h"
#include "ttf_ip_comm.h"
#include "ttf_util.h"
#include "netfilter_ex.h"
#include "netfilter_ex_ctrl.h"
#include "ips_traffic_statistic.h"

#define THIS_FILE_ID PS_FILE_ID_ACPU_IPS_MNTN_C

#define IPS_IFNAMSIZ 16 /* 虚拟网卡名称长度 */
#define IPS_END_NAME 16
#define IPS_MNTN_TRACE_MAX_BYTE_LEN 1514 /* 捕获TCP/IP协议栈报文最长长度 */

/* TCP/IP协议栈报文捕获信息 */
typedef struct {
    VOS_UINT16 primId;
    VOS_UINT16 toolId;
    VOS_UINT8  netIfName[IPS_IFNAMSIZ]; /* 虚拟网卡名称 */
    VOS_UINT16 len;
    VOS_UINT8  data[2];
} IpsMntnPktInfo;

/* 网桥中转报文捕获信息 */
typedef struct {
    VOS_UINT16 primId;
    VOS_UINT16 toolId;
    VOS_UINT8  srcPort[IPS_END_NAME];  /* 发送虚拟网卡信息 */
    VOS_UINT8  destPort[IPS_END_NAME]; /* 接收虚拟网卡名称 */
    VOS_UINT16 len;
    VOS_UINT8  data[2];
} IpsMntnBridgePktInfo;

/* APP控制命令捕获信息 */
typedef struct {
    VOS_UINT16 len;
    VOS_UINT8  data[2];
} IpsMntnAppCmdInfo;

/* 流控信息 */
typedef struct {
    VOS_UINT16 primId;
    VOS_UINT16 toolId;
    VOS_UINT32 fcType;
} IpsMntnFlowCtrlInfo;

IPS_MntnTraceCfgReq        g_ipsTraceMsgCfg; /* TCP/IP协议栈可维可测配置信息 */
IPS_MntnTraceCfgNotifyFunc g_treceCfgNotifyCb = VOS_NULL_PTR;

/* 向OM发送配置确认消息，V_SendMsg中释放透传内容指针 */
VOS_VOID IPSMNTN_SndCfgCnf2Om(VOS_UINT32 contentLen, const VOS_VOID *transMsg)
{
    /* 分配并填写消息 */
    MsgBlock *sendMsg = VOS_AllocMsg(ACPU_PID_NFEXT, contentLen - VOS_MSG_HEAD_LENGTH);

    if (sendMsg == VOS_NULL_PTR) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "Call VOS_AllocMsg fail!\n");
        return;
    }

    (VOS_VOID)memcpy_s(sendMsg, contentLen, transMsg, contentLen);

    if (VOS_SendMsg(ACPU_PID_NFEXT, sendMsg) != VOS_OK) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "Call VOS_SendMsg fail!\n");
    }
}

#if (FEATURE_NFEXT == FEATURE_ON)

/* 向OM发送透传消息, 自处理任务勾包完成后释放透传指针; 由于可能导致乱序，不再支持非中断上下文直接勾包 */
STATIC VOS_VOID IPSMNTN_TransMsg(VOS_UINT8 *traceData, VOS_UINT32 contentLen, TTF_MntnMsgTypeUint16 msgName)
{
    NfExtDataRingBuf       bufData     = {0};
    mdrv_diag_trans_ind_s *diagRptData = (mdrv_diag_trans_ind_s*)traceData;

    /* 填充信息 */
    diagRptData->ulPid    = ACPU_PID_NFEXT;
    diagRptData->ulMsgId  = msgName;
    diagRptData->ulModule = MDRV_DIAG_GEN_MODULE(MODEM_ID_0, DIAG_MODE_COMM);
    diagRptData->ulLength = contentLen;
    diagRptData->pData    = traceData + sizeof(mdrv_diag_trans_ind_s);

    bufData.data = traceData;

    if (NFEXT_AddDataToRingBuf(&bufData) != VOS_OK) {
        NF_EXT_MEM_FREE(ACPU_PID_NFEXT, traceData);
    }
}

/* 根据消息配置信息，获取捕获报文长度 */
STATIC VOS_UINT32 IPSMNTN_GetPktLenByTraceCfg(const IPS_MntnTraceCfg *traceCfg, VOS_UINT16 pktLen, VOS_UINT8 *pktData,
    VOS_UINT32 *dataLen)
{
    IPS_MntnTraceChosenUint32 choice       = traceCfg->choice;
    VOS_UINT32                traceDataLen = 0;
    *dataLen                               = 0;

    choice = PS_MIN(choice, IPS_MNTN_TRACE_MSG_HEADER_CHOSEN);

    /* 判断定时器是否启动 */
    switch (choice) {
        case IPS_MNTN_TRACE_NULL_CHOSEN:
            return VOS_ERR;
        case IPS_MNTN_TRACE_MSG_HEADER_CHOSEN:
            traceDataLen = TTF_GetIpDataTraceLen(ACPU_PID_NFEXT, pktData, pktLen);
            *dataLen     = TTF_MIN(traceDataLen, pktLen);
            break;
        case IPS_MNTN_TRACE_CONFIGURABLE_LEN_CHOSEN:
            *dataLen = TTF_MIN(traceCfg->traceDataLen, pktLen);
            break;
        case IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN:
            *dataLen = pktLen;
            break;
        default:
            return VOS_ERR;
    }

    return VOS_OK;
}

STATIC IPS_MntnTraceCfg* IPSMNTN_MatchPktTraceCfg(TTF_MntnMsgTypeUint16 msgType)
{
    /* 根据消息类型选择对应配置信息 */
    if (msgType == ID_IPS_TRACE_INPUT_DATA_INFO) {
        return &(g_ipsTraceMsgCfg.preRoutingTraceCfg);
    } else if (msgType == ID_IPS_TRACE_OUTPUT_DATA_INFO) {
        return &(g_ipsTraceMsgCfg.postRoutingTraceCfg);
    } else if ((msgType >= ID_IPS_TRACE_BRIDGE_DATA_INFO) && (msgType <= ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_STOP)) {
        return &(g_ipsTraceMsgCfg.localTraceCfg);
    } else {
        return VOS_NULL_PTR;
    }
}

#define IPS_MNTN_TRACE_GRO_MAX_BYTE_LEN 64000 /* 捕获TCP/IP协议栈报文最长长度 */

/* 捕获TCP/IP协议栈接收和发送报文回调函数 */
VOS_VOID IPSMNTN_PktInfoCB(const VOS_CHAR *netIfName, VOS_UINT32 nameLen, const struct sk_buff *skb,
    TTF_MntnMsgTypeUint16 msgType)
{
    VOS_UINT32              ipDataLen = 0;
    VOS_UINT32              captureLen;
    VOS_UINT8              *traceData = VOS_NULL_PTR;
    IpsMntnPktInfo         *traceMsg  = VOS_NULL_PTR;
    const IPS_MntnTraceCfg* traceCfg  = VOS_NULL_PTR;
    VOS_UINT8              *ipHeader  = VOS_NULL_PTR;

    /* HIDS未连接 */
    if (mdrv_diag_get_conn_state() == VOS_NO) {
        return;
    }

    traceCfg = IPSMNTN_MatchPktTraceCfg(msgType);
    if (traceCfg == VOS_NULL_PTR) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "msgType is not Expectes!\n");
        return;
    }

    ipHeader = skb_network_header(skb);
    /* 根据消息配置信息，获取输出报文长度 */
    if (IPSMNTN_GetPktLenByTraceCfg(traceCfg, skb->len, ipHeader, &ipDataLen) == VOS_ERR) {
        /* 不捕获该报文 */
        return;
    }

    ipDataLen = PS_MIN(IPS_MNTN_TRACE_GRO_MAX_BYTE_LEN, ipDataLen);
    TTF_GET_OFFSET(captureLen, IpsMntnPktInfo, data);
    /* IP数据长度加MAC头长度对应WireShark解析报文中的Capture Length，即捕获长度 */
    captureLen += ipDataLen;
    captureLen += MAC_HEADER_LENGTH;

    if (IPSMNTN_PktIsOverFlow(captureLen) == VOS_TRUE) {
        return;
    }

    /* 从Linux内核申请内存 */
    traceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, captureLen + sizeof(mdrv_diag_trans_ind_s));
    if (traceData == VOS_NULL_PTR) {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_ALLOC_MEM_FAIL);
        return;
    }
    /* 构建透明消息 */
    traceMsg = (IpsMntnPktInfo*)(traceData + sizeof(mdrv_diag_trans_ind_s));
    /* traceMsg->len对应WireShark解析报文中的Frame Length，即帧长度 */
    traceMsg->len    = skb->len + MAC_HEADER_LENGTH;
    traceMsg->primId = msgType;
    TTF_ACORE_SF_CHK(memcpy_s(traceMsg->netIfName, IPS_IFNAMSIZ, netIfName, nameLen));

    /* 在某些情况下(如配置 traceCfg->ulTraceDataLen 为0)或ipDataLen为0的情况，存在异常，这里进行保护 */
    /*lint -e{416,426,669} */
    if (ipDataLen > 0) {
        if (skb_copy_bits(skb, 0, traceMsg->data + MAC_HEADER_LENGTH, ipDataLen) < 0) {
            TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "Call skb_copy_bits fail!\n");
            NF_EXT_MEM_FREE(ACPU_PID_NFEXT, traceData);
            return;
        }
    }

    /* user版本下脱敏IP地址 */
    TTF_MaskIpAddrTraces(ACPU_PID_NFEXT, traceMsg->data + MAC_HEADER_LENGTH, (VOS_UINT16)ipDataLen);

    IPSMNTN_TransMsg(traceData, captureLen, msgType);
}

STATIC IPS_MntnTraceCfg* IPSMNTN_MatchBridgePktTraceCfg(TTF_MntnMsgTypeUint16 msgType)
{
    if ((msgType >= ID_IPS_TRACE_BRIDGE_DATA_INFO) && (msgType <= ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_STOP)) {
        return &(g_ipsTraceMsgCfg.bridgeArpTraceCfg);
    }
    return VOS_NULL_PTR;
}

/* 捕获网桥中转报文回调函数 */
VOS_VOID IPSMNTN_BridgePktInfoCB(const VOS_CHAR *srcPort, const VOS_CHAR *destPort, VOS_UINT8 *pktData, VOS_UINT16 pktLen,
    TTF_MntnMsgTypeUint16 msgType)
{
    VOS_UINT32              ipDataLen = 0;
    VOS_UINT32              captureLen;
    IpsMntnBridgePktInfo   *traceMsg  = VOS_NULL_PTR;
    VOS_UINT8              *traceData = VOS_NULL_PTR;
    const IPS_MntnTraceCfg* traceCfg  = VOS_NULL_PTR;

    if (pktData == VOS_NULL_PTR) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "pktData is NULL!\n");
        return;
    }

    if (mdrv_diag_get_conn_state() == VOS_NO) {
        return;
    }
    /* 根据消息类型选择对应配置信息 */
    traceCfg = IPSMNTN_MatchBridgePktTraceCfg(msgType);
    if (traceCfg == VOS_NULL_PTR) {
        TTF_LOG1(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "msgType is not Expectes!\n", msgType);
        return;
    }
    /* 获得IP报文长度 */
    if (IPSMNTN_GetPktLenByTraceCfg(traceCfg, pktLen, pktData, &ipDataLen) == VOS_ERR) {
        /* 不捕获该报文 */
        return;
    }

    ipDataLen = PS_MIN(IPS_MNTN_TRACE_MAX_BYTE_LEN, ipDataLen);
    TTF_GET_OFFSET(captureLen, IpsMntnBridgePktInfo, data);
    /* IP数据长度加MAC头长度对应WireShark解析报文中的Capture Length，即捕获长度 */
    captureLen += ipDataLen;
    captureLen += MAC_HEADER_LENGTH;

    /* 从Linux内核申请内存 */
    traceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, captureLen + sizeof(mdrv_diag_trans_ind_s));
    if (traceData == VOS_NULL_PTR) {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_ALLOC_MEM_FAIL);
        return;
    }

    /* 构建透明消息 */
    traceMsg = (IpsMntnBridgePktInfo*)(traceData + sizeof(mdrv_diag_trans_ind_s));
    /* traceMsg->len对应WireShark解析报文中的Frame Length，即帧长度 */
    traceMsg->len    = pktLen + MAC_HEADER_LENGTH;
    traceMsg->primId = msgType;

    if (srcPort != VOS_NULL_PTR) {
        TTF_ACORE_SF_CHK(memcpy_s(traceMsg->srcPort, IPS_END_NAME, srcPort, IPS_END_NAME));
    }
    if (destPort != VOS_NULL_PTR) {
        TTF_ACORE_SF_CHK(memcpy_s(traceMsg->destPort, IPS_END_NAME, destPort, IPS_END_NAME));
    }
    if (ipDataLen > 0) {
        TTF_ACORE_SF_CHK(memcpy_s(traceMsg->data + MAC_HEADER_LENGTH, ipDataLen, pktData, ipDataLen));
    }

    /* user版本下脱敏IP地址 */
    TTF_MaskIpAddrTraces(ACPU_PID_NFEXT, traceMsg->data + MAC_HEADER_LENGTH, (VOS_UINT16)ipDataLen);

    IPSMNTN_TransMsg(traceData, captureLen, msgType);
}

/* 捕获TCP/IP协议栈控制报文回调函数 */
VOS_VOID IPSMNTN_CtrlPktInfoCB(const VOS_CHAR *netIfName, VOS_UINT32 nameLen, const VOS_UINT8 *pktData, VOS_UINT16 pktLen,
    TTF_MntnMsgTypeUint16 msgType)
{
    VOS_UINT8      *traceData = VOS_NULL_PTR;
    IpsMntnPktInfo *traceMsg  = VOS_NULL_PTR;
    VOS_UINT32      ipDataLen;
    VOS_UINT32      captureLen = 0;

    if (pktData == VOS_NULL_PTR) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "pucPktData is NULL!\n");
        return;
    }

    /* HIDS未连接 */
    if (mdrv_diag_get_conn_state() == VOS_NO) {
        return;
    }
    ipDataLen = PS_MIN(IPS_MNTN_TRACE_MAX_BYTE_LEN, pktLen);
    TTF_GET_OFFSET(captureLen, IpsMntnPktInfo, data);
    /* 对应WireShark解析报文中的Capture Length，即捕获长度 */
    captureLen += ipDataLen;
    /* 从Linux内核申请内存 */
    traceData = NF_EXT_MEM_ALLOC(ACPU_PID_NFEXT, captureLen + sizeof(mdrv_diag_trans_ind_s));
    if (traceData == VOS_NULL_PTR) {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_ALLOC_MEM_FAIL);
        return;
    }

    /* 构建透明消息 */
    traceMsg = (IpsMntnPktInfo*)(traceData + sizeof(mdrv_diag_trans_ind_s));
    /* traceMsg->len对应WireShark解析报文中的Frame Length，即帧长度 */
    traceMsg->len    = pktLen;
    traceMsg->primId = msgType;
    TTF_ACORE_SF_CHK(memcpy_s(traceMsg->netIfName, IPS_IFNAMSIZ, netIfName, nameLen));
    if (ipDataLen > 0) {
        TTF_ACORE_SF_CHK(memcpy_s(traceMsg->data, ipDataLen, pktData, ipDataLen));
    }

    /* user版本下脱敏IP地址 */
    TTF_MaskIpAddrTraces(ACPU_PID_NFEXT, traceMsg->data, (VOS_UINT16)ipDataLen);

    IPSMNTN_TransMsg(traceData, captureLen, msgType);
}

#define MNTN_FLOW_CTRL_INFO_LEN ((VOS_UINT32)sizeof(IpsMntnFlowCtrlInfo))
/* 协议栈流控可维可测上报 */
VOS_VOID IPSMNTN_FlowCtrl(VOS_UINT32 fcType, TTF_MntnMsgTypeUint16 msgType)
{
    mdrv_diag_trans_ind_s transData   = {0};
    IpsMntnFlowCtrlInfo   flowCtrlMsg = {0};

    flowCtrlMsg.fcType = fcType;
    flowCtrlMsg.primId = msgType;

    /* 填充信息 */
    transData.ulPid    = ACPU_PID_NFEXT;
    transData.ulMsgId  = msgType;
    transData.ulModule = MDRV_DIAG_GEN_MODULE(MODEM_ID_0, DIAG_MODE_COMM);
    transData.ulLength = MNTN_FLOW_CTRL_INFO_LEN;
    transData.pData    = &flowCtrlMsg;

    if (mdrv_diag_trans_report(&transData) != VOS_OK) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "Call mdrv_diag_trans_report fail!\n");
    }
}

#endif /* #if (FEATURE_ON == FEATURE_NFEXT) */

/* 网桥中转钩包配置参数检查 */
STATIC PS_BoolUint8 IPSMNTN_BridgeTraceCfgChkParam(const IPS_MntnTraceCfgReq *msg)
{
    if (msg->bridgeArpTraceCfg.choice > IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN) {
        return PS_FALSE;
    }
    return PS_TRUE;
}

/* TCP/IP协议栈报文钩包配置参数检查 */
STATIC PS_BoolUint8 IPSMNTN_TraceCfgChkParam(const IPS_MntnTraceCfgReq *msg)
{
    if ((msg->preRoutingTraceCfg.choice > IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN) ||
        (msg->postRoutingTraceCfg.choice > IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN) ||
        (msg->localTraceCfg.choice > IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN) ||
        (msg->bridgeArpTraceCfg.choice > IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN)) {
        return PS_FALSE;
    }

    if (msg->adsIpConfig > WTTF_TRACE_MSG_YES) {
        return PS_FALSE;
    }
    return PS_TRUE;
}

/* 钩包配置参数检查 */
STATIC PS_BoolUint8 IPSMNTN_TraceAdvancedCfgChkParam(const IPS_MntnTraceCfgReq *msg)
{
    /* TCP/IP协议栈报文钩包配置参数检查和网桥中转报文钩包配置参数检查 */
    if ((IPSMNTN_TraceCfgChkParam(msg) == PS_FALSE) || (IPSMNTN_BridgeTraceCfgChkParam(msg) == PS_FALSE)) {
        return PS_FALSE;
    }

    if (msg->adsIpConfig > WTTF_TRACE_MSG_YES) {
        return PS_FALSE;
    }

    return PS_TRUE;
}

/* 读取脱敏NV配置，返回脱敏开启标志 */
STATIC VOS_UINT8 IPSMNTN_GetPrivacyFilterNvCfg(VOS_VOID)
{
    VOS_UINT8 logFilterFlag = VOS_TRUE;

    return logFilterFlag;
}

/* 根据脱敏配置，调整钩包配置 */
STATIC VOS_VOID IPSMNTN_TraceSensitiveAdjustSwitch(IPS_MntnTraceCfgReq *traceCfg)
{
    VOS_UINT8 adjustFlag = PS_FALSE;

    /* 非敏感状态不做任何更改 */
    if (IPSMNTN_GetPrivacyFilterNvCfg() != VOS_TRUE) {
        return;
    }

    /* 如下校正勾包状态 */
    if (traceCfg->bridgeArpTraceCfg.choice > IPS_MNTN_TRACE_MSG_HEADER_CHOSEN) {
        traceCfg->bridgeArpTraceCfg.choice = IPS_MNTN_TRACE_MSG_HEADER_CHOSEN;
        adjustFlag                         = PS_TRUE;
    }

    if (traceCfg->preRoutingTraceCfg.choice > IPS_MNTN_TRACE_MSG_HEADER_CHOSEN) {
        traceCfg->preRoutingTraceCfg.choice = IPS_MNTN_TRACE_MSG_HEADER_CHOSEN;
        adjustFlag                          = PS_TRUE;
    }

    if (traceCfg->postRoutingTraceCfg.choice > IPS_MNTN_TRACE_MSG_HEADER_CHOSEN) {
        traceCfg->postRoutingTraceCfg.choice = IPS_MNTN_TRACE_MSG_HEADER_CHOSEN;
        adjustFlag                           = PS_TRUE;
    }

    if (traceCfg->localTraceCfg.choice > IPS_MNTN_TRACE_MSG_HEADER_CHOSEN) {
        traceCfg->localTraceCfg.choice = IPS_MNTN_TRACE_MSG_HEADER_CHOSEN;
        adjustFlag                     = PS_TRUE;
    }

    if (adjustFlag == PS_TRUE) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "In Sensitive status, Some config has been modify!\n");
    }
}

STATIC VOS_VOID IPSMNTN_TraceCfgNotify(VOS_VOID)
{
    VOS_BOOL traceEnable = (g_ipsTraceMsgCfg.adsIpConfig == WTTF_TRACE_MSG_YES) ? VOS_TRUE : VOS_FALSE;

    if (g_treceCfgNotifyCb != VOS_NULL_PTR) {
        g_treceCfgNotifyCb(traceEnable);
    }
}

/* 保存钩包配置 */
VOS_VOID IPSMNTN_TraceAdvancedCfgReq(const MsgBlock *msg)
{
    PS_BoolUint8                     result         = PS_FALSE;
    const OM_IpsAdvancedTraceCfgReq* cfgReq         = (OM_IpsAdvancedTraceCfgReq*)msg;
    IPS_OmAdvancedTraceCfgCnf        ipsTraceCfgCnf = {{0}};

    /* 检测配置参数是否合法 */
    result = IPSMNTN_TraceAdvancedCfgChkParam(&(cfgReq->ipsAdvanceCfgReq));

    /* Fill DIAG trans msg header */
    TTF_SET_MSG_SENDER_ID(&(ipsTraceCfgCnf.diagHdr), ACPU_PID_NFEXT);
    TTF_SET_MSG_RECEIVER_ID(
        &(ipsTraceCfgCnf.diagHdr), MSP_PID_DIAG_APP_AGENT); /* 把应答消息发送给DIAG，由DIAG把透传命令的处理结果发送给HIDS工具 */
    TTF_SET_MSG_LEN(&(ipsTraceCfgCnf.diagHdr), sizeof(IPS_OmAdvancedTraceCfgCnf) - VOS_MSG_HEAD_LENGTH);
    ipsTraceCfgCnf.diagHdr.msgId = ID_IPS_OM_ADVANCED_TRACE_CONFIG_CNF;

    /* DIAG透传命令中的特定信息 */
    ipsTraceCfgCnf.diagHdr.originalId = cfgReq->diagHdr.originalId;
    ipsTraceCfgCnf.diagHdr.terminalId = cfgReq->diagHdr.terminalId;
    ipsTraceCfgCnf.diagHdr.timeStamp  = cfgReq->diagHdr.timeStamp;
    ipsTraceCfgCnf.diagHdr.sn         = cfgReq->diagHdr.sn;

    if (result == PS_FALSE) {
        /* 如果参数检测不合法，向OM回复配置失败 */
        ipsTraceCfgCnf.ipsAdvanceCfgCnf.result = PS_FAIL;
        IPSMNTN_SndCfgCnf2Om(sizeof(IPS_OmAdvancedTraceCfgCnf), &ipsTraceCfgCnf);
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "IPSMNTN_TraceAdvancedCfgReq check fail!\n");
        return;
    }

#if (FEATURE_NFEXT == FEATURE_ON)
    if (NFEXT_ConfigEffective(&(cfgReq->ipsAdvanceCfgReq)) == PS_FALSE) {
        /* 如果注册钩子函数失败，向OM回复配置失败 */
        ipsTraceCfgCnf.ipsAdvanceCfgCnf.result = PS_FAIL;
        IPSMNTN_SndCfgCnf2Om(sizeof(IPS_OmAdvancedTraceCfgCnf), &ipsTraceCfgCnf);
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_ERROR, "IPS_MNTN_SetCallbackFunc fail!\n");
        return;
    }
#endif
    /* 保存配置参数 */
    (VOS_VOID)memcpy_s(
        &g_ipsTraceMsgCfg, sizeof(g_ipsTraceMsgCfg), &(cfgReq->ipsAdvanceCfgReq), sizeof(cfgReq->ipsAdvanceCfgReq));

    /* 根据脱敏状态调整勾包开关 */
    IPSMNTN_TraceSensitiveAdjustSwitch(&g_ipsTraceMsgCfg);

    IPSMNTN_TraceCfgNotify();

    /* 向OM回复配置成功 */
    ipsTraceCfgCnf.ipsAdvanceCfgCnf.result = PS_SUCC;

    IPSMNTN_SndCfgCnf2Om(sizeof(IPS_OmAdvancedTraceCfgCnf), &ipsTraceCfgCnf);
}

VOS_VOID IPSMNTN_RegTraceCfgNotify(IPS_MntnTraceCfgNotifyFunc notifyFunc)
{
    g_treceCfgNotifyCb = notifyFunc;
}
