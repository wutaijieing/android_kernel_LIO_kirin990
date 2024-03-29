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

#include "ps_common_def.h"
#include "RnicMsgProc.h"
#include "RnicEntity.h"
#include "RnicMntn.h"
#include "RnicIfaceOndemand.h"
#include "RnicLog.h"
#include "bastet_rnic_interface.h"
#include "rnic_ondemand_i.h"
#include "rnic_policy_manage.h"
#include "securec.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_RNIC_IFACE_CFG_C


VOS_VOID RNIC_IFACE_SetNapiCfg(RNIC_DEV_ID_ENUM_UINT8 rmNetId)
{
    struct rnic_napi_config_s napiCfg;

    napiCfg.devid             = rmNetId;
    napiCfg.napi_enable       = RNIC_GET_NAPI_FEATURE(rmNetId);
    napiCfg.gro_enable        = RNIC_GET_GRO_FEATURE(rmNetId);
    napiCfg.napi_weight       = RNIC_GET_NAPI_WEIGHT(rmNetId);
    napiCfg.napi_queue_length = RNIC_GET_NAPI_POLL_QUE_MAX_LEN(rmNetId);

    rnic_set_dev_napi_config(&napiCfg);
    return;
}


VOS_VOID RNIC_IFACE_SetNapiLbCfg(RNIC_DEV_ID_ENUM_UINT8 rmNetId)
{
    struct rnic_lb_config_s napiLbCfg;

    napiLbCfg.devid       = rmNetId;
    napiLbCfg.enable      = RNIC_GET_NAPI_LB_FEATURE(rmNetId);
    napiLbCfg.cpu_bitmask = RNIC_GET_NAPI_LB_CPUMASK(rmNetId);

    if (memcpy_s(napiLbCfg.level_cfg, sizeof(struct rnic_lb_level_config_s) * RNIC_LB_MAX_LEVEL,
                 RNIC_GET_NAPI_LB_LEVEL_CFG(rmNetId),
                 sizeof(RNIC_NAPI_LbLevelCfg) * RNIC_NVIM_NAPI_LB_MAX_LEVEL) != EOK) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_SetNapiLbCfg: memcpy fail!");
    }

    rnic_set_dev_napi_lb_config(&napiLbCfg);
    return;
}


VOS_VOID RNIC_IFACE_SetNapiLbLevel(RNIC_DEV_ID_ENUM_UINT8 rmNetId, VOS_UINT8 lbLevel)
{
    if (lbLevel != RNIC_GET_NAPI_LB_CUR_LEVEL(rmNetId)) {
        if (!rnic_set_dev_napi_lb_level(rmNetId, lbLevel)) {
            RNIC_SET_NAPI_LB_CUR_LEVEL(rmNetId, lbLevel);
        }
    }
    return;
}


VOS_VOID RNIC_IFACE_SetNapiWeight(RNIC_DEV_ID_ENUM_UINT8 rmNetId, VOS_UINT8 weight)
{
    if (weight != RNIC_GET_NAPI_WEIGHT(rmNetId)) {
        rnic_set_dev_napi_weight(rmNetId, weight);
        RNIC_SET_NAPI_WEIGHT(rmNetId, weight);
    }

    return;
}


VOS_VOID RNIC_IFACE_SetRhcCfg(VOS_VOID)
{
    struct rnic_rhc_config_s rhcCfg;

    rhcCfg.rhc_feature = g_rnicCtx.cfg.feature;

    if (memcpy_s(rhcCfg.level_cfg, sizeof(struct rnic_rhc_level_config_s) * RNIC_RHC_MAX_LEVEL, g_rnicCtx.cfg.levelCfg,
                 sizeof(RNIC_RhcLevel) * RNIC_NVIM_RHC_MAX_LEVEL) != EOK) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_SetRhcCfg: memcpy_s fail!");
        return;
    }

    rnic_set_dev_rhc_config(&rhcCfg);
}


