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

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/percpu.h>
#include <linux/version.h>
#include "securec.h"
#include "rnic_dev.h"
#include "rnic_dev_debug.h"
#include "rnic_dev_config.h"
#include "rnic_dev_handle.h"
#include "rnic_policy_manage.h"


/*
 * rnic_kfree_skb_extern - Free skb base on flag.
 * @skb: message buffer
 * @flag: maa own flag
 */
STATIC void rnic_kfree_skb_extern(struct sk_buff *skb, uint32_t flag)
{
	dev_kfree_skb_any(skb);
}

/*
 * rnic_kfree_skb - Free sys own skb.
 * @skb: message buffer
 * @flag: maa own flag
 */
STATIC void rnic_kfree_skb(struct sk_buff *skb)
{
	rnic_kfree_skb_extern(skb, 0);
}

/*
 * rnic_napi_is_scheduled - Check if napi is scheduled.
 * @napi: napi context
 */
STATIC inline bool rnic_napi_is_scheduled(struct napi_struct *napi)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	return test_bit(NAPI_STATE_SCHED, &napi->state);
#else
	return napi->state & NAPIF_STATE_SCHED;
#endif
}




/*
 * rnic_napi_check_skb_gro - Check if skb can be handled by GRO engin.
 * @skb: sk buffer
 */
STATIC bool rnic_napi_check_skb_gro(struct sk_buff *skb)
{
	struct iphdr *iph = NULL;
	struct ipv6hdr *ipv6h = NULL;

	switch (skb->protocol) {
	case RNIC_NS_ETH_TYPE_IP:
		iph = (struct iphdr *)skb->data;
		if (iph->protocol == IPPROTO_TCP)
			return true;
		break;
	case RNIC_NS_ETH_TYPE_IPV6:
		ipv6h = (struct ipv6hdr *)skb->data;
		if (ipv6h->nexthdr == IPPROTO_TCP)
			return true;
		break;
	default:
		break;
	}

	return false;
}

/*
 * rnic_napi_schedule - Schedule napi for receive.
 * @info: private info of netdevice
 */
void rnic_napi_schedule(void *info)
{
	struct rnic_dev_priv_s *priv = (struct rnic_dev_priv_s *)info;
	uint32_t cur_cpu;

	cur_cpu = smp_processor_id();

	if (napi_schedule_prep(&priv->napi)) {
		__napi_schedule(&priv->napi);
		priv->lb_stats[cur_cpu].schedul_on_cpu_num++;
	} else {
		priv->lb_stats[cur_cpu].schedule_fail_num++;
	}
}

STATIC void rnic_lb(struct rnic_dev_priv_s *priv)
{
	struct rnic_lb_policy_s *policy = &priv->lb_policy;
	int napi_cpu;
	int cur_cpu;

	set_bit(RNIC_LB_RUNING, &policy->lb_status);

	cur_cpu = (int)smp_processor_id();
	priv->lb_stats[cur_cpu].rx_lb_num++;

	if (policy->trig_cpu != cur_cpu) {
		rnic_update_lb_cpumask(policy, cur_cpu);
	}
	rnic_select_cpu_candidacy(policy);
	napi_cpu = atomic_read(&policy->napi_cpu);
	priv->lb_stats[napi_cpu].select_cpu_num++;

	if (!cpu_online(napi_cpu) || (cur_cpu == napi_cpu)) {
		rnic_napi_schedule(priv);
		clear_bit(RNIC_LB_RUNING, &policy->lb_status);
		return;
	}

	priv->lb_stats[cur_cpu].hirq_num++;
	if (!schedule_work(&priv->napi_work))
		priv->lb_stats[cur_cpu].work_pend_exec_num++;

	clear_bit(RNIC_LB_RUNING, &policy->lb_status);
}

#ifdef CONFIG_RPS
/*
 * rnic_rx_congestion_check - Check if netdevice is in receivce congestion state.
 * @priv: private info of netdeive
 * @skb: sk buffer
 *
 * Return true when congestion occurs, otherwise return false.
 */
/*lint -save -e801*/
STATIC bool rnic_rx_congestion_check(struct rnic_dev_priv_s *priv,
				     struct sk_buff *skb)
{
	const struct rps_sock_flow_table *sock_flow_table = NULL;
	struct netdev_rx_queue *rxqueue = NULL;
	struct rps_dev_flow_table *flow_table = NULL;
	struct rps_map *map = NULL;
	struct rps_dev_flow *rflow = NULL;
	uint32_t tcpu;
	uint32_t hash;
	uint32_t next_cpu;
	uint32_t ident;
	bool ret = false;

	rcu_read_lock();
	rxqueue = priv->netdev->_rx;
	flow_table = rcu_dereference(rxqueue->rps_flow_table);
	map = rcu_dereference(rxqueue->rps_map);
	sock_flow_table = rcu_dereference(rps_sock_flow_table);
	if (flow_table == NULL || map == NULL || sock_flow_table == NULL)
		goto done;

