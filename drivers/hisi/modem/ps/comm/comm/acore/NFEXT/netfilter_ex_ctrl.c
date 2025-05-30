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

#include "netfilter_ex_ctrl.h"
#include "securec.h"
#include "v_typdef.h"
#include "PsTypeDef.h"
#include "ttf_util.h"
#include "gucttf_tag.h"
#include "ttf_oam_interface.h"
#include "ttf_comm.h"
#include "netfilter_ex.h"
#include "ips_mntn.h"
#include "ips_traffic_statistic.h"


#define THIS_FILE_ID PS_FILE_ID_ACPU_NFEX_CTRL_C
#define THIS_MODU mod_nfext

NfExtEntity g_exEntity          = {0};
STATIC VOS_UINT32 g_nfExtTaskId = 0;


/* netfilter钩子函数掩码 */
enum NfExtHoodOnMaskIdxEnum {
    NF_EXT_BR_PRE_ROUTING_HOOK_ON_MASK_IDX       = 0x00,
    NF_EXT_BR_POST_ROUTING_HOOK_ON_MASK_IDX      = 0x01,
    NF_EXT_BR_FORWARD_HOOK_ON_MASK_IDX           = 0x02,
    NF_EXT_BR_LOCAL_IN_HOOK_ON_MASK_IDX          = 0x03,
    NF_EXT_BR_LOCAL_OUT_HOOK_ON_MASK_IDX         = 0x04,
    NF_EXT_ARP_LOCAL_IN_ON_MASK_IDX              = 0x05,
    NF_EXT_ARP_LOCAL_OUT_ON_MASK_IDX             = 0x06,
    NF_EXT_IP4_PRE_ROUTING_HOOK_ON_MASK_IDX      = 0x07,
    NF_EXT_IP4_POST_ROUTING_HOOK_ON_MASK_IDX     = 0x08,
    NF_EXT_IP4_LOCAL_IN_HOOK_ON_MASK_IDX         = 0x09,
    NF_EXT_IP4_LOCAL_OUT_HOOK_ON_MASK_IDX        = 0x0A,
    NF_EXT_IP4_FORWARD_HOOK_ON_MASK_IDX          = 0x0B,
    NF_EXT_IP6_PRE_ROUTING_HOOK_ON_MASK_IDX      = 0x0C,
    NF_EXT_IP6_POST_ROUTING_HOOK_ON_MASK_IDX     = 0x0D,
    NF_EXT_IP6_LOCAL_IN_HOOK_ON_MASK_IDX         = 0x0E,
    NF_EXT_IP6_LOCAL_OUT_HOOK_ON_MASK_IDX        = 0x0F,
    NF_EXT_IP6_FORWARD_HOOK_ON_MASK_IDX          = 0x10,
    NF_EXT_BR_FORWARD_FLOW_CTRL_HOOK_ON_MASK_IDX = 0x11,
    NF_EXT_HOOK_ON_MASK_IDX_ENUM_BUTT
};
typedef VOS_UINT8 NfExtHoodOnMaskIdxEnumUint8;

enum NfExtFlagBlockEnum {
    NF_EXT_FLAG_BLOCKED   = 0, /* 阻塞标志 */
    NF_EXT_FLAG_UNBLOCKED = 1, /* 非阻塞标志 */

    NF_EXT_FLAG_BUTT
};

/* netfilter钩子函数掩码优先级(规避MIXED_ENUMS新增) */
enum NfExtHookOnMaskPriEnum {
    NF_EXT_BR_PRI_FILTER_OTHER   = NF_BR_PRI_FILTER_OTHER,
    NF_EXT_BR_PRI_FILTER_BRIDGED = NF_BR_PRI_FILTER_BRIDGED,
    NF_EXT_IP_PRI_CONNTRACK      = NF_IP_PRI_CONNTRACK,
    NF_EXT_IP_PRI_MANGLE         = NF_IP_PRI_MANGLE,
    NF_EXT_IP_PRI_SELINUX_LAST   = NF_IP_PRI_SELINUX_LAST,

    NF_EXT_HOOK_ON_MASK_PRIORITY_ENUM_BUTT
};

#if (FEATURE_NFEXT == FEATURE_ON)
typedef struct {
    VOS_UINT32 brArpMask; /* 网桥和ARP钩子函数对应的掩码 */
    VOS_UINT32 inMask;    /* IP层PRE_ROUTING钩子函数对应的掩码 */
    VOS_UINT32 outMask;   /* IP层POST_ROUTING钩子函数对应的掩码 */
    VOS_UINT32 fcMask;    /* 网桥流控钩子函数所对应的掩码 */
    VOS_UINT32 localMask; /* IP层LOCAL钩子函数对应的掩码  */
    VOS_UINT32 rsv;
} NfExtHookMaskNv;

/* 勾子开关掩码映射表结构体 */
typedef struct {
    u_int32_t          hookMask;
    VOS_UINT8          rsv[4];
    struct nf_hook_ops nfExtOps;
} NfExtMaskOps;

#define NF_EXT_GET_MASK_FROM_INDEX(idx) (1 << (idx))

NfExtFlowCtrlEntity g_exFlowCtrlEntity;
STATIC NfExtNv g_nfExtNv;
STATIC NfExtHookMaskNv g_exHookMask;

