find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/submodules/imgui)

add_executable(
        render_mandelbrot_imgui
        render_mandelbrot_imgui.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)
target_include_directories(render_mandelbrot_imgui PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
target_link_libraries(render_mandelbrot_imgui glfw OpenGL::GL)
