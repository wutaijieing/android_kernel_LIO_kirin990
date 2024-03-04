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

/*
 * Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
 */
#define THIS_FILE_ID PS_FILE_ID_ADS_DEBUG_C

#include "AdsDebug.h"
#include "AdsCtx.h"
#include "securec.h"


ADS_StatsInfo      g_adsStats;
ADS_LogLevelUint32 g_adsLogLevel = ADS_LOG_LEVEL_ERROR;


VOS_VOID ADS_SetTxWakeLockTmrLen(VOS_UINT32 value)
{
    g_adsCtx.adsIpfCtx.txWakeLockTmrLen = value;
    return;
}


VOS_VOID ADS_SetRxWakeLockTmrLen(VOS_UINT32 value)
{
    g_adsCtx.adsIpfCtx.rxWakeLockTmrLen = value;
    return;
}


VOS_VOID ADS_SetMaxQueueLength(VOS_UINT32 length, VOS_UINT32 instanceIndex)
{
    g_adsCtx.adsSpecCtx[instanceIndex].adsUlCtx.ulMaxQueueLength = length;
}


VOS_VOID ADS_ShowEntityStats(VOS_VOID)
{
    VOS_UINT32 i;
    VOS_UINT32 j;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++) {
        PS_PRINTF_INFO("ADS Modem ID %d\n", i);

        for (j = ADS_RAB_ID_MIN; j <= ADS_RAB_ID_MAX; j++) {
            if (VOS_NULL_PTR != g_adsCtx.adsSpecCtx[i].adsUlCtx.adsUlQueue[j].adsUlLink) {
                PS_PRINTF_INFO("ADS Queue length is %d\n",
                               IMM_ZcQueueLen(g_adsCtx.adsSpecCtx[i].adsUlCtx.adsUlQueue[j].adsUlLink));
            }

            PS_PRINTF_INFO("ADS DL RabId is %d\n",
                           g_adsCtx.adsSpecCtx[i].adsDlCtx.adsDlRabInfo[j - ADS_RAB_ID_OFFSET].rabId);
#if (FEATURE_ON == FEATURE_DEBUG_PRINT_ADDRESS)
            PS_PRINTF_INFO("ADS DL Rcv Func is %p\n",
                           g_adsCtx.adsSpecCtx[i].adsDlCtx.adsDlRabInfo[j - ADS_RAB_ID_OFFSET].rcvDlDataFunc);
#endif
        }
    }

    PS_PRINTF_INFO("ADS UL is sending flag, SendingFlg = %d\n", g_adsCtx.adsIpfCtx.sendingFlg);

    return;
}


VOS_VOID ADS_ShowEventProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS EVENT PROC STATS START:              \n");
    PS_PRINTF_INFO("ULQueNonEmptyTrigEvent           %10u\n", g_adsStats.ulComStatsInfo.ulQueNonEmptyTrigEvent);
    PS_PRINTF_INFO("ULQueFullTrigEvent               %10u\n", g_adsStats.ulComStatsInfo.ulQueFullTrigEvent);
    PS_PRINTF_INFO("ULQueHitThresTrigEvent           %10u\n", g_adsStats.ulComStatsInfo.ulQueHitThresTrigEvent);
    PS_PRINTF_INFO("ULTmrHitThresTrigEvent           %10u\n", g_adsStats.ulComStatsInfo.ulTmrHitThresTrigEvent);
    PS_PRINTF_INFO("UL10MsTmrTrigEvent               %10u\n", g_adsStats.ulComStatsInfo.uL10MsTmrTrigEvent);
    PS_PRINTF_INFO("ULSpeIntTrigEvent                %10u\n", g_adsStats.ulComStatsInfo.ulSpeIntTrigEvent);
    PS_PRINTF_INFO("ULProcEventNum                   %10u\n", g_adsStats.ulComStatsInfo.ulProcEventNum);
    PS_PRINTF_INFO("DLRcvIpfRdIntNum                 %10u\n", g_adsStats.dlComStatsInfo.dlRcvIpfRdIntNum);
    PS_PRINTF_INFO("DLCCoreResetTrigRdEvent          %10u\n", g_adsStats.dlComStatsInfo.dlcCoreResetTrigEvent);
    PS_PRINTF_INFO("DLProcIpfRdEventNum              %10u\n", g_adsStats.dlComStatsInfo.dlProcIpfRdEventNum);
    PS_PRINTF_INFO("DLRcvIpfAdqEmptyIntNum           %10u\n", g_adsStats.dlComStatsInfo.dlRcvIpfAdqEmptyIntNum);
    PS_PRINTF_INFO("DLRecycleMemTrigAdEvent          %10u\n", g_adsStats.dlComStatsInfo.dlRecycleMemTrigEvent);
    PS_PRINTF_INFO("DLProcIpfAdEventNum              %10u\n", g_adsStats.dlComStatsInfo.dlProcIpfAdEventNum);
    PS_PRINTF_INFO("ADS EVENT PROC STATS END.                \n");

    return;
}


