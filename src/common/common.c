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

#include <stdlib.h>
#include <string.h>

#include <common/common.h>
#include <common/structures.h>
#include <common/tail_queue.h>

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
int init_cpuset_stats_lists(cg_cpuset_stats_lists** cg_stats_lists) {

	if (!cg_stats_lists)
		return -1;
	*cg_stats_lists = (cg_cpuset_stats_lists*) malloc(
			sizeof(cg_cpuset_stats_lists));
	if (!(*cg_stats_lists))
		return -1;
	memset(*cg_stats_lists, 0, sizeof(cg_cpuset_stats_lists));
	TAILQ_INIT(*cg_stats_lists);
	(*cg_stats_lists)->count = 0;
	return 0;
}

/*
 * Uninitialized the head of cgroup_cpuset_stats.
 *
 * IN:
 * @cg_stats_lists:  The structure of cgroup_cpuset_stats_lists.
 */
void uninit_cpuset_stats_lists(cg_cpuset_stats_lists* cg_stats_lists) {

	if (!cg_stats_lists)
		return;
	if (!TAILQ_EMPTY(cg_stats_lists))
		clear_cpuset_stats(cg_stats_lists);
	if (cg_stats_lists)
		free(cg_stats_lists);
}

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

int init_cpuset_stats(cg_cpuset_stats** cg_stats, const int id) {

	if (!cg_stats)
		return -1;
	*cg_stats = (cg_cpuset_stats*) malloc(sizeof(cg_cpuset_stats));
	if (!(*cg_stats))
		return -1;
	memset(*cg_stats, 0, sizeof(cg_cpuset_stats));
	(*cg_stats)->stats = (struct cpuset_stats*) malloc(CPUSET_STATS_SIZE);
	if (!(*cg_stats)->stats) {
		free(*cg_stats);
		return -1;
	}
	memset((*cg_stats)->stats, 0, CPUSET_STATS_SIZE);
	(*cg_stats)->id = id;
	return 0;
}

/*
 * Uninitialized the structure of cgroup_cpuset_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_cpuset_stats.
 */
void uninit_cpuset_stats(cg_cpuset_stats* cg_stats) {

	if (cg_stats) {
		if (cg_stats->stats)
			free(cg_stats->stats);
		free(cg_stats);
	}
}

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
		cg_cpuset_stats** cg_stats, const int id) {

	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists) || !cg_stats)
		return -1;
	TAILQ_FOREACH(*cg_stats, cg_stats_lists, next)
		if ((*cg_stats)->id == id)
			return 0;
	return -1;
}

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
 */BOOL is_cpuset_exists(cg_cpuset_stats_lists* cg_stats_lists, const int id) {

	cg_cpuset_stats* cg_stats = NULL;
	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return FALSE;
	TAILQ_FOREACH(cg_stats, cg_stats_lists, next)
		if (cg_stats->id == id)
			return TRUE;
	return FALSE;
}

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
		cg_cpuset_stats* cg_stats) {

	if (!cg_stats_lists || !cg_stats)
		return -1;
	if (!is_cpuset_exists(cg_stats_lists, cg_stats->id)) {
		TAILQ_INSERT_TAIL(cg_stats_lists, cg_stats, next);
		cg_stats_lists->count++;
	}
	return 0;
}

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
		cg_cpuset_stats* cg_stats) {

	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return -1;
	if (cg_stats) {
		TAILQ_REMOVE(cg_stats_lists, cg_stats, next);
		cg_stats_lists->count--;
		uninit_cpuset_stats(cg_stats);
	}
	return 0;
}

/*
 * Clear up the lists of cpuset statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of cpuset statistics informations.
 */
