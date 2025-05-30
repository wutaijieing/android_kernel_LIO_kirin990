/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the TCP protocol.
 *
 * Version:	@(#)tcp.h	1.0.2	04/28/93
 *
 * Author:	Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#ifndef _LINUX_TCP_H
#define _LINUX_TCP_H


#include <linux/skbuff.h>
#include <linux/win_minmax.h>
#include <net/sock.h>
#include <net/inet_connection_sock.h>
#include <net/inet_timewait_sock.h>
#include <uapi/linux/tcp.h>
#ifdef CONFIG_TCP_ARGO
#include <huawei_platform/emcom/argo/tcp_argo.h>
#endif /* CONFIG_TCP_ARGO */

static inline struct tcphdr *tcp_hdr(const struct sk_buff *skb)
{
	return (struct tcphdr *)skb_transport_header(skb);
}

static inline unsigned int __tcp_hdrlen(const struct tcphdr *th)
{
	return th->doff * 4;
}

static inline unsigned int tcp_hdrlen(const struct sk_buff *skb)
{
	return __tcp_hdrlen(tcp_hdr(skb));
}

static inline struct tcphdr *inner_tcp_hdr(const struct sk_buff *skb)
{
	return (struct tcphdr *)skb_inner_transport_header(skb);
}

static inline unsigned int inner_tcp_hdrlen(const struct sk_buff *skb)
{
	return inner_tcp_hdr(skb)->doff * 4;
}

static inline unsigned int tcp_optlen(const struct sk_buff *skb)
{
	return (tcp_hdr(skb)->doff - 5) * 4;
}

/* TCP Fast Open */
#define TCP_FASTOPEN_COOKIE_MIN	4	/* Min Fast Open Cookie size in bytes */
#define TCP_FASTOPEN_COOKIE_MAX	16	/* Max Fast Open Cookie size in bytes */
#ifdef CONFIG_MPTCP
#define TCP_FASTOPEN_COOKIE_SIZE 4	/* the size employed by this impl. */
#else
#define TCP_FASTOPEN_COOKIE_SIZE 8	/* the size employed by this impl. */
#endif
/* TCP Fast Open Cookie as stored in memory */
struct tcp_fastopen_cookie {
	union {
		u8	val[TCP_FASTOPEN_COOKIE_MAX];
#if IS_ENABLED(CONFIG_IPV6)
		struct in6_addr addr;
#endif
	};
	s8	len;
	bool	exp;	/* In RFC6994 experimental option format */
};

/* This defines a selective acknowledgement block. */
struct tcp_sack_block_wire {
	__be32	start_seq;
	__be32	end_seq;
};

struct tcp_sack_block {
	u32	start_seq;
	u32	end_seq;
};

#ifdef CONFIG_MPTCP
struct tcp_out_options {
	u16	options;	/* bit field of OPTION_* */
	u8	ws;		/* window scale, 0 to disable */
	u8	num_sack_blocks;/* number of SACK blocks to include */
	u8	hash_size;	/* bytes in hash_location */
	u16	mss;		/* 0 to disable */
	__u8	*hash_location;	/* temporary pointer, overloaded */
	__u32	tsval, tsecr;	/* need to include OPTION_TS */
	struct tcp_fastopen_cookie *fastopen_cookie;	/* Fast open cookie */
	u16	mptcp_options;	/* bit field of MPTCP related OPTION_* */
	u8	dss_csum:1,	/* dss-checksum required? */
		add_addr_v4:1,
		add_addr_v6:1,
		mptcp_ver:4;

	union {
		struct {
			__u64	sender_key;	/* sender's key for mptcp */
			__u64	receiver_key;	/* receiver's key for mptcp */
		} mp_capable;

		struct {
			__u64	sender_truncated_mac;
			__u32	sender_nonce;
					/* random number of the sender */
			__u32	token;	/* token for mptcp */
			u8	low_prio:1;
		} mp_join_syns;
	};

	struct {
		__u64 trunc_mac;
		struct in_addr addr;
		u16 port;
		u8 addr_id;
	} add_addr4;

	struct {
		__u64 trunc_mac;
		struct in6_addr addr;
		u16 port;
		u8 addr_id;
	} add_addr6;

	u16	remove_addrs;	/* list of address id */
	u8	addr_id;	/* address id (mp_join or add_address) */
};

struct mptcp_cb;
struct mptcp_tcp_sock;
struct tcp_md5sig_key;
#endif /* CONFIG_MPTCP */

/*These are used to set the sack_ok field in struct tcp_options_received */
#define TCP_SACK_SEEN     (1 << 0)   /*1 = peer is SACK capable, */
#define TCP_FACK_ENABLED  (1 << 1)   /*1 = FACK is enabled locally*/
#define TCP_DSACK_SEEN    (1 << 2)   /*1 = DSACK was received from peer*/

struct tcp_options_received {
/*	PAWS/RTTM data	*/
	long	ts_recent_stamp;/* Time we stored ts_recent (for aging) */
	u32	ts_recent;	/* Time stamp to echo next		*/
	u32	rcv_tsval;	/* Time stamp value             	*/
	u32	rcv_tsecr;	/* Time stamp echo reply        	*/
	u16 	saw_tstamp : 1,	/* Saw TIMESTAMP on last packet		*/
		tstamp_ok : 1,	/* TIMESTAMP seen on SYN packet		*/
		dsack : 1,	/* D-SACK is scheduled			*/
		wscale_ok : 1,	/* Wscale seen on SYN packet		*/
		sack_ok : 4,	/* SACK seen on SYN packet		*/
		snd_wscale : 4,	/* Window scaling received from sender	*/
		rcv_wscale : 4;	/* Window scaling to send to receiver	*/
	u8	num_sacks;	/* Number of SACK blocks		*/
	u16	user_mss;	/* mss requested by user in ioctl	*/
	u16	mss_clamp;	/* Maximal mss, negotiated at connection setup */
};

static inline void tcp_clear_options(struct tcp_options_received *rx_opt)
{
	rx_opt->tstamp_ok = rx_opt->sack_ok = 0;
	rx_opt->wscale_ok = rx_opt->snd_wscale = 0;
}

/* This is the max number of SACKS that we'll generate and process. It's safe
 * to increase this, although since:
 *   size = TCPOLEN_SACK_BASE_ALIGNED (4) + n * TCPOLEN_SACK_PERBLOCK (8)
 * only four options will fit in a standard TCP header */
#define TCP_NUM_SACKS 4

struct tcp_request_sock_ops;

struct tcp_request_sock {
	struct inet_request_sock 	req;
	const struct tcp_request_sock_ops *af_specific;
	u64				snt_synack; /* first SYNACK sent time */
	bool				tfo_listener;
	u32				txhash;
	u32				rcv_isn;
	u32				snt_isn;
	u32				ts_off;
	u32				last_oow_ack_time; /* last SYNACK */
	u32				rcv_nxt; /* the ack # by SYNACK. For
						  * FastOpen it's the seq#
						  * after data-in-SYN.
						  */
};

static inline struct tcp_request_sock *tcp_rsk(const struct request_sock *req)
{
	return (struct tcp_request_sock *)req;
}

#ifdef CONFIG_MPTCP
struct mptcp_subflow_buf_info {
	char if_name[IFNAMSIZ];
	u32 snd_buf;
	u32 rcv_buf;
	u8 low_prio;
};

#ifndef MAX_SUPPORT_SUBFLOW_NUM
#define MAX_SUPPORT_SUBFLOW_NUM 3
#endif
#endif

struct tcp_sock {
	/* inet_connection_sock has to be the first member of tcp_sock */
	struct inet_connection_sock	inet_conn;
	u16	tcp_header_len;	/* Bytes of tcp header to send		*/
	u16	gso_segs;	/* Max number of segs per GSO packet	*/

/*
 *	Header prediction flags
 *	0x5?10 << 16 + snd_wnd in net byte order
 */
	__be32	pred_flags;

/*
 *	RFC793 variables by their proper names. This means you can
 *	read the code and the spec side by side (and laugh ...)
 *	See RFC793 and RFC1122. The RFC writes these in capitals.
 */
	u64	bytes_received;	/* RFC4898 tcpEStatsAppHCThruOctetsReceived
				 * sum(delta(rcv_nxt)), or how many bytes
				 * were acked.
				 */
	u32	segs_in;	/* RFC4898 tcpEStatsPerfSegsIn
				 * total number of segments in.
				 */
	u32	data_segs_in;	/* RFC4898 tcpEStatsPerfDataSegsIn
				 * total number of data segments in.
				 */
 	u32	rcv_nxt;	/* What we want to receive next 	*/
	u32	copied_seq;	/* Head of yet unread data		*/
	u32	rcv_wup;	/* rcv_nxt on last window update sent	*/
 	u32	snd_nxt;	/* Next sequence we send		*/
	u32	segs_out;	/* RFC4898 tcpEStatsPerfSegsOut
				 * The total number of segments sent.
				 */
	u32	data_segs_out;	/* RFC4898 tcpEStatsPerfDataSegsOut
				 * total number of data segments sent.
				 */
	u64	bytes_acked;	/* RFC4898 tcpEStatsAppHCThruOctetsAcked
				 * sum(delta(snd_una)), or how many bytes
				 * were acked.
				 */
 	u32	snd_una;	/* First byte we want an ack for	*/
 	u32	snd_sml;	/* Last byte of the most recently transmitted small packet */
	u32	rcv_tstamp;	/* timestamp of last received ACK (for keepalives) */
	u32	lsndtime;	/* timestamp of last sent data packet (for restart window) */
	u32	last_oow_ack_time;  /* timestamp of last out-of-window ACK */
	u32	compressed_ack_rcv_nxt;

	u32	tsoffset;	/* timestamp offset */

	struct list_head tsq_node; /* anchor in tsq_tasklet.head list */
	struct list_head tcs_node; /* anchor in tcs_tasklet.head list */

	u32	snd_wl1;	/* Sequence for window update		*/
	u32	snd_wnd;	/* The window we expect to receive	*/
	u32	max_window;	/* Maximal window ever seen from peer	*/
	u32	mss_cache;	/* Cached effective mss, not including SACKS */

	u32	window_clamp;	/* Maximal window to advertise		*/
	u32	rcv_ssthresh;	/* Current window clamp			*/

	/* Information of the most recently (s)acked skb */
	struct tcp_rack {
		u64 mstamp; /* (Re)sent time of the skb */
		u32 rtt_us;  /* Associated RTT */
		u32 end_seq; /* Ending TCP sequence of the skb */
		u8 advanced; /* mstamp advanced since last lost marking */
		u8 reord;    /* reordering detected */
	} rack;
	u16	advmss;		/* Advertised MSS			*/
	u8	compressed_ack;
	u32	chrono_start;	/* Start time in jiffies of a TCP chrono */
	u32	chrono_stat[3];	/* Time in jiffies for chrono_stat stats */
	u8	chrono_type:2,	/* current chronograph type */
		rate_app_limited:1,  /* rate_{delivered,interval_us} limited? */
		fastopen_connect:1, /* FASTOPEN_CONNECT sockopt */
		is_sack_reneg:1,    /* in recovery from loss with SACK reneg? */
		unused:3;
	u8	nonagle     : 4,/* Disable Nagle algorithm?             */
		thin_lto    : 1,/* Use linear timeouts for thin streams */
		unused1	    : 1,
		repair      : 1,
		frto        : 1;/* F-RTO (RFC5682) activated in CA_Loss */
	u8	repair_queue;
	u8	syn_data:1,	/* SYN includes data */
		syn_fastopen:1,	/* SYN includes Fast Open option */
		syn_fastopen_exp:1,/* SYN includes Fast Open exp. option */
		syn_fastopen_ch:1, /* Active TFO re-enabling probe */
		syn_data_acked:1,/* data in SYN is acked by SYN-ACK */
		save_syn:1,	/* Save headers of SYN packet */
		is_cwnd_limited:1;/* forward progress limited by snd_cwnd? */
	u32	tlp_high_seq;	/* snd_nxt at the time of TLP retransmit. */

/* RTT measurement */
	u64	tcp_mstamp;	/* most recent packet received/sent */
#ifdef CONFIG_HW_NETWORK_QOE
	u32	ofo_tstamp; /* timestamp of ofo queue become not null */
	u32	rcv_nxt_ofo; /* equals rcv_nxt+rcv_wnd */
	u32	rtt_update_tstamp; /* timestamp of rtt update */
	u32	prior_srtt_us; /* srtt << 3 in usecs at last rtt update */
	u32	monitored;
#endif
	u32	srtt_us;	/* smoothed round trip time << 3 in usecs */
	u32	mdev_us;	/* medium deviation			*/
	u32	mdev_max_us;	/* maximal mdev for the last rtt period	*/
	u32	rttvar_us;	/* smoothed mdev_max			*/
	u32	rtt_seq;	/* sequence number to update rttvar	*/
	struct  minmax rtt_min;

	u32	packets_out;	/* Packets which are "in flight"	*/
	u32	retrans_out;	/* Retransmitted packets out		*/
	u32	max_packets_out;  /* max packets_out in last window */
	u32	max_packets_seq;  /* right edge of max_packets_out flight */

	u16	urg_data;	/* Saved octet of OOB data and control flags */
	u8	ecn_flags;	/* ECN status bits.			*/
	u8	keepalive_probes; /* num of allowed keep alive probes	*/
	u32	reordering;	/* Packet reordering metric.		*/
	u32	snd_up;		/* Urgent pointer		*/

/*
 *      Options received (usually on last packet, some only on SYN packets).
 */
	struct tcp_options_received rx_opt;

/*
 *	Slow start and congestion control (see also Nagle, and Karn & Partridge)
 */
 	u32	snd_ssthresh;	/* Slow start size threshold		*/
 	u32	snd_cwnd;	/* Sending congestion window		*/
	u32	snd_cwnd_cnt;	/* Linear increase counter		*/
	u32	snd_cwnd_clamp; /* Do not allow snd_cwnd to grow above this */
	u32	snd_cwnd_used;
	u32	snd_cwnd_stamp;
	u32	prior_cwnd;	/* cwnd right before starting loss recovery */
	u32	prr_delivered;	/* Number of newly delivered packets to
				 * receiver in Recovery. */
	u32	prr_out;	/* Total number of pkts sent during Recovery. */
	u32	delivered;	/* Total data packets delivered incl. rexmits */
	u32	lost;		/* Total data packets lost incl. rexmits */
	u32	app_limited;	/* limited until "delivered" reaches this val */
	u64	first_tx_mstamp;  /* start of window send phase */
	u64	delivered_mstamp; /* time we reached "delivered" */
	u32	rate_delivered;    /* saved rate sample: packets delivered */
	u32	rate_interval_us;  /* saved rate sample: time elapsed */

 	u32	rcv_wnd;	/* Current receiver window		*/
	u32	write_seq;	/* Tail(+1) of data held in tcp send buffer */
	u32	notsent_lowat;	/* TCP_NOTSENT_LOWAT */
	u32	pushed_seq;	/* Last pushed seq, required to talk to windows */
	u32	lost_out;	/* Lost packets			*/
	u32	sacked_out;	/* SACK'd packets			*/
	u32	fackets_out;	/* FACK'd packets			*/

	struct hrtimer	pacing_timer;
	struct hrtimer	compressed_ack_timer;

#ifdef CONFIG_TCP_NODELAY
	u16	nodelay_size;	/* packet size by delayed */
	u8	nodelay;	/* Auto tcp no delay is disabled */
	u8	pingpong;	/* send msg count without response */
#endif

	/* from STCP, retrans queue hinting */
	struct sk_buff* lost_skb_hint;
	struct sk_buff *retransmit_skb_hint;

	/* OOO segments go in this rbtree. Socket lock must be held. */
	struct rb_root	out_of_order_queue;
	struct sk_buff	*ooo_last_skb; /* cache rb_last(out_of_order_queue) */

	/* SACKs data, these 2 need to be together (see tcp_options_write) */
	struct tcp_sack_block duplicate_sack[1]; /* D-SACK block */
	struct tcp_sack_block selective_acks[4]; /* The SACKS themselves*/

	struct tcp_sack_block recv_sack_cache[4];

	struct sk_buff *highest_sack;   /* skb just after the highest
					 * skb with SACKed bit set
					 * (validity guaranteed only if
					 * sacked_out > 0)
					 */

	int     lost_cnt_hint;

	u32	prior_ssthresh; /* ssthresh saved at recovery start	*/
	u32	high_seq;	/* snd_nxt at onset of congestion	*/

	u32	retrans_stamp;	/* Timestamp of the last retransmit,
				 * also used in SYN-SENT to remember stamp of
				 * the first SYN. */
	u32	undo_marker;	/* snd_una upon a new recovery episode. */
	int	undo_retrans;	/* number of undoable retransmissions. */
	u32	total_retrans;	/* Total retransmits for entire connection */

	u32	urg_seq;	/* Seq of received urgent pointer */
	unsigned int		keepalive_time;	  /* time before keep alive takes place */
	unsigned int		keepalive_intvl;  /* time interval between keep alive probes */

	int			linger2;

/* Receiver side RTT estimation */
	struct {
		u32	rtt_us;
		u32	seq;
		u64	time;
	} rcv_rtt_est;

/* Receiver queue space */
	struct {
		u32	space;
		u32	seq;
		u64	time;
	} rcvq_space;

#ifdef CONFIG_HW_NETQOS_SCHED
	struct {
		u32	segs;
		u32	min_rtt;
		u32	bw;
		u32	rcv_wnd;
	} rcv_rate;
#endif

/* TCP-specific MTU probe information. */
	struct {
		u32		  probe_seq_start;
		u32		  probe_seq_end;
	} mtu_probe;
	u32	mtu_info; /* We received an ICMP_FRAG_NEEDED / ICMPV6_PKT_TOOBIG
			   * while socket was owned by user.
			   */

#ifdef CONFIG_TCP_MD5SIG
/* TCP AF-Specific parts; only used by MD5 Signature support so far */
	const struct tcp_sock_af_ops	*af_specific;

/* TCP MD5 Signature Option information */
	struct tcp_md5sig_info	__rcu *md5sig_info;
#endif

/* TCP fastopen related information */
	struct tcp_fastopen_request *fastopen_req;
	/* fastopen_rsk points to request_sock that resulted in this big
	 * socket. Used to retransmit SYNACKs etc.
	 */
	struct request_sock *fastopen_rsk;
	u32	*saved_syn;

#ifdef CONFIG_TCP_ARGO
/* TCP ARGO */
	struct tcp_argo *argo;
#endif /* CONFIG_TCP_ARGO */

#ifdef CONFIG_CHR_NETLINK_MODULE
	u8 first_data_flag;
	u8 data_net_flag;
#endif
#ifdef CONFIG_MPTCP
	/* MPTCP/TCP-specific callbacks */
	const struct tcp_sock_ops	*ops;

	struct mptcp_cb		*mpcb;
	struct sock		*meta_sk;
	/* We keep these flags even if CONFIG_MPTCP is not checked, because
	 * it allows checking MPTCP capability just by checking the mpc flag,
	 * rather than adding ifdefs everywhere.
	 */
	u32     mpc:1,          /* Other end is multipath capable */
		inside_tk_table:1, /* Is the tcp_sock inside the token-table? */
		send_mp_fclose:1,
		request_mptcp:1, /* Did we send out an MP_CAPABLE?
				  * (this speeds up mptcp_doit() in tcp_recvmsg)
				  */
		pf:1, /* Potentially Failed state: when this flag is set, we
		       * stop using the subflow
		       */
		mp_killed:1, /* Killed with a tcp_done in mptcp? */
		is_master_sk:1,
		close_it:1,	/* Must close socket in mptcp_data_ready? */
		closing:1,
		mptcp_ver:4,
		mptcp_sched_setsockopt:1,
		mptcp_pm_setsockopt:1,
		record_master_info:1,
		tcp_disconnect:1,
		mptcp_cap_flag:3,
		user_switch:1, /* indicate whether can create subflows */
		mptcp_sched_prim_intf:1,
		syn_fallback:1;
	struct mptcp_tcp_sock *mptcp;
#define MPTCP_SCHED_NAME_MAX 16
#define MPTCP_PM_NAME_MAX 16
#define MPTCP_SCHED_PARAMS 4
	struct hlist_nulls_node tk_table;
	u32		mptcp_loc_token;
	u64		mptcp_loc_key;
	char		mptcp_sched_name[MPTCP_SCHED_NAME_MAX];
	char		mptcp_pm_name[MPTCP_PM_NAME_MAX];
	char		prim_iface[IFNAMSIZ];
	char mptcp_sched_params[MPTCP_SCHED_PARAMS] __aligned(8);
	struct mptcp_subflow_buf_info hw_subflows[MAX_SUPPORT_SUBFLOW_NUM];
	struct sockaddr server_addr;
#endif /* CONFIG_MPTCP */
};

enum tsq_enum {
	TSQ_THROTTLED,
	TSQ_QUEUED,
	TCP_TSQ_DEFERRED,	   /* tcp_tasklet_func() found socket was owned */
	TCP_WRITE_TIMER_DEFERRED,  /* tcp_write_timer() found socket was owned */
	TCP_DELACK_TIMER_DEFERRED, /* tcp_delack_timer() found socket was owned */
	TCP_MTU_REDUCED_DEFERRED,  /* tcp_v{4|6}_err() could not call
				    * tcp_v{4|6}_mtu_reduced()
				    */
	TCP_COMP_SACK_TIMER_DEFERRED, /* tcp_tcs_tasklet_func() found socket was owned */
#ifdef CONFIG_MPTCP
	/* MPTCP deferred creation of new subflows */
	MPTCP_PATH_MANAGER_DEFERRED,
	MPTCP_SUB_DEFERRED, /* A subflow got deferred - process them */
	/* A user switch event deferred - process them */
	MPTCP_USER_SWTCH_DEFERRED,
#endif /* CONFIG_MPTCP */
};

enum tsq_flags {
	TSQF_THROTTLED			= (1UL << TSQ_THROTTLED),
	TSQF_QUEUED			= (1UL << TSQ_QUEUED),
	TCPF_TSQ_DEFERRED		= (1UL << TCP_TSQ_DEFERRED),
	TCPF_WRITE_TIMER_DEFERRED	= (1UL << TCP_WRITE_TIMER_DEFERRED),
	TCPF_DELACK_TIMER_DEFERRED	= (1UL << TCP_DELACK_TIMER_DEFERRED),
	TCPF_MTU_REDUCED_DEFERRED	= (1UL << TCP_MTU_REDUCED_DEFERRED),
	TCPF_COMP_SACK_TIMER_DEFERRED	= (1UL << TCP_COMP_SACK_TIMER_DEFERRED),
#ifdef CONFIG_MPTCP
	TCPF_PATH_MANAGER_DEFERRED	= (1UL << MPTCP_PATH_MANAGER_DEFERRED),
	TCPF_SUB_DEFERRED		= (1UL << MPTCP_SUB_DEFERRED),
	TCPF_USER_SWTCH_DEFERRED	= (1UL << MPTCP_USER_SWTCH_DEFERRED),
#endif /* CONFIG_MPTCP */
};

static inline struct tcp_sock *tcp_sk(const struct sock *sk)
{
	return (struct tcp_sock *)sk;
}

struct tcp_timewait_sock {
	struct inet_timewait_sock tw_sk;
#define tw_rcv_nxt tw_sk.__tw_common.skc_tw_rcv_nxt
#define tw_snd_nxt tw_sk.__tw_common.skc_tw_snd_nxt
	u32			  tw_rcv_wnd;
	u32			  tw_ts_offset;
	u32			  tw_ts_recent;

	/* The time we sent the last out-of-window ACK: */
	u32			  tw_last_oow_ack_time;

	long			  tw_ts_recent_stamp;
#ifdef CONFIG_TCP_MD5SIG
	struct tcp_md5sig_key	  *tw_md5_key;
#endif
#ifdef CONFIG_MPTCP
	struct mptcp_tw		  *mptcp_tw;
#endif /* CONFIG_MPTCP */
};

static inline struct tcp_timewait_sock *tcp_twsk(const struct sock *sk)
{
	return (struct tcp_timewait_sock *)sk;
}

static inline bool tcp_passive_fastopen(const struct sock *sk)
{
	return (sk->sk_state == TCP_SYN_RECV &&
		tcp_sk(sk)->fastopen_rsk != NULL);
}

static inline void fastopen_queue_tune(struct sock *sk, int backlog)
{
	struct request_sock_queue *queue = &inet_csk(sk)->icsk_accept_queue;
	int somaxconn = READ_ONCE(sock_net(sk)->core.sysctl_somaxconn);

	queue->fastopenq.max_qlen = min_t(unsigned int, backlog, somaxconn);
}

static inline void tcp_move_syn(struct tcp_sock *tp,
				struct request_sock *req)
{
	tp->saved_syn = req->saved_syn;
	req->saved_syn = NULL;
}

static inline void tcp_saved_syn_free(struct tcp_sock *tp)
{
	kfree(tp->saved_syn);
	tp->saved_syn = NULL;
}

struct sk_buff *tcp_get_timestamping_opt_stats(const struct sock *sk);

static inline u16 tcp_mss_clamp(const struct tcp_sock *tp, u16 mss)
{
	/* We use READ_ONCE() here because socket might not be locked.
	 * This happens for listeners.
	 */
	u16 user_mss = READ_ONCE(tp->rx_opt.user_mss);

	return (user_mss && user_mss < mss) ? user_mss : mss;
}

int tcp_skb_shift(struct sk_buff *to, struct sk_buff *from, int pcount,
		  int shiftlen);

#endif	/* _LINUX_TCP_H */
