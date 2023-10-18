cmake_minimum_required(VERSION 3.16)
project(math_cpp)

set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# find OpenCV
find_package(OpenCV REQUIRED)

add_executable(
        gen_img_mandelbrot
        gen_img_mandelbrot.cpp
        mandelbrot.hpp
        utilities.hpp
)
# link OpenCV
target_link_libraries(gen_img_mandelbrot ${OpenCV_LIBS})

# ------------------------- GUI build -----------------------
find_package(OpenGL REQUIRED)
# source https://www.glfw.org/docs/3.3/build_guide.html
find_package(glfw3 3.3 REQUIRED)

# Dear ImGui
set(IMGUI_DIR submodules/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

add_executable(
        run_gui_mandelbrot
        run_gui_mandelbrot.cpp
        mandelbrot.hpp
        utilities.hpp
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
)

add_executable(
        render_mandelbrot_opengl
        render_mandelbrot_opengl.cpp
)

target_link_libraries(run_gui_mandelbrot glfw OpenGL::GL)
target_link_libraries(render_mandelbrot_opengl glfw OpenGL::GL)
target_include_directories(run_gui_mandelbrot PUBLIC submodules/imgui)
