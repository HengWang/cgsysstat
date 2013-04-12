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

cg_blkio_stats* cg_stats;
int init_suite_blkio() {
	return 0;
}

int uninit_suite_blkio() {
	return 0;
}

void test_init_blkio_stats() {

	int ret = init_blkio_stats(NULL );
	CU_ASSERT(ret!=0);
	ret = init_blkio_stats(&cg_stats);
	CU_ASSERT(ret==0);
	CU_ASSERT(cg_stats!=NULL);
	CU_ASSERT(cg_stats->io_merged!=NULL);
	CU_ASSERT(cg_stats->io_queued!=NULL);
	CU_ASSERT(cg_stats->io_service_bytes!=NULL);
	CU_ASSERT(cg_stats->io_service_time!=NULL);
	CU_ASSERT(cg_stats->io_serviced!=NULL);
	CU_ASSERT(cg_stats->io_wait_time!=NULL);
	CU_ASSERT(cg_stats->sectors!=NULL);
	CU_ASSERT(cg_stats->throttle_io_service_bytes!=NULL);
	CU_ASSERT(cg_stats->throttle_io_serviced!=NULL);
	CU_ASSERT(cg_stats->throttle_read_bps_device!=NULL);
	CU_ASSERT(cg_stats->throttle_read_iops_device!=NULL);
	CU_ASSERT(cg_stats->throttle_write_bps_device!=NULL);
	CU_ASSERT(cg_stats->throttle_write_iops_device!=NULL);
	CU_ASSERT(cg_stats->time!=NULL);
	CU_ASSERT(cg_stats->weight==0);
	CU_ASSERT(cg_stats->weight_device==0);
}

void test_uninit_blkio_stats() {

	uninit_blkio_stats(cg_stats);
	cg_stats = NULL;
	CU_ASSERT(cg_stats==NULL);
	uninit_blkio_stats(NULL );
}

int main(int argc, char *argv[]) {

	CU_pSuite pSuite = NULL;
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();
	pSuite = CU_add_suite("Suite_blkio", init_suite_blkio, uninit_suite_blkio);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test init_blkio_stats() function",
					test_init_blkio_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL
			== CU_add_test(pSuite, "test uninit_blkio_stats() function",
					test_uninit_blkio_stats))) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
