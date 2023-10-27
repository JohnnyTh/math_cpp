import typing

import numpy as np
import matplotlib as mpl

colormap = mpl.colormaps["turbo"]

colors_bgr = np.clip(np.array(colormap.colors[::-1]) * 255, 0, 255).astype(int)

def mandelbrot(
    z_value: typing.Union[float, complex], complex_set: np._typing.NDArray
) -> np._typing.NDArray:
    return z_value**2 + complex_set


mandelbrot_vec = np.vectorize(mandelbrot)


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

    complex_set = np.zeros((dim_y, dim_x)).astype(np.complex_)
    for idx_col in range(dim_y):
        for idx_row in range(dim_x):
            complex_set[idx_col, idx_row] = values_real[idx_row] + values_imag[idx_col] * 1j

    return complex_set


def gen_complex_v1(
    dim_x: int,
    dim_y: int,
    min_real: float,
    max_real: float,
    min_imag: float,
    max_imag: float,
) -> np._typing.NDArray:
    values_real = np.linspace(min_real, max_real, dim_x)
    values_imag = np.linspace(min_imag, max_imag, dim_y) * 1j

    complex_set = np.zeros((dim_y, dim_x)).astype(np.complex_)
    for idx_col in range(dim_y):
        for idx_row in range(dim_x):
            complex_set[idx_col, idx_row] = values_real[idx_row] + values_imag[idx_col]

    return complex_set


def gen_complex_v2(
    dim_x: int,
    dim_y: int,
    min_real: float,
    max_real: float,
    min_imag: float,
    max_imag: float,
) -> np._typing.NDArray:
    values_real = np.linspace(min_real, max_real, dim_x)
    values_imag = np.linspace(min_imag, max_imag, dim_y) * 1j

    product = np.transpose([np.tile(values_real, len(values_imag)), np.repeat(values_imag, len(values_real))])
    complex_set = product.sum(axis=1).reshape(dim_y,-1)

    return complex_set


def mandelbrot_sequence(
    n_iterations: int,
    complex_set: np._typing.NDArray,
    mandelbrot_set: np._typing.NDArray,
    threshold: float = 2.0,
) -> np._typing.NDArray:
    already_set = np.zeros(shape=mandelbrot_set.shape[:2], dtype=bool)

    z_value_iterated = 0.0
    for idx in range(n_iterations):
        z_value_iterated = mandelbrot_vec(z_value_iterated, complex_set)

        idx_color = int(len(colormap.colors) * idx / n_iterations)
        color_rgb = colormap.colors[idx_color]
        color_bgr = np.clip(np.array(color_rgb[::-1]) * 255, 0, 255).astype(int)

        threshold_crossed = np.abs(z_value_iterated) > threshold
        already_set[threshold_crossed] = True
        mandelbrot_set[np.logical_not(threshold_crossed, already_set)] = color_bgr

    return mandelbrot_set


def mandelbrot_sequence_v1(
    n_iterations: int,
    complex_set: np._typing.NDArray,
    mandelbrot_set: np._typing.NDArray,
    threshold: float = 2.0,
) -> np._typing.NDArray:
    already_set = np.zeros(shape=mandelbrot_set.shape[:2], dtype=bool)

    z_value_iterated = 0.0
    for idx in range(n_iterations):
        z_value_iterated = mandelbrot_vec(z_value_iterated, complex_set)

        idx_color = int(len(colormap.colors) * idx / n_iterations)
        color_bgr = colors_bgr[idx_color]

        threshold_crossed = np.abs(z_value_iterated) > threshold
        already_set[threshold_crossed] = True
        mandelbrot_set[np.logical_not(threshold_crossed, already_set)] = color_bgr

    return mandelbrot_set


def mandelbrot_sequence_v2(
    n_iterations: int,
    complex_set: np._typing.NDArray,
    mandelbrot_set: np._typing.NDArray,
    threshold: float = 2.0,
) -> np._typing.NDArray:
    already_set = np.zeros(shape=mandelbrot_set.shape[:2], dtype=bool)

    z_value_iterated = 0.0
    for idx in range(n_iterations):
        z_value_iterated = mandelbrot(z_value_iterated, complex_set)

        idx_color = int(len(colormap.colors) * idx / n_iterations)
        color_bgr = colors_bgr[idx_color]

        threshold_crossed = np.abs(z_value_iterated) > threshold
        already_set[threshold_crossed] = True
        mandelbrot_set[np.logical_not(threshold_crossed, already_set)] = color_bgr

    return mandelbrot_set