/* 注意不同内核版本对于nf_hook_ops的定义,参考kernel\linux 4.x\include\linux\netfilter.h */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
#define INIT_MASK_OPS_ITEM(mask_, func_, priv_, pf_, hooknum_, priority_)                                          \
    {                                                                                                              \
        .hookMask = (mask_), .nfExtOps.hook = (nf_hookfn*)(func_), .nfExtOps.priv = (priv_), .nfExtOps.pf = (pf_), \
        .nfExtOps.hooknum = (hooknum_), .nfExtOps.priority = (priority_)                                           \
    }
#else
#define INIT_MASK_OPS_ITEM(mask_, func_, priv_, pf_, hooknum_, priority_)                \
    {                                                                                    \
        .hookMask = (mask_), .nfExtOps.hook = (nf_hookfn*)(func_), .nfExtOps.pf = (pf_), \
        .nfExtOps.hooknum = (hooknum_), .nfExtOps.priority = (priority_)                 \
    }
#endif

/* 扩展netfilter开关映射表 */
STATIC NfExtMaskOps g_nfExtMaskOps[NF_EXT_HOOK_ON_MASK_IDX_ENUM_BUTT] = {
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_BR_PRE_ROUTING_HOOK_ON_MASK_IDX),
        NFEXT_BrPreRoutingHook, NULL, NFPROTO_BRIDGE, NF_BR_PRE_ROUTING, NF_EXT_BR_PRI_FILTER_OTHER),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_BR_POST_ROUTING_HOOK_ON_MASK_IDX),
        NFEXT_BrPostRoutingHook, NULL, NFPROTO_BRIDGE, NF_BR_POST_ROUTING, NF_EXT_BR_PRI_FILTER_OTHER),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_BR_FORWARD_HOOK_ON_MASK_IDX),
        NFEXT_BrForwardHook, NULL, NFPROTO_BRIDGE, NF_BR_FORWARD, NF_EXT_BR_PRI_FILTER_OTHER),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_BR_LOCAL_IN_HOOK_ON_MASK_IDX),
        NFEXT_BrLocalInHook, NULL, NFPROTO_BRIDGE, NF_BR_LOCAL_IN, NF_EXT_BR_PRI_FILTER_OTHER),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_BR_LOCAL_OUT_HOOK_ON_MASK_IDX),
        NFEXT_BrLocalOutHook, NULL, NFPROTO_BRIDGE, NF_BR_LOCAL_OUT, NF_EXT_BR_PRI_FILTER_OTHER),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_ARP_LOCAL_IN_ON_MASK_IDX),
        NFEXT_ArpInHook, NULL, NFPROTO_ARP, NF_ARP_IN, NF_EXT_IP_PRI_CONNTRACK),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_ARP_LOCAL_OUT_ON_MASK_IDX),
        NFEXT_ArpOutHook, NULL, NFPROTO_ARP, NF_ARP_OUT, NF_EXT_IP_PRI_CONNTRACK),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_PRE_ROUTING_HOOK_ON_MASK_IDX),
        NFEXT_Ip4PreRoutingHook, NULL, NFPROTO_IPV4, NF_INET_PRE_ROUTING, NF_EXT_IP_PRI_MANGLE),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_POST_ROUTING_HOOK_ON_MASK_IDX),
        NFEXT_Ip4PostRoutingHook, NULL, NFPROTO_IPV4, NF_INET_POST_ROUTING, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_LOCAL_IN_HOOK_ON_MASK_IDX),
        NFEXT_Ip4LocalInHook, NULL, NFPROTO_IPV4, NF_INET_LOCAL_IN, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_LOCAL_OUT_HOOK_ON_MASK_IDX),
        NFEXT_Ip4LocalOutHook, NULL, NFPROTO_IPV4, NF_INET_LOCAL_OUT, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_FORWARD_HOOK_ON_MASK_IDX),
        NFEXT_Ip4ForwardHook, NULL, NFPROTO_IPV4, NF_INET_FORWARD, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_PRE_ROUTING_HOOK_ON_MASK_IDX),
        NFEXT_Ip6PreRoutingHook, NULL, NFPROTO_IPV6, NF_INET_PRE_ROUTING, NF_EXT_IP_PRI_MANGLE),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_POST_ROUTING_HOOK_ON_MASK_IDX),
        NFEXT_Ip6PostRoutingHook, NULL, NFPROTO_IPV6, NF_INET_POST_ROUTING, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_LOCAL_IN_HOOK_ON_MASK_IDX),
        NFEXT_Ip6LocalInHook, NULL, NFPROTO_IPV6, NF_INET_LOCAL_IN, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_LOCAL_OUT_HOOK_ON_MASK_IDX),
        NFEXT_Ip6LocalOutHook, NULL, NFPROTO_IPV6, NF_INET_LOCAL_OUT, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_FORWARD_HOOK_ON_MASK_IDX),
        NFEXT_Ip6ForwardHook, NULL, NFPROTO_IPV6, NF_INET_FORWARD, NF_EXT_IP_PRI_SELINUX_LAST),
    INIT_MASK_OPS_ITEM(NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_BR_FORWARD_FLOW_CTRL_HOOK_ON_MASK_IDX),
        NFEXT_BrForwardFlowCtrlHook, NULL, NFPROTO_BRIDGE, NF_BR_FORWARD, NF_EXT_BR_PRI_FILTER_BRIDGED)
};

