cmake_minimum_required(VERSION 2.8.10)
project(bvar_c++ C CXX)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(bvar_test bvar_test.cpp)
add_dependencies(bvar_test bvar-static butil-static)
target_link_libraries(bvar_test bvar-static butil-static pthread ${GFLAGS_LIBRARY})

add_executable(benchmark benchmark.cpp)
add_dependencies(benchmark bvar-static butil-static)
target_link_libraries(benchmark bvar-static butil-static pthread ${GFLAGS_LIBRARY})

add_executable(benchmark_atomic benchmark_atomic.cpp)
add_dependencies(benchmark_atomic bvar-static butil-static)
target_link_libraries(benchmark_atomic bvar-static butil-static pthread ${GFLAGS_LIBRARY})
