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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/utsname.h>
#include <libcgroup.h>

#include <cgsysstat.h>
#include "rd_stats.h"
#include "compute.h"

static long interval = 0;
static char group_name[LINE_LEN][LINE_LEN] = { 0 };
cg_group_lists *io_groups = NULL;
static struct io_stats* total_io_stats = NULL;
static struct cpuset_stats* avg_cpuset_stats = NULL;

/*
 * The alarm handler to trigger the alarm every interval seconds.
 */
void alarm_handler(int sig) {
	signal(SIGALRM, alarm_handler);
	alarm(interval);
}

/*
 * Write the full cpuset statistic information in the lists.
 *
 * IN:
 * @cpuset_stats_lists:              The array lists of the cpuset statistics information.
 * @cur:                          The index of array.
 */
void write_full_cpusets_stats(cg_cpuset_stats_lists ** cpuset_stats_lists,
		int cur) {

	unsigned long long itv = 0;
	cg_cpuset_stats* ptr_cpuset_stats = NULL;
	cg_cpuset_stats* old_cpuset_stats = NULL;
	printf(
			"cpu_id:   %%user     %%nice   %%system   %%iowait    %%steal     %%idle\n");
	if (!cpuset_stats_lists[cur]->count)
		printf("       no cpusets\n\n");
	TAILQ_FOREACH(ptr_cpuset_stats,cpuset_stats_lists[cur],next)
	{
		if (!get_cpuset_stats(cpuset_stats_lists[!cur], &old_cpuset_stats,
				ptr_cpuset_stats->id)) {
			itv = get_interval(ptr_cpuset_stats, old_cpuset_stats);
			printf("cpu%d  :  %6.2f    %6.2f    %6.2f  %8.2f  %8.2f  %8.2f\n\n",
					ptr_cpuset_stats->id,
					((double) ll_value(old_cpuset_stats->stats->cpu_user,
							ptr_cpuset_stats->stats->cpu_user)) / itv * 100,
					((double) ll_value(old_cpuset_stats->stats->cpu_nice,
							ptr_cpuset_stats->stats->cpu_nice)) / itv * 100,
					((double) ll_value(
							old_cpuset_stats->stats->cpu_sys
									+ old_cpuset_stats->stats->cpu_softirq
									+ old_cpuset_stats->stats->cpu_hardirq,
							ptr_cpuset_stats->stats->cpu_sys
									+ ptr_cpuset_stats->stats->cpu_softirq
									+ ptr_cpuset_stats->stats->cpu_hardirq))
							/ itv * 100,
					((double) ll_value(old_cpuset_stats->stats->cpu_iowait,
							ptr_cpuset_stats->stats->cpu_iowait)) / itv * 100,
					((double) ll_value(old_cpuset_stats->stats->cpu_steal,
							ptr_cpuset_stats->stats->cpu_steal)) / itv * 100,
					(ptr_cpuset_stats->stats->cpu_idle
							< old_cpuset_stats->stats->cpu_idle) ?
							0.0 :
							((double) ll_value(
									old_cpuset_stats->stats->cpu_idle,
									ptr_cpuset_stats->stats->cpu_idle)) / itv
									* 100);
		} else {
			itv = get_interval(ptr_cpuset_stats, NULL );
			printf("cpu%d  :  %6.2f    %6.2f    %6.2f  %8.2f  %8.2f  %8.2f\n\n",
					ptr_cpuset_stats->id,
					((double) ll_value(0, ptr_cpuset_stats->stats->cpu_user))
							/ itv * 100,
					((double) ll_value(0, ptr_cpuset_stats->stats->cpu_nice))
							/ itv * 100,
					((double) ll_value(0,
							ptr_cpuset_stats->stats->cpu_sys
									+ ptr_cpuset_stats->stats->cpu_softirq
									+ ptr_cpuset_stats->stats->cpu_hardirq))
							/ itv * 100,
					((double) ll_value(0, ptr_cpuset_stats->stats->cpu_iowait))
							/ itv * 100, 100.00);
		}
	}
}

