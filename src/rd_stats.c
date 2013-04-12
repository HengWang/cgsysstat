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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libcgroup.h>
#include <cgsysstat.h>
#include "rd_stats.h"

/*
 * The macro for io_stats. All of the keywords and relative length are used
 * to get the value of corresponding field.
 */
/** The rchar keyword and the length of keyword.*/
#define RCHAR "rchar"
#define RCHAR_LEN 5
/** The wchar keyword and the length of keyword.*/
#define WCHAR "wchar"
#define WCHAR_LEN 5
/** The syscr keyword and the length of keyword.*/
#define SYSCR "syscr"
#define SYSCR_LEN 5
/** The syscw keyword and the length of keyword.*/
#define SYSCW "syscw"
#define SYSCW_LEN 5
/** The read_bytes keyword and the length of keyword.*/
#define READ_BYTES "read_bytes"
#define READ_BYTES_LEN 10
/** The write_bytes keyword and the length of keyword.*/
#define WRITE_BYTES "write_bytes"
#define WRITE_BYTES_LEN 11
/** The cancelled_write_bytes keyword and the length of keyword.*/
#define CANCELLED_WRITE_BYTES "cancelled_write_bytes"
#define CANCELLED_WRITE_BYTES_LEN 21

/*
 * The macro for mem_stats. All of the keywords and relative length are used
 * to get the value of corresponding field.
 */
#ifdef CGROUP_MEMORY_STATS
/** The cache keyword and the length of keyword.*/
#define CACHE "cache"
#define CACHE_LEN 5
/** The rss keyword and the length of keyword.*/
#define RSS "rss"
#define RSS_LEN 3
/** The mapped_file keyword and the length of keyword.*/
#define MAPPED_FILE "mapped_file"
#define MAPPED_FILE_LEN 11
/** The pgpgin keyword and the length of keyword.*/
#define PGPGIN "pgpgin"
#define PGPGIN_LEN 6
/** The pgpgout keyword and the length of keyword.*/
#define PGPGOUT "pgpgout"
#define PGPGOUT_LEN 7
/** The swap keyword and the length of keyword.*/
#define SWAP "swap"
#define SWAP_LEN 4
/** The inavtive_anon keyword and the length of keyword.*/
#define INACTIVE_ANON "inactive_anon"
#define INACTIVE_ANON_LEN 13
/** The avtive_anon keyword and the length of keyword.*/
#define ACTIVE_ANON "active_anon"
#define ACTIVE_ANON_LEN 11
/** The inactive_file keyword and the length of keyword.*/
#define INACTIVE_FILE "inactive_file"
#define INACTIVE_FILE_LEN 13
/** The actvice_file keyword and the length of keyword.*/
#define ACTIVE_FILE "active_file"
#define ACTIVE_FILE_LEN 11
/** The unevictable keyword and the length of keyword.*/
#define UNEVICTABLE "unevictable"
#define UNEVICTABLE_LEN 11
/** The hierarchical_memory_limit keyword and the length of keyword.*/
#define HIERARCHICAL_MEMORY_LIMIT "hierarchical_memory_limit"
#define HIERARCHICAL_MEMORY_LIMIT_LEN 25
/** The hierarchical_memsw_limit keyword and the length of keyword.*/
#define HIERARCHICAL_MEMSW_LIMIT "hierarchical_memsw_limit"
#define HIERARCHICAL_MEMSW_LIMIT_LEN 24
/** The total_cache keyword and the length of keyword.*/
#define TOTAL_CACHE "total_cache"
#define TOTAL_CACHE_LEN 11
/** The total_rss keyword and the length of keyword.*/
#define TOTAL_RSS "total_rss"
#define TOTAL_RSS_LEN 9
/** The total_mapped_file keyword and the length of keyword.*/
#define TOTAL_MAPPED_FILE "total_mapped_file"
#define TOTAL_MAPPED_FILE_LEN 17
/** The total_pgpgin keyword and the length of keyword.*/
#define TOTAL_PGPGIN "total_pgpgin"
#define TOTAL_PGPGIN_LEN 12
/** The total_pgpgout keyword and the length of keyword.*/
#define TOTAL_PGPGOUT "total_pgpgout"
#define TOTAL_PGPGOUT_LEN 13
/** The total_swap keyword and the length of keyword.*/
#define TOTAL_SWAP "total_swap"
#define TOTAL_SWAP_LEN 10
/** The total_inactive_anon keyword and the length of keyword.*/
#define TOTAL_INACTIVE_ANON "total_inactive_anon"
#define TOTAL_INACTIVE_ANON_LEN 19
/** The total_active_anon keyword and the length of keyword.*/
#define TOTAL_ACTIVE_ANON "total_active_anon"
#define TOTAL_ACTIVE_ANON_LEN 17
/** The total_inactive_file keyword and the length of keyword.*/
#define TOTAL_INACTIVE_FILE "total_inactive_file"
#define TOTAL_INACTIVE_FILE_LEN 19
/** The total_active_file keyword and the length of keyword.*/
#define TOTAL_ACTIVE_FILE "total_active_file"
#define TOTAL_ACTIVE_FILE_LEN 17
/** The total_unevictable keyword and the length of keyword.*/
#define TOTAL_UNEVICTABLE "total_unevictable"
#define TOTAL_UNEVICTABLE_LEN 17
#endif

/*
 * The file name in each group of cgroup.
 */
/** The file stores all of the process id of current group.*/
#define CGROUP_PROCS "cgroup.procs"
/** The file stores the allocated cpuset id  of current group.*/
#define CPUSET_CPUS "cpuset.cpus"

#ifdef CGROUP_BLKIO_STATS
/** The file stores the io weight of current group, just to
 *  be effective in the CFQ schedule.*/
#define BLKIO_WEIGHT "blkio.weight"
/** The file stores the io weight of current group, just to
 *  be effective in the CFQ schedule.*/
