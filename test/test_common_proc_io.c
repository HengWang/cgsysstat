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

cg_proc_stats_lists* proc_stats_lists = NULL;
cg_proc_stats* stats = NULL;

int init_suite_io() {
	return 0;
}

int uninit_suite_io() {
	return 0;
}

void test_init_io_stats_lists() {

	int ret = init_proc_stats_lists(&proc_stats_lists);
	CU_ASSERT(ret == 0);
	CU_ASSERT(proc_stats_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(proc_stats_lists));
	/** Test the NULL.*/
	ret = init_proc_stats_lists(NULL );
	CU_ASSERT(ret == -1);
}

void test_uninit_io_stats_lists() {

	uninit_proc_stats_lists(NULL );
	uninit_proc_stats_lists(proc_stats_lists);
	proc_stats_lists = NULL;
	CU_ASSERT(proc_stats_lists == NULL);
}

void test_init_io_stats() {
	int ret = init_proc_stats(&stats, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(stats != NULL);
	CU_ASSERT(stats->id == 1);
	/** Test the NULL.*/
	ret = init_proc_stats(NULL, 1);
	CU_ASSERT(ret == -1);

	stats->io_stats->io_cwbytes = 1;
	stats->io_stats->io_rbytes = 1;
	stats->io_stats->io_rchar = 1;
	stats->io_stats->io_syscr = 1;
	stats->io_stats->io_syscw = 1;
	stats->io_stats->io_wbytes = 1;
	stats->io_stats->io_wchar = 1;

	stats->proc_stat->pid = 1;

	CU_ASSERT(stats->io_stats->io_cwbytes == 1);
	CU_ASSERT(stats->io_stats->io_rbytes == 1);
	CU_ASSERT(stats->io_stats->io_rchar == 1);
	CU_ASSERT(stats->io_stats->io_syscr == 1);
	CU_ASSERT(stats->io_stats->io_syscw == 1);
	CU_ASSERT(stats->io_stats->io_wbytes == 1);
	CU_ASSERT(stats->io_stats->io_wchar == 1);

	CU_ASSERT(stats->proc_stat->pid==1);
}

void test_uninit_io_stats() {

	int ret = init_proc_stats(&stats, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(stats->id == 1);
	uninit_proc_stats(NULL );
	uninit_proc_stats(stats);
	stats = NULL;
	CU_ASSERT(stats == NULL);
}

void test_get_io_stats() {

	cg_proc_stats* result = NULL;
	int ret = get_proc_stats(proc_stats_lists, &result, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->id == 1);
	CU_ASSERT(result->io_stats->io_cwbytes == 1);
	ret = get_proc_stats(proc_stats_lists, &result, 2);
	CU_ASSERT(ret == -1);
	CU_ASSERT(result == NULL);
	/** Test the NULL*/
	ret = get_proc_stats(proc_stats_lists, NULL, 1);
	CU_ASSERT(ret == -1);
	ret = get_proc_stats(NULL, &result, 1);
	CU_ASSERT(ret == -1);
}

void test_is_io_exists() {

	BOOL ret = is_proc_exists(proc_stats_lists, 1);
	CU_ASSERT(ret == TRUE);
	ret = is_proc_exists(proc_stats_lists, 2);
	CU_ASSERT(ret == FALSE);
	ret = is_proc_exists(NULL, 1);
	CU_ASSERT(ret == FALSE);
}

void test_add_io_stats() {

	int ret = add_proc_stats(proc_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(proc_stats_lists->count == 1);
	CU_ASSERT(!TAILQ_EMPTY(proc_stats_lists));
	CU_ASSERT(proc_stats_lists->tqh_first->id == 1);
	CU_ASSERT(proc_stats_lists->tqh_first->io_stats->io_cwbytes == 1);
	/** Add the stats again.*/
	ret = add_proc_stats(proc_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(proc_stats_lists->count == 1);
	/** Test the NULL*/
	ret = add_proc_stats(NULL, stats);
	CU_ASSERT(ret == -1);
	ret = add_proc_stats(proc_stats_lists, NULL );
	CU_ASSERT(ret == -1);
}

void test_remove_io_stats() {

	int ret = remove_proc_stats(proc_stats_lists, NULL );
	CU_ASSERT(ret == 0);
	ret = remove_proc_stats(NULL, stats);
	CU_ASSERT(ret == -1);
	ret = remove_proc_stats(proc_stats_lists, stats);
	CU_ASSERT(ret == 0);
	CU_ASSERT(proc_stats_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(proc_stats_lists));
}

void test_clear_io_stats() {

	clear_proc_stats(NULL );
	clear_proc_stats(proc_stats_lists);
	CU_ASSERT(proc_stats_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(proc_stats_lists));
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_io", init_suite_io, uninit_suite_io);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_io_stats_lists() function",
					test_init_io_stats_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_io_stats() function",
					test_init_io_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test add_io_stats() function",
					test_add_io_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test get_io_stats() function",
					test_get_io_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test is_io_exists() function",
					test_is_io_exists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test remove_io_stats() function",
					test_remove_io_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_io_stats() function",
					test_uninit_io_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test clear_io_stats() function",
					test_clear_io_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_io_stats_lists() function",
					test_uninit_io_stats_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
