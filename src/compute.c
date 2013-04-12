/* Copyright (c) 2012, Heng.Wang. All rights reserved.

 This program is aimed to monitor the cgroup like iostat for each group,
 if you have more effective processing methods or some ideas to solve
 the problem, thanks for your sharing with the developers. It's pleasure
 for you to contact me.

 @Author: Heng.Wang
 @Date  : 11/07/2012
 @Email : heng.wang@qunar.com
 wangheng.king@gmail.com
 king_wangheng@163.com
 @Github: https://github.com/HengWang/
 @Blog  : http://hengwang.blog.chinaunix.net
 */

#include <cgsysstat.h>
/*
 * Calculate the differ of two unsigned long long values.
 *
 * IN:
 * @value1:            The unsigned long long value.
 * @value2             The unsigned long long value.
 *
 * RETURN
 * The differ of the two unsigned long long values.
 */
unsigned long long ll_value(unsigned long long value1,
		unsigned long long value2) {

	if ((value2 < value1) && (value1 <= 0xffffffff))
		return ((value2 - value1) & 0xffffffff);
	else
		return (value2 - value1);
}

/*
 * Get the interval of the cpuset statistics information.
 *
 * IN:
 * @cur:            The current cpuset statistics information.
 * @old:            The old cpuset statistics information.
 *
 * RETURN:
 * The interval value.
 */
unsigned long long get_interval(cg_cpuset_stats* cur, cg_cpuset_stats* old) {

	unsigned long long itv = 1;
	if (old)
		itv = (cur->stats->cpu_user - old->stats->cpu_user)
				+ (cur->stats->cpu_sys - old->stats->cpu_sys)
				+ (cur->stats->cpu_steal - old->stats->cpu_steal)
				+ (cur->stats->cpu_softirq - old->stats->cpu_softirq)
				+ (cur->stats->cpu_nice - old->stats->cpu_nice)
				+ (cur->stats->cpu_iowait - old->stats->cpu_iowait)
				+ (cur->stats->cpu_idle - old->stats->cpu_idle)
				+ (cur->stats->cpu_hardirq - old->stats->cpu_hardirq);
	else
		itv = (cur->stats->cpu_user - 0) + (cur->stats->cpu_sys - 0)
				+ (cur->stats->cpu_steal - 0) + (cur->stats->cpu_softirq - 0)
				+ (cur->stats->cpu_nice - 0) + (cur->stats->cpu_iowait - 0)
				+ (cur->stats->cpu_idle - 0) + (cur->stats->cpu_hardirq - 0);
	if (!itv)
		itv = 1;
	return itv;
}

/*
 * Get the total interval of the cpuset statistics information lists.
 *
 * IN:
 * @cur:               The current cpuset statistics information.
 * @old:               The cpuset statistics information before interval.
 *
 * RETURN:
 * The total interval.
 */
unsigned long long get_total_interval(cg_cpuset_stats_lists* cur,
		cg_cpuset_stats_lists* old) {

	unsigned long long itv = 0;
	cg_cpuset_stats* ptr_cpuset_stats = NULL, *old_cpuset_stats = NULL;
	TAILQ_FOREACH(ptr_cpuset_stats,cur,next)
	{
		if (!get_cpuset_stats(old, &old_cpuset_stats, ptr_cpuset_stats->id)) {
			itv += (ptr_cpuset_stats->stats->cpu_user
					- old_cpuset_stats->stats->cpu_user)
					+ (ptr_cpuset_stats->stats->cpu_sys
							- old_cpuset_stats->stats->cpu_sys)
					+ (ptr_cpuset_stats->stats->cpu_steal
							- old_cpuset_stats->stats->cpu_steal)
					+ (ptr_cpuset_stats->stats->cpu_softirq
							- old_cpuset_stats->stats->cpu_softirq)
					+ (ptr_cpuset_stats->stats->cpu_nice
							- old_cpuset_stats->stats->cpu_nice)
					+ (ptr_cpuset_stats->stats->cpu_iowait
							- old_cpuset_stats->stats->cpu_iowait)
					+ (ptr_cpuset_stats->stats->cpu_idle
							- old_cpuset_stats->stats->cpu_idle)
					+ (ptr_cpuset_stats->stats->cpu_hardirq
							- old_cpuset_stats->stats->cpu_hardirq);
		} else
			itv += (ptr_cpuset_stats->stats->cpu_user - 0)
					+ (ptr_cpuset_stats->stats->cpu_sys - 0)
					+ (ptr_cpuset_stats->stats->cpu_steal - 0)
					+ (ptr_cpuset_stats->stats->cpu_softirq - 0)
					+ (ptr_cpuset_stats->stats->cpu_nice - 0)
					+ (ptr_cpuset_stats->stats->cpu_iowait - 0)
					+ (ptr_cpuset_stats->stats->cpu_idle - 0)
					+ (ptr_cpuset_stats->stats->cpu_hardirq - 0);

	}

	if (!itv)
		itv = 1;
	return itv;
}

