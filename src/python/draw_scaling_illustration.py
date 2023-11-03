import numpy as np
import matplotlib.pyplot as plt


def draw_offset():
    scale_factor = 0.75
    dim_x = 8
    dim_y = 8
    min_real = -2.5
    max_real = 1.0
    min_imag = -1.1
    max_imag = 1.1

    values_real = np.repeat(np.linspace(min_real, max_real, dim_x), dim_x)
    values_imag = np.tile(np.linspace(min_imag, max_imag, dim_y), dim_y)

    plt.figure(dpi=150)

    plt.scatter(
        values_real,
        values_imag,
        alpha=0.6,
        edgecolor="black",
        color="g",
        label="original pixels",
    )
    plt.scatter(
        values_real * scale_factor,
        values_imag * scale_factor,
        alpha=0.6,
        edgecolor="black",
        color="r",
        label=f"pixels scaled by {scale_factor}",
    )

    plt.tight_layout(pad=2.0)

    ax = plt.gca()

    ax.legend()

    ax.set_xlim([-3.75, 2.25])
    ax.set_ylim([-1.75, 1.75])

    ax.axhline(0, color="black", linestyle="--")
    ax.axvline(0, color="black", linestyle="--")

    plt.xlabel("Real axis")
    plt.ylabel("Imaginary axis")

    plt.savefig("scaling_illustration_offset.png", transparent=False)


def draw_centered():
    scale_factor = 0.75
    dim_x = 8
    dim_y = 8
    min_real = -1.0
    max_real = 1.0
    min_imag = -1.1
    max_imag = 1.1

    values_real = np.repeat(np.linspace(min_real, max_real, dim_x), dim_x)
    values_imag = np.tile(np.linspace(min_imag, max_imag, dim_y), dim_y)

    plt.figure(dpi=150)

    plt.scatter(
        values_real,
        values_imag,
        alpha=0.6,
        edgecolor="black",
        color="g",
        label="original pixels",
    )
    plt.scatter(
        values_real * scale_factor,
        values_imag * scale_factor,
        alpha=0.6,
        edgecolor="black",
        color="r",
        label=f"pixels scaled by {scale_factor}",
    )

    plt.tight_layout(pad=2.0)

    ax = plt.gca()

    ax.legend()

    ax.set_xlim([-2.0, 2.0])
    ax.set_ylim([-1.75, 1.75])

    ax.axhline(0, color="black", linestyle="--")
    ax.axvline(0, color="black", linestyle="--")

    plt.xlabel("Real axis")
    plt.ylabel("Imaginary axis")

    plt.savefig("scaling_illustration_centered.png", transparent=False)


if __name__ == "__main__":
    draw_offset()
    draw_centered()
