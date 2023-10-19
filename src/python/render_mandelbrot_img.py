# This is a sample Python script.
import copy
import time

import cv2
import numpy as np
import matplotlib as mpl

colormap = mpl.colormaps["turbo"]


def check_set_divergence(
        n_iterations: int,
        complex_set: np._typing.NDArray,
        divergence_plot: np._typing.NDArray,
        threshold: float = 2.0
):
    already_set = np.zeros(shape=divergence_plot.shape[:2], dtype=bool)
    set_derived = copy.deepcopy(complex_set)

    for idx in range(n_iterations):
        idx_color = int(len(colormap.colors) * idx / n_iterations)
        color_rgb = colormap.colors[idx_color]
        color_bgr = np.clip(np.array(color_rgb[::-1]) * 255, 0, 255).astype(int)

        if idx > 0:
            vectorized = np.vectorize(mandelbrot)
            set_derived = vectorized(set_derived, complex_set)

        threshold_crossed = np.abs(set_derived) > threshold
        already_set[threshold_crossed] = True
        divergence_plot[np.logical_not(threshold_crossed, already_set)] = color_bgr

    return divergence_plot


def mandelbrot(z_value: int, complex_set: np._typing.NDArray) -> np._typing.NDArray:
    return z_value ** 2 + complex_set


def complex_array(
        dim_x: int,
        dim_y: int,
        min_real: float,
        max_real: float,
        min_imag: float,
        max_imag: float
) -> np._typing.NDArray:
    values_real = np.linspace(min_real, max_real, dim_x)
    values_imag = np.linspace(min_imag, max_imag, dim_y)

    array = np.zeros((dim_x, dim_y)).astype(np.complex_)
    for idx_row in range(dim_x):
        for idx_col in range(dim_y):
            array[idx_row, idx_col] = values_real[idx_row] + values_imag[idx_col] * 1j

    return array


def main():
    dim_x = 800
    dim_y = 800

    t_0 = time.perf_counter()

    divergence_plot = np.zeros((dim_y, dim_x, 3), dtype=np.uint8)

    complex_values = complex_array(dim_x, dim_y, -2, 1, -1, 1)
    mandelbrot_set = mandelbrot(0, complex_values)

    divergence_plot = check_set_divergence(10, mandelbrot_set, divergence_plot, threshold=2.0)
    print(f"Done in: {time.perf_counter() - t_0:.3f}")
    cv2.imwrite("plot.png", divergence_plot)


if __name__ == '__main__':
    main()