VOS_VOID RNIC_IFACE_ResetNapiConfig(RNIC_DEV_ID_ENUM_UINT8 rmNetId)
{
    VOS_UINT8 minWeight = RNIC_GET_NAPI_WEIGHT_LEVEL1(rmNetId);

    if (RNIC_FEATURE_ON == RNIC_GET_NAPI_FEATURE(rmNetId)) {
        if (0 != rnic_set_dev_napi_weight(rmNetId, minWeight)) {
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_ResetNapiConfig: set rnic_set_napi_weight fail!");
        }
        RNIC_SET_NAPI_WEIGHT(rmNetId, minWeight);
    }

    if (RNIC_FEATURE_ON == RNIC_GET_NAPI_LB_FEATURE(rmNetId)) {
        if (0 != rnic_set_dev_napi_lb_level(rmNetId, 0)) {
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_ResetNapiConfig: set rnic_set_lb_level fail!");
        }
        RNIC_SET_NAPI_LB_CUR_LEVEL(rmNetId, 0);
    }

    return;
}


VOS_VOID RNIC_IFACE_ResetRhcLevel(VOS_VOID)
{
    if (g_rnicCtx.cfg.feature == RNIC_FEATURE_ON) {
        rnic_set_dev_rhc_level(0);
        g_rnicCtx.cfg.rhcLevel = 0;
    }
}


VOS_UINT8 RNIC_IFACE_CalcNapiWeight(VOS_UINT8 rmNetId)
{
    VOS_UINT8  weight;
    VOS_UINT32 dlNapiRecvPktNum;

    weight           = RNIC_NAPI_POLL_DEFAULT_WEIGHT;
    dlNapiRecvPktNum = RNIC_GET_IFACE_PERIOD_RECV_PKT(rmNetId);

    /*
     * 分为5档:2/4/8/32/64
     * 150M bps每秒收包数在12300左右，在[0,12300]这个范围内，weight值设置为2.
     * 300M bps每秒收包数在24600左右，在[12300,24600]这个范围内，weight值设置为4.
     * 450M bps每秒收包数在38400左右，在[24600,38400]这个范围内，weight值设置为8.
     * 600M bps每秒收包数在49200左右，在[38400,49200]这个范围内，weight值设置为32.
     * 大于600M，weight值统一设置为64.
     */

    if (dlNapiRecvPktNum <= RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL1(rmNetId)) {
        weight = RNIC_GET_NAPI_WEIGHT_LEVEL1(rmNetId);
    } else if ((dlNapiRecvPktNum > RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL1(rmNetId)) &&
               (dlNapiRecvPktNum <= RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL2(rmNetId))) {
        weight = RNIC_GET_NAPI_WEIGHT_LEVEL2(rmNetId);
    } else if ((dlNapiRecvPktNum > RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL2(rmNetId)) &&
               (dlNapiRecvPktNum <= RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL3(rmNetId))) {
        weight = RNIC_GET_NAPI_WEIGHT_LEVEL3(rmNetId);
    } else if ((dlNapiRecvPktNum > RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL3(rmNetId)) &&
               (dlNapiRecvPktNum <= RNIC_GET_DL_PKT_NUM_PER_SEC_LEVEL4(rmNetId))) {
        weight = RNIC_GET_NAPI_WEIGHT_LEVEL4(rmNetId);
    } else {
        weight = RNIC_GET_NAPI_WEIGHT_LEVEL5(rmNetId);
    }

    return weight;
}

VOS_VOID RNIC_IFACE_AdjustNapiWeight(VOS_UINT8 rmNetId)
{
    VOS_UINT8 weight;

    /* NAPI Weight动态调整模式，才去计算weight值并调整。其他情况默认采用静态default weight值。 */
    if (NAPI_WEIGHT_ADJ_DYNAMIC_MODE == RNIC_GET_NAPI_WEIGHT_ADJ_MODE(rmNetId)) {
        weight = RNIC_IFACE_CalcNapiWeight(rmNetId);
        RNIC_IFACE_SetNapiWeight(rmNetId, weight);
    }

    return;
}