void clear_cpuset_stats(cg_cpuset_stats_lists* cg_stats_lists) {

	cg_cpuset_stats* cg_stats = NULL;
	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return;
	for (cg_stats = TAILQ_FIRST(cg_stats_lists); cg_stats; cg_stats =
			TAILQ_FIRST(cg_stats_lists))
		remove_cpuset_stats(cg_stats_lists, cg_stats);
	cg_stats_lists->count = 0;
	cg_stats = NULL;
}
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
int init_proc_stats_lists(cg_proc_stats_lists** cg_stats_lists) {

	if (!cg_stats_lists)
		return -1;
	*cg_stats_lists = (cg_proc_stats_lists*) malloc(
			sizeof(cg_proc_stats_lists));
	if (!(*cg_stats_lists))
		return -1;
	memset(*cg_stats_lists, 0, sizeof(cg_proc_stats_lists));
	TAILQ_INIT(*cg_stats_lists);
	(*cg_stats_lists)->count = 0;
	return 0;
}

/*
 * Uninitialized the head of cgroup_proc_stats.
 *
 * IN:
 * @cg_stats_lists:  The structure of cgroup_proc_stats_lists.
 */
void uninit_proc_stats_lists(cg_proc_stats_lists* cg_stats_lists) {

	if (!cg_stats_lists)
		return;
	if (!TAILQ_EMPTY(cg_stats_lists))
		clear_proc_stats(cg_stats_lists);
	if (cg_stats_lists)
		free(cg_stats_lists);
}

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
int init_proc_stats(cg_proc_stats** cg_stats, const int id) {

	if (!cg_stats)
		return -1;
	*cg_stats = (cg_proc_stats*) malloc(sizeof(cg_proc_stats));
	if (!(*cg_stats))
		return -1;
	memset(*cg_stats, 0, sizeof(cg_proc_stats));
	(*cg_stats)->io_stats = (struct io_stats*) malloc(IO_STATS_SIZE);
	if (!(*cg_stats)->io_stats) {
		goto err;
	}
	memset((*cg_stats)->io_stats, 0, IO_STATS_SIZE);
#ifdef CGROUP_VMSTAT
	(*cg_stats)->proc_stat = (struct proc_stat*) malloc(PROC_STAT_SIZE);
	if (!(*cg_stats)->proc_stat) {
		goto err;
	}
	memset((*cg_stats)->proc_stat, 0, PROC_STAT_SIZE);
	(*cg_stats)->proc_statm = (struct proc_statm*) malloc(PROC_STATM_SIZE);
	if (!(*cg_stats)->proc_statm) {
		goto err;
	}
	memset((*cg_stats)->proc_statm, 0, PROC_STATM_SIZE);
#endif
	(*cg_stats)->id = id;

	return 0;
	err: uninit_proc_stats(*cg_stats);
	return -1;
}

/*
 * Uninitialized the structure of cgroup_proc_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_proc_stats.
 */
void uninit_proc_stats(cg_proc_stats* cg_stats) {

	if (cg_stats) {
		if (cg_stats->io_stats)
			free(cg_stats->io_stats);
#ifdef CGROUP_VMSTAT
		if (cg_stats->proc_stat)
			free(cg_stats->proc_stat);
		if (cg_stats->proc_statm)
			free(cg_stats->proc_statm);
#endif
		free(cg_stats);
	}
}

/*
 * Get the io statistics information element of given id from the lists of
 * io statistics informations.
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
		cg_proc_stats** cg_stats, const int id) {

	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists) || !cg_stats)
		return -1;
	TAILQ_FOREACH(*cg_stats, cg_stats_lists, next)
		if (*cg_stats && (*cg_stats)->id == id)
			return 0;
	return -1;
}

/*
 * Check the given process id whether in the list of io statistics informations or not.
 *
 * IN :
 * @cg_stats_lists:   The lists of io statistics informations.
 * @id:               The process id.
 *
 * RETURN:
 * TRUE  :               Exists.
 * FALSE :               Not exists.
 */BOOL is_proc_exists(cg_proc_stats_lists* cg_stats_lists, const int id) {

	cg_proc_stats* cg_stats = NULL;
	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists) || !id)
		return FALSE;
	TAILQ_FOREACH(cg_stats, cg_stats_lists, next)
		if (cg_stats && (cg_stats)->id == id)
			return TRUE;
	return FALSE;
}

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
int add_proc_stats(cg_proc_stats_lists* cg_stats_lists, cg_proc_stats* cg_stats) {

	if (!cg_stats_lists || !cg_stats)
		return -1;
	if (!is_proc_exists(cg_stats_lists, cg_stats->id)) {
		TAILQ_INSERT_TAIL(cg_stats_lists, cg_stats, next);
		cg_stats_lists->count++;
	}
	return 0;
}

