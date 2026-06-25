find_package(OpenCV REQUIRED)

add_executable(
        render_mandelbrot_opencv_img
        render_mandelbrot_opencv_img.cpp
)
target_include_directories(render_mandelbrot_opencv_img PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(render_mandelbrot_opencv_img ${OpenCV_LIBS} spdlog::spdlog_header_only cxxopts::cxxopts)
