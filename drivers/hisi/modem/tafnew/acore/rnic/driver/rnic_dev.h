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

#ifndef __RNIC_DEV_H__
#define __RNIC_DEV_H__

#include <linux/types.h>
#include <linux/etherdevice.h>
#include <linux/atomic.h>
#include <linux/notifier.h>
#include <linux/cpu.h>
#include <linux/workqueue.h>
#include <linux/version.h>
#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM))
#include <linux/timer.h>
#endif
#include "adrv.h"
#include "product_config.h"
#include "rnic_dev_def.h"
#include "rnic_dev_i.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#if (defined(LLT_OS_VER))
#define STATIC
#else
#define STATIC static
#endif

#define RNIC_DEV_CTX()		(&rnic_dev_context)

#define RNIC_DEFAULT_MTU	ETH_DATA_LEN
#define RNIC_MIN_MTU		68
#define RNIC_MAX_MTU		2000

#define RNIC_LB_IDLE		0
#define RNIC_LB_RUNING		1

#if (defined(CONFIG_BALONG_SPE))
#if (defined(CONFIG_BALONG_SPE_WAN))
#define RNIC_SPE_IPF_PORT_FLAG	(0)
#else
#define RNIC_SPE_IPF_PORT_FLAG	(1)
#endif
#endif

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM))
#define RNIC_TPUT_TIMER_INTERVAL (1*HZ)
#endif

#if (FEATURE_ON == FEATURE_RMNET_CUSTOM)
#define RNIC_DEV_NAME_PREFIX	"eth_x"
#else
#define RNIC_DEV_NAME_PREFIX	"rmnet"
#endif

#if (FEATURE_ON == FEATURE_RMNET_CUSTOM)
#define RNIC_DEV_NAME_SUFFIX_RMNET0		""
#else
#define RNIC_DEV_NAME_SUFFIX_RMNET0		"0"
#endif
#define RNIC_DEV_NAME_SUFFIX_RMNET1		"1"
#define RNIC_DEV_NAME_SUFFIX_RMNET2		"2"
#define RNIC_DEV_NAME_SUFFIX_RMNET3		"3"
#define RNIC_DEV_NAME_SUFFIX_RMNET4		"4"
#define RNIC_DEV_NAME_SUFFIX_RMNET5		"5"
#define RNIC_DEV_NAME_SUFFIX_RMNET6		"6"
#define RNIC_DEV_NAME_SUFFIX_RMNET7		"7"
#define RNIC_DEV_NAME_SUFFIX_RMNET8		"8"
#define RNIC_DEV_NAME_SUFFIX_RMNET9		"9"
#define RNIC_DEV_NAME_SUFFIX_RMNET10	"10"
#define RNIC_DEV_NAME_SUFFIX_RMNET11	"11"

#define RNIC_DEV_NAME_SUFFIX_RMNET_IMS00	"_ims00"
#define RNIC_DEV_NAME_SUFFIX_RMNET_IMS10	"_ims10"
#define RNIC_DEV_NAME_SUFFIX_RMNET_EMC0		"_emc0"
#define RNIC_DEV_NAME_SUFFIX_RMNET_EMC1		"_emc1"

#define RNIC_DEV_NAME_SUFFIX_RMNET_R_IMS00	"_r_ims00"
#define RNIC_DEV_NAME_SUFFIX_RMNET_R_IMS01	"_r_ims01"
#define RNIC_DEV_NAME_SUFFIX_RMNET_R_IMS10	"_r_ims10"
#define RNIC_DEV_NAME_SUFFIX_RMNET_R_IMS11	"_r_ims11"

#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN00	"_tun00"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN01	"_tun01"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN02	"_tun02"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN03	"_tun03"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN04	"_tun04"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN10	"_tun10"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN11	"_tun11"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN12	"_tun12"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN13	"_tun13"
#define RNIC_DEV_NAME_SUFFIX_RMNET_TUN14	"_tun14"

#define RNIC_DEV_NAME_ELEMENT(ifname)\
	{\
		RNIC_DEV_ID_##ifname,\
		RNIC_DEV_NAME_PREFIX,\
		RNIC_DEV_NAME_SUFFIX_##ifname,\
	}

struct rnic_dev_name_param_s {
	uint8_t devid;
	char *prefix;
	char *suffix;
};

struct rnic_dev_eth {
	uint8_t h_dest[ETH_ALEN];
	uint8_t h_source[ETH_ALEN];
	uint16_t h_proto;
};

struct rnic_rhc_stats_s {
	uint32_t dis_isolation_num;
	uint16_t cpufreq_update_num;
	uint16_t cur_rhc_level;
	uint32_t rxqueue_null;
	uint32_t rxqueue_rpsmap_null;
};