/*
 * Remove the io statistics information element from the lists of io statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of io statistics informations.
 * @cg_stats:         The element of io statistics information.
 *
 * RETURN:
 * 0  :               Success.
 * -1 :               Failed.
 */
int remove_proc_stats(cg_proc_stats_lists* cg_stats_lists,
		cg_proc_stats* cg_stats) {

	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return -1;
	if (cg_stats) {
		TAILQ_REMOVE(cg_stats_lists, cg_stats, next);
		cg_stats_lists->count--;
		uninit_proc_stats(cg_stats);
	}
	return 0;
}

/*
 * Clear up the lists of pid io statistics informations.
 *
 * IN :
 * @cg_stats_lists:   The lists of pid io statistics informations.
 */
void clear_proc_stats(cg_proc_stats_lists* cg_stats_lists) {

	cg_proc_stats* cg_stats = NULL;
	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return;
	for (cg_stats = TAILQ_FIRST(cg_stats_lists); cg_stats; cg_stats =
			TAILQ_FIRST(cg_stats_lists))
		remove_proc_stats(cg_stats_lists, cg_stats);
	cg_stats_lists->count = 0;
	cg_stats = NULL;
}
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
int init_mem_stats(cg_mem_stats** cg_stats) {

	if (!cg_stats)
		return -1;
	*cg_stats = (cg_mem_stats*) malloc(sizeof(cg_mem_stats));
	if (!(*cg_stats))
		return -1;
	memset(*cg_stats, 0, sizeof(cg_mem_stats));
	(*cg_stats)->stats = (struct mem_stats*) malloc(sizeof(struct mem_stats));
	if (!(*cg_stats)->stats) {
		free(*cg_stats);
		return -1;
	}
	memset((*cg_stats)->stats, 0, sizeof(struct mem_stats));
	return 0;
}

/*
 * Uninitialized the structure of mem_stats.
 *
 * IN:
 * @cg_stats:         The structure of mem_stats.
 */
void uninit_mem_stats(cg_mem_stats* cg_stats) {

	if (cg_stats) {
		if (cg_stats->stats)
			free(cg_stats->stats);
		free(cg_stats);
	}
}
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
int init_device_stats_lists(cg_device_stats_lists** cg_stats_lists) {

	if (!cg_stats_lists)
		return -1;
	*cg_stats_lists = (cg_device_stats_lists*) malloc(
			sizeof(cg_device_stats_lists));
	if (!(*cg_stats_lists))
		return -1;
	memset(*cg_stats_lists, 0, sizeof(cg_device_stats_lists));
	TAILQ_INIT(*cg_stats_lists);
	return 0;
}

/*
 * Uninitialized the head of cgroup_device_stats.
 *
 * IN:
 * @cg_stats_lists:  The structure of cgroup_device_stats_lists.
 */
void uninit_device_stats_lists(cg_device_stats_lists* cg_value_lists) {

	if (!cg_value_lists)
		return;
	if (!TAILQ_EMPTY(cg_value_lists))
		clear_device_stats(cg_value_lists);
	if (cg_value_lists)
		free(cg_value_lists);
}

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
int init_device_stats(cg_device_stats** cg_stats, const int dev, const int idx) {

	if (!cg_stats)
		return -1;
	*cg_stats = (cg_device_stats*) malloc(sizeof(cg_device_stats));
	if (!(*cg_stats))
		return -1;
	memset(*cg_stats, 0, sizeof(cg_device_stats));
	(*cg_stats)->stats = (struct device_stats*) malloc(
			sizeof(struct device_stats));
	if (!(*cg_stats)->stats) {
		free(*cg_stats);
		return -1;
	}
	memset((*cg_stats)->stats, 0, sizeof(struct device_stats));
	(*cg_stats)->dev = dev;
	(*cg_stats)->idx = idx;
	return 0;
}