#define BLKIO_WEIGHT_DEVICE "blkio.weight_device"
/** The file stores the read speed of bits per second.*/
#define BLKIO_THROTTLE_READ_BPS "blkio.throttle.read_bps_device"
/** The file stores the write speed of bits per second.*/
#define BLKIO_THROTTLE_WRITE_BPS "blkio.throttle.write_bps_device"
/** The file stores the read speed of bits per second.*/
#define BLKIO_THROTTLE_READ_IOPS "blkio.throttle.read_iops_device"
/** The file stores the write speed of bits per second.*/
#define BLKIO_THROTTLE_WRITE_IOPS "blkio.throttle.write_iops_device"
/** The file stores the value of io service bytes.*/
#define BLKIO_IO_SERVICE_BYTES "blkio.io_service_bytes"
/** The file stores the value of io serviced.*/
#define BLKIO_IO_SERVICED "blkio.io_serviced"
/** The file stores the value of io service time.*/
#define BLKIO_IO_SERVICE_TIME "blkio.io_service_time"
/** The file stores the value of io wait time.*/
#define BLKIO_IO_WAIT_TIME "blkio.io_wait_time"
/** The file stores the value of io merged.*/
#define BLKIO_IO_MERGED "blkio.io_merged"
/** The file stores the value of io queued.*/
#define BLKIO_IO_QUEUED "blkio.io_queued"
/** The file stores the value of throttle io service bytes.*/
#define BLKIO_THROTTLE_IO_SERVICE_BYTES "blkio.throttle.io_service_bytes"
/** The file stores the value of throttle io serviced.*/
#define BLKIO_THROTTLE_IO_SERVICED "blkio.throttle.io_serviced"
/** The file stores the value of io time.*/
#define BLKIO_TIME "blkio.time"
/** The file stores the value of sectors.*/
#define BLKIO_SECTORS "blkio.sectors"

/** The read keyword and the length of keyword.*/
#define BLKIO_READ "Read"
#define BLKIO_READ_LEN 4
/** The write keyword and the length of keyword.*/
#define BLKIO_WRITE "Write"
#define BLKIO_WRITE_LEN 5
/** The sync keyword and the length of keyword.*/
#define BLKIO_SYNC "Sync"
#define BLKIO_SYNC_LEN 4
/** The async keyword and the length of keyword.*/
#define BLKIO_ASYNC "Async"
#define BLKIO_ASYNC_LEN 5
/** The total keyword and the length of keyword.*/
#define BLKIO_TOTAL "Total"
#define BLKIO_TOTAL_LEN 5
#endif
/** The file stores the cpuset statistics information.*/
#ifndef STAT
#define STAT "/proc/stat"
#endif

#ifndef DISKSTAT
#define DISKSTAT "/proc/diskstats"
#endif

#ifdef CGROUP_MEMORY_STATS
/** The file store the memory statistics information.*/
#define MEMORY_STATS "memory.stat"
/** The file store the memory usage.*/
#define MEMORY_USAGE "memory.usage_in_bytes"
/** The file store the memory and swap usage.*/
#define MEMORY_MEMSW_USAGE "memory.memsw.usage_in_bytes"
/** The file store the max memory usage.*/
#define MEMORY_MAX_USAGE "memory.max_usage_in_bytes"
/** The file store the max memory and swap usage.*/
#define MEMORY_MEMSW_MAX_USAGE "memory.memsw.max_usage_in_bytes"
/** The file store the memory limitation.*/
#define MEMORY_LIMIT "memory.limit_in_bytes"
/** The file store the memory and swap limitation usage.*/
#define MEMORY_MEMSW_LIMIT "memory.memsw.limit_in_bytes"
/** The file store the count of meeting the limit.*/
#define MEMORY_FAILCNT "memory.failcnt"
/** The file store the count of meeting the memory and swap limit.*/
#define MEMORY_MEMSW_FAILCNT "memory.memsw.failcnt"
/** The file store the value of memory swap schedule which user set. */
#define MEMORY_SWAPPINESS "memory.swappiness"
/** The file store the status of storing the memory of cgroup hierarchy.*/
#define MEMORY_USE_HIERARCHY "memory.use_hierarchy"
#endif

/*
 * The macro to read the value of the cpuset.
 */
#define read_cpuset_cpus(A,B) read_record_value(CG_CPUSET,A,CPUSET_CPUS,B)

/*
 * The macro to read the value of the memory.
 */
#ifdef CGROUP_MEMORY_STATS
#define read_mem_usage(A,B) read_ull_value(CG_MEMORY,A,MEMORY_USAGE,B)
#define read_mem_swap_usage(A,B) read_ull_value(CG_MEMORY,A,MEMORY_MEMSW_USAGE,B)
#define read_mem_max_usage(A,B) read_ull_value(CG_MEMORY,A,MEMORY_MAX_USAGE,B)
#define read_mem_swap_max_usage(A,B) read_ull_value(CG_MEMORY,A,MEMORY_MEMSW_MAX_USAGE,B)
#define read_mem_limit(A,B) read_ull_value(CG_MEMORY,A,MEMORY_LIMIT,B)
#define read_mem_swap_limit(A,B) read_ull_value(CG_MEMORY,A,MEMORY_MEMSW_LIMIT,B)
#define read_mem_failcnt(A,B) read_ull_value(CG_MEMORY,A,MEMORY_FAILCNT,B)
#define read_mem_swap_failcnt(A,B) read_ull_value(CG_MEMORY,A,MEMORY_MEMSW_FAILCNT,B)
#define read_mem_swappiness(A,B) read_ull_value(CG_MEMORY,A,MEMORY_SWAPPINESS,B)
#define read_mem_use_hierarchy(A,B) read_ull_value(CG_MEMORY,A,MEMORY_USE_HIERARCHY,B)
#endif

/*
 * The macro to read the value of the blkio.
 */
