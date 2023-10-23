#include <chrono>

#include <opencv2/imgcodecs.hpp>
#include <cxxopts.hpp>

#include "src/cpp/mandelbrot.hpp"
#include "src/cpp/utilities_opencv.hpp"


int main(int argc, char *argv[]) {
    cxxopts::Options options{argv[0], "Mandelbrot set image rendering tool"};
    options.add_options()
            ("w,width", "Image width", cxxopts::value<int>()->default_value("1980"))
            ("h,height", "Image height", cxxopts::value<int>()->default_value("1080"))
            ("rmin,real_min", "Real number minimum", cxxopts::value<double>()->default_value("-3.0"))
            ("rmax,real_max", "Real number maximum", cxxopts::value<double>()->default_value("1.0"))
            ("imin,imag_min", "Imaginary number minimum", cxxopts::value<double>()->default_value("-1.5"))
            ("imax,imag_max", "Imaginary number maximum", cxxopts::value<double>()->default_value("1.5"))
            ("i,n_iterations", "Number of iterations", cxxopts::value<int>()->default_value("35"))
            ("t,threshold", "Abs value threshold", cxxopts::value<double>()->default_value("6.0"))
            ("p,img_p", "Image path", cxxopts::value<std::string>()->default_value("mandelbrot.png"));

    auto result = options.parse(argc, argv);

    auto start = std::chrono::high_resolution_clock::now();

    int width = result["width"].as<int>();
    int height = result["height"].as<int>();

    double real_min = result["real_min"].as<double>();
    double real_max = result["real_max"].as<double>();
    double imag_min = result["imag_min"].as<double>();
    double imag_max = result["imag_max"].as<double>();

    int n_iterations = result["n_iterations"].as<int>();
    double threshold = result["threshold"].as<double>();

    spdlog::info("Begin mandelbrot image generation");

    auto complex_set = mandelbrot::gen_complex_set(width, height, real_min, real_max, imag_min, imag_max);
    spdlog::info("Done gen_complex_set");

    // check sequence condition (divergence to infinity for each value)
    std::vector<int> threshold_crossed_at_iter = mandelbrot::mandelbrot_sequence(complex_set, threshold, n_iterations);
    spdlog::info("Done mandelbrot_sequence");

    std::vector<int> greyscale_values;
    for (const auto &iter: threshold_crossed_at_iter) {
        int greyscale_color = math_cpp_utils::assign_greyscale_color_based_on_value(
                iter,
                0.0,
                n_iterations,
                255.0,
                0
        );
        greyscale_values.push_back(greyscale_color);
    }

    cv::Mat greyscale_mat = math_cpp_utils_opencv::get_greyscale_mat(greyscale_values, width, height);
    spdlog::info("Done postprocess");

    //mandelbrot::print_complex_set(complex_set);
    //mandelbrot::print_vector(threshold_crossed_at_iter);
    std::string img_name = result["img_p"].as<std::string>();

    spdlog::info("Save image at: {}", img_name);
    (void) cv::imwrite(img_name, greyscale_mat);

    spdlog::info("Done finish program");
    return 0;
}