STATIC VOS_UINT32 NFEXT_ReadNvCfg(VOS_VOID);
STATIC VOS_VOID NFEXT_SetDefaultNvCfg(VOS_VOID);
STATIC VOS_VOID NFEXT_EntityInit(VOS_VOID);
STATIC VOS_INT NFEXT_Init(VOS_VOID);
STATIC VOS_VOID NFEXT_SndDataNotify(VOS_VOID);
STATIC VOS_INT NFEXT_RingBufferGet(OM_RING_ID rngId, VOS_CHAR *buffer, VOS_INT maxbytes);
STATIC VOS_VOID NFEXT_RcvNfExtInfoCfgReq(const MsgBlock *msg);
STATIC VOS_VOID NFEXT_EventProc(VOS_UINT32 event);
STATIC VOS_INT NFEXT_RingBufferPut(OM_RING_ID rngId, VOS_CHAR *buffer, VOS_INT nbytes);
STATIC VOS_VOID NFEXT_FlowCtrlInit(VOS_VOID);
STATIC VOS_INT NFEXT_ReRegHooks(VOS_UINT32 mask);
STATIC VOS_VOID NFEXT_DiagConnStateNotifyCB(mdrv_diag_state_e state);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
STATIC VOS_INT nf_register_hook(struct nf_hook_ops* reg)
{
    VOS_INT     ret          = 0;
    VOS_UINT32  rollBackFlag = VOS_NO;
    struct net* net          = VOS_NULL_PTR;
    struct net* last         = VOS_NULL_PTR;

    rtnl_lock();
    for_each_net(net)
    {
        ret = nf_register_net_hook(net, reg);
        if ((ret != 0) && (ret != -ENOENT)) {
            rollBackFlag = VOS_YES;
            break;
        }
    }

    if (rollBackFlag == VOS_YES) {
        last = net;
        for_each_net(net)
        {
            if (net == last) {
                break;
            }
            nf_unregister_net_hook(net, reg);
        }
    }
    rtnl_unlock();

    return ret;
}

STATIC VOS_VOID nf_unregister_hook(struct nf_hook_ops* reg)
{
    struct net* net = VOS_NULL_PTR;

    rtnl_lock();
    for_each_net(net)
    {
        nf_unregister_net_hook(net, reg);
    }
    rtnl_unlock();

    return;
}
#endif
VOS_UINT32 NFEXT_GetEntityOmIp(VOS_VOID)
{
    return g_exEntity.omIp;
}

VOS_VOID NFEXT_TxBytesInc(VOS_UINT32 incValue)
{
    g_exFlowCtrlEntity.txBytesCnt[NF_EXT_TX_BYTES_CNT_BR] += incValue;
}

VOS_UINT32 NFEXT_GetFcMask(VOS_VOID)
{
    return g_exFlowCtrlEntity.fcMask;
}

/* 根据需要停止抓包模块的掩码，将抓包的钩子函数解除内核注册 */
VOS_VOID NFEXT_UnregHooks(VOS_UINT32 mask)
{
    VOS_UINT idx;

    for (idx = 0; idx < ARRAY_SIZE(g_nfExtMaskOps); idx++) {
        if (g_nfExtMaskOps[idx].hookMask == (mask & g_nfExtMaskOps[idx].hookMask)) {
            /* 卸载钩子函数 */
            nf_unregister_hook(&(g_nfExtMaskOps[idx].nfExtOps));
            /* 重置相应的掩码位 */
            g_exEntity.curHookOnMask &= (~g_nfExtMaskOps[idx].hookMask);
        }
    }
}

/* 根据需要开启抓包模块的掩码，将抓包的钩子函数注册到内核 */
VOS_INT NFEXT_RegHooks(VOS_UINT32 mask)
{
    VOS_INT  ret;
    VOS_UINT idx;

    for (idx = 0; idx < ARRAY_SIZE(g_nfExtMaskOps); idx++) {
        if (g_nfExtMaskOps[idx].hookMask != (mask & g_nfExtMaskOps[idx].hookMask)) {
            continue;
        }

        /* 注册相应的钩子函数 */
        ret = nf_register_hook(&(g_nfExtMaskOps[idx].nfExtOps));

        if (ret != 0) {
            TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_WARNING, "register_hook error!!\n");

            /* 若有一个注册失败则卸载当前所有已经注册上的钩子函数 */
            NFEXT_UnregHooks(g_exEntity.curHookOnMask);
            return ret;
        }

        g_exEntity.curHookOnMask |= g_nfExtMaskOps[idx].hookMask;
    }

    return 0;
}

/* 重新注册钩子函数 */
STATIC VOS_INT NFEXT_ReRegHooks(VOS_UINT32 mask)
{
    VOS_INT ret;

    /* 重新注册前先卸载当前所有的钩子函数 */
    if (g_exEntity.curHookOnMask != 0) {
        NFEXT_UnregHooks(g_exEntity.curHookOnMask);
    }

    ret = NFEXT_RegHooks(mask);
    return ret;
}

