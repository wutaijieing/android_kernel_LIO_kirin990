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
#include "taf_type_def.h"
#include "securec.h"
#include "dms_debug.h"
#include "dms_port_i.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */

#define THIS_FILE_ID PS_FILE_ID_DMS_ACM_AT_TX_C

DMS_StaticBuf g_dmsStaticBufInfo;

#if (VOS_WIN32 == VOS_OS_VER)
VOS_UINT8 g_staticBuffer[DMS_LOG_STATIC_ONE_BUF_SIZE * DMS_LOG_STATIC_BUF_NUM + 32] = {0};
#endif
VOS_UINT8 *g_staticBuf = NULL;


VOS_INT32 DMS_WriteData(DMS_PHY_Bear phyBear, VOS_UINT8 *data, VOS_UINT16 len)
{
    VOS_UINT8 *senBuf = NULL;
    VOS_INT32  ret    = VOS_ERROR;
    errno_t    memResult;

    if ((NULL == data) || (0 == len)) {
        return VOS_ERROR;
    }

    DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "len = %d, buf = %s", len, data);

    if ((DMS_PHY_BEAR_USB_PCUI == phyBear) || (DMS_PHY_BEAR_USB_CTRL == phyBear) ||
        (DMS_PHY_BEAR_USB_PCUI2 == phyBear)) {
        senBuf = Dms_GetStaticBuf(len);

        if (NULL == senBuf) {
            return VOS_ERROR;
        }

        memResult = memcpy_s(senBuf, len, data, len);
        TAF_MEM_CHK_RTN_VAL(memResult, len, len);

        ret = (VOS_INT32)DMS_VcomWriteAsync(phyBear, senBuf, len);

        if (ERR_MSP_SUCCESS != ret) {
            Dms_FreeStaticBuf(senBuf);
        }

    } else if (DMS_PHY_BEAR_USB_NCM == phyBear) {
        senBuf = Dms_GetStaticBuf(len);

        if (NULL == senBuf) {
            return VOS_ERROR;
        }

        memResult = memcpy_s(senBuf, len, data, len);
        TAF_MEM_CHK_RTN_VAL(memResult, len, len);

        ret = (VOS_INT32)DMS_NcmSendData(senBuf, len);

        if (ERR_MSP_SUCCESS != ret) {
            Dms_FreeStaticBuf(senBuf);
        }
    } else {
        ret = VOS_ERROR;
    }

    return ret;
}


VOS_VOID Dms_StaticBufInit(VOS_VOID)
{
    VOS_UINT32 i    = 0;
    VOS_UINT8 *temp = NULL;

#if (VOS_LINUX == VOS_OS_VER)
    VOS_UINT32 bufSize;

    bufSize     = (DMS_LOG_STATIC_ONE_BUF_SIZE * DMS_LOG_STATIC_BUF_NUM + 32);
    g_staticBuf = kmalloc(bufSize, GFP_KERNEL | __GFP_DMA);

    if (g_staticBuf == VOS_NULL) {
        return;
    }
#endif

#if (VOS_WIN32 == VOS_OS_VER)
    g_staticBuf = g_staticBuffer;
#endif

    /* 取32字节对齐的地址 */
    temp = g_staticBuf + (32 - ((VOS_UINT_PTR)g_staticBuf % 32));

    g_dmsStaticBufInfo.bufType = DMS_BUF_TYP_DYMIC;

    /* 初始化缓冲信息 */
    for (i = 0; i < DMS_LOG_STATIC_BUF_NUM; i++) {
        g_dmsStaticBufInfo.bufSta[i].pcuBuf = (VOS_UINT8 *)((VOS_ULONG)i * DMS_LOG_STATIC_ONE_BUF_SIZE + temp);
        g_dmsStaticBufInfo.bufSta[i].busy   = STATIC_BUF_STA_IDLE;
    }

    return;
}

