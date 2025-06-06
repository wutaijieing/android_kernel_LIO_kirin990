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

#ifndef __ATCMDCSSPROC_H__
#define __ATCMDCSSPROC_H__

#include "AtCtx.h"
#include "AtParse.h"
#include "css_at_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/* AT与IMSA模块间消息处理函数指针 */
typedef VOS_UINT32 (*AT_CSS_MSG_PROC_FUNC)(struct MsgCB *pMsg);

/*
 * 结构说明: AT与CSS消息与对应处理函数的结构
 */
typedef struct {
    CSS_AT_MsgTypeUint32 msgId;
    AT_CSS_MSG_PROC_FUNC procMsgFunc;
} AT_CSS_MsgProFunc;

VOS_VOID AT_ProcCssMsg(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssMccInfoSetCnf(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssMccVersionQryCnf(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssMccNotify(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssBlackCellListSetCnf(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssBlackCellListQryCnf(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssBlackCellMccNotify(struct MsgCB *msg);

VOS_UINT32 AT_RcvCssLineIndexListSetCnf(struct MsgCB *msg);
VOS_UINT32 AT_RcvCssLineIndexListQryCnf(struct MsgCB *msg);
VOS_UINT32 AT_RcvCssLineDetailSetCnf(struct MsgCB *msg);
VOS_UINT32 AT_RcvCssLinePlmnNotify(struct MsgCB *msg);
VOS_UINT32 AT_RcvCssLineIndexNotify(struct MsgCB *msg);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of AtCmdCssProc.h */
