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

#include "imm_mem_zc.h"
#include "v_id.h"
#include "product_config.h"
#include "PsTypeDef.h"
#include "ttf_util.h"
#include "securec.h"

#define THIS_FILE_ID PS_FILE_ID_IMM_ZC_C

#define IMM_PRIV_CB(skb) ((IMM_PrivCb*)((skb)->cb))

#pragma pack(4)
typedef struct {
    unsigned short app;
    unsigned char  rsv[2];
} IMM_PrivCb;
#pragma pack()

/*
 * ��A�������ڴ��������ڴ�ķ��亯��
 * ����ֵ: �ɹ�������Ŀ��IMM_Zc��ָ�� ʧ�ܣ�����NULL
 */
IMM_Zc* IMM_ZcStaticAllocDebug(unsigned short fileId, unsigned short lineNum, unsigned int len)
{
    /* ���ܻ���̬��, ����skbϵͳ�ڴ� */
    IMM_Zc *alloc = (IMM_Zc*)IMM_ZcLargeMemAlloc(len);
    return alloc;
}

/*
 * IMM_ZC�����ݽṹ�Ŀ��������ƽڵ�����ݿ飩�������ݿ��Linux�Դ����ڴ濽����A�˹����ڴ�
 * A����������ΪMEM_TYPE_SYS_DEFINED�����ݿ飬���ݴ���C��ǰ��һ��Ҫ���ñ��ӿڣ������ݿ�����A�˹����ڴ�
 * ����ֵ: �ɹ�������Ŀ��IMM_Zc��ָ�� ʧ�ܣ�����NULL
 */
IMM_Zc* IMM_ZcStaticCopyDebug(VOS_UINT16 fileId, VOS_UINT16 lineNum, IMM_Zc *immZc)
{
    return VOS_NULL_PTR;
}

/* �ͷ�IMM_Zc���ƽڵ㣬���ݿ鲻�ͷš� */
void IMM_ZcHeadFree(IMM_Zc *immZc)
{
}

/* ��IMM_Zc�㿽�����ƽڵ�ת����IMM_Mem���ƽڵ� */
IMM_Mem* IMM_ZcMapToImmMemDebug(unsigned short fileId, unsigned short lineNum, IMM_Zc *immZc)
{
    return VOS_NULL_PTR;
}

/* ��IMM_Zc�㿽���ṹ���MACͷ */
unsigned int IMM_ZcAddMacHead(IMM_Zc *immZc, const unsigned char *addData)
{
    unsigned char* destAddr = VOS_NULL_PTR;

    if ((immZc == VOS_NULL_PTR) || (addData == VOS_NULL_PTR)) {
        return VOS_ERR;
    }

    if ((immZc->data - immZc->head) < IMM_MAC_HEADER_RES_LEN) {
        return VOS_ERR;
    }

    destAddr = IMM_ZcPush(immZc, IMM_MAC_HEADER_RES_LEN);
    TTF_ACORE_SF_CHK(memcpy_s(destAddr, IMM_MAC_HEADER_RES_LEN, addData, IMM_MAC_HEADER_RES_LEN));
    return VOS_OK;
}

/* �Ƴ��㿽���ṹMACͷ */
unsigned int IMM_ZcRemoveMacHead(IMM_Zc *immZc)
{
    if ((immZc == VOS_NULL_PTR) || (immZc->len < IMM_MAC_HEADER_RES_LEN)) {
        return VOS_ERR;
    }

    IMM_ZcPull(immZc, IMM_MAC_HEADER_RES_LEN);
    return VOS_OK;
}

/* ������ӵ���Ч���ݿ��ͷ���� ���ӿ�ֻ�ƶ�ָ��, ������ӵ���Ч���ݿ��ͷ��֮ǰ,���ñ��ӿ� */
unsigned char* IMM_ZcPushDebug(unsigned short fileId, unsigned short lineNum, IMM_Zc *immZc, unsigned int len)
{
    unsigned char* ret = skb_push(immZc, len);
    return ret;
}

/* ��IMM_ZCָ������ݿ��ͷ��ȡ�����ݡ� */
unsigned char* IMM_ZcPullDebug(unsigned short fileId, unsigned short lineNum, IMM_Zc *immZc, unsigned int len)
{
    unsigned char* ret = skb_pull(immZc, len);
    return ret;
}

/* ���������IMM_ZCָ������ݿ��β���� */
unsigned char* IMM_ZcPutDebug(unsigned short fileId, unsigned short lineNum, IMM_Zc *immZc, unsigned int len)
{
    unsigned char* ret = skb_put(immZc, len);
    return ret;
}

/* Ԥ��IMM_ZCָ������ݿ�ͷ���ռ䡣 */
void IMM_ZcReserveDebug(unsigned short fileId, unsigned short lineNum, IMM_Zc *immZc, unsigned int len)
{
    skb_reserve(immZc, (int)len);
}

/* �õ�UserApp�� */
unsigned short IMM_ZcGetUserApp(IMM_Zc *immZc)
{
    if (immZc == VOS_NULL_PTR) {
        return 0;
    }

    return IMM_PRIV_CB(immZc)->app;
}

/* ����UserApp�� */
void IMM_ZcSetUserApp(IMM_Zc *immZc, unsigned short app)
{
    if (immZc != VOS_NULL_PTR) {
        IMM_PRIV_CB(immZc)->app = app;
    }
}

/* ���ݿ�Ĺҽӵ�IMM_Zc�ϡ� */
IMM_Zc* IMM_ZcDataTransformImmZcDebug(unsigned short fileId, unsigned short lineNum, const unsigned char *data,
    unsigned int len, void *ttfMem)
{
    return VOS_NULL_PTR;
}