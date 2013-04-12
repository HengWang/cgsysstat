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

#ifndef __STRUCTURES_H
#define __STRUCTURES_H

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

#ifndef CGROUP_MEMORY_STATS
#define CGROUP_MEMORY_STATS
#endif

#ifndef CGROUP_BLKIO_STATS
#define CGROUP_BLKIO_STATS
#endif

#ifndef CGROUP_VMSTAT
#define CGROUP_VMSTAT
#endif

#ifndef CGROUP_PROC_STAT
#define CGROUP_PROC_STAT
#endif

struct io_stats {
	unsigned long long io_rchar __attribute__ ((aligned (16)));
	unsigned long long io_wchar __attribute__ ((aligned (16)));
	unsigned long long io_syscr __attribute__ ((aligned (16)));
	unsigned long long io_syscw __attribute__ ((aligned (16)));
	unsigned long long io_rbytes __attribute__ ((aligned (16)));
	unsigned long long io_wbytes __attribute__ ((aligned (16)));
	unsigned long long io_cwbytes __attribute__ ((aligned (16)));
};

#define IO_STATS_SIZE	(sizeof(struct io_stats))

/*
 * Structure for CPU statistics.
 * In activity buffer: First structure is for global CPU utilisation ("all").
 * Following structures are for each individual CPU (0, 1, etc.)
 */
struct cpuset_stats {
	unsigned long long cpu_user __attribute__ ((aligned (16)));
	unsigned long long cpu_nice __attribute__ ((aligned (16)));
	unsigned long long cpu_sys __attribute__ ((aligned (16)));
	unsigned long long cpu_idle __attribute__ ((aligned (16)));
	unsigned long long cpu_iowait __attribute__ ((aligned (16)));
	unsigned long long cpu_steal __attribute__ ((aligned (16)));
	unsigned long long cpu_hardirq __attribute__ ((aligned (16)));
	unsigned long long cpu_softirq __attribute__ ((aligned (16)));
	unsigned long long cpu_guest __attribute__ ((aligned (16)));
};

#define CPUSET_STATS_SIZE	(sizeof(struct cpuset_stats))

struct proc_stat {
	int pid; /** The process id. **/
	char exName[_POSIX_PATH_MAX]; /** The filename of the executable **/
	char state; /** 1 **//** R is running, S is sleeping,
	 D is sleeping in an uninterruptible wait,
	 Z is zombie, T is traced or stopped **/
	unsigned euid, /** effective user id **/
	egid; /** effective group id */
	int ppid; /** The pid of the parent. **/
	int pgrp; /** The pgrp of the process. **/
	int session; /** The session id of the process. **/
	int tty; /** The tty the process uses **/
	int tpgid; /** (too long) **/
	unsigned int flags; /** The flags of the process. **/
	unsigned int minflt; /** The number of minor faults **/
	unsigned int cminflt; /** The number of minor faults with childs **/
	unsigned int majflt; /** The number of major faults **/
	unsigned int cmajflt; /** The number of major faults with childs **/
	int utime; /** user mode jiffies **/
	int stime; /** kernel mode jiffies **/
	int cutime; /** user mode jiffies with childs **/
	int cstime; /** kernel mode jiffies with childs **/
	int counter; /** process's next timeslice **/
	int priority; /** the standard nice value, plus fifteen **/
	unsigned int timeout; /** The time in jiffies of the next timeout **/
	unsigned int itrealvalue; /** The time before the next SIGALRM is sent to the process **/
	int starttime; /** 20 **//** Time the process started after system boot **/
	unsigned int vsize; /** Virtual memory size **/
	unsigned int rss; /** Resident Set Size **/
	unsigned int rlim; /** Current limit in bytes on the rss **/
	unsigned int startcode; /** The address above which program text can run **/
	unsigned int endcode; /** The address below which program text can run **/
	unsigned int startstack; /** The address of the start of the stack **/
	unsigned int kstkesp; /** The current value of ESP **/
	unsigned int kstkeip; /** The current value of EIP **/
	int signal; /** The bitmap of pending signals **/
	int blocked; /** 30 **//** The bitmap of blocked signals **/
	int sigignore; /** The bitmap of ignored signals **/
	int sigcatch; /** The bitmap of catched signals **/
	unsigned int wchan; /** 33 **//** (too long) **/
	int sched, /** scheduler **/
	sched_priority; /** scheduler priority **/
};

#define PROC_STAT_SIZE (sizeof(struct proc_stat))

#ifdef CGROUP_VMSTAT
struct proc_statm {
	unsigned long long size __attribute__ ((aligned (16)));
	unsigned long long resident __attribute__ ((aligned (16)));
	unsigned long long shared __attribute__ ((aligned (16)));
	unsigned long long trs __attribute__ ((aligned (16)));
	unsigned long long lrs __attribute__ ((aligned (16)));
	unsigned long long drs __attribute__ ((aligned (16)));
	unsigned long long dt __attribute__ ((aligned (16)));
};
#define PROC_STATM_SIZE (sizeof(struct proc_statm))
#endif

#ifdef CGROUP_MEMORY_STATS
struct mem_stats {
	unsigned long long cache __attribute__ ((aligned (16)));
	unsigned long long rss __attribute__ ((aligned (16)));
	unsigned long long mapped_file __attribute__ ((aligned (16)));
	unsigned long long pgpgin __attribute__ ((aligned (16)));
	unsigned long long pgpgout __attribute__ ((aligned (16)));
	unsigned long long swap __attribute__ ((aligned (16)));
	unsigned long long inactive_anon __attribute__ ((aligned (16)));
	unsigned long long active_anon __attribute__ ((aligned (16)));
	unsigned long long inactive_file __attribute__ ((aligned (16)));
	unsigned long long active_file __attribute__ ((aligned (16)));
	unsigned long long unevictable __attribute__ ((aligned (16)));
	unsigned long long hierarchical_memory_limit __attribute__ ((aligned (16)));
	unsigned long long hierarchical_memsw_limit __attribute__ ((aligned (16)));
	unsigned long long total_cache __attribute__ ((aligned (16)));
	unsigned long long total_rss __attribute__ ((aligned (16)));
	unsigned long long total_mapped_file __attribute__ ((aligned (16)));
	unsigned long long total_pgpgin __attribute__ ((aligned (16)));
	unsigned long long total_pgpgout __attribute__ ((aligned (16)));
	unsigned long long total_swap __attribute__ ((aligned (16)));
	unsigned long long total_inactive_anon __attribute__ ((aligned (16)));
	unsigned long long total_active_anon __attribute__ ((aligned (16)));
	unsigned long long total_inactive_file __attribute__ ((aligned (16)));
	unsigned long long total_active_file __attribute__ ((aligned (16)));
	unsigned long long total_unevictable __attribute__ ((aligned (16)));
};
#define MEM_STATS_SIZE (sizeof(struct mem_stats))
#endif

#ifdef CGROUP_BLKIO_STATS
struct device_stats {
	unsigned long long read __attribute__ ((aligned (16)));
	unsigned long long write __attribute__ ((aligned (16)));
	unsigned long long sync __attribute__ ((aligned (16)));
	unsigned long long async __attribute__ ((aligned (16)));
	unsigned long long total __attribute__ ((aligned (16)));
};
#define DEVICE_BLKIO_SIZE (sizeof(struct device_stats))
#endif

#endif

