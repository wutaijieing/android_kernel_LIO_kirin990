/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
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

#include "dms_dev.h"
#include "dms_dev_i.h"
#include "dms_debug.h"
#include "dms_port_i.h"
#include "imm_interface.h"
#include "securec.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_DMS_DEV_I_C


VOS_VOID* DMS_DEV_InitCtx(DMS_PortIdUint16 portId, const struct DMS_PortFuncTbl *portFuncTbl)
{
    struct DMS_DevInfo *devInfo = VOS_NULL_PTR;

    DMS_PORT_LOGI(portId, "enter.");

    if (portFuncTbl == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "portFuncTbl null.");
        return VOS_NULL_PTR;
    }

    devInfo = DMS_DEV_GetDevEntityByPortId(portId);
    devInfo->portFuncTbl = portFuncTbl;

    return devInfo;
}


STATIC VOS_VOID DMS_HSUART_InitStats(VOS_VOID)
{
    (VOS_VOID)memset_s(&g_dmsStatsInfo.hsuartStats, sizeof(g_dmsStatsInfo.hsuartStats), 0x00,
                       sizeof(struct DMS_DebugHsuartStats));
    return;
}


STATIC VOS_VOID DMS_MDM_InitStats(VOS_VOID)
{
    (VOS_VOID)memset_s(&g_dmsStatsInfo.modemStats, sizeof(g_dmsStatsInfo.modemStats), 0x00,
                       sizeof(struct DMS_DebugModemStats));
    return;
}


STATIC VOS_VOID DMS_DEV_RcvUsbEnableInd(VOS_VOID)
{
    DMS_LOGI("enter.");

    DMS_MDM_InitStats();
    DMS_DEV_RcvConnectStateInd(DMS_PORT_MODEM, DMS_CONNECT);
    DMS_DEV_RcvConnectStateInd(DMS_PORT_NCM_DATA, DMS_CONNECT);
}


STATIC VOS_VOID DMS_DEV_RcvUsbDisableInd(VOS_VOID)
{
    DMS_LOGI("enter.");

    DMS_DEV_RcvConnectStateInd(DMS_PORT_MODEM, DMS_DISCONNECT);
    DMS_DEV_RcvConnectStateInd(DMS_PORT_NCM_DATA, DMS_DISCONNECT);
}


STATIC VOS_UINT32 DMS_DEV_IsAlwaysOnPort(DMS_PortIdUint16 portId)
{
    /*
     * usb相关端口都为非一直在线端口，当前仅判断modem和ncm data口，
     * 后续收编老的DSM时再判断其他usb端口
     */
    if (portId == DMS_PORT_MODEM || portId == DMS_PORT_NCM_DATA) {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


VOS_VOID DMS_DEV_InitDev(VOS_VOID)
{
    struct DMS_DevInfo *devInfo = VOS_NULL_PTR;
    VOS_UINT32          i;

    DMS_LOGI("enter.");

    for (i = 0; i < DMS_PORT_BUTT; i++) {
        if (DMS_DEV_IsAlwaysOnPort((DMS_PortIdUint16)i) == VOS_TRUE) {
            devInfo = DMS_DEV_GetDevEntityByPortId((VOS_UINT8)i);
            DMS_DEV_Open(devInfo);
        }
    }

    DMS_HSUART_InitStats();

    /* NDIS DATA 和modem口通过usb enable cb打开 */
    mdrv_usb_reg_enablecb((usb_udi_enable_cb)DMS_DEV_RcvUsbEnableInd);
    mdrv_usb_reg_disablecb((usb_udi_enable_cb)DMS_DEV_RcvUsbDisableInd);
}


STATIC VOS_UINT32 DMS_DEV_CommCheck(struct DMS_DevInfo *dev)
{
    DMS_LOGI("enter.");

    if (dev == VOS_NULL_PTR) {
        DMS_LOGE("dev is null.");
        return VOS_ERR;
    }

    if (dev->handle == UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(dev->portId, "dev handle is invalid.");
        return VOS_ERR;
    }

    if (dev->ops == VOS_NULL_PTR) {
        DMS_PORT_LOGE(dev->portId, "dev ops is null.");
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 DMS_DEV_SendDataSync(VOS_VOID *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    /* 由上层保证下发的data和len不为空指针和0 */
    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SendDataSync == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SendDataSync(dev, data, len);
}


VOS_UINT32 DMS_DEV_SendDataAsync(VOS_VOID *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    /* 由上层保证下发的data和len不为空指针和0 */
    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SendDataAsync == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SendDataAsync(dev, data, len);
}


VOS_UINT32 DMS_DEV_SendPrivData(VOS_VOID *dev, IMM_Zc *immZc)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    /* 由上层保证下发的immZc不为空指针 */
    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        IMM_ZcFree(immZc);
        return VOS_ERR;
    }

    if (devInfo->ops->SendPrivData == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        IMM_ZcFree(immZc);
        return VOS_ERR;
    }

    return devInfo->ops->SendPrivData(dev, immZc);
}


VOS_UINT32 DMS_DEV_SendMscCmd(VOS_VOID *dev, struct modem_msc_stru *msc)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SendMscCmd == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SendMscCmd(dev, msc);
}


VOS_UINT32 DMS_DEV_SetIpv6Dns(VOS_VOID *dev, struct ncm_ipv6_dns *dns)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SetIpv6Dns == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SetIpv6Dns(dev, dns);
}


