cmake_minimum_required(VERSION 3.0.0)
project(smpHttp VERSION 0.1.0)

list(APPEND dics /home/ele/libuv/include/ 
${PROJECT_SOURCE_DIR}/src/libuv_cpp_wrapper/
${PROJECT_SOURCE_DIR}/src/
${PROJECT_SOURCE_DIR}/src/http_parser/
/usr/local/mysql/connector-c++-8.0/include/
${PROJECT_SOURCE_DIR}/src/mysql_con/
)

include_directories(${dics})
add_subdirectory(src/libuv_cpp_wrapper)
add_subdirectory(src/http_parser)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src srcs)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/test srcs)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src/mysql_con srcs)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 
SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(src/nlohmann_json)

list(APPEND links /home/ele/libuv/out/Debug/libuv.a -pthread 
/usr/local/lib/libjwt.a
HttpParser Tcp Loop Handle Connection
# /usr/local/mysql/connector-c++-8.0/lib64/debug/libmysqlcppconn8.so
/home/ele/mysql-cpp-8-static/libmysqlcppconn8-static.a 
-lcrypto
-lssl
)
add_executable(smpHttp ${srcs})
target_LINK_LIbraries(smpHttp PRIVATE nlohmann_json::nlohmann_json ${links})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
