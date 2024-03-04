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

#include <dms.h>
#include "dms_core.h"
#include "dms_debug.h"
#include "dms_port.h"
#include "dms_dev.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */

#define THIS_FILE_ID PS_FILE_ID_DMS_DEBUG_C

DMS_DebugInfoTbl    g_dmsSdmInfoTable[DMS_SDM_DBG_INFO_MAX];
DMS_PortDebugCfg    g_dmsPortDebugCfg;

struct DMS_DebugStats g_dmsStatsInfo;


VOS_BOOL DMS_TestBit(VOS_UINT64 bitMaskValue, VOS_UINT32 bitNum)
{
    return (VOS_BOOL)(1ULL & (bitMaskValue >> bitNum));
}

DMS_DebugInfoTbl* DMS_GetDebugInfo(VOS_VOID)
{
    return &(g_dmsSdmInfoTable[0]);
}

VOS_VOID DMS_Debug(DMS_SDM_MsgId type, VOS_UINT32 rserved1, VOS_UINT32 rserved2, VOS_UINT32 rserved3)
{
    g_dmsSdmInfoTable[type].calledNum += 1;
    g_dmsSdmInfoTable[type].rserved1 = rserved1;
    g_dmsSdmInfoTable[type].rserved2 = rserved2;
    g_dmsSdmInfoTable[type].rserved3 = rserved3;
    g_dmsSdmInfoTable[type].rtcTime  = VOS_GetTick();
    return;
}


VOS_VOID DMS_SendPortDebugNvCfg(VOS_UINT32 debugLevel, VOS_UINT32 portIdMask1, VOS_UINT32 portIdMask2)
{
    g_dmsPortDebugCfg.debugLevel = debugLevel;
    g_dmsPortDebugCfg.portIdMask = portIdMask2;
    g_dmsPortDebugCfg.portIdMask = (g_dmsPortDebugCfg.portIdMask << 32) | portIdMask1;
}

#if (FEATURE_ON == FEATURE_DEBUG_PRINT_ADDRESS)
VOS_VOID DMS_ShowDebugInfo(VOS_VOID)
{
    DMS_MainInfo *mainInfo = DMS_GetMainInfo();

    PS_PRINTF_INFO("<DMS_ShowDebugInfo> g_acmReadData = %p\n", g_acmReadData);
    PS_PRINTF_INFO("<DMS_ShowDebugInfo> pstMainInfo->pfnRdDataCallback = %p\n", mainInfo->pfnRdDataCallback);
    return;
}
#endif



VOS_VOID DMS_ShowPortInfo(DMS_PortIdUint16 portId)
{
    struct DMS_PortInfo *portInfo = DMS_PORT_GetPortInfo(portId);

    PS_PRINTF_INFO("<DMS_ShowPortInfo> portId:%d portType:%d clientType:%d clientId:%d \n", portInfo->portId,
                   portInfo->portType, portInfo->clientInfo.clientType, portInfo->clientInfo.clientId);

    if (portInfo->clientInfo.clientCBTbl == VOS_NULL_PTR) {
        PS_PRINTF_INFO("<DMS_ShowPortInfo> %d clientFuncTbl is null\n", portId);
    }

    if (portInfo->devInfo == VOS_NULL_PTR) {
        PS_PRINTF_INFO("<DMS_ShowPortInfo> %d devInfo is null\n", portId);
    }

    return;
}



VOS_VOID DMS_ShowDevInfo(DMS_PortIdUint16 portId)
{
    struct DMS_DevInfo *devInfo = VOS_NULL_PTR;

    if (portId >= DMS_PORT_BUTT) {
        PS_PRINTF_INFO("<DMS_ShowDevInfo> portId:%d is invalid\n", portId);
        return;
    }

    devInfo = DMS_DEV_GetDevEntityByPortId(portId);

    PS_PRINTF_INFO("<DMS_ShowDevInfo> handle:%d portId:%d devId:%d\n", devInfo->handle, devInfo->portId,
                   devInfo->devId);

    if (devInfo->portFuncTbl == VOS_NULL_PTR) {
        PS_PRINTF_INFO("<DMS_ShowDevInfo> %d portFuncTbl is null\n", portId);
    }

    if (devInfo->ops == VOS_NULL_PTR) {
        PS_PRINTF_INFO("<DMS_ShowDevInfo> %d ops is null\n", portId);
    }

    return;
}

