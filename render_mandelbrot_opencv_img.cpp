#include <chrono>

#include <opencv2/imgcodecs.hpp>

#include "src/cpp/mandelbrot.hpp"
#include "src/cpp/utilities_opencv.hpp"


int main() {
    auto start = std::chrono::high_resolution_clock::now();

    int size_x = 1600 * 1;
    int size_y = 900 * 1;

    double real_min = -3.0;
    double real_max = 1.0;
    double imag_min = -1.5;
    double imag_max = 1.5;

    int n_iterations = 35;
    double threshold = 6.0;

    (void) printf("Generating Mandelbrot set\n");

    auto complex_set = mandelbrot::gen_complex_set(size_x, size_y, real_min, real_max, imag_min, imag_max);

    // check sequence condition (divergence to infinity for each value)
    std::vector<int> threshold_crossed_at_iter = mandelbrot::mandelbrot_sequence(complex_set, threshold, n_iterations);

    math_cpp_utils::print_time_from_start("Done mandelbrot_sequence", start);

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

    cv::Mat greyscale_mat = math_cpp_utils_opencv::get_greyscale_mat(greyscale_values, size_x, size_y);

    math_cpp_utils::print_time_from_start("Done postprocess", start);

    //mandelbrot::print_complex_set(complex_set);
    //mandelbrot::print_vector(threshold_crossed_at_iter);
    std::ostringstream stream;
    stream << "mandelbrot_" << n_iterations << ".png";
    std::string img_name = stream.str();

    (void) printf("Save image at: %s", img_name.c_str());
    (void) cv::imwrite(img_name, greyscale_mat);

    math_cpp_utils::print_time_from_start("Done finish program", start);
    return 0;
}
