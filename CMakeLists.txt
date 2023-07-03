cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)
project(Capstone)


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")


add_executable(Capstone src/main.cpp)

target_link_libraries(Capstone ${OpenCV_LIBS})