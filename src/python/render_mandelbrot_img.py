# This is a sample Python script.
import time

import cv2
import numpy as np
import matplotlib as mpl

colormap = mpl.colormaps["turbo"]


def mandelbrot(z_value: float, complex_set: np._typing.NDArray) -> np._typing.NDArray:
    return z_value**2 + complex_set


mandelbrot_vec = np.vectorize(mandelbrot)


def mandelbrot_sequence(
    n_iterations: int,
    complex_set: np._typing.NDArray,
    mandelbrot_set: np._typing.NDArray,
    threshold: float = 2.0,
) -> np._typing.NDArray:
    already_set = np.zeros(shape=mandelbrot_set.shape[:2], dtype=bool)

    z_value_iterated = 0.0
    for idx in range(n_iterations):
        if idx > 0:
            z_value_iterated = mandelbrot_vec(z_value_iterated, complex_set)

        idx_color = int(len(colormap.colors) * idx / n_iterations)
        color_rgb = colormap.colors[idx_color]
        color_bgr = np.clip(np.array(color_rgb[::-1]) * 255, 0, 255).astype(int)

        threshold_crossed = np.abs(z_value_iterated) > threshold
        already_set[threshold_crossed] = True
        mandelbrot_set[np.logical_not(threshold_crossed, already_set)] = color_bgr

    return mandelbrot_set


def gen_complex_set(
    dim_x: int,
    dim_y: int,
    min_real: float,
    max_real: float,
    min_imag: float,
    max_imag: float,
) -> np._typing.NDArray:
    values_real = np.linspace(min_real, max_real, dim_x)
    values_imag = np.linspace(min_imag, max_imag, dim_y)

    set_ = np.zeros((dim_y, dim_x)).astype(np.complex_)
    for idx_col in range(dim_y):
        for idx_row in range(dim_x):
            set_[idx_col, idx_row] = values_real[idx_row] + values_imag[idx_col] * 1j

    return set_


def main():
    dim_x = 1980
    dim_y = 1080

    min_real = -2.5
    max_real = 1.0
    min_imag = -1.5
    max_imag = 1.5

    n_iterations = 10
    threshold = 2.0

    t_0 = time.perf_counter()

    mandelbrot_set = np.zeros((dim_y, dim_x, 3), dtype=np.uint8)

    complex_set = gen_complex_set(
        dim_x,
        dim_y,
        min_real=min_real,
        max_real=max_real,
        min_imag=min_imag,
        max_imag=max_imag,
    )
    mandelbrot_set = mandelbrot_sequence(
        n_iterations, complex_set, mandelbrot_set, threshold=threshold
    )

    print(f"Done in: {time.perf_counter() - t_0:.3f}")
    cv2.imwrite("mandelbrot_set.png", mandelbrot_set)


if __name__ == "__main__":
    main()