/*
 * Uninitialized the structure of cgroup_device_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_device_stats.
 */
void uninit_device_stats(cg_device_stats* cg_stats) {

	if (cg_stats) {
		if (cg_stats->stats)
			free(cg_stats->stats);
		free(cg_stats);
	}
}

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
		cg_device_stats** cg_stats, const int dev, const int idx) {

	if (!cg_value_lists || TAILQ_EMPTY(cg_value_lists) || !cg_stats)
		return -1;
	TAILQ_FOREACH((*cg_stats), cg_value_lists, next)
		if (*cg_stats && (*cg_stats)->dev == dev && (*cg_stats)->idx == idx)
			return 0;
	return -1;
}

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
		const int dev, const int idx) {

	cg_device_stats* cg_stats = NULL;
	if (!cg_value_lists || TAILQ_EMPTY(cg_value_lists))
		return FALSE;
	TAILQ_FOREACH(cg_stats, cg_value_lists, next)
		if (cg_stats && cg_stats->dev == dev && cg_stats->idx == idx)
			return TRUE;
	return FALSE;
}

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
		cg_device_stats* cg_stats) {

	if (!cg_stats_lists || !cg_stats)
		return -1;
	if (!is_device_stats_exists(cg_stats_lists, cg_stats->dev, cg_stats->idx)) {
		TAILQ_INSERT_TAIL(cg_stats_lists, cg_stats, next);
		cg_stats_lists->count++;
	}
	return 0;
}

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
		cg_device_stats* cg_stats) {

	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return -1;
	if (cg_stats) {
		TAILQ_REMOVE(cg_stats_lists, cg_stats, next);
		cg_stats_lists->count--;
		uninit_device_stats(cg_stats);
	}
	return 0;
}

/*
 * Clear up the lists of device statistics information.
 *
 * IN :
 * @cg_stats_lists:        The lists of device statistics information.
 */
void clear_device_stats(cg_device_stats_lists* cg_stats_lists) {

	cg_device_stats* cg_stats = NULL;
	if (!cg_stats_lists || TAILQ_EMPTY(cg_stats_lists))
		return;
	for (cg_stats = TAILQ_FIRST(cg_stats_lists); cg_stats; cg_stats =
			TAILQ_FIRST(cg_stats_lists))
		remove_device_stats(cg_stats_lists, cg_stats);
	cg_stats_lists->count = 0;
	cg_stats = NULL;
}

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
int init_device_value_lists(cg_device_value_lists** cg_value_lists) {

	if (!cg_value_lists)
		return -1;
	*cg_value_lists = (cg_device_value_lists*) malloc(
			sizeof(cg_device_value_lists));
	if (!(*cg_value_lists))
		return -1;
	memset(*cg_value_lists, 0, sizeof(cg_device_value_lists));
	TAILQ_INIT(*cg_value_lists);
	return 0;
}

/*
 * Uninitialized the head of cgroup_device_value.
 *
 * IN:
 * @cg_value_lists:  The structure of cgroup_device_value_lists.
 */
void uninit_device_value_lists(cg_device_value_lists* cg_value_lists) {

	if (!cg_value_lists)
		return;
	if (!TAILQ_EMPTY(cg_value_lists))
		clear_device_value(cg_value_lists);
	if (cg_value_lists)
		free(cg_value_lists);
}

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
int init_device_value(cg_device_value** cg_value, const int dev, const int idx) {

	if (!cg_value)
		return -1;
	*cg_value = (cg_device_value*) malloc(sizeof(cg_device_value));
	if (!(*cg_value))
		return -1;
	memset(*cg_value, 0, sizeof(cg_device_value));
	(*cg_value)->dev = dev;
	(*cg_value)->idx = idx;
	return 0;
}

