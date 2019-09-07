cmake_minimum_required(VERSION 3.0.0)
project(smpHttp VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(/home/ele/libuv/include)
include_directories(./src)
include_directories(./src/radix_tree)
include_directories(./src/http_parser)
add_definitions("-Wall -g -fPIC")
add_library(radix src/radix_tree/radix_tree.c)
add_library(http_parser src/http_parser/http_parser.c)
add_library(http_request_handles src/http_request_handles.cpp)
add_library(Net src/Net.cpp)
add_library(HttpRequest src/HttpRequest.cpp)
add_library(RequestContext src/RequestContext.cpp)
add_library(uv_handles src/uv_handles.cpp)

target_link_libraries(Net /home/ele/libuv/out/Debug/libuv.a radix http_request_handles pthread HttpRequest)
target_link_libraries(HttpRequest Net)
target_link_libraries(RequestContext HttpRequest)
target_link_libraries(uv_handles RequestContext http_parser radix HttpRequest) 
target_link_libraries(http_request_handles HttpRequest uv_handles)

list(APPEND deps /home/ele/libuv/out/Debug/libuv.a HttpRequest radix http_parser http_request_handles Net RequestContext uv_handles)

add_executable(smpHttp test/main.cpp)
target_LINK_LIbraries(smpHttp ${deps})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