/* 根据配置注册对应的钩子函数 */
PS_BoolUint8 NFEXT_ConfigEffective(const IPS_MntnTraceCfgReq *msg)
{
    /* 流控hook，默认挂上 */
    VOS_UINT32 mask = 0;

    mask |= g_exHookMask.fcMask;
    if ((msg->bridgeArpTraceCfg.choice > IPS_MNTN_TRACE_NULL_CHOSEN) &&
        (msg->bridgeArpTraceCfg.choice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN)) {
        mask |= g_exHookMask.brArpMask;
    }

    if ((msg->preRoutingTraceCfg.choice > IPS_MNTN_TRACE_NULL_CHOSEN) &&
        (msg->preRoutingTraceCfg.choice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN)) {
        mask |= g_exHookMask.inMask;
    }

    if ((msg->postRoutingTraceCfg.choice > IPS_MNTN_TRACE_NULL_CHOSEN) &&
        (msg->postRoutingTraceCfg.choice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN)) {
        mask |= g_exHookMask.outMask;
    }

    if ((msg->localTraceCfg.choice > IPS_MNTN_TRACE_NULL_CHOSEN) &&
        (msg->localTraceCfg.choice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN)) {
        mask |= g_exHookMask.localMask;
    }

    if (NFEXT_ReRegHooks(mask) < 0) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_WARNING, "NFEXT_ReRegHooks failed!\n");
        return PS_FALSE;
    }

    return PS_TRUE;
}

/* 获取设备第一个Ipv4地址 */
VOS_UINT32 NFEXT_Get1stInetIpv4Addr(const struct net_device *dev)
{
    struct in_device* inDev = VOS_NULL_PTR;

    /* 使用Linux内核结构，使用Linux风格 */
    if ((dev == NULL) || (dev->ip_ptr == NULL)) {
        return 0;
    }

    inDev = (struct in_device*)(dev->ip_ptr);
    if (inDev->ifa_list == NULL) {
        return 0;
    }

    return (VOS_UINT32)htonl((VOS_ULONG)inDev->ifa_list->ifa_address);
}

/* 读取NV配置 */
STATIC VOS_UINT32 NFEXT_ReadNvCfg(VOS_VOID)
{
    VOS_UINT32 ret;

    /* 读取钩子函数注册点掩码 */
    ret = GUCTTF_NV_Read(MODEM_ID_0, NV_ITEM_NETFILTER_HOOK_MASK, &g_nfExtNv, sizeof(NfExtNv));
    if (ret != NV_OK) {
        TTF_PRINT_ERR("[init]<NFEXT_ReadNvCfg>: Read NV FAIL, Error Code \n");
        return VOS_ERR;
    }

    /* 将NV结构中读出的掩码赋给g_exHookMask */
    g_exHookMask.brArpMask = g_nfExtNv.netfilterPara1;
    g_exHookMask.inMask    = g_nfExtNv.netfilterPara2;
    g_exHookMask.outMask   = g_nfExtNv.netfilterPara3;
    g_exHookMask.fcMask    = g_nfExtNv.netfilterPara4;
    g_exHookMask.localMask = g_nfExtNv.netfilterPara5;

    return VOS_OK;
}

/* 设置默认NV配置 */
STATIC VOS_VOID NFEXT_SetDefaultNvCfg(VOS_VOID)
{
    g_exHookMask.brArpMask = (NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_ARP_LOCAL_IN_ON_MASK_IDX) |
                              NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_ARP_LOCAL_OUT_ON_MASK_IDX));
    g_exHookMask.inMask    = (NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_PRE_ROUTING_HOOK_ON_MASK_IDX) |
                           NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_PRE_ROUTING_HOOK_ON_MASK_IDX));
    g_exHookMask.outMask   = (NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_POST_ROUTING_HOOK_ON_MASK_IDX) |
                            NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_POST_ROUTING_HOOK_ON_MASK_IDX));
    g_exHookMask.fcMask    = 0;
    g_exHookMask.localMask = (NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_LOCAL_IN_HOOK_ON_MASK_IDX) |
                              NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP4_LOCAL_OUT_HOOK_ON_MASK_IDX) |
                              NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_LOCAL_IN_HOOK_ON_MASK_IDX) |
                              NF_EXT_GET_MASK_FROM_INDEX(NF_EXT_IP6_LOCAL_OUT_HOOK_ON_MASK_IDX));
}

/* NFExt模块实体全局变量初始化 */
STATIC VOS_VOID NFEXT_EntityInit(VOS_VOID)
{
    g_exEntity.curHookOnMask = 0;
    g_exEntity.isBlkflag     = NF_EXT_FLAG_BLOCKED;
    g_exEntity.isDeviceOpen  = 0;
    g_exEntity.omIp          = 0;
}

/* 模块初始化 */
STATIC VOS_INT NFEXT_Init(VOS_VOID)
{
    if (NFEXT_ReadNvCfg() == VOS_ERR) {
        NFEXT_SetDefaultNvCfg();
    }

    NFEXT_EntityInit();

    NFEXT_FlowCtrlInit();

    (VOS_VOID)mdrv_diag_conn_state_notify_fun_reg(NFEXT_DiagConnStateNotifyCB);

    return VOS_OK;
}

