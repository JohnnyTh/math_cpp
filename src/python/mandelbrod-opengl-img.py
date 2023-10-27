import time

import numpy as np
import moderngl
from PIL import Image


"""
Inspired by:
- https://www.youtube.com/watch?v=h5PuIm6fRr8
- https://www.youtube.com/watch?v=sW56us0ZBEQ
- https://github.com/JJuiice/fractals/blob/main/main.py
- https://github.com/shBLOCK/FractalExplorer
"""


def main():

    with open("src/python/shaders/mandelbrod/vertex.vert", 'rt', encoding="utf8") as v, \
        open("src/python/shaders/mandelbrod/fragment.frag", 'rt', encoding="utf8") as f:
        vert = v.read()
        frag = f.read()

    t_0 = time.perf_counter()

    ctx = moderngl.create_standalone_context()
    prog = ctx.program(
        vertex_shader=vert,
        fragment_shader=frag,
    )

    resolution = (1920, 1080)

    prog["resolution"] = np.array(resolution)

    vertices = np.array([-1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0], dtype='f4')

    vbo = ctx.buffer(vertices)
    vao = ctx.simple_vertex_array(prog, vbo, 'in_position')

    fbo = ctx.simple_framebuffer(resolution)
    fbo.use()
    fbo.clear(0.0, 0.0, 0.0, 1.0)
    vao.render(moderngl.TRIANGLE_STRIP)

    img = Image.frombytes('RGB', fbo.size, fbo.read(), 'raw', 'RGB', 0, -1)

    print(f"Done in: {time.perf_counter() - t_0:.3f}")

    img.save("mandelbrod-set-opengl-py-img.png")


if __name__ == "__main__":
    main()
