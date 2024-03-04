/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: This file is the internal header file for the
 *              TCP/IP parameter collection module.
 * Author: linlixin2@huawei.com
 * Create: 2019-03-19
 */

#ifndef _IP_PARA_COLLEC_EX_H
#define _IP_PARA_COLLEC_EX_H

#include <linux/skbuff.h>

#include "netlink_handle.h"
#include "ip_para_collec.h"

#define JIFFIES_MS (1000 / HZ)
#define MAX_U32_VALUE 2147483647
#define US_MS 1024
#define MAX_RTT 10000 // ms
#define MAX_RTT_US 80000000 // max value of rtt
#define MULTIPLE_OF_RTT 8
#define OFO_RTT_UPDATE_THRESHOLD_US 8000000 // 1000ms << 3
#define SRTT_LIFE_CYCLE_MS 5000

/* initialization function for external modules */
msg_process *ip_para_collec_init(notify_event *fn);
void ip_para_collec_exit(void);
void udp_in_hook(struct sk_buff *skb, struct sock *sk);
void udp6_in_hook(struct sk_buff *skb, struct sock *sk);
void dec_sk_num_for_qoe(struct sock *sk, int new_state);
void update_ofo_rtt_for_qoe(struct sock *sk);
void update_ofo_tstamp_for_qoe(struct sock *sk);
void update_tcp_para_without_skb(struct sock *sk,
	unsigned int hook);
void update_dupack_num(struct sock *sk,
	bool is_dupack, unsigned int hook);

#endif // _IP_PARA_COLLEC_EX_H
