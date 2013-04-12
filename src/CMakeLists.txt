cmake_minimum_required(VERSION 2.6)

PROJECT(cgsysstat)

ADD_SUBDIRECTORY(common)

INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/include
)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/)

SET(CGIOSTAT_SOURCES cgiostat.c rd_stats.c compute.c)
ADD_EXECUTABLE(cgiostat ${CGIOSTAT_SOURCES})

TARGET_LINK_LIBRARIES(cgiostat cgroup common)
ADD_DEPENDENCIES(cgiostat cgroup common)

SET(CGIOSTAT_SOURCES cgiostat_ext.c rd_stats.c compute.c)
ADD_EXECUTABLE(cgiostat_ext ${CGIOSTAT_SOURCES})

TARGET_LINK_LIBRARIES(cgiostat_ext cgroup common)
ADD_DEPENDENCIES(cgiostat_ext cgroup common)

SET(CGFREE_SOURCES cgfree.c rd_stats.c compute.c)
ADD_EXECUTABLE(cgfree ${CGFREE_SOURCES})

TARGET_LINK_LIBRARIES(cgfree cgroup common)
ADD_DEPENDENCIES(cgfree cgroup common)


SET(CGVMSTAT_SOURCES cgvmstat.c rd_stats.c compute.c)
ADD_EXECUTABLE(cgvmstat ${CGVMSTAT_SOURCES})

TARGET_LINK_LIBRARIES(cgvmstat cgroup common)
ADD_DEPENDENCIES(cgvmstat cgroup common)
