/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: This module is to collect TCP/IP stack parameters
 * Author: linlixin2@huawei.com
 * Create: 2019-03-19
 */

#include "ip_para_collec.h"

#include <linux/net.h>
#include <net/sock.h>
#include <linux/list.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/tcp.h>

#include "ip_para_collec_ex.h"
#include "netlink_handle.h"
#ifdef CONFIG_HW_NETWORK_SLICE
#include "network_slice_management.h"
#endif

#ifdef CONFIG_STREAM_DETECT
#include "stream_detect.h"
#endif

#define TCP_HDR_DOFF_QUAD 4
#define TCP_HDR_IHL_QUAD 4

static u32 g_channel_live_tstamp[CHANNEL_NUM];
static struct app_ctx *g_app_ctx = NULL;
static struct tcp_rtt_of_app g_tcp_rtt_backup[TCP_RTT_ARRAY_LEN];
static int g_rtt_index = 0;

inline u32 get_sock_uid(struct sock *sk)
{
	return (u32)sk->sk_uid.val;
}

#if defined(CONFIG_HUAWEI_KSTATE) || defined(CONFIG_MPTCP)
inline u32 get_sock_pid(struct sock *sk)
{
	if (sk->sk_socket == NULL)
		return 0;
	return (u32)sk->sk_socket->pid;
}
#endif

static bool is_local_or_lan_addr(__be32 ip_addr)
{
	if (ipv4_is_loopback(ip_addr) ||
		ipv4_is_multicast(ip_addr) ||
		ipv4_is_local_multicast(ip_addr) ||
		ipv4_is_lbcast(ip_addr) ||
		ipv4_is_private_10(ip_addr) ||
		ipv4_is_private_172(ip_addr) ||
		ipv4_is_private_192(ip_addr) ||
		ipv4_is_linklocal_169(ip_addr))
		return true;
	else
		return false;
}

static bool is_private_v6_addr(struct sock *sk)
{
	const unsigned int clat_ipv4_index = 3;
	__be32 addr;
	int addr_type;
#if IS_ENABLED(CONFIG_IPV6)
	if (sk == NULL)
		return false;
	addr_type = ipv6_addr_type(&sk->sk_v6_daddr);
	if ((sk->sk_family == AF_INET6) &&
		(addr_type & IPV6_ADDR_MAPPED)) {
		addr = sk->sk_v6_daddr.s6_addr32[clat_ipv4_index];
		return is_local_or_lan_addr(addr);
	}
#endif
	return false;
}

static bool is_sk_daddr_private(struct sock *sk)
{
	if (sk->sk_family == AF_INET6)
		return is_private_v6_addr(sk);
	else if (sk->sk_family == AF_INET)
		return is_local_or_lan_addr(sk->sk_daddr);

	return false;
}

static bool need_monitor(struct sock *sk, unsigned int direction)
{
	struct tcp_sock *tp = NULL;
	if (direction == NF_INET_NEW_SK_HOOK)
		return true;
	if (sk == NULL || !sk_fullsock(sk))
		return false;

	tp = tcp_sk(sk);
	return (tp->monitored == 1);
}

static void update_channel_alive_tstamp(u32 dev_id)
{
	if (dev_id < 0 || dev_id >= CHANNEL_NUM)
		return;

	g_channel_live_tstamp[dev_id] = tcp_jiffies32;
}

static bool is_channel_alive(u32 dev_id)
{
	u32 time;
	if (dev_id < 0 ||
		dev_id >= CHANNEL_NUM)
		return true;

	if (g_channel_live_tstamp[dev_id] == 0)
		return true;

	time = jiffies_to_msecs(tcp_jiffies32 - g_channel_live_tstamp[dev_id]);

	if (time <= CHANNEL_NO_DATA_MAX_TIME)
		return true;

	return false;
}

static bool need_update_rtt_ofo(struct sock *sk)
{
	struct tcp_sock *tp = NULL;
	u32 time;

	if (sk == NULL || !sk_fullsock(sk) || is_sk_daddr_private(sk))
		return false;
	if (sk->sk_state != TCP_ESTABLISHED &&
		sk->sk_state != TCP_SYN_SENT)
		return false;

	tp = tcp_sk(sk);
	if (tp->monitored == 0)
		return false;

	time = jiffies_to_usecs(tcp_jiffies32 - tp->ofo_tstamp) *
		MULTIPLE_OF_RTT;
	if (tp->ofo_tstamp == 0)
		return false;

	if (!RB_EMPTY_ROOT(&tp->out_of_order_queue)) {
		if (before(tp->rcv_nxt_ofo, tp->rcv_nxt) &&
			tp->rcv_nxt_ofo != 0)
			return true;
		else if (tp->prior_srtt_us < MAX_RTT_US &&
			time > tp->prior_srtt_us + OFO_RTT_UPDATE_THRESHOLD_US)
			return true;
	} else {
		if (tp->rcv_nxt_ofo != 0 &&
			tp->ofo_tstamp != 0)
			return true;
	}

	return false;
}

void update_ofo_rtt_for_qoe(struct sock *sk)
{
	if (!need_update_rtt_ofo(sk))
		return;
	update_tcp_para_without_skb(sk, NF_INET_OFO_HOOK);
}

void dec_sk_num_for_qoe(struct sock *sk, int new_state)
{
	int old_state;

	if (!need_monitor(sk, NF_INET_DELETE_SK_HOOK))
		return;

	old_state = sk->sk_state;
	if ((old_state == TCP_ESTABLISHED ||
		old_state == TCP_SYN_SENT) &&
		new_state != TCP_ESTABLISHED &&
		new_state != TCP_SYN_SENT)
		update_tcp_para_without_skb(sk, NF_INET_DELETE_SK_HOOK);
}

static void delete_old_rtt_sample_impl(int *tcp_sk_num, int *sk_rtt_sum,
	struct sock *sk)
{
	struct tcp_sock *tp = NULL;

	if (sk == NULL ||
		!sk_fullsock(sk))
		return;
	if (tcp_sk_num == NULL ||
		sk_rtt_sum == NULL)
		return;

	tp = tcp_sk(sk);
	if (tp->monitored == 0)
		return;

