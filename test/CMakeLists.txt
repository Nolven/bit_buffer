project(BM test)

include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

add_executable(${PROJECT_NAME} main.cpp ${CMAKE_CURRENT_LIST_DIR}/BinaryMessage.cpp)