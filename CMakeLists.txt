cmake_minimum_required(VERSION 3.16)
project(math_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        math_cpp
        main.cpp
        mandelbrot.hpp
)