VOS_VOID RNIC_IFACE_AdjNapiLbLevel(VOS_UINT8 rmNetId)
{
    VOS_UINT32 lbLevel;
    VOS_UINT32 dlNapiRecvPktNum;

    /* NAPI负载均衡特性打开时，才根据流量调整CPU的权重 */
    if (RNIC_FEATURE_ON == RNIC_GET_NAPI_LB_FEATURE(rmNetId)) {
        dlNapiRecvPktNum = RNIC_GET_IFACE_PERIOD_RECV_PKT(rmNetId);

        for (lbLevel = 0; lbLevel < (RNIC_NVIM_NAPI_LB_MAX_LEVEL - 1); lbLevel++) {
            if ((dlNapiRecvPktNum >= RNIC_GET_NAPI_LB_LEVEL_PPS(rmNetId, lbLevel)) &&
                (dlNapiRecvPktNum < RNIC_GET_NAPI_LB_LEVEL_PPS(rmNetId, lbLevel + 1UL)))
                break;
        }

        RNIC_IFACE_SetNapiLbLevel(rmNetId, (VOS_UINT8)lbLevel);
    }

    return;
}


VOS_UINT8 RNIC_IFACE_GetLevelByPps(VOS_UINT32 totalPps, VOS_UINT32 nonTcpPps)
{
    VOS_UINT32 lbLevel;

    for (lbLevel = 0; lbLevel < RNIC_NVIM_RHC_MAX_LEVEL; lbLevel++) {
        if ((totalPps >= g_rnicCtx.cfg.levelCfg[lbLevel].totalPps) &&
            (nonTcpPps >= g_rnicCtx.cfg.levelCfg[lbLevel].nonTcpPps)) {
            continue;
        }
        break;
    }
    lbLevel = (lbLevel == 0) ? 0 : (lbLevel - 1);

    return (VOS_UINT8)lbLevel;
}


VOS_VOID RNIC_IFACE_AdjRhcLevel(RNIC_DSFLOW_Stats *pdsFlowTotalStats)
{
    VOS_UINT8 rhcLevel = 0;

    if (g_rnicCtx.cfg.feature == RNIC_FEATURE_OFF) {
        return;
    }

    rhcLevel = RNIC_IFACE_GetLevelByPps(pdsFlowTotalStats->periodRecvPktNum, pdsFlowTotalStats->periodRecvNonTcpPktNum);

    if ((rhcLevel != g_rnicCtx.cfg.rhcLevel) || (g_rnicCtx.cfg.levelCfg[rhcLevel].isolationDisable)) {
        rnic_set_dev_rhc_level(rhcLevel);
        g_rnicCtx.cfg.rhcLevel = rhcLevel;
    }

    return;
}

#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)

VOS_VOID RNIC_IFACE_NapiSchedule(VOS_ULONG usrData)
{
    rnic_rx_complete((VOS_UINT8)usrData);
    return;
}


VOS_INT RNIC_IFACE_RegRxHandler(RNIC_IFACE_Ctx *ifaceCtx)
{
    struct ads_iface_rx_handler_s rxHandler;

    memset_s(&rxHandler, sizeof(rxHandler), 0x00, sizeof(rxHandler));

    rxHandler.user_data      = ifaceCtx->rmNetId;
    rxHandler.rx_dma_support = true;
    rxHandler.rx_func        = RNIC_DataRxProc;
    rxHandler.rx_cmplt_func  = RNIC_IFACE_NapiSchedule;

    return ads_iface_register_rx_handler(ifaceCtx->ifaceId, &rxHandler);
}


VOS_VOID RNIC_IFACE_SetNetDevUp(RNIC_IFACE_Ctx *ifaceCtx, VOS_UINT8 ipFamily)
{
    struct rnic_ps_iface_config_s psIfaceCfg;

    memset_s(&psIfaceCfg, sizeof(psIfaceCfg), 0x00, sizeof(psIfaceCfg));

    psIfaceCfg.devid     = ifaceCtx->rmNetId;
    psIfaceCfg.ip_family = ipFamily;

    psIfaceCfg.info.iface_id       = ifaceCtx->ifaceId;
    psIfaceCfg.info.pdu_session_id = (RNIC_IPV4_ADDR == ipFamily) ? ifaceCtx->psIfaceInfo.ipv4ExRabId :
                                                                    ifaceCtx->psIfaceInfo.ipv6ExRabId;
    psIfaceCfg.info.fc_head  = RNIC_GET_FC_HEAD_BY_MODEMID(ifaceCtx->psIfaceInfo.modemId);
    psIfaceCfg.info.modem_id = (VOS_UINT8)ifaceCtx->psIfaceInfo.modemId;

#if ((FEATURE_ON == FEATURE_IMS) && (FEATURE_ON == FEATURE_ACPU_PHONE_MODE))
    if (RNIC_PS_RAT_TYPE_IWLAN == ifaceCtx->psIfaceInfo.ratType) {
        psIfaceCfg.data_tx_func = RNIC_VoWifiDataTxProc;
    } else
#endif
    {
        psIfaceCfg.data_tx_func = (RNIC_IPV4_ADDR == ipFamily) ? RNIC_V4DataTxProc : RNIC_V6DataTxProc;
    }

    rnic_set_dev_ps_iface_up(&psIfaceCfg);

    return;
}