#ifdef CGROUP_BLKIO_STATS
#define read_io_weight(A,B) read_ull_value(CG_BLKIO,A,BLKIO_WEIGHT,B)
#define read_io_weight_device(A,B) read_ull_value(CG_BLKIO,A,BLKIO_WEIGHT,B)

#define read_io_service_bytes(A,B) read_device_stats(CG_BLKIO,A,BLKIO_IO_SERVICE_BYTES,B)
#define read_io_serviced(A,B) read_device_stats(CG_BLKIO,A,BLKIO_IO_SERVICED,B)
#define read_io_service_time(A,B) read_device_stats(CG_BLKIO,A,BLKIO_IO_SERVICE_TIME,B)
#define read_io_wait_time(A,B) read_device_stats(CG_BLKIO,A,BLKIO_IO_WAIT_TIME,B)
#define read_io_merged(A,B) read_device_stats(CG_BLKIO,A,BLKIO_IO_MERGED,B)
#define read_io_queued(A,B) read_device_stats(CG_BLKIO,A,BLKIO_IO_QUEUED,B)
#define read_io_throttle_io_service_bytes(A,B) read_device_stats(CG_BLKIO,A,BLKIO_THROTTLE_IO_SERVICE_BYTES,B)
#define read_io_throttle_io_serviced(A,B) read_device_stats(CG_BLKIO,A,BLKIO_THROTTLE_IO_SERVICED,B)

#define read_io_throttle_read_bps(A,B) read_device_value(CG_BLKIO,A,BLKIO_THROTTLE_READ_BPS,B)
#define read_io_throttle_write_bps(A,B) read_device_value(CG_BLKIO,A,BLKIO_THROTTLE_WRITE_BPS,B)
#define read_io_throttle_read_iops(A,B) read_device_value(CG_BLKIO,A,BLKIO_THROTTLE_READ_IOPS,B)
#define read_io_throttle_write_iops(A,B) read_device_value(CG_BLKIO,A,BLKIO_THROTTLE_WRITE_IOPS,B)
#define read_io_time(A,B) read_device_value(CG_BLKIO,A,BLKIO_TIME,B)
#define read_io_sectors(A,B) read_device_value(CG_BLKIO,A,BLKIO_SECTORS,B)

#endif
/*
 * Read the record from the given group of the device, the record just have
 * the value of parameter, so nothing to process.
 *
 * IN:
 * @device_name:          The given device name.
 * @group_name:           The given group name.
 * @param:                The parameter.
 *
 * OUT:
 * @vaule:                The value of the parameter.
 *
 * RETURN
 * 0:                     Success.
 * -1:                    Failed.
 */
static int read_record_value(const char* device_name, const char* group_name,
		const char* param, char* value) {

	void* handle;
	char line[LINE_LEN];
	if (!device_name || !group_name || !param || !value) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: the input parameters is invalid.\n");
#endif
		return -1;
	}
	/** Start the reading of the variable value */
	if (cgroup_read_value_begin(device_name, group_name, param, &handle, line,
			MAX_LINE_LEN)) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: read the %s value of %s in '%s failed'\n",
				device_name, param, group_name);
#endif
		return -1;
	}
	strncpy(value, line, strlen(line));
	/** End the reading.*/
	cgroup_read_value_end(&handle);
	return 0;
}

/*
 * Read the record from the given group of the device, the record just have
 * the value of parameter, so nothing to process, just convert the characters
 * into type of unsigned long long.
 *
 * IN:
 * @device_name:          The given device name.
 * @group_name:           The given group name.
 * @param:                The parameter.
 *
 * OUT:
 * @vaule:                The value of the parameter.
 *
 * RETURN
 * 0:                     Success.
 * -1:                    Failed.
 */
static int read_ull_value(const char* device_name, const char* group_name,
		const char* param, unsigned long long* value) {

	char str[LINE_LEN] = { 0 };
	if (!device_name || !group_name || !param || !value) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: the input parameters is invalid.\n");
#endif
		return -1;
	}

	if (read_record_value(device_name, group_name, param, str)) {
#ifdef DEBUG
		fprintf(stderr,
				"rd_stats: read the value of %s %s in group %s failed\n",
				device_name, param, group_name);
#endif
		return -1;
	}
	*value = (unsigned long long) atoll(str);
	return 0;
}

#ifdef CGROUP_BLKIO_STATS
/*
 * Read the device statistics information.
 *
 * IN:
 * @device_name:          The given device name.
 * @group_name:           The given group name.
 * @param:                The parameter.
 * @cg_stats_lists:       The lists of device statistics information.
 *
 * RETURN
 * 0:                     Success.
 * -1:                    Failed.
 */
static int read_device_stats(const char* device_name, const char* group_name,
		const char* param, cg_device_stats_lists* cg_stats_lists) {

	void* handle;
	char line[FN_LEN];
	int dev = 0, idx = 0;
	char key[LINE_LEN] = { 0 };
	long long value = -1;
	int len = 0;
	cg_device_stats* cg_stats = NULL;

	if (!device_name || !group_name || !param || !cg_stats_lists) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: the input parameters is invalid.\n");
#endif
		return -1;
	}
	/** Clear the history information.*/
	clear_device_stats(cg_stats_lists);
	/** Start the reading of the variable value */
	if (cgroup_read_value_begin(device_name, group_name, param, &handle, line,
			FN_LEN)) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: read the %s value of %s in '%s failed'\n",
				device_name, param, group_name);
