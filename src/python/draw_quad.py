import matplotlib.pyplot as plt
import matplotlib.patches as patches


def main():
    vertices = [
        [-1.0, -1.0],  # Bottom-left
        [1.0, -1.0],  # Bottom-right
        [-1.0, 1.0],  # Top-left
        [1.0, 1.0],  # Top-right
    ]

    connections = [
        (0, 1),
        (1, 2),
        (2, 3),
    ]

    fig, ax = plt.subplots()
    for idx, vertex in enumerate(vertices):
        ax.plot(vertex[0], vertex[1], "o")

        vert_x = vertex[0]
        vert_y = vertex[1]

        vert_x = vert_x + (vert_x * 0.1)
        vert_y = vert_y + (vert_y * 0.1)

        ax.text(
            vert_x,
            vert_y,
            f"{idx}",
            horizontalalignment="center",
            verticalalignment="center",
            fontsize=12,
            color="black",
        )

    for conn in connections:
        ax.annotate(
            "",
            xy=vertices[conn[1]],
            xytext=vertices[conn[0]],
            arrowprops=dict(arrowstyle="->"),
        )

    # Draw the quad
    quad_1 = patches.Polygon(
        (vertices[0], vertices[1], vertices[2]),
        closed=True,
        fill=True,
        edgecolor="r",
        color="r",
        alpha=0.3,
    )
    quad_2 = patches.Polygon(
        (vertices[1], vertices[2], vertices[3]),
        closed=True,
        fill=True,
        edgecolor="r",
        color="g",
        alpha=0.3,
    )
    ax.add_patch(quad_1)
    ax.add_patch(quad_2)

    ax.text(
        -0.5,
        -0.5,
        "Triangle 1",
        horizontalalignment="center",
        verticalalignment="center",
        fontsize=12,
        color="k",
        # fontstyle='oblique',
        weight="bold",
        fontfamily="serif",
    )
    ax.text(
        0.5,
        0.5,
        "Triangle 2",
        horizontalalignment="center",
        verticalalignment="center",
        fontsize=12,
        color="k",
        # fontstyle='oblique'
        weight="bold",
        fontfamily="serif",
    )

    # Set axis limits and show plot
    ax.set_xlim([-1.5, 1.5])
    ax.set_ylim([-1.5, 1.5])
    plt.tight_layout()
    plt.savefig("quad.png", transparent=False)


if __name__ == "__main__":
    main()