VOS_VOID ADS_ShowULPktProcStats(VOS_VOID)
{
    VOS_UINT32 instance;

    PS_PRINTF_INFO("ADS UL PKT PROC STATS START:             \n");
    PS_PRINTF_INFO("ULRmnetRxPktNum                  %10u\n", g_adsStats.ulComStatsInfo.ulRmnetRxPktNum);
    PS_PRINTF_INFO("ULRmnetModemIdErrNum             %10u\n", g_adsStats.ulComStatsInfo.ulRmnetModemIdErrNum);
    PS_PRINTF_INFO("ULRmnetRabIdErrNum               %10u\n", g_adsStats.ulComStatsInfo.ulRmnetRabIdErrNum);
    PS_PRINTF_INFO("ULRmnetEnQueSuccNum              %10u\n", g_adsStats.ulComStatsInfo.ulRmnetEnQueSuccNum);
    PS_PRINTF_INFO("ULRmnetEnQueFailNum              %10u\n", g_adsStats.ulComStatsInfo.ulRmnetEnQueFailNum);
    PS_PRINTF_INFO("ULPktEnQueSuccNum                %10u\n", g_adsStats.ulComStatsInfo.ulPktEnQueSuccNum);
    PS_PRINTF_INFO("ULPktEnQueFailNum                %10u\n", g_adsStats.ulComStatsInfo.ulPktEnQueFailNum);

    for (instance = 0; instance < ADS_INSTANCE_MAX_NUM; instance++) {
        PS_PRINTF_INFO("ULBuffPktNum[MDOEM:%d]            %10u\n", instance,
                       ADS_UL_GetInstanceAllQueueDataNum(instance));
    }

    PS_PRINTF_INFO("ULBuffThresholdCurrent           %10u\n", g_adsCtx.adsIpfCtx.thredHoldNum);
    PS_PRINTF_INFO("ULBuffThreshold1                 %10u\n",
                   g_adsCtx.adsIpfCtx.ulAssemParmInfo.thresholdLevel.threshold1);
    PS_PRINTF_INFO("ULBuffThreshold2                 %10u\n",
                   g_adsCtx.adsIpfCtx.ulAssemParmInfo.thresholdLevel.threshold1);
    PS_PRINTF_INFO("ULBuffThreshold3                 %10u\n",
                   g_adsCtx.adsIpfCtx.ulAssemParmInfo.thresholdLevel.threshold1);
    PS_PRINTF_INFO("ULBuffThreshold4                 %10u\n",
                   g_adsCtx.adsIpfCtx.ulAssemParmInfo.thresholdLevel.threshold1);
    PS_PRINTF_INFO("ULWaterLevel1HitNum              %10u\n", g_adsStats.ulComStatsInfo.ulWmLevel1HitNum);
    PS_PRINTF_INFO("ULWaterLevel2HitNum              %10u\n", g_adsStats.ulComStatsInfo.ulWmLevel2HitNum);
    PS_PRINTF_INFO("ULWaterLevel3HitNum              %10u\n", g_adsStats.ulComStatsInfo.ulWmLevel3HitNum);
    PS_PRINTF_INFO("ULWaterLevel4HitNum              %10u\n", g_adsStats.ulComStatsInfo.ulWmLevel4HitNum);
    PS_PRINTF_INFO("ADS UL PKT PROC STATS END.               \n");

    return;
}