VOS_VOID DMS_ShowVcomStats(VOS_VOID)
{
    DMS_DebugInfoTbl *table;

    /* 如果某个成员在某个类别没有使用可以不打印 */

    table = DMS_GetDebugInfo();

    PS_PRINTF_INFO("ENABLE             \n");
    PS_PRINTF_INFO("USB ENABLE counts  : %d \n USB ENABLE fail counts  : %d \n USB ENABLE fail branch  : %d \n",
                   table[DMS_SDM_USB_ENABLE].calledNum, table[DMS_SDM_USB_ENABLE_ERR].calledNum,
                   table[DMS_SDM_USB_ENABLE_ERR].rserved3);
    PS_PRINTF_INFO("DISABLE             \n");
    PS_PRINTF_INFO("USB DISABLE counts  : %d \n USB DISABLE fail counts  : %d \n USB DISABLE fail branch  : %d \n",
                   table[DMS_SDM_USB_DISABLE].calledNum, table[DMS_SDM_USB_DISABLE_ERR].calledNum,
                   table[DMS_SDM_USB_DISABLE_ERR].rserved3);

    PS_PRINTF_INFO("OPEN             \n");
    PS_PRINTF_INFO(
        "VCOM_PCUI OPEN counts  : %d \n VCOM_PCUI OPEN fail counts  : %d \n  VCOM_PCUI OPEN fail value : %d\n",
        table[DMS_SDM_VCOM_OPEN_PCUI].calledNum, table[DMS_SDM_VCOM_OPEN_ERR_PCUI].calledNum,
        table[DMS_SDM_VCOM_OPEN_ERR_PCUI].rserved1);
    PS_PRINTF_INFO(
        "VCOM_CTRL OPEN counts  : %d \n VCOM_CTRL OPEN fail counts  : %d \n  VCOM_CTRL OPEN fail value : %d\n",
        table[DMS_SDM_VCOM_OPEN_CTRL].calledNum, table[DMS_SDM_VCOM_OPEN_ERR_CTRL].calledNum,
        table[DMS_SDM_VCOM_OPEN_ERR_CTRL].rserved1);
    PS_PRINTF_INFO(
        "VCOM_PCUI2 OPEN counts  : %d \n VCOM_PCUI2 OPEN fail counts  : %d \n  VCOM_PCUI2 OPEN fail value : %d\n",
        table[DMS_SDM_VCOM_OPEN_PCUI2].calledNum, table[DMS_SDM_VCOM_OPEN_ERR_PCUI2].calledNum,
        table[DMS_SDM_VCOM_OPEN_ERR_PCUI2].rserved1);

    PS_PRINTF_INFO("CLOSE             \n");
    PS_PRINTF_INFO("VCOM_AT CLOSE counts  : %d \n VCOM_AT CLOSE fail counts  : %d \n  VCOM_AT CLOSE fail value : %d\n",
                   table[DMS_SDM_VCOM_CLOSE_PCUI].calledNum, table[DMS_SDM_VCOM_CLOSE_ERR_PCUI].calledNum,
                   table[DMS_SDM_VCOM_CLOSE_ERR_PCUI].rserved1);
    PS_PRINTF_INFO(
        "VCOM_CTRL CLOSE counts  : %d \n VCOM_CTRL CLOSE fail counts  : %d \n  VCOM_CTRL CLOSE fail value : %d\n",
        table[DMS_SDM_VCOM_CLOSE_CTRL].calledNum, table[DMS_SDM_VCOM_CLOSE_ERR_CTRL].calledNum,
        table[DMS_SDM_VCOM_CLOSE_ERR_CTRL].rserved1);
    PS_PRINTF_INFO(
        "VCOM_PCUI2 CLOSE counts  : %d \n VCOM_PCUI2 CLOSE fail counts  : %d \n  VCOM_PCUI2 CLOSE fail value : %d\n",
        table[DMS_SDM_VCOM_CLOSE_PCUI2].calledNum, table[DMS_SDM_VCOM_CLOSE_ERR_PCUI2].calledNum,
        table[DMS_SDM_VCOM_CLOSE_ERR_PCUI2].rserved1);

    PS_PRINTF_INFO("WRITE_ASYNCHRONOUS           \n");
    PS_PRINTF_INFO("VCOM_AT write counts  : %d \n VCOM_AT write OK counts  : %d \n  VCOM_AT Last write OK len : %d\n",
                   table[DMS_SDM_VCOM_WRT_PCUI].calledNum, table[DMS_SDM_VCOM_WRT_SUSS_PCUI].calledNum,
                   table[DMS_SDM_VCOM_WRT_SUSS_PCUI].rserved1);
    PS_PRINTF_INFO(
        "VCOM_CTRL write counts  : %d \n VCOM_CTRL write OK counts  : %d \n  VCOM_CTRL Last write OK len : %d\n",
        table[DMS_SDM_VCOM_WRT_CTRL].calledNum, table[DMS_SDM_VCOM_WRT_SUSS_CTRL].calledNum,
        table[DMS_SDM_VCOM_WRT_SUSS_CTRL].rserved1);
    PS_PRINTF_INFO(
        "VCOM_PCUI2 write counts  : %d \n VCOM_PCUI2 write OK counts  : %d \n  VCOM_PCUI2 Last write OK len : %d\n",
        table[DMS_SDM_VCOM_WRT_PCUI2].calledNum, table[DMS_SDM_VCOM_WRT_SUSS_PCUI2].calledNum,
        table[DMS_SDM_VCOM_WRT_SUSS_PCUI2].rserved1);

    PS_PRINTF_INFO("WRITE CALL BACK              \n");
    PS_PRINTF_INFO(
        "VCOM_AT WRITE CALL BACK counts  : %d \n VCOM_AT WRITE CALL BACK fail counts  : %d \n  VCOM_AT VCOM_AT fail addr : %d\nVCOM2 WRITE fail len :%d\n",
        table[DMS_SDM_VCOM_WRT_CB_PCUI].calledNum, table[DMS_SDM_VCOM_WRT_CB_ERR_PCUI].calledNum,
        table[DMS_SDM_VCOM_WRT_CB_ERR_PCUI].rserved1, table[DMS_SDM_VCOM_WRT_CB_ERR_PCUI].rserved2);
    PS_PRINTF_INFO(
        "VCOM_CTRL WRITE CALL BACK counts  : %d \n VCOM_CTRL WRITE CALL BACK fail counts  : %d \n  VCOM_CTRL VCOM_AT fail addr : %d\nVCOM2 WRITE fail len :%d\n",
        table[DMS_SDM_VCOM_WRT_CB_CTRL].calledNum, table[DMS_SDM_VCOM_WRT_CB_ERR_CTRL].calledNum,
        table[DMS_SDM_VCOM_WRT_CB_ERR_CTRL].rserved1, table[DMS_SDM_VCOM_WRT_CB_ERR_CTRL].rserved2);
    PS_PRINTF_INFO(
        "VCOM_PCUI2 WRITE CALL BACK counts  : %d \n VCOM_PCUI2 WRITE CALL BACK fail counts  : %d \n  VCOM_PCUI2 VCOM_AT fail addr : %d\nVCOM2 WRITE fail len :%d\n",
        table[DMS_SDM_VCOM_WRT_CB_PCUI2].calledNum, table[DMS_SDM_VCOM_WRT_CB_ERR_PCUI2].calledNum,
        table[DMS_SDM_VCOM_WRT_CB_ERR_PCUI2].rserved1, table[DMS_SDM_VCOM_WRT_CB_ERR_PCUI2].rserved2);

    PS_PRINTF_INFO("EVT CALL BACK           \n");
    PS_PRINTF_INFO("VCOM_PCUI EVT CALL BACK counts  : %d \n VCOM_PCUI EVT  type : %d\n",
                   table[DMS_SDM_VCOM_EVT_PCUI].calledNum, table[DMS_SDM_VCOM_EVT_PCUI].rserved1);
    PS_PRINTF_INFO("VCOM_CTRL EVT CALL BACK counts  : %d \n VCOM_CTRL EVT  type : %d\n",
                   table[DMS_SDM_VCOM_EVT_CTRL].calledNum, table[DMS_SDM_VCOM_EVT_CTRL].rserved1);
    PS_PRINTF_INFO("VCOM_PCUI2 EVT CALL BACK counts  : %d \n VCOM_PCUI2 EVT  type : %d\n",
                   table[DMS_SDM_VCOM_EVT_PCUI2].calledNum, table[DMS_SDM_VCOM_EVT_PCUI2].rserved1);

    PS_PRINTF_INFO("NCM SEND TO DRV INFO       \n");
    PS_PRINTF_INFO("times of sending to drv: %d\n send_buf: %x\n send_length:%d\n times of success return: %d \n",
                   table[DMS_SDM_VCOM_WRT_NCM].calledNum, table[DMS_SDM_VCOM_WRT_NCM].rserved1,
                   table[DMS_SDM_VCOM_WRT_NCM].rserved2, table[DMS_SDM_VCOM_WRT_SUSS_NCM].calledNum);

    return;
}


