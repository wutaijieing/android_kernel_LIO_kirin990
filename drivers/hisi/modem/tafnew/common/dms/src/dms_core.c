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

#include <product_config.h>
#include "vos.h"
#include "msp_errno.h"
#include <dms.h>
#include "dms_core.h"
#include "ps_lib.h"
#include "mdrv_nvim.h"
#include "taf_type_def.h"
#include "acore_nv_stru_gucnas.h"
#include "securec.h"
#include "dms_debug.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */

#define THIS_FILE_ID PS_FILE_ID_DMS_CORE_C

DMS_MainInfo g_dmsMainInfo = {0};

extern VOS_VOID   At_MsgProc(MsgBlock *msg);
extern VOS_UINT32 At_PidInit(enum VOS_InitPhaseDefine phase);

extern VOS_VOID   APP_VCOM_SendDebugNvCfg(VOS_UINT32 portIdMask1, VOS_UINT32 portIdMask2, VOS_UINT32 debugLevel);
extern VOS_VOID   DMS_SendPortDebugNvCfg(VOS_UINT32 debugLevel, VOS_UINT32 portIdMask1, VOS_UINT32 portIdMask2);


VOS_VOID DMS_ReadPortDebugCfgNV(VOS_VOID)
{
    TAF_NV_PortDebugCfg portDebugNVCfg;

    memset_s(&portDebugNVCfg, sizeof(portDebugNVCfg), 0x00, sizeof(portDebugNVCfg));

    /* 读取NV项 */
    if (NV_OK != TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_DMS_DEBUG_CFG, &portDebugNVCfg, sizeof(TAF_NV_PortDebugCfg))) {
        return;
    }

    APP_VCOM_SendDebugNvCfg(portDebugNVCfg.appVcomPortIdMask1, portDebugNVCfg.appVcomPortIdMask2,
                            portDebugNVCfg.appVcomDebugLevel);
    DMS_SendPortDebugNvCfg(portDebugNVCfg.dmsDebugLevel, portDebugNVCfg.dmsPortIdMask1, portDebugNVCfg.dmsPortIdMask2);

    return;
}


DMS_READ_DATA_PFN DMS_GetDataReadFun(VOS_VOID)
{
    return g_dmsMainInfo.pfnRdDataCallback;
}


DMS_MainInfo* DMS_GetMainInfo(VOS_VOID)
{
    return &g_dmsMainInfo;
}


DMS_PHY_BearProperty* DMS_GetPhyBearProperty(DMS_PHY_Bear phyBear)
{
    return &(DMS_GetMainInfo()->phyProperty[phyBear]);
}


VOS_VOID DMS_UsbDisableCB(VOS_VOID)
{
    VOS_UINT32 ret;

    DMS_DBG_SDM_FUN(DMS_SDM_USB_DISABLE, 0, 0, 0);

    /* 关闭PCUI通道 */
    ret = DMS_VcomPcuiClose();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_DISABLE_ERR, 0, 0, 3);
    }

    /* 关闭CTRL通道 */
    ret = DMS_VcomCtrlClose();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_DISABLE_ERR, 0, 0, 5);
    }

    /* 关闭PCUI2通道 */
    ret = DMS_VcomPcui2Close();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_DISABLE_ERR, 0, 0, 6);
    }

    /* 关闭NDIS CTRL通道 */
    ret = DMS_NcmClose();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_DISABLE_ERR, 0, 0, 4);
    }

    return;
}


VOS_VOID DMS_UsbEnableCB(VOS_VOID)
{
    VOS_UINT32 ret;

    DMS_DBG_SDM_FUN(DMS_SDM_USB_ENABLE, 0, 0, 0);

    /* 打开 AT PCUI 通道 */
    ret = DMS_VcomPcuiOpen();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_ENABLE_ERR, 0, 0, 3);
    }

    /* 打开 AT CTRL 通道 */
    ret = DMS_VcomCtrlOpen();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_ENABLE_ERR, 0, 0, 5);
    }

    /* 打开 AT PCUI2 通道 */
    ret = DMS_VcomPcui2Open();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_ENABLE_ERR, 0, 0, 6);
    }

    /* 打开 NDIS CTRL 通道 */
    ret = DMS_NcmOpen();
    if (ret != ERR_MSP_SUCCESS) {
        DMS_DBG_SDM_FUN(DMS_SDM_USB_ENABLE_ERR, 0, 0, 4);
    }

    g_ndisCfgFlag = 1;

    return;
}


VOS_VOID DMS_Init(VOS_VOID)
{
    VOS_UINT32 ulport;

    memset_s(g_dmsSdmInfoTable, sizeof(g_dmsSdmInfoTable), 0x00, sizeof(g_dmsSdmInfoTable));

    memset_s(&g_dmsPortDebugCfg, sizeof(g_dmsPortDebugCfg), 0x00, sizeof(g_dmsPortDebugCfg));

    g_dmsMainInfo.pfnRdDataCallback  = NULL;

    /* 初始化所有端口的handle */
    for (ulport = 0; ulport < DMS_PHY_BEAR_LAST; ulport++) {
        g_dmsMainInfo.phyProperty[ulport].portHandle = UDI_INVALID_HANDLE;
    }

    /* 初始化AT通道使用的静态内存 */
    Dms_StaticBufInit();

    DMS_ReadPortDebugCfgNV();

    mdrv_usb_reg_enablecb(DMS_UsbEnableCB);
    mdrv_usb_reg_disablecb(DMS_UsbDisableCB);

    return;
}


VOS_UINT32 DMS_DsFidInit(enum VOS_InitPhaseDefine ip)
{
    VOS_UINT32 relVal = 0;

    switch (ip) {
        case VOS_IP_LOAD_CONFIG:

            /* 原有dms pcui、ctrl、pcui2口的初始化 关注与DMS_InitCtx调用顺序
             * 先调用DMS_Init，再调用AT_InitPort，再调用DMS_PORT_Init，
             * 依据：保证回调指针赋值给底软接口前，dms port模块的回调指针已经赋值
             */
            DMS_Init();

            relVal = VOS_RegisterPIDInfo(WUEPS_PID_AT, (InitFunType)At_PidInit, (MsgFunType)At_MsgProc);
            if (relVal != VOS_OK) {
                return VOS_ERR;
            }

            relVal = VOS_RegisterTaskPrio(MSP_APP_DS_FID, DMS_APP_DS_TASK_PRIORITY);
            if (relVal != VOS_OK) {
                return VOS_ERR;
            }

            break;

        default:
            break;
    }

    return VOS_OK;
}

