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

#include "dms_port_i.h"
#include "dms.h"
#include "dms_debug.h"
#include "imm_interface.h"
#include "at_app_vcom_interface.h"
#include "taf_type_def.h"
#include "securec.h"
#include "dms_dev_i.h"
#include "cbt_cpm.h"
#include "dms_dev.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_DMS_DEV_C


STATIC VOS_INT DMS_COMM_CBCheck(DMS_PortIdUint16 portId)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    VOS_INT32                     handle;

    DMS_LOGI("enter.");

    if (portId >= DMS_PORT_BUTT) {
        DMS_LOGE("portId:%d is invalid.", portId);
        return VOS_ERROR;
    }

    handle = DMS_DEV_GetDevHandlerByPortId(portId);
    if (handle == UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(portId, "handle is null.");
        return VOS_ERROR;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(portId);
    if (portTblInfo == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "portTblInfo is not registered.");
        return VOS_ERROR;
    }

    return VOS_OK;
}


STATIC VOS_INT DMS_APP_RcvData(VOS_UINT8 devId, VOS_UINT8 *data, VOS_UINT32 len)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    DMS_PortIdUint16              portId      = DMS_PORT_APP + devId;

    DMS_LOGI("enter.");

    if (devId >= APP_VCOM_DEV_INDEX_BUTT) {
        DMS_LOGE("devid: %d incorrect.", devId);
        return VOS_ERROR;
    }

    if (data == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "data is NULL PTR.");
        return VOS_ERROR;
    }

    if (len == 0) {
        DMS_PORT_LOGE(portId, "len is 0.");
        return VOS_ERROR;
    }

    if (DMS_COMM_CBCheck(portId) != VOS_OK) {
        return VOS_ERROR;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(portId);
    if (portTblInfo->RcvData == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "ReadData is not registered.");
        return VOS_ERROR;
    }

    return portTblInfo->RcvData(portId, data, len);
}


STATIC VOS_VOID DMS_APP_RcvSwitchGwMode(VOS_VOID)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    if (DMS_COMM_CBCheck(DMS_PORT_APP) != VOS_OK) {
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_APP);

    if (portTblInfo->RcvSwitchGwModeInd == VOS_NULL_PTR) {
        DMS_LOGE("APPVCOM0 RcvSwitchGwModeInd is NULL.");
        return;
    }

    portTblInfo->RcvSwitchGwModeInd(DMS_PORT_APP);
}


STATIC VOS_UINT32 DMS_APP_Open(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    dev->handle = DMS_INIT_HANDLE;

    if (dev->devId == APP_VCOM_DEV_INDEX_0) {
        /* 注册DRV回调函数指针 */
        if (DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC((USB_NET_DEV_SWITCH_GATEWAY)DMS_APP_RcvSwitchGwMode) == VOS_ERROR) {
            DMS_PORT_LOGE(dev->portId, "DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC Failed.");
        }
    }

    return APP_VCOM_REG_DATA_CALLBACK((VOS_UINT8)dev->devId, DMS_APP_RcvData);
}


