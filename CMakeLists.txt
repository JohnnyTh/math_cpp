cmake_minimum_required(VERSION 3.16)
project(math_cpp)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# general-purpose libs
find_package(spdlog REQUIRED)
find_package(cxxopts REQUIRED)
# ------------------------- OpenCV image generation build --------------------
# find OpenCV
find_package(OpenCV REQUIRED)

add_executable(
        render_mandelbrot_opencv_img
        render_mandelbrot_opencv_img.cpp
        src/cpp/mandelbrot.hpp
        src/cpp/utilities.hpp
        src/cpp/utilities_opencv.hpp
        src/cpp/timer.hpp
)
# link OpenCV
target_link_libraries(render_mandelbrot_opencv_img ${OpenCV_LIBS} spdlog::spdlog_header_only cxxopts::cxxopts)
# ------------------------- OpenGl render build --------------------
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
# source https://www.glfw.org/docs/3.3/build_guide.html
find_package(glfw3 3.3 REQUIRED)

# unoptimized: generate on cpu -> texture -> render
add_executable(
        render_mandelbrot_opengl
        render_mandelbrot_opengl_base.cpp
        src/cpp/mandelbrot.hpp
        src/cpp/utilities_shaders.hpp
)
target_link_libraries(render_mandelbrot_opengl glfw OpenGL::GL GLEW::GLEW)

# compute mandelbrot using shader
add_executable(
        render_mandelbrot_opengl_shader
        render_mandelbrot_opengl_shader.cpp
        src/cpp/mandelbrot.hpp
        src/cpp/utilities_shaders.hpp
        src/cpp/colormaps.hpp
)
target_link_libraries(
        render_mandelbrot_opengl_shader
        glfw
        OpenGL::GL
        GLEW::GLEW
        spdlog::spdlog_header_only
        cxxopts::cxxopts
)
# ------------------------- IMGUI render build -----------------------
# Dear ImGui
set(IMGUI_DIR submodules/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

add_executable(
        render_mandelbrot_imgui
        render_mandelbrot_imgui.cpp
        src/cpp/mandelbrot.hpp
        src/cpp/utilities.hpp
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        src/cpp/utilities_shaders.hpp
)

target_link_libraries(render_mandelbrot_imgui glfw OpenGL::GL)
target_include_directories(render_mandelbrot_imgui PUBLIC submodules/imgui)

# -------------------------- Experiments -------------

add_executable(
        experiments
        experiments.cpp
        src/cpp/colormaps.hpp
)