VOS_VOID RNIC_IFACE_SetNetDevDown(RNIC_IFACE_Ctx *ifaceCtx, VOS_UINT8 ipFamily)
{
    struct rnic_ps_iface_config_s psIfaceCfg;

    memset_s(&psIfaceCfg, sizeof(psIfaceCfg), 0x00, sizeof(psIfaceCfg));

    psIfaceCfg.devid               = ifaceCtx->rmNetId;
    psIfaceCfg.ip_family           = ipFamily;
    psIfaceCfg.info.iface_id       = RNIC_INVALID_IFACE_ID;
    psIfaceCfg.info.pdu_session_id = RNIC_INVALID_PDU_SESSION_ID;
    psIfaceCfg.info.fc_head        = RNIC_INVALID_FC_HEAD;
    psIfaceCfg.info.modem_id       = MODEM_ID_BUTT;
    psIfaceCfg.data_tx_func        = VOS_NULL_PTR;

    rnic_set_dev_ps_iface_down(&psIfaceCfg);

    RNIC_IFACE_ResetNapiConfig(ifaceCtx->rmNetId);

    return;
}
#else

VOS_VOID RNIC_IFACE_NapiSchedule(VOS_UINT32 usrData)
{
    rnic_rx_complete((VOS_UINT8)usrData);
    return;
}


VOS_INT RNIC_IFACE_RegRxHandler(RNIC_IFACE_Ctx *ifaceCtx, VOS_UINT8 ipFamilyMask)
{
    VOS_UINT8 exRabId = RNIC_RAB_ID_INVALID;

    if (RNIC_BIT8_IS_SET(ipFamilyMask, RNIC_IPV4_ADDR)) {
        exRabId = ifaceCtx->psIfaceInfo.ipv4ExRabId;
    }

    if (RNIC_BIT8_IS_SET(ipFamilyMask, RNIC_IPV6_ADDR)) {
        exRabId = ifaceCtx->psIfaceInfo.ipv6ExRabId;
    }

    if (RNIC_PS_RAT_TYPE_3GPP == ifaceCtx->psIfaceInfo.ratType) {
        /* 注册下行发送函数，ADS调用注册的函数发送下行数据 */
        (VOS_VOID)ADS_DL_RegDlDataCallback(exRabId, RNIC_DataRxProc, ifaceCtx->rmNetId);

#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
        /* NAPI特性打开才执行 */
        if (RNIC_FEATURE_ON == RNIC_GET_NAPI_FEATURE(ifaceCtx->rmNetId)) {
            /* 注册下行NAPI处理的回调函数 */
            (VOS_VOID)ADS_DL_RegNapiCallback(exRabId, RNIC_IFACE_NapiSchedule, ifaceCtx->rmNetId);
        }
#endif
    }

    return VOS_OK;
}


VOS_VOID RNIC_IFACE_SetNetDevUp(RNIC_IFACE_Ctx *ifaceCtx, VOS_UINT8 ipFamily)
{
    struct rnic_ps_iface_config_s psIfaceCfg;

    memset_s(&psIfaceCfg, sizeof(psIfaceCfg), 0x00, sizeof(psIfaceCfg));

    psIfaceCfg.devid     = ifaceCtx->rmNetId;
    psIfaceCfg.ip_family = ipFamily;

    /* 向网卡驱动挂载上行收包回调 */
#if ((FEATURE_ON == FEATURE_IMS) && (FEATURE_ON == FEATURE_ACPU_PHONE_MODE))
    if (RNIC_PS_RAT_TYPE_IWLAN == ifaceCtx->psIfaceInfo.ratType) {
        psIfaceCfg.data_tx_func = RNIC_VoWifiDataTxProc;
    } else
#endif
    {
        psIfaceCfg.data_tx_func = (RNIC_IPV4_ADDR == ipFamily) ? RNIC_V4DataTxProc : RNIC_V6DataTxProc;
    }

    if (0 != rnic_set_dev_ps_iface_up(&psIfaceCfg)) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_SetDevUp: set rnic_set_ps_iface_up fail!");
    }

    return;
}