STATIC VOS_UINT32 DMS_APP_SendDataAsync(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (APP_VCOM_SEND((VOS_UINT8)dev->devId, data, len) != VOS_OK) {
        DMS_PORT_LOGE(dev->portId, "write data async error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_INT DMS_ACM_RcvData(VOS_UINT8 devId, VOS_UINT8 *data, VOS_UINT16 uslength)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    DMS_PortIdUint16              portId      = DMS_PORT_PCUI + devId;
    VOS_INT32                     ret         = 0;

    DMS_LOGI("enter.");

    if (devId > DMS_PHY_BEAR_USB_PCUI2) {
        DMS_LOGE("devid: %d incorrect.", devId);
        return VOS_ERROR;
    }

    if (data == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "data is NULL PTR.");
        return VOS_ERROR;
    }

    if (uslength == 0) {
        DMS_PORT_LOGE(portId, "len is 0.");
        return VOS_ERROR;
    }

    if (DMS_COMM_CBCheck(portId) != VOS_OK) {
        return VOS_ERROR;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(portId);
    if (portTblInfo->RcvData == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "ReadData is not registered.");
        return VOS_ERROR;
    }

    ret = portTblInfo->RcvData(portId, data, uslength); /* 调用DMS port模块的回调函数 */

    return ret;
}


STATIC VOS_UINT32 DMS_ACM_Open(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    dev->handle = DMS_INIT_HANDLE;

    /* 注册VCOM上行数据接收回调 */
    if (DMS_ACMRecvFuncReg(DMS_ACM_RcvData) != ERR_MSP_SUCCESS) {
        return VOS_ERR;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_ACM_SendDataAsync(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (DMS_COM_SEND((DMS_PHY_Bear)dev->devId, data, len) != VOS_OK) {
        DMS_PORT_LOGE(dev->portId, "write data async error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_VOID DMS_NCM_RcvConnStatusChg(enum ncm_ioctrl_connect_stus status, VOS_VOID *buffer)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (status != NCM_IOCTL_STUS_CONNECT && status != NCM_IOCTL_STUS_BREAK) {
        DMS_LOGE("status uncorrect.");
        return;
    }

    if (DMS_COMM_CBCheck(DMS_PORT_NCM_CTRL) != VOS_OK) {
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_NCM_CTRL);
    if (portTblInfo->RcvConnStatusChgInd == VOS_NULL_PTR) {
        DMS_LOGE("RcvConnStatusChgInd is not registered.");
        return;
    }

    portTblInfo->RcvConnStatusChgInd(DMS_PORT_NCM_CTRL, status);
}


STATIC VOS_INT DMS_NCM_CtrlRcvData(VOS_UINT8 *data, VOS_UINT16 uslength)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (data == VOS_NULL_PTR) {
        DMS_LOGE("data is NULL PTR.");
        return VOS_ERROR;
    }

    if (uslength == 0) {
        DMS_LOGE("len is 0.");
        return VOS_ERROR;
    }

    if (DMS_COMM_CBCheck(DMS_PORT_NCM_CTRL) != VOS_OK) {
        return VOS_ERROR;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_NCM_CTRL);
    if (portTblInfo->RcvData == VOS_NULL_PTR) {
        DMS_LOGE("ReadData is not registered.");
        return VOS_ERROR;
    }

    /* 设置NDIS通道状态为可上报数据 */
    DMS_SetNdisChanStatus(ACM_EVT_DEV_READY);

    return portTblInfo->RcvData(DMS_PORT_NCM_CTRL, data, (VOS_UINT32)uslength);
}


STATIC VOS_UINT32 DMS_NCM_CtrlOpen(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    dev->handle = DMS_INIT_HANDLE;

    /* 注册NDIS设备回调 */
    if (DMS_NcmExtFuncReg(DMS_NCM_RcvConnStatusChg, DMS_NCM_CtrlRcvData) != ERR_MSP_SUCCESS) {
        DMS_PORT_LOGE(dev->portId, "Reg ndis callback failed.\r");
        return VOS_ERR;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_CtrlSendDataAsync(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (DMS_COM_SEND((DMS_PHY_Bear)dev->devId, data, len) != VOS_OK) {
        DMS_PORT_LOGE(dev->portId, "write data async error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_DataOpen(struct DMS_DevInfo *dev)
{
    struct udi_open_param stParam;
    VOS_INT               handle = UDI_INVALID_HANDLE;

    DMS_PORT_LOGI(dev->portId, "enter.");

    stParam.devid = (enum udi_device_id)dev->devId;

    handle = mdrv_udi_open(&stParam);
    if (handle == UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(dev->portId, "dev handle is invalid.");
        return VOS_ERR;
    }

    dev->handle = handle;

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_DataClose(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (dev->handle != UDI_INVALID_HANDLE) {
        mdrv_udi_close(dev->handle);
        dev->handle = UDI_INVALID_HANDLE;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_SetIpv6Dns(struct DMS_DevInfo *dev, struct ncm_ipv6_dns *dns)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, NCM_IOCTL_SET_IPV6_DNS, dns) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set ipv6 dns error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_ChangeFlowCtrl(struct DMS_DevInfo *dev, VOS_UINT32 enableFlg)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, NCM_IOCTL_FLOW_CTRL_NOTIF, &enableFlg) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "ntf flow ctrl error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_ChangeConnSpeed(struct DMS_DevInfo *dev, struct ncm_ioctl_connection_speed *connSpeed)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, connSpeed) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "ntf speed error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_NCM_ChangeConnLinkState(struct DMS_DevInfo *dev, VOS_UINT32 linkStatus)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, NCM_IOCTL_NETWORK_CONNECTION_NOTIF, &linkStatus) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "ntf conn error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_VOID DMS_UART_RcvData(VOS_VOID)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    struct acm_wr_async_info      ctlParam;
    VOS_INT32                     handle;

    DMS_LOGI("enter.");

    if (DMS_COMM_CBCheck(DMS_PORT_UART) != VOS_OK) {
        return;
    }

    handle = DMS_DEV_GetDevHandlerByPortId(DMS_PORT_UART);

    /* 获取底软上行数据BUFFER */
    ctlParam.virt_addr = VOS_NULL_PTR;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(handle, UART_IOCTL_GET_RD_BUFF, &ctlParam) != VOS_OK) {
        DMS_LOGE("get buffer failed.");
        return;
    }

    if ((ctlParam.virt_addr == VOS_NULL_PTR) || (ctlParam.size == 0)) {
        DMS_LOGE("data buffer error.");
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_UART);
    if (portTblInfo->RcvData == VOS_NULL_PTR) {
        DMS_LOGE("ReadData is not registered.");
        return;
    }

    (VOS_VOID)portTblInfo->RcvData(DMS_PORT_UART, (VOS_UINT8 *)ctlParam.virt_addr, ctlParam.size);
}


STATIC VOS_UINT32 DMS_UART_Open(struct DMS_DevInfo *dev)
{
    struct udi_open_param stParam;
    VOS_INT               handle = UDI_INVALID_HANDLE;

    DMS_PORT_LOGI(dev->portId, "enter.");

    stParam.devid = (enum udi_device_id)dev->devId;

    handle = mdrv_udi_open(&stParam);
    if (handle == UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(dev->portId, "dev handle is invalid.");
        return VOS_ERR;
    }

    dev->handle = handle;

    /* 注册UART设备上行数据接收回调 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_READ_CB, DMS_UART_RcvData) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg uart read callback failed.\r");
        return VOS_ERR;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_UART_SendDataSync(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_write(dev->handle, data, len) != VOS_OK) {
        DMS_PORT_LOGE(dev->portId, "write data sync is error.");
        DMS_HSUART_DBG_DL_WRITE_SYNC_FAIL_LEN(len);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_HSUART_DBG_DL_WRITE_SYNC_SUCC_LEN(len);
    return VOS_OK;
}


STATIC VOS_VOID DMS_MDM_RcvData(VOS_VOID)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    struct acm_wr_async_info      ctlParam;
    VOS_INT32                     handle;
    IMM_Zc                       *immZc = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    DMS_MODEM_DBG_UL_DATA_READ_CB_NUM(1);

    if (DMS_COMM_CBCheck(DMS_PORT_MODEM) != VOS_OK) {
        return;
    }

    handle = DMS_DEV_GetDevHandlerByPortId(DMS_PORT_MODEM);

    /* 获取底软上行数据BUFFER */
    ctlParam.virt_addr = VOS_NULL_PTR;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(handle, ACM_IOCTL_GET_RD_BUFF, &ctlParam) != VOS_OK) {
        DMS_LOGE("get buffer failed.");
        DMS_MODEM_DBG_UL_GET_RD_FAIL_NUM(1);
        return;
    }

    if ((ctlParam.virt_addr == VOS_NULL_PTR) || (ctlParam.size == 0)) {
        DMS_LOGE("data buffer error.");
        DMS_MODEM_DBG_UL_INVALID_RD_NUM(1);
        return;
    }

    DMS_MODEM_DBG_UL_GET_RD_SUCC_NUM(1);
    immZc = (IMM_Zc *)ctlParam.virt_addr;

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_MODEM);
    if (portTblInfo->RcvPrivData == VOS_NULL_PTR) {
        DMS_LOGE("ReadSkbData is not registered.");
        return;
    }

    (VOS_VOID)portTblInfo->RcvPrivData(DMS_PORT_MODEM, immZc);
}


STATIC VOS_VOID DMS_MDM_RcvWriteDone(VOS_CHAR *buf)
{
    if (buf != VOS_NULL_PTR) {
        DMS_MODEM_DBG_DL_FREE_BUFF_NUM(1);
        IMM_ZcFree((IMM_Zc *)buf);
    }
}


STATIC VOS_VOID DMS_MDM_RcvMscCmd(MODEM_MSC_STRU *rcvedMsc)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    struct modem_msc_stru        *dceMsc      = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (rcvedMsc == VOS_NULL_PTR) {
        DMS_LOGE("rcvedMsc is null.");
        return;
    }

    dceMsc = (struct modem_msc_stru *)rcvedMsc;

    if (DMS_COMM_CBCheck(DMS_PORT_MODEM) != VOS_OK) {
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_MODEM);
    if (portTblInfo->RcvMscInd == VOS_NULL_PTR) {
        DMS_LOGE("ReadMsc is not registered.");
        return;
    }

    portTblInfo->RcvMscInd(DMS_PORT_MODEM, dceMsc);
}


STATIC VOS_UINT32 DMS_MDM_Open(struct DMS_DevInfo *dev)
{
    struct acm_read_buff_info readBuffInfo;
    struct udi_open_param     stParam;
    VOS_INT                   handle = UDI_INVALID_HANDLE;

    DMS_PORT_LOGI(dev->portId, "enter.");

    stParam.devid = (enum udi_device_id)dev->devId;

    handle = mdrv_udi_open(&stParam);
    if (handle == UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(dev->portId, "dev handle is invalid.");
        return VOS_ERR;
    }

    dev->handle = handle;

    /* 注册MODEM设备上行数据接收回调 */
    if (mdrv_udi_ioctl(dev->handle, ACM_IOCTL_SET_READ_CB, DMS_MDM_RcvData) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg MODEM read callback failed.\r");
        return VOS_ERR;
    }

    /* 注册UART下行数据内存释放接口 */
    if (mdrv_udi_ioctl(dev->handle, ACM_IOCTL_SET_FREE_CB, DMS_MDM_RcvWriteDone) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg MODEM write done callback failed.\r");
        return VOS_ERR;
    }

    /* 注册MODEM下行数据不需要拷贝 */
    if (mdrv_udi_ioctl(dev->handle, ACM_IOCTL_WRITE_DO_COPY, (void *)0) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Set not do copy for modem failed.\r");
        return VOS_ERR;
    }

    /* 注册管脚信号通知回调 */
    if (mdrv_udi_ioctl(dev->handle, ACM_MODEM_IOCTL_SET_MSC_READ_CB, DMS_MDM_RcvMscCmd) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg MODEM msc callback failed.\r");
        return VOS_ERR;
    }

    readBuffInfo.buff_size = DMS_MODEM_UL_DATA_BUFF_SIZE;
    readBuffInfo.buff_num  = DMS_MODEM_UL_DATA_BUFF_NUM;

    if (mdrv_udi_ioctl(dev->handle, ACM_IOCTL_RELLOC_READ_BUFF, &readBuffInfo) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Initialize data buffer failed.\r");
        return VOS_ERR;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_MDM_Close(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (dev->handle != UDI_INVALID_HANDLE) {
        mdrv_udi_close(dev->handle);
        dev->handle = UDI_INVALID_HANDLE;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_MDM_SendPrivData(struct DMS_DevInfo *dev, IMM_Zc *data)
{
    struct acm_wr_async_info ctlParam;

    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 将数据写往MODEM设备，写成功后内存由底软负责释放 */
    ctlParam.virt_addr = (VOS_CHAR *)data;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(dev->handle, ACM_IOCTL_WRITE_ASYNC, &ctlParam) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "write async fail.");
        DMS_MODEM_DBG_DL_WRITE_ASYNC_FAIL_NUM(1);
        IMM_ZcFree(data);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_MODEM_DBG_DL_WRITE_ASYNC_SUCC_NUM(1);
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_MDM_SendDataAsync(struct DMS_DevInfo *dev, VOS_UINT8 *dataBuff, VOS_UINT16 len)
{
    IMM_Zc   *data      = VOS_NULL_PTR;
    VOS_CHAR *zcPutData = VOS_NULL_PTR;
    errno_t   memResult;

    DMS_PORT_LOGI(dev->portId, "enter.");

    data = IMM_ZcStaticAlloc((VOS_UINT16)len);

    if (data == VOS_NULL_PTR) {
        DMS_PORT_LOGE(dev->portId, "alloc fail.");
        return VOS_ERR;
    }

    /* 此步骤不能少，用来偏移数据尾指针 */
    zcPutData = (VOS_CHAR *)IMM_ZcPut(data, len);

    memResult = memcpy_s(zcPutData, len, dataBuff, len);
    TAF_MEM_CHK_RTN_VAL(memResult, len, len);

    return DMS_MDM_SendPrivData(dev, data);
}


STATIC VOS_UINT32 DMS_MDM_FreeUlDataBuff(struct DMS_DevInfo *dev, VOS_VOID *data)
{
    struct acm_wr_async_info ctlParam;

    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 填写需要释放的内存指针 */
    ctlParam.virt_addr = (VOS_CHAR *)data;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(dev->handle, ACM_IOCTL_RETURN_BUFF, &ctlParam) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "free data error.");
        DMS_MODEM_DBG_UL_RETURN_BUFF_FAIL_NUM(1);
        IMM_ZcFree((IMM_Zc *)data);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_MODEM_DBG_UL_RETURN_BUFF_SUCC_NUM(1);
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_MDM_SendMscCmd(struct DMS_DevInfo *dev, struct modem_msc_stru *msc)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, ACM_MODEM_IOCTL_MSC_WRITE_CMD, msc) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set msc error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_SOCK_RcvData(VOS_UINT8 *data, VOS_UINT32 len)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (data == VOS_NULL_PTR) {
        DMS_LOGE("data is NULL PTR.");
        return VOS_ERR;
    }

    if (len == 0) {
        DMS_LOGE("len is 0.");
        return VOS_ERR;
    }

    if (DMS_COMM_CBCheck(DMS_PORT_SOCK) != VOS_OK) {
        return VOS_ERR;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_SOCK);
    if (portTblInfo->RcvData == VOS_NULL_PTR) {
        DMS_LOGE("ReadData is not registered.");
        return VOS_ERR;
    }

    if (portTblInfo->RcvData(DMS_PORT_SOCK, data, len) != VOS_OK) {
        return VOS_ERR;
    } else {
        return VOS_OK;
    }
}


STATIC VOS_UINT32 DMS_SOCK_Open(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    dev->handle = DMS_INIT_HANDLE;

    /* 注册Sock上行数据接收回调 */
    (VOS_VOID)mdrv_cpm_logic_rcv_reg((mdrv_cpm_logic_port_type_e)dev->devId, DMS_SOCK_RcvData);

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_SOCK_SendDataAsync(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_misc_support_check(BSP_MODULE_TYPE_WIFI) != BSP_MODULE_SUPPORT) {
        DMS_PORT_LOGE(dev->portId, "bsp module not support.");
        return VOS_ERR;
    }

    return mdrv_cpm_com_send(dev->devId, data, VOS_NULL_PTR, len);
}

#if (FEATURE_AT_HSUART == FEATURE_ON)

STATIC VOS_VOID DMS_HSUART_RcvData(VOS_VOID)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;
    IMM_Zc                       *immZc       = VOS_NULL_PTR;
    struct acm_wr_async_info      ctlParam;
    VOS_INT32                     handle;

    DMS_LOGI("enter.");
    DMS_HSUART_DBG_UL_DATA_READ_CB_NUM(1);

    if (DMS_COMM_CBCheck(DMS_PORT_HSUART) != VOS_OK) {
        return;
    }

    handle = DMS_DEV_GetDevHandlerByPortId(DMS_PORT_HSUART);

    /* 获取底软上行数据BUFFER */
    ctlParam.virt_addr = VOS_NULL_PTR;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(handle, UART_IOCTL_GET_RD_BUFF, &ctlParam) != VOS_OK) {
        DMS_LOGE("get buffer failed.");
        DMS_HSUART_DBG_UL_GET_RD_FAIL_NUM(1);
        return;
    }

    if ((ctlParam.virt_addr == VOS_NULL_PTR) || (ctlParam.size == 0)) {
        DMS_LOGE("data buffer error.");
        DMS_HSUART_DBG_UL_INVALID_RD_NUM(1);
        return;
    }

    DMS_HSUART_DBG_UL_GET_RD_SUCC_NUM(1);
    immZc = (IMM_Zc *)ctlParam.virt_addr;

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_HSUART);
    if (portTblInfo->RcvPrivData == VOS_NULL_PTR) {
        DMS_LOGE("ReadSkbData is not registered.");
        return;
    }

    (VOS_VOID)portTblInfo->RcvPrivData(DMS_PORT_HSUART, immZc);
}


STATIC VOS_VOID DMS_HSUART_RcvMscCmd(struct modem_msc_stru *dceMsc)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");
    DMS_HSUART_DBG_IOCTL_MSC_READ_CB_NUM(1);

    if (dceMsc == VOS_NULL_PTR) {
        DMS_LOGE("dceMsc is null.");
        return;
    }

    if (DMS_COMM_CBCheck(DMS_PORT_HSUART) != VOS_OK) {
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_HSUART);
    if (portTblInfo->RcvMscInd == VOS_NULL_PTR) {
        DMS_LOGE("ReadMsc is not registered.");
        return;
    }

    portTblInfo->RcvMscInd(DMS_PORT_HSUART, dceMsc);
}


STATIC VOS_VOID DMS_HSUART_RcvEscapeSequence(VOS_VOID)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");
    DMS_HSUART_DBG_IOCTL_SWITCH_CB_NUM(1);

    if (DMS_COMM_CBCheck(DMS_PORT_HSUART) != VOS_OK) {
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_HSUART);
    if (portTblInfo->RcvEscapeSequence == VOS_NULL_PTR) {
        DMS_LOGE("RcvEscapeSequence is not registered.");
        return;
    }

    portTblInfo->RcvEscapeSequence(DMS_PORT_HSUART);
}