/*
 * Write the average cpuset statistic information in the lists.
 *
 * IN:
 * @cpuset_stats_lists:              The array lists of the cpuset statistics information.
 * @cur:                          The index of array.
 * @itv:                          The total interval of the cpuset statistics information lists.
 */
void write_avg_cpuset_stats(cg_cpuset_stats_lists ** cpuset_stats_lists,
		int cur, long itv) {

	int count = cpuset_stats_lists[cur]->count;
	memset(avg_cpuset_stats, 0, sizeof(struct cpuset_stats));
	compute_avg_cpuset_stats(cpuset_stats_lists, avg_cpuset_stats, cur);
	printf(
			"avg_cpu: %%user     %%nice   %%system   %%iowait    %%steal     %%idle\n");
	printf("      %d %6.2f    %6.2f    %6.2f  %8.2f  %8.2f  %8.2f\n\n", count,
			itv ? ((((double) avg_cpuset_stats->cpu_user) / itv) * 100) : 0.00,
			itv ? ((((double) avg_cpuset_stats->cpu_nice) / itv) * 100) : 0.00,
			itv ? ((((double) (avg_cpuset_stats->cpu_sys
					+ avg_cpuset_stats->cpu_hardirq
					+ avg_cpuset_stats->cpu_softirq)) / itv) * 100) :
					0.00,
			itv ? ((((double) avg_cpuset_stats->cpu_iowait) / itv) * 100) : 0.00,
			itv ? ((((double) avg_cpuset_stats->cpu_steal) / itv) * 100) : 0.00,
			itv ? ((((double) avg_cpuset_stats->cpu_idle) / itv) * 100) : 100.00);
}

/*
 * Write the header of io statistic information.
 *
 * IN:
 * @flag:               The display flag.
 *
 * OUT:
 * unit:                The display unit.
 */
void write_io_stats_header(int flag, int* unit) {

	if (DISPLAY_EXTENDED(flag)) {
		printf("io_stat: pids     rrq/s     wrq/s       r/s       w/s");
		if (DISPLAY_KILOBYTES(flag)) {
			printf("     rKB/s     wKB/s       KB_rlimit        KB_wlimit");
			*unit = 1024;
		} else if (DISPLAY_MEGABYTES(flag)) {
			printf("     rMB/s     wMB/s       MB_rlimit        MB_wlimit");
			*unit = 1024 * 1024;
		} else {
			printf("      rB/s      wB/s        B_rlimit         B_wlimit");
			*unit = 1;
		}
		printf("     %%rutil    %%wutil\n");
	} else {
		printf("io_stat: pids       tps");
		if (DISPLAY_KILOBYTES(flag)) {
			printf(
					"     rKB/s     wKB/s       KB_read       KB_wrtn      KB_rlimit      KB_wlimit");
			*unit = 1024;
		} else if (DISPLAY_MEGABYTES(flag)) {
			printf(
					"     rMB/s     wMB/s       MB_read       MB_wrtn      MB_rlimit      MB_wlimit");
			*unit = 1024 * 1024;
		} else {
			printf(
					"      rB/s      wB/s        B_read        B_wrtn       B_rlimit       B_wlimit");
			*unit = 1;
		}
		printf("     %%util\n");
	}
}

/*
 * Write the io statistic information in the lists.
 *
 * IN:
 * @io_stats_lists:               The array lists of the io statistics information.
 * @cur:                          The index of array.
 * @itv:                          The total interval of the io statistics information lists.
 * @flag:                         The display flag.
 * @read_bps:                     The read limitation.
 * @write_bps:                    The write limitation.
 */