/* 流控实体初始化 */
STATIC VOS_VOID NFEXT_FlowCtrlInit(VOS_VOID)
{
    if (NFEXT_RegHooks(g_exHookMask.fcMask)) {
        TTF_PRINT_ERR("[init]<NFEXT_FlowCtrlInit>: NFEXT_RegHooks fail!\n");
        return;
    }

    g_exFlowCtrlEntity.fcMask = 0;
    g_exFlowCtrlEntity.brDev  = NULL;

    (VOS_VOID)memset_s(
        g_exFlowCtrlEntity.txBytesCnt, sizeof(g_exFlowCtrlEntity.txBytesCnt), 0, sizeof(g_exFlowCtrlEntity.txBytesCnt));

    IPSMNTN_TrafficCtrlInit();
}

/* 网桥流控状态设置 */
VOS_UINT32 NFEXT_BrSetFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2)
{
    VOS_ULONG flag = 0UL;

    VOS_SpinLockIntLock(&g_exEntity.lockTxTask, flag);
    g_exFlowCtrlEntity.fcMask |= NF_EXT_BR_FORWARD_FLOW_CTRL_MASK;
    VOS_SpinUnlockIntUnlock(&g_exEntity.lockTxTask, flag);

    IPSMNTN_FlowCtrl(NF_EXT_BR_FORWARD_FLOW_CTRL_MASK, ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_START);

    return VOS_OK;
}

/* 网桥流控状态解除 */
VOS_UINT32 NFEXT_BrStopFlowCtrl(VOS_UINT32 param1, VOS_UINT32 param2)
{
    VOS_ULONG flag = 0UL;

    VOS_SpinLockIntLock(&g_exEntity.lockTxTask, flag);
    g_exFlowCtrlEntity.fcMask &= ~NF_EXT_BR_FORWARD_FLOW_CTRL_MASK;
    VOS_SpinUnlockIntUnlock(&g_exEntity.lockTxTask, flag);

    IPSMNTN_FlowCtrl(NF_EXT_BR_FORWARD_FLOW_CTRL_MASK, ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_STOP);

    return VOS_OK;
}

/* 获取当前网桥发送接收数据统计信息 */
VOS_UINT32 NFEXT_GetBrBytesCnt(VOS_VOID)
{
    return 0;
}


#endif
/* 发送数据处理指示NFEXT_DATA_PROC_NOTIFY */
STATIC VOS_VOID NFEXT_SndDataNotify(VOS_VOID)
{
    (VOS_VOID)VOS_EventWrite(g_nfExtTaskId, NFEXT_DATA_PROC_NOTIFY);

    return;
}

/* OM_RingBufferIsEmpty没有锁保护，为了保证在多线程下互斥保护，封装带锁的接口供内部使用 */
STATIC VOS_BOOL NFEXT_RingBufferIsEmpty(OM_RING_ID rngId)
{
    VOS_ULONG flag = 0UL;
    VOS_BOOL  result;

    VOS_SpinLockIntLock(&g_exEntity.lockTxTask, flag);
    result = OM_RingBufferIsEmpty(rngId);
    VOS_SpinUnlockIntUnlock(&g_exEntity.lockTxTask, flag);

    return result;
}

/* 将数据放到ring buffer里 */
STATIC VOS_INT NFEXT_RingBufferPut(OM_RING_ID rngId, VOS_CHAR *buffer, VOS_INT nbytes)
{
    VOS_ULONG flag   = 0UL;
    VOS_INT   result = 0;

    VOS_SpinLockIntLock(&g_exEntity.lockTxTask, flag);
    if ((VOS_UINT32)OM_RingBufferFreeBytes(g_exEntity.ringBufId) >= sizeof(NfExtDataRingBuf)) {
        result = OM_RingBufferPut(rngId, buffer, nbytes);
    }
    VOS_SpinUnlockIntUnlock(&g_exEntity.lockTxTask, flag);

    return result;
}

/* 将数据从ring buffer里取出 */
STATIC VOS_INT NFEXT_RingBufferGet(OM_RING_ID rngId, VOS_CHAR *buffer, VOS_INT maxbytes)
{
    VOS_ULONG flag   = 0UL;
    VOS_INT   result = 0;

    VOS_SpinLockIntLock(&g_exEntity.lockTxTask, flag);
    if (!OM_RingBufferIsEmpty(rngId)) {
        result = OM_RingBufferGet(rngId, buffer, maxbytes);
    }
    VOS_SpinUnlockIntUnlock(&g_exEntity.lockTxTask, flag);

    return result;
}

/* 将数据从ring buffer里取出 */
VOS_VOID NFEXT_FlushRingBuffer(OM_RING_ID rngId)
{
    NfExtDataRingBuf data     = {0};
    VOS_ULONG        flag     = 0UL;
    VOS_INT          result   = 0;
    VOS_INT          flushCnt = 0;

    do {
        result = NFEXT_RingBufferGet(rngId, (VOS_CHAR*)(&data), (VOS_INT)sizeof(NfExtDataRingBuf));
        if (result == sizeof(NfExtDataRingBuf)) {
            NF_EXT_MEM_FREE(ACPU_PID_NFEXT, data.data);
            flushCnt++;
        }
    } while (result == sizeof(NfExtDataRingBuf));

    TTF_LOG2(ACPU_PID_NFEXT, 0, PS_PRINT_WARNING, "Flush ring buffer!", result, flushCnt);

    VOS_SpinLockIntLock(&g_exEntity.lockTxTask, flag);
    OM_RingBufferFlush(rngId);
    VOS_SpinUnlockIntUnlock(&g_exEntity.lockTxTask, flag);
}