STATIC VOS_VOID DMS_HSUART_RcvWaterMarkDetectInd(water_level level)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_LOGI("enter.");

    if (level != HIGH_LEVEL && level != LOW_LEVEL) {
        DMS_LOGE("level abnormal: %d.", level);
        return;
    }

    if (DMS_COMM_CBCheck(DMS_PORT_HSUART) != VOS_OK) {
        return;
    }

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(DMS_PORT_HSUART);
    if (portTblInfo->RcvWaterMarkInd == VOS_NULL_PTR) {
        DMS_LOGE("RcvWaterMarkInd is not registered.");
        return;
    }

    portTblInfo->RcvWaterMarkInd(DMS_PORT_HSUART, level);
}


STATIC VOS_VOID DMS_HSUART_RcvWriteDone(IMM_Zc *immZc)
{
    if (immZc != VOS_NULL_PTR) {
        DMS_HSUART_DBG_DL_FREE_BUFF_NUM(1);
        IMM_ZcFree(immZc);
    }
}


STATIC VOS_UINT32 DMS_HSUART_Open(struct DMS_DevInfo *dev)
{
    struct acm_read_buff_info stReadBuffInfo;
    struct udi_open_param     stParam;
    VOS_INT                   handle = UDI_INVALID_HANDLE;

    DMS_PORT_LOGI(dev->portId, "enter.");

    stParam.devid = (enum udi_device_id)dev->devId;

    handle = mdrv_udi_open(&stParam);
    if (handle == UDI_INVALID_HANDLE) {
        DMS_PORT_LOGE(dev->portId, "dev handle is invalid.");
        return VOS_ERR;
    }

    dev->handle = handle;

    stReadBuffInfo.buff_size = DMS_UART_UL_DATA_BUFF_SIZE;
    stReadBuffInfo.buff_num  = DMS_UART_UL_DATA_BUFF_NUM;

    /* 注册HSUART设备上行数据接收回调 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_READ_CB, DMS_HSUART_RcvData) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg hsuart read callback failed.\r");
        return VOS_ERR;
    }

    /* 设置UART设备上行数据缓存规格 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Set read buff callback for hsuart failed.\r");
        return VOS_ERR;
    }

    /* 注册UART下行数据内存释放接口 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_FREE_CB, DMS_HSUART_RcvWriteDone) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg hsuart write done callback failed.\r");
        return VOS_ERR;
    }

    /* 注册管脚信号通知回调 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_MSC_READ_CB, DMS_HSUART_RcvMscCmd) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg hsuart msc callback failed.\r");
        return VOS_ERR;
    }

    /* 注册"+++"命令检测回调 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SWITCH_MODE_CB, DMS_HSUART_RcvEscapeSequence) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg hsuart escape sequence callback failed.\r");
        return VOS_ERR;
    }

    /* 注册HSUART端口水线监测回调 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_WATER_CB, DMS_HSUART_RcvWaterMarkDetectInd) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Reg hsuart watermark callback failed.\r");
        return VOS_ERR;
    }

    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_SendPrivData(struct DMS_DevInfo *dev, IMM_Zc *data)
{
    struct acm_wr_async_info ctlParam;

    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 将数据写往hsuart设备，写成功后内存由底软负责释放 */
    ctlParam.virt_addr = (VOS_CHAR *)data;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_WRITE_ASYNC, &ctlParam) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "write async fail.");
        DMS_HSUART_DBG_DL_WRITE_ASYNC_FAIL_NUM(1);
        IMM_ZcFree(data);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_HSUART_DBG_DL_WRITE_ASYNC_SUCC_NUM(1);
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_SendDataAsync(struct DMS_DevInfo *dev, VOS_UINT8 *data, VOS_UINT16 len)
{
    IMM_Zc   *immZc   = VOS_NULL_PTR;
    VOS_CHAR *putData = VOS_NULL_PTR;
    errno_t   memResult;

    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 从A核数传内存中分配空间 */
    immZc = IMM_ZcStaticAlloc((VOS_UINT16)len);
    if (immZc == VOS_NULL_PTR) {
        DMS_PORT_LOGE(dev->portId, "alloc fail.");
        return VOS_ERR;
    }

    /* 偏移数据尾指针 */
    putData = (VOS_CHAR *)IMM_ZcPut(immZc, len);

    /* 拷贝数据 */
    memResult = memcpy_s(putData, len, data, len);
    TAF_MEM_CHK_RTN_VAL(memResult, len, len);

    return DMS_HSUART_SendPrivData(dev, immZc);
}


