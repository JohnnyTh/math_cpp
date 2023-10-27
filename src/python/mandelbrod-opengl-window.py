from pathlib import Path
import moderngl_window as mglw


class Mandelbrod(mglw.WindowConfig):
    gl_version = (3, 3)
    window_size = (1920, 1080)
    resource_dir = (Path(__file__).parent / "shaders/mandelbrod").resolve()

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.quad = mglw.geometry.quad_fs()
        self.prog = self.load_program(vertex_shader="vertex.vert",
                                      fragment_shader="fragment.frag")
        self.set_uniform("resolution", self.window_size)

    def set_uniform(self, u_name, u_value):
        try:
            self.prog[u_name] = u_value
        except KeyError:
            print(f"uniform {u_name} is not in shader")

    def render(self, time_, frametime):
        self.ctx.clear()
        self.quad.render(self.prog)


if __name__ == "__main__":
    mglw.run_window_config(Mandelbrod)