#endif
		return -1;
	}
	/** Read the device number, key and the value, and then set the value to the parameter.*/
	do {
		sscanf(line, "%d:%d %s %llu", &dev, &idx, &key, &value);
		if (value < 0)
			break;
		/** If the device number is not exists, initialized the device statistics information
		 *  and add the device statistics information into the lists.*/
		if (get_device_stats(cg_stats_lists, &cg_stats, dev, idx)) {
			if (init_device_stats(&cg_stats, dev, idx)) {
#ifdef DEBUG
				fprintf(stderr,
						"rd_stats: initialized the device stats of %s %s in '%s failed'\n",
						device_name, param, group_name);
#endif
				goto err;
			}
			if (add_device_stats(cg_stats_lists, cg_stats)) {
#ifdef DEBUG
				fprintf(stderr,
						"rd_stats: add the device stats into the lists of %s %s in '%s failed'\n",
						device_name, param, group_name);
#endif
				goto err;
			}
		}
		len = strlen(key);
		/** Set the value to the corresponding parameter.*/
		if (!strncmp(key, BLKIO_READ, max(len,BLKIO_READ_LEN)))
			cg_stats->stats->read = value;
		else if (!strncmp(key, BLKIO_WRITE, max(len,BLKIO_WRITE_LEN)))
			cg_stats->stats->write = value;
		else if (!strncmp(key, BLKIO_SYNC, max(len,BLKIO_SYNC_LEN)))
			cg_stats->stats->sync = value;
		else if (!strncmp(key, BLKIO_ASYNC, max(len,BLKIO_ASYNC_LEN)))
			cg_stats->stats->async = value;
		else if (!strncmp(key, BLKIO_TOTAL, max(len,BLKIO_TOTAL_LEN)))
			cg_stats->stats->total = value;

	} while (!cgroup_read_value_next(&handle, line, FN_LEN));
	/** End the reading.*/
	cgroup_read_value_end(&handle);
	return 0;

	err: if (cg_stats_lists && !TAILQ_EMPTY(cg_stats_lists))
		clear_device_stats(cg_stats_lists);
	/** End the reading.*/
	cgroup_read_value_end(&handle);
	return 1;
}

/*
 * Read the device values.
 *
 * IN:
 * @device_name:          The given device name.
 * @group_name:           The given group name.
 * @param:                The parameter.
 * @cg_value_lists:       The lists of device values.
 *
 * RETURN
 * 0:                     Success.
 * -1:                    Failed.
 */
static int read_device_value(const char* device_name, const char* group_name,
		const char* param, cg_device_value_lists* cg_value_lists) {

	void* handle;
	char line[FN_LEN];
	int dev = 0, idx = 0;
	long long value = -1;
	int len = 0;
	cg_device_value* cg_value = NULL;

	if (!device_name || !group_name || !param || !cg_value_lists) {
		fprintf(stderr, "rd_stats: the input parameters is invalid.\n");
		return -1;
	}
	/** Clear the history information.*/
	clear_device_value(cg_value_lists);
	/** Start the reading of the variable value */
	if (cgroup_read_value_begin(device_name, group_name, param, &handle, line,
			FN_LEN)) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: read the %s value of %s in '%s failed'\n",
				device_name, param, group_name);
#endif
		return -1;
	}
	/** Read the device number and the value, then store the value.*/
	do {
		sscanf(line, "%d:%d %llu", &dev, &idx, &value);
		if (value < 0)
			break;
		/** If the device number is not exists, initialized the device value and
		 *  add the device value into the lists.*/
		if (get_device_value(cg_value_lists, &cg_value, dev, idx)) {
			if (init_device_value(&cg_value, dev, idx)) {
#ifdef DEBUG
				fprintf(stderr,
						"rd_stats: initialized the device value of %s %s in '%s failed'\n",
						device_name, param, group_name);
#endif
				goto err;
			}
			if (add_device_value(cg_value_lists, cg_value)) {
#ifdef DEBUG
				fprintf(stderr,
						"rd_stats: add the device value into the lists of %s %s in '%s failed'\n",
						device_name, param, group_name);
#endif
				goto err;
			}
		}
		cg_value->value = value;
	} while (!cgroup_read_value_next(&handle, line, FN_LEN));
	/** Skip the keyword of the parameter.*/

	/** End the reading.*/
	cgroup_read_value_end(&handle);
	return 0;

	err: if (cg_value_lists && !TAILQ_EMPTY(cg_value_lists))
		clear_device_value(cg_value_lists);
	/** End the reading.*/
	cgroup_read_value_end(&handle);
	return 1;
}
#endif

/*
 * Read the cpuset statistics information of given group name.
 *
 * IN:
 * @group_name :          The given group name.
 * @stats_lists:          The lists of cpuset statistics information.
 *
 * RETURN
 * 0                     Success.
 * -1                    Failed.
 */
