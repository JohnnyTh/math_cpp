//
// Created by maksym on 18/10/23.
//
#include <chrono>

#include <spdlog/spdlog.h>
#include <cxxopts.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/cpp/colormaps.hpp"
#include "src/cpp/mandelbrot.hpp"
#include "src/cpp/utilities_shaders.hpp"
#include "src/cpp/shaders_mandelbrot.hpp"

static void glfw_error_callback(int error, const char *description) {
    spdlog::error("GLFW Error {}: {}", error, description);
}

struct MouseState {
    bool dragging = false;
    double last_x = 0.0, last_y = 0.0;
};

struct AppState {
    mandelbrot::ViewParams *vp = nullptr;
    mandelbrot::ViewParams *vp_initial = nullptr;
    int width = 0, height = 0;
    bool view_dirty = false;
    MouseState mouse;
    int n_iterations = 100;
    int n_iters_delta = 1;
    int n_iters_delta_initial = 1;
    float threshold = 6.0f;
};

static void scroll_callback(GLFWwindow *window, double /*xoffset*/, double yoffset) {
    auto *app = static_cast<AppState *>(glfwGetWindowUserPointer(window));
    app->vp->zoom(1.0 - yoffset * app->vp->scale_delta * 5.0);
    app->view_dirty = true;
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int /*mods*/) {
    auto *app = static_cast<AppState *>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        app->mouse.dragging = (action == GLFW_PRESS);
        glfwGetCursorPos(window, &app->mouse.last_x, &app->mouse.last_y);
    }
}

static void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    auto *app = static_cast<AppState *>(glfwGetWindowUserPointer(window));
    switch (key) {
        case GLFW_KEY_X:
            app->n_iterations += app->n_iters_delta;
            spdlog::debug("N iterations: {}", app->n_iterations);
            break;
        case GLFW_KEY_Z:
            app->n_iterations -= app->n_iters_delta;
            spdlog::debug("N iterations: {}", app->n_iterations);
            break;
        case GLFW_KEY_R:
            app->vp->reset(*app->vp_initial);
            app->n_iters_delta = app->n_iters_delta_initial;
            spdlog::debug("RESET complex set!");
            app->view_dirty = true;
            break;
        case GLFW_KEY_UP:    app->vp->pan_imag(app->vp->imag_delta);  app->view_dirty = true; break;
        case GLFW_KEY_DOWN:  app->vp->pan_imag(-app->vp->imag_delta); app->view_dirty = true; break;
        case GLFW_KEY_LEFT:  app->vp->pan_real(-app->vp->real_delta);  app->view_dirty = true; break;
        case GLFW_KEY_RIGHT: app->vp->pan_real(app->vp->real_delta);   app->view_dirty = true; break;
        case GLFW_KEY_MINUS: app->vp->zoom(1.0 + app->vp->scale_delta); app->view_dirty = true; break;
        case GLFW_KEY_EQUAL: app->vp->zoom(1.0 - app->vp->scale_delta); app->view_dirty = true; break;
        default: break;
    }
}

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    auto *app = static_cast<AppState *>(glfwGetWindowUserPointer(window));
    if (!app->mouse.dragging) return;

    double dx = (xpos - app->mouse.last_x) / app->width;
    double dy = (ypos - app->mouse.last_y) / app->height;
    app->mouse.last_x = xpos;
    app->mouse.last_y = ypos;

    double real_range = app->vp->real_max - app->vp->real_min;
    double imag_range = app->vp->imag_max - app->vp->imag_min;
    app->vp->pan_real(-dx * real_range);
    app->vp->pan_imag(dy * imag_range);  // ponytail: y flipped — screen y grows down, imag grows up
    app->view_dirty = true;
}

std::chrono::time_point<std::chrono::steady_clock> time_last = std::chrono::steady_clock::now();

int fps_count = 0;
int fps = 0;