/* 将需要通过OAM发送的数据放到自处理任务的RingBuf里面，由自处理任务发送出去 */
VOS_UINT32 NFEXT_AddDataToRingBuf(NfExtDataRingBuf *ringBuf)
{
    VOS_UINT32       isNeedWakeUp = VOS_FALSE;
    VOS_INT          result;
    NfExtDataRingBuf data = {0};

    if (g_exEntity.ringBufId == VOS_NULL_PTR) {
        TTF_LOG(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_WARNING, "g_exEntity.ringBufId is null!\n");
        return VOS_ERR;
    }

    /* 空到非空，唤醒任务处理勾包 */
    if (NFEXT_RingBufferIsEmpty(g_exEntity.ringBufId)) {
        isNeedWakeUp = VOS_TRUE;
    }

    result = NFEXT_RingBufferPut(g_exEntity.ringBufId, (VOS_CHAR*)ringBuf, (VOS_INT)(sizeof(NfExtDataRingBuf)));
    if (result == sizeof(NfExtDataRingBuf)) {
        if (isNeedWakeUp == VOS_TRUE) {
            NFEXT_SndDataNotify();
        }
        return VOS_OK;
    } else if (result == 0) {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_BUF_FULL_DROP);
        /* 队列满，唤醒任务处理勾包 */
        NFEXT_SndDataNotify();
        return VOS_ERR;
    } else {
        (VOS_VOID)NFEXT_RingBufferGet(g_exEntity.ringBufId, (VOS_CHAR*)(&data), result);
        NF_EXT_STATS_INC(1, NF_EXT_STATS_PUT_BUF_FAIL);
        return VOS_ERR;
    }
}

/* 接收到OM配置可维可测信息捕获配置请求 */
STATIC VOS_VOID NFEXT_RcvNfExtInfoCfgReq(const MsgBlock *msg)
{
    const OM_IpsMntnCfgReq* nfExtCfgReq = (OM_IpsMntnCfgReq*)msg;
    IPS_OmMntnCfgCnf        nfExtCfgCnf = {{0}};

    /* 构建回复消息 */
    /* Fill DIAG trans msg header */
    TTF_SET_MSG_SENDER_ID(&(nfExtCfgCnf.diagHdr), ACPU_PID_NFEXT);
    TTF_SET_MSG_RECEIVER_ID(&(nfExtCfgCnf.diagHdr), MSP_PID_DIAG_APP_AGENT); /* 把应答消息发送给DIAG，由DIAG把透传命令的处理结果发送给HIDS工具 */
    TTF_SET_MSG_LEN(&(nfExtCfgCnf.diagHdr), sizeof(IPS_OmMntnCfgCnf) - VOS_MSG_HEAD_LENGTH);
    nfExtCfgCnf.diagHdr.msgId = ID_IPS_OM_MNTN_INFO_CONFIG_CNF;

    /* DIAG透传命令中的特定信息 */
    nfExtCfgCnf.diagHdr.originalId = nfExtCfgReq->diagHdr.originalId;
    nfExtCfgCnf.diagHdr.terminalId = nfExtCfgReq->diagHdr.terminalId;
    nfExtCfgCnf.diagHdr.timeStamp  = nfExtCfgReq->diagHdr.timeStamp;
    nfExtCfgCnf.diagHdr.sn         = nfExtCfgReq->diagHdr.sn;

    /* 填充回复OM申请的确认信息 */
    nfExtCfgCnf.ipsMntnCfgCnf.command = nfExtCfgReq->ipsMntnCfgReq.command;
    nfExtCfgCnf.ipsMntnCfgCnf.result  = IPS_MNTN_RESULT_OK;

    /* 发送OM透明消息 */
    IPSMNTN_SndCfgCnf2Om(sizeof(IPS_OmMntnCfgCnf), &nfExtCfgCnf);
}

/* NFExt接收到来自OM模块的消息处理 */
VOS_VOID NFEXT_RcvOmMsg(const MsgBlock *msg)
{
    VOS_UINT16 msgId;

    if (msg == VOS_NULL_PTR) {
        return;
    }

    msgId = (VOS_UINT16)(*((VOS_UINT32*)((VOS_UINT8*)(msg) + VOS_MSG_HEAD_LENGTH)));

    switch (msgId) {
        case ID_OM_IPS_ADVANCED_TRACE_CONFIG_REQ:
            IPSMNTN_TraceAdvancedCfgReq(msg);
            break;
        case ID_OM_IPS_MNTN_INFO_CONFIG_REQ:
            NFEXT_RcvNfExtInfoCfgReq(msg);
            break;
        case ID_OM_IPS_MNTN_TRAFFIC_CTRL_REQ:
            IPSMNTN_TrafficCtrlReq(msg);
            break;
        default:
            TTF_LOG1(ACPU_PID_NFEXT, DIAG_MODE_COMM, PS_PRINT_WARNING, "NFExt_RcvConfig:Receive Unkown Type Message!\n",
                msgId);
            break;
    }
}

