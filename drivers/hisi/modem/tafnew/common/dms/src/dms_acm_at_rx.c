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
#include "mdrv.h"
#include "taf_type_def.h"
#include "securec.h"
#include "dms_debug.h"
#include "dms_port_i.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */

#define THIS_FILE_ID PS_FILE_ID_DMS_ACM_AT_RX_C

pComRecv g_acmReadData = VOS_NULL_PTR;


VOS_UINT32 Dms_AtReadData(DMS_PHY_Bear phyBear, VOS_UINT8 *dataBuf, VOS_UINT32 len)
{
    VOS_UINT32 ret = 0;

    if ((VOS_NULL == g_acmReadData) || (VOS_NULL == dataBuf)) {
        return ERR_MSP_INVALID_PARAMETER;
    }

    if ((DMS_PHY_BEAR_USB_PCUI != phyBear) && (DMS_PHY_BEAR_USB_CTRL != phyBear) &&
        (DMS_PHY_BEAR_USB_PCUI2 != phyBear)) {
        return ERR_MSP_INVALID_PARAMETER;
    }

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "len = %d, buf = %s", len, dataBuf);

    ret = (VOS_UINT32)g_acmReadData((VOS_UINT8)phyBear, dataBuf, (VOS_UINT16)len);

    return ret;
}


VOS_INT32 DMS_ACMRecvFuncReg(pComRecv callback)
{
    DMS_MainInfo *mainInfo = DMS_GetMainInfo();

    if (VOS_NULL == callback) {
        return ERR_MSP_INVALID_PARAMETER;
    }

    mainInfo->pfnRdDataCallback = Dms_AtReadData;
    g_acmReadData               = callback;

    return ERR_MSP_SUCCESS;
}


VOS_UINT32 DMS_UsbPortOpen(DMS_PHY_Bear phyBear, enum udi_device_id deviceId, VOS_VOID *readCB, VOS_VOID *writeCB,
                           VOS_VOID *stateCB)
{
    DMS_PHY_BearProperty     *phyBearProp = NULL;
    struct udi_open_param     openParam;
    struct acm_read_buff_info readBuffInfo;
    int                       handle = UDI_INVALID_HANDLE;

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Open Enter.");

    phyBearProp = DMS_GetPhyBearProperty(phyBear);

    if (UDI_INVALID_HANDLE != phyBearProp->portHandle) {
        DMS_PORT_LOGW(phyBear + DMS_PORT_PCUI, "Already open.");
        return ERR_MSP_SUCCESS;
    }

    DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_OPEN_BEGIN + (VOS_UINT32)phyBear), 0, 0, 0);

    openParam.devid = deviceId;

    readBuffInfo.buff_size = DMS_UL_DATA_BUFF_SIZE;
    readBuffInfo.buff_num  = DMS_UL_DATA_BUFF_NUM;

    handle = mdrv_udi_open(&openParam);
    if (UDI_INVALID_HANDLE != handle) {
        if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_RELLOC_READ_BUFF, &readBuffInfo)) {
            DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_RELLOC_READ_BUFF fail.");
            DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_OPEN_ERR_BEGIN + (VOS_UINT32)phyBear),
                            (VOS_UINT32)handle, 0, 1);
        }

        if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_SET_WRITE_CB, writeCB)) {
            DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_SET_WRITE_CB fail.");
            DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_OPEN_ERR_BEGIN + (VOS_UINT32)phyBear),
                            (VOS_UINT32)handle, 0, 3);
        }

        if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_WRITE_DO_COPY, (void *)0)) {
            DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_WRITE_DO_COPY fail.");
            DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_OPEN_ERR_BEGIN + (VOS_UINT32)phyBear),
                            (VOS_UINT32)handle, 0, 4);
        }

        if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_SET_EVT_CB, stateCB)) {
            DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_SET_EVT_CB fail.");
            DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_OPEN_ERR_BEGIN + (VOS_UINT32)phyBear),
                            (VOS_UINT32)handle, 0, 5);
        }

        phyBearProp->portHandle = handle;

        if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_SET_READ_CB, readCB)) {
            DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_SET_READ_CB fail.");
            DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_OPEN_ERR_BEGIN + (VOS_UINT32)phyBear),
                            (VOS_UINT32)handle, 0, 2);
        }

        DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Open success.");
        return ERR_MSP_SUCCESS;
    }

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Open fail.");
    DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_OPEN_ERR_BEGIN + (VOS_UINT32)phyBear), (VOS_UINT32)handle, 0,
                    6);
    return ERR_MSP_FAILURE;
}


