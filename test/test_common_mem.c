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

cg_mem_stats* cg_stats = NULL;
int init_suite_mem() {
	return 0;
}

int uninit_suite_mem() {
	return 0;
}

void test_init_mem_stats() {

	int ret = init_mem_stats(&cg_stats);
	CU_ASSERT(ret==0);
	CU_ASSERT(cg_stats!=NULL);
	CU_ASSERT(cg_stats->stats!=NULL);
}

void test_uninit_mem_stats() {

	uninit_mem_stats(cg_stats);
	cg_stats = NULL;
	CU_ASSERT(cg_stats==NULL);
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_mem", init_suite_mem, uninit_suite_mem);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_mem_stats() function",
					test_init_mem_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_mem_stats() function",
					test_uninit_mem_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