/*
 * Uninitialized the structure of cgroup_device_value.
 *
 * IN:
 * @cg_value:         The structure of cgroup_device_value.
 */
void uninit_device_value(cg_device_value* cg_value) {

	if (cg_value) {
		free(cg_value);
	}
}

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
		cg_device_value** cg_value, const int dev, const int idx) {

	if (!cg_value_lists || TAILQ_EMPTY(cg_value_lists) || !cg_value)
		return -1;
	TAILQ_FOREACH((*cg_value), cg_value_lists, next)
		if (*cg_value && (*cg_value)->dev == dev && (*cg_value)->idx == idx)
			return 0;
	return -1;
}

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
		const int dev, const idx) {

	cg_device_value* cg_value = NULL;
	if (!cg_value_lists || TAILQ_EMPTY(cg_value_lists))
		return FALSE;
	TAILQ_FOREACH(cg_value, cg_value_lists, next)
		if (cg_value && cg_value->dev == dev && cg_value->idx == idx)
			return TRUE;
	return FALSE;
}

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
		cg_device_value* cg_value) {

	if (!cg_value_lists || !cg_value)
		return -1;
	if (!is_device_value_exists(cg_value_lists, cg_value->dev, cg_value->idx)) {
		TAILQ_INSERT_TAIL(cg_value_lists, cg_value, next);
		cg_value_lists->count++;
	}
	return 0;
}

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
		cg_device_value* cg_value) {

	if (!cg_value_lists || TAILQ_EMPTY(cg_value_lists))
		return -1;
	if (cg_value) {
		TAILQ_REMOVE(cg_value_lists, cg_value, next);
		cg_value_lists->count--;
		uninit_device_value(cg_value);
	}
	return 0;
}

/*
 * Clear up the lists of device values.
 *
 * IN :
 * @cg_value_lists:        The lists of device values.
 */
void clear_device_value(cg_device_value_lists* cg_value_lists) {

	cg_device_value* cg_value = NULL;
	if (!cg_value_lists || TAILQ_EMPTY(cg_value_lists))
		return;
	for (cg_value = TAILQ_FIRST(cg_value_lists); cg_value; cg_value =
			TAILQ_FIRST(cg_value_lists))
		remove_device_value(cg_value_lists, cg_value);
	cg_value_lists->count = 0;
	cg_value = NULL;
}

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
int init_blkio_stats(cg_blkio_stats** cg_stats) {

	if (!cg_stats)
		return -1;
	*cg_stats = (cg_blkio_stats*) malloc(sizeof(cg_blkio_stats));
	if (!(*cg_stats))
		return -1;
	memset(*cg_stats, 0, sizeof(cg_blkio_stats));
	if (init_device_stats_lists(&((*cg_stats)->io_merged))
			|| init_device_stats_lists(&((*cg_stats)->io_queued))
			|| init_device_stats_lists(&((*cg_stats)->io_service_bytes))
			|| init_device_stats_lists(&((*cg_stats)->io_service_time))
			|| init_device_stats_lists(&((*cg_stats)->io_serviced))
			|| init_device_stats_lists(&((*cg_stats)->io_wait_time))
			|| init_device_stats_lists(
					&((*cg_stats)->throttle_io_service_bytes))
			|| init_device_stats_lists(&((*cg_stats)->throttle_io_serviced))
			|| init_device_value_lists(&((*cg_stats)->time))
			|| init_device_value_lists(&((*cg_stats)->sectors))
			|| init_device_value_lists(&((*cg_stats)->throttle_read_bps_device))
			|| init_device_value_lists(
					&((*cg_stats)->throttle_read_iops_device))
			|| init_device_value_lists(
					&((*cg_stats)->throttle_write_bps_device))
			|| init_device_value_lists(
					&((*cg_stats)->throttle_write_iops_device)))
		goto err;
	return 0;

	err: uninit_blkio_stats(*cg_stats);
	return -1;
}