VOS_UINT32 DMS_DEV_ChangeFlowCtrl(VOS_VOID *dev, VOS_UINT32 enableFlg)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->ChangeFlowCtrl == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->ChangeFlowCtrl(dev, enableFlg);
}


VOS_UINT32 DMS_DEV_ChangeConnSpeed(VOS_VOID *dev, struct ncm_ioctl_connection_speed *connSpeed)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->ChangeConnSpeed == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->ChangeConnSpeed(dev, connSpeed);
}


VOS_UINT32 DMS_DEV_ChangeConnLinkState(VOS_VOID *dev, VOS_UINT32 linkStatus)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->ChangeConnLinkState == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->ChangeConnLinkState(dev, linkStatus);
}


VOS_UINT32 DMS_DEV_FreeUlDataBuff(VOS_VOID *dev, VOS_VOID *data)
{
    struct DMS_DevInfo *devInfo = dev;
    VOS_UINT32          ret;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->FreeUlDataBuff == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    ret = devInfo->ops->FreeUlDataBuff(dev, data);

    return ret;
}


VOS_UINT32 DMS_DEV_Open(VOS_VOID *dev)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (devInfo == VOS_NULL_PTR) {
        DMS_LOGE("dev is null.");
        return VOS_ERR;
    }

    if (devInfo->handle != UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(devInfo->portId, "dev is opened.");
        return VOS_ERR;
    }

    if (devInfo->ops == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "ops is null.");
        return VOS_ERR;
    }

    if (devInfo->ops->Open == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->Open(dev);
}


VOS_UINT32 DMS_DEV_Close(VOS_VOID *dev)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->Close == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->Close(dev);
}


VOS_INT32 DMS_DEV_GetHandle(VOS_VOID *dev)
{
    struct DMS_DevInfo *devInfo = dev;
    return devInfo->handle;
}

#if (FEATURE_AT_HSUART == FEATURE_ON)


VOS_UINT32 DMS_DEV_ConfigFlowCtrl(VOS_VOID *dev, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->ConfigFlowCtrl == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->ConfigFlowCtrl(dev, rtsFlag, ctsFlag);
}


VOS_UINT32 DMS_DEV_SetCharFrame(VOS_VOID *dev, VOS_UINT32 udiDataLen, VOS_UINT32 udiStpLen, VOS_UINT32 udiParity)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SetCharFrame == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SetCharFrame(dev, udiDataLen, udiStpLen, udiParity);
}


VOS_UINT32 DMS_DEV_SetBaud(VOS_VOID *dev, VOS_UINT32 baudRate)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SetBaud == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SetBaud(dev, baudRate);
}


VOS_UINT32 DMS_DEV_SetACShell(VOS_VOID *dev, VOS_UINT32 mode)
{
    struct DMS_DevInfo *devInfo = dev;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->SetACShell == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    return devInfo->ops->SetACShell(dev, mode);
}


VOS_UINT32 DMS_DEV_FlushTxData(VOS_VOID *dev)
{
    struct DMS_DevInfo *devInfo = dev;
    VOS_UINT32          ret;

    DMS_LOGI("enter.");

    if (DMS_DEV_CommCheck(devInfo) != VOS_OK) {
        return VOS_ERR;
    }

    if (devInfo->ops->FlushTxData == VOS_NULL_PTR) {
        DMS_PORT_LOGE(devInfo->portId, "dev ops func is null.");
        return VOS_ERR;
    }

    ret = devInfo->ops->FlushTxData(dev);

    return ret;
}
#endif