/* 绑定Task到指定CPU上面 */
VOS_VOID NFEXT_BindToCpu(VOS_VOID)
{
    VOS_LONG ret;
    VOS_INT  cpu;
    pid_t    targetPid;

    /* 获取当前线程的Pid */
    targetPid = current->pid;
    /* 获取当前线程的affinity */
    ret = sched_getaffinity(targetPid, &(g_exEntity.origMask));
    if (ret < 0) {
        TTF_PRINT_ERR("[init]<NFEXT_BindToCpu>: unable to get cpu affinity\n");
        return;
    }

    (VOS_VOID)memset_s(&(g_exEntity.currMask), cpumask_size(), 0, cpumask_size());
    /* 设置当前线程的affinity */
    for_each_cpu(cpu, &(g_exEntity.origMask))
    {
        /* 绑定CPU0 */
        if ((cpu > 0) && (cpumask_test_cpu(cpu, &(g_exEntity.origMask)))) {
            cpumask_set_cpu((unsigned int)cpu, &(g_exEntity.currMask));
        }
    }

    if (cpumask_weight(&(g_exEntity.currMask)) == 0) {
        cpumask_set_cpu(0, &(g_exEntity.currMask));
        return;
    }

    ret = sched_setaffinity(targetPid, &(g_exEntity.currMask));
    if (ret < 0) {
        TTF_PRINT_ERR("[init]<NFEXT_BindToCpu>: unable to set cpu affinity\n");
        return;
    }
}

#define NF_ONCE_DEAL_MAX_CNT 200

VOS_VOID NFEXT_ProcDataNotify(VOS_VOID)
{
    NfExtDataRingBuf data;
    VOS_INT          result = 0;
    VOS_INT          transResult;
    VOS_INT          firstErrCode = 0;
    VOS_UINT32       dealCntOnce  = 0;
    VOS_BOOL         rptErrFlag   = VOS_FALSE;

    if (g_exEntity.ringBufId == VOS_NULL_PTR) {
        TTF_LOG(ACPU_PID_NFEXT, 0, PS_PRINT_WARNING, "NFExt_ProcDataNotify : ERROR : ringBufId is NULL!\n");
        return;
    }

    do {
        /* 一次任务调度，最多处理200个勾包 */
        if (dealCntOnce >= NF_ONCE_DEAL_MAX_CNT) {
            NFEXT_SndDataNotify();
            break;
        }
        (VOS_VOID)memset_s(&data, sizeof(NfExtDataRingBuf), 0, sizeof(NfExtDataRingBuf));
        result = NFEXT_RingBufferGet(g_exEntity.ringBufId, (VOS_CHAR*)&data, sizeof(NfExtDataRingBuf));
        if (result == sizeof(NfExtDataRingBuf)) {
            dealCntOnce++;
            if (mdrv_diag_get_conn_state() != VOS_YES) {
                /* 为防止只有fc流控打开的情况下， NFExt_ProcHidsDisconnNotify中走不到清除分支，这里continue继续释放 */
                NF_EXT_MEM_FREE(ACPU_PID_NFEXT, data.data);
                rptErrFlag = VOS_TRUE;
                continue;
            }

            transResult = mdrv_diag_trans_report((mdrv_diag_trans_ind_s*)(data.data));
            if ((transResult != VOS_OK) && (rptErrFlag == VOS_FALSE)) {
                firstErrCode = transResult;
                rptErrFlag   = VOS_TRUE;
            }
            NF_EXT_MEM_FREE(ACPU_PID_NFEXT, data.data);
        }
    } while (result == sizeof(NfExtDataRingBuf));

    if ((result != sizeof(NfExtDataRingBuf)) && (result != 0)) {
        rptErrFlag = VOS_TRUE;
        NF_EXT_STATS_INC(1, NF_EXT_STATS_GET_BUF_FAIL);
        NFEXT_FlushRingBuffer(g_exEntity.ringBufId);
    }

    if (rptErrFlag == VOS_TRUE) {
        TTF_LOG3(
            ACPU_PID_NFEXT, 0, PS_PRINT_WARNING, "NFExt_ProcDataNotify, ERROR!\n", dealCntOnce, result, firstErrCode);
    }
}

VOS_VOID NFEXT_ProcHidsDisconnNotify(VOS_VOID)
{
#if (FEATURE_NFEXT == FEATURE_ON)
    VOS_UINT32 mask = 0;

    mask = g_exHookMask.fcMask ^ g_exEntity.curHookOnMask;
    /* 只有fc流控打开的场景，在NFExt_ProcDataNotify中继续释放 */
    if (mask != 0) {
        /* step1.ringbuffer清0 */
        NFEXT_FlushRingBuffer(g_exEntity.ringBufId);
        /* step2.卸载当前除流控外钩子函数 */
        NFEXT_UnregHooks(mask);
    }
#endif
}

/* NFExt事件处理函数 */
STATIC VOS_VOID NFEXT_EventProc(VOS_UINT32 event)
{
    if (event & NFEXT_DATA_PROC_NOTIFY) {
        NFEXT_ProcDataNotify();
    }
    if (event & NFEXT_HIDS_DISCONN_NOTIFY) {
        NFEXT_ProcHidsDisconnNotify();
    }
}

