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

#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <time.h>

#include <common/tail_queue.h>
#include <common/structures.h>

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 * The macro for max min value.
 */
#define max(A,B) ((A) > (B) ? (A):(B))
#define min(A,B) ((A) > (B) ? (B):(A))
/*
 * The structure of cpuset statistics information for cgroup.
 * @id             The cpuset id.
 * @stats          The cpuset statistics information.
 * @next           The next element.
 */
typedef struct cgroup_cpuset_stats {
	int id;
	struct cpuset_stats* stats;
	TAILQ_ENTRY(cgroup_cpuset_stats)
	next;
} cg_cpuset_stats;

/*
 * The head of cpuset statistics information for cgroup.
 * @tqh_first		First element.
 * @tqh_last		Address of last next element.
 * @count           The count of elements.
 */
TAILQ_HEAD(cgroup_cpuset_stats_lists, cgroup_cpuset_stats);
typedef struct cgroup_cpuset_stats_lists cg_cpuset_stats_lists;

/*
 * The structure of io statistics information for cgroup.
 * @id             The process id.
 * @stats          The io statistics information.
 * @next           The next element.
 */
typedef struct cgroup_proc_stats {
	int id;
	struct io_stats* io_stats;
#ifdef CGROUP_VMSTAT
	struct proc_stat* proc_stat;
	struct proc_statm* proc_statm;
#endif
	TAILQ_ENTRY(cgroup_proc_stats)
	next;
} cg_proc_stats;

/*
 * The head of io statistics information for cgroup.
 * @tqh_first		First element.
 * @tqh_last		Address of last next element.
 * @count           The count of elements.
 */
TAILQ_HEAD(cgroup_proc_stats_lists, cgroup_proc_stats);
typedef struct cgroup_proc_stats_lists cg_proc_stats_lists;

#ifdef CGROUP_MEMORY_STATS
/*
 * The struct of memory statistics information for cgroup
 */
typedef struct cgroup_mem_stats {
	struct mem_stats* stats;
	unsigned long long usage;
	unsigned long long mem_swap_usage;
	unsigned long long max_usage;
	unsigned long long mem_swap_max_usage;
	unsigned long long limit;
	unsigned long long mem_swap_limit;
	unsigned long long failcnt;
	unsigned long long mem_swap_failcnt;
	unsigned long long swappiness;
	unsigned long long use_hierarchy;
} cg_mem_stats;

#endif

#ifdef CGROUP_BLKIO_STATS
/*
 * The structure of blkio.
 * @name:   The device name.
 * @stats:  The device statistics information.
 * @next:   The next element.
 */
typedef struct cgroup_device_stats {
	int dev;
	int idx;
	struct device_stats* stats;
	TAILQ_ENTRY(cgroup_device_stats)
	next;
} cg_device_stats;

/*
 * The head of device stats for cgroup.
 * @tqh_first		First element.
 * @tqh_last		Address of last next element.
 * @count           The count of elements.
 */
TAILQ_HEAD(cgroup_device_stats_lists, cgroup_device_stats);
typedef struct cgroup_device_stats_lists cg_device_stats_lists;

typedef struct cgroup_device_value {
	int dev;
	int idx;
	unsigned long long value;
	TAILQ_ENTRY(cgroup_device_value)
	next;
} cg_device_value;

/*
 * The head of device values for cgroup.
 * @tqh_first		First element.
 * @tqh_last		Address of last next element.
 * @count           The count of elements.
 */
TAILQ_HEAD(cgroup_device_value_lists, cgroup_device_value);
typedef struct cgroup_device_value_lists cg_device_value_lists;