	skb_reset_network_header(skb);
	hash = skb_get_hash(skb);
	if (!hash)
		goto done;

	/* First check into global flow table if there is a match */
	ident = sock_flow_table->ents[hash & sock_flow_table->mask];
	if ((ident ^ hash) & ~rps_cpu_mask)
		goto done;

	next_cpu = ident & rps_cpu_mask;

	/* OK, now we know there is a match,
	 * we can look at the local (per receive queue) flow table
	 */
	rflow = &flow_table->flows[hash & flow_table->mask];
	tcpu = rflow->cpu;

	/*
	 * If the desired CPU (where last recvmsg was done) is
	 * different from current CPU (one in the rx-queue flow
	 * table entry), current CPU is valid (< nr_cpu_ids)
	 * and online, and the current CPU's queue hasn't advanced
	 * beyond the last packet that was enqueued using this table entry.
	 */
	if (tcpu >= nr_cpu_ids || !cpu_online((int)tcpu))
		goto done;

	if (test_bit((int)tcpu, &priv->rps_pend_cpu_bitmask)) {
		priv->lb_stats[tcpu].rps_pend_num++;
		ret = true;
		goto done;
	}

	if (tcpu != next_cpu &&
	    ((int)(rflow->last_qtail -
	     per_cpu(softnet_data, tcpu).input_queue_head)) > 0 &&
	    (skb_queue_len(&per_cpu(softnet_data, tcpu).input_pkt_queue) +
	     skb_queue_len(&per_cpu(softnet_data, tcpu).process_queue)) >
	     rnic_dev_context.rcc_cfg.backlog_que_limit_len) {
		__test_and_set_bit((int)tcpu, &priv->rps_pend_cpu_bitmask);
		priv->lb_stats[tcpu].rps_pend_num++;
		ret = true;
		goto done;
	}

done:
	rcu_read_unlock();
	return ret;
}
/*lint -restore +e801*/
#endif

/*
 * rnic_napi_rx_skb - Post skb to the network using napi.
 * @priv: private info of netdeive
 * @budget: napi weight
 *
 * Return 0 on success, negative on failure
 */
/*lint -save -e801*/
STATIC int rnic_napi_rx_skb(struct rnic_dev_priv_s *priv, int budget)
{
	struct rnic_dev_eth *eth = NULL;
	struct sk_buff *skb = NULL;
	unsigned long flags;
	int rx_packets = 0;
	uint32_t rx_bytes = 0;
	bool again = true;

	while (again) {
		while ((skb = __skb_dequeue(&priv->process_queue))) {
			if (skb->hash)
				goto skb_deliver;

			skb_push(skb, ETH_HLEN);

			eth = skb->protocol == RNIC_NS_ETH_TYPE_IP ?
			      &priv->v4_eth : &priv->v6_eth;
			memcpy_s(skb->data, ETH_HLEN, eth, ETH_HLEN);
			skb->protocol = eth_type_trans(skb, priv->netdev);
	skb_deliver:
			if (priv->gro_enable && rnic_napi_check_skb_gro(skb)) {
				rx_bytes += skb->len - ETH_HLEN;
				napi_gro_receive(&priv->napi, skb);
			} else {
#ifdef CONFIG_RPS
				if (rnic_dev_context.rcc_cfg.enable &&
				    rnic_rx_congestion_check(priv, skb)) {
					__skb_queue_tail(&priv->rps_pend_queue, skb);
					continue;
				} else
#endif
				{
					rx_bytes += skb->len - ETH_HLEN;
					netif_receive_skb(skb);
				}
			}

			if (++rx_packets >= budget)
				goto rx_completed;
		}

		spin_lock_irqsave(&priv->napi_queue.lock, flags);
		if (skb_queue_empty(&priv->napi_queue))
			again = false;
		else
			skb_queue_splice_tail_init(&priv->napi_queue,
					&priv->process_queue);
		spin_unlock_irqrestore(&priv->napi_queue.lock, flags);
	}

rx_completed:
#ifdef CONFIG_RPS
	if (!skb_queue_empty(&priv->rps_pend_queue)) {
		priv->data_stats.rx_poll_pend_packets += 
			skb_queue_len(&priv->rps_pend_queue);
		skb_queue_splice_tail_init(&priv->process_queue, &priv->rps_pend_queue);
	}
#endif
	priv->stats.rx_packets += rx_packets;
	priv->stats.rx_bytes += rx_bytes;
	priv->data_stats.rx_packets += rx_packets;

	return rx_packets;
}
/*lint -restore +e801*/

/*
 * rnic_napi_poll - Polling function of napi.
 * @napi: napi contest
 * @budget: napi weight
 *
 * Return number of packet post to network.
 */
