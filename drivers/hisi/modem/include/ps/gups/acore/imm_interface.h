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

#ifndef IMM_INTERFACE_H
#define IMM_INTERFACE_H

#include "vos.h"
#include "product_config.h"
#include "imm_mem_ps.h"

#if (defined(CONFIG_BALONG_SPE) && (VOS_OS_VER == VOS_LINUX))
#include "mdrv_spe_wport.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#define IMM_MAX_ETH_FRAME_LEN 1536
#define IMM_MAC_HEADER_RES_LEN 14
#define IMM_INVALID_VALUE 0xFFFFFFFF

/* 
 * �˷��亯��������ACPU����������ʹ�õ��ڴ�
 * �ɹ�������ָ��IMM_ZC_STRU��ָ��, ʧ�ܣ�����NULL��
 */
extern IMM_Zc *IMM_ZcStaticAllocDebug(unsigned short fileId, unsigned short lineNums, unsigned int len);

#define IMM_ZcStaticAlloc(len) IMM_ZcStaticAllocDebug(THIS_FILE_ID, __LINE__, len)

/* 
 * ��Linuxϵͳ�������ڴ�ķ��亯��
 * �ɹ�������ָ��IMM_ZC_STRU��ָ��, ʧ�ܣ�����NULL
 */
#define IMM_ZcLargeMemAlloc(len) dev_alloc_skb(len)

/*
 * ����IMM_ZC_STRU����ͷ����C�˿�˹����ڴ�ҽӵ�IMM_ZC_STRU��
 * �ɹ�������ָ��IMM_ZC_STRU��ָ��, ʧ�ܣ�����NULL��
 * �˺������ֲ�Ʒ����̬,��MBB�ϴ˽ӿ���Ч,���ܻ��Ϸ��ؿ�ָ��
 */
extern IMM_Zc *IMM_ZcDataTransformImmZcDebug(unsigned short fileId, unsigned short lineNum,
                                                   const unsigned char *data, unsigned int len, void *ttfMem);

#define IMM_DataTransformImmZc(data, len, ttfMem) \
    IMM_ZcDataTransformImmZcDebug(THIS_FILE_ID, __LINE__, data, len, ttfMem)

/*
 * IMM_ZC�����ݽṹ�Ŀ��������ƽڵ�����ݿ飩�������ݿ��Linux�Դ����ڴ濽����A�˹����ڴ�
 * �ɹ�������Ŀ��IMM_ZC_STRU��ָ��, ʧ�ܣ�����NULL
 * �˺������ֲ�Ʒ����̬,��MBB�ϴ˽ӿ���Ч,���ܻ��Ϸ��ؿ�ָ��
 * MBB��,A����������ΪMEM_TYPE_SYS_DEFINED�����ݿ�,���ݴ���C��ǰ,
 * һ��Ҫ���ñ��ӿ�,�����ݿ�����A�˹����ڴ档
 */
extern IMM_Zc *IMM_ZcStaticCopyDebug(VOS_UINT16 fileId, VOS_UINT16 lineNums, IMM_Zc *immZc);

#define IMM_ZcStaticCopy(immZc) IMM_ZcStaticCopyDebug(THIS_FILE_ID, __LINE__, immZc)

/* �ͷ�IMM_ZC_STRU�ڴ� */
#if (defined(CONFIG_BALONG_SPE) && (VOS_OS_VER == VOS_LINUX))
#define IMM_ZcFree(immZc) mdrv_spe_wport_recycle(immZc)
#define IMM_ZcFreeAny(pstImmZc) mdrv_spe_wport_recycle((pstImmZc))
#else
#define IMM_ZcFree(immZc) kfree_skb(immZc)
#define IMM_ZcFreeAny(pstImmZc) dev_kfree_skb_any((pstImmZc))
#endif


/* �ͷ�IMM_ZC_STRU���ƽڵ㣬���ݿ鲻�ͷš� */
extern void IMM_ZcHeadFree(IMM_Zc *immZc);

/* �ͷ�Զ��TTF_Mem, addr - ָ��C�˵�TTF_MEM��ָ�� */
extern VOS_VOID IMM_RbRemoteFreeMem(VOS_VOID *addr);

#define IMM_RemoteFreeTtfMem(addr) IMM_RbRemoteFreeMem((addr))

/*
 * ���MACͷ. IMM_Zc - ָ��IMM_ZC_STRU��ָ��, AddData - ��ӵ�MACͷ�׵�ַ, Len - MACͷ�ĳ���
 * �����ɹ����, VOS_OK - �ɹ�, VOS_ERR - ʧ��
 * �˺������ֲ�Ʒ����̬,��MBB�ϴ˽ӿ���Ч,���ܻ��Ϸ���ERR
 * 
 */
extern unsigned int IMM_ZcAddMacHead(IMM_Zc *immZc, const unsigned char *addData);

/*
 * ����̫֡�а�ȥMACͷ��
 * �����ɹ����, VOS_OK - �ɹ�, VOS_ERR - ʧ��
 * �˺������ֲ�Ʒ����̬,��MBB�ϴ˽ӿ���Ч,���ܻ��Ϸ���ERR
 * 
 */
extern unsigned int IMM_ZcRemoveMacHead(IMM_Zc *immZc);

