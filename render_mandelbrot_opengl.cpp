//
// Created by maksym on 18/10/23.
//
//#include <GL/glew.h>
#include <cstdio>
#include <chrono>

#include <GLFW/glfw3.h>

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

    // Open a window and create its OpenGL context
    GLFWwindow *window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1280, 720, "Mandelbrot render", nullptr, nullptr);


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

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while (
            glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0) {

        get_fps();

        glClear(GL_COLOR_BUFFER_BIT);

        // Draw nothing

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}