int rnic_napi_poll(struct napi_struct *napi, int budget)
{
	struct rnic_dev_priv_s *priv = container_of(napi,
						    struct rnic_dev_priv_s,
						    napi);
	uint32_t cur_cpu;
	int work = 0;
#ifdef CONFIG_RPS
	unsigned long flags;
	bool cong_stat = false;
#endif

	if (unlikely(budget <= 0))
		return budget;

	cur_cpu = smp_processor_id();
	priv->lb_stats[cur_cpu].poll_on_cpu_num++;

	work = rnic_napi_rx_skb(priv, budget);
#ifdef CONFIG_RPS
	if (!skb_queue_empty(&priv->rps_pend_queue)) {
		spin_lock_irqsave(&priv->napi_queue.lock, flags);
		skb_queue_splice_init(&priv->rps_pend_queue, &priv->napi_queue);
		spin_unlock_irqrestore(&priv->napi_queue.lock, flags);
		priv->rps_pend_cpu_bitmask = 0;
		cong_stat = true;
	}
#endif

	/* If weight not fully consumed, exit the polling mode */
	if (work < budget) {
		napi_complete(&priv->napi);
		priv->lb_stats[cur_cpu].napi_cmpl_num++;

#ifdef CONFIG_RPS
		/* Avoid input_queue or napi_queue residu,
		 * push to schedule rx napi again on cpu0.
		 */
		if (cong_stat) {
			priv->lb_stats[cur_cpu].napi_rx_push_num++;
			rnic_select_cpu_candidacy(&priv->lb_policy);
			if (!schedule_work(&priv->napi_work))
				priv->lb_stats[cur_cpu].work_pend_exec_num++;
		}
#endif
	}

	return work;
}

/*
 * rnic_napi_schedule_smp_on - Schedule napi on specific cpu for receive.
 * @priv: private info of netdeive
 * @targ_cpu: target cpu
 */
void rnic_napi_schedule_smp_on(struct rnic_dev_priv_s *priv, int targ_cpu)
{
	int wait = 0; /* asynchronous IPI */

	if (targ_cpu >= 0 && targ_cpu < nr_cpu_ids) {
		priv->lb_stats[targ_cpu].smp_on_cpu_num++;

		if (smp_call_function_single(targ_cpu, rnic_napi_schedule, priv, wait))
			priv->lb_stats[targ_cpu].smp_fail_num++;
	}
}

/*
 * rnic_napi_dispatcher_cb - Callback function of napi dispatch work.
 * @work: work context
 */
void rnic_napi_dispatcher_cb(struct work_struct *work)
{
	struct rnic_dev_priv_s *priv = container_of(work,
						    struct rnic_dev_priv_s,
						    napi_work);
	int napi_cpu;

	napi_cpu = atomic_read(&priv->lb_policy.napi_cpu);
	if (napi_cpu >= 0 && napi_cpu < nr_cpu_ids) {
		get_online_cpus();
		if (!cpu_online(napi_cpu)) {
			rnic_napi_schedule(priv);
			priv->lb_stats[napi_cpu].dispatch_fail_num++;
		} else if (!rnic_napi_is_scheduled(&priv->napi)) {
			rnic_napi_schedule_smp_on(priv, napi_cpu);
		} else {
			priv->lb_stats[napi_cpu].napi_sched_hold_num++;
		}
		put_online_cpus();
	}
}

/*
 * rnic_net_rx_skb - Post skb to network using netif_rx/netif_rx_ni.
 * @priv: private info of netdeive
 * @skb: sk buffer
 * @in_intr: interrupt context flag
 */
STATIC void rnic_net_rx_skb(struct rnic_dev_priv_s *priv,
			    struct sk_buff *skb, bool in_intr)
{
	struct rnic_dev_eth *eth = NULL;
	uint32_t data_len = skb->len;

	skb_push(skb, ETH_HLEN);

	eth = skb->protocol == RNIC_NS_ETH_TYPE_IP ?
	      &priv->v4_eth : &priv->v6_eth;
	memcpy_s(skb->data, ETH_HLEN, eth, ETH_HLEN);

	skb->protocol = eth_type_trans(skb, priv->netdev);

	if (in_intr)
		netif_rx(skb);
	else
		netif_rx_ni(skb);

	priv->stats.rx_packets++;
	priv->stats.rx_bytes += data_len;
	priv->data_stats.rx_packets++;
}

/*
 * rnic_net_rx_handle - Host rx handle function.
 * @priv: private info of netdevice
 * @skb: sk buffer
 */
STATIC void rnic_net_rx_handle(struct rnic_dev_priv_s *priv,
			       struct sk_buff *skb)
{

	if (in_interrupt()) {
		if (priv->napi_enable)
			__skb_queue_tail(&priv->input_queue, skb);
		else
			rnic_net_rx_skb(priv, skb, true);
	} else {
		rnic_net_rx_skb(priv, skb, false);
	}
}

/*
 * rnic_rx_handle - Rx handle function.
 * @devid: id of netdevice
 * @skb: sk buffer
 *
 * Return 0 on success, negative on failure
 */