int read_cg_cpu_stats(const char* group_name,
		cg_cpuset_stats_lists* stats_lists) {

	FILE *fp;
	int i = 0, j = 0, num = 0, len = 0;
	char* pre = NULL, *ptr = NULL, temp;
	char cpus[LINE_LEN] = { 0 };
	int cpus_id[LINE_LEN];
	char cpuset_name[FN_LEN] = { 0 };
	char line[MAX_LINE_LEN] = { 0 };
	cg_cpuset_stats* cg_stats = NULL;

	if (!group_name || !stats_lists) {
#ifdef DEBUG
		fprintf(stderr,
				"rd_stats: the group name or cpuset stats lists is null.\n");
#endif
		return -1;
	}
	/** Clear the history information.*/
	clear_cpuset_stats(stats_lists);
	/** Read the cpus of the group in cpuset. */
	if (read_cpuset_cpus(group_name,cpus)) {
#ifdef DEBUG
		fprintf(stderr,
				"rd_stats: read the value of cpusets in cpuset %s failed\n",
				group_name);
#endif
		return -1;
	}
	/** Get cpuset id from cpusets. If the format like 1-5, the cpuset id should be
	 *  completed. */
	while (cpus[i] != '\0' && i < LINE_LEN) {
		if (isdigit(cpus[i])) {
			pre = cpus + i;
			for (ptr = pre + 1; isdigit(*ptr); ptr++, i++)
				;
			temp = *ptr;
			*ptr = '\0';
			num = atoi(pre);
			*ptr = temp;
			if ((pre - 1) && (*(pre - 1) == '-')) {
				while (j > 0 && (cpus_id[j - 1] < num - 1)) {
					cpus_id[j] = cpus_id[j - 1] + 1;
					j++;
				}
			}
			cpus_id[j++] = num;
		}
		i++;
	}
	/** Open the /proc/stat to get the statistics information of cpuset id in cpusets.*/
	if (!(fp = fopen(STAT, "r"))) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: Cannot open %s\n", STAT);
#endif
		return -1;
	}
	/** Read the statistics information from /proc/stat and get the information in
	 *  cpuset id array, and initialized the cpuset stats to store the value of parameter.
	 *  Finally, add the cpuset stats into the lists of the cpusets stats.*/
	while (fgets(line, MAX_LINE_LEN, fp)) {

		for (i = 0; i < j; i++) {
			snprintf(cpuset_name, FN_LEN, "%s%d ", CG_CPU, cpus_id[i]);
			len = strlen(cpuset_name);
			if (!strncmp(line, cpuset_name, len)) {
				/** Initialized the cpuset stats.*/
				if (init_cpuset_stats(&cg_stats, cpus_id[i])) {
#ifdef DEBUG
					fprintf(stderr,
							"rd_stats: initialize the cpuset stats of group %s failed\n",
							group_name);
#endif
					goto err;
				}
				/** Store the value of the parameter.*/
				sscanf(line + len,
						"%llu %llu %llu %llu %llu %llu %llu %llu %llu",
						&cg_stats->stats->cpu_user, &cg_stats->stats->cpu_nice,
						&cg_stats->stats->cpu_sys, &cg_stats->stats->cpu_idle,
						&cg_stats->stats->cpu_iowait,
						&cg_stats->stats->cpu_hardirq,
						&cg_stats->stats->cpu_softirq,
						&cg_stats->stats->cpu_steal,
						&cg_stats->stats->cpu_guest);
				/** Add the cpusets statistics information into the lists.*/
				if (add_cpuset_stats(stats_lists, cg_stats)) {
#ifdef DEBUG
					fprintf(stderr,
							"rd_stats: add the cpuset stats into cpuset stats list of group %s failed\n",
							group_name);
#endif
					goto err;
				}
			}
		}
	}
	if (fp)
		fclose(fp);
	return 0;

	err: if (cg_stats)
		uninit_cpuset_stats(cg_stats);
	clear_cpuset_stats(stats_lists);
	if (fp)
		fclose(fp);
	return -1;
}

static int read_proc_io(struct io_stats* io_stats, pid_t pid) {

	FILE *fp = NULL;
	int i = 0, j = 0;
	char proc_name[FN_LEN] = { 0 };
	char line[MAX_LINE_LEN] = { 0 };

	snprintf(proc_name, FN_LEN, "/proc/%llu/io", pid);
	/** Open the /proc/<PID>/io.*/
	if (!(fp = fopen(proc_name, "r"))) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: Cannot open %s\n", proc_name);
#endif
		return -1;
	}
	/** Store the value of the parameter.*/
	while (fgets(line, LINE_LEN, fp)) {
		if (!strncmp(line, RCHAR, RCHAR_LEN))
			sscanf(line + RCHAR_LEN + 1, "%llu", &(io_stats->io_rchar));
		else if (!strncmp(line, WCHAR, WCHAR_LEN))
			sscanf(line + WCHAR_LEN + 1, "%llu", &(io_stats->io_wchar));
		else if (!strncmp(line, SYSCR, SYSCR_LEN))
			sscanf(line + SYSCR_LEN + 1, "%llu", &(io_stats->io_syscr));
		else if (!strncmp(line, SYSCW, SYSCW_LEN))
			sscanf(line + SYSCW_LEN + 1, "%llu", &(io_stats->io_syscw));
		else if (!strncmp(line, READ_BYTES, READ_BYTES_LEN))
			sscanf(line + READ_BYTES_LEN + 1, "%llu", &(io_stats->io_rbytes));
		else if (!strncmp(line, WRITE_BYTES, WRITE_BYTES_LEN))
			sscanf(line + WRITE_BYTES_LEN + 1, "%llu", &(io_stats->io_wbytes));
		else if (!strncmp(line, CANCELLED_WRITE_BYTES,
				CANCELLED_WRITE_BYTES_LEN))
			sscanf(line + CANCELLED_WRITE_BYTES_LEN + 1, "%llu",
					&(io_stats->io_cwbytes));
	}
	if (fp)
		fclose(fp);
	return 0;
}

