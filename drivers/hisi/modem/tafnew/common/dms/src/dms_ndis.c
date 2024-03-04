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

#include "product_config.h"
#include "msp_errno.h"
#include <dms.h>
#include "dms_core.h"
#include "vos.h"
#include "dms_debug.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */

#define THIS_FILE_ID PS_FILE_ID_DMS_NDIS_C

VOS_UINT32             g_ndisCfgFlag  = 0xffffffffU;
usb_ndis_stus_chg_func g_atConnectBrk = NULL;
usb_ndis_at_recv_func  g_atCmdRcv     = NULL;


VOS_UINT32 DMS_NcmSendData(VOS_UINT8 *data, VOS_UINT32 len)
{
    VOS_INT32             ret         = ERR_MSP_SUCCESS;
    struct ncm_at_rsp     aTResponse  = {0};
    DMS_PHY_BearProperty *phyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);
    VOS_UINT_PTR          ptrAddr;

    ptrAddr = (VOS_UINT_PTR)data;

    /* 检查NDIS通道状态 */
    if (ACM_EVT_DEV_SUSPEND == phyBearProp->chanStat) {
        return ERR_MSP_FAILURE;
    }

    if ((data == NULL) || (len == 0) || (len > 2048)) {
        return ERR_MSP_INVALID_PARAMETER;
    }

    aTResponse.at_answer = data;
    aTResponse.length    = len;

    if (UDI_INVALID_HANDLE == phyBearProp->portHandle) {
        return ERR_MSP_FAILURE;
    }

    DMS_DBG_SDM_FUN(DMS_SDM_VCOM_WRT_NCM, (VOS_UINT32)(ptrAddr & (~0U)), len, 0);

    ret = mdrv_udi_ioctl(phyBearProp->portHandle, NCM_IOCTL_AT_RESPONSE, &aTResponse);
    if (ERR_MSP_SUCCESS != ret) {
        return ERR_MSP_FAILURE;
    }

    DMS_DBG_SDM_FUN(DMS_SDM_VCOM_WRT_SUSS_NCM, 0, 0, 0);

    return ERR_MSP_SUCCESS;
}


VOS_VOID DMS_NcmProcCbReg(usb_ndis_stus_chg_func connectBrk, USB_NAS_AT_CMD_RECV atCmdRcv)
{
    DMS_PHY_BearProperty *phyBearProp = NULL;
    VOS_INT32             ret         = -1;

    if ((connectBrk == NULL) || (atCmdRcv == NULL)) {
        return;
    }

    /* NDIS 通道没有打开或者 通道已经被配置，则直接返回 */
    if ((g_ndisCfgFlag == 0xffffffffU) || (g_ndisCfgFlag == 0)) {
        return;
    }

    phyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (UDI_INVALID_HANDLE == phyBearProp->portHandle) {
        return;
    }

    ret = mdrv_udi_ioctl(phyBearProp->portHandle, NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC,
                         (usb_ndis_stus_chg_func)(connectBrk));
    if (ERR_MSP_SUCCESS != ret) {
        return;
    }

    ret = mdrv_udi_ioctl(phyBearProp->portHandle, NCM_IOCTL_REG_AT_PROCESS_FUNC, (usb_ndis_at_recv_func)(atCmdRcv));
    if (ERR_MSP_SUCCESS != ret) {
        return;
    }

    g_ndisCfgFlag = 0;
    return;
}


int DMS_NcmExtFuncReg(usb_ndis_stus_chg_func connectBrk, USB_NAS_AT_CMD_RECV atCmdRcv)
{
    g_atConnectBrk = (usb_ndis_stus_chg_func)connectBrk;
    g_atCmdRcv     = (usb_ndis_at_recv_func)atCmdRcv;

    DMS_NcmProcCbReg((usb_ndis_stus_chg_func)connectBrk, (USB_NAS_AT_CMD_RECV)atCmdRcv);

    return ERR_MSP_SUCCESS;
}


VOS_VOID DMS_SetNdisChanStatus(ACM_EVT_E status)
{
    DMS_PHY_BearProperty *phyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    phyBearProp->chanStat = (VOS_UINT8)status;
    return;
}

VOS_VOID DMS_NcmWrtCB(char *doneBuff, int status)
{
    if (Dms_IsStaticBuf((VOS_UINT8 *)doneBuff)) {
        Dms_FreeStaticBuf((VOS_UINT8 *)doneBuff);
    } else {
        if (doneBuff != NULL) {
#if (VOS_LINUX == VOS_OS_VER)
            kfree(doneBuff);
#endif
        }
    }

    return;
}


VOS_UINT32 DMS_NcmOpen(VOS_VOID)
{
    VOS_UINT32            ret         = ERR_MSP_SUCCESS;
    int                   handle      = UDI_INVALID_HANDLE;
    DMS_PHY_BearProperty *phyBearProp = NULL;
    struct udi_open_param param;

    phyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (UDI_INVALID_HANDLE != phyBearProp->portHandle) {
        return ERR_MSP_SUCCESS;
    }

    param.devid = UDI_NCM_CTRL_ID;

    handle = mdrv_udi_open(&param);
    if (UDI_INVALID_HANDLE == handle) {
        ret = ERR_MSP_FAILURE;
    } else {
        phyBearProp->portHandle = handle;

        if (VOS_OK != mdrv_udi_ioctl(handle, NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC, DMS_NcmWrtCB)) {
            DMS_LOGW("NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC fail.");
        }
    }

    /* 配置 NDIS CTRL 通道 */
    DMS_NcmProcCbReg(g_atConnectBrk, (USB_NAS_AT_CMD_RECV)g_atCmdRcv);

    return ret;
}


VOS_UINT32 DMS_NcmClose(VOS_VOID)
{
    VOS_INT32             ret         = ERR_MSP_SUCCESS;
    DMS_PHY_BearProperty *phyBearProp = NULL;

    phyBearProp = DMS_GetPhyBearProperty(DMS_PHY_BEAR_USB_NCM);

    if (UDI_INVALID_HANDLE == phyBearProp->portHandle) {
        DMS_LOGW("[%d] Already close.", DMS_PHY_BEAR_USB_NCM);
        return ERR_MSP_SUCCESS;
    }

    ret = mdrv_udi_close(phyBearProp->portHandle);

    phyBearProp->portHandle = UDI_INVALID_HANDLE;

    return (VOS_UINT32)ret;
}