	(*tcp_sk_num)--;
	*sk_rtt_sum -= (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
	tp->prior_srtt_us = 0;
	tp->rtt_update_tstamp = 0;
}

static void increace_sk_num(int *tcp_sk_num, struct tcp_sock *tp)
{
	if (tp == NULL || tcp_sk_num == NULL)
		return;
	if (tp->rtt_update_tstamp == 0)
		(*tcp_sk_num)++;
}

static void update_sk_num_common(int *tcp_sk_num, int *sk_rtt_sum,
	struct sock *sk, unsigned int direction)
{
	struct tcp_sock *tp = NULL;

	if (sk == NULL ||
		!sk_fullsock(sk))
		return;
	if (tcp_sk_num == NULL ||
		sk_rtt_sum == NULL)
		return;

	tp = tcp_sk(sk);
	if (direction == NF_INET_NEW_SK_HOOK) {
		tp->monitored = 1;
	} else if (direction == NF_INET_DELETE_SK_HOOK) {
		if (tp->monitored == 0 || tp->rtt_update_tstamp == 0)
			return;
		tp->monitored = 0;

		(*tcp_sk_num)--;
		*sk_rtt_sum -= (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
		tp->prior_srtt_us = 0;
		tp->rtt_update_tstamp = 0;
	}
}

static void update_sk_rtt_sum_common(int *tcp_sk_num, int *sk_rtt_sum,
	struct sock *sk)
{
	struct tcp_sock *tp = NULL;
	if (tcp_sk_num == NULL || sk_rtt_sum == NULL)
		return;
	if (sk == NULL || !sk_fullsock(sk))
		return;
	if (sk->sk_state != TCP_ESTABLISHED &&
		sk->sk_state != TCP_SYN_SENT)
		return;

	tp = tcp_sk(sk);
	if (tp->monitored == 0)
		return;

