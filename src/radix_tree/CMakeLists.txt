cmake_minimum_required(VERSION 3.0.0)
project(radix_tree VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(radix_treec ./radix_tree.c)
add_library(radix_tree RadixTree.cpp)
target_link_libraries(radix_tree radix_treec)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