/*lint -save -e801*/
int rnic_rx_handle(uint8_t devid, struct sk_buff *skb)
{
	struct rnic_dev_priv_s *priv = NULL;

	priv = rnic_get_priv(devid);
	if (unlikely(priv == NULL)) {
		RNIC_LOGE("device not found: devid is %d.", devid);
		rnic_kfree_skb(skb);
		return -ENODEV;
	}

	priv->data_stats.rx_total_packets++;
	priv->dsflow_stats.rx_packets++;
	priv->dsflow_stats.rx_bytes += skb->len;


	if (unlikely(skb->len > ETH_DATA_LEN)) {
		priv->data_stats.rx_length_errors++;
		goto out_free_drop;
	}


	if (unlikely(!netif_running(priv->netdev))) {
		priv->data_stats.rx_link_errors++;
		goto out_free_drop;
	}

	rnic_net_rx_handle(priv, skb);


	return 0;

out_free_drop:
	rnic_kfree_skb(skb);
	priv->stats.rx_dropped++;
	priv->data_stats.rx_dropped++;

	return -EFAULT;
}
/*lint -restore +e801*/

/*
 * rnic_rx_complete - Rx complete function.
 * @devid: id of netdevice
 *
 * Return 0 on success, negative on failure
 */
int rnic_rx_complete(uint8_t devid)
{
	struct rnic_dev_priv_s *priv = NULL;
	struct sk_buff *skb = NULL;
	unsigned long flags;

	priv = rnic_get_priv(devid);
	if (unlikely(priv == NULL)) {
		RNIC_LOGE("device not found: devid is %d.", devid);
		return -ENODEV;
	}

	priv->data_stats.rx_cmpl_num++;

	if (priv->napi_enable) {
		if (skb_queue_len(&priv->napi_queue) < priv->napi_queue_length) {
			spin_lock_irqsave(&priv->napi_queue.lock, flags);
			skb_queue_splice_tail_init(&priv->input_queue,
						   &priv->napi_queue);
			spin_unlock_irqrestore(&priv->napi_queue.lock, flags);
		} else {
			while ((skb = __skb_dequeue(&priv->input_queue))) {
				kfree_skb(skb);
				priv->stats.rx_dropped++;
				priv->data_stats.rx_dropped++;
				priv->data_stats.rx_enqueue_errors++;
			}
		}

		if (priv->lb_policy.enable) {
			if (!test_bit(RNIC_LB_RUNING, &priv->lb_policy.lb_status))
				rnic_lb(priv);
		} else {
			rnic_napi_schedule(priv);
		}
	}

	return 0;
}

/*
 * rnic_tx_hanlde - Tx handle function.
 * @priv: private info of netdevice
 * @skb: sk buffer
 */
/*lint -save -e801*/
netdev_tx_t rnic_tx_hanlde(struct rnic_dev_priv_s *priv, struct sk_buff *skb)
{
	rnic_ps_iface_data_tx_func data_tx_func = NULL;
	uint32_t data_len;
	int rc;

	priv->data_stats.tx_total_packets++;

	if (skb_linearize(skb)) {
		priv->data_stats.tx_linearize_errors++;
		goto out_free_drop;
	}

	if (skb->len < ETH_HLEN) {
		priv->data_stats.tx_length_errors++;
		goto out_free_drop;
	}

	skb_pull(skb, ETH_HLEN);
	priv->dsflow_stats.tx_bytes += skb->len;
	priv->dsflow_stats.tx_packets++;

	switch (ntohs(skb->protocol)) {
	case ETH_P_IP:
		data_tx_func = priv->v4_data_tx_func;
		break;
	case ETH_P_IPV6:
		data_tx_func = priv->v6_data_tx_func;
		break;
	default:
		priv->data_stats.tx_proto_errors++;
		goto out_free_drop;
	}

	if (data_tx_func != NULL) {
		data_len = skb->len;
		rc = data_tx_func(skb, priv->devid);
	} else {
		if (ntohs(skb->protocol) == ETH_P_IP && priv->drop_notifier_func)
			priv->drop_notifier_func(priv->devid);

		priv->data_stats.tx_carrier_errors++;
		goto out_free_drop;
	}

	if (!rc) {
		priv->stats.tx_bytes += data_len;
		priv->stats.tx_packets++;
		priv->data_stats.tx_packets++;
	} else {
		priv->stats.tx_dropped++;
		priv->data_stats.tx_dropped++;
		priv->data_stats.tx_iface_errors++;
	}

	return NETDEV_TX_OK;

out_free_drop:
	rnic_kfree_skb(skb);
	priv->stats.tx_dropped++;
	priv->data_stats.tx_dropped++;

	return NETDEV_TX_OK;
}
/*lint -restore +e801*/

/*
 * rnic_get_data_stats - Get data stats of netdevice.
 * @devid: id of netdevice
 *
 * Return data stats of netdevice.
 */
