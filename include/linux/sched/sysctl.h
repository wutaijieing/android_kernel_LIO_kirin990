/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCHED_SYSCTL_H
#define _LINUX_SCHED_SYSCTL_H

#include <linux/types.h>

struct ctl_table;

#ifdef CONFIG_DETECT_HUNG_TASK
extern int	     sysctl_hung_task_check_count;
extern unsigned int  sysctl_hung_task_panic;
extern unsigned long sysctl_hung_task_timeout_secs;
extern int sysctl_hung_task_warnings;
extern int proc_dohung_task_timeout_secs(struct ctl_table *table, int write,
					 void __user *buffer,
					 size_t *lenp, loff_t *ppos);
#else
/* Avoid need for ifdefs elsewhere in the code */
enum { sysctl_hung_task_timeout_secs = 0 };
#endif

#ifdef CONFIG_BOOST_KILL
extern unsigned int sysctl_boost_killing;
#endif

#ifdef CONFIG_HW_VIP_THREAD
#include <chipset_common/hwcfs/hwcfs_sysctl.h>
#endif

#ifdef CONFIG_HW_QOS_THREAD
#include <chipset_common/hwqos/hwqos_sysctl.h>
#endif

extern unsigned int sysctl_sched_latency;
extern unsigned int sysctl_sched_min_granularity;
extern unsigned int sysctl_sched_sync_hint_enable;
extern unsigned int sysctl_sched_cstate_aware;
extern unsigned int sysctl_sched_wakeup_granularity;
extern unsigned int sysctl_sched_child_runs_first;
#ifdef CONFIG_SCHED_WALT
extern unsigned int sysctl_sched_use_walt_cpu_util;
extern unsigned int sysctl_sched_use_walt_task_util;
extern unsigned int sysctl_sched_walt_init_task_load_pct;
extern unsigned int sysctl_sched_walt_cpu_high_irqload;
#ifdef CONFIG_HISI_EAS_SCHED
extern unsigned int sysctl_sched_walt_cpu_overload_irqload;
extern unsigned int sysctl_sched_force_upmigrate_duration;
#endif
#ifdef CONFIG_SCHED_HISI_RUNNING_TASK_ROTATION
extern unsigned int sysctl_sched_walt_rotate_big_tasks;
#endif
#endif

#ifdef CONFIG_HISI_RT_CAS
extern unsigned int sysctl_sched_enable_rt_cas;
#endif
#ifdef CONFIG_HISI_RT_ACTIVE_LB
extern unsigned int sysctl_sched_enable_rt_active_lb;
#endif

#ifdef CONFIG_HISI_RENDER_RT_DEBUG
extern unsigned int sysctl_sched_enable_render_rt_trace;
#endif

enum sched_tunable_scaling {
	SCHED_TUNABLESCALING_NONE,
	SCHED_TUNABLESCALING_LOG,
	SCHED_TUNABLESCALING_LINEAR,
	SCHED_TUNABLESCALING_END,
};
extern enum sched_tunable_scaling sysctl_sched_tunable_scaling;

extern unsigned int sysctl_numa_balancing_scan_delay;
extern unsigned int sysctl_numa_balancing_scan_period_min;
extern unsigned int sysctl_numa_balancing_scan_period_max;
extern unsigned int sysctl_numa_balancing_scan_size;

#ifdef CONFIG_SCHED_DEBUG
extern __read_mostly unsigned int sysctl_sched_migration_cost;
extern __read_mostly unsigned int sysctl_sched_nr_migrate;
extern __read_mostly unsigned int sysctl_sched_time_avg;
#endif

int sched_proc_update_handler(struct ctl_table *table, int write,
		void __user *buffer, size_t *length,
		loff_t *ppos);

/*
 *  control realtime throttling:
 *
 *  /proc/sys/kernel/sched_rt_period_us
 *  /proc/sys/kernel/sched_rt_runtime_us
 */
extern unsigned int sysctl_sched_rt_period;
extern int sysctl_sched_rt_runtime;

#ifdef CONFIG_CFS_BANDWIDTH
extern unsigned int sysctl_sched_cfs_bandwidth_slice;
#endif

#ifdef CONFIG_SCHED_AUTOGROUP
extern unsigned int sysctl_sched_autogroup_enabled;
#endif

extern int sysctl_sched_rr_timeslice;
extern int sched_rr_timeslice;

extern int sched_rr_handler(struct ctl_table *table, int write,
		void __user *buffer, size_t *lenp,
		loff_t *ppos);

extern int sched_rt_handler(struct ctl_table *table, int write,
		void __user *buffer, size_t *lenp,
		loff_t *ppos);

extern int sysctl_numa_balancing(struct ctl_table *table, int write,
				 void __user *buffer, size_t *lenp,
				 loff_t *ppos);

extern int sysctl_schedstats(struct ctl_table *table, int write,
				 void __user *buffer, size_t *lenp,
				 loff_t *ppos);

#endif /* _LINUX_SCHED_SYSCTL_H */