#ifdef CGROUP_VMSTAT
static int read_proc_stat(struct proc_stat* proc_stat, pid_t pid) {

	FILE *fp = NULL;
	int i = 0, j = 0;
	char proc_name[FN_LEN] = { 0 };
	char line[MAX_LINE_LEN] = { 0 };

	snprintf(proc_name, FN_LEN, "/proc/%llu/stat", pid);
	/** Open the /proc/<PID>/io.*/
	if (!(fp = fopen(proc_name, "r"))) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: Cannot open %s\n", proc_name);
#endif
		return -1;
	}

	/** Store the value of the parameter.*/
	if (fgets(line, LINE_LEN, fp)) {
		sscanf(line, "%d %s %c %u %u %d %d %d %d %d %u %u %u %u %u %d "
				"%d %d %d %d %d  %u %u %d %u %u %u %u %u %u %u %u "
				"%d %d %d %d %u %d %d ", &(proc_stat->pid),
				&(proc_stat->exName), &(proc_stat->state), &(proc_stat->euid),
				&(proc_stat->egid), &(proc_stat->ppid), &(proc_stat->pgrp),
				&(proc_stat->session), &(proc_stat->tty), &(proc_stat->tpgid),
				&(proc_stat->flags), &(proc_stat->minflt),
				&(proc_stat->cminflt), &(proc_stat->majflt),
				&(proc_stat->cmajflt), &(proc_stat->utime), &(proc_stat->stime),
				&(proc_stat->cutime), &(proc_stat->cstime),
				&(proc_stat->counter), &(proc_stat->priority),
				&(proc_stat->timeout), &(proc_stat->itrealvalue),
				&(proc_stat->starttime), &(proc_stat->vsize), &(proc_stat->rss),
				&(proc_stat->rlim), &(proc_stat->startcode),
				&(proc_stat->endcode), &(proc_stat->startstack),
				&(proc_stat->kstkesp), &(proc_stat->kstkeip),
				&(proc_stat->signal), &(proc_stat->blocked),
				&(proc_stat->sigignore), &(proc_stat->sigcatch),
				&(proc_stat->wchan), &(proc_stat->sched),
				&(proc_stat->sched_priority));
	}
	if (fp)
		fclose(fp);
	return 0;
}
static int read_proc_statm(struct proc_statm* proc_statm, pid_t pid) {

	FILE *fp = NULL;
	int i = 0, j = 0;
	char proc_name[FN_LEN] = { 0 };
	char line[MAX_LINE_LEN] = { 0 };

	snprintf(proc_name, FN_LEN, "/proc/%llu/statm", pid);
	/** Open the /proc/<PID>/io.*/
	if (!(fp = fopen(proc_name, "r"))) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: Cannot open %s\n", proc_name);
#endif
		return -1;
	}

	/** Store the value of the parameter.*/
	if (fgets(line, LINE_LEN, fp)) {
		sscanf(line, "%llu %llu %llu %llu %llu %llu %llu", &(proc_statm->size),
				&(proc_statm->resident), &(proc_statm->shared),
				&(proc_statm->trs), &(proc_statm->lrs), &(proc_statm->drs),
				&(proc_statm->dt));
	}
	if (fp)
		fclose(fp);
	return 0;
}
#endif

/*
 * Read the proc statistics information of given group name.
 *
 * IN:
 * @group_name :          The given group name.
 * @stats_lists:          The lists of proc statistics information.
 *
 * RETURN
 * 0                     Success.
 * -1                    Failed.
 */
int read_cg_proc_stats(const char* group_name, cg_proc_stats_lists* stats_lists) {

	FILE *fp = NULL;
	int i = 0, j = 0, num = 0;
	char* pre = NULL;
	pid_t* procs;
	char proc_name[FN_LEN] = { 0 };
	char line[MAX_LINE_LEN] = { 0 };
	cg_proc_stats* cg_stats = NULL;

	if (!group_name || !stats_lists) {
#ifdef DEBUG
		fprintf(stderr,
				"rd_stats: the group name or io stats lists is null.\n");
#endif
		return -1;
	}
	/** Clear the history information.*/
	clear_proc_stats(stats_lists);
	/** Get all of the process id in the group. But the variables procs should
	 *  be free at the end of this procedure.*/
	if (cgroup_get_procs(group_name, CG_CPUSET, &procs, &j)) {
#ifdef DEBUG
		fprintf(stderr,
				"rd_stats: read the value of procs in cpuset %s failed\n",
				group_name);
#endif
		return -1;
	}
	/** Read the io statistics information of each process id(/proc/<PID>/io).
	 *  Initialized the io statistics information and store the value of the
	 *  parameter. Finally add the io statistics information into the lists.*/
	for (i = 0; i < j; i++) {
		/** Initialized the io statistics information.*/
		if (init_proc_stats(&cg_stats, procs[i])) {
			fprintf(stderr, "rd_stats: initialize the io stats failed\n");
			goto err;
		}
		if (read_proc_io(cg_stats->io_stats, procs[i])
				|| read_proc_stat(cg_stats->proc_stat, procs[i])
				|| read_proc_statm(cg_stats->proc_statm, procs[i])) {
#ifdef DEBUG
			fprintf(stderr, "rd_stats: read the proc io stats, proc"
					" stat and proc statm into io stats list failed\n");
#endif
			goto err;
		}
		/** Add the io statistics information into the lists.*/
		if (add_proc_stats(stats_lists, cg_stats)) {
#ifdef DEBUG
			fprintf(stderr,
					"rd_stats: add the io stats into io stats list failed\n");
#endif
			goto err;
		}
	}
	if (procs)
		free(procs);
	return 0;

	err: if (cg_stats)
		uninit_proc_stats(cg_stats);
	clear_proc_stats(stats_lists);
	if (procs)
		free(procs);
	if (fp)
		fclose(fp);
	return -1;
}

#ifdef CGROUP_MEMORY_STATS
/*
 * Read the memory statistics information of given group name.
 *
 * IN:
 * @group_name :          The given group name.
 * @stats:                The memory statistics information.
 *
 * RETURN
 * 0                     Success.
 * -1                    Failed.
 */
static int read_mem_stats(const char*group_name, struct mem_stats* stats) {

	void *handle = NULL;
	char line[FN_LEN] = { 0 };
	/** Clear the history information.*/
	memset(stats, 0, sizeof(struct mem_stats));
	/** Start the reading of the variable value */
	if (cgroup_read_value_begin(CG_MEMORY, group_name, MEMORY_STATS, &handle,
			line, FN_LEN)) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: read the %s value of %s in '%s failed'\n",
				CG_MEMORY, MEMORY_STATS, group_name);