VOS_VOID ADS_ShowULBdProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS UL BD PROC STATS START:              \n");
    PS_PRINTF_INFO("ULBdqCfgIpfHaveNoBd              %10u\n", g_adsStats.ulComStatsInfo.ulBdqCfgIpfHaveNoBd);
    PS_PRINTF_INFO("ULBdqCfgBdSuccNum                %10u\n", g_adsStats.ulComStatsInfo.ulBdqCfgBdSuccNum);
    PS_PRINTF_INFO("ULBdqCfgBdFailNum                %10u\n", g_adsStats.ulComStatsInfo.ulBdqCfgBdFailNum);
    PS_PRINTF_INFO("ULBdqCfgIpfSuccNum               %10u\n", g_adsStats.ulComStatsInfo.ulBdqCfgIpfSuccNum);
    PS_PRINTF_INFO("ULBdqCfgIpfFailNum               %10u\n", g_adsStats.ulComStatsInfo.ulBdqCfgIpfFailNum);
    PS_PRINTF_INFO("ULBdqSaveSrcMemNum               %10u\n", g_adsStats.ulComStatsInfo.ulBdqSaveSrcMemNum);
    PS_PRINTF_INFO("ULBdqFreeSrcMemNum               %10u\n", g_adsStats.ulComStatsInfo.ulBdqFreeSrcMemNum);
    PS_PRINTF_INFO("ULBdqFreeSrcMemErr               %10u\n", g_adsStats.ulComStatsInfo.ulBdqFreeSrcMemErr);
    PS_PRINTF_INFO("ADS UL BD PROC STATS END.                \n");

    return;
}


VOS_VOID ADS_ShowDLPktProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS DL PKT PROC STATS START:             \n");
    PS_PRINTF_INFO("DLRmnetTxPktNum                  %10u\n", g_adsStats.dlComStatsInfo.dlRmnetTxPktNum);
    PS_PRINTF_INFO("DLRmnetModemIdErrNum             %10u\n", g_adsStats.dlComStatsInfo.dlRmnetModemIdErrNum);
    PS_PRINTF_INFO("DLRmnetRabIdErrNum               %10u\n", g_adsStats.dlComStatsInfo.dlRmnetRabIdErrNum);
    PS_PRINTF_INFO("DLRmnetNoFuncFreePktNum          %10u\n", g_adsStats.dlComStatsInfo.dlRmnetNoFuncFreePktNum);
    PS_PRINTF_INFO("ADS DL PKT PROC STATS END.               \n");

    return;
}


VOS_VOID ADS_ShowDLRdProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS DL RD PROC STATS START:              \n");
    PS_PRINTF_INFO("DLRdqRxRdNum                     %10u\n", g_adsStats.dlComStatsInfo.dlRdqRxRdNum);
    PS_PRINTF_INFO("DLRdqGetRd0Num                   %10u\n", g_adsStats.dlComStatsInfo.dlRdqGetRd0Num);
    PS_PRINTF_INFO("DLRdqTransMemFailNum             %10u\n", g_adsStats.dlComStatsInfo.dlRdqTransMemFailNum);
    PS_PRINTF_INFO("DLRdqRxNormPktNum                %10u\n", g_adsStats.dlComStatsInfo.dlRdqRxNormPktNum);
    PS_PRINTF_INFO("DLRdqRxNdPktNum                  %10u\n", g_adsStats.dlComStatsInfo.dlRdqRxNdPktNum);
    PS_PRINTF_INFO("DLRdqRxDhcpPktNum                %10u\n", g_adsStats.dlComStatsInfo.dlRdqRxDhcpPktNum);
    PS_PRINTF_INFO("DLRdqRxErrPktNum                 %10u\n", g_adsStats.dlComStatsInfo.dlRdqRxErrPktNum);
    PS_PRINTF_INFO("DLRdqFilterErrNum                %10u\n", g_adsStats.dlComStatsInfo.dlRdqFilterErrNum);
    PS_PRINTF_INFO("ADS DL RD PROC STATS END.                \n");

    return;
}


