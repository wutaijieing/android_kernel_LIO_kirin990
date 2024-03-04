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

#ifndef IMM_MEM_PS_H
#define IMM_MEM_PS_H

#include "vos.h"
#include "product_config.h"
#if (VOS_OS_VER == VOS_LINUX)
#include <linux/skbuff.h>
#else
#include "skbuff.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#define FEATURE_IMM_MEM_DEBUG (FEATURE_TTF_MEM_DEBUG)

enum IMM_Bool {
    IMM_FALSE = 0,
    IMM_TRUE  = 1,
    IMM_BOOL_BUTT
};
typedef unsigned char IMM_BoolUint8;

enum IMM_RsltCode {
    IMM_SUCC = 0,
    IMM_FAIL = 1,
};
typedef unsigned int IMM_RsltCodeUint32;

enum IMM_MemBlkState {
    MEM_BLK_STATE_FREE,  /* 该内存未申请过或已释放 */
    MEM_BLK_STATE_ALLOC, /* 该内存已申请 */
    MEM_BLK_STATE_BUTT
};
typedef unsigned int IMM_MemBlkStateUint32;

/* 内存可维可测存储结构 */
typedef struct {
    IMM_MemBlkStateUint32 state;
    unsigned int    allocTick;      /* CPU tick when alloc or free */
    unsigned short  allocFileId;  /* File ID when alloc or free */
    unsigned short  allocLineNum; /* File Line when alloc or free */
    unsigned short  traceFileId;  /* File ID when traced */
    unsigned short  traceLineNum; /* File Line when traced */
    unsigned int    traceTick;      /* CPU tick when traced */
} IMM_BlkMemDebug;

enum IMM_MemPoolId {
    IMM_MEM_POOL_ID_SHARE = 0, /* A核共享内存池ID */
    IMM_MEM_POOL_ID_EXT,       /* A核外部内存池ID */

    IMM_MEM_POOL_ID_BUTT
};
typedef unsigned char IMM_MemPoolIdUint8;

typedef struct {
    IMM_MemPoolIdUint8 poolId;    /* 本内存块是是属于哪个内存池 */
    unsigned char      clusterId; /* 本内存是属于哪一个级别 */
    unsigned short     magicNum;  /* 内存控制块魔术字 */
#if (FEATURE_IMM_MEM_DEBUG == FEATURE_ON)
    IMM_BlkMemDebug    dbgInfo;   /* 内存控制块的DEBUG信息存储块 */
#endif
    unsigned char     *memBlk; /* 存放数据的指针，物理上指向结构体的连续内存 */
} IMM_Mem;

/* 为了屏蔽不同版本的差异, IMM_ZcHeader/ImmZc放到头文件中 */
typedef struct sk_buff_head IMM_ZcHeader;

typedef struct sk_buff IMM_Zc;


typedef void (*IMM_MemEventCB)(unsigned int maxClusterFreeCnt);

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

#endif