struct rnic_data_stats_s *rnic_get_data_stats(uint8_t devid)
{
	struct rnic_dev_priv_s *priv = NULL;

	priv = rnic_get_priv(devid);
	if (unlikely(priv == NULL)) {
		RNIC_LOGE("device not found: devid is %d.", devid);
		return NULL;
	}

	return &priv->data_stats;
}

/*
 * rnic_get_napi_stats - Get napi stats of netdevice.
 * @napi_stats: napi stats
 *
 * Return 0 on success, negative on failure.
 */
int rnic_get_napi_stats(uint8_t devid, struct rnic_napi_stats_s *napi_stats)
{
	struct rnic_dev_priv_s *priv = NULL;
	int act_cpu_nums = nr_cpu_ids;

	priv = rnic_get_priv(devid);
	if (unlikely(priv == NULL)) {
		RNIC_LOGE("device not found: devid is %d.", devid);
		return -EFAULT;
	}

	napi_stats->napi_enable = priv->napi_enable;
	napi_stats->gro_enable = priv->gro_enable;
	napi_stats->napi_weight = priv->napi_weight;
	napi_stats->napi_lb_enable = (uint8_t)priv->lb_policy.enable;
	napi_stats->lb_level = priv->lb_policy.cur_level;
	napi_stats->act_cpu_nums = (uint8_t)act_cpu_nums;

	memcpy_s(&napi_stats->lb_stats[0],
		 sizeof(struct rnic_lb_stats_s) * \
		 (act_cpu_nums > RNIC_NAPI_STATS_MAX_CPUS ?
		  RNIC_NAPI_STATS_MAX_CPUS : act_cpu_nums),
		 &priv->lb_stats[0],
		 sizeof(struct rnic_lb_stats_s) * \
		 (act_cpu_nums > RNIC_NAPI_STATS_MAX_CPUS ?
		  RNIC_NAPI_STATS_MAX_CPUS : act_cpu_nums));

	return 0;
}

/*
 * rnic_get_dsflow_stats - Get dsflow stats of netdevice.
 * @devid: id of netdevice
 *
 * Return dsflow stats of netdevice.
 */
struct rnic_dsflow_stats_s *rnic_get_dsflow_stats(uint8_t devid)
{
	struct rnic_dev_priv_s *priv = NULL;

	priv = rnic_get_priv(devid);
	if (unlikely(priv == NULL)) {
		RNIC_LOGE("device not found: devid is %d.", devid);
		return NULL;
	}

	return &priv->dsflow_stats;
}

/*
 * rnic_clear_dsflow_stats - Clear data flow stats.
 * @devid: id of netdevice
 *
 * Return 0 on success, negative on failure.
 */
int rnic_clear_dsflow_stats(uint8_t devid)
{
	struct rnic_dev_priv_s *priv = NULL;

	priv = rnic_get_priv(devid);
	if (unlikely(priv == NULL)) {
		RNIC_LOGE("device not found: devid is %d.", devid);
		return -ENODEV;
	}

	priv->dsflow_stats.rx_packets = 0;
	priv->dsflow_stats.tx_packets = 0;
	priv->dsflow_stats.rx_bytes   = 0;
	priv->dsflow_stats.tx_bytes   = 0;
	priv->dsflow_stats.rx_tcp_pkts = 0;
	priv->dsflow_stats.rx_non_tcp_pkts = 0;

	return 0;
}

/*
 * rnic_show_cpufreq_req_stats - Show cpufreq req stats.
 */
void rnic_show_cpufreq_req_stats(void)
{
	uint32_t i;

	pr_err("cpufreq_update_num            %6u\n", rnic_dev_context.rhc_stats.cpufreq_update_num);
	pr_err("dis_isolation_num             %6u\n", rnic_dev_context.rhc_stats.dis_isolation_num);
	pr_err("rxqueue_null                  %6u\n", rnic_dev_context.rhc_stats.rxqueue_null);
	pr_err("rxqueue_rpsmap_null           %6u\n", rnic_dev_context.rhc_stats.rxqueue_rpsmap_null);
	pr_err("act_cluster_num               %6u\n", rnic_dev_context.act_cluster_num);
	for (i = 0; i < RNIC_MAX_CLUSTERS; i++) {
		pr_err("[cluster%d] init_succ          %10u\n", i, rnic_dev_context.cpufreq_req[i].init_succ);
		pr_err("[cluster%d] first_cpu          %10u\n", i, rnic_dev_context.cpufreq_req[i].first_cpu);
		pr_err("[cluster%d] last_cpu           %10u\n", i, rnic_dev_context.cpufreq_req[i].last_cpu);
		pr_err("[cluster%d] last req freq      %10u\n", i, rnic_dev_context.cpufreq_req[i].last_req_freq);
	}
}

/*
 * rnic_show_data_stats - Show data stats of netdevice.
 * @devid: id of netdevice
 */
