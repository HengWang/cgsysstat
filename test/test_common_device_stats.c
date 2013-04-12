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

cg_device_stats_lists* cg_stats_lists = NULL;
cg_device_stats* stats = NULL;

int init_suite_device_stats() {
	return 0;
}

int uninit_suite_device_stats() {
	return 0;
}

void test_init_device_stats_lists() {

	int ret = init_device_stats_lists(&cg_stats_lists);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_stats_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(cg_stats_lists));
	/** Test the NULL.*/
	ret = init_device_stats_lists(NULL );
	CU_ASSERT(ret != 0);
}

void test_uninit_device_stats_lists() {

	uninit_device_stats_lists(NULL );
	uninit_device_stats_lists(cg_stats_lists);
	cg_stats_lists = NULL;
	CU_ASSERT(cg_stats_lists == NULL);
}

void test_init_device_stats() {

	int ret = init_device_stats(&stats, 1, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(stats != NULL);
	CU_ASSERT(stats->stats!=NULL);
	CU_ASSERT(stats->dev == 1);
	CU_ASSERT(stats->idx == 1);

	stats->stats->async = 1;
	stats->stats->read = 1;
	stats->stats->sync = 1;
	stats->stats->total = 1;
	stats->stats->write = 1;

	CU_ASSERT(stats->stats->async == 1);
	CU_ASSERT(stats->stats->read == 1);
	CU_ASSERT(stats->stats->sync == 1);
	CU_ASSERT(stats->stats->total == 1);
	CU_ASSERT(stats->stats->write == 1);
	/** Test the NULL.*/
	ret = init_device_stats(NULL, 1, 1);
	CU_ASSERT(ret == -1);
}

void test_uninit_device_stats() {

	int ret = init_device_stats(&stats, 1, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(stats->dev == 1);
	CU_ASSERT(stats->idx==1);
	uninit_device_stats(NULL );
	uninit_device_stats(stats);
	stats = NULL;
	CU_ASSERT(stats == NULL);
}

void test_get_device_stats() {

	cg_device_stats* result = NULL;
	int ret = get_device_stats(cg_stats_lists, &result, 1, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->dev == 1);
	CU_ASSERT(result->idx==1);
	CU_ASSERT(result->stats->read == 1);
	ret = get_device_stats(cg_stats_lists, &result, 2, 1);
	CU_ASSERT(ret == -1);
	CU_ASSERT(result == NULL);
	/** Test the NULL*/
	ret = get_device_stats(cg_stats_lists, NULL, 1, 1);
	CU_ASSERT(ret == -1);
	ret = get_device_stats(NULL, &result, 1, 1);
	CU_ASSERT(ret == -1);
}

void test_is_device_stats_exists() {

	BOOL ret = is_device_stats_exists(cg_stats_lists, 1, 1);
	CU_ASSERT(ret == TRUE);
	ret = is_device_stats_exists(cg_stats_lists, 2, 1);
	CU_ASSERT(ret == FALSE);
	ret = is_device_stats_exists(NULL, 1, 1);
	CU_ASSERT(ret == FALSE);
}

void test_add_device_stats() {

	int ret = add_device_stats(cg_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_stats_lists->count == 1);
	CU_ASSERT(!TAILQ_EMPTY(cg_stats_lists));
	CU_ASSERT(cg_stats_lists->tqh_first->dev == 1);
	CU_ASSERT(cg_stats_lists->tqh_first->idx == 1);
	CU_ASSERT(cg_stats_lists->tqh_first->stats!=NULL);
	CU_ASSERT(cg_stats_lists->tqh_first->stats->read == 1);
	/** Add the stats again.*/
	ret = add_device_stats(cg_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_stats_lists->count == 1);
	/** Test the NULL*/
	ret = add_device_stats(NULL, stats);
	CU_ASSERT(ret == -1);
	ret = add_device_stats(cg_stats_lists, NULL );
	CU_ASSERT(ret == -1);
}

void test_remove_device_stats() {
	int ret = remove_device_stats(cg_stats_lists, NULL );
	CU_ASSERT(ret == 0);
	ret = remove_device_stats(NULL, stats);
	CU_ASSERT(ret == -1);
	ret = remove_device_stats(cg_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_stats_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(cg_stats_lists));
}

void test_clear_device_stats() {

	clear_device_stats(NULL );
	clear_device_stats(cg_stats_lists);
	CU_ASSERT(cg_stats_lists != NULL);
	CU_ASSERT(cg_stats_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(cg_stats_lists));
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_device_stats", init_suite_device_stats,
			uninit_suite_device_stats);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_device_stats_lists() function",
					test_init_device_stats_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_device_stats() function",
					test_init_device_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test add_device_stats() function",
					test_add_device_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test get_device_stats() function",
					test_get_device_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test is_device_stats_exists() function",
					test_is_device_stats_exists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test remove_device_stats() function",
					test_remove_device_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_device_stats() function",
					test_uninit_device_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test clear_device_stats() function",
					test_clear_device_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_device_stats_lists() function",
					test_uninit_device_stats_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}