VOS_UINT32 DMS_UsbPortClose(DMS_PHY_Bear phyBear)
{
    DMS_PHY_BearProperty *phyBearProp = NULL;
    VOS_INT32             ret         = ERR_MSP_SUCCESS;

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Enter.");

    phyBearProp = DMS_GetPhyBearProperty(phyBear);

    if (UDI_INVALID_HANDLE == phyBearProp->portHandle) {
        DMS_PORT_LOGW(phyBear + DMS_PORT_PCUI, "Already close.");
        return ERR_MSP_SUCCESS;
    }

    DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_CLOSE_BEGIN + (VOS_UINT32)phyBear), 0, 0, 0);

    ret = mdrv_udi_close(phyBearProp->portHandle);
    if (ret == ERR_MSP_SUCCESS) {
        DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Close success.");
        phyBearProp->portHandle = UDI_INVALID_HANDLE;
        phyBearProp->chanStat   = ACM_EVT_DEV_SUSPEND;
        return (VOS_UINT32)ret;
    }

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Close fail.");
    DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_CLOSE_ERR_BEGIN + (VOS_UINT32)phyBear), (VOS_UINT32)ret, 0,
                    0);
    return ERR_MSP_FAILURE;
}


VOS_VOID DMS_UsbPortReadCB(DMS_PHY_Bear phyBear)
{
    DMS_PHY_BearProperty    *phyBearProp = NULL;
    DMS_READ_DATA_PFN        rdDataFun   = NULL;
    struct acm_wr_async_info acmInfo;
    int                      handle = UDI_INVALID_HANDLE;

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "Read begin.");

    memset_s(&acmInfo, sizeof(acmInfo), 0x00, sizeof(acmInfo));

    phyBearProp = DMS_GetPhyBearProperty(phyBear);

    handle = phyBearProp->portHandle;
    if (UDI_INVALID_HANDLE == handle) {
        DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "UDI_INVALID_HANDLE.");
        return;
    }

    DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_RD_CB_BEGIN + (VOS_UINT32)phyBear), 0, 0, 0);

    if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_GET_RD_BUFF, &acmInfo)) {
        DMS_PORT_LOGE(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_GET_RD_BUFF fail.");
        DMS_DBG_SDM_FUN((DMS_SDM_MsgId )(DMS_SDM_VCOM_RD_CB_ERR_BEGIN + (VOS_UINT32)phyBear), (VOS_UINT32)handle, 0, 0);
        return;
    }

    phyBearProp->chanStat = ACM_EVT_DEV_READY;

    rdDataFun = DMS_GetDataReadFun();
    if (NULL != rdDataFun) {
        (VOS_VOID)rdDataFun(phyBear, (VOS_UINT8 *)acmInfo.virt_addr, acmInfo.size);
    }

    if (MDRV_OK != mdrv_udi_ioctl(handle, ACM_IOCTL_RETURN_BUFF, &acmInfo)) {
        DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "ACM_IOCTL_RETURN_BUFF fail.");
    }

    return;
}


VOS_VOID DMS_UsbPortWrtCB(DMS_PHY_Bear phyBear, VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize)
{
    VOS_UINT_PTR ptrDebugVirAddr;
    VOS_UINT_PTR ptrDebugPhyAddr;

    ptrDebugVirAddr = (VOS_UINT_PTR)pcVirAddr;
    ptrDebugPhyAddr = (VOS_UINT_PTR)pcPhyAddr;

    DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_WRT_CB_BEGIN + (VOS_UINT32)phyBear),
                    (VOS_UINT32)(ptrDebugVirAddr & (~0U)), (VOS_UINT32)(ptrDebugPhyAddr & (~0U)), (VOS_UINT32)doneSize);

    if (doneSize < 0) {
        DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_WRT_CB_ERR_BEGIN + (VOS_UINT32)phyBear),
                        (VOS_UINT32)(ptrDebugVirAddr & (~0U)), (VOS_UINT32)(ptrDebugPhyAddr & (~0U)),
                        (VOS_UINT32)doneSize);
    }

    if (VOS_TRUE == Dms_IsStaticBuf((VOS_UINT8 *)pcVirAddr)) {
        Dms_FreeStaticBuf((VOS_UINT8 *)pcVirAddr);
    } else {
        if (NULL != pcVirAddr) {
#if (VOS_LINUX == VOS_OS_VER)
            kfree(pcVirAddr);
#endif
        }
    }

    return;
}