VOS_VOID ADS_ShowDLAdProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS DL ADQ PROC STATS START:             \n");
    PS_PRINTF_INFO("DLAdqAllocSysMemSuccNum          %10u\n", g_adsStats.dlComStatsInfo.dlAdqAllocSysMemSuccNum);
    PS_PRINTF_INFO("DLAdqAllocSysMemFailNum          %10u\n", g_adsStats.dlComStatsInfo.dlAdqAllocSysMemFailNum);
    PS_PRINTF_INFO("DLAdqAllocMemSuccNum             %10u\n", g_adsStats.dlComStatsInfo.dlAdqAllocMemSuccNum);
    PS_PRINTF_INFO("DLAdqAllocMemFailNum             %10u\n", g_adsStats.dlComStatsInfo.dlAdqAllocMemFailNum);
    PS_PRINTF_INFO("DLAdqFreeMemNum                  %10u\n", g_adsStats.dlComStatsInfo.dlAdqFreeMemNum);
    PS_PRINTF_INFO("DLAdqRecycleMemSuccNum           %10u\n", g_adsStats.dlComStatsInfo.dlAdqRecycleMemSuccNum);
    PS_PRINTF_INFO("DLAdqRecycleMemFailNum           %10u\n", g_adsStats.dlComStatsInfo.dlAdqRecycleMemFailNum);
    PS_PRINTF_INFO("DLAdqGetFreeAdSuccNum            %10u\n", g_adsStats.dlComStatsInfo.dlAdqGetFreeAdSuccNum);
    PS_PRINTF_INFO("DLAdqGetFreeAdFailNum            %10u\n", g_adsStats.dlComStatsInfo.dlAdqGetFreeAdFailNum);
    PS_PRINTF_INFO("DLAdqCfgAdNum                    %10u\n", g_adsStats.dlComStatsInfo.dlAdqCfgAdNum);
    PS_PRINTF_INFO("DLAdqCfgAd0Num                   %10u\n", g_adsStats.dlComStatsInfo.dlAdqCfgAd0Num);
    PS_PRINTF_INFO("DLAdqCfgAd1Num                   %10u\n", g_adsStats.dlComStatsInfo.dlAdqCfgAd1Num);
    PS_PRINTF_INFO("DLAdqFreeAd0Num                  %10u\n", g_adsStats.dlComStatsInfo.dlAdqFreeAd0Num);
    PS_PRINTF_INFO("DLAdqFreeAd1Num                  %10u\n", g_adsStats.dlComStatsInfo.dlAdqFreeAd1Num);
    PS_PRINTF_INFO("DLAdqCfgIpfSuccNum               %10u\n", g_adsStats.dlComStatsInfo.dlAdqCfgIpfSuccNum);
    PS_PRINTF_INFO("DLAdqCfgIpfFailNum               %10u\n", g_adsStats.dlComStatsInfo.dlAdqCfgIpfFailNum);
    PS_PRINTF_INFO("DLAdqStartEmptyTmrNum            %10u\n", g_adsStats.dlComStatsInfo.dlAdqStartEmptyTmrNum);
    PS_PRINTF_INFO("DLAdqEmptyTmrTimeoutNum          %10u\n", g_adsStats.dlComStatsInfo.dlAdqEmptyTmrTimeoutNum);
    PS_PRINTF_INFO("DLAdqRcvAd0EmptyIntNum           %10u\n", g_adsStats.dlComStatsInfo.dlAdqRcvAd0EmptyIntNum);
    PS_PRINTF_INFO("DLAdqRcvAd1EmptyIntNum           %10u\n", g_adsStats.dlComStatsInfo.dlAdqRcvAd1EmptyIntNum);
    PS_PRINTF_INFO("ADS DL ADQ PROC STATS END.               \n");

    return;
}


VOS_VOID ADS_ShowResetProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS RESET PROC STATS START:             \n");
    PS_PRINTF_INFO("ULResetSem                       %ld\n", g_adsCtx.hULResetSem);
    PS_PRINTF_INFO("ULResetCreateSemFailNum          %10u\n", g_adsStats.resetStatsInfo.ulResetCreateSemFailNum);
    PS_PRINTF_INFO("ULResetLockFailNum               %10u\n", g_adsStats.resetStatsInfo.ulResetLockFailNum);
    PS_PRINTF_INFO("ULResetSuccNum                   %10u\n", g_adsStats.resetStatsInfo.ulResetSuccNum);
    PS_PRINTF_INFO("DLResetSem                       %ld\n", g_adsCtx.hDLResetSem);
    PS_PRINTF_INFO("DLResetCreateSemFailNum          %10u\n", g_adsStats.resetStatsInfo.dlResetCreateSemFailNum);
    PS_PRINTF_INFO("DLResetLockFailNum               %10u\n", g_adsStats.resetStatsInfo.dlResetLockFailNum);
    PS_PRINTF_INFO("DLResetSuccNum                   %10u\n", g_adsStats.resetStatsInfo.dlResetSuccNum);
    PS_PRINTF_INFO("ADS RESET PROC STATS END.                \n");

    return;
}