STATIC VOS_UINT32 DMS_HSUART_SendMscCmd(struct DMS_DevInfo *dev, struct modem_msc_stru *msc)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_MSC_WRITE_CMD, (VOS_VOID *)msc) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set msc error.");
        DMS_HSUART_DBG_IOCTL_MSC_WRITE_FAIL_NUM(1);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_HSUART_DBG_IOCTL_MSC_WRITE_SUCC_NUM(1);
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_CfgFlowCtrl(struct DMS_DevInfo *dev, VOS_UINT32 rtsFlag, VOS_UINT32 ctsFlag)
{
    uart_flow_ctrl_union unFlowCtrlValue;

    DMS_PORT_LOGI(dev->portId, "enter.");

    (VOS_VOID)memset_s(&unFlowCtrlValue, sizeof(unFlowCtrlValue), 0x00, sizeof(unFlowCtrlValue));

    unFlowCtrlValue.reg.rtsen = rtsFlag;
    unFlowCtrlValue.reg.ctsen = ctsFlag;

    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_FLOW_CONTROL, &unFlowCtrlValue) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "cfg flow ctrl error.");
        DMS_HSUART_DBG_IOCTL_CFG_FC_FAIL_NUM(1);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_HSUART_DBG_IOCTL_CFG_FC_SUCC_NUM(1);
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_SetCharFrame(struct DMS_DevInfo *dev, VOS_UINT32 udiDataLen, VOS_UINT32 udiStpLen,
                                          VOS_UINT32 udiParity)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 调用DRV函数设置串口数据位长度 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_WLEN, (VOS_VOID *)&udiDataLen) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set wlen error.");
        return VOS_ERR;
    }

    /* 调用DRV函数设置串口停止位长度 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_STP2, (VOS_VOID *)&udiStpLen) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set stp error.");
        return VOS_ERR;
    }

    /* 调用DRV函数设置串口的校验位 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_EPS, (VOS_VOID *)&udiParity) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set eps error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_SetBaud(struct DMS_DevInfo *dev, VOS_UINT32 baudRate)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 调用DRV函数设置串口的波特率 */
    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_BAUD, (VOS_VOID *)&baudRate) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set baud rate error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_SetACShell(struct DMS_DevInfo *dev, VOS_UINT32 mode)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_SET_AC_SHELL, &mode) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "set ac shell error.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_FreeUlDataBuff(struct DMS_DevInfo *dev, VOS_VOID *data)
{
    struct acm_wr_async_info ctlParam;

    DMS_PORT_LOGI(dev->portId, "enter.");

    /* 填写需要释放的内存指针 */
    ctlParam.virt_addr = (VOS_CHAR *)data;
    ctlParam.phy_addr  = VOS_NULL_PTR;
    ctlParam.size      = 0;
    ctlParam.priv      = VOS_NULL_PTR;

    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_RETURN_BUFF, &ctlParam) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "free data error.");
        DMS_HSUART_DBG_UL_RETURN_BUFF_FAIL_NUM(1);
        IMM_ZcFree((IMM_Zc *)data);
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    DMS_HSUART_DBG_UL_RETURN_BUFF_SUCC_NUM(1);
    return VOS_OK;
}


