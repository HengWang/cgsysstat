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

cg_group_lists* group_lists = NULL;
cg_group* group = NULL;
cg_cpuset_stats_lists* cpuset_stats_lists = NULL;
cg_proc_stats_lists* proc_stats_lists = NULL;
cg_mem_stats *mem_stats = NULL;
cg_blkio_stats* blkio_stats = NULL;

int init_suite_group() {
	return 0;
}

int uninit_suite_group() {
	return 0;
}

void test_init_group_lists() {

	int ret = init_group_lists(&group_lists);
	CU_ASSERT(ret == 0);
	CU_ASSERT(group_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(group_lists));
	/** Test the NULL.*/
	ret = init_group_lists(NULL );
	CU_ASSERT(ret == -1);
}
void test_uninit_group_lists() {

	uninit_group_lists(NULL );
	uninit_group_lists(group_lists);
	group_lists = NULL;
	CU_ASSERT(group_lists == NULL);
}

void test_init_group() {

	cg_cpuset_stats* cpuset_stats = NULL;
	cg_proc_stats* io_stats = NULL;

	char* name = "test";
	char* path = "/home/q/";

	int ret = init_group(NULL, name, NULL );
	CU_ASSERT(ret == -1);

	ret = init_group(&group, NULL, path);
	CU_ASSERT(ret == -1);
	CU_ASSERT(group==NULL);

	ret = init_group(&group, name, NULL );
	CU_ASSERT(ret == 0);
	CU_ASSERT(group!=NULL);
	CU_ASSERT(strlen(group->name)==4)
	CU_ASSERT( !strncmp(group->name, name,4));
	CU_ASSERT(group->path == NULL);
	CU_ASSERT(group->cpuset_stats[0]!=NULL);
	CU_ASSERT(group->cpuset_stats[1]!=NULL);
	CU_ASSERT(TAILQ_EMPTY(group->cpuset_stats[0]));
	CU_ASSERT(group->cpuset_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->cpuset_stats[1]));
	CU_ASSERT(group->cpuset_stats[1]->count==0);
	CU_ASSERT(group->blkio_stats[0]!=NULL);
	CU_ASSERT(group->blkio_stats[1]!=NULL);
	CU_ASSERT(group->blkio_stats[0]->io_merged!=NULL);
	CU_ASSERT(group->blkio_stats[1]->io_merged!=NULL);
	CU_ASSERT(group->blkio_stats[0]->io_merged->count==0);
	CU_ASSERT(group->blkio_stats[1]->io_merged->count==0);

	CU_ASSERT(group->mem_stats[0]!=NULL);
	CU_ASSERT(group->mem_stats[1]!=NULL);
	CU_ASSERT(group->mem_stats[0]->stats!=NULL);
	CU_ASSERT(group->mem_stats[1]->stats!=NULL);

	CU_ASSERT(group->proc_stats[0]!=NULL);
	CU_ASSERT(group->proc_stats[1]!=NULL);
	CU_ASSERT(group->proc_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->proc_stats[0]));
	CU_ASSERT(group->proc_stats[1]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->proc_stats[1]));

	uninit_group(group);
	group = NULL;
	ret = init_group(&group, name, path);
	CU_ASSERT(ret == 0);
	CU_ASSERT(group!=NULL);
	CU_ASSERT( !strncmp(group->name, name,4));
	CU_ASSERT( !strncmp(group->path, path,8));
	CU_ASSERT(group->cpuset_stats[0]!=NULL);
	CU_ASSERT(group->cpuset_stats[1]!=NULL);
	CU_ASSERT(TAILQ_EMPTY(group->cpuset_stats[0]));
	CU_ASSERT(group->cpuset_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->cpuset_stats[1]));
	CU_ASSERT(group->cpuset_stats[1]->count==0);

	CU_ASSERT(group->blkio_stats[0]!=NULL);
	CU_ASSERT(group->blkio_stats[1]!=NULL);
	CU_ASSERT(group->blkio_stats[0]->io_merged!=NULL);
	CU_ASSERT(group->blkio_stats[1]->io_merged!=NULL);
	CU_ASSERT(group->blkio_stats[0]->io_merged->count==0);
	CU_ASSERT(group->blkio_stats[1]->io_merged->count==0);

	CU_ASSERT(group->mem_stats[0]!=NULL);
	CU_ASSERT(group->mem_stats[1]!=NULL);
	CU_ASSERT(group->mem_stats[0]->stats!=NULL);
	CU_ASSERT(group->mem_stats[1]->stats!=NULL);

	CU_ASSERT(group->proc_stats[0]!=NULL);
	CU_ASSERT(group->proc_stats[1]!=NULL);
	CU_ASSERT(group->proc_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->proc_stats[0]));
	CU_ASSERT(group->proc_stats[1]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->proc_stats[1]));

}

