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
static cg_group_lists* mem_groups = NULL;
static struct cpuset_stats* avg_cpuset_stats = NULL;

/*
 * The alarm handler to trigger the alarm every interval seconds.
 */
void alarm_handler(int sig) {
	signal(SIGALRM, alarm_handler);
	alarm(interval);
}

/*
 * Uninitialized the statistics information.
 */
void uninit_stats() {

	if (avg_cpuset_stats)
		free(avg_cpuset_stats);
	uninit_group_lists(mem_groups);
}

/*
 * Initialized the statistics information.
 */
int init_stats() {

	int i = 0;
	/* Initialize the cgroup. */
	if (cgroup_init()) {
		fprintf(stderr, "cgvmstat: initialize the cgroup failed.\n");
		printf("error: Initialize the cgroup failed.\n");
		uninit_stats();
		return -1;
	}
	if (init_group_lists(&mem_groups)) {
		fprintf(stderr, "cgvmstat: initialize the memory group lists failed.\n",
				CG_CPUSET);
		printf("error: Initialize the memory group lists failed.\n");
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
	return 0;
}

void write_vm_header(int flag, unsigned long long* unit) {

	if (DISPLAY_ACTIVE(flag))
		printf("group   procs    ----------memory---------"
				"  ---io--- --------cpu--------\n"
				"name     r   b  swpd   free  inact  active"
				"  pi   po  us  sy  id  wa  st\n");
	else
		printf("group   procs    ----------memory---------"
				"  ---io--- --------cpu--------\n"
				"name     r   b  swpd   free   buff  cache"
				"  pi   po  us  sy  id  wa  st\n");
	if (DISPLAY_KILOBYTES(flag))
		*unit = 1024;
	else if (DISPLAY_MEGABYTES(flag))
		*unit = 1024 * 1024;
	else if (DISPLAY_GIGABYTES(flag))
		*unit = 1024 * 1024 * 1024;
}

static unsigned int cnt = 0;
void write_vm_stats(cg_group* group, int cur, int flag, unsigned long long itv) {

	unsigned long long unit = 1;
	unsigned int run = 0, sleep = 0;

	if (!cnt++)
		write_vm_header(flag, &unit);
	else if (!DISPLAY_UNHEADER(flag) && cnt > 50) {
		cnt = 1;
		write_vm_header(flag, &unit);
	}
	memset(avg_cpuset_stats, 0, sizeof(struct cpuset_stats));
	compute_procs_state(group->proc_stats[cur], &run, &sleep);
	compute_avg_cpuset_stats(group->cpuset_stats, avg_cpuset_stats, cur);
	if (!DISPLAY_ACTIVE(flag))

		printf("%s  %2d  %2d  %2.0f  %2.0f  %2.0f  %2.0f  %2.0f"
				"  %2.0f  %2.0f  %2.0f  %2.0f  %2.0f  %2.0f\n",
		/** The group name.*/
		group->name,
		/** The running processes count.*/
		run,
		/** The sleeping processes count.*/
		sleep,
				/** The swap usage*/
				(double) (ll_value(group->mem_stats[!cur]->mem_swap_usage,
						group->mem_stats[cur]->mem_swap_usage)
						- ll_value(group->mem_stats[!cur]->usage,
								group->mem_stats[cur]->usage)) / unit,
				/** The free space.*/
				(double) (group->mem_stats[cur]->limit
						- group->mem_stats[cur]->usage) / unit,
				/** The buffer space, there is no statistic information. */
				(double) 0.00,
				/** The cache space.*/
				(double) (group->mem_stats[cur]->stats->cache) / unit,
				/** The count of page in.*/
				(double) ll_value(group->mem_stats[!cur]->stats->pgpgin,
						group->mem_stats[cur]->stats->pgpgin),
				/** The count of page out.*/
				(double) ll_value(group->mem_stats[!cur]->stats->pgpgout,
						group->mem_stats[cur]->stats->pgpgout),
				/** The cpu user time.*/
				itv ? (((double) avg_cpuset_stats->cpu_user) / itv) * 100 : 0.00,
				/** The cpu system time.*/
				itv ? (((double) avg_cpuset_stats->cpu_sys) / itv) * 100 : 0.00,
				/** The cpu idle time.*/
				itv ? (((double) avg_cpuset_stats->cpu_idle) / itv) * 100 : 0.00,
				/** The cpu io wait time.*/
				itv ? (((double) avg_cpuset_stats->cpu_iowait) / itv) * 100 : 0.00,
				/** The cpu steal time.*/
				itv ? (((double) avg_cpuset_stats->cpu_steal) / itv) * 100 : 0.00);
	else
		printf("%s  %2d  %2d  %2.0f  %2.0f  %2.0f  %2.0f  %2.0f"
				"  %2.0f  %2.0f  %2.0f  %2.0f  %2.0f  %2.0f\n",
		/** The group name.*/
		group->name,
		/** The running processes count.*/
		run,
		/** The sleeping processes count.*/
		sleep,
				/** The swap usage*/
				(double) (ll_value(group->mem_stats[!cur]->mem_swap_usage,
						group->mem_stats[cur]->mem_swap_usage)
						- ll_value(group->mem_stats[!cur]->usage,
                                                                group->mem_stats[cur]->usage)) / unit,
				/** The free space.*/
				(double) (group->mem_stats[cur]->limit
						- group->mem_stats[cur]->usage) / unit,
				/** The inactive memory space.*/
				(double) (group->mem_stats[cur]->stats->inactive_anon
						+ group->mem_stats[cur]->stats->inactive_file) / unit,
				/** The active memory space.*/
				(double) (group->mem_stats[cur]->stats->active_anon
						+ group->mem_stats[cur]->stats->active_file) / unit,
				/** The count of page in.*/
				(double) ll_value(group->mem_stats[!cur]->stats->pgpgin,
						group->mem_stats[cur]->stats->pgpgin),
				/** The count of page out.*/
				(double) ll_value(group->mem_stats[!cur]->stats->pgpgout,
						group->mem_stats[cur]->stats->pgpgout),
				/** The cpu user time.*/
				itv ? (((double) avg_cpuset_stats->cpu_user) / itv) * 100 : 0.00,
				/** The cpu system time.*/
				itv ? (((double) avg_cpuset_stats->cpu_sys) / itv) * 100 : 0.00,
				/** The cpu idle time.*/
				itv ? (((double) avg_cpuset_stats->cpu_idle) / itv) * 100 : 0.00,
				/** The cpu io wait time.*/
				itv ? (((double) avg_cpuset_stats->cpu_iowait) / itv) * 100 : 0.00,
				/** The cpu steal time.*/
				itv ? (((double) avg_cpuset_stats->cpu_steal) / itv) * 100 : 0.00);
}

/*
 * The loop process procedure.
 *
 * IN:
 * @count:               Loop count.
 * @flag :               Display flag.
 * @group_count:         The count of the groups.
 */
void rw_stats_loop(long count, int flag, int group_count) {

	int i = 0, cur = 0;
	cg_group* mem_group = NULL;
	unsigned long long itv = 0;
	setbuf(stdout, NULL );
	if (group_count > GROUP_COUNT_MAX)
		group_count = GROUP_COUNT_MAX;
	do {

		/** Display all of the groups statistics information. */
		for (i = 0; i < group_count; i++) {
			if (group_name[i] && !TAILQ_EMPTY(mem_groups)
					&& !get_group(mem_groups, &mem_group, group_name[i])) {
				if (read_cg_mem_stats(group_name[i], mem_group->mem_stats[cur])
						|| read_cg_cpu_stats(group_name[i],
								mem_group->cpuset_stats[cur])
						|| read_cg_io_stats(group_name[i],
								mem_group->blkio_stats[cur])) {
					fprintf(stderr,
							"cgvmstat: read the cpu, io and memory stats of group: %s failed.\n",
							group_name[i]);
					printf(
							"error: Read the cpu, io and memory stats of group: %s failed.\n",
							group_name[i]);
					return;
				}
				itv = get_total_interval(mem_group->cpuset_stats[cur],
						mem_group->cpuset_stats[!cur]);
				write_vm_stats(mem_group, cur, flag, itv);
			} else
				printf("no statistics information in group: %s\n",
						group_name[i]);
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
 * Print the usage of the cgvmstat.
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
		fprintf(stdout,
				"Usage: %s [-h] [[-b] [-k] [-m] [-G] [-t] [-g] <group_name>] "
						"delay count\n", program_name);
		fprintf(stdout, "  -g <group1,...>   Display the io utilization of \n"
				"                    group named <group_name>. \n");
		fprintf(stdout,
				"  -a                Display inactive/active page stats. \n");
		fprintf(stdout, "  -n                Display the headers not to be\n"
				"                      reprinted regularly. \n");
		fprintf(stdout, "  -b                Display the memory in Bytes. \n");
		fprintf(stdout, "  -k                Display the memory in KB. \n");
		fprintf(stdout, "  -m                Display the memory in MB. \n");
		fprintf(stdout, "  -G                Display the memory in GB. \n");
		fprintf(stdout, "  -h                Display this help. \n");
		fprintf(stdout, "  delay is the delay between updates in seconds. \n");
		fprintf(stdout, "  count is the number of updates.. \n");
		fprintf(stdout, "  -V                Display the version.\n");
		fprintf(stdout,
				"If not given the -g option, list all cgroups information\n");
	}
}

static void show_version() {

	printf("cgvmstat version 1.0\n");
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
				case 'a':
					flags |= I_D_ACTIVE;
					break;
				case 'b':
					flags |= I_D_BYTES;
					break;
				case 'k':
					flags |= I_D_KILOBYTES;
					break;
				case 'm':
					flags |= I_D_MEGABYTES;
					break;
				case 'n':
					flags |= I_D_UNHEADER;
				case 'G':
					flags |= I_D_GIGABYTES;
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
		} else if (it > 0) {
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
	/* Set a handler for SIGALRM */
	alarm_handler(0);

	if (init_stats()) {
		fprintf(stderr, "cgvmstat: initialize the stats failed.\n");
		printf("error: Initialize the stats failed.\n");
		goto err;
	}
	/** Read all of the groups from CPUSET.*/
	if (read_all_groups(mem_groups, CG_MEMORY)) {
		fprintf(stderr, "cgvmstat: read all of the groups in %s failed.\n",
				CG_CPUSET);
		printf("error: Read all of the groups in %s failed.\n", CG_CPUSET);
		goto err;
	}
	/** If no group to display, then display all of the group statistics information.*/
	if (!j && mem_groups && !TAILQ_EMPTY(mem_groups))
		TAILQ_FOREACH(group, mem_groups,next)
			strncpy(group_name[j++], group->name, strlen(group->name));
	/* Main loop */
	if (j)
		rw_stats_loop(count, flags, j);

	err: uninit_stats();
	return ret;
}