VOS_VOID RNIC_IFACE_SetNetDevDown(RNIC_IFACE_Ctx *ifaceCtx, VOS_UINT8 ipFamily)
{
    struct rnic_ps_iface_config_s psIfaceCfg;

    memset_s(&psIfaceCfg, sizeof(psIfaceCfg), 0x00, sizeof(psIfaceCfg));

    psIfaceCfg.devid        = ifaceCtx->rmNetId;
    psIfaceCfg.ip_family    = ipFamily;
    psIfaceCfg.data_tx_func = VOS_NULL_PTR;

    if (0 != rnic_set_dev_ps_iface_down(&psIfaceCfg)) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_SetDevDown: set rnic_set_ps_iface_down fail!");
    }

    RNIC_IFACE_ResetNapiConfig(ifaceCtx->rmNetId);

    return;
}
#endif /* FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM */





VOS_VOID RNIC_IFACE_SetFeatureCfg(VOS_VOID)
{
    VOS_UINT32 indexNum;

    for (indexNum = 0; indexNum <= RNIC_3GPP_NET_ID_MAX_NUM; indexNum++) {
        /* 设置NAPI特性 */
        RNIC_IFACE_SetNapiCfg((VOS_UINT8)indexNum);
        RNIC_IFACE_SetNapiLbCfg((VOS_UINT8)indexNum);

#if (FEATURE_OFF == FEATURE_ACPU_PHONE_MODE)
        /* 注册设备上行丢包回调 */
        RNIC_IFACE_RegTxDropCB((VOS_UINT8)indexNum);
#endif
    }

    RNIC_IFACE_SetRhcCfg();
    return;
}

VOS_VOID RNIC_IFACE_SetDsFlowStats(VOS_UINT8 rmNetId, RNIC_DSFLOW_Stats *dsFlowTotalStats)
{
    struct rnic_dsflow_stats_s *dsflowStats = VOS_NULL_PTR;

    /* 调用驱动接口获取流量信息 */
    dsflowStats = rnic_get_dsflow_stats(rmNetId);

    if (VOS_NULL_PTR == dsflowStats) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_SetDsFlowStats fail, get dsflow stats failed !");
        return;
    }

    g_rnicCtx.ifaceCtx[rmNetId].dsFlowStats.currentRecvRate  = dsflowStats->rx_bytes;
    g_rnicCtx.ifaceCtx[rmNetId].dsFlowStats.currentSendRate  = dsflowStats->tx_bytes;
    g_rnicCtx.ifaceCtx[rmNetId].dsFlowStats.periodRecvPktNum = dsflowStats->rx_packets;
    /* 累加上行发送报文，按需断开定时器超时清零 */
    g_rnicCtx.ifaceCtx[rmNetId].dsFlowStats.periodSendPktNum += dsflowStats->tx_packets;

    g_rnicCtx.ifaceCtx[rmNetId].dsFlowStats.periodRecvTcpPktNum    = dsflowStats->rx_tcp_pkts;
    g_rnicCtx.ifaceCtx[rmNetId].dsFlowStats.periodRecvNonTcpPktNum = dsflowStats->rx_non_tcp_pkts;

    dsFlowTotalStats->currentRecvRate += dsflowStats->rx_bytes;
    dsFlowTotalStats->currentSendRate += dsflowStats->tx_bytes;

    dsFlowTotalStats->periodRecvPktNum += dsflowStats->rx_packets;
    dsFlowTotalStats->periodSendPktNum += dsflowStats->tx_packets;
    dsFlowTotalStats->periodRecvTcpPktNum += dsflowStats->rx_tcp_pkts;
    dsFlowTotalStats->periodRecvNonTcpPktNum += dsflowStats->rx_non_tcp_pkts;

    RNIC_NORMAL_LOG4(ACPU_PID_RNIC, "RNIC_IFACE_SetDsFlowStats: <rx_bytes, tx_bytes, rx_packets, tx_packets> ",
                     dsflowStats->rx_bytes, dsflowStats->tx_bytes, dsflowStats->rx_packets, dsflowStats->tx_packets);

    return;
}


