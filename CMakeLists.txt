cmake_minimum_required(VERSION 3.28)
project(pwrrt_autonomy)

set(CMAKE_CXX_STANDARD 20)

add_executable(pwrrt_autonomy main.cpp)
add_executable(position_interpolator position_interpolator.cpp)
