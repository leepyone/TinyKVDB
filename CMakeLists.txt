cmake_minimum_required(VERSION 3.23)
project(TinyKVDB)

set(CMAKE_CXX_STANDARD 11)

add_executable(TinyKVDB src/skiplist.cpp include/skiplist.h include/common.h include/byte_array.h src/byte_array.cpp include/options.h include/status.h src/options.cpp src/status.cpp include/random.h include/memory_pool.h src/memory_pool.cpp include/table.h src/table_impl.cpp example/simply_using.cpp)