VOS_VOID RNIC_IFACE_ClearDsFlowFlux(VOS_UINT8 rmNetId)
{
    rnic_clear_dsflow_stats(rmNetId);
    return;
}


VOS_VOID RNIC_IFACE_ClearDsFlowStats(VOS_UINT8 rmNetId)
{
    RNIC_IFACE_Ctx *ifaceCtx = VOS_NULL_PTR;

    ifaceCtx = RNIC_GET_IFACE_CTX_ADR(rmNetId);

    ifaceCtx->dsFlowStats.currentRecvRate  = 0;
    ifaceCtx->dsFlowStats.periodRecvPktNum = 0;

    ifaceCtx->dsFlowStats.currentSendRate        = 0;
    ifaceCtx->dsFlowStats.periodSendPktNum       = 0;
    ifaceCtx->dsFlowStats.periodRecvTcpPktNum    = 0;
    ifaceCtx->dsFlowStats.periodRecvNonTcpPktNum = 0;

    return;
}


VOS_VOID RNIC_IFACE_NetdevNotify(VOS_VOID)
{
    RNIC_DEV_READY_STRU *sndMsg;

    /* 内存分配 */
    sndMsg = (RNIC_DEV_READY_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_RNIC, sizeof(RNIC_DEV_READY_STRU));

    if (VOS_NULL_PTR == sndMsg) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendNetdevReadyInd: Malloc failed!");
        return;
    }

    /* 填写消息 */
    RNIC_CFG_INTRA_MSG_HDR(sndMsg, ID_RNIC_NETDEV_READY_IND);

    /* 发送消息 */
    if (VOS_OK != PS_SEND_MSG(ACPU_PID_RNIC, sndMsg)) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendNetdevReadyInd: Send msg failed!");
    }

    return;
}


VOS_VOID RNIC_IFACE_RegDevNotifyCB(VOS_VOID)
{
    struct rnic_deivce_notifier_s devNotifier;

    devNotifier.dev_notifier_func = RNIC_IFACE_NetdevNotify;

    rnic_register_device_notifier(&devNotifier);
    return;
}


VOS_VOID RNIC_IFACE_RegOnDemandOpsCB(VOS_VOID)
{
    struct rnic_ondemand_cb_ops_s ondemandOpsCB;

    ondemandOpsCB.dial_mode_cfg_func    = RNIC_IFACE_OndemandDialModeProc;
    ondemandOpsCB.idle_time_cfg_func    = RNIC_IFACE_OndemandIdleTimeProc;
    ondemandOpsCB.event_report_cfg_func = RNIC_IFACE_OndemandEventReportProc;

    rnic_register_ondemand_ops(&ondemandOpsCB);
    return;
}


VOS_VOID RNIC_IFACE_RegTxDropCB(VOS_UINT8 rmNetId)
{
    struct rnic_ps_iface_drop_notifier_s dropNotify;

    /* 网卡0上面上行报文触发按需拨号 */
    if (rmNetId == RNIC_DEV_ID_RMNET0) {
        dropNotify.devid              = rmNetId;
        dropNotify.drop_notifier_func = RNIC_TxDropProc;

        rnic_register_ps_iface_drop_notifier(&dropNotify);
    }

    return;
}


VOS_VOID RNIC_IFACE_ResetPsIface(VOS_UINT8 rmNetId)
{
    struct rnic_ps_iface_config_s psIfaceCfg;

    psIfaceCfg.devid        = rmNetId;
    psIfaceCfg.data_tx_func = VOS_NULL_PTR;
#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    psIfaceCfg.info.iface_id       = RNIC_INVALID_IFACE_ID;
    psIfaceCfg.info.pdu_session_id = RNIC_INVALID_PDU_SESSION_ID;
    psIfaceCfg.info.fc_head        = RNIC_INVALID_FC_HEAD;
    psIfaceCfg.info.modem_id       = MODEM_ID_BUTT;
#endif

    /* Down V4 网卡 */
    psIfaceCfg.ip_family = RNIC_IPV4_ADDR;
    rnic_set_dev_ps_iface_down(&psIfaceCfg);

    /* Down V6 网卡 */
    psIfaceCfg.ip_family = RNIC_IPV6_ADDR;
    rnic_set_dev_ps_iface_down(&psIfaceCfg);

    RNIC_IFACE_ResetNapiConfig(rmNetId);
    return;
}


