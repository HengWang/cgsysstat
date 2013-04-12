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
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

cg_cpuset_stats_lists * cpuset_stats_lists = NULL;
cg_cpuset_stats* stats = NULL;

int init_suite_cpu(void) {
	return 0;

}
int uninit_suite_cpu(void) {
	return 0;
}

/*
 * Test the init_cpuset_stats_lists function.
 */
void test_init_cpuset_stats_lists() {
	int ret = init_cpuset_stats_lists(&cpuset_stats_lists);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cpuset_stats_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(cpuset_stats_lists));
	/** Test the NULL.*/
	ret = init_cpuset_stats_lists(NULL );
	CU_ASSERT(ret == -1);
}

/*
 * Test the uninit_cpuset_stats_lists function.
 */
void test_uninit_cpuset_stats_lists() {

	uninit_cpuset_stats_lists(NULL );
	uninit_cpuset_stats_lists(cpuset_stats_lists);
	cpuset_stats_lists = NULL;
	CU_ASSERT(cpuset_stats_lists == NULL);
}

/*
 * Test the init_cpuset_stats function.
 */
void test_init_cpuset_stats() {

	int ret = init_cpuset_stats(&stats, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(stats!=NULL);
	CU_ASSERT(stats->id == 1);
	/** Test the NULL.*/
	ret = init_cpuset_stats(NULL, 1);
	CU_ASSERT(ret == -1);

	stats->stats->cpu_user = 1;
	stats->stats->cpu_sys = 1;
	stats->stats->cpu_steal = 1;
	stats->stats->cpu_softirq = 1;
	stats->stats->cpu_nice = 1;
	stats->stats->cpu_iowait = 1;
	stats->stats->cpu_idle = 1;
	stats->stats->cpu_hardirq = 1;
	stats->stats->cpu_guest = 1;

	CU_ASSERT(stats->stats->cpu_user == 1);
	CU_ASSERT(stats->stats->cpu_sys == 1);
	CU_ASSERT(stats->stats->cpu_steal == 1);
	CU_ASSERT(stats->stats->cpu_softirq == 1);
	CU_ASSERT(stats->stats->cpu_nice == 1);
	CU_ASSERT(stats->stats->cpu_iowait == 1);
	CU_ASSERT(stats->stats->cpu_idle == 1);
	CU_ASSERT(stats->stats->cpu_hardirq == 1);
	CU_ASSERT(stats->stats->cpu_guest == 1);
}

/*
 * Test the uninit_cpuset_stats function.
 */
void test_uninit_cpuset_stats() {

	int ret = init_cpuset_stats(&stats, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(stats->id == 1);
	uninit_cpuset_stats(NULL );
	uninit_cpuset_stats(stats);
	stats = NULL;
	CU_ASSERT(stats == NULL);
}

/*
 * Test the get_cpuset_stats function.
 */
void test_get_cpuset_stats() {

	cg_cpuset_stats* result = NULL;
	int ret = get_cpuset_stats(cpuset_stats_lists, &result, 1);
	CU_ASSERT(ret ==0);
	CU_ASSERT(result!=NULL);
	CU_ASSERT(result->id == 1);
	CU_ASSERT(result->stats->cpu_user == 1);
	ret = get_cpuset_stats(cpuset_stats_lists, &result, 2);
	CU_ASSERT(ret == -1);
	CU_ASSERT(result == NULL);
	/** Test the NULL*/
	ret = get_cpuset_stats(cpuset_stats_lists, NULL, 1);
	CU_ASSERT(ret == -1);
	ret = get_cpuset_stats(NULL, &result, 1);
	CU_ASSERT(ret == -1);
}

/*
 * Test the is_cpuset_exists function.
 */
void test_is_cpuset_exists() {

	BOOL ret = is_cpuset_exists(cpuset_stats_lists, 1);
	CU_ASSERT(ret == TRUE);
	ret = is_cpuset_exists(cpuset_stats_lists, 2);
	CU_ASSERT(ret == FALSE);
	ret = is_cpuset_exists(NULL, 1);
	CU_ASSERT(ret == FALSE);
}

/*
 * Test the add_cpuset_stats function.
 */
void test_add_cpuset_stats() {

	int ret = add_cpuset_stats(cpuset_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cpuset_stats_lists->count == 1);
	CU_ASSERT(!TAILQ_EMPTY(cpuset_stats_lists));
	CU_ASSERT(cpuset_stats_lists->tqh_first->id == 1);
	CU_ASSERT(cpuset_stats_lists->tqh_first->stats->cpu_user == 1);
	/** Add the stats again.*/
	ret = add_cpuset_stats(cpuset_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cpuset_stats_lists->count == 1);
	/** Test the NULL*/
	ret = add_cpuset_stats(NULL, stats);
	CU_ASSERT(ret == -1);
	ret = add_cpuset_stats(cpuset_stats_lists, NULL );
	CU_ASSERT(ret == -1);
}

/*
 * Test the remove_cpuset_stats function.
 */
void test_remove_cpuset_stats() {

	int ret = remove_cpuset_stats(cpuset_stats_lists, NULL );
	CU_ASSERT(ret == 0);
	ret = remove_cpuset_stats(NULL, stats);
	CU_ASSERT(ret == -1);
	ret = remove_cpuset_stats(cpuset_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cpuset_stats_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(cpuset_stats_lists));
}

/*
 * Test the clear_cpuset_stats function.
 */
void test_clear_cpuset_stats() {

	clear_cpuset_stats(NULL );
	clear_cpuset_stats(cpuset_stats_lists);
	CU_ASSERT(cpuset_stats_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(cpuset_stats_lists));
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_cpu", init_suite_cpu, uninit_suite_cpu);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_cpuset_stats_lists() function",
					test_init_cpuset_stats_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_cpuset_stats() function",
					test_init_cpuset_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test add_cpuset_stats() function",
					test_add_cpuset_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test get_cpuset_stats() function",
					test_get_cpuset_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test is_cpuset_exists() function",
					test_is_cpuset_exists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test remove_cpuset_stats() function",
					test_remove_cpuset_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_cpuset_stats() function",
					test_uninit_cpuset_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test clear_cpuset_stats() function",
					test_clear_cpuset_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_cpuset_stats_lists() function",
					test_uninit_cpuset_stats_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
	return 0;
}

