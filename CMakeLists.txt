cmake_minimum_required(VERSION 2.8)

project(test_task_server)

#find_package (Threads REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -pthread" )


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/
)
add_executable(${PROJECT_NAME}
    "main.cpp"
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processRequests.cpp)
