import os
import sys
import time

import cv2
import numpy as np

# enable imports when running from project root
sys.path.append(os.environ["PWD"])

from src.python.mandelbrot import (gen_complex_set, 
                                   gen_complex_v1,
                                   gen_complex_v2,
                                   mandelbrot_sequence,
                                   mandelbrot_sequence_v1,
                                   mandelbrot_sequence_v2,)


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

    complex_set = gen_complex_v2(
        dim_x,
        dim_y,
        min_real=min_real,
        max_real=max_real,
        min_imag=min_imag,
        max_imag=max_imag,
    )
    mandelbrot_set = mandelbrot_sequence_v2(
        n_iterations, complex_set, mandelbrot_set, threshold=threshold
    )

    print(f"Done in: {time.perf_counter() - t_0:.3f}")
    cv2.imwrite("mandelbrot_set.png", mandelbrot_set)


if __name__ == "__main__":
    main()
