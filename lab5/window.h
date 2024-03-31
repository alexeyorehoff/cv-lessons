#ifndef CV_LESSONS_WINDOW_H
#define CV_LESSONS_WINDOW_H


#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cmath>
#include <iostream>

#define PI 3.1415

void glfw_error_callback(int error, const char* description) {
    std::cerr << "Glfw Error" << error << " : " << description << std::endl;
}

static void glfw_resize_callback(GLFWwindow*, int new_width, int new_height) {
    std::cout << "window is resized to " << new_width << " by " << new_height << std::endl;
}

class Window {
private:
    GLFWwindow* window;
public:
    Window(GLint width, GLint height, const char* title = "window");

    bool is_running() { return !glfwWindowShouldClose(window); }

    void swap() { glfwSwapBuffers(window); }

    GLFWwindow* get_window() { return window; }

    ~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

Window::Window(GLint width, GLint height, const char *title) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "GLFW initializing failed" << std::endl;
        exit(2);
    }

    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    if (width == 0 && height == 0) {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        width = 640;
        height = 480;
    }

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "GLFW window creation failed" << std::endl;
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(2);
    }

    std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    glfwSwapInterval(1);
    glfwSetWindowSizeCallback(window, glfw_resize_callback);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


//    gluPerspective(fovy, aspect, 0.1, 100);

    glViewport(0, 0, width, height);
}

#endif //CV_LESSONS_WINDOW_H