void write_io_stats(cg_proc_stats_lists** proc_stats_lists, int cur, long itv,
		int flag, unsigned long long read_bps, unsigned long long write_bps) {

	int unit = 0;
	double temp1, temp2;
	struct io_stat* stats = NULL;
	memset(total_io_stats, 0, sizeof(struct io_stats));
	compute_total_io_stats(proc_stats_lists, total_io_stats, cur);
	write_io_stats_header(flag, &unit);
	if (DISPLAY_EXTENDED(flag)) {
		/* pids  rrq/s  wrq/s  r/s   w/s    rKB/s  wKB/s  read_bps  write_bps  %rutil  %wutil*/
		printf(
				"%13d  %8.2f  %8.2f  %8.2f  %8.2f  %8.2f  %8.2f    %12.2f     %12.2f   %8.2f  %8.2f\n",
				proc_stats_lists[cur]->count,
				((double) total_io_stats->io_syscr) / itv * HZ,
				((double) total_io_stats->io_syscw) / itv * HZ,
				(((double) total_io_stats->io_rchar) / unit) / itv * HZ,
				(((double) total_io_stats->io_wchar) / unit) / itv * HZ,
				(((double) total_io_stats->io_rbytes) / unit) / itv * HZ,
				(((double) total_io_stats->io_wbytes) / unit) / itv * HZ,
				((double) read_bps) / unit, ((double) write_bps) / unit,
				((temp1 = ((double) total_io_stats->io_rbytes)
						/ (read_bps ? read_bps : 1)) > 1) ? 100.00 : temp1,
				((temp2 = ((double) total_io_stats->io_wbytes)
						/ (write_bps ? write_bps : 1)) > 1) ? 100.00 : temp2);
	} else {
		temp1 = ((double) total_io_stats->io_rbytes)
				/ (read_bps ? read_bps : 1);
		temp2 = ((double) total_io_stats->io_wbytes)
				/ (write_bps ? write_bps : 1);

		/*pids  tps   rKB/s  wKB/s  KB_read  KB_wrtn  read_bps  write_bps  %util*/
		printf(
				"%13d  %8.2f  %8.2f  %8.2f  %12.0f  %12.0f   %12.2f   %12.2f  %8.2f\n",
				proc_stats_lists[cur]->count,
				((double) (total_io_stats->io_syscr + total_io_stats->io_syscw))
						/ itv * HZ,
				(((double) total_io_stats->io_rbytes) / unit) / itv * HZ,
				(((double) total_io_stats->io_wbytes) / unit) / itv * HZ,
				(((double) total_io_stats->io_rbytes) / unit),
				(((double) total_io_stats->io_wbytes) / unit),
				((double) read_bps) / unit, ((double) write_bps) / unit,
				((temp1 > 1 ? 100.00 : temp1) + (temp2 > 1 ? 100.00 : temp2))
						/ 2);
	}
}

/*
 * Uninitialized the statistics information.
 */
void uninit_stats() {

	uninit_group_lists(io_groups);
	if (avg_cpuset_stats)
		free(avg_cpuset_stats);
	if (total_io_stats)
		free(total_io_stats);
}

/*
 * Initialized the statistics information.
 */
int init_stats() {

	int i = 0;
	/* Initialize the cgroup. */
	if (cgroup_init()) {
		fprintf(stderr, "cgiostat: initialize the cgroup failed.\n");
		printf("error: Initialize the cgroup failed.\n");
		uninit_stats();
		return -1;
	}
	if (init_group_lists(&io_groups)) {
		fprintf(stderr, "cgiostat: initialize the io group lists failed.\n",
				CG_BLKIO);
		printf("error: Initialize the io group lists failed.\n");
		uninit_stats();
		return -1;
	}
	if (!(avg_cpuset_stats = (struct cpuset_stats*) malloc(
			sizeof(struct cpuset_stats)))) {
		fprintf(stderr,
				"cgiostat: malloc the result of cpuset stats failed.\n");
		printf("error: Malloc the result of cpuset stats failed.\n");
		uninit_stats();
		return -1;
	}
	memset(avg_cpuset_stats, 0, sizeof(struct cpuset_stats));
	if (!(total_io_stats = (struct io_stats*) malloc(sizeof(struct io_stats)))) {
		fprintf(stderr, "cgiostat: malloc the total of io stats failed.\n");
		printf("error: Malloc the total of io stats failed.\n");
		uninit_stats();
		return -1;
	}
	memset(total_io_stats, 0, sizeof(struct io_stats));
	return 0;
}