void test_uninit_group() {

	char* name = "test1";
	char* path = "/home/q/";
	int ret = init_group(&group, name, path);
	CU_ASSERT(ret == 0);
	CU_ASSERT(group!=NULL);
	CU_ASSERT( !strncmp(group->name, name,5));
	CU_ASSERT( !strncmp(group->path, path,8));
	CU_ASSERT(group->cpuset_stats[0]!=NULL);
	CU_ASSERT(group->cpuset_stats[1]!=NULL);
	CU_ASSERT(TAILQ_EMPTY(group->cpuset_stats[0]));
	CU_ASSERT(group->cpuset_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->cpuset_stats[1]));
	CU_ASSERT(group->cpuset_stats[1]->count==0);

	CU_ASSERT(group->blkio_stats[0]!=NULL);
	CU_ASSERT(group->blkio_stats[1]!=NULL);
	CU_ASSERT(group->blkio_stats[0]->io_merged!=NULL);
	CU_ASSERT(group->blkio_stats[1]->io_merged!=NULL);
	CU_ASSERT(group->blkio_stats[0]->io_merged->count==0);
	CU_ASSERT(group->blkio_stats[1]->io_merged->count==0);

	CU_ASSERT(group->mem_stats[0]!=NULL);
	CU_ASSERT(group->mem_stats[1]!=NULL);
	CU_ASSERT(group->mem_stats[0]->stats!=NULL);
	CU_ASSERT(group->mem_stats[1]->stats!=NULL);

	CU_ASSERT(group->proc_stats[0]!=NULL);
	CU_ASSERT(group->proc_stats[1]!=NULL);
	CU_ASSERT(group->proc_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->proc_stats[0]));
	CU_ASSERT(group->proc_stats[1]->count==0);
	CU_ASSERT(TAILQ_EMPTY(group->proc_stats[1]));

	uninit_group(NULL );
	uninit_group(group);
	group = NULL;
	CU_ASSERT(group == NULL);
}

void test_get_group() {

	cg_group* result = NULL;
	char* name = "test";
	char* false_name = "abc";
	char* path = "/home/q/";
	int ret = get_group(group_lists, &result, name);
	CU_ASSERT(ret ==0);
	CU_ASSERT(result!=NULL);
	CU_ASSERT( !strncmp(result->name,name,4));
	CU_ASSERT( !strncmp(result->path,path,8));
	CU_ASSERT(result->cpuset_stats[0]!=NULL);
	CU_ASSERT(result->cpuset_stats[1]!=NULL);
	CU_ASSERT(TAILQ_EMPTY(result->cpuset_stats[0]));
	CU_ASSERT(result->cpuset_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(result->cpuset_stats[1]));
	CU_ASSERT(result->cpuset_stats[1]->count==0);

	CU_ASSERT(result->blkio_stats[0]!=NULL);
	CU_ASSERT(result->blkio_stats[1]!=NULL);
	CU_ASSERT(result->blkio_stats[0]->io_merged!=NULL);
	CU_ASSERT(result->blkio_stats[1]->io_merged!=NULL);
	CU_ASSERT(result->blkio_stats[0]->io_merged->count==0);
	CU_ASSERT(result->blkio_stats[1]->io_merged->count==0);

	CU_ASSERT(result->mem_stats[0]!=NULL);
	CU_ASSERT(result->mem_stats[1]!=NULL);
	CU_ASSERT(result->mem_stats[0]->stats!=NULL);
	CU_ASSERT(result->mem_stats[1]->stats!=NULL);

	CU_ASSERT(result->proc_stats[0]!=NULL);
	CU_ASSERT(result->proc_stats[1]!=NULL);
	CU_ASSERT(result->proc_stats[0]->count==0);
	CU_ASSERT(TAILQ_EMPTY(result->proc_stats[0]));
	CU_ASSERT(result->proc_stats[1]->count==0);
	CU_ASSERT(TAILQ_EMPTY(result->proc_stats[1]));

	ret = get_group(group_lists, &result, false_name);
	CU_ASSERT(ret == -1);
	CU_ASSERT(result == NULL);
	/** Test the NULL*/
	ret = get_group(group_lists, NULL, name);
	CU_ASSERT(ret == -1);
	ret = get_group(NULL, &result, name);
	CU_ASSERT(ret == -1);
}

void test_is_group_exists() {

	char* name = "test";
	char* false_name = "abc";
	BOOL ret = is_group_exists(group_lists, name);
	CU_ASSERT(ret == TRUE);
	ret = is_group_exists(group_lists, false_name);
	CU_ASSERT(ret == FALSE);
	ret = is_group_exists(NULL, name);
	CU_ASSERT(ret == FALSE);
}

void test_add_group() {

	char* name = "test";
	char* path = "/home/q/";
	int ret = add_group(group_lists, group);
	CU_ASSERT(ret == 0);
	CU_ASSERT(group_lists->count == 1);
	CU_ASSERT(!TAILQ_EMPTY(group_lists));
	CU_ASSERT( !strncmp(group_lists->tqh_first->name, name,4));
	CU_ASSERT( !strncmp(group_lists->tqh_first->path,path,8));

	/** Add the stats again.*/
	ret = add_group(group_lists, group);
	CU_ASSERT(ret == 0);
	CU_ASSERT(group_lists->count == 1);
	/** Test the NULL*/
	ret = add_group(NULL, group);
	CU_ASSERT(ret == -1);
	ret = add_group(group_lists, NULL );
	CU_ASSERT(ret == -1);
}

void test_remove_group() {

	int ret = remove_group(group_lists, NULL );
	CU_ASSERT(ret == 0);
	ret = remove_group(NULL, group);
	CU_ASSERT(ret == -1);
	ret = remove_group(group_lists, group);
	CU_ASSERT(ret == 0);
	CU_ASSERT(group_lists->count == 0);
	CU_ASSERT(TAILQ_EMPTY(group_lists));
}

void test_clear_group() {

	clear_group(NULL );
	clear_group(group_lists);
	CU_ASSERT(group_lists != NULL);
	CU_ASSERT(TAILQ_EMPTY(group_lists));
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_group", init_suite_group, uninit_suite_group);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_group_lists() function",
					test_init_group_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_group() function",
					test_init_group))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test add_group() function", test_add_group))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test get_group() function", test_get_group))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test is_group_exists() function",
					test_is_group_exists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test remove_group() function",
					test_remove_group))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_group() function",
					test_uninit_group))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test clear_group() function",
					test_clear_group))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_group_lists() function",
					test_uninit_group_lists))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
	return 0;
}