STATIC VOS_UINT32 DMS_HSUART_FlushTxData(struct DMS_DevInfo *dev)
{
    DMS_PORT_LOGI(dev->portId, "enter.");

    if (mdrv_udi_ioctl(dev->handle, UART_IOCTL_RELEASE_BUFF, VOS_NULL_PTR) != MDRV_OK) {
        DMS_PORT_LOGE(dev->portId, "Release buffer failed.");
        return VOS_ERR;
    }

    DMS_PORT_LOGI(dev->portId, "success.");
    return VOS_OK;
}
#endif

STATIC const struct DMS_DevOps g_dmsDevOps[] = {
    [DMS_DEV_TYPE_APPVCOM] = {
        .Open          = DMS_APP_Open,
        .SendDataAsync = DMS_APP_SendDataAsync,
    },

    [DMS_DEV_TYPE_ACM_COM] = {
        .Open          = DMS_ACM_Open,
        .SendDataAsync = DMS_ACM_SendDataAsync,
    },

    [DMS_DEV_TYPE_NCM_CTRL] = {
        .Open          = DMS_NCM_CtrlOpen,
        .SendDataAsync = DMS_NCM_CtrlSendDataAsync,
    },

    [DMS_DEV_TYPE_NCM_DATA] = {
        .Open                = DMS_NCM_DataOpen,
        .Close               = DMS_NCM_DataClose,
        .SetIpv6Dns          = DMS_NCM_SetIpv6Dns,
        .ChangeFlowCtrl      = DMS_NCM_ChangeFlowCtrl,
        .ChangeConnSpeed     = DMS_NCM_ChangeConnSpeed,
        .ChangeConnLinkState = DMS_NCM_ChangeConnLinkState,
    },

    [DMS_DEV_TYPE_ACM_MODEM] = {
        .Open           = DMS_MDM_Open,
        .Close          = DMS_MDM_Close,
        .SendDataAsync  = DMS_MDM_SendDataAsync,
        .SendMscCmd     = DMS_MDM_SendMscCmd,
        .FreeUlDataBuff = DMS_MDM_FreeUlDataBuff,
        .SendPrivData   = DMS_MDM_SendPrivData,
    },

    [DMS_DEV_TYPE_UART] = {
        .Open         = DMS_UART_Open,
        .SendDataSync = DMS_UART_SendDataSync,
    },
#if (FEATURE_AT_HSUART == FEATURE_ON)
    [DMS_DEV_TYPE_HSUART] = {
        .Open           = DMS_HSUART_Open,
        .SendDataSync   = DMS_UART_SendDataSync,
        .SendDataAsync  = DMS_HSUART_SendDataAsync,
        .SendMscCmd     = DMS_HSUART_SendMscCmd,
        .ConfigFlowCtrl = DMS_HSUART_CfgFlowCtrl,
        .SetCharFrame   = DMS_HSUART_SetCharFrame,
        .SetBaud        = DMS_HSUART_SetBaud,
        .SetACShell     = DMS_HSUART_SetACShell,
        .FreeUlDataBuff = DMS_HSUART_FreeUlDataBuff,
        .SendPrivData   = DMS_HSUART_SendPrivData,
        .FlushTxData    = DMS_HSUART_FlushTxData,
    },
#endif
    [DMS_DEV_TYPE_SOCK] = {
        .Open          = DMS_SOCK_Open,
        .SendDataAsync = DMS_SOCK_SendDataAsync,
    },
};