	increace_sk_num(tcp_sk_num, tp);
	*sk_rtt_sum -= (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
	tp->prior_srtt_us = (tp->srtt_us > MAX_RTT_US) ?
		MAX_RTT_US : tp->srtt_us;
	tp->rtt_update_tstamp = tcp_jiffies32;
	*sk_rtt_sum += (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
}

static void update_sk_rtt_retrans_common(int *tcp_sk_num, int *sk_rtt_sum,
	struct sock *sk)
{
	struct inet_connection_sock *icsk = NULL;
	struct tcp_sock *tp = NULL;

	if (tcp_sk_num == NULL || sk_rtt_sum == NULL)
		return;
	if (sk == NULL || !sk_fullsock(sk))
		return;
	if (sk->sk_state != TCP_ESTABLISHED &&
		sk->sk_state != TCP_SYN_SENT)
		return;
	tp = tcp_sk(sk);
	if (tp->monitored == 0)
		return;
	icsk = inet_csk(sk);
	if (icsk == NULL)
		return;

	increace_sk_num(tcp_sk_num, tp);
	*sk_rtt_sum -= (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
	tp->prior_srtt_us = jiffies_to_usecs(icsk->icsk_rto) * MULTIPLE_OF_RTT;
	tp->prior_srtt_us = (tp->prior_srtt_us > MAX_RTT_US_RETRANS) ?
		MAX_RTT_US_RETRANS : tp->prior_srtt_us;
	tp->rtt_update_tstamp = tcp_jiffies32;
	*sk_rtt_sum += (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
}

static void update_sk_rtt_ofo_common(int *tcp_sk_num, int *sk_rtt_sum,
	struct sock *sk)
{
	struct tcp_sock *tp = NULL;
	u32 time;

	if (tcp_sk_num == NULL || sk_rtt_sum == NULL)
		return;
	if (sk == NULL || !sk_fullsock(sk))
		return;
	if (sk->sk_state != TCP_ESTABLISHED &&
		sk->sk_state != TCP_SYN_SENT)
		return;
	tp = tcp_sk(sk);
	if (tp->monitored == 0)
		return;

	time = jiffies_to_usecs(tcp_jiffies32 - tp->ofo_tstamp) *
		MULTIPLE_OF_RTT;
	increace_sk_num(tcp_sk_num, tp);
	*sk_rtt_sum -= (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
	tp->prior_srtt_us = (time > MAX_RTT_US) ?
		MAX_RTT_US : time;
	tp->rtt_update_tstamp = tcp_jiffies32;
	*sk_rtt_sum += (tp->prior_srtt_us / MULTIPLE_OF_RTT / US_MS);
}

static void delete_old_rtt_sample(struct sock *sk)
{
	struct tcp_sock *tp = NULL;
	u32 time;

	tp = tcp_sk(sk);
	if (tp == NULL)
		return;
	if (tp->monitored == 0)
		return;
	if (tp->rtt_update_tstamp == 0)
		return;

	time = jiffies_to_msecs(tcp_jiffies32 - tp->rtt_update_tstamp);
	if (time > SRTT_LIFE_CYCLE_MS)
		update_tcp_para_without_skb(sk, NF_INET_DELETE_OLD_RTT_HOOK);
}

void update_ofo_tstamp_for_qoe(struct sock *sk)
{
	struct tcp_sock *tp = NULL;

	if (sk == NULL || !sk_fullsock(sk))
		return;

	tp = tcp_sk(sk);

	if (!RB_EMPTY_ROOT(&tp->out_of_order_queue)) {
		if (tp->rcv_nxt_ofo == 0 &&
			tp->ofo_tstamp == 0) {
			tp->rcv_nxt_ofo = tp->rcv_nxt + tp->rcv_wnd;
			tp->ofo_tstamp = tcp_jiffies32;
		} else if (before(tp->rcv_nxt_ofo, tp->rcv_nxt) &&
			tp->rcv_nxt_ofo != 0) {
			tp->rcv_nxt_ofo = tp->rcv_nxt + tp->rcv_wnd;
			tp->ofo_tstamp = tcp_jiffies32;
		}
	} else {
		if (tp->ofo_tstamp != 0) {
			tp->rcv_nxt_ofo = 0;
			tp->ofo_tstamp = 0;
		}
	}

	delete_old_rtt_sample(sk);
}

/*
 * the udp pkts from hooks which pid and uid can get.
 *
 * hooks 1  2  3  4
 * sock  0  0  Y  Y
 * UID   N  N  y  y
 * PID   N  N  N  N
 *
 * the tcp pkts from hooks which pid and uid can get.
 *
 * hooks 1  2  3  4
 * sock  1  0  1  1
 * UID   y  N  Y  Y
 * PID   N  N  Y  Y
 */
static u32 match_app(struct sock *sk, struct tcp_res *stat,
	u8 protocol, u32 direction)
{
	u32 uid;
	u32 pid;

	// forward
	if (stat->uid == 0)
		return BACKGROUND;
	if (sk == NULL)
		return FORGROUND_UNMACH;
	if ((protocol == IPPROTO_TCP) && (!sk_fullsock(sk)))
		return FORGROUND_UNMACH; // ignore timewait or request socket
	uid = get_sock_uid(sk);
#if defined(CONFIG_HUAWEI_KSTATE) || defined(CONFIG_MPTCP)
	if (stat->pid == 0) {
		if (uid == stat->uid)
			return FORGROUND_MACH; // udp_out
	} else {
		// add in any pid if pid=0 is not existence udp_out
		if (protocol == IPPROTO_UDP && uid == stat->uid)
			return BACKGROUND;
		// tcp_in
		if (direction == NF_INET_LOCAL_IN)
			pid = 0;
		else
			pid = get_sock_pid(sk);
		// tcp_out
		if (uid == stat->uid && pid == stat->pid)
			return FORGROUND_MACH;
	}
#else
	if (uid == stat->uid)
		return FORGROUND_MACH;
#endif

	return FORGROUND_UNMACH;
}

static void set_dev_name(struct sock *sk, const struct sk_buff *skb)
{
	unsigned int dev_max_len;

	if (sk == NULL || skb == NULL || skb->dev == NULL)
		return;

#ifdef CONFIG_HW_WIFIPRO
	if ((1 << sk->sk_state) & TCPF_SYN_SENT) {
		dev_max_len = strnlen(skb->dev->name,
			IFNAMSIZ - 1);
		strncpy(sk->wifipro_dev_name, skb->dev->name,
			dev_max_len);
		sk->wifipro_dev_name[dev_max_len] = '\0';
	}
#endif
}

static void update_post_routing_para(struct tcp_res_info *stat,
	struct sk_buff *skb, struct sock *sk, struct tcphdr *th, s32 tcp_len)
{
	struct tcp_sock *tp = NULL;

	tp = (struct tcp_sock *)sk;
	if (sk->sk_state == TCP_ESTABLISHED) {
		stat->out_pkt++;
		stat->out_len += skb->len;
	} else if (sk->sk_state == TCP_SYN_SENT) {
		if (th->syn == 1) {
			stat->tcp_syn_pkt++;
			set_dev_name(sk, skb);
			update_sk_num_common(
				&stat->tcp_sk_num,
				&stat->sk_rtt_sum,
				sk, NF_INET_NEW_SK_HOOK);
		}
	} else {
		if (th->fin == 1)
			stat->tcp_fin_pkt++;
	}
	if (tp->snd_nxt != 0 &&
		before(ntohl(th->seq), tp->snd_nxt) &&
		(sk->sk_state == TCP_ESTABLISHED ||
		sk->sk_state == TCP_SYN_SENT))
		stat->out_rts_pkt++;
}

/*
 * tcp pkt_in pid=0(default),uid is ok, pkt_out pid,uid is ok.
 */
static void update_tcp_para(struct tcp_res_info *stat, struct sk_buff *skb,
	struct sock *sk, u8 direction, u16 payload_len)
{
	struct tcphdr *th = tcp_hdr(skb);
	struct tcp_sock *tp = NULL;
	u32 rtt_ms;
	s32 tcp_len;

	if (direction == NF_INET_FORWARD) // sk == NULL
		stat->out_all_pkt++;
	if (sk == NULL)
		return;
	if (th == NULL)
		return;
	if (direction == NF_INET_BUFHOOK)
		return;
	tp = (struct tcp_sock *)sk;
	if (tp == NULL)
		return;
	tcp_len = payload_len - th->doff * TCP_HDR_DOFF_QUAD; // 32bits to byte
	if (tcp_len < 0)
		return;
	if (direction == NF_INET_LOCAL_IN) {
		if (sk->sk_state != TCP_ESTABLISHED)
			return;
		stat->in_pkt++;
		stat->in_len += skb->len;
		if (before(ntohl(th->seq) + tcp_len - 1, tp->rcv_nxt) &&
			tcp_len > 0)
			stat->in_rts_pkt++;

		rtt_ms = (tp->srtt_us) / US_MS / 8; // 8 srtt translate to rtt
		stat->rtt += (rtt_ms > MAX_RTT) ? MAX_RTT : rtt_ms;
	} else if (direction == NF_INET_POST_ROUTING) {
		update_post_routing_para(stat, skb, sk, th, tcp_len);
	} else if (direction == NF_INET_LOCAL_OUT) {
		stat->out_all_pkt++;
	}
}
/*
 * udp udp_in uid=0, udp_out pid=0(or any pid,uid is ok),uid is ok
 */
void update_udp_para(struct tcp_res_info *stat, struct sk_buff *skb,
	u8 direction)
{
	if (direction == NF_INET_UDP_IN_HOOK) {
		stat->in_udp_pkt++;
		stat->in_udp_len += skb->len;
	} else if (direction == NF_INET_POST_ROUTING) {
		stat->out_udp_pkt++;
		stat->out_udp_len += skb->len;
	} else if (direction == NF_INET_FORWARD || direction == NF_INET_LOCAL_OUT) {
		stat->out_all_pkt++;
	}
}

static int match_net_dev(struct sk_buff *skb, struct sock *sk, u32 *dev_id)
{
	char *dev_name = NULL;

	if (skb == NULL || sk == NULL || dev_id == NULL)
		return NET_DEV_ERR;
	if (skb->dev == NULL || skb->dev->name == NULL) {
#ifdef CONFIG_HW_WIFIPRO
		if (sk_fullsock(sk) &&
			(sk->sk_family == AF_INET ||
			sk->sk_family == AF_INET6) &&
			sk->sk_protocol == IPPROTO_TCP)
			dev_name = sk->wifipro_dev_name;
#endif
	} else {
		dev_name = skb->dev->name;
	}

	if (dev_name == NULL)
		return NET_DEV_ERR;

	*dev_id = 0;
	if (strncmp(dev_name, DS_NET, DS_NET_LEN) == 0)
		*dev_id = DS_NET_ID;
	else if (strncmp(dev_name, DS_NET_SLAVE, DS_NET_LEN) == 0)
		*dev_id = DS_NET_SLAVE_ID;
	else if (strncmp(dev_name, WIFI_NET, WIFI_NET_LEN) == 0)
		*dev_id = WIFI_NET_ID;
	else if (strncmp(dev_name, WIFI_NET_SLAVE, WIFI_NET_LEN) == 0)
		*dev_id = WIFI_NET_SLAVE_ID;
	else
		return NET_DEV_ERR;
	return RTN_SUCC;
}

static struct app_list *get_match_node_from_list(struct sock *sk,
	struct app_list **background,
	unsigned int cpu, u8 protocol, unsigned int direction)
{
	struct app_stat *app = NULL;
	struct app_list *pos = NULL;
	struct app_list *select = NULL;
	u32 app_type;

	if (sk == NULL || background == NULL)
		return NULL;

	if (cpu < 0 || cpu >= CONFIG_NR_CPUS)
		return NULL;

	app = &g_app_ctx->cur[cpu];

	if (list_empty_careful(&app->head))
		return NULL;

	list_for_each_entry(pos, &app->head, head) {
		app_type = match_app(sk, &pos->stat, protocol, direction);
		if (app_type == BACKGROUND) {
			if (select == NULL)
				select = pos;

			*background = pos;
		} else if (app_type == FORGROUND_MACH) {
			select = pos;
		}

		if (select != NULL && *background != NULL &&
			select != *background)
			break;
	}
	return select;
}

static void update_tcp_rtt_sk_num(int *tcp_sk_num, int *sk_rtt_sum,
	struct sock *sk, unsigned int direction)
{
	if (direction == NF_INET_NEW_SK_HOOK ||
		direction == NF_INET_DELETE_SK_HOOK)
		update_sk_num_common(tcp_sk_num, sk_rtt_sum, sk, direction);
	else if (direction == NF_INET_UPDATE_RTT_HOOK)
		update_sk_rtt_sum_common(tcp_sk_num, sk_rtt_sum, sk);
	else if (direction == NF_INET_RETRANS_TIMER_HOOK)
		update_sk_rtt_retrans_common(tcp_sk_num, sk_rtt_sum, sk);
	else if (direction == NF_INET_OFO_HOOK)
		update_sk_rtt_ofo_common(tcp_sk_num, sk_rtt_sum, sk);
	else if (direction == NF_INET_DELETE_OLD_RTT_HOOK)
		delete_old_rtt_sample_impl(tcp_sk_num, sk_rtt_sum, sk);
}

static bool sk_born_after_monitor(struct sock *sk, unsigned long fore_tstamp)
{
	if (sk == NULL || !sk_fullsock(sk))
		return false;
	if (fore_tstamp == 0)
		return true;
#ifdef CONFIG_HW_DPIMARK_MODULE
	return !time_before(sk->sk_born_stamp, fore_tstamp);
#else
	return true;
#endif
}

static struct tcp_rtt_info *get_backup_tcp_rtt_info(struct sock *sk,
	unsigned int cpu, u32 dev_id)
{
	u32 uid;
	int array_index;

	if (sk == NULL ||
		!sk_fullsock(sk))
		return NULL;

	if (dev_id < 0 ||
		dev_id >= CHANNEL_NUM)
		return NULL;

	if (cpu < 0 || cpu >= CONFIG_NR_CPUS)
		return NULL;

	uid = get_sock_uid(sk);
	array_index = g_rtt_index + 1;
	array_index = array_index % TCP_RTT_ARRAY_LEN;

	if (uid != g_tcp_rtt_backup[array_index].uid)
		return NULL;

	if (!sk_born_after_monitor(sk,
		g_tcp_rtt_backup[array_index].fore_tstamp))
		return NULL;

	return &g_tcp_rtt_backup[array_index].info[cpu][dev_id];
}

static void process_stat_info_tcp_layer(struct sock *sk,
	unsigned int cpu, u8 protocol, unsigned int direction, u32 dev_id)
{
	struct app_list *pos_selec = NULL;
	struct app_list *background_pos = NULL;
	struct tcp_rtt_info *info = NULL;
	int sk_num_delta;
	int sk_rtt_delta;

	pos_selec = get_match_node_from_list(sk,
		&background_pos, cpu, protocol, direction);
	if (pos_selec == NULL || background_pos == NULL)
		return;
	if (dev_id < 0 ||
		dev_id >= CHANNEL_NUM)
		return;

	if (direction >= NF_INET_NEW_SK_HOOK && direction <= NF_INET_OFO_HOOK) {
		sk_num_delta = background_pos->stat.info[dev_id].tcp_sk_num;
		sk_rtt_delta = background_pos->stat.info[dev_id].sk_rtt_sum;
		update_tcp_rtt_sk_num(
			&background_pos->stat.info[dev_id].tcp_sk_num,
			&background_pos->stat.info[dev_id].sk_rtt_sum,
			sk, direction);
		sk_num_delta = background_pos->stat.info[dev_id].tcp_sk_num -
			sk_num_delta;
		sk_rtt_delta = background_pos->stat.info[dev_id].sk_rtt_sum -
			sk_rtt_delta;

		if (sk_num_delta == 0 && sk_rtt_delta == 0)
			return;

		if (pos_selec != background_pos && sk_born_after_monitor(sk,
			pos_selec->fore_tstamp)) {
			pos_selec->stat.info[dev_id].tcp_sk_num +=
				sk_num_delta;
			pos_selec->stat.info[dev_id].sk_rtt_sum +=
				sk_rtt_delta;
		}
		info = get_backup_tcp_rtt_info(sk, cpu, dev_id);
		if (info != NULL) {
			info->tcp_sk_num += sk_num_delta;
			info->sk_rtt_sum += sk_rtt_delta;
		}
	} else if (direction == NF_INET_TCP_DUPACK_IN_HOOK) {
		pos_selec->stat.info[dev_id].in_dupack_pkt++;
	}
}

static void stat_proces(struct sk_buff *skb, struct sock *sk,
	const struct nf_hook_state *state, u8 protocol, u16 payload_len)
{
	struct app_stat *app = NULL;
	struct app_list *pos_selec = NULL;
	struct app_list *background_pos = NULL;
	unsigned int cpu = smp_processor_id();
	u32 dev_id;

	if (skb == NULL || sk == NULL)
		return;
	if (cpu < 0 || cpu >= CONFIG_NR_CPUS)
		return;
	if (state == NULL)
		return;

	app = &g_app_ctx->cur[cpu];
	spin_lock_bh(&app->lock);

	if (match_net_dev(skb, sk, &dev_id) != RTN_SUCC)
		goto unlock;

	update_channel_alive_tstamp(dev_id);

	if (state->hook >= NF_INET_TCP_DUPACK_IN_HOOK) {
		process_stat_info_tcp_layer(sk,
			cpu, protocol, state->hook, dev_id);
	} else {
		pos_selec = get_match_node_from_list(sk, &background_pos, cpu,
			protocol, state->hook);
		if (pos_selec == NULL)
			goto unlock;

		if (protocol == IPPROTO_TCP)
			update_tcp_para(&pos_selec->stat.info[dev_id], skb, sk,
				state->hook, payload_len);
		else if (protocol == IPPROTO_UDP)
			update_udp_para(&pos_selec->stat.info[dev_id], skb,
				state->hook);
	}

unlock:
	spin_unlock_bh(&app->lock);
}

void update_dupack_num(struct sock *sk,
	bool is_dupack, unsigned int hook)
{
	if (is_dupack)
		update_tcp_para_without_skb(sk, NF_INET_TCP_DUPACK_IN_HOOK);
}

static void update_tcp_para_with_skb(struct sk_buff *skb, struct sock *sk,
	unsigned int hook)
{
	struct nf_hook_state state;
	if (skb == NULL || !need_monitor(sk, hook) || is_sk_daddr_private(sk))
		return;

	memset(&state, 0, sizeof(struct nf_hook_state));
	state.hook = hook;
	stat_proces(skb, sk, &state, IPPROTO_TCP, (u16)skb->len);
}

static bool is_tcp_udp_sock(struct sock *sk)
{
	if (sk == NULL)
		return false;
	if (!sk_fullsock(sk))
		return false;
	return (sk->sk_type == SOCK_STREAM || sk->sk_type == SOCK_DGRAM);
}

static unsigned int hook4(void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
	const struct iphdr *iph = NULL;
	struct sock *sk = NULL;
	u16 payload_len;

#ifdef CONFIG_HW_NETWORK_SLICE
	if (state->hook == NF_INET_LOCAL_OUT)
		slice_ip_para_report(skb, AF_INET);
#endif

#ifdef CONFIG_STREAM_DETECT
	if (state->hook == NF_INET_POST_ROUTING)
		stream_process_hook_out(skb, AF_INET);
	if (state->hook == NF_INET_LOCAL_IN)
		stream_process_hook_in(skb, AF_INET);
#endif

	iph = ip_hdr(skb);
	if (iph == NULL)
		return NF_ACCEPT;
	if (iph->protocol == IPPROTO_UDP && state->hook == NF_INET_LOCAL_IN)
		return NF_ACCEPT;
	sk = skb_to_full_sk(skb);
	if (!is_tcp_udp_sock(sk))
		return NF_ACCEPT;

	if (iph->protocol == IPPROTO_TCP &&
		is_local_or_lan_addr(sk->sk_daddr))
		return NF_ACCEPT;

	payload_len = ntohs(iph->tot_len) - iph->ihl * TCP_HDR_IHL_QUAD; // 32bits to byte
	if (iph->protocol == IPPROTO_TCP || iph->protocol == IPPROTO_UDP)
		stat_proces(skb, sk, state, iph->protocol, payload_len);
	return NF_ACCEPT;
}

void update_tcp_para_without_skb(struct sock *sk, unsigned int hook)
{
	struct sk_buff skb;

	if (sk == NULL)
		return;

	memset(&skb, 0, sizeof(struct sk_buff));
	skb.len = IP_TCP_HEAD_LEN;
	update_tcp_para_with_skb(&skb, sk, hook);
}

void udp_in_hook(struct sk_buff *skb, struct sock *sk)
{
	const struct iphdr *iph = NULL;
	u16 payload_len;
	struct nf_hook_state state;

	memset(&state, 0, sizeof(struct nf_hook_state));
	state.hook = NF_INET_UDP_IN_HOOK;
	if (skb == NULL)
		return;
	iph = ip_hdr(skb);
	if (iph == NULL)
		return;
	payload_len = ntohs(iph->tot_len) - iph->ihl * TCP_HDR_IHL_QUAD; // 32bits to byte
	stat_proces(skb, sk, &state, iph->protocol, payload_len);
}

void udp6_in_hook(struct sk_buff *skb, struct sock *sk)
{
	const struct ipv6hdr *iph = NULL;
	u16 payload_len;
	struct nf_hook_state state;

	memset(&state, 0, sizeof(struct nf_hook_state));
	state.hook = NF_INET_UDP_IN_HOOK;

	if (skb == NULL || sk == NULL)
		return;
	iph = ipv6_hdr(skb);
	if (iph == NULL)
		return;
	payload_len = ntohs(iph->payload_len);
	stat_proces(skb, sk, &state, iph->nexthdr, payload_len);
}

static unsigned int hook6(void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
	const struct ipv6hdr *iph = NULL;
	struct sock *sk = NULL;
	u16 payload_len;

#ifdef CONFIG_HW_NETWORK_SLICE
	if (state->hook == NF_INET_LOCAL_OUT)
		slice_ip_para_report(skb, AF_INET6);
#endif

#ifdef CONFIG_STREAM_DETECT
	if (state->hook == NF_INET_POST_ROUTING)
		stream_process_hook_out(skb, AF_INET6);
	if (state->hook == NF_INET_LOCAL_IN)
		stream_process_hook_in(skb, AF_INET6);
#endif

	iph = ipv6_hdr(skb);
	if (iph == NULL)
		return NF_ACCEPT;
	if (iph->nexthdr == IPPROTO_UDP && state->hook == NF_INET_LOCAL_IN)
		return NF_ACCEPT;
	sk = skb_to_full_sk(skb);
	if (!is_tcp_udp_sock(sk))
		return NF_ACCEPT;

	if (iph->nexthdr == IPPROTO_TCP &&
		is_private_v6_addr(sk))
		return NF_ACCEPT;

	payload_len = ntohs(iph->payload_len);
	if (iph->nexthdr == IPPROTO_TCP || iph->nexthdr == IPPROTO_UDP)
		stat_proces(skb, sk, state, iph->nexthdr, payload_len);

	return NF_ACCEPT;
}

static struct nf_hook_ops net_hooks[] = {
	{
		.hook = hook4,
		.pf = PF_INET,
		.hooknum = NF_INET_LOCAL_IN,
		.priority = NF_IP_PRI_FILTER + 1,
	},
	{
		.hook = hook4,
		.pf = PF_INET,
		.hooknum = NF_INET_POST_ROUTING,
		.priority = NF_IP_PRI_FILTER + 1,
	},
	{
		.hook = hook4,
		.pf = PF_INET,
		.hooknum = NF_INET_FORWARD,
		.priority = NF_IP_PRI_FILTER - 1,
	},
	{
		.hook = hook4,
		.pf = PF_INET,
		.hooknum = NF_INET_LOCAL_OUT,
		.priority = NF_IP_PRI_FILTER - 1,
	},
	{
		.hook = hook6,
		.pf = PF_INET6,
		.hooknum = NF_INET_LOCAL_IN,
		.priority = NF_IP_PRI_FILTER + 1,
	},
	{
		.hook = hook6,
		.pf = PF_INET6,
		.hooknum = NF_INET_POST_ROUTING,
		.priority = NF_IP_PRI_FILTER + 1,
	},
	{
		.hook = hook6,
		.pf = PF_INET6,
		.hooknum = NF_INET_FORWARD,
		.priority = NF_IP_PRI_FILTER - 1,
	},
	{
		.hook = hook6,
		.pf = PF_INET6,
		.hooknum = NF_INET_LOCAL_OUT,
		.priority = NF_IP_PRI_FILTER - 1,
	},
};

static void add_to_res(struct tcp_res *add, struct tcp_res *sum)
{
	int i;

	for (i = 0; i < CHANNEL_NUM; i++) {
		sum->info[i].in_rts_pkt += add->info[i].in_rts_pkt;
		sum->info[i].in_pkt += add->info[i].in_pkt;
		sum->info[i].in_len += add->info[i].in_len;
		sum->info[i].out_rts_pkt += add->info[i].out_rts_pkt;
		sum->info[i].out_pkt += add->info[i].out_pkt;
		sum->info[i].out_len += add->info[i].out_len;
		sum->info[i].rtt += add->info[i].rtt;
		sum->info[i].out_all_pkt += add->info[i].out_all_pkt;
		sum->info[i].in_udp_pkt += add->info[i].in_udp_pkt;
		sum->info[i].out_udp_pkt += add->info[i].out_udp_pkt;
		sum->info[i].in_udp_len += add->info[i].in_udp_len;
		sum->info[i].out_udp_len += add->info[i].out_udp_len;
		sum->info[i].tcp_syn_pkt += add->info[i].tcp_syn_pkt;
		sum->info[i].tcp_fin_pkt += add->info[i].tcp_fin_pkt;
		sum->info[i].in_dupack_pkt += add->info[i].in_dupack_pkt;
		sum->info[i].tcp_sk_num += add->info[i].tcp_sk_num;
		sum->info[i].sk_rtt_sum += add->info[i].sk_rtt_sum;
		if (sum->info[i].in_len > MAX_U32_VALUE)
			sum->info[i].in_len = MAX_U32_VALUE;
		if (sum->info[i].out_len > MAX_U32_VALUE)
			sum->info[i].out_len = MAX_U32_VALUE;
		if (sum->info[i].rtt > MAX_U32_VALUE)
			sum->info[i].rtt = MAX_U32_VALUE;
		if (sum->info[i].in_udp_len > MAX_U32_VALUE)
			sum->info[i].in_udp_len = MAX_U32_VALUE;
		if (sum->info[i].out_udp_len > MAX_U32_VALUE)
			sum->info[i].out_udp_len = MAX_U32_VALUE;
	}
}

static void reset_stat(struct tcp_res *entry)
{
	int i;

	for (i = 0; i < CHANNEL_NUM; i++) {
		entry->info[i].in_rts_pkt = 0;
		entry->info[i].in_pkt = 0;
		entry->info[i].in_len = 0;
		entry->info[i].out_rts_pkt = 0;
		entry->info[i].out_pkt = 0;
		entry->info[i].out_len = 0;
		entry->info[i].rtt = 0;
		entry->info[i].out_all_pkt = 0;
		entry->info[i].in_udp_pkt = 0;
		entry->info[i].out_udp_pkt = 0;
		entry->info[i].in_udp_len = 0;
		entry->info[i].out_udp_len = 0;
		entry->info[i].tcp_syn_pkt = 0;
		entry->info[i].tcp_fin_pkt = 0;
		entry->info[i].in_dupack_pkt = 0;
		if (!is_channel_alive(i)) {
			entry->info[i].tcp_sk_num = 0;
			entry->info[i].sk_rtt_sum = 0;
		}
	}
}

static void reset_backup_tcp_rtt(unsigned int cpu)
{
	int j;
	int array_index;

	if (cpu < 0 || cpu >= CONFIG_NR_CPUS)
		return;

	array_index = g_rtt_index + 1;
	array_index = array_index % TCP_RTT_ARRAY_LEN;

	for (j = 0; j < CHANNEL_NUM; j++) {
		if (is_channel_alive(j))
			continue;
		g_tcp_rtt_backup[array_index].info[cpu][j].sk_rtt_sum = 0;
		g_tcp_rtt_backup[array_index].info[cpu][j].tcp_sk_num = 0;
	}
}

static void rm_list(struct list_head *list)
{
	struct list_head *p = NULL;
	struct list_head *n = NULL;
	struct app_list *pos = NULL;

	list_for_each_safe(p, n, list) {
		pos = list_entry(p, struct app_list, head);
		list_del_init(p);
		if (pos == NULL)
			return;
		kfree(pos);
	}
	list->prev = list;
	list->next = list;
}

static void cum_cpus_stat(struct app_stat *cur,
	struct tcp_collect_res *tcp, u16 cnt)
{
	struct app_list *pos = NULL;
	int i;
	bool need_add = true;

	spin_lock_bh(&cur->lock);
	list_for_each_entry(pos, &cur->head, head) {
		need_add = true;
		for (i = 0; i < tcp->cnt; i++) {
			if (pos->stat.pid == tcp->res[i].pid &&
				pos->stat.uid == tcp->res[i].uid) {
				add_to_res(&pos->stat, &tcp->res[i]);
				reset_stat(&pos->stat);
				need_add = false;
				break;
			}
		}
		if (need_add) {
			if (tcp->cnt >= cnt)
				break;
			tcp->res[tcp->cnt].pid = pos->stat.pid;
			tcp->res[tcp->cnt].uid = pos->stat.uid;
			add_to_res(&pos->stat, &tcp->res[tcp->cnt]);
			reset_stat(&pos->stat);
			tcp->cnt++;
		}
	}
	spin_unlock_bh(&cur->lock);
}

static void stat_report(unsigned long sync)
{
	struct res_msg *res = NULL;
	u16 cnt;
	u16 len;
	u32 i;
	u32 j;

	spin_lock_bh(&g_app_ctx->lock);
	g_app_ctx->timer.data = sync + 1; // number of reports
	g_app_ctx->timer.function = stat_report;
	mod_timer(&g_app_ctx->timer, jiffies + g_app_ctx->jcycle);
	cnt = g_app_ctx->cnt;
	len = sizeof(struct res_msg) + cnt * sizeof(struct tcp_res);
	res = kmalloc(len, GFP_ATOMIC);

	if (res == NULL)
		goto report_end;

	memset(res, 0, len);
	res->type = APP_QOE_RPT;
	res->len = len;
	res->res.tcp.sync = (u16)sync;

	for (i = 0; i < cnt; i++) {
		for (j = 0; j < CHANNEL_NUM; j++)
			res->res.tcp.res[i].info[j].dev_id = j;
	}
	for (i = 0; i < CONFIG_NR_CPUS; i++)
		cum_cpus_stat(g_app_ctx->cur + i, &res->res.tcp, cnt);

	if (g_app_ctx->fn)
		g_app_ctx->fn((struct res_msg_head *)res);

	kfree(res);

report_end:
	spin_unlock_bh(&g_app_ctx->lock);
}

static void save_tcp_rtt_of_fore_app(struct app_stat *stat, int cpu)
{
	struct list_head *p = NULL;
	struct list_head *n = NULL;
	struct app_list *pos = NULL;
	struct tcp_rtt_info *info = NULL;
	int i;

	if (stat == NULL)
		return;
	if (cpu < 0 || cpu >= CONFIG_NR_CPUS)
		return;

	list_for_each_safe(p, n, &stat->head) {
		pos = list_entry(p, struct app_list, head);
		if (pos == NULL)
			continue;
		if (pos->stat.uid == 0 || pos->stat.uid == DNS_UID)
			continue;
		if (g_rtt_index < 0 || g_rtt_index >= TCP_RTT_ARRAY_LEN)
			return;
		g_tcp_rtt_backup[g_rtt_index].uid = pos->stat.uid;
		g_tcp_rtt_backup[g_rtt_index].fore_tstamp = pos->fore_tstamp;
		for (i = 0; i < CHANNEL_NUM; i++) {
			info = &(g_tcp_rtt_backup[g_rtt_index].info[cpu][i]);
			info->tcp_sk_num = pos->stat.info[i].tcp_sk_num;
			info->sk_rtt_sum = pos->stat.info[i].sk_rtt_sum;
		}
		break;
	}
}

static void restore_tcp_rtt_for_fore_app(struct app_list *pos, int cpu)
{
	struct tcp_rtt_info *info = NULL;
	struct tcp_res *res = NULL;
	int i;
	int array_index;

	if (pos == NULL)
		return;

	res = &pos->stat;
	if (res == NULL ||
		res->uid == 0 ||
		res->uid == DNS_UID)
		return;
	if (cpu < 0 || cpu >= CONFIG_NR_CPUS)
		return;

	array_index = g_rtt_index + 1;
	array_index = array_index % TCP_RTT_ARRAY_LEN;

	if (res->uid != g_tcp_rtt_backup[array_index].uid)
		return;

	pos->fore_tstamp = g_tcp_rtt_backup[array_index].fore_tstamp;
	for (i = 0; i < CHANNEL_NUM; i++) {
		info = &(g_tcp_rtt_backup[array_index].info[cpu][i]);
		res->info[i].tcp_sk_num = info->tcp_sk_num;
		res->info[i].sk_rtt_sum = info->sk_rtt_sum;
	}
}

static void update_foreground_time_stamp(struct app_list *pos)
{
	if (pos == NULL)
		return;

	if (pos->stat.uid == 0 || pos->stat.uid == DNS_UID)
		return;

	pos->fore_tstamp = jiffies;
}

static void sync_apps_list(struct app_id *id, u16 cnt,
	struct app_stat *stat, int cpu)
{
	u16 i;
	bool need_new = true;
	struct list_head tmp_head;
	struct list_head *p = NULL;
	struct list_head *n = NULL;
	struct app_list *pos = NULL;

	tmp_head.next = &tmp_head;
	tmp_head.prev = &tmp_head;
	for (i = 0; i < cnt; i++) {
		need_new = true;
		list_for_each_safe(p, n, &stat->head) {
			pos = list_entry(p, struct app_list, head);
			if (pos->stat.uid != id[i].uid ||
				pos->stat.pid != id[i].pid)
				continue;
			list_move(p, &tmp_head);
			need_new = false;
			break;
		}
		if (need_new) {
			pos = kmalloc(sizeof(struct app_list), GFP_ATOMIC);
			if (pos == NULL)
				goto list_end;
			memset(pos, 0, sizeof(struct app_list));
			pos->stat.pid = id[i].pid;
			pos->stat.uid = id[i].uid;
			update_foreground_time_stamp(pos);
			restore_tcp_rtt_for_fore_app(pos, cpu);
			list_add(&pos->head, &tmp_head);
		}
	}
	save_tcp_rtt_of_fore_app(stat, cpu);
	rm_list(&stat->head);
	stat->head.prev = tmp_head.prev;
	stat->head.next = tmp_head.next;
	tmp_head.prev->next = &stat->head;
	tmp_head.next->prev = &stat->head;
	return;

list_end:
	save_tcp_rtt_of_fore_app(stat, cpu);
	rm_list(&tmp_head);
}

static void sync_to_cpus(struct tcp_collect_req *req)
{
	int i;
	struct app_stat *stat = g_app_ctx->cur;

	for (i = 0; i < CONFIG_NR_CPUS; i++) {
		spin_lock_bh(&stat[i].lock);
		sync_apps_list(req->id, req->cnt, &stat[i], i);
		reset_backup_tcp_rtt(i);
		spin_unlock_bh(&stat[i].lock);
	}
	g_app_ctx->cnt = req->cnt;
	g_rtt_index++;
	g_rtt_index = g_rtt_index % TCP_RTT_ARRAY_LEN;
}

static u32 ms_convert_jiffies(u32 cycle)
{
	return cycle / JIFFIES_MS;
}

static void process_sync(struct req_msg *msg)
{
	struct tcp_collect_req *req = &msg->req.tcp_req;
	u16 flag = req->req_flag;
	u32 cycle = req->rpt_cycle;

	pr_info("[IP_PARA]%s data: %d %d", __func__, req->req_flag, req->rpt_cycle);
	spin_lock_bh(&g_app_ctx->lock);
	if (flag & RESTART_SYNC) {
		g_app_ctx->timer.data = 0;
		g_app_ctx->timer.function = stat_report;
		g_app_ctx->jcycle = ms_convert_jiffies(cycle);
		mod_timer(&g_app_ctx->timer, jiffies + g_app_ctx->jcycle);
	} else {
		if (!timer_pending(&g_app_ctx->timer))
			goto sync_end;
		del_timer(&g_app_ctx->timer);
	}
sync_end:
	spin_unlock_bh(&g_app_ctx->lock);
}

static void process_app_update(struct req_msg *msg)
{
	struct tcp_collect_req *req = &msg->req.tcp_req;
	u16 flag = req->req_flag;
	u32 cycle = req->rpt_cycle;

	pr_info("[IP_PARA] %s data :%d %d %d", __func__, req->req_flag,
		req->rpt_cycle, req->cnt);
	spin_lock_bh(&g_app_ctx->lock);

	if (req->cnt > MAX_APP_LIST_LEN)
		goto app_end;

	if ((flag & FORFROUND_STAT) || (flag & BACKGROUND_STAT))
		sync_to_cpus(req);

	if (flag & RESTART_SYNC) {
		g_app_ctx->timer.data = 0;
		g_app_ctx->timer.function = stat_report;
		g_app_ctx->jcycle = ms_convert_jiffies(cycle);
		mod_timer(&g_app_ctx->timer, jiffies + g_app_ctx->jcycle);
	}

app_end:
	spin_unlock_bh(&g_app_ctx->lock);
}

static void cmd_process(struct req_msg_head *msg)
{
	if (msg->len > MAX_REQ_DATA_LEN)
		return;

	if (msg->len < sizeof(struct req_msg))
		return;

	if (msg->type == APP_QOE_SYNC_CMD)
		process_sync((struct req_msg *)msg);
	else if (msg->type == UPDATE_APP_INFO_CMD)
		process_app_update((struct req_msg *)msg);
	else
		pr_info("[IP_PARA]map msg error\n");
}

/*
 * Initialize internal variables and external interfaces
 */
msg_process *ip_para_collec_init(notify_event *fn)
{
	int i;
	int ret;

	if (fn == NULL)
		return NULL;

	g_app_ctx = kmalloc(sizeof(struct app_ctx), GFP_KERNEL);
	if (g_app_ctx == NULL)
		return NULL;
	memset(g_app_ctx, 0, sizeof(struct app_ctx));
	g_app_ctx->fn = fn;
	g_app_ctx->cur = kmalloc(sizeof(struct app_stat) * CONFIG_NR_CPUS,
				 GFP_KERNEL);
	if (g_app_ctx->cur == NULL)
		goto init_error;
	memset(g_app_ctx->cur, 0, sizeof(struct app_stat) * CONFIG_NR_CPUS);
	for (i = 0; i < CONFIG_NR_CPUS; i++) {
		spin_lock_init(&g_app_ctx->cur[i].lock);
		g_app_ctx->cur[i].head.prev = &g_app_ctx->cur[i].head;
		g_app_ctx->cur[i].head.next = &g_app_ctx->cur[i].head;
	}
	spin_lock_init(&g_app_ctx->lock);
	init_timer(&g_app_ctx->timer);
	memset(g_tcp_rtt_backup, 0,
		sizeof(struct tcp_rtt_of_app) * TCP_RTT_ARRAY_LEN);
	memset(g_channel_live_tstamp, 0, sizeof(u32) * CHANNEL_NUM);
	g_rtt_index = 0;
	ret = nf_register_net_hooks(&init_net, net_hooks,
				    ARRAY_SIZE(net_hooks));
	if (ret) {
		pr_info("[IP_PARA]nf_init_in ret=%d  ", i);
		goto init_error;
	}
	return cmd_process;

init_error:
	if (g_app_ctx->cur != NULL)
		kfree(g_app_ctx->cur);
	if (g_app_ctx != NULL)
		kfree(g_app_ctx);
	g_app_ctx = NULL;
	return NULL;
}

void ip_para_collec_exit(void)
{
	if (g_app_ctx->cur != NULL)
		kfree(g_app_ctx->cur);
	if (g_app_ctx != NULL)
		kfree(g_app_ctx);
	g_app_ctx = NULL;
}
