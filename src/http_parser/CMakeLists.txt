cmake_minimum_required(VERSION 3.0.0)
project(http-parser VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(HttpParser HttpParser.cpp)
add_library(HttpResult HttpResult.cpp)
target_link_libraries(HttpParser HttpResult)
add_executable(http-parser main.cpp)
target_link_libraries(http-parser HttpParser)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