/*
 * The structure of blkio statistics information.
 * @io_service_bytes:             The lists of io service bytes.
 * @io_serviced:                  The lists of io serviced.
 * @io_service_time:              The lists of io service time.(ns)
 * @io_wait_time:                 The lists of io wait time.(ns)
 * @io_merged:                    The lists of io merged.
 * @io_queued:                    The lists of io queued.
 * @throttle_io_service_bytes:    The lists of throttle io service bytes.
 * @throttle_io_serviced:         The lists of throttle io serviced.
 * @weight:                       The blkio weight.
 * @weight_device:                The device weight.
 * @time:                         The lists of io time.(ms)
 * @sectors:                      The lists of sectors.
 * @throttle_read_bps_device:     The lists of read bps.
 * @throttle_read_iops_device:    The lists of read iops.
 * @throttle_write_bps_device:    The lists of write bps.
 * @throttle_write_iops_device:   The lists of write iops.
 * @stats:                        The statistics information.
 */
typedef struct cgroup_blkio_stats {
	cg_device_stats_lists* io_service_bytes;
	cg_device_stats_lists* io_serviced;
	cg_device_stats_lists* io_service_time;
	cg_device_stats_lists* io_wait_time;
	cg_device_stats_lists* io_merged;
	cg_device_stats_lists* io_queued;
	cg_device_stats_lists* throttle_io_service_bytes;
	cg_device_stats_lists* throttle_io_serviced;
	cg_device_value_lists* time;
	cg_device_value_lists* sectors;
	cg_device_value_lists* throttle_read_bps_device;
	cg_device_value_lists* throttle_read_iops_device;
	cg_device_value_lists* throttle_write_bps_device;
	cg_device_value_lists* throttle_write_iops_device;
	unsigned long long weight;
	unsigned long long weight_device;
} cg_blkio_stats;
#endif

/*
 * The structure of io statistics information for cgroup.
 * @name           The Name of the group.
 * @path           Path of the group.
 * @next           The next group.
 * @cpuset_stats[2]   The cpuset statistics informations now and
 * 				   before interval seconds.
 * @io_stats[2]    The io statistics informations now and
 * 				   before interval seconds.
 * @mem_stats[2]   The memory statistics informations now and
 * 				   before interval seconds.
 */
typedef struct cgroup_group {
	char* name;
	char* path;
	TAILQ_ENTRY(cgroup_group)
	next;
	cg_cpuset_stats_lists *cpuset_stats[2];
#ifdef CGROUP_MEMORY_STATS
	cg_mem_stats *mem_stats[2];
#endif
#ifdef CGROUP_BLKIO_STATS
	cg_blkio_stats* blkio_stats[2];
#endif
#ifdef CGROUP_PROC_STAT
	cg_proc_stats_lists* proc_stats[2];
#endif
} cg_group;

/*
 * The head of group for cgroup.
 * @tqh_first		First element.
 * @tqh_last		Address of last next element.
 * @count           The count of elements.
 */
TAILQ_HEAD(cgroup_group_lists, cgroup_group);
typedef struct cgroup_group_lists cg_group_lists;

/*
 * Initialized the head of cgroup_cpuset_stats.
 *
 * OUT:
 * @cg_stats_lists:  The structure of cgroup_cpuset_stats_lists.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_cpuset_stats_lists(cg_cpuset_stats_lists** cg_stats_lists);

/*
 * Uninitialized the head of cgroup_cpuset_stats.
 *
 * IN:
 * @cg_stats_lists:  The structure of cgroup_cpuset_stats_lists.
 */
void uninit_cpuset_stats_lists(cg_cpuset_stats_lists* cg_stats_lists);

/*
 * Initialized the structure of cgroup_cpuset_stats.
 *
 * OUT:
 * @cg_stats:         The structure of cgroup_cpuset_stats.
 *
 * IN :
 * @id:               The cpuset id.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_cpuset_stats(cg_cpuset_stats** cg_stats, const int id);

/*
 * Uninitialized the structure of cgroup_cpuset_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_cpuset_stats.
 */
void uninit_cpuset_stats(cg_cpuset_stats* cg_stats);