void get_fps() {
    auto time_current = std::chrono::steady_clock::now();

    const auto time_elapsed = (
            std::chrono::duration_cast<std::chrono::nanoseconds>(time_current - time_last)
                    .count()
    );
    ++fps_count;

    if (time_elapsed > 1000000000) {
        time_last = time_current;
        fps = fps_count;
        fps_count = 0;

        spdlog::info("fps: {}", fps);
    }
}

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);

    cxxopts::Options options{argv[0], "Mandelbrot set image rendering tool"};
    options.add_options()
            ("w,width", "Image width", cxxopts::value<int>())
            ("h,height", "Image height", cxxopts::value<int>())
            ("rmin,real_min", "Real number minimum", cxxopts::value<float>()->default_value("-2.5"))
            ("rmax,real_max", "Real number maximum", cxxopts::value<float>()->default_value("1.0"))
            ("imin,imag_min", "Imaginary number minimum", cxxopts::value<float>()->default_value("-1.1"))
            ("imax,imag_max", "Imaginary number maximum", cxxopts::value<float>()->default_value("1.1"))
            ("i,n_iterations", "Number of iterations", cxxopts::value<int>()->default_value("100"))
            ("t,threshold", "Abs value threshold", cxxopts::value<float>()->default_value("6.0"));
    auto result = options.parse(argc, argv);

    // Initialise GLFW
    glfwSetErrorCallback(glfw_error_callback);

    if (glfwInit() != 1) {
        spdlog::error("Failed to initialize GLFW");
        return -1;
    }

    GLFWmonitor *primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primary);

    int width = result.count("width") ? result["width"].as<int>() : mode->width;
    int height = result.count("height") ? result["height"].as<int>() : mode->height;

    mandelbrot::ViewParams vp_initial{
        result["real_min"].as<float>(), result["real_max"].as<float>(),
        result["imag_min"].as<float>(), result["imag_max"].as<float>(),
        0.025, 0.025, 0.01
    };
    mandelbrot::ViewParams vp = vp_initial;
    AppState app{};
    app.vp = &vp;
    app.vp_initial = &vp_initial;
    app.width = width;
    app.height = height;
    app.n_iterations = result["n_iterations"].as<int>();
    app.threshold = result["threshold"].as<float>();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(
            GLFW_OPENGL_PROFILE,
            GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow *window;
    window = glfwCreateWindow(
            width, height, "Mandelbrot render",
            nullptr, nullptr
    );

    if (window == nullptr) {
        spdlog::error(
                "Failed to open GLFW window. "
                "If you have an Intel GPU, they are not 3.3 compatible."
        );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    // glfwGetFramebufferSize(window, &width, &height);
    spdlog::info("framebuffer width={}, height={}", width, height);

    // enable GLEW
    if (glewInit() != GLEW_OK) {
        spdlog::error("Failed to initialize GLEW");
        return -1;
    }

    // --------- VBO definition
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
            -1.0f, 1.0f, 0.0f, 1.0f, // Top-left
            1.0f, 1.0f, 1.0f, 1.0f, // Top-right
    };
    unsigned int indices[] = {0, 1, 2, 1, 2, 3};

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // --------- VAO definition
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(0);

    GLuint shader_program = utils_shaders::LoadShadersFromSource(shaders_mandelbrot::VERTEX_SRC, shaders_mandelbrot::FRAGMENT_SRC);

    if (shader_program == 0) {
        spdlog::error("Error loading shader");
        return -1;
    }

    // ------------ create COMPLEX VALUES TEXTURE ----------------------------
    std::vector<float> complex_set = mandelbrot::gen_complex_set_2_shader(width, height, vp);
    mandelbrot::print_complex_set_bounds(complex_set, width, height);

    GLuint tex_complex;
    glGenTextures(1, &tex_complex);
    glBindTexture(GL_TEXTURE_2D, tex_complex);
    // GL_RG stores 2 values per pixel - Red, Green
    // Red will store real part of complex number and Green - imaginary
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, complex_set.data()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // -------------------------------------------------------------

    // --------------- create COLORMAP TEXTURE ------------------------------
    int n_colors = sizeof(colormaps::cmap_ocean_256) / (3 * sizeof(colormaps::cmap_ocean_256[0]));

    GLuint tex_colormap;
    glGenTextures(1, &tex_colormap);
    glBindTexture(GL_TEXTURE_1D, tex_colormap);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, n_colors, 0, GL_RGB, GL_FLOAT, colormaps::cmap_ocean_256);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // ------------ get locations of dynamic uniform parameters
    GLint loc_threshold = glGetUniformLocation(shader_program, "threshold");
    GLint loc_n_iterations = glGetUniformLocation(shader_program, "n_iterations");
    GLint loc_colormap = glGetUniformLocation(shader_program, "colormap");
    GLint loc_complex_set = glGetUniformLocation(shader_program, "complexSet");

    int tex_unit_complex_set = 1;
    int tex_unit_colormap = 0;

    glfwSetWindowTitle(window, "Mandelbrot | zoom: 1x");

    GLenum err_setup = glGetError();
    if (err_setup != 0) {
        spdlog::error("Got !=0 error code from OpenGL: {}", err_setup);
        return -1;
    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0) {

        get_fps();

        // Render on the whole framebuffer, complete from the lower left corner to
        // the upper right
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        // --------------------- Draw section -------------------------
        if (app.view_dirty) {
            app.view_dirty = false;
            complex_set = mandelbrot::gen_complex_set_2_shader(width, height, vp);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, complex_set.data());
            double zoom = (vp_initial.real_max - vp_initial.real_min) / (vp.real_max - vp.real_min);
            glfwSetWindowTitle(window, ("Mandelbrot | zoom: " + std::to_string((long long)zoom) + "x").c_str());
        }
        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0 + tex_unit_complex_set);
        glBindTexture(GL_TEXTURE_2D, tex_complex);
        glUniform1i(loc_complex_set, tex_unit_complex_set);

        glActiveTexture(GL_TEXTURE0 + tex_unit_colormap);
        glBindTexture(GL_TEXTURE_1D, tex_colormap);
        glUniform1i(loc_colormap, tex_unit_colormap);

        glUniform1f(loc_threshold, app.threshold);
        glUniform1i(loc_n_iterations, app.n_iterations);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // -------------------- END draw section ----------------------

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Delete buffer
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // Delete vertex array
    glDeleteVertexArrays(1, &VAO);
    // Delete shader program
    glDeleteProgram(shader_program);
    // Delete textures
    glDeleteTextures(1, &tex_complex);
    glDeleteTextures(1, &tex_colormap);
    // terminate GLFW and exiting
    glfwTerminate();
    return 0;
}