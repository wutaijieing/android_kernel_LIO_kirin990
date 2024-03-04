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

#ifndef __DMS_H__
#define __DMS_H__

#include "mdrv.h"
#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DMS_LOG_STATIC_BUF_NUM 16
#define DMS_LOG_STATIC_ONE_BUF_SIZE 5120

/* 以下的枚举值不能随便修改 */
typedef enum {
    DMS_PHY_BEAR_USB_PCUI,
    DMS_PHY_BEAR_USB_CTRL,
    DMS_PHY_BEAR_USB_PCUI2,
    DMS_PHY_BEAR_USB_NCM,
    DMS_PHY_BEAR_LAST
} DMS_PHY_Bear;

typedef enum {
    STATIC_BUF_STA_IDLE = 0,
    STATIC_BUF_STA_BUSY,
    STATIC_BUF_STA_BUTT
} DMS_BufStatus;

typedef enum {
    DMS_BUF_TYP_DYMIC = 0,
    DMS_BUF_TYP_STATIC,
    DMS_BUF_TYP_BUTT
} DMS_BufTyp;

typedef struct {
    VOS_UINT8    *pcuBuf;
    DMS_BufStatus busy;
    VOS_UINT8     reserved[4];
} DMS_BufSta;

typedef struct {
    DMS_BufTyp bufType;
    VOS_UINT8  reserved[4];
    DMS_BufSta bufSta[DMS_LOG_STATIC_BUF_NUM];
} DMS_StaticBuf;

typedef VOS_UINT32 (*DMS_READ_DATA_PFN)(DMS_PHY_Bear enPhyBear, VOS_UINT8 *pDataBuf, VOS_UINT32 ulLen);

typedef int (*USB_NAS_AT_CMD_RECV)(unsigned char *pBuff, unsigned short usLen);
typedef int (*pComRecv)(unsigned char uPortNo, unsigned char *pData, unsigned short uslength);

extern VOS_INT32 DMS_ACMRecvFuncReg(pComRecv callback);

extern int DMS_NcmExtFuncReg(usb_ndis_stus_chg_func connectBrk, USB_NAS_AT_CMD_RECV atCmdRcv);

extern VOS_INT32 DMS_WriteData(DMS_PHY_Bear phyBear, VOS_UINT8 *data, VOS_UINT16 len);
#define DMS_COM_SEND(uPortNo, pData, uslength) DMS_WriteData(uPortNo, pData, uslength)

extern VOS_VOID   Dms_FreeStaticBuf(VOS_UINT8 *buf);
extern VOS_UINT8* Dms_GetStaticBuf(VOS_UINT32 len);
extern VOS_BOOL   Dms_IsStaticBuf(VOS_UINT8 *buf);
extern VOS_VOID   Dms_StaticBufInit(VOS_VOID);
extern VOS_UINT32 DMS_VcomPcuiOpen(VOS_VOID);
extern VOS_UINT32 DMS_VcomCtrlOpen(VOS_VOID);
extern VOS_UINT32 DMS_VcomPcui2Open(VOS_VOID);
extern VOS_UINT32 DMS_VcomPcuiClose(VOS_VOID);
extern VOS_UINT32 DMS_VcomCtrlClose(VOS_VOID);
extern VOS_UINT32 DMS_VcomPcui2Close(VOS_VOID);
extern VOS_VOID   DMS_VcomPcuiEvtCB(ACM_EVT_E evt);
extern VOS_VOID   DMS_VcomCtrlEvtCB(ACM_EVT_E evt);
extern VOS_VOID   DMS_VcomPcui2EvtCB(ACM_EVT_E evt);
extern VOS_VOID   DMS_VcomPcuiReadCB(VOS_VOID);
extern VOS_VOID   DMS_VcomCtrlReadCB(VOS_VOID);
extern VOS_VOID   DMS_VcomPcui2ReadCB(VOS_VOID);
extern VOS_VOID   DMS_VcomPcuiWrtCB(VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize);
extern VOS_VOID   DMS_VcomCtrlWrtCB(VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize);
extern VOS_VOID   DMS_VcomPcui2WrtCB(VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize);
extern VOS_VOID   DMS_UsbPortReadCB(DMS_PHY_Bear phyBear);
extern VOS_VOID   DMS_UsbPortWrtCB(DMS_PHY_Bear phyBear, VOS_CHAR *pcVirAddr, VOS_CHAR *pcPhyAddr, VOS_INT doneSize);
extern VOS_VOID   DMS_UsbPortEvtCB(DMS_PHY_Bear phyBear, ACM_EVT_E evt);

extern VOS_UINT32        DMS_NcmOpen(VOS_VOID);
extern VOS_UINT32        DMS_NcmClose(VOS_VOID);
extern VOS_UINT32        DMS_NcmSendData(VOS_UINT8 *data, VOS_UINT32 len);
extern VOS_UINT32        DMS_VcomWriteAsync(DMS_PHY_Bear phyBear, VOS_UINT8 *dataBuf, VOS_UINT32 len);
extern DMS_READ_DATA_PFN DMS_GetDataReadFun(VOS_VOID);

extern VOS_VOID DMS_SetNdisChanStatus(ACM_EVT_E status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DMS_H__ */