void rnic_show_data_stats(uint8_t devid)
{
	struct rnic_data_stats_s *data_stats = NULL;

	if (devid >= RNIC_DEV_ID_BUTT) {
		pr_err("devid %d overtop.\n", devid);
		return;
	}

	data_stats = rnic_get_data_stats(devid);
	if (data_stats == NULL) {
		pr_err("get data stats fail, devid: %d.\n", devid);
		return;
	}

	pr_err("[RMNET%d] tx stats start:\n", devid);
	pr_err("[RMNET%d] tx_total_packets               %10u\n", devid, data_stats->tx_total_packets);
	pr_err("[RMNET%d] tx_packets                     %10u\n", devid, data_stats->tx_packets);
	pr_err("[RMNET%d] tx_dropped                     %10u\n", devid, data_stats->tx_dropped);
	pr_err("[RMNET%d] tx_length_errors               %10u\n", devid, data_stats->tx_length_errors);
	pr_err("[RMNET%d] tx_proto_errors                %10u\n", devid, data_stats->tx_proto_errors);
	pr_err("[RMNET%d] tx_carrier_errors              %10u\n", devid, data_stats->tx_carrier_errors);
	pr_err("[RMNET%d] tx_iface_errors                %10u\n", devid, data_stats->tx_iface_errors);
	pr_err("[RMNET%d] tx_linearize_errors            %10u\n", devid, data_stats->tx_linearize_errors);
	pr_err("[RMNET%d] tx stats end:\n", devid);
	pr_err("[RMNET%d] rx stats start:\n", devid);
	pr_err("[RMNET%d] rx_total_packets               %10u\n", devid, data_stats->rx_total_packets);
	pr_err("[RMNET%d] rx_packets                     %10u\n", devid, data_stats->rx_packets);
	pr_err("[RMNET%d] rx_poll_pend_packets           %10u\n", devid, data_stats->rx_poll_pend_packets);
	pr_err("[RMNET%d] rx_cmpl_nums                   %10u\n", devid, data_stats->rx_cmpl_num);
	pr_err("[RMNET%d] rx_dropped                     %10u\n", devid, data_stats->rx_dropped);
	pr_err("[RMNET%d] rx_length_errors               %10u\n", devid, data_stats->rx_length_errors);
	pr_err("[RMNET%d] rx_link_errors                 %10u\n", devid, data_stats->rx_link_errors);
	pr_err("[RMNET%d] rx_enqueue_errors              %10u\n", devid, data_stats->rx_enqueue_errors);
	pr_err("[RMNET%d] rx_trans_errors                %10u\n", devid, data_stats->rx_trans_errors);
	pr_err("[RMNET%d] rx stats end:\n", devid);
}

/*
 * rnic_show_napi_stats - Show napi stats of netdevice.
 * @devid: id of netdevice
 */