VOS_UINT8* Dms_GetStaticBuf(VOS_UINT32 len)
{
    VOS_UINT32  i   = 0;
    VOS_UINT32 *buf = NULL;

    /* 数据长度超过静态buf最大size，动态申请buf */
    if (len > DMS_LOG_STATIC_ONE_BUF_SIZE) {
#if (VOS_LINUX == VOS_OS_VER)
        buf = kmalloc(len, GFP_KERNEL | __GFP_DMA);
#else
        buf = VOS_NULL;
#endif
        return (VOS_UINT8 *)buf;
    }

    for (i = 0; i < DMS_LOG_STATIC_BUF_NUM; i++) {
        if (g_dmsStaticBufInfo.bufSta[i].busy == STATIC_BUF_STA_IDLE) {
            g_dmsStaticBufInfo.bufSta[i].busy = STATIC_BUF_STA_BUSY;

            return g_dmsStaticBufInfo.bufSta[i].pcuBuf;
        }
    }

    /* 极限场景下 如果静态buf用完，申请动态内存使用 */
#if (VOS_LINUX == VOS_OS_VER)
    buf = kmalloc(len, GFP_KERNEL | __GFP_DMA);
#else
    buf = VOS_NULL;
#endif

    return (VOS_UINT8 *)buf;
}



VOS_BOOL Dms_IsStaticBuf(VOS_UINT8 *buf)
{
    if ((buf >= g_staticBuf) && (buf < g_staticBuf + DMS_LOG_STATIC_ONE_BUF_SIZE * DMS_LOG_STATIC_BUF_NUM + 32)) {
        return VOS_TRUE;
    } else {
        return VOS_FALSE;
    }
}


VOS_VOID Dms_FreeStaticBuf(VOS_UINT8 *buf)
{
    VOS_UINT32 i = 0;

    if (NULL == buf) {
        return;
    }

    /* 静态buf释放 */
    for (i = 0; i < DMS_LOG_STATIC_BUF_NUM; i++) {
        if (g_dmsStaticBufInfo.bufSta[i].pcuBuf == buf) {
            g_dmsStaticBufInfo.bufSta[i].busy = STATIC_BUF_STA_IDLE;
            return;
        }
    }

    /* 动态buf释放 */
    if (i == DMS_LOG_STATIC_BUF_NUM) {
#if (VOS_LINUX == VOS_OS_VER)
        kfree(buf);
#endif
    }

    return;
}

VOS_UINT32 DMS_VcomWriteAsync(DMS_PHY_Bear phyBear, VOS_UINT8 *dataBuf, VOS_UINT32 len)
{
    DMS_PHY_BearProperty    *phyBearProp = NULL;
    struct acm_wr_async_info acmInfo     = {0};
    int                      handle      = UDI_INVALID_HANDLE;
    VOS_INT32                ret         = ERR_MSP_SUCCESS;

    acmInfo.virt_addr = (VOS_CHAR *)dataBuf;
    acmInfo.phy_addr  = NULL;
    acmInfo.size      = len;

    phyBearProp = DMS_GetPhyBearProperty(phyBear);

    handle = phyBearProp->portHandle;
    if (UDI_INVALID_HANDLE == handle) {
        DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "INVALID HANDLE.");
        return ERR_MSP_FAILURE;
    }

    if (ACM_EVT_DEV_SUSPEND == phyBearProp->chanStat) {
        DMS_PORT_LOGI(phyBear + DMS_PORT_PCUI, "DEV SUSPEND.");
        return ERR_MSP_FAILURE;
    }

    DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_WRT_BEGIN + (VOS_UINT32)phyBear), len, 0, 0);

    ret = mdrv_udi_ioctl(handle, ACM_IOCTL_WRITE_ASYNC, &acmInfo);
    if (ERR_MSP_SUCCESS == ret) {
        DMS_DBG_SDM_FUN((DMS_SDM_MsgId)(DMS_SDM_VCOM_WRT_SUSS_BEGIN + (VOS_UINT32)phyBear), len, 0, 0);
    }

    return (VOS_UINT32)ret;
}

