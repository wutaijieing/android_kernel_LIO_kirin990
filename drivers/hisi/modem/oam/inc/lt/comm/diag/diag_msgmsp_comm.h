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


#ifndef __DIAG_MSGMSP_COMM_H__
#define __DIAG_MSGMSP_COMM_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  <vos.h>
#include  <mdrv.h>
#include  <msp_errno.h>
#include  "blist.h"
#include  "diag_msg_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 macro
*****************************************************************************/

#if(VOS_OS_VER == VOS_LINUX)
#define ERR_HIDS_CORE_ERROR     ERR_MSP_DIAG_ACORE_ERROR
#else
#define ERR_HIDS_CORE_ERROR     ERR_MSP_DIAG_CCORE_ERROR
#endif

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/


/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
描述 : 获取modem个数
ID   : DIAG_CMD_GET_MODEM_NUM
REQ : DIAG_CMD_GET_MODEM_NUM_REQ_STRU
CNF : DIAG_CMD_GET_MODEM_NUM_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulRsv;
} DIAG_CMD_GET_MODEM_NUM_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulAuid;
    VOS_UINT32 ulSn;
    VOS_UINT32 ulRc;
    VOS_UINT32 ulNum;
} DIAG_CMD_GET_MODEM_NUM_CNF_STRU;


/*****************************************************************************
描述 : 获取有效PID列表
ID   : DIAG_CMD_PID_TABLE_MSG
REQ : DIAG_CMD_PID_TABLE_REQ_STRU
CNF : DIAG_CMD_PID_TABLE_CNF_STRU
*****************************************************************************/

typedef struct
{
    VOS_UINT32 ulAuid;          /* 原AUID */
    VOS_UINT32 ulSn;            /* HSO分发，插件命令管理 */
    VOS_UINT32 ulreserve;
} DIAG_CMD_PID_TABLE_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulAuid;          /* 原AUID */
    VOS_UINT32 ulSn;            /* HSO分发，插件命令管理 */
    VOS_UINT32 ulRc;            /* 结果码 0-success */
    VOS_UINT32 ulPidNum;        /* PID number */
    VOS_UINT32 aulPid[0];        /* PID的值 */       /*lint !e43 */
} DIAG_CMD_PID_TABLE_CNF_STRU;

typedef VOS_UINT32 (*DIAG_MSGMSP_PROC_FUNC)(VOS_UINT8* pstReq);

typedef struct
{
    VOS_UINT32              ulCmdId;
    DIAG_MSGMSP_PROC_FUNC   pfnProc;
}DIAG_MSGMSP_PROC_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/

/*****************************************************************************
  9 OTHERS
*****************************************************************************/
VOS_UINT32 diag_AppTransMspProc(MsgBlock* pMsgBlock);
VOS_UINT32 diag_MspMsgProc(msp_diag_frame_info_s *pData);
VOS_UINT32 diag_RegMsgMspProc(VOS_UINT32 ulCmdId, DIAG_MSGMSP_PROC_FUNC pfnProc);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of msp_diag.h */