/*
 * Get the cpuset statistics information element of given id from the lists of
 * cpuset statistics informations.
 *
 * OUT:
 * @cg_stats:         The element of given id.
 *
 * IN :
 * @cg_stats_lists:   The lists of cpuset statistics informations.
 * @id:               The cpuset id.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int get_cpuset_stats(cg_cpuset_stats_lists* cg_stats_lists,
		cg_cpuset_stats** cg_stats, const int id);

/*
 * Check the given id whether in the list of cpuset statistics informations or not.
 *
 * IN :
 * @cg_stats_lists:   The lists of cpuset statistics informations.
 * @id:               The cpuset id.
 *
 * RETURN:
 * TRUE  :               Exists.
 * FALSE :               Not exists.
 */BOOL is_cpuset_exists(cg_cpuset_stats_lists* cg_stats_lists, const int id);

/*
 * Add the cpuset statistics information element into the lists of cpuset statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of cpuset statistics informations.
 * @cg_stats:         The element of cpuset statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int add_cpuset_stats(cg_cpuset_stats_lists* cg_stats_lists,
		cg_cpuset_stats* cg_stats);

/*
 * Remove the cpuset statistics information element from the lists of cpuset statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of cpuset statistics informations.
 * @cg_stats:         The element of cpuset statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int remove_cpuset_stats(cg_cpuset_stats_lists* cg_stats_lists,
		cg_cpuset_stats* cg_stats);

/*
 * Clear up the lists of cpuset statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of cpuset statistics informations.
 */
void clear_cpuset_stats(cg_cpuset_stats_lists* cg_stats_lists);

#ifdef CGROUP_PROC_STAT
/*
 * Initialized the head of cgroup_proc_stats.
 *
 * OUT:
 * @cg_stats_lists:  The structure of cgroup_proc_stats_lists.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_proc_stats_lists(cg_proc_stats_lists** cg_stats_lists);

/*
 * Uninitialized the head of cgroup_proc_stats.
 *
 * IN:
 * @cg_stats_lists:  The structure of cgroup_proc_stats_lists.
 */
void uninit_proc_stats_lists(cg_proc_stats_lists* cg_stats_lists);

/*
 * Initialized the structure of cgroup_proc_stats.
 *
 * OUT:
 * @cg_stats:         The structure of cgroup_proc_stats.
 *
 * IN :
 * @id:               The process id.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_proc_stats(cg_proc_stats** cg_stats, const int id);

/*
 * Uninitialized the structure of cgroup_proc_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_proc_stats.
 */
void uninit_proc_stats(cg_proc_stats* cg_stats);

/*
 * Get the pid io statistics information element of given id from the lists of
 * pid io statistics informations.
 *
 * OUT:
 * @cg_stats:         The element of given id.
 *
 * IN :
 * @cg_stats_lists:   The lists of pid io statistics informations.
 * @id:               The process id.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int get_proc_stats(cg_proc_stats_lists* cg_stats_lists,
		cg_proc_stats** cg_stats, const int id);

/*
 * Check the given process id whether in the list of pid io statistics informations or not.
 *
 * IN :
 * @cg_stats_lists:   The lists of pid io statistics informations.
 * @id:               The process id.
 *
 * RETURN:
 * TRUE  :               Exists.
 * FALSE :               Not exists.
 */BOOL is_pid_io_exists(cg_proc_stats_lists* cg_stats_lists, const int id);

/*
 * Add the pid io statistics information element into the lists of io statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of pid io statistics informations.
 * @cg_stats:         The element of pid io statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int add_proc_stats(cg_proc_stats_lists* cg_stats_lists, cg_proc_stats* cg_stats);

/*
 * Remove the pid io statistics information element from the lists of io statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of pid io statistics informations.
 * @cg_stats:         The element of pid io statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int remove_proc_stats(cg_proc_stats_lists* cg_stats_lists,
		cg_proc_stats* cg_stats);

/*
 * Clear up the lists of pid io statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of pid io statistics informations.
 */
void clear_proc_stats(cg_proc_stats_lists* cg_stats_lists);
#endif