VOS_VOID RNIC_IFACE_PsIfaceUp(VOS_UINT8 rmNetId, VOS_UINT8 ipFamilyMask)
{
    RNIC_IFACE_Ctx    *ifaceCtx    = VOS_NULL_PTR;
    RNIC_PS_IfaceInfo *psIfaceInfo = VOS_NULL_PTR;

    ifaceCtx    = RNIC_GET_IFACE_CTX_ADR(rmNetId);
    psIfaceInfo = RNIC_GET_IFACE_PDN_INFO_ADR(rmNetId);

    RNIC_NORMAL_LOG3(ACPU_PID_RNIC, "RNIC_IFACE_PsIfaceUp: <ucRmNetId, ucIpFamilyMask, ucRatType>", rmNetId,
                     ipFamilyMask, psIfaceInfo->ratType);

    if (RNIC_BIT8_IS_SET(ipFamilyMask, RNIC_IPV4_ADDR)) {
        RNIC_IFACE_SetNetDevUp(ifaceCtx, RNIC_IPV4_ADDR);

        /* 停止按需拨号保护定时器 */
        if (RNIC_DEV_ID_RMNET0 == rmNetId) {
            RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_PROTECT);
        }
    }

    if (RNIC_BIT8_IS_SET(ipFamilyMask, RNIC_IPV6_ADDR)) {
        RNIC_IFACE_SetNetDevUp(ifaceCtx, RNIC_IPV6_ADDR);
    }

#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    RNIC_IFACE_RegRxHandler(ifaceCtx);
#else
    RNIC_IFACE_RegRxHandler(ifaceCtx, ipFamilyMask);
#endif

    if (rmNetId <= RNIC_3GPP_NET_ID_MAX_NUM) {
        RNIC_StartTimer(TI_RNIC_DSFLOW_STATS, TI_RNIC_DSFLOW_STATS_LEN);
    }

    RNIC_BIT32_SET(g_rnicCtx.dsflowTimerMask, rmNetId);

    return;
}


VOS_VOID RNIC_IFACE_PsIfaceDown(VOS_UINT8 rmNetId, VOS_UINT8 ipFamilyMask)
{
    RNIC_IFACE_Ctx *ifaceCtx = VOS_NULL_PTR;

    ifaceCtx = RNIC_GET_IFACE_CTX_ADR(rmNetId);

    RNIC_NORMAL_LOG2(ACPU_PID_RNIC, "RNIC_IFACE_PsIfaceDown: <ucRmNetId, ucIpFamilyMask>", rmNetId, ipFamilyMask);

    if (RNIC_BIT8_IS_SET(ipFamilyMask, RNIC_IPV4_ADDR)) {
        RNIC_IFACE_SetNetDevDown(ifaceCtx, RNIC_IPV4_ADDR);

        /* RMNET0为IPV4类型时才会有按需拨号, 网卡down的时候停按需断开定时器 */
        if (RNIC_DEV_ID_RMNET0 == rmNetId) {
            RNIC_StopTimer(TI_RNIC_DEMAND_DIAL_DISCONNECT);
            RNIC_CLEAR_TI_DIALDOWN_EXP_CONT();
        }
    }

    if (RNIC_BIT8_IS_SET(ipFamilyMask, RNIC_IPV6_ADDR)) {
        RNIC_IFACE_SetNetDevDown(ifaceCtx, RNIC_IPV6_ADDR);
    }

    if ((RNIC_BIT_OPT_FALSE == RNIC_GET_IFACE_PDN_IPV4_ACT_STATE(rmNetId)) &&
        (RNIC_BIT_OPT_FALSE == RNIC_GET_IFACE_PDN_IPV6_ACT_STATE(rmNetId))) {
        RNIC_BIT32_CLR(g_rnicCtx.dsflowTimerMask, rmNetId);

        /* 如果数据网卡都down了，则停止流量统计定时器，清除流量统计 */
        if (!(g_rnicCtx.dsflowTimerMask & RNIC_3GPP_NET_MAX_NUM_MASK)) {
            RNIC_StopTimer(TI_RNIC_DSFLOW_STATS);
            RNIC_IFACE_ClearDsFlowStats(rmNetId);
            RNIC_IFACE_ResetRhcLevel();
        }
    }

    return;
}


