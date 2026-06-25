cmake_minimum_required(VERSION 3.16)
project(math_cpp)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# general-purpose libs (available to all subdirectories)
find_package(spdlog REQUIRED)
find_package(cxxopts REQUIRED)

add_subdirectory(renderers/opencv_img)
add_subdirectory(renderers/opengl_base)
add_subdirectory(renderers/opengl_shader)
add_subdirectory(renderers/imgui)
add_subdirectory(experiments)
