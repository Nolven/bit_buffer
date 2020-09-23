cmake_minimum_required(VERSION 3.17)
project(BMtest CXX)

set(CMAKE_CXX_STANDARD 14)


include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

add_executable(${PROJECT_NAME} main.cpp ${CMAKE_CURRENT_LIST_DIR}/../BinaryMessage.cpp)