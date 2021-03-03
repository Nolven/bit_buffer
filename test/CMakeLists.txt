cmake_minimum_required(VERSION 3.17)
project(Test CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

# GTEST
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
enable_testing()

# Units tests
##################################################################################
add_executable(${PROJECT_NAME} main.cpp ${CMAKE_CURRENT_LIST_DIR}/../binaryBuffer.cpp test.cpp)
target_link_libraries(${PROJECT_NAME}  gtest gtest_main)

add_test( ${PROJECT_NAME} test.cpp )