struct rnic_cpufreq_req_info_s {
	uint32_t last_req_freq;
	uint16_t init_succ;
	uint8_t first_cpu;
	uint8_t last_cpu;
};

struct rnic_rcc_config_s {
	uint32_t enable;
	uint32_t backlog_que_limit_len;
	unsigned long rps_bitmask;
	cpumask_var_t rps_cpumask ____cacheline_aligned;
};

struct rnic_dev_context_s {
	struct device *dev;
	struct net_device *netdev[RNIC_DEV_ID_BUTT];
	struct rnic_dev_priv_s *priv[RNIC_DEV_ID_BUTT];
	uint32_t ready;
	rnic_device_notifier_func dev_notifier_func;
#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
	rnic_ep_data_stats_hook data_stats_hook;
	rnic_ep_napi_stats_hook napi_stats_hook;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0))
	enum cpuhp_state online_state;
	uint32_t resevered;
#endif

#if (defined(CONFIG_BALONG_SPE))
	int spe_port;
	int ipf_port_flag;
#endif

#ifdef CONFIG_HISI_CPUFREQ_DT
	struct cpufreq_req cluster[RNIC_MAX_CLUSTERS];
#endif
	struct rnic_rhc_config_s rhc_cfg;
	struct rnic_cpufreq_req_info_s cpufreq_req[RNIC_MAX_CLUSTERS];
	uint16_t act_cluster_num;
	struct rnic_rcc_config_s rcc_cfg;
	struct rnic_rhc_stats_s rhc_stats;
};

struct rnic_lb_policy_s {
	struct rnic_dev_priv_s *priv;
	unsigned long lb_status;
	spinlock_t lb_lock ____cacheline_aligned;

	/* load balance config */
	bool cap_valid;
	bool enable;
	uint8_t cur_level;
	unsigned long cpu_bitmask;
	struct rnic_lb_level_config_s level_cfg[RNIC_LB_MAX_LEVEL];

	/* Variable for load balance cpu selection */
	int trig_cpu;
	atomic_t napi_cpu;
	cpumask_var_t cpumask_curr_avail ____cacheline_aligned;
	cpumask_var_t cpumask_orig;
	cpumask_var_t cpumask_candidacy;
	uint8_t weight_orig[NR_CPUS];
	uint8_t weight_remaind[NR_CPUS];
};

struct rnic_dev_priv_s {
	struct device *dev;
	struct net_device *netdev;

	struct net_device_stats stats;
	struct rnic_data_stats_s data_stats;
	struct rnic_dsflow_stats_s dsflow_stats;
	struct rnic_lb_stats_s lb_stats[NR_CPUS];

	uint8_t devid;
	uint8_t napi_enable;
	uint8_t gro_enable;
	uint8_t napi_weight;
	uint32_t napi_queue_length;
	unsigned long rps_pend_cpu_bitmask;

	struct sk_buff_head napi_queue ____cacheline_aligned;
	struct sk_buff_head input_queue ____cacheline_aligned;
	struct sk_buff_head process_queue ____cacheline_aligned;
	struct sk_buff_head rps_pend_queue ____cacheline_aligned;
	struct napi_struct napi ____cacheline_aligned;
	struct work_struct napi_work ____cacheline_aligned;

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
	struct sk_buff_head peth_rx_queue ____cacheline_aligned;
#endif

	struct rnic_dev_eth v4_eth ____cacheline_aligned;
	struct rnic_dev_eth v6_eth;

#if (defined(CONFIG_BALONG_SPE))
	struct rnic_dev_eth v4_fix_eth ____cacheline_aligned;
	struct rnic_dev_eth v6_fix_eth;

	int spe_port;
	int ipf_port_flag;
#endif

	unsigned long addr_family_mask;

	struct rnic_ps_iface_info_s v4_info;
	struct rnic_ps_iface_info_s v6_info;

	rnic_ps_iface_data_tx_func v4_data_tx_func;
	rnic_ps_iface_data_tx_func v6_data_tx_func;
	rnic_ps_iface_drop_notifier_func drop_notifier_func;

	struct rnic_lb_policy_s lb_policy;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	struct notifier_block cpu_hotplug_notifier;
#endif

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM))
	struct timer_list tput_timer;
	unsigned long rx_cmplt_mark;
#endif
};

extern struct rnic_dev_context_s rnic_dev_context;

struct net_device *rnic_get_netdev_by_devid(uint8_t devid);
struct rnic_dev_priv_s *rnic_get_priv(uint8_t devid);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* __RNIC_DEV_H__ */
