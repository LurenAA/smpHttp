cmake_minimum_required(VERSION 3.0.0)
project(smpHttp VERSION 0.1.0)

list(APPEND dics /root/libuv/include
${PROJECT_SOURCE_DIR}/src/libuv_cpp_wrapper/
${PROJECT_SOURCE_DIR}/src/
${PROJECT_SOURCE_DIR}/src/http_parser/
${PROJECT_SOURCE_DIR}/src/modules/mysql-connector-cpp/include/
${PROJECT_SOURCE_DIR}/src/mysql_con/
${PROJECT_SOURCE_DIR}/src/modules/
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
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g3 -ggdb")
# set(JSON_BuildTests OFF CACHE INTERNAL "")

if(NOT DEFINED NLO_PATH)
  set(NLO_PATH "src/nlohmann_json")
endif() 
add_subdirectory(${NLO_PATH})

if(NOT DEFINED LIBMYSQL)
  set(MYSQL_PATH "${PROJECT_SOURCE_DIR}/src/modules/mysql-connector-cpp/libmysqlcppconn8-static.a")
endif()

list(APPEND links ${PROJECT_SOURCE_DIR}/src/modules/libuv/libuv.a -pthread 
${PROJECT_SOURCE_DIR}/src/modules/jwt/libjwt.a
HttpParser libuv_cpp_wrapper
# /usr/local/mysql/connector-c++-8.0/lib64/debug/libmysqlcppconn8.so
# /usr/local/mysql/connector-c++-8.0/lib64/debug/libmysqlcppconn8.so.2
# /home/ele/mysql-cpp-8-static/libmysqlcppconn8-static.a 
# ${PROJECT_SOURCE_DIR}/src/modules/mysql-connector-cpp/libmysqlcppconn8-static.a
${MYSQL_PATH}
-lcrypto
-lssl
)
add_executable(smpHttp ${srcs})
target_LINK_LIbraries(smpHttp PRIVATE nlohmann_json::nlohmann_json ${links})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)
