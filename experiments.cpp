#include <complex>

template<typename T>
std::complex<double> mandelbrot_func(T z_val, std::complex<double> complex_val) {
    return std::pow(std::complex<double>(z_val), 2) + complex_val;
}

int main() {
    std::complex<double> complex(2.0, 1.0);
    std::complex<double> complex_0(0.0);

    auto output_1 = mandelbrot_func(0.0, complex);
    auto output_2 = mandelbrot_func(complex_0, complex);

    return 0;
}