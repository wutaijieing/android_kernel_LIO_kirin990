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

#ifndef TTF_LINK_INTERFACE_H
#define TTF_LINK_INTERFACE_H

#include "product_config.h"
#include "vos.h"
#include "PsTypeDef.h"
#include "ps_lib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

enum TTF_PsDataPriority {
    TTF_PS_DATA_PRIORITY_NORMAL = 0,
    TTF_PS_DATA_PRIORITY_MIDDLE,
    TTF_PS_DATA_PRIORITY_HIGH,

    TTF_PS_DATA_PRIORITY_BUTT
};
typedef unsigned char TTF_PsDataPriorityUint8;

typedef struct tagTtfNode {
    struct tagTtfNode* next;
    struct tagTtfNode* prev;
} TTF_Node;

#define TTF_LINK_CNT(link) ((link)->cnt)

#define TTF_LINK_INIT_NODE(node) do { \
    (node)->prev = VOS_NULL_PTR; \
    (node)->next = VOS_NULL_PTR; \
} while (0)

#pragma pack(4)

/*
 * 链表头stHead也是一个节点
 * stHead.pNext保存链表的头指针
 * stHead.pPrev保存链表的尾指针
 * 链表的第一个元素的pPrev始终指向链表头
 * 链表的最后一个元素的pNext也始终指向链表头
 */
typedef struct {
    TTF_Node     head;
    VOS_UINT32   cnt;
    VOS_SPINLOCK spinLock;
} TTF_Link;

#pragma pack()

/* 注意: 使用者必须保证link非空，否则需要使用函数 */
#define TTF_LINK_IS_EMPTY(link) ((link)->cnt == 0)
#define TTF_LINK_IS_NOT_EMPTY(link) ((link)->cnt != 0)

#define TTF_LINK_INIT(link) do { \
    (link)->head.next = &((link)->head); \
    (link)->head.prev = &((link)->head); \
    (link)->cnt        = 0;                 \
} while (0)

#define TTF_LINK_INSERT_TAIL(link, insert) do { \
    (insert)->next             = (TTF_Node*)(&((link)->head)); \
    (insert)->prev             = (link)->head.prev;              \
    (link)->head.prev->next = insert;                            \
    (link)->head.prev        = insert;                            \
    (link)->cnt++;                                                 \
} while (0)

#define TTF_LINK_PEEK_HEAD(link, node, ptrType) do { \
    if ((link)->cnt == 0) {                   \
        node = (ptrType)VOS_NULL_PTR;           \
    } else {                                    \
        node = (ptrType)((link)->head.next); \
    }                                           \
} while (0)

#define TTF_LINK_REMOVE_NODE(link, removeNode) do { \
    (removeNode)->next->prev = (removeNode)->prev; \
    (removeNode)->prev->next = (removeNode)->next; \
    (removeNode)->prev        = VOS_NULL_PTR;        \
    (removeNode)->next        = VOS_NULL_PTR;        \
    (link)->cnt--;                                  \
} while (0)

extern VOS_VOID   TTF_LinkInit(VOS_UINT32 pid, TTF_Link* link);
extern VOS_VOID   TTF_NodeInit(TTF_Node* node);
extern VOS_VOID   TTF_LinkFree(VOS_UINT32 pid, TTF_Link* link);
extern VOS_UINT32 TTF_LinkCheckNodeInLink(VOS_UINT32 pid, TTF_Link* link, TTF_Node* curr);

#ifdef _lint
/*
 * 基于L2的内存管理机制：调用者申请一块内存，只要将其插入到链表中，调用者的责任已经完成了，
 * 该内存块的释放会有统一的机制进行处理，原调用者不再负责释放。目前进行PCLINT时，出现一些
 * 内存泄漏误告警，对L2的内存管理函数进行适当修改，以适应PCLINT的检查处理。
 */
#ifdef  TTF_LinkInsertHead
#undef  TTF_LinkInsertHead
#endif
#define TTF_LinkInsertHead(pid, link, insert) do { \
    /*lint -e155 */                       \
    (VOS_VOID)(link);                     \
    free(insert);                         \
} while (0) /*lint +e155 */

#ifdef  TTF_LinkInsertTail
#undef  TTF_LinkInsertTail
#endif
#define TTF_LinkInsertTail(pid, link, insert) do { \
    /*lint -e155 */                       \
    (VOS_VOID)(link);                     \
    free(insert);                         \
} while (0) /*lint +e155 */

