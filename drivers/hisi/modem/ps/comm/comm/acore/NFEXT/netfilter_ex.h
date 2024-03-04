/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019.All rights reserved.
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

#ifndef _NETFILTEREX_H_
#define _NETFILTEREX_H_

#include "v_id.h"
#include "vos.h"
#include "mdrv_nvim.h"
#include "network_interface.h"
#include "PsTypeDef.h"
#include "om_ring_buffer.h"
#include "ps_common_def.h"
#include "acore_nv_id_gucttf.h"
#include "acore_nv_stru_gucttf.h"
#ifndef MODEM_FUSION_VERSION
#include "msp_diag_comm.h"
#else
#include "mdrv_diag.h"
#endif
#include "ips_mntn.h"
#include "linux_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef __UT_CENTER__
#define NF_EXT_MEM_ALLOC(pid, size) kmalloc(size, GFP_ATOMIC)
#define NF_EXT_MEM_FREE(pid, p) kfree(p);
#else
#define NF_EXT_MEM_ALLOC(pid, size) PS_MEM_ALLOC(pid, size)
#define NF_EXT_MEM_FREE(pid, p) PS_MEM_FREE(pid, p);
#endif

#if (FEATURE_NFEXT == FEATURE_ON)
#define MAC_HEADER_LENGTH 14
#define NF_EXT_BR_FORWARD_FLOW_CTRL_MASK 1U
#endif

enum NfExtFlagOmDataEnum {
    NF_EXT_FLAG_OM_DATA     = 0, /* OM消息标志 */
    NF_EXT_FLAG_NOT_OM_DATA = 1, /* 非OM消息标志 */

    NF_EXT_FLAG_OM_DATA_BUTT
};
typedef VOS_INT NfExtFlagOmDataEnumU32;

enum NfExtTxBytesCntEnum {
    NF_EXT_TX_BYTES_CNT_BR = 0, /* 统计类型 */
    NF_EXT_TX_BYTES_CNT_BUTT
};

#ifdef CONFIG_UART_SHELL
enum NfExtStatsEnum {
    NF_EXT_STATS_BR_FC_DROP = 0,
    NF_EXT_STATS_BR_FC_ENTER,
    NF_EXT_STATS_BUF_FULL_DROP,
    NF_EXT_STATS_PUT_BUF_FAIL,
    NF_EXT_STATS_ALLOC_MEM_FAIL,
    NF_EXT_STATS_GET_BUF_FAIL,

    NF_EXT_STATS_BUT
};
#endif

typedef struct {
    VOS_UINT32     isBlkflag;     /* 阻塞条件 */
    VOS_UINT32     curHookOnMask; /* 当前Hook掩码 */
    VOS_UINT32     isDeviceOpen;  /* 设备是否开启的标志 */
    VOS_UINT32     omIp;
    OM_RING_ID     ringBufId;  /* 环形buff */
    VOS_SPINLOCK   lockTxTask; /* 自旋锁，用于环形buff操作的互斥保护 */
    VOS_UINT8      rsv2[4];
    struct cpumask origMask;
    struct cpumask currMask;
} NfExtEntity;

#if (FEATURE_NFEXT == FEATURE_ON)
typedef struct {
    VOS_UINT32         fcMask;
    VOS_UINT32         txBytesCnt[NF_EXT_TX_BYTES_CNT_BUTT];
    struct net_device* brDev;
} NfExtFlowCtrlEntity;
#endif

#ifdef CONFIG_UART_SHELL
typedef struct {
    VOS_UINT32 stats[NF_EXT_STATS_BUT];
} NfExtStats;
#endif

#ifdef CONFIG_UART_SHELL
extern NfExtStats g_nfExtStats;

#define NF_EXT_STATS_INC(a, b) (g_nfExtStats.stats[(b)] += (a))
#else
#define NF_EXT_STATS_INC(a, b) do { \
} while (0)
#endif

#define NFEXT_DATA_PROC_NOTIFY 0x0001
#define NFEXT_HIDS_DISCONN_NOTIFY 0x0002

#if (FEATURE_NFEXT == FEATURE_ON)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
extern unsigned int NFEXT_BrPreRoutingHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_BrPostRoutingHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_BrLocalInHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_BrLocalOutHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_BrForwardHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_ArpInHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_ArpOutHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip4PreRoutingHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip4PostRoutingHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip4LocalInHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip4LocalOutHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip4ForwardHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip6PreRoutingHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip6PostRoutingHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip6LocalInHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip6LocalOutHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_Ip6ForwardHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

extern unsigned int NFEXT_BrForwardFlowCtrlHook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

#else
extern unsigned int NFEXT_BrPreRoutingHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_BrPostRoutingHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_BrLocalInHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_BrLocalOutHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_BrForwardHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_ArpInHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_ArpOutHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip4PreRoutingHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip4PostRoutingHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip4LocalInHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip4LocalOutHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip4ForwardHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip6PreRoutingHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip6PostRoutingHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip6LocalInHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip6LocalOutHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_Ip6ForwardHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));

extern unsigned int NFEXT_BrForwardFlowCtrlHook(unsigned int hookNum, struct sk_buff *skb, const struct net_device *in,
    const struct net_device *out, int (*okfn)(struct sk_buff*));
#endif

extern NfExtFlagOmDataEnumU32 NFEXT_IsOmData(struct sk_buff *skb);
extern VOS_VOID NFEXT_BrDataExport(struct sk_buff *skb, const struct net_device *device_in,
    const struct net_device *device_out, TTF_MntnMsgTypeUint16 msgType);
extern VOS_VOID NFEXT_ArpDataExport(struct sk_buff *skb, const struct net_device *device, TTF_MntnMsgTypeUint16 msgType);
extern VOS_VOID NFEXT_IpDataExport(struct sk_buff *skb, const struct net_device *device, TTF_MntnMsgTypeUint16 msgType);
extern VOS_VOID NFEXT_UnregHooks(VOS_UINT32 mask);
extern VOS_INT NFEXT_RegHooks(VOS_UINT32 mask);
extern VOS_UINT32 NFEXT_Get1stInetIpv4Addr(const struct net_device *dev);
extern PS_BoolUint8 NFEXT_ConfigEffective(const IPS_MntnTraceCfgReq *msg);
extern VOS_UINT32 NFEXT_BrSetFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2);
extern VOS_UINT32 NFEXT_BrStopFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2);
extern VOS_UINT32 NFEXT_GetBrBytesCnt(VOS_VOID);
#endif
extern VOS_VOID NFEXT_FlushRingBuffer(OM_RING_ID rngId);
extern VOS_VOID NFEXT_BindToCpu(VOS_VOID);
extern VOS_VOID NFEXT_RcvOmMsg(const MsgBlock *msg);
extern VOS_VOID NFEXT_ProcDataNotify(VOS_VOID);
extern VOS_VOID NFEXT_MsgProc(const struct MsgCB *msg);
extern VOS_UINT32 NFEXT_PidInit(enum VOS_InitPhaseDefine ip);
extern VOS_VOID NFEXT_FidTask(VOS_UINT32 para0, VOS_UINT32 para1, VOS_UINT32 para2, VOS_UINT32 para3);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
