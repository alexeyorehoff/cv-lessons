#include <iostream>
#include <fstream>
#include <sstream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const int width = 640;
const int height = 480;

const char* vertex_shader_path = "../gl-report/shaders/vertex.glsl";
const char* fragment_shader_path = "../gl-report/shaders/fragment.glsl";


GLfloat cube_vertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   // Bottom-left, RED
        0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   // Bottom-right, RED
        0.5f,  0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   // Top-right, RED
        -0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   // Top-left, RED

        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   // Bottom-left, GREEN
        0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   // Bottom-right, GREEN
        0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   // Top-right, GREEN
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   // Top-left, GREEN
};

GLuint cube_indices[] = {
        0, 1, 2,
        0, 2, 3,
        2, 1, 5,
        2, 5, 6,
        3, 2, 6,
        3, 6, 7,
        0, 3, 7,
        0, 7, 4,
        1, 0, 4,
        1, 4, 5,
        6, 5, 4,
        6, 4, 7,
};


unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    GLint compiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE ){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        auto log = (char*)malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, log);
        std::cerr << "Error while compiling a " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader, log = " << log << std::endl;
        delete log;
        glDeleteShader(id);
        exit(1);
    }
    return id;
}


unsigned int CreateShader(const GLchar* vs_path, const GLchar* fs_path){
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    std::ifstream fs_file;
    std::ifstream vs_file;

    vs_file.exceptions(std::ifstream::badbit);
    fs_file.exceptions(std::ifstream::badbit);
    try{
        vs_file.open(vs_path);
        fs_file.open(fs_path);
        std::stringstream vs_stream, fs_stream;
        vs_stream << vs_file.rdbuf();
        fs_stream << fs_file.rdbuf();
        vs_file.close();
        fs_file.close();
        vertexShaderSource = vs_stream.str();
        fragmentShaderSource = fs_stream.str();
    }
    catch (const std::ifstream::failure& e){
        std::cout << "Shader file read failed" << std::endl;
    }

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}


int main() {
    GLFWwindow* window;
    GLuint vao_id, vbo_id, ebo_id, shader;

    if (!glfwInit()) {
        std::cerr << "GLFW init failed" << std::endl;
        return 1;
    }

    window = glfwCreateWindow(width, height, "opengl report", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "GLFW window init failed" << std::endl;
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 3;
    }
    std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    glfwSwapInterval(1);
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 1);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Color attribute
    glEnableVertexAttribArray(1);


    shader = CreateShader(vertex_shader_path, fragment_shader_path);


    // Projection matrix (perspective projection)
    float aspectRatio = (float)width / (float)height;
    float fov = glm::radians(45.0f); // 45 degrees field of view
    float nearPlane = 0.01f;
    float farPlane = 10.0f;
    glm::mat4 projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);

    // View matrix (camera)
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f); // Place the camera at (0,0,3)
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Look at the origin
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Up vector
    glm::mat4 viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao_id);
        glUseProgram(shader);

        float angle = (float)glfwGetTime() * 50.0f;
        auto modelMatrix = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(1, 1, 0));

        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}