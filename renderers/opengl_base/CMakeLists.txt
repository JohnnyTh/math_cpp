find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_executable(
        render_mandelbrot_opengl
        render_mandelbrot_opengl_base.cpp
)
target_include_directories(render_mandelbrot_opengl PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(render_mandelbrot_opengl glfw OpenGL::GL GLEW::GLEW)

# copy shaders next to the binary at configure time
file(COPY vertex.vert fragment.frag fragment_mb.frag
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