struct DMS_DevInfo g_dmsDevInfo[] = {
    DMS_DEV_ITEM(DMS_PORT_APP, APP_VCOM_DEV_INDEX_0, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP1, APP_VCOM_DEV_INDEX_1, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
#if (FEATURE_VCOM_EXT == FEATURE_ON)
    DMS_DEV_ITEM(DMS_PORT_APP2, APP_VCOM_DEV_INDEX_2, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP3, APP_VCOM_DEV_INDEX_3, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP4, APP_VCOM_DEV_INDEX_4, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP5, APP_VCOM_DEV_INDEX_5, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP6, APP_VCOM_DEV_INDEX_6, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP7, APP_VCOM_DEV_INDEX_7, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP8, APP_VCOM_DEV_INDEX_8, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP9, APP_VCOM_DEV_INDEX_9, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP10, APP_VCOM_DEV_INDEX_10, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP11, APP_VCOM_DEV_INDEX_11, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP12, APP_VCOM_DEV_INDEX_12, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP13, APP_VCOM_DEV_INDEX_13, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP14, APP_VCOM_DEV_INDEX_14, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP15, APP_VCOM_DEV_INDEX_15, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP16, APP_VCOM_DEV_INDEX_16, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP17, APP_VCOM_DEV_INDEX_17, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP18, APP_VCOM_DEV_INDEX_18, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP19, APP_VCOM_DEV_INDEX_19, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP20, APP_VCOM_DEV_INDEX_20, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP21, APP_VCOM_DEV_INDEX_21, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP22, APP_VCOM_DEV_INDEX_22, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP23, APP_VCOM_DEV_INDEX_23, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP24, APP_VCOM_DEV_INDEX_24, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP25, APP_VCOM_DEV_INDEX_25, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP26, APP_VCOM_DEV_INDEX_26, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP27, APP_VCOM_DEV_INDEX_27, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP28, APP_VCOM_DEV_INDEX_28, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP29, APP_VCOM_DEV_INDEX_29, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP30, APP_VCOM_DEV_INDEX_30, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP31, APP_VCOM_DEV_INDEX_31, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP32, APP_VCOM_DEV_INDEX_32, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP33, APP_VCOM_DEV_INDEX_33, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP34, APP_VCOM_DEV_INDEX_34, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP35, APP_VCOM_DEV_INDEX_35, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP36, APP_VCOM_DEV_INDEX_36, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP37, APP_VCOM_DEV_INDEX_37, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP38, APP_VCOM_DEV_INDEX_38, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP39, APP_VCOM_DEV_INDEX_39, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP40, APP_VCOM_DEV_INDEX_40, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP41, APP_VCOM_DEV_INDEX_41, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP42, APP_VCOM_DEV_INDEX_42, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP43, APP_VCOM_DEV_INDEX_43, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP44, APP_VCOM_DEV_INDEX_44, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP45, APP_VCOM_DEV_INDEX_45, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP46, APP_VCOM_DEV_INDEX_46, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP47, APP_VCOM_DEV_INDEX_47, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP48, APP_VCOM_DEV_INDEX_48, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP49, APP_VCOM_DEV_INDEX_49, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP50, APP_VCOM_DEV_INDEX_50, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP51, APP_VCOM_DEV_INDEX_51, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
    DMS_DEV_ITEM(DMS_PORT_APP52, APP_VCOM_DEV_INDEX_52, &g_dmsDevOps[DMS_DEV_TYPE_APPVCOM]),
#endif

    DMS_DEV_ITEM(DMS_PORT_PCUI, DMS_PHY_BEAR_USB_PCUI, &g_dmsDevOps[DMS_DEV_TYPE_ACM_COM]),
    DMS_DEV_ITEM(DMS_PORT_CTRL, DMS_PHY_BEAR_USB_CTRL, &g_dmsDevOps[DMS_DEV_TYPE_ACM_COM]),
    DMS_DEV_ITEM(DMS_PORT_PCUI2, DMS_PHY_BEAR_USB_PCUI2, &g_dmsDevOps[DMS_DEV_TYPE_ACM_COM]),
    DMS_DEV_ITEM(DMS_PORT_NCM_CTRL, DMS_PHY_BEAR_USB_NCM, &g_dmsDevOps[DMS_DEV_TYPE_NCM_CTRL]),
    DMS_DEV_ITEM(DMS_PORT_UART, UDI_UART_0_ID, &g_dmsDevOps[DMS_DEV_TYPE_UART]),
    DMS_DEV_ITEM(DMS_PORT_SOCK, CPM_AT_COMM, &g_dmsDevOps[DMS_DEV_TYPE_SOCK]),
    DMS_DEV_ITEM(DMS_PORT_MODEM, UDI_ACM_MODEM_ID, &g_dmsDevOps[DMS_DEV_TYPE_ACM_MODEM]),

#if (FEATURE_AT_HSUART == FEATURE_ON)
    DMS_DEV_ITEM(DMS_PORT_HSUART, UDI_HSUART_0_ID, &g_dmsDevOps[DMS_DEV_TYPE_HSUART]),
#endif

    DMS_DEV_ITEM(DMS_PORT_NCM_DATA, UDI_NCM_NDIS_ID, &g_dmsDevOps[DMS_DEV_TYPE_NCM_DATA]),
};


VOS_VOID DMS_DEV_RcvConnectStateInd(DMS_PortIdUint16 portId, VOS_UINT8 connState)
{
    const struct DMS_PortFuncTbl *portTblInfo = VOS_NULL_PTR;

    DMS_PORT_LOGI(portId, "enter.");

    portTblInfo = DMS_DEV_GetDevPortTblByPortId(portId);
    if (portTblInfo == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "portTblInfo not registered.");
        return;
    }

    if (portTblInfo->RcvConnectInd == VOS_NULL_PTR) {
        DMS_PORT_LOGE(portId, "RcvConnectInd is not registered.");
        return;
    }

    portTblInfo->RcvConnectInd(portId, connState);
}