VOS_VOID ADS_ShowDLMemStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS DL MEM STATS START:                   \n");
    PS_PRINTF_INFO("DLUsingAd0Num                     %10u\n",
                   (g_adsStats.dlComStatsInfo.dlAdqCfgAd0Num - g_adsStats.dlComStatsInfo.dlAdqFreeAd0Num));
    PS_PRINTF_INFO("DLUsingAd1Num                     %10u\n",
                   (g_adsStats.dlComStatsInfo.dlAdqCfgAd1Num - g_adsStats.dlComStatsInfo.dlAdqFreeAd1Num));
    PS_PRINTF_INFO("DLRdqRxRdNum                      %10u\n", g_adsStats.dlComStatsInfo.dlRdqRxRdNum);
    PS_PRINTF_INFO("DLRmnetNoFuncFreePktNum           %10u\n", g_adsStats.dlComStatsInfo.dlRmnetNoFuncFreePktNum);
    PS_PRINTF_INFO("DLGetIpfAd0FailedNum              %10u\n", g_adsStats.dlComStatsInfo.dlAdqGetIpfAd0FailNum);
    PS_PRINTF_INFO("DLGetIpfAd1FailedNum              %10u\n", g_adsStats.dlComStatsInfo.dlAdqGetIpfAd1FailNum);
    PS_PRINTF_INFO("ADS DL MEM STATS END.                     \n");

    return;
}


VOS_VOID ADS_ShowDLInfoStats(VOS_VOID)
{
    ADS_ShowEventProcStats();
    ADS_ShowDLPktProcStats();
    ADS_ShowDLAdProcStats();
    ADS_ShowDLRdProcStats();
    ADS_ShowDLMemStats();

    return;
}

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)

VOS_VOID ADS_ShowPc5PktProcStats(VOS_VOID)
{
    PS_PRINTF_INFO("ADS PC5 PKT PROC STATS START:            \n");
    PS_PRINTF_INFO("ulUlRxPc5PktNum                  %10u\n", g_adsStats.ulComStatsInfo.ulRxPc5PktNum);
    PS_PRINTF_INFO("ulUlPc5PktEnQueSuccNum           %10u\n", g_adsStats.ulComStatsInfo.ulPc5PktEnQueSuccNum);
    PS_PRINTF_INFO("ulUlPc5PktEnQueFailNum           %10u\n", g_adsStats.ulComStatsInfo.ulPc5PktEnQueFailNum);
    PS_PRINTF_INFO("ulULPc5QueNonEmptyTrigEvent      %10u\n", g_adsStats.ulComStatsInfo.ulPc5QueNonEmptyTrigEvent);
    PS_PRINTF_INFO("ulULPc5BdqCfgBdSuccNum           %10u\n", g_adsStats.ulComStatsInfo.ulPc5BdqCfgBdSuccNum);
    PS_PRINTF_INFO("ulDLRdqRxPc5PktNum               %10u\n", g_adsStats.dlComStatsInfo.dLRdqRxPc5PktNum);
    PS_PRINTF_INFO("ulDLRdqTxPc5PktNum               %10u\n", g_adsStats.dlComStatsInfo.dLRdqTxPc5PktNum);
    PS_PRINTF_INFO("ulDLPc5FragEnqueNum              %10u\n", g_adsStats.dlComStatsInfo.dLPc5FragEnqueNum);
    PS_PRINTF_INFO("ulDLPc5FragFreeNum               %10u\n", g_adsStats.dlComStatsInfo.dLPc5FragFreeNum);
    PS_PRINTF_INFO("ulDLPc5FragReleaseNum            %10u\n", g_adsStats.dlComStatsInfo.dLPc5FragReleaseNum);
    PS_PRINTF_INFO("ulDLPc5FragAssemSuccNum          %10u\n", g_adsStats.dlComStatsInfo.dLPc5FragAssemSuccNum);
    PS_PRINTF_INFO("pc5TxPktLen                      %10u\n", g_adsStats.ulComStatsInfo.pc5TxPktLen);
    PS_PRINTF_INFO("pc5RxPktLen                      %10u\n", g_adsStats.dlComStatsInfo.pc5RxPktLen);
    PS_PRINTF_INFO("ADS PC5 PKT PROC STATS END.              \n");

    return;
}


