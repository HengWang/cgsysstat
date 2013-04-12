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

cg_device_value_lists* cg_value_lists = NULL;
cg_device_value* cg_value = NULL;

int init_suite_device_value() {
	return 0;
}

int uninit_suite_device_value() {
	return 0;
}

void test_init_device_value_lists() {

	int ret = init_device_value_lists(&cg_value_lists);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_value_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(cg_value_lists));
	/** Test the NULL.*/
	ret = init_device_value_lists(NULL );
	CU_ASSERT(ret != 0);
}

void test_uninit_device_value_lists() {

	uninit_device_value_lists(NULL );
	uninit_device_value_lists(cg_value_lists);
	cg_value_lists = NULL;
	CU_ASSERT(cg_value_lists == NULL);
}

void test_init_device_value() {

	int ret = init_device_value(&cg_value, 1, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_value != NULL);
	CU_ASSERT(cg_value->value==0);
	CU_ASSERT(cg_value->dev == 1);
	CU_ASSERT(cg_value->idx == 1);
	cg_value->value = 1;
	CU_ASSERT(cg_value->value == 1);
	/** Test the NULL.*/
	ret = init_device_value(NULL, 1, 1);
	CU_ASSERT(ret == -1);
}

void test_uninit_device_value() {

	int ret = init_device_value(&cg_value, 1,1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_value->dev == 1);
	CU_ASSERT(cg_value->idx==1);
	uninit_device_value(NULL );
	uninit_device_value(cg_value);
	cg_value = NULL;
	CU_ASSERT(cg_value == NULL);
}

void test_get_device_value() {

	cg_device_value* result = NULL;
	int ret = get_device_value(cg_value_lists, &result, 1, 1);
	CU_ASSERT(ret == 0);
	CU_ASSERT(result != NULL);
	CU_ASSERT(result->dev == 1);
	CU_ASSERT(result->idx==1);
	CU_ASSERT(result->value == 1);
	ret = get_device_value(cg_value_lists, &result, 2, 1);
	CU_ASSERT(ret == -1);
	CU_ASSERT(result == NULL);
	/** Test the NULL*/
	ret = get_device_value(cg_value_lists, NULL, 1, 1);
	CU_ASSERT(ret == -1);
	ret = get_device_value(NULL, &result, 1, 1);
	CU_ASSERT(ret == -1);
}

void test_is_device_value_exists() {

	BOOL ret = is_device_value_exists(cg_value_lists, 1, 1);
	CU_ASSERT(ret == TRUE);
	ret = is_device_value_exists(cg_value_lists, 2, 1);
	CU_ASSERT(ret == FALSE);
	ret = is_device_value_exists(NULL, 1, 1);
	CU_ASSERT(ret == FALSE);
}

void test_add_device_value() {

	int ret = add_device_value(cg_value_lists, cg_value);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_value_lists->count == 1);
	CU_ASSERT(!TAILQ_EMPTY(cg_value_lists));
	CU_ASSERT(cg_value_lists->tqh_first->dev == 1);
	CU_ASSERT(cg_value_lists->tqh_first->idx == 1);
	CU_ASSERT(cg_value_lists->tqh_first->value==1);
	/** Add the value again.*/
	ret = add_device_value(cg_value_lists, cg_value);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_value_lists->count == 1);
	/** Test the NULL*/
	ret = add_device_value(NULL, cg_value);
	CU_ASSERT(ret == -1);
	ret = add_device_value(cg_value_lists, NULL );
	CU_ASSERT(ret == -1);
}

void test_remove_device_value() {

	int ret = remove_device_value(cg_value_lists, NULL );
	CU_ASSERT(ret == 0);
	ret = remove_device_value(NULL, cg_value);
	CU_ASSERT(ret == -1);
	ret = remove_device_value(cg_value_lists, cg_value);
	CU_ASSERT(ret == 0);
	CU_ASSERT(cg_value_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(cg_value_lists));
}

void test_clear_device_value() {

	clear_device_value(NULL );
	clear_device_value(cg_value_lists);
	CU_ASSERT(cg_value_lists != NULL);
	CU_ASSERT(cg_value_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(cg_value_lists));
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_device_value", init_suite_device_value,
			uninit_suite_device_value);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_device_value_lists() function",
					test_init_device_value_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_device_value() function",
					test_init_device_value))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test add_device_value() function",
					test_add_device_value))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test get_device_value() function",
					test_get_device_value))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test is_device_value_exists() function",
					test_is_device_value_exists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test remove_device_value() function",
					test_remove_device_value))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_device_value() function",
					test_uninit_device_value))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test clear_device_value() function",
					test_clear_device_value))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_device_value_lists() function",
					test_uninit_device_value_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}

