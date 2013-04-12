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

#ifndef __RD_STATS_H
#define __RD_STATS_H

/*
 * The masks of display. If have other display options, just add the masks
 * before I_D_ALL mask.
 */
#define I_D_CPU       0x00000001
#define I_D_GROUP     0x00000002
#define I_D_TIMESTAMP 0x00000004
#define I_D_BYTES     0x00000008
#define I_D_KILOBYTES 0x00000010
#define I_D_MEGABYTES 0x00000020
#define I_D_GIGABYTES 0x00000040
#define I_D_EXTENDED  0x00000080
#ifdef CGROUP_MEMORY_STATS
#define I_D_TOTAL     0x00000100
#define I_D_OLD       0x00000200
#endif
#ifdef CGROUP_VMSTAT
#define I_D_ACTIVE    0x00000400
#define I_D_UNHEADER    0x00000800
#endif
#define I_D_ALL       0xffffffff

/*
 * Based the display masks to check the display option.
 */
#define DISPLAY_CPU(m)	        (((m) & I_D_CPU) == I_D_CPU)
#define DISPLAY_TIMESTAMP(m)	(((m) & I_D_TIMESTAMP) == I_D_TIMESTAMP)
#define DISPLAY_BYTES(m)        (((m) & I_D_BYTES) == I_D_BYTES)
#define DISPLAY_KILOBYTES(m)	(((m) & I_D_KILOBYTES) == I_D_KILOBYTES)
#define DISPLAY_MEGABYTES(m)	(((m) & I_D_MEGABYTES) == I_D_MEGABYTES)
#define DISPLAY_GIGABYTES(m)    (((m) & I_D_GIGABYTES) == I_D_GIGABYTES)
#define DISPLAY_EXTENDED(m)	    (((m) & I_D_EXTENDED) == I_D_EXTENDED)
#ifdef CGROUP_MEMORY_STATS
#define DISPLAY_TOTAL(m)        (((m) & I_D_TOTAL) == I_D_TOTAL)
#define DISPLAY_OLD(m)          (((m) & I_D_OLD) == I_D_OLD)
#endif
#ifdef CGROUP_VMSTAT
#define DISPLAY_ACTIVE(m)       (((m) & I_D_ACTIVE) == I_D_ACTIVE)
#define DISPLAY_UNHEADER(m)       (((m) & I_D_UNHEADER) == I_D_UNHEADER)
#endif

/*
 * Some constant value.
 */
#define DIGITS		"0123456789"
#define K_ALL       "*"

#ifndef BASE_DIR
#define BASE_DIR "/"
#endif

#ifndef DEPTH
#define DEPTH 0
#endif
/*
 * Some limitation for cgroup monitor.
 */
/** The max group count.*/
#ifndef GROUP_COUNT_MAX
#define GROUP_COUNT_MAX 16
#endif
/** The max cgroup hierarchical.*/
#ifndef CG_HIER_MAX
#define CG_HIER_MAX 100
#endif
/** The length of a line.*/
#ifndef LINE_LEN
#define LINE_LEN CG_HIER_MAX
#endif
/** The max length of a line.*/
#ifndef MAX_LINE_LEN
#define MAX_LINE_LEN 8192
#endif
/** The max length of file name.*/
#ifndef FN_LEN
#define FN_LEN 255
#endif
/** The max device number.*/
#ifndef MAX_DEVICE_NUM
#define MAX_DEVICE_NUM 16
#endif

/*
 * The macro for cgroup. Some of them are not used but reserved in order to
 * display more perfect monitoring information in the future.
 */
/** The cpuset keyword and the length of keyword.*/
#define CG_CPU "cpu"
#define CG_CPU_LEN 3
/** The cpuset keyword and the length of keyword.*/
#define CG_CPUSET "cpuset"
#define CG_CPUSET_LEN 6
/** The cpusetacct keyword and the length of keyword.*/
#define CG_CPUACCT "cpuacct"
#define CG_CPUACCT_LEN 7
/** The blkio keyword and the length of keyword.*/
#define CG_BLKIO "blkio"
#define CG_BLKIO_LEN 5
/** The memory keyword and the length of keyword.*/
#define CG_MEMORY "memory"
#define CG_MEMORY_LEN 6
/** The devices keyword and the length of keyword.*/
#define CG_DEVICES "devices"
#define CG_DEVICES_LEN 7
/** The freezer keyword and the length of keyword.*/
#define CG_FREEZER "freezer"
#define CG_FREEZER_LEN 7
/** The net_cls keyword and the length of keyword.*/
#define CG_NET_CLS "net_cls"
#define CG_NET_CLS_LEN 7

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
		cg_cpuset_stats_lists* stats_lists);

#ifdef CGROUP_MEMORY_STATS
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
int read_cg_mem_stats(const char* group_name, cg_mem_stats* cg_stats);
#endif

#ifdef CGROUP_PROC_STAT
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
int read_cg_proc_stats(const char* group_name,
		cg_proc_stats_lists* stats_lists);
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
int read_cg_io_stats(const char* group_name, cg_blkio_stats* cg_stats);
#endif

/*
 * Read all of the groups statistics information.
 *
 * IN:
 * @groups:                The lists of groups.
 * @device_name:           The given device name.
 */
int read_all_groups(cg_group_lists* groups, const char* device_name);

#define HZ		hz
extern unsigned int hz;
/*
 * Get the HZ of system.
 */
void get_HZ(void);

/*
 * Get the current localtime.
 * OUT:
 * @rectime:          The current time.
 *
 * RETURN:
 * The current time.
 */
time_t get_localtime(struct tm *rectime);
#endif

