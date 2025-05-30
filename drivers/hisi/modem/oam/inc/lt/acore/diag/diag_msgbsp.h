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


#ifndef __DIAG_MSGBSP_H__
#define __DIAG_MSGBSP_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "securec.h"
#include <vos.h>
#include <mdrv.h>
#include <msp_errno.h>
#include <mdrv_diag_system.h>
#include <msp_diag_comm.h>
#include "diag_common.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 macro
*****************************************************************************/

#define DIAG_MSG_BSP_ACORE_CFG_PROC(ulLen, pstDiagHead, pstInfo, ret) \
do {    \
    ulLen = sizeof(DIAG_BSP_MSG_A_TRANS_C_STRU)-VOS_MSG_HEAD_LENGTH + pstDiagHead->ulMsgLen;  \
    pstInfo = (DIAG_BSP_MSG_A_TRANS_C_STRU*)VOS_AllocMsg(MSP_PID_DIAG_APP_AGENT, ulLen);  \
    if(VOS_NULL == pstInfo) \
    {   \
        ret = ERR_MSP_MALLOC_FAILUE;    \
        goto DIAG_ERROR;    \
    }   \
    pstInfo->ulReceiverPid = MSP_PID_DIAG_AGENT;    \
    pstInfo->ulSenderPid   = MSP_PID_DIAG_APP_AGENT;    \
    pstInfo->ulMsgId       = DIAG_MSG_BSP_A_TRANS_C_REQ;    \
    ulLen = sizeof(msp_diag_frame_info_s)+pstDiagHead->ulMsgLen; \
    ret = memcpy_s(&pstInfo->stInfo, ulLen, pstDiagHead, ulLen);   \
    if(ret) \
    {   \
        diag_error("memcpy_s fail, ret=0x%x\n", ret); \
    } \
    ret = VOS_SendMsg(MSP_PID_DIAG_APP_AGENT, pstInfo); \
    if(ret) \
    {   \
        goto DIAG_ERROR;    \
    }   \
}while(0)
#define DIAG_MSG_BSP_CFG_ACORE_TO_NRM(ulLen, pstDiagHead, pstInfo, ret) \
    do {    \
        ulLen = sizeof(DIAG_BSP_MSG_A_TRANS_C_STRU)- VOS_MSG_HEAD_LENGTH + pstDiagHead->ulMsgLen;  \
        pstInfo = (DIAG_BSP_MSG_A_TRANS_C_STRU*)VOS_AllocMsg(MSP_PID_DIAG_APP_AGENT, ulLen);  \
        if(VOS_NULL == pstInfo) \
        {   \
            ret = ERR_MSP_MALLOC_FAILUE;    \
            goto DIAG_ERROR;    \
        }   \
        pstInfo->ulReceiverPid = MSP_PID_DIAG_NRM_AGENT;    \
        pstInfo->ulSenderPid   = MSP_PID_DIAG_APP_AGENT;    \
        pstInfo->ulMsgId       = DIAG_MSG_BSP_A_TRANS_C_REQ;    \
        ulLen = sizeof(msp_diag_frame_info_s)+pstDiagHead->ulMsgLen; \
        ret = (VOS_UINT32)memcpy_s(&pstInfo->stInfo, ulLen, pstDiagHead, ulLen);   \
        if(ret) \
        {   \
            diag_error("memcpy_s fail, ret=0x%x\n", ret); \
        } \
        ret = VOS_SendMsg(MSP_PID_DIAG_APP_AGENT, pstInfo); \
        if(ret) \
        {   \
            goto DIAG_ERROR;    \
        }   \
    }while(0)

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/

enum
{
    DIAG_LEVEL_NORMAL = 0,
    DIAG_LEVEL_ADVANCED
};

/*****************************************************************************
   5 STRUCT
*****************************************************************************/

typedef struct
{
    VOS_UINT32  ulAuid;                     /* 原AUID*/
    VOS_UINT32  ulSn;                       /* HSO分发，插件命令管理*/
    VOS_UINT32  ulRet;
}diag_comm_cnf_s;


typedef VOS_UINT32 (*DIAG_BSP_PROC)(VOS_UINT8 *pData);
typedef struct
{
    DIAG_BSP_PROC   pFunc;
    VOS_UINT32      ulCmdId;
    VOS_UINT32      ulReserve;
}DIAG_BSP_PROC_FUN_STRU;

typedef struct
{
    VOS_UINT32  ulChannelNum;
    VOS_UINT32  ulChannelId;
}DIAG_BSP_CTRL;

/* C核给A核发送NV鉴权结果 */
typedef struct
{
     VOS_MSG_HEADER                     /*VOS头 */
     VOS_UINT32                         ulMsgId;
     VOS_UINT32                         ulLevel;
}DIAG_BSP_NV_AUTH_STRU;

#define    DIAG_HDS_CMD_MAX    50
typedef struct
{
     VOS_MSG_HEADER                     /*VOS头 */
     VOS_UINT32                         ulMsgId;
     VOS_UINT32                         ulCmdNum;
     VOS_UINT32                         ulCmdList[50];
}DIAG_BSP_CMDLIST_STRU;

typedef struct
{
    VOS_UINT32 ulNVId;         /* 需要写入的NV ID*/
    VOS_UINT32 ulDataSize;     /* 需要写入的NV项数据的大小*/
    VOS_UINT8  aucData[0];     /* 数据缓冲区*/
}DIAG_NV_WRITE_TABLE_STRU;
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
VOS_UINT32 diag_BspMsgProc(msp_diag_frame_info_s *pData);
VOS_VOID diag_BspMsgInit(VOS_VOID);
VOS_VOID diag_BspRecvCmdList(MsgBlock* pMsgBlock);
VOS_UINT32 diag_BspConnMgr(VOS_UINT8 *pData);
VOS_BOOL diag_BspIsNrmCmd(VOS_UINT32 ulCmdId);


/*****************************************************************************
  9 OTHERS
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of msp_diag.h */