VOS_VOID DMS_UsbPortEvtCB(DMS_PHY_Bear phyBear, ACM_EVT_E evt)
{
    DMS_PHY_BearProperty *phyBearProp = NULL;

    DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_EVT_BEGIN + (VOS_UINT32)phyBear), evt, 0, 0);

    phyBearProp = DMS_GetPhyBearProperty(phyBear);

    if (ACM_EVT_DEV_READY == evt) {
        phyBearProp->chanStat = ACM_EVT_DEV_READY;
    } else {
        phyBearProp->chanStat = ACM_EVT_DEV_SUSPEND;
    }

    return;
}


VOS_UINT32 DMS_VcomPcuiOpen(VOS_VOID)
{
    return DMS_UsbPortOpen(DMS_PHY_BEAR_USB_PCUI, UDI_ACM_AT_ID, DMS_VcomPcuiReadCB, DMS_VcomPcuiWrtCB,
                           DMS_VcomPcuiEvtCB);
}


VOS_UINT32 DMS_VcomPcuiClose(VOS_VOID)
{
    return DMS_UsbPortClose(DMS_PHY_BEAR_USB_PCUI);
}


VOS_VOID DMS_VcomPcuiReadCB(VOS_VOID)
{
    DMS_UsbPortReadCB(DMS_PHY_BEAR_USB_PCUI);
    return;
}


VOS_VOID DMS_VcomPcuiWrtCB(VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize)
{
    DMS_UsbPortWrtCB(DMS_PHY_BEAR_USB_PCUI, pcVirAddr, pcPhyAddr, doneSize);
    return;
}


VOS_VOID DMS_VcomPcuiEvtCB(ACM_EVT_E evt)
{
    DMS_UsbPortEvtCB(DMS_PHY_BEAR_USB_PCUI, evt);
    return;
}


VOS_UINT32 DMS_VcomCtrlOpen(VOS_VOID)
{
    return DMS_UsbPortOpen(DMS_PHY_BEAR_USB_CTRL, UDI_ACM_CTRL_ID, DMS_VcomCtrlReadCB, DMS_VcomCtrlWrtCB,
                           DMS_VcomCtrlEvtCB);
}


VOS_UINT32 DMS_VcomCtrlClose(VOS_VOID)
{
    return DMS_UsbPortClose(DMS_PHY_BEAR_USB_CTRL);
}


VOS_VOID DMS_VcomCtrlReadCB(VOS_VOID)
{
    DMS_UsbPortReadCB(DMS_PHY_BEAR_USB_CTRL);
    return;
}


VOS_VOID DMS_VcomCtrlWrtCB(VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize)
{
    DMS_UsbPortWrtCB(DMS_PHY_BEAR_USB_CTRL, pcVirAddr, pcPhyAddr, doneSize);
    return;
}


VOS_VOID DMS_VcomCtrlEvtCB(ACM_EVT_E evt)
{
    DMS_UsbPortEvtCB(DMS_PHY_BEAR_USB_CTRL, evt);
    return;
}


VOS_UINT32 DMS_VcomPcui2Open(VOS_VOID)
{
    return DMS_UsbPortOpen(DMS_PHY_BEAR_USB_PCUI2, UDI_ACM_SKYTONE_ID, DMS_VcomPcui2ReadCB, DMS_VcomPcui2WrtCB,
                           DMS_VcomPcui2EvtCB);
}


VOS_UINT32 DMS_VcomPcui2Close(VOS_VOID)
{
    return DMS_UsbPortClose(DMS_PHY_BEAR_USB_PCUI2);
}


VOS_VOID DMS_VcomPcui2ReadCB(VOS_VOID)
{
    DMS_UsbPortReadCB(DMS_PHY_BEAR_USB_PCUI2);
    return;
}


VOS_VOID DMS_VcomPcui2WrtCB(VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize)
{
    DMS_UsbPortWrtCB(DMS_PHY_BEAR_USB_PCUI2, pcVirAddr, pcPhyAddr, doneSize);
    return;
}


VOS_VOID DMS_VcomPcui2EvtCB(ACM_EVT_E evt)
{
    DMS_UsbPortEvtCB(DMS_PHY_BEAR_USB_PCUI2, evt);
    return;
}