VOS_VOID RNIC_IFACE_TetherInfo(RNIC_PS_IfaceTetherInfo *tetherInfo)
{
#if (FEATURE_ON == FEATURE_RNIC_NAPI_GRO)
    RNIC_DEV_ID_ENUM_UINT8 rmNetId;

    if (VOS_NULL_PTR == tetherInfo) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_TetherInfo fail, pstTetherInfo is NULL!");
        return;
    }

    if (VOS_OK != rnic_get_devid_by_name((char *)tetherInfo->rmnetName, &rmNetId)) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_TetherInfo fail, no such RmnetName!");
        return;
    }

    if (rmNetId >= RNIC_DEV_ID_BUTT) {
        RNIC_ERROR_LOG1(ACPU_PID_RNIC, "RNIC_IFACE_TetherInfo fail, invalid enRmNetId ", rmNetId);
        return;
    }

    /* 配置指定网卡的GRO功能 */
    if (AT_RNIC_USB_TETHER_CONNECTED == tetherInfo->tetherConnStat) {
        /* 先保存指定网卡的GRO开关 */
        RNIC_SET_TETHER_ORIG_GRO_FEATURE(RNIC_GET_GRO_FEATURE(rmNetId));

        RNIC_SET_GRO_FEATURE(rmNetId, RNIC_FEATURE_OFF);
    } else if (AT_RNIC_USB_TETHER_DISCONNECT == tetherInfo->tetherConnStat) {
        /* 恢复指定网卡的GRO开关 */
        RNIC_SET_GRO_FEATURE(rmNetId, RNIC_GET_TETHER_ORIG_GRO_FEATURE());

        /* 初始化Tethering信息，避免影响下次的设置 */
        RNIC_InitTetherInfo();
    } else {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_IFACE_TetherInfo fail, enTetherConnStat invalid!");
        return;
    }

    RNIC_IFACE_SetNapiCfg(rmNetId);
#else
    (VOS_VOID)tetherInfo;
#endif

    return;
}


VOS_INT RNIC_BST_GetModemInfo(const char *netDevName, BST_RNIC_ModemInfo *modemInfo)
{
    RNIC_IFACE_Ctx        *ifaceCtx = VOS_NULL_PTR;
    RNIC_DEV_ID_ENUM_UINT8 rmNetId;

    if (VOS_NULL_PTR == netDevName) {
        return VOS_ERROR;
    }

    if (VOS_NULL_PTR == modemInfo) {
        return VOS_ERROR;
    }

    if (VOS_OK != rnic_get_devid_by_name(netDevName, &rmNetId)) {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_BST_GetModemInfo fail, no such RmnetName!");
        return VOS_ERROR;
    }

    if (rmNetId >= RNIC_DEV_ID_BUTT) {
        return VOS_NULL_PTR;
    }

    ifaceCtx = RNIC_GET_IFACE_CTX_ADR(rmNetId);

    modemInfo->ipv4State = (RNIC_BIT_OPT_FALSE == ifaceCtx->psIfaceInfo.opIpv4Act) ? BST_RNIC_PDP_STATE_INACTIVE :
                                                                                     BST_RNIC_PDP_STATE_ACTIVE;
    modemInfo->ipv6State = (RNIC_BIT_OPT_FALSE == ifaceCtx->psIfaceInfo.opIpv6Act) ? BST_RNIC_PDP_STATE_INACTIVE :
                                                                                     BST_RNIC_PDP_STATE_ACTIVE;

    modemInfo->modemId = ifaceCtx->psIfaceInfo.modemId;
    modemInfo->ipv4RabId = ifaceCtx->psIfaceInfo.ipv4RabId;
    modemInfo->ipv6RabId = ifaceCtx->psIfaceInfo.ipv6RabId;

    return VOS_OK;
}