#endif
		return -1;
	}
	/** Read the device number and the value, then store the value.*/
	do {
		/** Store the value of the parameter.*/
		if (!strncmp(line, CACHE, CACHE_LEN))
			sscanf(line + CACHE_LEN + 1, "%ll", &(stats->cache));
		else if (!strncmp(line, RSS, RSS_LEN))
			sscanf(line + RSS_LEN + 1, "%llu", &(stats->rss));
		else if (!strncmp(line, MAPPED_FILE, MAPPED_FILE_LEN))
			sscanf(line + MAPPED_FILE_LEN + 1, "%llu", &(stats->mapped_file));
		else if (!strncmp(line, PGPGIN, PGPGIN_LEN))
			sscanf(line + PGPGIN_LEN + 1, "%llu", &(stats->pgpgin));
		else if (!strncmp(line, PGPGOUT, PGPGOUT_LEN))
			sscanf(line + PGPGOUT_LEN + 1, "%llu", &(stats->pgpgout));
		else if (!strncmp(line, SWAP, SWAP_LEN))
			sscanf(line + SWAP_LEN + 1, "%llu", &(stats->swap));
		else if (!strncmp(line, INACTIVE_ANON, INACTIVE_ANON_LEN))
			sscanf(line + INACTIVE_ANON_LEN + 1, "%llu",
					&(stats->inactive_anon));
		else if (!strncmp(line, ACTIVE_ANON, ACTIVE_ANON_LEN))
			sscanf(line + ACTIVE_ANON_LEN + 1, "%llu", &(stats->active_anon));
		else if (!strncmp(line, INACTIVE_FILE, INACTIVE_FILE_LEN))
			sscanf(line + INACTIVE_FILE_LEN + 1, "%llu",
					&(stats->inactive_file));
		else if (!strncmp(line, ACTIVE_FILE, ACTIVE_FILE_LEN))
			sscanf(line + ACTIVE_FILE_LEN + 1, "%llu", &(stats->active_file));
		else if (!strncmp(line, UNEVICTABLE, UNEVICTABLE_LEN))
			sscanf(line + UNEVICTABLE_LEN + 1, "%llu", &(stats->unevictable));
		else if (!strncmp(line, HIERARCHICAL_MEMORY_LIMIT,
				HIERARCHICAL_MEMORY_LIMIT_LEN))
			sscanf(line + HIERARCHICAL_MEMORY_LIMIT_LEN + 1, "%llu",
					&(stats->hierarchical_memory_limit));
		else if (!strncmp(line, HIERARCHICAL_MEMSW_LIMIT,
				HIERARCHICAL_MEMSW_LIMIT_LEN))
			sscanf(line + HIERARCHICAL_MEMSW_LIMIT_LEN + 1, "%llu",
					&(stats->hierarchical_memsw_limit));
		else if (!strncmp(line, TOTAL_CACHE, TOTAL_CACHE_LEN))
			sscanf(line + TOTAL_CACHE_LEN + 1, "%llu", &(stats->total_cache));
		else if (!strncmp(line, TOTAL_RSS, TOTAL_RSS_LEN))
			sscanf(line + TOTAL_RSS_LEN + 1, "%llu", &(stats->total_rss));
		else if (!strncmp(line, TOTAL_MAPPED_FILE, TOTAL_MAPPED_FILE_LEN))
			sscanf(line + TOTAL_MAPPED_FILE_LEN + 1, "%llu",
					&(stats->total_mapped_file));
		else if (!strncmp(line, TOTAL_PGPGIN, TOTAL_PGPGIN_LEN))
			sscanf(line + TOTAL_PGPGIN_LEN + 1, "%llu", &(stats->total_pgpgin));
		else if (!strncmp(line, TOTAL_PGPGOUT, TOTAL_PGPGOUT_LEN))
			sscanf(line + TOTAL_PGPGOUT_LEN + 1, "%llu",
					&(stats->total_pgpgout));
		else if (!strncmp(line, TOTAL_SWAP, TOTAL_SWAP_LEN))
			sscanf(line + TOTAL_SWAP_LEN + 1, "%llu", &(stats->total_swap));
		else if (!strncmp(line, TOTAL_INACTIVE_ANON, TOTAL_INACTIVE_ANON_LEN))
			sscanf(line + TOTAL_INACTIVE_ANON_LEN + 1, "%llu",
					&(stats->total_inactive_anon));
		else if (!strncmp(line, TOTAL_ACTIVE_ANON, TOTAL_ACTIVE_ANON_LEN))
			sscanf(line + TOTAL_ACTIVE_ANON_LEN + 1, "%llu",
					&(stats->total_active_anon));
		else if (!strncmp(line, TOTAL_INACTIVE_FILE, TOTAL_INACTIVE_FILE_LEN))
			sscanf(line + TOTAL_INACTIVE_FILE_LEN + 1, "%llu",
					&(stats->total_inactive_file));
		else if (!strncmp(line, TOTAL_ACTIVE_FILE, TOTAL_ACTIVE_FILE_LEN))
			sscanf(line + TOTAL_ACTIVE_FILE_LEN + 1, "%llu",
					&(stats->total_active_file));
		else if (!strncmp(line, TOTAL_UNEVICTABLE, TOTAL_UNEVICTABLE_LEN))
			sscanf(line + TOTAL_UNEVICTABLE_LEN + 1, "%llu",
					&(stats->total_unevictable));
	} while (!cgroup_read_value_next(&handle, line, FN_LEN));
	/** Skip the keyword of the parameter.*/

	/** End the reading.*/
	cgroup_read_value_end(&handle);
	return 0;
}

/*
 * Read the cgroup memory statistics information of given group name.
 *
 * IN:
 * @group_name :          The given group name.
 * @cg_stats:             The memory statistics information.
 *
 * RETURN
 * 0                     Success.
 * -1                    Failed.
 */
int read_cg_mem_stats(const char* group_name, cg_mem_stats* cg_stats) {

	if (!group_name || !cg_stats) {
		fprintf(stderr, "rd_stats: the group name or memory stats is null.\n");
		return -1;
	}
	read_mem_stats(group_name, cg_stats->stats);
	read_mem_usage(group_name, &(cg_stats->usage));
	read_mem_swap_usage(group_name, &(cg_stats->mem_swap_usage));
	read_mem_max_usage(group_name, &(cg_stats->max_usage));
	read_mem_swap_max_usage(group_name, &(cg_stats->mem_swap_max_usage));
	read_mem_limit(group_name, &(cg_stats->limit));
	read_mem_swap_limit(group_name, &(cg_stats->mem_swap_limit));
	return 0;
}
#endif