VOS_VOID DMS_ShowModemDataStats(VOS_VOID)
{
    PS_PRINTF_INFO("MODEM UL DATA STATS                 \n");
    PS_PRINTF_INFO("UL Read CB NUM:             %d\n", g_dmsStatsInfo.modemStats.ulDataReadCBNum);
    PS_PRINTF_INFO("UL RD SUCC NUM:             %d\n", g_dmsStatsInfo.modemStats.ulGetRDSuccNum);
    PS_PRINTF_INFO("UL RD Fail NUM:             %d\n", g_dmsStatsInfo.modemStats.ulGetRDFailNum);
    PS_PRINTF_INFO("UL Invalid RD NUM:          %d\n", g_dmsStatsInfo.modemStats.ulInvalidRDNum);
    PS_PRINTF_INFO("UL Retrun BUFF SUCC NUM:    %d\n", g_dmsStatsInfo.modemStats.ulReturnBuffSuccNum);
    PS_PRINTF_INFO("UL Retrun BUFF FAIL NUM:    %d\n", g_dmsStatsInfo.modemStats.ulReturnBuffFailNum);
    PS_PRINTF_INFO("MODEM DL DATA STATS                 \n");
    PS_PRINTF_INFO("DL Write Async SUCC NUM:    %d\n", g_dmsStatsInfo.modemStats.dlWriteAsyncSuccNum);
    PS_PRINTF_INFO("DL Write Async FAIL NUM:    %d\n", g_dmsStatsInfo.modemStats.dlWriteAsyncFailNum);
    PS_PRINTF_INFO("DL Free BUFF NUM:           %d\n", g_dmsStatsInfo.modemStats.dlFreeBuffNum);

    return;
}

