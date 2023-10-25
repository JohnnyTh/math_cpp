#include <chrono>

#include <opencv2/imgcodecs.hpp>
#include <cxxopts.hpp>
#include "spdlog/spdlog.h"

#include "src/cpp/timer.hpp"
#include "src/cpp/mandelbrot.hpp"
#include "src/cpp/utilities_opencv.hpp"


int main(int argc, char *argv[]) {
    cxxopts::Options options{argv[0], "Mandelbrot set image rendering tool"};
    options.add_options()
            ("w,width", "Image width", cxxopts::value<int>()->default_value("1980"))
            ("h,height", "Image height", cxxopts::value<int>()->default_value("1080"))
            ("rmin,real_min", "Real number minimum", cxxopts::value<double>()->default_value("-2.5"))
            ("rmax,real_max", "Real number maximum", cxxopts::value<double>()->default_value("1.0"))
            ("imin,imag_min", "Imaginary number minimum", cxxopts::value<double>()->default_value("-1.1"))
            ("imax,imag_max", "Imaginary number maximum", cxxopts::value<double>()->default_value("1.1"))
            ("i,n_iterations", "Number of iterations", cxxopts::value<int>()->default_value("35"))
            ("t,threshold", "Abs value threshold", cxxopts::value<double>()->default_value("6.0"))
            ("p,img_p", "Image path", cxxopts::value<std::string>()->default_value("mandelbrot.png"));

    auto result = options.parse(argc, argv);

    auto t_0 = std::chrono::high_resolution_clock::now();

    int width = result["width"].as<int>();
    int height = result["height"].as<int>();

    double real_min = result["real_min"].as<double>();
    double real_max = result["real_max"].as<double>();
    double imag_min = result["imag_min"].as<double>();
    double imag_max = result["imag_max"].as<double>();

    int n_iterations = result["n_iterations"].as<int>();
    double threshold = result["threshold"].as<double>();

    std::string img_name = result["img_p"].as<std::string>();

    timer::Timer timer;

    spdlog::info("Begin mandelbrot set image generation");

    auto t_1 = std::chrono::high_resolution_clock::now();
    auto complex_set = mandelbrot::gen_complex_set(width, height, real_min, real_max, imag_min, imag_max);
    timer.timeit("gen_complex_set()", t_1);

    // check sequence condition (divergence to infinity for each value)
    auto t_2 = std::chrono::high_resolution_clock::now();
    std::vector<int> mandelbrot_set = mandelbrot::mandelbrot_sequence(complex_set, threshold, n_iterations);
    timer.timeit("mandelbrot_sequence()", t_2);

    auto t_3 = std::chrono::high_resolution_clock::now();
    cv::Mat greyscale_mat = math_cpp_utils_opencv::get_greyscale_mat(mandelbrot_set, width, height);
    timer.timeit("get_greyscale_mat()", t_3);

    spdlog::info("Save image at: {}", img_name);
    auto t_4 = std::chrono::high_resolution_clock::now();
    (void) cv::imwrite(img_name, greyscale_mat);
    timer.timeit("cv::imwrite()", t_4);

    timer.timeit("main()", t_0);
    timer.logTime();
    return 0;
}
