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

static void glfw_error_callback(int error, const char *description) {
    spdlog::error("GLFW Error {}: {}", error, description);
}

std::chrono::time_point<std::chrono::steady_clock> time_last = std::chrono::steady_clock::now();

int fps_count = 0;
int fps = 0;

float real_delta = 0.05f;
float imag_delta = 0.05f;
float scale_delta = 0.05f;


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
            ("w,width", "Image width", cxxopts::value<int>()->default_value("1980"))
            ("h,height", "Image height", cxxopts::value<int>()->default_value("1080"))
            ("rmin,real_min", "Real number minimum", cxxopts::value<float>()->default_value("-2.5"))
            ("rmax,real_max", "Real number maximum", cxxopts::value<float>()->default_value("1.0"))
            ("imin,imag_min", "Imaginary number minimum", cxxopts::value<float>()->default_value("-1.1"))
            ("imax,imag_max", "Imaginary number maximum", cxxopts::value<float>()->default_value("1.1"))
            ("i,n_iterations", "Number of iterations", cxxopts::value<int>()->default_value("1000"))
            ("t,threshold", "Abs value threshold", cxxopts::value<float>()->default_value("6.0"));
    auto result = options.parse(argc, argv);

    int width = result["width"].as<int>();
    int height = result["height"].as<int>();

    float real_min = result["real_min"].as<float>();
    float real_max = result["real_max"].as<float>();
    float imag_min = result["imag_min"].as<float>();
    float imag_max = result["imag_max"].as<float>();

    int n_iterations = result["n_iterations"].as<int>();
    float threshold = result["threshold"].as<float>();

    // Initialise GLFW
    glfwSetErrorCallback(glfw_error_callback);

    if (glfwInit() != 1) {
        spdlog::error("Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(
            GLFW_OPENGL_PROFILE,
            GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow *window;
    window = glfwCreateWindow(width, height, "Mandelbrot render", nullptr, nullptr);

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
    glfwGetFramebufferSize(window, &width, &height);
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

    GLuint shader_program = utils_shaders::LoadShaders("vertex.vert", "fragment_mb.frag");

    if (shader_program == 0) {
        spdlog::error("Error loading shader");
        return -1;
    }

    // ------------ create COMPLEX VALUES TEXTURE ----------------------------
    std::vector<float> complex_set = mandelbrot::gen_complex_set_2_shader(
            width, height, real_min, real_max, imag_min, imag_max
    );
    mandelbrot::print_complex_set_bounds(complex_set, width, height);

    GLuint tex_complex;
    glGenTextures(1, &tex_complex);
    glBindTexture(GL_TEXTURE_2D, tex_complex);
    // GL_RG stores 2 values per pixel - Red, Green
    // Red will store real part of complex number and Green - imaginary
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, complex_set.data()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // -------------------------------------------------------------

    // --------------- create COLORMAP TEXTURE ------------------------------
    int n_colors = sizeof(colormaps::cmap_gist_ncar_256) / (3 * sizeof(colormaps::cmap_gist_ncar_256[0]));

    GLuint tex_colormap;
    glGenTextures(1, &tex_colormap);
    glBindTexture(GL_TEXTURE_1D, tex_colormap);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, n_colors, 0, GL_RGB, GL_FLOAT, colormaps::cmap_gist_ncar_256);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // ------------ get locations of dynamic uniform parameters
    GLint loc_threshold = glGetUniformLocation(shader_program, "threshold");
    GLint loc_n_iterations = glGetUniformLocation(shader_program, "n_iterations");
    GLint loc_colormap = glGetUniformLocation(shader_program, "colormap");
    GLint loc_complex_set = glGetUniformLocation(shader_program, "complexSet");

    int tex_unit_complex_set = 1;
    int tex_unit_colormap = 0;

    GLenum err_setup = glGetError();
    if (err_setup != 0) {
        spdlog::error("Got !=0 error code from OpenGL: {}", err_setup);
        return -1;

    }
    // TODO: add reset button
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0) {

        bool is_complex_set_modified = false;

        get_fps();

        // --------------------- Handle user controls -------------------------
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            // imag += imag_diff;
            mandelbrot::complex_set_adjust_imag(complex_set, imag_delta);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            is_complex_set_modified = true;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            // imag -= imag_diff;
            mandelbrot::complex_set_adjust_imag(complex_set, -imag_delta);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            is_complex_set_modified = true;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            // real -= real_delta;
            mandelbrot::complex_set_adjust_real(complex_set, -real_delta);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            is_complex_set_modified = true;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            // real += real_delta;
            mandelbrot::complex_set_adjust_real(complex_set, real_delta);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            is_complex_set_modified = true;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
            // zoom out (increase all values)
            float scale_adjust = 1 + scale_delta;
            mandelbrot::complex_set_adjust_scale(complex_set, scale_adjust);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            real_delta = real_delta * scale_adjust;
            imag_delta = imag_delta * scale_adjust;
            is_complex_set_modified = true;
        }
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
            // zoom in (decrease all values)
            float scale_adjust = 1 - scale_delta;
            mandelbrot::complex_set_adjust_scale(complex_set, scale_adjust);
            mandelbrot::print_complex_set_bounds(complex_set, width, height);
            real_delta = real_delta * scale_adjust;
            imag_delta = imag_delta * scale_adjust;
            is_complex_set_modified = true;
        }
        // Render on the whole framebuffer, complete from the lower left corner to
        // the upper right
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT);
        // --------------------- Draw section -------------------------
        if (is_complex_set_modified) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RG, GL_FLOAT, complex_set.data());
        }
        glUseProgram(shader_program);

        glActiveTexture(GL_TEXTURE0 + tex_unit_complex_set);
        glBindTexture(GL_TEXTURE_2D, tex_complex);
        glUniform1i(loc_complex_set, tex_unit_complex_set);

        glActiveTexture(GL_TEXTURE0 + tex_unit_colormap);
        glBindTexture(GL_TEXTURE_1D, tex_colormap);
        glUniform1i(loc_colormap, tex_unit_colormap);

        glUniform1f(loc_threshold, threshold);
        glUniform1i(loc_n_iterations, n_iterations);

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