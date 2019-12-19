cmake_minimum_required(VERSION 3.0.0)
project(libuv_cpp_wrapper VERSION 0.1.0)

link_directories(/usr/local/lib)
add_definitions("-Wall -g -mcmodel=large")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
include_directories(/root/libuv-v1.30.0/include)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} srcs)
# add_executable(libuv_cpp_wrapper ${srcs})
add_library(libuv_cpp_wrapper ${srcs})

target_link_libraries(libuv_cpp_wrapper pthread /root/libuv-v1.30.0/out/Debug/libuv.a /root/log4cpp/src/.libs/liblog4cpp.a)