void rnic_show_napi_stats(uint8_t devid)
{
	struct rnic_napi_stats_s napi_stats;
	struct rnic_dev_priv_s *priv = NULL;

	if (devid >= RNIC_DEV_ID_BUTT) {
		pr_err("devid %d overtop.\n", devid);
		return;
	}

	if (rnic_get_napi_stats(devid, &napi_stats)) {
		pr_err("get napi stats fail, devid: %d.\n", devid);
		return;
	}

	priv = rnic_get_priv(devid);

	pr_err("[RMNET%d] napi enable        :(0:disable/1:enable)    %d\n", devid, napi_stats.napi_enable);
	pr_err("[RMNET%d] gro enable         :(0:disable/1:enable)    %d\n", devid, napi_stats.gro_enable);
	pr_err("[RMNET%d] napi weight        :                        %d\n", devid, napi_stats.napi_weight);
	pr_err("[RMNET%d] napi lb enable     :(0:disable/1:enable)    %d\n", devid, napi_stats.napi_lb_enable);
	pr_err("[RMNET%d] napi_cpu           :                        %d\n", devid, atomic_read(&priv->lb_policy.napi_cpu));
	pr_err("[RMNET%d] lb current levle   :                        %d\n", devid, napi_stats.lb_level);
	pr_err("[RMNET%d] cur_rhc_level      :                        %d\n", devid, rnic_dev_context.rhc_stats.cur_rhc_level);
	pr_err("[RMNET%d] congest ctrl enable:                        %d\n", devid, rnic_dev_context.rcc_cfg.enable);
	pr_err("[RMNET%d] rps_cpu_mask       :                        %lx\n", devid, rnic_dev_context.rcc_cfg.rps_bitmask);
	pr_err("[RMNET%d] BKL_que_limit_len  :                        %d\n", devid, rnic_dev_context.rcc_cfg.backlog_que_limit_len);
	pr_err("[RMNET%d] napi scheduled     :                        %d\n", devid, rnic_napi_is_scheduled(&priv->napi));
	pr_err("[RMNET%d] input_queue len    :                        %d\n", devid, skb_queue_len(&priv->input_queue));
	pr_err("[RMNET%d] napi_queue len     :                        %d\n", devid, skb_queue_len(&priv->napi_queue));
	pr_err("[RMNET%d] rps_pend_queue len :                        %d\n", devid, skb_queue_len(&priv->rps_pend_queue));
	pr_err("[RMNET%d] process_queue len  :                        %d\n", devid, skb_queue_len(&priv->process_queue));
	pr_err("[RMNET%d] rx_lb_num          : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].rx_lb_num,
		napi_stats.lb_stats[1].rx_lb_num,
		napi_stats.lb_stats[2].rx_lb_num,
		napi_stats.lb_stats[3].rx_lb_num,
		napi_stats.lb_stats[4].rx_lb_num,
		napi_stats.lb_stats[5].rx_lb_num,
		napi_stats.lb_stats[6].rx_lb_num,
		napi_stats.lb_stats[7].rx_lb_num);
	pr_err("[RMNET%d] hirq_num           : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].hirq_num,
		napi_stats.lb_stats[1].hirq_num,
		napi_stats.lb_stats[2].hirq_num,
		napi_stats.lb_stats[3].hirq_num,
		napi_stats.lb_stats[4].hirq_num,
		napi_stats.lb_stats[5].hirq_num,
		napi_stats.lb_stats[6].hirq_num,
		napi_stats.lb_stats[7].hirq_num);
	pr_err("[RMNET%d] non_hirq_num       : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].non_hirq_num,
		napi_stats.lb_stats[1].non_hirq_num,
		napi_stats.lb_stats[2].non_hirq_num,
		napi_stats.lb_stats[3].non_hirq_num,
		napi_stats.lb_stats[4].non_hirq_num,
		napi_stats.lb_stats[5].non_hirq_num,
		napi_stats.lb_stats[6].non_hirq_num,
		napi_stats.lb_stats[7].non_hirq_num);
	pr_err("[RMNET%d] rps_pend_num       : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].rps_pend_num,
		napi_stats.lb_stats[1].rps_pend_num,
		napi_stats.lb_stats[2].rps_pend_num,
		napi_stats.lb_stats[3].rps_pend_num,
		napi_stats.lb_stats[4].rps_pend_num,
		napi_stats.lb_stats[5].rps_pend_num,
		napi_stats.lb_stats[6].rps_pend_num,
		napi_stats.lb_stats[7].rps_pend_num);
	/* for non 0 cpu, select_cpu_num = work_pend_exec_num + napi_sched_hold_num +
		dispatch_fail_num + schedul_on_cpu_num + schedule_fail_num + smp_fail_num */
	pr_err("[RMNET%d] select_cpu_num     : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].select_cpu_num,
		napi_stats.lb_stats[1].select_cpu_num,
		napi_stats.lb_stats[2].select_cpu_num,
		napi_stats.lb_stats[3].select_cpu_num,
		napi_stats.lb_stats[4].select_cpu_num,
		napi_stats.lb_stats[5].select_cpu_num,
		napi_stats.lb_stats[6].select_cpu_num,
		napi_stats.lb_stats[7].select_cpu_num);
	pr_err("[RMNET%d] work_pend_exec_num : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].work_pend_exec_num,
		napi_stats.lb_stats[1].work_pend_exec_num,
		napi_stats.lb_stats[2].work_pend_exec_num,
		napi_stats.lb_stats[3].work_pend_exec_num,
		napi_stats.lb_stats[4].work_pend_exec_num,
		napi_stats.lb_stats[5].work_pend_exec_num,
		napi_stats.lb_stats[6].work_pend_exec_num,
		napi_stats.lb_stats[7].work_pend_exec_num);
	pr_err("[RMNET%d] napi_sched_hold_num: %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].napi_sched_hold_num,
		napi_stats.lb_stats[1].napi_sched_hold_num,
		napi_stats.lb_stats[2].napi_sched_hold_num,
		napi_stats.lb_stats[3].napi_sched_hold_num,
		napi_stats.lb_stats[4].napi_sched_hold_num,
		napi_stats.lb_stats[5].napi_sched_hold_num,
		napi_stats.lb_stats[6].napi_sched_hold_num,
		napi_stats.lb_stats[7].napi_sched_hold_num);
	pr_err("[RMNET%d] dispatch_fail_num  : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].dispatch_fail_num,
		napi_stats.lb_stats[1].dispatch_fail_num,
		napi_stats.lb_stats[2].dispatch_fail_num,
		napi_stats.lb_stats[3].dispatch_fail_num,
		napi_stats.lb_stats[4].dispatch_fail_num,
		napi_stats.lb_stats[5].dispatch_fail_num,
		napi_stats.lb_stats[6].dispatch_fail_num,
		napi_stats.lb_stats[7].dispatch_fail_num);
	pr_err("[RMNET%d] schedul_on_cpu_num : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].schedul_on_cpu_num,
		napi_stats.lb_stats[1].schedul_on_cpu_num,
		napi_stats.lb_stats[2].schedul_on_cpu_num,
		napi_stats.lb_stats[3].schedul_on_cpu_num,
		napi_stats.lb_stats[4].schedul_on_cpu_num,
		napi_stats.lb_stats[5].schedul_on_cpu_num,
		napi_stats.lb_stats[6].schedul_on_cpu_num,
		napi_stats.lb_stats[7].schedul_on_cpu_num);
	pr_err("[RMNET%d] schedule_fail_num  : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].schedule_fail_num,
		napi_stats.lb_stats[1].schedule_fail_num,
		napi_stats.lb_stats[2].schedule_fail_num,
		napi_stats.lb_stats[3].schedule_fail_num,
		napi_stats.lb_stats[4].schedule_fail_num,
		napi_stats.lb_stats[5].schedule_fail_num,
		napi_stats.lb_stats[6].schedule_fail_num,
		napi_stats.lb_stats[7].schedule_fail_num);
	pr_err("[RMNET%d] smp_fail_num       : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].smp_fail_num,
		napi_stats.lb_stats[1].smp_fail_num,
		napi_stats.lb_stats[2].smp_fail_num,
		napi_stats.lb_stats[3].smp_fail_num,
		napi_stats.lb_stats[4].smp_fail_num,
		napi_stats.lb_stats[5].smp_fail_num,
		napi_stats.lb_stats[6].smp_fail_num,
		napi_stats.lb_stats[7].smp_fail_num);
	pr_err("[RMNET%d] smp_on_cpu_num     : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].smp_on_cpu_num,
		napi_stats.lb_stats[1].smp_on_cpu_num,
		napi_stats.lb_stats[2].smp_on_cpu_num,
		napi_stats.lb_stats[3].smp_on_cpu_num,
		napi_stats.lb_stats[4].smp_on_cpu_num,
		napi_stats.lb_stats[5].smp_on_cpu_num,
		napi_stats.lb_stats[6].smp_on_cpu_num,
		napi_stats.lb_stats[7].smp_on_cpu_num);
	pr_err("[RMNET%d] poll_on_cpu_num    : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].poll_on_cpu_num,
		napi_stats.lb_stats[1].poll_on_cpu_num,
		napi_stats.lb_stats[2].poll_on_cpu_num,
		napi_stats.lb_stats[3].poll_on_cpu_num,
		napi_stats.lb_stats[4].poll_on_cpu_num,
		napi_stats.lb_stats[5].poll_on_cpu_num,
		napi_stats.lb_stats[6].poll_on_cpu_num,
		napi_stats.lb_stats[7].poll_on_cpu_num);
	pr_err("[RMNET%d] napi_rx_push_num   : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].napi_rx_push_num,
		napi_stats.lb_stats[1].napi_rx_push_num,
		napi_stats.lb_stats[2].napi_rx_push_num,
		napi_stats.lb_stats[3].napi_rx_push_num,
		napi_stats.lb_stats[4].napi_rx_push_num,
		napi_stats.lb_stats[5].napi_rx_push_num,
		napi_stats.lb_stats[6].napi_rx_push_num,
		napi_stats.lb_stats[7].napi_rx_push_num);
	pr_err("[RMNET%d] napi_cmpl_num      : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].napi_cmpl_num,
		napi_stats.lb_stats[1].napi_cmpl_num,
		napi_stats.lb_stats[2].napi_cmpl_num,
		napi_stats.lb_stats[3].napi_cmpl_num,
		napi_stats.lb_stats[4].napi_cmpl_num,
		napi_stats.lb_stats[5].napi_cmpl_num,
		napi_stats.lb_stats[6].napi_cmpl_num,
		napi_stats.lb_stats[7].napi_cmpl_num);
	pr_err("[RMNET%d] hotplug_online_num : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].hotplug_online_num,
		napi_stats.lb_stats[1].hotplug_online_num,
		napi_stats.lb_stats[2].hotplug_online_num,
		napi_stats.lb_stats[3].hotplug_online_num,
		napi_stats.lb_stats[4].hotplug_online_num,
		napi_stats.lb_stats[5].hotplug_online_num,
		napi_stats.lb_stats[6].hotplug_online_num,
		napi_stats.lb_stats[7].hotplug_online_num);
	pr_err("[RMNET%d] hotplug_down_num   : %d %d %d %d %d %d %d %d\n",
		devid,
		napi_stats.lb_stats[0].hotplug_down_num,
		napi_stats.lb_stats[1].hotplug_down_num,
		napi_stats.lb_stats[2].hotplug_down_num,
		napi_stats.lb_stats[3].hotplug_down_num,
		napi_stats.lb_stats[4].hotplug_down_num,
		napi_stats.lb_stats[5].hotplug_down_num,
		napi_stats.lb_stats[6].hotplug_down_num,
		napi_stats.lb_stats[7].hotplug_down_num);
}

