#include <chrono>
#include "mandelbrot.hpp"


int main() {
    auto start = std::chrono::high_resolution_clock::now();

    int size_x = 800;
    int size_y = 800;

    double real_min = -2.0;
    double real_max = 1.0;
    double imag_min = -1.0;
    double imag_max = 1.0;

    int n_iterations = 10;
    double threshold = 2.0;

    (void) printf("Generating Mandelbrot set\n");

    auto complex_set = mandelbrot::gen_complex_set(size_x, size_y, real_min, real_max, imag_min, imag_max);

    auto threshold_crossed_at_iter = mandelbrot::mandelbrot_sequence(complex_set, threshold, n_iterations);

    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    (void) printf("Elapsed time: %.9f seconds\n", elapsed.count());

    //mandelbrot::print_complex_set(complex_set);
    //mandelbrot::print_vector(threshold_crossed_at_iter);
    return 0;
}