/*
 * Uninitialized the structure of cgroup_blkio_stats.
 *
 * IN:
 * @cg_stats:         The structure of cgroup_blkio_stats.
 */
void uninit_blkio_stats(cg_blkio_stats* cg_stats) {

	if (cg_stats) {
		if (cg_stats->io_merged)
			uninit_device_stats_lists(cg_stats->io_merged);
		if (cg_stats->io_queued)
			uninit_device_stats_lists(cg_stats->io_queued);
		if (cg_stats->io_service_bytes)
			uninit_device_stats_lists(cg_stats->io_service_bytes);
		if (cg_stats->io_service_time)
			uninit_device_stats_lists(cg_stats->io_service_time);
		if (cg_stats->io_serviced)
			uninit_device_stats_lists(cg_stats->io_serviced);
		if (cg_stats->io_wait_time)
			uninit_device_stats_lists(cg_stats->io_wait_time);
		if (cg_stats->throttle_io_service_bytes)
			uninit_device_stats_lists(cg_stats->throttle_io_service_bytes);
		if (cg_stats->throttle_io_serviced)
			uninit_device_stats_lists(cg_stats->throttle_io_serviced);
		if (cg_stats->time)
			uninit_device_value_lists(cg_stats->time);
		if (cg_stats->sectors)
			uninit_device_value_lists(cg_stats->sectors);
		if (cg_stats->throttle_read_bps_device)
			uninit_device_value_lists(cg_stats->throttle_read_bps_device);
		if (cg_stats->throttle_read_iops_device)
			uninit_device_value_lists(cg_stats->throttle_read_iops_device);
		if (cg_stats->throttle_write_bps_device)
			uninit_device_value_lists(cg_stats->throttle_write_bps_device);
		if (cg_stats->throttle_write_iops_device)
			uninit_device_value_lists(cg_stats->throttle_write_iops_device);

		free(cg_stats);
	}
}

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
int init_group_lists(cg_group_lists** group_lists) {

	if (!group_lists)
		return -1;
	*group_lists = (cg_group_lists*) malloc(sizeof(cg_group_lists));
	if (!(*group_lists))
		return -1;
	memset(*group_lists, 0, sizeof(cg_group_lists));
	TAILQ_INIT(*group_lists);
	return 0;
}

/*
 * Uninitialized the head of groups.
 *
 * IN:
 * @group_lists:      The structure of cgroup_group_lists.
 */
void uninit_group_lists(cg_group_lists* group_lists) {

	if (!group_lists)
		return;
	if (!TAILQ_EMPTY(group_lists))
		clear_group(group_lists);
	if (group_lists)
		free(group_lists);
}

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
int init_group(cg_group** group, const char* name, const char* path) {

	cg_cpuset_stats* ptr_cpuset_stats = NULL;
	cg_proc_stats* ptr_io_stats = NULL;

	if (!group || !name)
		return -1;
	if (!(*group = (cg_group*) malloc(sizeof(cg_group))))
		return -1;
	(*group)->name = strdup(name);
	if (!(*group)->name)
		goto err;
	if (path)
		(*group)->path = strdup(path);
	/** Initialized the cpuset statistics information array.*/
	if (init_cpuset_stats_lists(&((*group)->cpuset_stats[0]))
			|| init_cpuset_stats_lists(&((*group)->cpuset_stats[1])))
		goto err;
#ifdef CGROUP_MEMORY_STATS
	/** Initialized the memory statistics information array.*/
	if (init_mem_stats(&((*group)->mem_stats[0]))
			|| init_mem_stats(&((*group)->mem_stats[1])))
		goto err;
#endif

#ifdef CGROUP_BLKIO_STATS
	if (init_blkio_stats(&((*group)->blkio_stats[0]))
			|| init_blkio_stats(&((*group)->blkio_stats[1])))
		goto err;
#endif
#ifdef CGROUP_PROC_STAT
	if (init_proc_stats_lists(&((*group)->proc_stats[0]))
			|| init_proc_stats_lists(&((*group)->proc_stats[1])))
		goto err;
#endif
	return 0;

	/** If happened error, free the memory resources.*/
	err: uninit_group(*group);
	return -1;
}