VOS_VOID ADS_SetPc5MaxQueueLen(VOS_UINT32 length)
{
    g_adsCtx.adsPc5Ctx.ulQueueMaxLen = length;
}
#endif


VOS_VOID ADS_Help(VOS_VOID)
{
    PS_PRINTF_INFO("ADS DEBUG ENTRY                    \n");
    PS_PRINTF_INFO("<ADS_ShowEntityStats>          \n");
    PS_PRINTF_INFO("<ADS_ShowEventProcStats>       \n");
    PS_PRINTF_INFO("<ADS_ShowULPktProcStats>       \n");
    PS_PRINTF_INFO("<ADS_ShowULBdProcStats>        \n");
    PS_PRINTF_INFO("<ADS_ShowDLInfoStats>          \n");
    PS_PRINTF_INFO("<ADS_ShowDLPktProcStats>       \n");
    PS_PRINTF_INFO("<ADS_ShowDLRdProcStats>        \n");
    PS_PRINTF_INFO("<ADS_ShowDLAdProcStats>        \n");
    PS_PRINTF_INFO("<ADS_ShowDLMemStats>           \n");
    PS_PRINTF_INFO("<ADS_ShowResetProcStats>       \n");
    PS_PRINTF_INFO("<ADS_ShowFeatureState>         \n");
    PS_PRINTF_INFO("<ADS_ResetDebugInfo>           \n");
#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
    PS_PRINTF_INFO("<ADS_ShowPc5PktProcStats>      \n");
#endif

    return;
}


VOS_VOID ADS_ResetDebugInfo(VOS_VOID)
{
    memset_s(&g_adsStats.ulComStatsInfo, sizeof(g_adsStats.ulComStatsInfo), 0x00, sizeof(g_adsStats.ulComStatsInfo));
    memset_s(&g_adsStats.dlComStatsInfo, sizeof(g_adsStats.dlComStatsInfo), 0x00, sizeof(g_adsStats.dlComStatsInfo));
    return;
}


VOS_VOID ADS_SetFlowDebugFlag(VOS_UINT32 flowDebugFlag)
{
    switch (flowDebugFlag) {
        case ADS_FLOW_DEBUG_DL_ON:
            g_adsStats.dlComStatsInfo.dlFlowDebugFlag    = PS_TRUE;
            g_adsStats.ulComStatsInfo.ulFlowDebugFlag    = PS_FALSE;
            g_adsStats.dlComStatsInfo.dlFlowRptThreshold = ADS_FLOW_DL_DEFAULT_RPT_THRESHOLD;
            break;

        case ADS_FLOW_DEBUG_UL_ON:
            g_adsStats.dlComStatsInfo.dlFlowDebugFlag    = PS_FALSE;
            g_adsStats.ulComStatsInfo.ulFlowDebugFlag    = PS_TRUE;
            g_adsStats.ulComStatsInfo.ulFlowRptThreshold = ADS_FLOW_UL_DEFAULT_RPT_THRESHOLD;
            break;

        case ADS_FLOW_DEBUG_ALL_ON:
            g_adsStats.dlComStatsInfo.dlFlowDebugFlag    = PS_TRUE;
            g_adsStats.ulComStatsInfo.ulFlowDebugFlag    = PS_TRUE;
            g_adsStats.dlComStatsInfo.dlFlowRptThreshold = ADS_FLOW_DL_DEFAULT_RPT_THRESHOLD;
            g_adsStats.ulComStatsInfo.ulFlowRptThreshold = ADS_FLOW_UL_DEFAULT_RPT_THRESHOLD;
            break;

        default:
            g_adsStats.dlComStatsInfo.dlFlowDebugFlag = PS_FALSE;
            g_adsStats.ulComStatsInfo.ulFlowDebugFlag = PS_FALSE;
            break;
    }

    return;
}


