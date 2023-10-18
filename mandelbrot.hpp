#include <complex>
#include <vector>

#include "utilities.hpp"

namespace mandelbrot {

    double interpolate(double value_left, double value_right, double frac_right) {
        return value_left * (1.0 - frac_right) + value_right * frac_right;
    }

    std::vector<std::complex<double>> gen_complex_set(
            int size_x,
            int size_y,
            double real_min,
            double real_max,
            double imag_min,
            double imag_max
    ) {
        // Y axis - imaginary numbers
        // X axis - real axis
        std::vector<std::complex<double>> complex_set;

        for (int i = 0; i < size_y; i++) {
            double imag_interpolation_frac = static_cast<double>(i) / (static_cast<double>(size_y) - 1.0);
            auto imag_value = mandelbrot::interpolate(imag_min, imag_max, imag_interpolation_frac);

            for (int j = 0; j < size_x; ++j) {
                double real_interpolation_frac = static_cast<double>(j) / (static_cast<double>(size_x) - 1.0);
                auto real_value = mandelbrot::interpolate(real_min, real_max, real_interpolation_frac);

                std::complex<double> complex_value(real_value, imag_value);
                complex_set.push_back(complex_value);
            }
        }

        return complex_set;
    }

    template<typename T>
    std::complex<double> mandelbrot_func(T z_val, std::complex<double> complex_val) {
        return std::pow(std::complex<double>(z_val), 2) + complex_val;
    }

    std::vector<int> mandelbrot_sequence(
            std::vector<std::complex<double>> &complex_set,
            double threshold,
            int n_iterations
    ) {
        auto n_values = static_cast<int>(complex_set.size());

        std::vector<int> threshold_crossed_at_iteration(n_values, -1);

        for (int idx_value = 0; idx_value < n_values; idx_value++) {

            auto complex_value = complex_set[idx_value];
            std::complex<double> complex_value_iterated;

            for (int idx_iter = 0; idx_iter < n_iterations; idx_iter++) {

                if (idx_iter == 0) {
                    // if it is first iteration, use fc(0) = z**2 + c
                    complex_value_iterated = mandelbrot_func(0.0, complex_value);
                } else {
                    // on other iterations, use fc(fc(0)), or fc(fc(fc(0))), etc ...
                    complex_value_iterated = mandelbrot_func(complex_value_iterated, complex_value);
                }

                if (abs(complex_value_iterated) > threshold) {
                    threshold_crossed_at_iteration[idx_value] = idx_iter;
                    break;
                }
            }
        }
        return threshold_crossed_at_iteration;
    }

    std::vector<float> gen_mandelbrot_greyscale(
            int size_x,
            int size_y,
            double real_min,
            double real_max,
            double imag_min,
            double imag_max,
            double threshold,
            int n_iterations
    ) {
        auto complex_set = mandelbrot::gen_complex_set(size_x, size_y, real_min, real_max, imag_min, imag_max);

        // check sequence condition (divergence to infinity for each value)
        std::vector<int> threshold_crossed_at_iter = mandelbrot::mandelbrot_sequence(
                complex_set,
                threshold,
                n_iterations
        );

        std::vector<float> greyscale_values;
        for (const auto &iter: threshold_crossed_at_iter) {
            float greyscale_color = math_cpp_utils::assign_greyscale_color_based_on_value_float(
                    iter,
                    0.0,
                    n_iterations,
                    0
            );
            greyscale_values.push_back(greyscale_color);
        }
        return greyscale_values;
    }

    void print_complex_set(const std::vector<std::complex<double>> &complex_set) {
        (void) printf("[");
        for (const auto &complex_num: complex_set) {
            (void) printf("(%f, %f), ", complex_num.real(), complex_num.imag());
        }
        (void) printf("]\n");
    }

    void print_vector(const std::vector<int> &vec) {
        for (int i: vec) {
            (void) printf("%d ", i);
        }
        (void) printf("\n");
    }

}
