cmake_minimum_required(VERSION 3.21)
project(kalman)

set(CMAKE_CXX_STANDARD 14)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(kalman main.cpp)
target_include_directories(kalman PRIVATE "/usr/include/eigen3")
target_link_libraries(kalman ${SDL2_LIBRARIES} SDL2_image SDL2_ttf)

