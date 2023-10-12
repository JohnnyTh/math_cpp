cmake_minimum_required(VERSION 3.16)
project(math_cpp)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# find OpenCV
find_package(OpenCV REQUIRED)

add_executable(
        run_mandelbrot
        run_mandelbrot.cpp
        mandelbrot.hpp
        utilities.hpp
)


# link OpenCV
target_link_libraries(run_mandelbrot ${OpenCV_LIBS})