/*
 * The loop process procedure.
 *
 * IN:
 * @count:               Loop count.
 * @flag :               Display flag.
 * @group_count:         The count of the groups.
 * @rectime:             The current local time.
 */
void rw_stats_loop(long count, int flag, int group_count, struct tm *rectime) {

	int i = 0, cur = 0, cpuset_count = 0;
	unsigned long long itv = 0;
	char timestamp[64];
	cg_group* io_group = NULL;
	unsigned long long read_bps, write_bps;
	setbuf(stdout, NULL );
	if (group_count > GROUP_COUNT_MAX)
		group_count = GROUP_COUNT_MAX;
	do {
		/* Get time */
		get_localtime(rectime);
		if (DISPLAY_TIMESTAMP(flag)) {
			strftime(timestamp, sizeof(timestamp), "%x %X", rectime);
			printf("%s\n", timestamp);
		}
		/** Display all of the groups statistics information. */
		for (i = 0; i < group_count; i++) {
			printf("------ %s ------\n", group_name[i]);
			if (group_name[i] && !TAILQ_EMPTY(io_groups)
					&& !get_group(io_groups, &io_group, group_name[i])) {
				if (read_cg_cpu_stats(group_name[i],
						io_group->cpuset_stats[cur])
						|| read_cg_io_stats(group_name[i],
								io_group->blkio_stats[cur])
						|| read_cg_proc_stats(group_name[i],
								io_group->proc_stats[cur])) {
					fprintf(stderr,
							"cgiostat: read the cpu and io stats of group: %s failed.\n",
							group_name[i]);
					printf(
							"error: Read the cpu and io stats of group: %s failed.\n",
							group_name[i]);
					return;
				}
				itv = get_total_interval(io_group->cpuset_stats[cur],
						io_group->cpuset_stats[!cur]);
				if (DISPLAY_CPU(flag)) {
					if (DISPLAY_EXTENDED(flag)) {
						write_full_cpusets_stats(io_group->cpuset_stats, cur);
					} else {
						write_avg_cpuset_stats(io_group->cpuset_stats, cur,
								itv);
					}
				}
				read_bps =
						io_group->blkio_stats[cur]->throttle_read_bps_device->count ?
								io_group->blkio_stats[cur]->throttle_read_bps_device->tqh_first->value :
								0;
				write_bps =
						io_group->blkio_stats[cur]->throttle_read_bps_device->count ?
								io_group->blkio_stats[cur]->throttle_read_bps_device->tqh_first->value :
								0;
				itv = get_interval(io_group->cpuset_stats[cur]->tqh_first,
						io_group->cpuset_stats[!cur]->tqh_first);
				write_io_stats(io_group->proc_stats, cur, itv, flag, read_bps,
						write_bps);
			}
		}
		if (count > 0) {
			count--;
		}
		if (count) {
			cur ^= 1;
			pause();
		}
	} while (count);

}

/*
 * Print the usage of the cgiostat.
 *
 * IN:
 * @status:             The status of program.
 * @program_name:       The program name.
 */
static void usage(int status, const char *program_name) {
	if (status != 0) {
		fprintf(stderr, "Wrong input parameters,"
				" try %s -h' for more information.\n", program_name);
	} else {
		fprintf(stdout, "Usage: %s [-h] [[-c] [-g] <group_name>] [-t] [-k] [-m]"
				"INTERVEL COUNT\n", program_name);
		fprintf(stdout,
				"  -c                Display utilization of cpuset. \n");
		fprintf(stdout, "  -g <group1,...>   Display the io utilization of \n"
				"                    group named <group_name>. \n");
		fprintf(stdout, "  -t                Display the timestamp \n");
		fprintf(stdout, "  -k                Display the status in KB/s. \n");
		fprintf(stdout, "  -m                Display the status in MB/s. \n");
		fprintf(stdout, "  -h                Display this help. \n");
		fprintf(stdout,
				"  -x                Display the extend information. \n");
		fprintf(stdout, "  INTERVEL COUNT    Display the information COUNT \n"
				"                    times in each INTERVEL seconds.\n");
		fprintf(stdout, "  -V                Display the version.\n");
		fprintf(stdout,
				"If not given the -g option, list all cgroups information\n");
	}
}

