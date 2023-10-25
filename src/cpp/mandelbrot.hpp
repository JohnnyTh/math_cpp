#include <complex>
#include <vector>
#include <istream>
#include <iostream>

#include "spdlog/spdlog.h"

#include "utilities.hpp"

namespace mandelbrot {

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    interpolate(T value_left, T value_right, T frac_right) {
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

    std::vector<float> gen_complex_set_2_shader(
            int size_x,
            int size_y,
            float real_min,
            float real_max,
            float imag_min,
            float imag_max) {
        std::vector<float> complex_set;

        for (int i_row = 0; i_row < size_y; i_row++) {
            auto imag_interpolation_frac = static_cast<float>( static_cast<float>(i_row) / (size_y - 1.0));
            float imag_value = mandelbrot::interpolate(imag_min, imag_max, imag_interpolation_frac);

            for (int i_col = 0; i_col < size_x; ++i_col) {
                auto real_interpolation_frac = static_cast<float>(static_cast<float>(i_col) / ((size_x) - 1.0));
                float real_value = mandelbrot::interpolate(real_min, real_max, real_interpolation_frac);

                complex_set.push_back(real_value);
                complex_set.push_back(imag_value);
            }
        }
        return complex_set;
    }

    std::complex<double> mandelbrot_func(std::complex<double> z_val, std::complex<double> complex_val) {
        return std::pow(z_val, 2) + complex_val;
    }

    std::vector<int> mandelbrot_sequence(
            const std::vector<std::complex<double>> &complex_set,
            double threshold,
            int n_iterations
    ) {
        // outputs greyscale color for each pixel in mandelbrot set
        auto n_values = static_cast<int>(complex_set.size());

        std::vector<int> mandelbrot_set;
        mandelbrot_set.reserve(n_values);

        for (int idx_value = 0; idx_value < n_values; idx_value++) {

            auto complex_value = complex_set[idx_value];
            std::complex<double> z_value_iterated(0.0);

            int idx_iter = 0;
            for (; idx_iter < n_iterations; idx_iter++) {
                // if it is first iteration, use fc(0) = z**2 + c
                // on other iterations, use fc(fc(0)), or fc(fc(fc(0))), etc ...
                z_value_iterated = mandelbrot_func(z_value_iterated, complex_value);

                if (std::abs(z_value_iterated) > threshold) {
                    break;
                }
            }
            if (idx_iter == n_iterations) {
                mandelbrot_set.push_back(0); // black
            } else {
                mandelbrot_set.push_back(static_cast<int>(255 * (static_cast<double>(idx_iter) / n_iterations)));
            }
        }
        return mandelbrot_set;
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

    void complex_set_adjust_real(std::vector<float> &complex_set, const float &real_delta) {
        int n_elems = static_cast<int>(complex_set.size());
        for (int i = 0; i < n_elems; i += 2) {
            complex_set[i] += real_delta;
        }
    }

    void complex_set_adjust_imag(std::vector<float> &complex_set, const float &imag_delta) {
        int n_elems = static_cast<int>(complex_set.size());
        for (int i = 1; i < n_elems; i += 2) {
            complex_set[i] += imag_delta;
        }
    }

    void complex_set_adjust_scale(std::vector<float> &complex_set, const float &scale) {
        if (complex_set.empty()) {
            std::cerr << "Warning: Input vector is empty." << std::endl;
            return;
        }
        for (auto &elem: complex_set) {
            float result = elem * scale;

            // Check for underflow and overflow
            if (result < -std::numeric_limits<float>::max()) {
                std::cerr << "Warning: Set scaling underflow." << std::endl;
                elem = -std::numeric_limits<float>::max();
            } else if (result > std::numeric_limits<float>::max()) {
                std::cerr << "Warning: Set scaling overflow." << std::endl;
                elem = std::numeric_limits<float>::max();
            } else {
                elem = result;
            }
        }
    }

    void print_complex_set_bounds(const std::vector<float> &complex_set, const int &width, const int &height) {
        // bl - bottom left
        // br - bottom right
        // tl - top left
        // tr - top right
        float real_bl = complex_set.at(0);
        float imag_bl = complex_set.at(1);

        float real_br = complex_set.at((width * 2) - 2);
        float imag_br = complex_set.at((width * 2) - 1);

        float real_tl = complex_set.at((width * (height - 1) * 2));
        float imag_tl = complex_set.at((width * (height - 1) * 2) + 1);

        float real_tr = complex_set.at((width * height * 2) - 2);
        float imag_tr = complex_set.at((width * height * 2) - 1);

        spdlog::debug("Complex set bounds: BL=({}r {}i), BR=({}r {}i), TL=({}r {}i), TR=({}r {}i)",
                      real_bl, imag_bl,
                      real_br, imag_br,
                      real_tl, imag_tl,
                      real_tr, imag_tr
        );
    }
}