/*
 * Compute the average cpuset statistics information.
 *
 * IN:
 * @cpuset_stats_lists:      The array of the cpuset statistics information lists.
 * @cur:                     The current index of the array.
 *
 * OUT:
 * @avg_cpuset_stats:        The average cpuset statistics information.
 */
void compute_avg_cpuset_stats(cg_cpuset_stats_lists** cpuset_stats_lists,
		struct cpuset_stats* avg_cpuset_stats, int cur) {

	cg_cpuset_stats* ptr_cpuset_stats = NULL;
	cg_cpuset_stats* old_cpuset_stats = NULL;
	TAILQ_FOREACH(ptr_cpuset_stats,cpuset_stats_lists[cur],next)
	{
		if (!get_cpuset_stats(cpuset_stats_lists[!cur], &old_cpuset_stats,
				ptr_cpuset_stats->id)) {
			avg_cpuset_stats->cpu_user += ll_value(
					old_cpuset_stats->stats->cpu_user,
					ptr_cpuset_stats->stats->cpu_user);
			avg_cpuset_stats->cpu_nice += ll_value(
					old_cpuset_stats->stats->cpu_nice,
					ptr_cpuset_stats->stats->cpu_nice);
			avg_cpuset_stats->cpu_sys += ll_value(
					old_cpuset_stats->stats->cpu_sys,
					ptr_cpuset_stats->stats->cpu_sys);
			avg_cpuset_stats->cpu_softirq += ll_value(
					old_cpuset_stats->stats->cpu_softirq,
					ptr_cpuset_stats->stats->cpu_softirq);
			avg_cpuset_stats->cpu_hardirq += ll_value(
					old_cpuset_stats->stats->cpu_hardirq,
					ptr_cpuset_stats->stats->cpu_hardirq);
			avg_cpuset_stats->cpu_iowait += ll_value(
					old_cpuset_stats->stats->cpu_iowait,
					ptr_cpuset_stats->stats->cpu_iowait);
			avg_cpuset_stats->cpu_steal += ll_value(
					old_cpuset_stats->stats->cpu_steal,
					ptr_cpuset_stats->stats->cpu_steal);
			avg_cpuset_stats->cpu_idle +=
					(ptr_cpuset_stats->stats->cpu_idle
							< old_cpuset_stats->stats->cpu_idle) ?
							0 :
							ll_value(old_cpuset_stats->stats->cpu_idle,
									ptr_cpuset_stats->stats->cpu_idle);
			avg_cpuset_stats->cpu_guest += ll_value(
					old_cpuset_stats->stats->cpu_guest,
					ptr_cpuset_stats->stats->cpu_guest);
		} else {
			avg_cpuset_stats->cpu_user += ptr_cpuset_stats->stats->cpu_user;
			avg_cpuset_stats->cpu_nice += ptr_cpuset_stats->stats->cpu_nice;
			avg_cpuset_stats->cpu_sys += ptr_cpuset_stats->stats->cpu_sys;
			avg_cpuset_stats->cpu_softirq +=
					ptr_cpuset_stats->stats->cpu_softirq;
			avg_cpuset_stats->cpu_hardirq +=
					ptr_cpuset_stats->stats->cpu_hardirq;
			avg_cpuset_stats->cpu_iowait += ptr_cpuset_stats->stats->cpu_iowait;
			avg_cpuset_stats->cpu_steal += ptr_cpuset_stats->stats->cpu_steal;
			avg_cpuset_stats->cpu_idle += ptr_cpuset_stats->stats->cpu_idle;
			avg_cpuset_stats->cpu_guest += ptr_cpuset_stats->stats->cpu_guest;
		}
	}
}

