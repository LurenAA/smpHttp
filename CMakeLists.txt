cmake_minimum_required(VERSION 3.0.0)
project(smpHttp VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(/home/ele/libuv/include)
include_directories(src/libuv_cpp_wrapper)
include_directories(./src)
include_directories(./src/radix_tree)
include_directories(./src/http_parser)
add_definitions("-Wall -g -fPIC")
add_library(radix src/radix_tree/radix_tree.c)
add_subdirectory(src/libuv_cpp_wrapper)
add_library(HttpServer src/HttpServer.cpp)
target_link_libraries(HttpServer Tcp Loop Handle Connection /home/ele/libuv/out/Debug/libuv.a pthread)
list(APPEND deps /home/ele/libuv/out/Debug/libuv.a radix HttpServer)

add_executable(smpHttp test/main.cpp)
target_LINK_LIbraries(smpHttp ${deps})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
