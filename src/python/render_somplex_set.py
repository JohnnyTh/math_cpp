import os
import sys

import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

from skimage import color

# enable imports when running from project root
sys.path.append(os.environ["PWD"])

from src.python.mandelbrot import gen_complex_set


def complex_grid_plot(complex_set):
    fig, (ax1, ax2) = plt.subplots(
        1, 2, figsize=(14, 6)
    )  # create two subplots side by side

    real_part = np.real(complex_set)  # extract the real part of the complex numbers
    imag_part = np.imag(
        complex_set
    )  # extract the imaginary part of the complex numbers

    sns.heatmap(real_part, ax=ax1, cbar=False)
    ax1.set_title("Real Part")
    ax1.set_xticks([])
    ax1.set_yticks([])

    sns.heatmap(imag_part, ax=ax2, cbar=False)
    ax2.set_title("Imaginary Part")
    ax2.set_xticks([])
    ax2.set_yticks([])

    plt.tight_layout()  # this ensures that the subplots do not overlap
    fig.savefig("complex_set3.png")


def hue_value_plot(complex_set):
    fig, ax = plt.subplots(dpi=200, figsize=(10, 7))

    phases = np.angle(complex_set, deg=True)
    # Convert from [-180, 180] to [0, 1]
    # For the complex plane, the angle ranges from -180 to 180.
    # We normalize this to range [0, 1] to fit into the colormap range.
    phases = (phases + 180.0) / 360.0

    x_ticks = np.real(complex_set[0, :])
    y_ticks = np.imag(complex_set[:, 0])

    cm = plt.get_cmap("twilight")
    plt.imshow(cm(phases), origin="lower")

    for (j, i), value in np.ndenumerate(complex_set):
        # Access the RGB values of the pixel
        rgb = cm(phases[j, i])
        # Convert to xyz and then calculate luminance
        xyz = color.rgb2xyz([[[rgb[0], rgb[1], rgb[2]]]])
        luminance = xyz[0, 0, 1]

        # Based on luminance decide foreground color
        text_color = "black" if luminance > 0.5 else "white"

        if np.imag(value) < 0:
            label = str(f"{np.real(value):.1f}{np.imag(value):.1f}i")
        else:
            label = str(f"{np.real(value):.1f}+{np.imag(value):.1f}i")

        ax.text(i, j, label, ha="center", va="center", color=text_color)

    plt.xticks([])
    plt.yticks([])

    plt.xticks(np.arange(len(x_ticks)), ["%.1f" % val for val in np.unique(x_ticks)])
    plt.yticks(np.arange(len(y_ticks)), [f"{val:.1f}i" for val in np.unique(y_ticks)])

    plt.xlabel("Real axis")
    plt.ylabel("Imaginary axis")

    plt.subplots_adjust(left=0, right=1, top=1, bottom=0.1)
    fig.savefig("complex_set2.png", transparent=True)


def main():
    dim_x = 10
    dim_y = 8

    min_real = -2.5
    max_real = 1.0
    min_imag = -1.5
    max_imag = 1.5

    complex_set = gen_complex_set(dim_x, dim_y, min_real, max_real, min_imag, max_imag)
    hue_value_plot(complex_set)
    complex_grid_plot(complex_set)

    print()


if __name__ == "__main__":
    main()
