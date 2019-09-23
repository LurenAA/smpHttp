cmake_minimum_required(VERSION 3.0.0)
project(smpHttp VERSION 0.1.0)

include(CTest)
enable_testing()

list(APPEND dics /home/ele/libuv/include/ 
${PROJECT_SOURCE_DIR}/src/libuv_cpp_wrapper/
${PROJECT_SOURCE_DIR}/src/
${PROJECT_SOURCE_DIR}/src/radix_tree/
${PROJECT_SOURCE_DIR}/src/http_parser/)

include_directories(${dics})
add_subdirectory(src/radix_tree)
add_subdirectory(src/libuv_cpp_wrapper)
add_subdirectory(src/http_parser)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src srcs)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/test srcs)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

list(APPEND links /home/ele/libuv/out/Debug/libuv.a -pthread 
radix_tree HttpParser Tcp Loop Handle Connection)
add_executable(smpHttp ${srcs})
target_LINK_LIbraries(smpHttp ${links})

MESSAGE("building...")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