/*
 * Compute the total io statistics information.
 *
 * IN:
 * @io_stats_lists:      The array of the io statistics information lists.
 * @cur:                 The current index of the array.
 *
 * OUT:
 * @total_io_stats:      The total io statistics information.
 */
void compute_total_io_stats(cg_proc_stats_lists** proc_stats_lists,
		struct io_stats* total_io_stats, int cur) {

	cg_proc_stats* ptr_io_stats = NULL;
	cg_proc_stats* old_io_stats = NULL;

	TAILQ_FOREACH(ptr_io_stats, proc_stats_lists[cur],next)
	{
		if (!get_proc_stats(proc_stats_lists[!cur], &old_io_stats,
				ptr_io_stats->id)) {
			total_io_stats->io_syscr += ll_value(
					old_io_stats->io_stats->io_syscr,
					ptr_io_stats->io_stats->io_syscr);
			total_io_stats->io_syscw += ll_value(
					old_io_stats->io_stats->io_syscw,
					ptr_io_stats->io_stats->io_syscw);
			total_io_stats->io_rchar += ll_value(
					old_io_stats->io_stats->io_rchar,
					ptr_io_stats->io_stats->io_rchar);
			total_io_stats->io_wchar += ll_value(
					old_io_stats->io_stats->io_wchar,
					ptr_io_stats->io_stats->io_wchar);
			total_io_stats->io_rbytes += ll_value(
					old_io_stats->io_stats->io_rbytes,
					ptr_io_stats->io_stats->io_rbytes);
			total_io_stats->io_wbytes += ll_value(
					old_io_stats->io_stats->io_wbytes,
					ptr_io_stats->io_stats->io_wbytes);
		} else {
			total_io_stats->io_syscr += ll_value(0,
					ptr_io_stats->io_stats->io_syscr);
			total_io_stats->io_syscw += ll_value(0,
					ptr_io_stats->io_stats->io_syscw);
			total_io_stats->io_rchar += ll_value(0,
					ptr_io_stats->io_stats->io_rchar);
			total_io_stats->io_wchar += ll_value(0,
					ptr_io_stats->io_stats->io_wchar);
			total_io_stats->io_rbytes += ll_value(0,
					ptr_io_stats->io_stats->io_rbytes);
			total_io_stats->io_wbytes += ll_value(0,
					ptr_io_stats->io_stats->io_wbytes);
		}
	}

}

/*
 * Compute the process state.
 *
 * IN:
 * @cg_stats_lists:            The process stats lists.
 *
 * OUT:
 * @run:                       The running processes.
 * @sleep:                     The sleep processes.
 */
void compute_procs_state(cg_proc_stats_lists* cg_stats_lists, unsigned int *run,
		unsigned int *sleep) {

	cg_proc_stats* cg_stats = NULL;
	run = 0;
	sleep = 0;
	TAILQ_FOREACH(cg_stats,cg_stats_lists,next)
	{
		if (cg_stats && cg_stats->proc_stat
				&& cg_stats->proc_stat->state == 'R')
			(*run)++;
		else if (cg_stats && cg_stats->proc_stat
				&& cg_stats->proc_stat->state == 'S')
			(*sleep)++;
	}
}