/* NFExt可维可测控制消息处理函数 */
VOS_VOID NFEXT_MsgProc(const struct MsgCB *msg)
{
    if (msg == VOS_NULL_PTR) {
        TTF_PRINT_WARNING("NFEXT_MsgProc: Message is NULL!\n");
        return;
    }

    switch (TTF_GET_MSG_SENDER_ID(msg)) {
        case MSP_PID_DIAG_APP_AGENT: /* 来自OM的透传消息处理 */
            NFEXT_RcvOmMsg((void*)msg);
            break;
        default:
            break;
    }
}

/* 读取事件并处理 */
STATIC VOS_VOID NFEXT_RcvEventProc(VOS_UINT32 taskId)
{
    VOS_UINT32       event       = 0;
    MsgBlock        *msg         = VOS_NULL_PTR;
    const VOS_UINT32 expectEvent = NFEXT_DATA_PROC_NOTIFY | NFEXT_HIDS_DISCONN_NOTIFY | VOS_MSG_SYNC_EVENT;
    const VOS_UINT32 eventMask   = VOS_EVENT_ANY | VOS_EVENT_WAIT;

    while (1) {
        if (VOS_EventRead(expectEvent, eventMask, 0, &event) != VOS_OK) {
            TTF_PRINT_WARNING("NFEXT_FidTask: read event error.\n");
            continue;
        }
        /* 事件处理 */
        if (event != VOS_MSG_SYNC_EVENT) {
            NFEXT_EventProc(event);
            continue;
        }

        msg = (MsgBlock*)VOS_GetMsg(taskId);
        if (msg != VOS_NULL_PTR) {
            if (TTF_GET_MSG_RECEIVER_ID(msg) == ACPU_PID_NFEXT) {
                NFEXT_MsgProc(msg);
            }

            (VOS_VOID)VOS_FreeMsg(ACPU_PID_NFEXT, msg);
        }
    }
}

/* 负责调用OAM消息发送接口，发送消息到HIDS */
VOS_VOID NFEXT_FidTask(VOS_UINT32 para1, VOS_UINT32 para2, VOS_UINT32 para3, VOS_UINT32 para4)
{
    VOS_UINT32 taskId = VOS_GetCurrentTaskID();

    if (taskId == PS_NULL_UINT32) {
        TTF_PRINT_WARNING("NFEXT_FidTask: taskId is invalid.\n");
        return;
    }

    if (VOS_CreateEvent(taskId) != VOS_OK) {
        TTF_PRINT_WARNING("NFEXT_FidTask: create event fail.\n");
        return;
    }

    g_nfExtTaskId = taskId;

    NFEXT_BindToCpu();

    NFEXT_RcvEventProc(taskId);
}

#define NF_EXT_RING_BUF_SIZE (8 * 1024 - 1) /* 环形buff的大小 */

/* NFExt_Pid初始化 */
VOS_UINT32 NFEXT_PidInit(enum VOS_InitPhaseDefine ip)
{
    if (ip == VOS_IP_LOAD_CONFIG) {
        /* 申请RingBuffer */
        g_exEntity.ringBufId = OM_RingBufferCreate(NF_EXT_RING_BUF_SIZE);
        if (g_exEntity.ringBufId == VOS_NULL_PTR) {
            TTF_PRINT_ERR("[init]<NFEXT_PidInit>: ERROR: Create ring buffer Failed!\n");
            return VOS_ERR;
        }
        /* 初始化锁 */
        VOS_SpinLockInit(&(g_exEntity.lockTxTask));
    }

    return VOS_OK;
}

/* NFExt_Fid初始化 */
VOS_UINT32 NFEXT_FidInit(enum VOS_InitPhaseDefine ip)
{
    VOS_UINT32 result = VOS_ERR;

    if (ip == VOS_IP_LOAD_CONFIG) {
#if (FEATURE_NFEXT == FEATURE_ON)
        /* 先完成模块初始化 */
        if (NFEXT_Init() != 0) {
            TTF_PRINT_ERR("[init]<NFEXT_FidInit>: NFEXT_Init FAIL!\n");
            return VOS_ERR;
        }
#endif
        /* 可维可测模块注册PID */
        result = VOS_RegisterPIDInfo(ACPU_PID_NFEXT, (InitFunType)NFEXT_PidInit, (MsgFunType)NFEXT_MsgProc);
        if (result != VOS_OK) {
            TTF_PRINT_ERR("[init]<NFEXT_FidInit>: VOS_RegisterPIDInfo FAIL!\n");
            return VOS_ERR;
        }

        result = VOS_RegisterMsgTaskEntry(ACPU_FID_NFEXT, NFEXT_FidTask);
        if (result != VOS_OK) {
            TTF_PRINT_ERR("[init]<NFEXT_FidInit>: VOS_RegisterMsgTaskEntry fail!\n");
            return VOS_ERR;
        }

        result = VOS_RegisterMsgTaskPrio(ACPU_FID_NFEXT, VOS_PRIORITY_M4);
        if (result != VOS_OK) {
            TTF_PRINT_ERR("[init]<NFEXT_FidInit>: VOS_RegisterTaskPrio Failed!\n");
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

/* 注册回调函数，用于diag模块diag连接上和断开时通知diag连接状态 */
VOS_VOID NFEXT_DiagConnStateNotifyCB(mdrv_diag_state_e state)
{
    if (state == DIAG_STATE_DISCONN) {
        (VOS_VOID)VOS_EventWrite(g_nfExtTaskId, NFEXT_HIDS_DISCONN_NOTIFY);
    }
}