#if (FEATURE_AT_HSUART == FEATURE_ON)


VOS_VOID DMS_ShowHsuartIoctlStats(VOS_VOID)
{
    PS_PRINTF_INFO("Config FLOW CTL SUCC NUM:   %d\n", g_dmsStatsInfo.hsuartStats.cfgFlowCtrlSuccNum);
    PS_PRINTF_INFO("Config FLOW CTL FAIL NUM:   %d\n", g_dmsStatsInfo.hsuartStats.cfgFlowCtrlFailNum);
    PS_PRINTF_INFO("Clear BUFF SUCC NUM:        %d\n", g_dmsStatsInfo.hsuartStats.clearBuffSuccNum);
    PS_PRINTF_INFO("Clear BUFF FAIL NUM:        %d\n", g_dmsStatsInfo.hsuartStats.clearBuffFailNum);
    PS_PRINTF_INFO("MSC Read CB NUM:            %d\n", g_dmsStatsInfo.hsuartStats.mscReadCBNum);
    PS_PRINTF_INFO("MSC Write SUCC NUM:         %d\n", g_dmsStatsInfo.hsuartStats.mscWriteSuccNum);
    PS_PRINTF_INFO("MSC Write FAIL NUM:         %d\n", g_dmsStatsInfo.hsuartStats.mscWriteFailNum);
    PS_PRINTF_INFO("MSC Switch CMD CB NUM:      %d\n", g_dmsStatsInfo.hsuartStats.switchCmdCBNum);

    return;
}


