cmake_minimum_required(VERSION 2.6)

PROJECT(test)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/include
)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/share/bin)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/ ${CMAKE_SOURCE_DIR}/lib/CUnit)

SET(TEST_COMMON_CPU test_common_cpu.c)
ADD_EXECUTABLE(test_common_cpu ${TEST_COMMON_CPU})
TARGET_LINK_LIBRARIES(test_common_cpu cgroup common cunit)
ADD_DEPENDENCIES(test_common_cpu cgroup common cunit)

SET(TEST_COMMON_PROC_IO  test_common_proc_io.c)
ADD_EXECUTABLE(test_common_proc_io ${TEST_COMMON_PROC_IO})
TARGET_LINK_LIBRARIES(test_common_proc_io cgroup common cunit)
ADD_DEPENDENCIES(test_common_proc_io cgroup common cunit)

SET(TEST_COMMON_GROUP  test_common_group.c)
ADD_EXECUTABLE(test_common_group ${TEST_COMMON_GROUP})
TARGET_LINK_LIBRARIES(test_common_group cgroup common cunit)
ADD_DEPENDENCIES(test_common_group cgroup common cunit)

SET(TEST_COMMON_MEM  test_common_mem.c)
ADD_EXECUTABLE(test_common_mem ${TEST_COMMON_MEM})
TARGET_LINK_LIBRARIES(test_common_mem cgroup common cunit)
ADD_DEPENDENCIES(test_common_mem cgroup common cunit)


SET(TEST_COMMON_BLKIO  test_common_blkio.c)
ADD_EXECUTABLE(test_common_blkio ${TEST_COMMON_BLKIO})
TARGET_LINK_LIBRARIES(test_common_blkio cgroup common cunit)
ADD_DEPENDENCIES(test_common_blkio cgroup common cunit)

SET(TEST_COMMON_DEVICE_STATS  test_common_device_stats.c)
ADD_EXECUTABLE(test_common_device_stats ${TEST_COMMON_DEVICE_STATS})
TARGET_LINK_LIBRARIES(test_common_device_stats cgroup common cunit)
ADD_DEPENDENCIES(test_common_device_stats cgroup common cunit)

SET(TEST_COMMON_DEVICE_VALUE  test_common_device_value.c)
ADD_EXECUTABLE(test_common_device_value ${TEST_COMMON_DEVICE_VALUE})
TARGET_LINK_LIBRARIES(test_common_device_value cgroup common cunit)
ADD_DEPENDENCIES(test_common_device_value cgroup common cunit)

SET(TEST_COMMON_PROC  test_common_proc.c)
ADD_EXECUTABLE(test_common_proc ${TEST_COMMON_PROC})
TARGET_LINK_LIBRARIES(test_common_proc cgroup common cunit)
ADD_DEPENDENCIES(test_common_proc cgroup common cunit)