/*
 * A�˰�IMM_ZC�㿽�����ƽڵ�ת����IMM_Mem���ƽڵ�
 * �˺������ֲ�Ʒ����̬,��MBB�ϴ˽ӿ���Ч,���ܻ��Ϸ��ؿ�ָ��
 */
extern IMM_Mem *IMM_ZcMapToImmMemDebug(unsigned short fileId, unsigned short lineNum, IMM_Zc *immZc);

#define IMM_ZcMapToImmMem(immZc) IMM_ZcMapToImmMemDebug(THIS_FILE_ID, __LINE__, immZc)

/* 
 * ������ӵ���Ч���ݿ��ͷ��
 * �������ص����ݿ��׵�ַ���������������֮������ݿ��ַ��
 * ���ӿ�ֻ�ƶ�ָ��, ������ӵ���Ч���ݿ��ͷ��֮ǰ,���ñ��ӿ�
 */
#define IMM_ZcPush(immZc, len) skb_push(immZc, len)

/*
 * ��IMM_ZCָ������ݿ��ͷ��ȡ������
 * ���ص����ݿ��׵�ַ��������ȡ������֮��ĵ�ַ
 * ���ӿ�ֻ�ƶ�ָ��
 */
#define IMM_ZcPull(immZc, len) skb_pull(immZc, len)

/*
 * ���������IMM_ZCָ������ݿ��β��
 * ���ص����ݿ�β����ַ���������������֮ǰ�����ݿ�β����ַ��
 * ���ӿ�ֻ�ƶ�ָ��
 */
#define IMM_ZcPut(immZc, len) skb_put(immZc, len)

/*
 * Ԥ��IMM_ZCָ������ݿ�ͷ���ռ䡣
 * ���ӿ�ֻ�ƶ�ָ�룬Ϊͷ��Ԥ���ռ䡣
 * ֻ���ڸշ����IMM_ZC,IMM_ZCָ������ݿ黹û��ʹ�ã�
 */
#define IMM_ZcReserve(immZc, len) skb_reserve(immZc, (int)len)

/* ����IMM_ZCβ����ַ */
#define IMM_ZcResetTailPointer(immZc) skb_reset_tail_pointer(immZc)

/* �õ�����ͷ��Ԥ���ռ��ֽ����� ����Ԥ���ռ��ֽ��� */
#define IMM_ZcHeadRoom(immZc) skb_headroom(immZc)

/* �õ�����β��Ԥ���ռ��ֽ����� ����Ԥ���ռ��ֽ��� */
#define IMM_ZcTailRoom(immZc) skb_tailroom(immZc)

/* �õ����ݿ��׵�ַ�� �������ݿ��׵�ַ */
#define IMM_ZcGetDataPtr(immZc) ((immZc)->data)

/* �õ����ݿ�ʹ�õ��ֽ����� �������ݿ�ʹ�õ��ֽ��� */
#define IMM_ZcGetUsedLen(immZc) ((immZc)->len)

/* �õ����ݿ�ʹ�õ�Protocol�� �������ݿ�ʹ��Protocol */
#define IMM_ZcGetProtocol(immZc) ((immZc)->protocol)

/* �������ݿ�ʹ�õ�Protocol */
#define IMM_ZcSetProtocol(immZc, proto) ((immZc)->protocol = proto)

/*
 * �õ�UserApp
 * ����MBB�����ܻ���̬,MBB�ϴ˽ӿ���Ч,���ܻ�����Ч,ֱ�ӷ���0
 * ���Ҫ���ⲿʹ���ߴ������
 */
extern unsigned short IMM_ZcGetUserApp(IMM_Zc *immZc);

/*
 * ����UserApp
 * ����MBB�����ܻ���̬,MBB�ϴ˽ӿ���Ч,���ܻ�����Ч,Ϊ�պ���
 * ���Ҫ���ⲿʹ���ߴ������
 */
extern void IMM_ZcSetUserApp(IMM_Zc *immZc, unsigned short app);

/* ���г�ʼ���� */
#define IMM_ZcQueueHeadInit(zcQueue) skb_queue_head_init(zcQueue)

/* Ԫ�ز������ͷ���� */
#define IMM_ZcQueueHead(zcQueue, newItem) skb_queue_head(zcQueue, newItem)

/* Ԫ�ز������β���� */
#define IMM_ZcQueueTail(zcQueue, newItem) skb_queue_tail(zcQueue, newItem)

/* �Ӷ���ͷ��ȡԪ�ء� ����ָ��IMM_ZC_STRU��ָ�� */
#define IMM_ZcDequeueHead(zcQueue) skb_dequeue(zcQueue)

/* �Ӷ���β��ȡԪ�ء� ����ָ��IMM_ZC_STRU��ָ�� */
#define IMM_ZcDequeueTail(zcQueue) skb_dequeue_tail(zcQueue)

/* �õ������е�Ԫ�ص���Ŀ�� */
#define IMM_ZcQueueLen(zcQueue) skb_queue_len(zcQueue)

/* �õ������еĶ���Ԫ�ص�ָ�� */
#define IMM_ZcQueuePeek(zcQueue) skb_peek(zcQueue)

/* �õ������еĶ�βԪ�ص�ָ�� */
#define IMM_ZcQueuePeekTail(zcQueue) skb_peek_tail(zcQueue)

/* �ṩ���ⲿģ����ü��A���ڴ��Ƿ�й¶ */
VOS_VOID IMM_MntnAcpuCheckPoolLeak(VOS_VOID);

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

