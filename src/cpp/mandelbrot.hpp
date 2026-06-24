#include <complex>
#include <vector>
#include <istream>
#include <iostream>

#include "spdlog/spdlog.h"

#include "utilities.hpp"

#ifndef MANDELBROT_HPP
#define MANDELBROT_HPP

namespace mandelbrot {

    struct ViewParams {
        double real_min, real_max, imag_min, imag_max;
        double real_delta, imag_delta, scale_delta;

        void pan_real(double d) { real_min += d; real_max += d; }
        void pan_imag(double d) { imag_min += d; imag_max += d; }

        void zoom(double scale) {
            double r_center = (real_min + real_max) / 2.0;
            double i_center = (imag_min + imag_max) / 2.0;
            real_min = r_center + (real_min - r_center) * scale;
            real_max = r_center + (real_max - r_center) * scale;
            imag_min = i_center + (imag_min - i_center) * scale;
            imag_max = i_center + (imag_max - i_center) * scale;
            real_delta *= scale;
            imag_delta *= scale;
        }

        void reset(const ViewParams &initial) { *this = initial; }
    };

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

    // split double into hi+lo floats such that (double)hi + (double)lo == val
    inline std::pair<float, float> dsplit(double val) {
        float hi = (float)val;
        float lo = (float)(val - (double)hi);
        return {hi, lo};
    }

    // stores real_hi, real_lo, imag_hi, imag_lo per pixel (4 floats)
    std::vector<float> gen_complex_set_2_shader(int size_x, int size_y, const ViewParams &vp) {
        std::vector<float> complex_set;
        complex_set.reserve(size_x * size_y * 4);

        for (int i_row = 0; i_row < size_y; i_row++) {
            double imag_frac = static_cast<double>(i_row) / (size_y - 1.0);
            double imag_value = mandelbrot::interpolate(vp.imag_min, vp.imag_max, imag_frac);
            auto [imag_hi, imag_lo] = dsplit(imag_value);

            for (int i_col = 0; i_col < size_x; ++i_col) {
                double real_frac = static_cast<double>(i_col) / (size_x - 1.0);
                double real_value = mandelbrot::interpolate(vp.real_min, vp.real_max, real_frac);
                auto [real_hi, real_lo] = dsplit(real_value);

                complex_set.push_back(real_hi);
                complex_set.push_back(real_lo);
                complex_set.push_back(imag_hi);
                complex_set.push_back(imag_lo);
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

    void complex_set_adjust_scale_centered(std::vector<float> &complex_set, const float &scale) {
        if (complex_set.empty()) {
            std::cerr << "Warning: Input vector is empty." << std::endl;
            return;
        }
        float rmin = complex_set.at(0);
        float rmax = complex_set.at(complex_set.size() - 2);

        float imin = complex_set.at(1);
        float imax = complex_set.at(complex_set.size() - 1);

        float r_center = (rmin + rmax) / 2;
        float i_center = (imin + imax) / 2;

        for (size_t i = 0; i < complex_set.size(); i++) {
            auto &elem = complex_set[i];
            float result;

            if (i % 2 == 0) {
                // imag values
                result = ((elem - r_center) * scale) + r_center;
            } else {
                // real values
                result = ((elem - i_center) * scale) + i_center;
            }
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

#endif