#ifdef CGROUP_MEMORY_STATS
/*
 * Initialized the structure of mem_stats.
 *
 * OUT:
 * @cg_stats:         The structure of mem_stats.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_mem_stats(cg_mem_stats** cg_stats);

/*
 * Uninitialized the structure of mem_stats.
 *
 * IN:
 * @cg_stats:         The structure of mem_stats.
 */
void uninit_mem_stats(cg_mem_stats* cg_stats);
#endif

#ifdef CGROUP_BLKIO_STATS
/*
 * Initialized the head of cgroup_device_stats.
 *
 * OUT:
 * @cg_stats_lists:         The structure of cgroup_device_stats_lists.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_device_stats_lists(cg_device_stats_lists** cg_stats_lists);

/*
 * Uninitialized the head of cgroup_device_stats.
 *
 * IN:
 * @cg_stats_lists:  The structure of cgroup_device_stats_lists.
 */
void uninit_device_stats_lists(cg_device_stats_lists* cg_value_lists);

/*
 * Initialized the structure of cgroup_device_stats.
 *
 * OUT:
 * @cg_stats :         The structure of cgroup_device_stats.
 *
 * IN :
 * @dev:               The device number.
 * @idx:               The index.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_device_stats(cg_device_stats** cg_stats, const int dev, const int idx);

/*
 * Uninitialized the structure of cgroup_device_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_device_stats.
 */
void uninit_device_stats(cg_device_stats* cg_stats);

/*
 * Get the device statistics information element of given  number and index from
 * the lists of device statistics information.
 *
 * OUT:
 * @cg_stats:            The element of device statistics information .
 *
 * IN :
 * @cg_stats_lists:      The lists of device statistics information .
 * @dev:               The device number.
 * @idx:               The index.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int get_device_stats(cg_device_stats_lists* cg_value_lists,
		cg_device_stats** cg_stats, const int dev, const int idx);

/*
 * Check the given device number and index whether in the list of device statistics
 *  information or not.
 *
 * IN :
 * @cg_stats_lists:         The lists of device statistics information .
 * @dev:               The device number.
 * @idx:               The index.
 *
 * RETURN:
 * TRUE  :               Exists.
 * FALSE :               Not exists.
 */BOOL is_device_stats_exists(cg_device_stats_lists* cg_value_lists,
		const int dev, const int idx);

/*
 * Add the device statistics information element into the lists of device
 * statistics information.
 *
 * IN :
 * @cg_stats_lists:   The lists of device statistics information.
 * @cg_stats:         The element of device statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int add_device_stats(cg_device_stats_lists* cg_stats_lists,
		cg_device_stats* cg_stats);

/*
 * Remove the device statistics information element from the lists of device
 * statistics information.
 *
 * IN :
 * @cg_stats_lists:         The lists of device statistics information.
 * @cg_stats:               The element of device statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int remove_device_stats(cg_device_stats_lists* cg_stats_lists,
		cg_device_stats* cg_stats);

/*
 * Clear up the lists of device statistics information.
 *
 * IN :
 * @cg_stats_lists:        The lists of device statistics information.
 */
void clear_device_stats(cg_device_stats_lists* cg_stats_lists);

/*
 * Initialized the head of cgroup_device_value.
 *
 * OUT:
 * @cg_value_lists:         The structure of cgroup_device_value_lists.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_device_value_lists(cg_device_value_lists** cg_value_lists);

/*
 * Uninitialized the head of cgroup_device_value.
 *
 * IN:
 * @cg_value_lists:  The structure of cgroup_device_value_lists.
 */
void uninit_device_value_lists(cg_device_value_lists* cg_value_lists);

/*
 * Initialized the structure of cgroup_device_value.
 *
 * OUT:
 * @cg_value :         The structure of cgroup_device_value.
 *
 * IN :
 * @dev:               The device number.
 * @idx:               The index.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_device_value(cg_device_value** cg_value, const int dev, const int idx);

/*
 * Uninitialized the structure of cgroup_device_value.
 *
 * IN:
 * @cg_value:         The structure of cgroup_device_value.
 */