VOS_VOID ADS_SetFlowDLRptThreshold(VOS_UINT32 flowDLRptThreshold)
{
    g_adsStats.dlComStatsInfo.dlFlowRptThreshold = flowDLRptThreshold;
    return;
}


VOS_VOID ADS_SetFlowULRptThreshold(VOS_UINT32 flowULRptThreshold)
{
    g_adsStats.ulComStatsInfo.ulFlowRptThreshold = flowULRptThreshold;
    return;
}


VOS_VOID ADS_SetDlDiscardPktFlag(VOS_UINT32 value)
{
    g_adsDlDiscardPktFlag = value;
    return;
}


VOS_VOID ADS_DLFlowAdd(VOS_UINT32 sduLen)
{
    if (PS_TRUE == g_adsStats.dlComStatsInfo.dlFlowDebugFlag) {
        /* ����ͳ�� */
        g_adsStats.dlComStatsInfo.dlFlowInfo += sduLen;

        /* ����ͳ���ϱ� */
        if (g_adsStats.dlComStatsInfo.dlFlowInfo >= g_adsStats.dlComStatsInfo.dlFlowRptThreshold) {
            g_adsStats.dlComStatsInfo.dlEndSlice = VOS_GetSlice();

            PS_PRINTF_INFO("DL Flow Info = %10d, Pkt Num = %10d, Slice = %10d, Time = %10d\n",
                           g_adsStats.dlComStatsInfo.dlFlowInfo, g_adsStats.dlComStatsInfo.dlRmnetTxPktNum,
                           g_adsStats.dlComStatsInfo.dlEndSlice,
                           (g_adsStats.dlComStatsInfo.dlEndSlice - g_adsStats.dlComStatsInfo.dlStartSlice));

            g_adsStats.dlComStatsInfo.dlStartSlice = g_adsStats.dlComStatsInfo.dlEndSlice;
            g_adsStats.dlComStatsInfo.dlFlowInfo   = 0;
        }
    }

    return;
}


VOS_VOID ADS_ULFlowAdd(VOS_UINT32 sduLen)
{
    if (PS_TRUE == g_adsStats.ulComStatsInfo.ulFlowDebugFlag) {
        /* ����ͳ�� */
        g_adsStats.ulComStatsInfo.ulFlowInfo += sduLen;

        /* ����ͳ���ϱ� */
        if (g_adsStats.ulComStatsInfo.ulFlowInfo >= g_adsStats.ulComStatsInfo.ulFlowRptThreshold) {
            g_adsStats.ulComStatsInfo.ulEndSlice = VOS_GetSlice();

            PS_PRINTF_INFO("UL Flow Info = %10d, Pkt Num = %10d, Slice = %10d, Time = %10d\n",
                           g_adsStats.ulComStatsInfo.ulFlowInfo, g_adsStats.ulComStatsInfo.ulRmnetRxPktNum,
                           g_adsStats.ulComStatsInfo.ulEndSlice,
                           (g_adsStats.ulComStatsInfo.ulEndSlice - g_adsStats.ulComStatsInfo.ulStartSlice));

            g_adsStats.ulComStatsInfo.ulStartSlice = g_adsStats.ulComStatsInfo.ulEndSlice;
            g_adsStats.ulComStatsInfo.ulFlowInfo   = 0;
        }
    }

    return;
}


VOS_VOID ADS_LogPrintf(ADS_LogLevelUint32 level, VOS_CHAR *pcFmt, ...)
{
    VOS_CHAR   acBuff[ADS_LOG_BUFF_LEN] = {0};
    va_list    argList;
    VOS_UINT32 printLength = 0;

    /* ��ӡ������� */
    if (level < g_adsLogLevel) {
        return;
    }

    /*lint -e713 -e507 -e530*/
    va_start(argList, pcFmt);
    printLength += vsnprintf_s(acBuff, ADS_LOG_BUFF_LEN, ADS_LOG_BUFF_LEN, pcFmt, argList);
    va_end(argList);
    /*lint +e713 +e507 +e530*/

    if (printLength > (ADS_LOG_BUFF_LEN - 1)) {
        printLength = ADS_LOG_BUFF_LEN - 1;
    }

    acBuff[printLength] = '\0';

    /* Ŀǰֻ��ERR��ӡ�ĵ��ã��Ժ�����ٽ�����չ */
    ADS_PR_LOGH("%s", acBuff);

    return;
}