/*
 * Uninitialized the structure of cgroup_group.
 *
 * IN:
 * @group:            The structure of cgroup_group.
 */
void uninit_group(cg_group* group) {

	if (group) {
		if (group->name)
			free(group->name);
		if (group->path)
			free(group->path);
		if (group->cpuset_stats[0])
			uninit_cpuset_stats_lists(group->cpuset_stats[0]);
		if (group->cpuset_stats[1])
			uninit_cpuset_stats_lists(group->cpuset_stats[1]);
#ifdef CGROUP_MEMORY_STATS
		if (group->mem_stats[0])
			uninit_mem_stats(group->mem_stats[0]);
		if (group->mem_stats[1])
			uninit_mem_stats(group->mem_stats[1]);
#endif
#ifdef CGROUP_BLKIO_STATS
		if (group->blkio_stats[0])
			uninit_blkio_stats(group->blkio_stats[0]);
		if (group->blkio_stats[1])
			uninit_blkio_stats(group->blkio_stats[1]);
#endif
#ifdef CGROUP_PROC_STAT
		if (group->proc_stats[0])
			uninit_proc_stats_lists(group->proc_stats[0]);
		if (group->proc_stats[1])
			uninit_proc_stats_lists(group->proc_stats[1]);
#endif
		free(group);
	}
}

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
int get_group(cg_group_lists* group_lists, cg_group** group, const char* name) {

	if (!group_lists || TAILQ_EMPTY(group_lists) || !group || !name)
		return -1;
	TAILQ_FOREACH(*group, group_lists, next)
		if (*group && (*group)->name
				&& !strncmp((*group)->name, name,
						max(strlen(name), strlen((*group)->name))))
			return 0;
	return -1;
}

/*
 * Check the given group name whether in the list of groups or not.
 *
 * IN :
 * @group_lists:      The lists of groups.
 * @name:             The group name.
 *l
 * RETURN:
 * TRUE  :            Exists.
 * FALSE :            Not exists.
 */BOOL is_group_exists(cg_group_lists* group_lists, const char* name) {

	cg_group* group = NULL;
	if (!group_lists || TAILQ_EMPTY(group_lists) || !name)
		return FALSE;
	TAILQ_FOREACH(group, group_lists, next)
		if (group && group->name
				&& !strncmp(group->name, name,
						max(strlen(name), strlen(group->name))))
			return TRUE;
	return FALSE;
}

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
int add_group(cg_group_lists* group_lists, cg_group* group) {

	cg_group* ptr_group = NULL;
	cg_cpuset_stats* ptr_cpuset_stats = NULL;
	cg_proc_stats* ptr_io_stats = NULL;

	if (!group_lists || !group)
		return -1;

	if (!is_group_exists(group_lists, group->name)) {
		TAILQ_INSERT_HEAD(group_lists, group, next);
		group_lists->count++;
	}
	return 0;
}

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
int remove_group(cg_group_lists* group_lists, cg_group* group) {

	if (!group_lists || TAILQ_EMPTY(group_lists))
		return -1;
	if (group) {
		TAILQ_REMOVE(group_lists, group, next);
		group_lists->count--;
		uninit_group(group);
	}
	return 0;
}

/*
 * Clear up the lists of groups.
 *
 * IN :
 * @group_lists:      The lists of groups.
 */
void clear_group(cg_group_lists* group_lists) {

	cg_group* group = NULL;
	if (!group_lists || TAILQ_EMPTY(group_lists))
		return;
	for (group = TAILQ_FIRST(group_lists); group; group =
			TAILQ_FIRST(group_lists))
		remove_group(group_lists, group);
	group_lists->count = 0;
	group = NULL;
}

