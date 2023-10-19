//
// Created by maksym on 18/10/23.
//
#include <cstdio>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/cpp/mandelbrot.hpp"
#include "src/cpp/utilities_shaders.hpp"

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();
int fpsCount = 0;
int fps = 0;

void get_fps() {
    auto currentTime = std::chrono::steady_clock::now();

    const auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime).count();
    ++fpsCount;

    if (elapsedTime > 1000000000) {
        lastTime = currentTime;
        fps = fpsCount;
        fpsCount = 0;

        printf("%d fps\n", fps); // print out fps in every second (or you can use it elsewhere)
    }
}


int main(int, char **) {
    // Initialise GLFW
    glfwSetErrorCallback(glfw_error_callback);

    if (glfwInit() != 1) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,  // Bottom-left
            1.0f, -1.0f, 1.0f, 0.0f,  // Bottom-right
            -1.0f, 1.0f, 0.0f, 1.0f,  // Top-left
            1.0f, 1.0f, 1.0f, 1.0f,  // Top-right
    };
    unsigned int indices[] = {
            0, 1, 2,
            1, 2, 3
    };

    int width = 1280;
    int height = 720;

    double real_min = -3.0;
    double real_max = 1.0;
    double imag_min = -1.5;
    double imag_max = 1.5;

    int n_iterations = 35;
    double threshold = 6.0;

    // Open a window and create its OpenGL context
    GLFWwindow *window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(width, height, "Mandelbrot render", nullptr, nullptr);

    if (window == nullptr) {
        fprintf(
                stderr,
                "Failed to open GLFW window. "
                "If you have an Intel GPU, they are not 3.3 compatible."
                "\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW

    glewExperimental = GL_TRUE; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    GLuint shaderProgram = utils_shaders::LoadShaders("vertex.vert", "fragment.frag");
    if (shaderProgram == 0) return -1;

    GLuint Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glfwGetFramebufferSize(window, &width, &height);

    // VBO definition
    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // VAO definition
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(0);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while (
            glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0) {

        get_fps();


        glViewport(0, 0, width,
                   height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

        glClear(GL_COLOR_BUFFER_BIT);

        // --------------------- Draw section -------------------------
        std::vector<float> mandelbrot_grey = mandelbrot::gen_mandelbrot_greyscale(
                width,
                height,
                real_min,
                real_max,
                imag_min,
                imag_max,
                threshold,
                n_iterations
        );
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_R32F,
                width,
                height,
                0,
                GL_RED,
                GL_FLOAT,
                mandelbrot_grey.data()
        );
        glGenerateMipmap(GL_TEXTURE_2D);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glActiveTexture(GL_TEXTURE0); // you can use GL_TEXTURE1, ..., GL_TEXTURE31
        glBindTexture(GL_TEXTURE_2D, Texture);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, Texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // -------------------- END draw section ----------------------

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // terminate GLFW and exiting
    glfwTerminate();
    return 0;
}