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

#ifndef __COMPUTE_H
#define __COMPUTE_H

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
		unsigned long long value2);

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
unsigned long long get_interval(cg_cpuset_stats* cur, cg_cpuset_stats* old);

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
		cg_cpuset_stats_lists* old);

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
		struct cpuset_stats* avg_cpuset_stats, int cur);

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
		struct io_stats* total_io_stats, int cur);

#endif /* __COMPUTE_H */
