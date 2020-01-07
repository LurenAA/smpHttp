cmake_minimum_required(VERSION 3.0.0)
project(libuv_cpp_wrapper VERSION 0.1.0)

# link_directories(/usr/local/lib)
add_definitions("-Wall -g -mcmodel=large")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
include_directories(${PROJECT_SOURCE_DIR}/../modules/libuv)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR} srcs)
# add_executable(libuv_cpp_wrapper ${srcs})
add_library(libuv_cpp_wrapper ${srcs})

target_link_libraries(libuv_cpp_wrapper pthread 
${PROJECT_SOURCE_DIR}/../modules/libuv/libuv.a 
${PROJECT_SOURCE_DIR}/../modules/log4cpp/log4cpp.a)