#ifdef  TTF_LinkInsertNext
#undef  TTF_LinkInsertNext
#endif
#define TTF_LinkInsertNext(pid, link, curr, insert) do { \
    /*lint -e155 */                             \
    (VOS_VOID)(link);                           \
    (VOS_VOID)(curr);                           \
    free(insert);                               \
} while (0) /*lint +e155 */

#ifdef  TTF_LinkInsertPrev
#undef  TTF_LinkInsertPrev
#endif
#define TTF_LinkInsertPrev(pid, link, curr, insert) do { \
    /*lint -e155 */                             \
    (VOS_VOID)(link);                           \
    (VOS_VOID)(curr);                           \
    free(insert);                               \
} while (0) /*lint +e155 */
#else
extern VOS_UINT32   TTF_LinkInsertHead(VOS_UINT32 pid, TTF_Link * link, TTF_Node *insert);
extern VOS_UINT32   TTF_LinkInsertPrev(VOS_UINT32 pid, TTF_Link * link, TTF_Node *curr, TTF_Node *insert);
extern VOS_UINT32   TTF_LinkInsertNext(VOS_UINT32 pid, TTF_Link * link, TTF_Node *curr, TTF_Node *insert);
extern VOS_UINT32   TTF_LinkInsertTail(VOS_UINT32 pid, TTF_Link * link, TTF_Node *insert);
#endif

extern VOS_VOID     TTF_LinkSafeInit(VOS_UINT32 pid, TTF_Link* link);
extern TTF_Node* TTF_LinkSafePeekHead(VOS_UINT32 pid, TTF_Link* link);
extern VOS_UINT32   TTF_LinkSafeInsertHead(VOS_UINT32 pid, TTF_Link* link, TTF_Node* insert);
extern VOS_UINT32   TTF_LinkSafeInsertPrev(VOS_UINT32 pid, TTF_Link* link, TTF_Node* curr, TTF_Node* insert);
extern VOS_UINT32   TTF_LinkSafeInsertNext(VOS_UINT32 pid, TTF_Link* link, TTF_Node* curr, TTF_Node* insert);
extern VOS_UINT32   TTF_LinkSafeInsertTail(
      VOS_UINT32 pid, TTF_Link* link, TTF_Node* insert, VOS_UINT32* nonEmptyEvent);
extern TTF_Node* TTF_LinkSafeRemoveHead(VOS_UINT32 pid, TTF_Link* link, VOS_UINT32* remainCnt);
extern TTF_Node* TTF_LinkSafeRemoveTail(VOS_UINT32 pid, TTF_Link* link);

extern VOS_VOID     TTF_LinkRemoveNode(VOS_UINT32 pid, TTF_Link* link, TTF_Node* removeNode);
extern TTF_Node* TTF_LinkRemoveTail(VOS_UINT32 pid, TTF_Link* link);
extern VOS_VOID     TTF_LinkSafeRemoveNode(VOS_UINT32 pid, TTF_Link* link, TTF_Node* removeNode);

extern VOS_UINT32   TTF_LinkPeekNext(VOS_UINT32 pid, const TTF_Link* link, TTF_Node* curr, TTF_Node** next);
extern VOS_UINT32   TTF_LinkPeekPrev(VOS_UINT32 pid, TTF_Link* link, TTF_Node* curr, TTF_Node** prev);
extern TTF_Node* TTF_LinkPeekTail(VOS_UINT32 pid, const TTF_Link* link);
extern VOS_UINT32   TTF_LinkStick(VOS_UINT32 pid, TTF_Link* link1, TTF_Link* link2);
extern TTF_Node* TTF_LinkRemoveHead(VOS_UINT32 pid, TTF_Link* link);
extern VOS_UINT32   TTF_LinkCnt(VOS_UINT32 pid, const TTF_Link* link);
extern VOS_UINT32   TTF_LinkIsEmpty(VOS_UINT32 pid, const TTF_Link* link);
extern TTF_Node* TTF_LinkPeekHead(VOS_UINT32 pid, const TTF_Link* link);

extern VOS_VOID TTF_InsertSortAsc16bit(VOS_UINT32 pid, VOS_UINT16 sortElement[],
    VOS_UINT32 elementCnt, VOS_UINT32 maxCnt);
extern VOS_VOID TTF_RemoveDupElement16bit(VOS_UINT32 pid, VOS_UINT16 sortElement[],
    VOS_UINT32* elementCnt, VOS_UINT32 maxCnt);


#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