/*
 * Show the version of cgiostat.
 */
static void show_version() {

	printf("cgiostat version 1.0\n");
}

/*
 * The main function.
 */
int main(int argc, char *argv[]) {
	int ret = 0, it = 0, opt = 1;
	int i, j = 0;
	long count = 1;
	unsigned int flags;
	char* t;
	char cur_date[64];
	struct utsname header;
	struct tm rectime;
	cg_group* group = NULL;
	/* Get HZ */
	get_HZ();
	/* parse arguments */
	while (opt < argc) {
		if (!strcmp(argv[opt], "-g")) {
			flags |= I_D_GROUP;
			if (argv[++opt] && (strspn(argv[opt], DIGITS) != strlen(argv[opt]))
					&& (strncmp(argv[opt], "-", 1))) {

				for (t = strtok(argv[opt], ","); t; t = strtok(NULL, ",")) {
					/* Store device name */
					strncpy(group_name[j++], t, strlen(t));
				}
				opt++;
			} else {
				flags |= I_D_ALL;
			}
		} else if (!strncmp(argv[opt], "-", 1)) {
			for (i = 1; *(argv[opt] + i); i++) {
				switch (*(argv[opt] + i)) {
				case 'h':
					usage(0, argv[0]);
					ret = 0;
					goto err;
				case 'c':
					flags |= I_D_CPU;
					break;
				case 't':
					flags |= I_D_TIMESTAMP;
					break;
				case 'k':
					flags |= I_D_KILOBYTES;
					break;
				case 'm':
					flags |= I_D_MEGABYTES;
					break;
				case 'x':
					flags |= I_D_EXTENDED;
					break;
				case 'V':
					show_version();
					return 0;
				default:
					usage(1, argv[0]);
					ret = 1;
					goto err;
				}
			}
			opt++;
		} else if (!isdigit(argv[opt][0])) {
			if (strcmp(argv[opt], K_ALL)) {
				/* Store device name */
				strncpy(group_name[j++], argv[opt], strlen(argv[opt]));
				opt++;
			} else {
				opt++;
			}
		} else if (!it) {
			interval = atol(argv[opt++]);
			if (interval < 0) {
				usage(1, argv[0]);
			}
			count = -1;
			it = 1;
		}

		else if (it > 0) {
			count = atol(argv[opt++]);
			if ((count < 1) || !interval) {
				usage(1, argv[0]);
			}
			it = -1;
		} else {
			usage(1, argv[0]);
		}
	}

	if (!interval) {
		count = 1;
	}
	/* Get current date. */
	get_localtime(&rectime);
	strftime(cur_date, sizeof(cur_date), "%Y-%m-%d", &rectime);

	/* Get system name, release number and hostname */
	uname(&header);
	printf("%s %s (%s) \t%s \t_%s\n\n", header.sysname, header.release,
			header.nodename, cur_date, header.machine);

	/* Set a handler for SIGALRM */
	alarm_handler(0);

	if (init_stats()) {
		fprintf(stderr, "cgiostat: initialize the stats failed.\n");
		printf("error: Initialize the stats failed.\n");
		goto err;
	}
	read_device();
	/**Read all of the groups from BLKIO.*/
	if (read_all_groups(io_groups, CG_BLKIO)) {
		fprintf(stderr, "cgiostat: read all of the groups in %s failed.\n",
				CG_BLKIO);
		printf("error: Read all of the groups in %s failed.\n", CG_BLKIO);
		goto err;
	}
	/** If no group to display, then display all of the group statistics information.*/
	if (!j) {
		if (io_groups && !TAILQ_EMPTY(io_groups))
			TAILQ_FOREACH(group, io_groups,next)
				strncpy(group_name[j++], group->name, strlen(group->name));
	}
	/* Main loop */
	if (j)
		rw_stats_loop(count, flags, j, &rectime);

	err: uninit_stats();
	return ret;
}

