find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(
        render_mandelbrot_opengl_shader
        render_mandelbrot_opengl_shader.cpp
)
target_include_directories(render_mandelbrot_opengl_shader PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(
        render_mandelbrot_opengl_shader
        glfw
        OpenGL::GL
        GLEW::GLEW
        spdlog::spdlog_header_only
        cxxopts::cxxopts
)
