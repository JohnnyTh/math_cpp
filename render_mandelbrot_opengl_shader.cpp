//
// Created by maksym on 18/10/23.
//
#include <chrono>
#include <cstdio>

#include <spdlog/spdlog.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/cpp/mandelbrot.hpp"
#include "src/cpp/utilities_shaders.hpp"

static void glfw_error_callback(int error, const char *description) {
    spdlog::error("GLFW Error {}: {}", error, description);
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

int main(int, char **) {
    spdlog::set_level(spdlog::level::debug);
    // Initialise GLFW
    glfwSetErrorCallback(glfw_error_callback);

    if (glfwInit() != 1) {
        spdlog::error("Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Apple compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, // Bottom-right
            -1.0f, 1.0f, 0.0f, 1.0f, // Top-left
            1.0f, 1.0f, 1.0f, 1.0f, // Top-right
    };
    unsigned int indices[] = {0, 1, 2, 1, 2, 3};

    float real_delta = 0.05f;
    float imag_delta = 0.05f;
    float scale_delta = 0.05f;

    int width = 1980;
    int height = 1080;

    float real_min = -3.0f;
    float real_max = 1.0f;
    float imag_min = -1.5f;
    float imag_max = 1.5f;

    int n_iterations = 35;
    float threshold = 6.0f;

    // Open a window and create its OpenGL context
    GLFWwindow *window; // (In the accompanying source code, this variable is
    // global for simplicity)
    window = glfwCreateWindow(width, height, "Mandelbrot render", nullptr, nullptr);

    if (window == nullptr) {
        spdlog::error(
                "Failed to open GLFW window. "
                "If you have an Intel GPU, they are not 3.3 compatible."
        );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW

    // enable GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        spdlog::error("Failed to initialize GLEW");
        return -1;
    }
    glfwGetFramebufferSize(window, &width, &height);
    spdlog::info("Got frame buffer size: width={}, height={}", width, height);

    GLuint shaderProgram = utils_shaders::LoadShaders("vertex.vert", "fragment_mb.frag");

    if (shaderProgram == 0) {
        fprintf(stderr, "Error loading shader");
        return -1;
    }

    // ------------ get locations of dynamic uniform parameters
    int threshold_loc = glGetUniformLocation(shaderProgram, "threshold");
    int n_iterations_loc = glGetUniformLocation(shaderProgram, "n_iterations");

    // ------------ create 2D texture to store set of complex values
    std::vector<float> complex_set = mandelbrot::gen_complex_set_2_shader(
            width, height, real_min, real_max, imag_min, imag_max
    );
    mandelbrot::print_complex_set_bounds(complex_set, width, height);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // GL_RG stores 2 values per pixel - Red, Green
    // Red will store real part of complex number and Green - imaginary
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, complex_set.data()
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // -------------------

    // --------- VBO definition
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

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

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(shaderProgram);

        // set uniform (shared) params
        glUniform1f(threshold_loc, threshold);
        glUniform1i(n_iterations_loc, n_iterations);

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
    glDeleteProgram(shaderProgram);
    // Delete textures
    glDeleteTextures(1, &texture);
    // terminate GLFW and exiting
    glfwTerminate();
    return 0;
}