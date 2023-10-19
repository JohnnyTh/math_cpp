#include <complex>
#include <vector>

#include "utilities.hpp"

namespace mandelbrot {

    struct ComplexVal {
        float real;
        float imag;

        ComplexVal(float r, float i) {
            real = r;
            imag = i;
        }
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

    std::vector<float> gen_complex_set_float(
            int size_x,
            int size_y,
            float real_min,
            float real_max,
            float imag_min,
            float imag_max) {
        std::vector<float> complex_set;

        for (int i = 0; i < size_y; i++) {
            auto imag_interpolation_frac = static_cast<float>(i / (size_y - 1.0));
            float imag_value = mandelbrot::interpolate(imag_min, imag_max, imag_interpolation_frac);

            for (int j = 0; j < size_x; ++j) {
                auto real_interpolation_frac = static_cast<float>(j / ((size_x) - 1.0));
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
            std::vector<std::complex<double>> &complex_set,
            double threshold,
            int n_iterations
    ) {
        auto n_values = static_cast<int>(complex_set.size());

        std::vector<int> threshold_crossed_at_iteration(n_values, -1);

        for (int idx_value = 0; idx_value < n_values; idx_value++) {

            auto complex_value = complex_set[idx_value];
            std::complex<double> z_value_iterated(0.0);

            for (int idx_iter = 0; idx_iter < n_iterations; idx_iter++) {

                // if it is first iteration, use fc(0) = z**2 + c
                // on other iterations, use fc(fc(0)), or fc(fc(fc(0))), etc ...
                z_value_iterated = mandelbrot_func(z_value_iterated, complex_value);

                if (abs(z_value_iterated) > threshold) {
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
