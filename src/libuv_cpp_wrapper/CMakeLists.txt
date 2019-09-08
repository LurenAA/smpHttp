cmake_minimum_required(VERSION 3.0.0)
project(libuv_cpp_wrapper VERSION 0.1.0)

include(CTest)
enable_testing()
include_directories(/home/ele/libuv/include)
add_definitions("-Wall -g -fPIC")
add_library(TcpAcceptor ./TcpAcceptor.cpp)
target_link_libraries(TcpAcceptor /home/ele/libuv/out/Debug/libuv.a)

add_executable(libuv_cpp_wrapper test.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