void uninit_device_value(cg_device_value* cg_value);

/*
 * Get the device value of given name from the lists of device values.
 *
 * OUT:
 * @cg_stats:            The value of device.
 *
 * IN :
 * @cg_value_lists:      The lists of device values.
 * @dev:               The device number.
 * @idx:               The index.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int get_device_value(cg_device_value_lists* cg_value_lists,
		cg_device_value** cg_value, const int dev, const int idx);

/*
 * Check the given device name whether in the list of device values or not.
 *
 * IN :
 * @cg_value_lists:         The lists of device values.
 * @dev:               The device number.
 * @idx:               The index.
 *
 * RETURN:
 * TRUE  :               Exists.
 * FALSE :               Not exists.
 */BOOL is_device_value_exists(cg_device_value_lists* cg_value_lists,
		const int dev, const idx);

/*
 * Add the device value into the lists of device values.
 *
 * IN :
 * @cg_value_lists:   The lists of device values.
 * @cg_value:         The value of device.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int add_device_value(cg_device_value_lists* cg_value_lists,
		cg_device_value* cg_value);

/*
 * Remove the device value from the lists of device values.
 *
 * IN :
 * @cg_value_lists:         The lists of device values.
 * @cg_value:               The value of device.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int remove_device_value(cg_device_value_lists* cg_value_lists,
		cg_device_value* cg_value);

/*
 * Clear up the lists of device values.
 *
 * IN :
 * @cg_value_lists:        The lists of device values.
 */
void clear_device_value(cg_device_value_lists* cg_value_lists);

/*
 * Initialized the structure of cgroup_blkio_stats.
 *
 * OUT:
 * @cg_stats:         The structure of cgroup_blkio_stats.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_blkio_stats(cg_blkio_stats** cg_stats);

/*
 * Uninitialized the structure of cgroup_blkio_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_blkio_stats.
 */
void uninit_blkio_stats(cg_blkio_stats* cg_stats);
#endif

/*
 * Initialized the head of groups.
 *
 * OUT:
 * @group_lists:      The structure of cgroup_group_lists.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_group_lists(cg_group_lists** group_lists);

/*
 * Uninitialized the head of groups.
 *
 * IN:
 * @group_lists:      The structure of cgroup_group_lists.
 */
void uninit_group_lists(cg_group_lists* group_lists);

/*
 * Initialized the structure of cgroup_group.
 *
 * OUT:
 * @group:            The structure of cgroup_group.
 *
 * IN :
 * @name:             The group name.
 * @path:             The full path of group.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int init_group(cg_group** group, const char* name, const char* path);

/*
 * Uninitialized the structure of cgroup_group.
 *
 * IN:
 * @group:            The structure of cgroup_group.
 */
void uninit_group(cg_group* group);

/*
 * Get the group of given name from the lists of groups.
 *
 * OUT:
 * @group:            The element of given name.
 *
 * IN :
 * @group_lists:      The lists of groups.
 * @name:             The group name.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int get_group(cg_group_lists* group_lists, cg_group** group, const char* name);

/*
 * Check the given group name whether in the list of groups or not.
 *
 * IN :
 * @group_lists:      The lists of groups.
 * @name:             The group name.
 *
 * RETURN:
 * TRUE  :            Exists.
 * FALSE :            Not exists.
 */BOOL is_group_exists(cg_group_lists* group_lists, const char* name);

/*
 * Add the group element into the lists of groups.
 *
 * IN :
 * @group_lists:      The lists of groups.
 * @group:            The element of group.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int add_group(cg_group_lists* group_lists, cg_group* group);

/*
 * Remove the group element from the lists of groups.
 *
 * IN :
 * @group_lists:      The lists of groups.
 * @group:            The element of group.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int remove_group(cg_group_lists* group_lists, cg_group* group);

/*
 * Clear up the lists of groups.
 *
 * IN :
 * @group_lists:      The lists of groups.
 */
void clear_group(cg_group_lists* group_lists);

#endif /* __COMMON_H */