#ifdef CGROUP_BLKIO_STATS
/*
 * Read the cgroup io statistics information of given group name.
 *
 * IN:
 * @group_name :          The given group name.
 * @cg_stats:             The memory statistics information.
 *
 * RETURN
 * 0                     Success.
 * -1                    Failed.
 */
int read_cg_io_stats(const char* group_name, cg_blkio_stats* cg_stats) {

	read_io_weight(group_name, &(cg_stats->weight));
	read_io_weight_device(group_name, &(cg_stats->weight_device));

	read_io_service_bytes(group_name, cg_stats->io_service_bytes);
	read_io_serviced(group_name, cg_stats->io_serviced);
	read_io_service_time(group_name, cg_stats->io_service_time);
	read_io_wait_time(group_name, cg_stats->io_wait_time);
	read_io_merged(group_name, cg_stats->io_merged);
	read_io_queued(group_name, cg_stats->io_queued);
	read_io_throttle_io_service_bytes(group_name,
			cg_stats->throttle_io_service_bytes);
	read_io_throttle_io_serviced(group_name, cg_stats->throttle_io_serviced);

	read_io_throttle_read_bps(group_name, cg_stats->throttle_read_bps_device);
	read_io_throttle_write_bps(group_name, cg_stats->throttle_write_bps_device);
	read_io_throttle_read_iops(group_name, cg_stats->throttle_read_iops_device);
	read_io_throttle_write_iops(group_name,
			cg_stats->throttle_write_iops_device);
	read_io_time(group_name, cg_stats->time);
	read_io_sectors(group_name, cg_stats->sectors);
	return 0;
}
#endif

/*
 * Read all of the groups statistics information.
 *
 * IN:
 * @groups:                The lists of groups.
 * @device_name:           The given device name.
 */
int read_all_groups(cg_group_lists* groups, const char* device_name) {

	int ret = 0, level;
	void *handle;
	struct cgroup_file_info info;
	cg_group* group = NULL;
	if (!groups || !device_name) {
		fprintf(stderr, "rd_stats: the input parameters is invalid.\n");
		return -1;
	}
	/** Clear the history information.*/
	clear_group(groups);
	/** Start the traversing of the directory. */
	ret = cgroup_walk_tree_begin(device_name, BASE_DIR, DEPTH, &handle, &info,
			&level);
	if (ret)
		goto err;
	/** Walk to the next. */
	while (!(ret = cgroup_walk_tree_next(0, &handle, &info, level))) {
		if (info.type == CGROUP_FILE_TYPE_DIR) {
			/** Initialized the group but the cpuset statistics information and the io
			 *  statistics information is NULL.*/
			if ((ret = init_group(&group, info.path, info.full_path))) {
#ifdef DEBUG
				fprintf(stderr,
						"rd_stats: initialize the group failed, group name: %s, path: %s.\n",
						info.path, info.full_path);
#endif
				goto err;
			}
			/** Add the group into the lists.*/
			if (group && add_group(groups, group)) {
#ifdef DEBUG
				fprintf(stderr,
						"rd_stats: add the group into lists failed, group name: %s, path: %s.\n",
						info.path, info.full_path);
#endif
				goto err;
			}
			group = NULL;
		}
	}
	/** End the traversing.*/
	cgroup_walk_tree_end(&handle);
	return 0;

	err: if (group)
		uninit_group(group);
	group = NULL;
	clear_group(groups);
	cgroup_walk_tree_end(&handle);
	return ret;
}

static char device_array[MAX_DEVICE_NUM][MAX_DEVICE_NUM][LINE_LEN] = { 0 };
/** Read the device information.*/
int read_device() {
	FILE *fp;
	int dev = 0, idx = 0;
	char name[LINE_LEN] = { 0 };
	char line[FN_LEN] = { 0 };

	/** Open the /proc/diskstat to get the statistics information of cpuset id in cpusets.*/
	if (!(fp = fopen(DISKSTAT, "r"))) {
#ifdef DEBUG
		fprintf(stderr, "rd_stats: Cannot open %s\n", STAT);
#endif
		return -1;
	}
	/** Read the statistics information from /proc/stat and get the information in
	 *  cpuset id array, and initialized the cpuset stats to store the value of parameter.
	 *  Finally, add the cpuset stats into the lists of the cpusets stats.*/
	while (fgets(line, FN_LEN, fp)) {
		sscanf(line, "%d %d %s", &dev, &idx, &name);
		if (*name != '\0')
			strncpy(device_array[dev][idx], name, LINE_LEN);
	}
	if (fp)
		fclose(fp);
	return 0;
}

/*
 * Get the device name of given device number and index.
 *
 * IN:
 * @dev:                  The device number.
 * @idx:                  The device index.
 *
 * OUT:
 * @name:                 The device name.
 *
 * RETURN
 * @The pointer to name.
 */
char* get_device(const int dev, const int idx, char* name) {

	strcpy(name, device_array[dev][idx]);
	return name;
}

/*
 * Get the current localtime.
 * OUT:
 * @rectime:          The current time.
 *
 * RETURN:
 * The current time.
 */
time_t get_localtime(struct tm *rectime) {
	time_t timer;
	struct tm *ltm;

	time(&timer);
	ltm = localtime(&timer);

	*rectime = *ltm;
	return timer;
}

/*
 * Get the HZ of system.
 */
unsigned int hz;
void get_HZ(void) {
	long ticks;

	if ((ticks = sysconf(_SC_CLK_TCK)) == -1) {
		perror("sysconf");
	}
	hz = (unsigned int) ticks;
}