VOS_VOID DMS_ShowHsuartDataStats(VOS_VOID)
{
    PS_PRINTF_INFO("HSUART UL DATA STATS                \n");
    PS_PRINTF_INFO("UL Read CB NUM:             %d\n", g_dmsStatsInfo.hsuartStats.ulDataReadCBNum);
    PS_PRINTF_INFO("UL RD SUCC NUM:             %d\n", g_dmsStatsInfo.hsuartStats.ulGetRDSuccNum);
    PS_PRINTF_INFO("UL RD Fail NUM:             %d\n", g_dmsStatsInfo.hsuartStats.ulGetRDFailNum);
    PS_PRINTF_INFO("UL Invalid RD NUM:          %d\n", g_dmsStatsInfo.hsuartStats.ulInvalidRDNum);
    PS_PRINTF_INFO("UL Invalid CMD DATA NUM:    %d\n", g_dmsStatsInfo.hsuartStats.ulRcvInvalidCmdNum);
    PS_PRINTF_INFO("UL Valid CMD DATA NUM:      %d\n", g_dmsStatsInfo.hsuartStats.ulValidCmdNum);
    PS_PRINTF_INFO("UL IP Data NUM:             %d\n", g_dmsStatsInfo.hsuartStats.ulIpDataNum);
    PS_PRINTF_INFO("UL PPP Data NUM:            %d\n", g_dmsStatsInfo.hsuartStats.ulPppDataNum);
    PS_PRINTF_INFO("UL CSD Data NUM:            %d\n", g_dmsStatsInfo.hsuartStats.ulOmDataNum);
    PS_PRINTF_INFO("UL OM Data NUM:             %d\n", g_dmsStatsInfo.hsuartStats.ulOmDataNum);
    PS_PRINTF_INFO("UL Invalid MODE DATA NUM:   %d\n", g_dmsStatsInfo.hsuartStats.ulInvalidModeDataNum);
    PS_PRINTF_INFO("UL Retrun BUFF SUCC NUM:    %d\n", g_dmsStatsInfo.hsuartStats.ulReturnBuffSuccNum);
    PS_PRINTF_INFO("UL Retrun BUFF FAIL NUM:    %d\n", g_dmsStatsInfo.hsuartStats.ulReturnBuffFailNum);
    PS_PRINTF_INFO("HSUART DL DATA STATS                \n");
    PS_PRINTF_INFO("DL Write Async SUCC NUM:    %d\n", g_dmsStatsInfo.hsuartStats.dlWriteAsyncSuccNum);
    PS_PRINTF_INFO("DL Write Async FAIL NUM:    %d\n", g_dmsStatsInfo.hsuartStats.dlWriteAsyncFailNum);
    PS_PRINTF_INFO("DL Write Sync SUCC NUM:     %d\n", g_dmsStatsInfo.hsuartStats.dlWriteSyncSuccNum);
    PS_PRINTF_INFO("DL Write Sync FAIL NUM:     %d\n", g_dmsStatsInfo.hsuartStats.dlWriteSyncFailNum);
    PS_PRINTF_INFO("DL Write Sync SUCC LEN:     %d\n", g_dmsStatsInfo.hsuartStats.dlWriteSyncSuccLen);
    PS_PRINTF_INFO("DL Write Sync FAIL LEN:     %d\n", g_dmsStatsInfo.hsuartStats.dlWriteSyncFailLen);
    PS_PRINTF_INFO("DL Free BUFF NUM:           %d\n", g_dmsStatsInfo.hsuartStats.dlFreeBuffNum);

    return;
}
#endif


VOS_VOID DMS_Help(VOS_VOID)
{
    PS_PRINTF_INFO("Debug Info                                            \n");
    PS_PRINTF_INFO("<DMS_SetLogLevel>                                     \n");
    PS_PRINTF_INFO("<DMS_ShowDebugInfo>                                   \n");
    PS_PRINTF_INFO("<DMS_ShowPortCfgInfo>                                 \n");
    PS_PRINTF_INFO("<DMS_ShowVcomStats>                                   \n");
    PS_PRINTF_INFO("<DMS_ShowPortInfo portId>                             \n");
    PS_PRINTF_INFO("<DMS_ShowDevInfo portId>                              \n");
    PS_PRINTF_INFO("<DMS_ShowModemDataStats>                              \n");
    PS_PRINTF_INFO("<DMS_ShowHsuartIoctlStats>                            \n");
    PS_PRINTF_INFO("<DMS_ShowHsuartDataStats>                             \n");

    return;
}

