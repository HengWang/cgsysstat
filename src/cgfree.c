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

static long interval = 0;
static char group_name[LINE_LEN][LINE_LEN] = { 0 };
cg_group_lists* mem_groups = NULL;

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

	uninit_group_lists(mem_groups);
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
	if (init_group_lists(&mem_groups)) {
		fprintf(stderr, "cgiostat: initialize the memory group lists failed.\n",
				CG_MEMORY);
		printf("error: Initialize the memory group lists failed.\n");
		uninit_stats();
		return -1;
	}
	return 0;
}

/*
 * Write the memory statistic information.
 *
 * IN:
 * @mem_stats:                    The memory statistics information.
 * @flag:                         The display flag.
 */
void write_mem_stats(cg_mem_stats* mem_stats, int flag) {

	unsigned long long unit = 1024;
	unsigned long long temp = 0;
	printf(
			"              total          used          free        shared       buffers        cached\n");
	if (DISPLAY_BYTES(flag))
		unit = 1;
	else if (DISPLAY_MEGABYTES(flag))
		unit = 1024 * 1024;
	else if (DISPLAY_GIGABYTES(flag))
		unit = 1024 * 1024 * 1024;
	printf("Mem:   %12.0f  %12.0f  %12.0f  %12.0f  %12.0f  %12.0f\n",
			((double) mem_stats->limit) / unit,
			((double) mem_stats->usage) / unit,
			((double) (mem_stats->limit - mem_stats->usage)) / unit, (double) 0,
			(double) 0, ((double) mem_stats->stats->cache) / unit);
	if (!DISPLAY_OLD(flag))
		printf("-/+ buffers/cache:   %12.0f  %12.0f\n",
				((double) (mem_stats->usage - mem_stats->stats->cache
						- mem_stats->stats->inactive_anon
						- mem_stats->stats->inactive_file)) / unit,
				((double) (mem_stats->limit - mem_stats->usage
						+ mem_stats->stats->cache)) / unit);
	temp = mem_stats->mem_swap_limit - mem_stats->limit;
	printf("Swap:  %12.0f  %12.0f  %12.0f\n", ((double) (temp)) / unit,
			((double) (mem_stats->mem_swap_usage - mem_stats->usage)) / unit,
			temp ? ((double) (temp - mem_stats->mem_swap_usage
					+ mem_stats->usage) / unit) :
					0);
	if (DISPLAY_TOTAL(flag))
		printf("Total: %12.0f  %12.0f  %12.0f\n",
				((double) mem_stats->mem_swap_limit) / unit,
				((double) mem_stats->mem_swap_usage) / unit,
				((double) (mem_stats->mem_swap_limit - mem_stats->mem_swap_usage))
						/ unit);

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
	unsigned long long mem_usage, mem_swap_usage, mem_max_usage,
			mem_swap_max_usage;
	unsigned long long mem_limit, mem_swap_limit;
	setbuf(stdout, NULL );
	if (group_count > GROUP_COUNT_MAX)
		group_count = GROUP_COUNT_MAX;
	do {

		/** Display all of the groups statistics information. */
		for (i = 0; i < group_count; i++) {
			printf("------ %s ------\n", group_name[i]);
			if (group_name[i] && !TAILQ_EMPTY(mem_groups)
					&& !get_group(mem_groups, &mem_group, group_name[i])) {
				if (read_cg_mem_stats(group_name[i],
						mem_group->mem_stats[cur])) {
					fprintf(stderr,
							"cgiostat: read the memory stats of group: %s failed.\n",
							group_name[i]);
					printf(
							"error: Read the memory stats of group: %s failed.\n",
							group_name[i]);
					return;
				}

				write_mem_stats(mem_group->mem_stats[cur], flag);
			} else
				printf("no memory information in group: %s\n", group_name[i]);
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
		fprintf(stdout,
				"Usage: %s [-h] [[-b] [-k] [-m] [-G] [-t] [-g] <group_name>] [-o] "
						"[-s INTERVEL] [-c COUNT] INTERVEL COUNT\n",
				program_name);
		fprintf(stdout,
				"  -g <group1,...>   Display the io utilization of group\n"
						"                              named <group_name>. \n");
		fprintf(stdout, "  -t                Display total for RAM + swap. \n");
		fprintf(stdout, "  -b                Display the memory in Bytes. \n");
		fprintf(stdout, "  -k                Display the memory in KB. \n");
		fprintf(stdout, "  -m                Display the memory in MB. \n");
		fprintf(stdout, "  -G                Display the memory in GB. \n");
		fprintf(stdout,
				"  -o                Use old format (no -/+buffers/cache line).\n");
		fprintf(stdout, "  -s                Update every [delay] seconds.\n");
		fprintf(stdout, "  -c                Update [count] times.\n");
		fprintf(stdout, "  -h                Display this help. \n");
		fprintf(stdout, "  INTERVEL COUNT    Display the information COUNT \n"
				"                    times in each INTERVEL seconds.\n");
		fprintf(stdout, "  -V                Display the version.\n");
		fprintf(stdout,
				"If not given the -g option, list all cgroups information\n");
	}
}

static void show_version() {

	printf("cgfree version 1.0\n");
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
				case 't':
					flags |= I_D_TOTAL;
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
				case 'G':
					flags |= I_D_GIGABYTES;
					break;
				case 'o':
					flags |= I_D_OLD;
					break;
				case 's':
					interval = atol(argv[++opt]);
					if (interval < 0) {
						usage(1, argv[0]);
					}
					count = -1;
					i = 0;
					break;
				case 'c':
					count = atol(argv[++opt]);
					if ((count < 1) || !interval) {
						usage(1, argv[0]);
					}
					i = 0;
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
	/* Set a handler for SIGALRM */
	alarm_handler(0);

	if (init_stats()) {
		fprintf(stderr, "cgiostat: initialize the stats failed.\n");
		printf("error: Initialize the stats failed.\n");
		goto err;
	}
	/** Read all of the groups from CPUSET.*/
	if (read_all_groups(mem_groups, CG_MEMORY)) {
		fprintf(stderr, "cgiostat: read all of the groups in %s failed.\n",
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

