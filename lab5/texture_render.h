#ifndef CV_LESSONS_RENDERER_H
#define CV_LESSONS_RENDERER_H

#include <iostream>
#include "GL/glew.h"
#include "opencv2/core.hpp"
#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const char* bg_vertex_shader_path = "../lab5/shaders/vertex.glsl";
const char* bg_fragment_shader_path = "../lab5/shaders/fragment.glsl";

const char* cube_vertex_shader_path = "../lab5/shaders/cube_vertex.glsl";
const char* cube_fragment_shader_path = "../lab5/shaders/cube_fragment.glsl";

#define RED 1, 0, 0
#define GREEN 0, 1, 0
#define BLUE 0, 0, 1


GLfloat bg_vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
};

GLuint bg_indices[] = {
        0, 1, 2,
        2, 3, 0
};  // Opengl vertices and indices needed to render Webcam stream into background


GLfloat cube_vertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f, RED,     // 0
        0.5f, -0.5f, 0.5f, GREEN,   // 1
        0.5f,  0.5f, 0.5f, BLUE,    // 2
        -0.5f,  0.5f, 0.5f, RED,     // 3
        // Back face
        -0.5f, -0.5f, -0.5f, GREEN,   // 4
        0.5f, -0.5f, -0.5f, BLUE,    // 5
        0.5f,  0.5f, -0.5f, RED,     // 6
        -0.5f,  0.5f, -0.5f, GREEN    // 7
};

GLuint cube_indices[] = {
        // Front face
        0, 1, 2,
        0, 2, 3,
        // Top face
        3, 2, 6,
        3, 6, 7,
        // Back face
        7, 6, 5,
        7, 5, 4,
        // Bottom face
        4, 5, 1,
        4, 1, 0,
        // Left face
        4, 0, 3,
        4, 3, 7,
        // Right face
        1, 5, 6,
        1, 6, 2
};


class Renderer {
private:
    GLuint vao_id = 0;
    GLuint vbo_id = 0;
    GLuint ebo_id = 0;

    GLuint cube_vao_id = 0;
    GLuint cube_vbo_id = 0;
    GLuint cube_ebo_id = 0;

    GLuint bg_shader;
    GLuint cube_shader;
    GLuint texture = 0;

public:
    Renderer();
    void Render() const;
    void update_texture(const cv::Mat& frame) const;
};

Renderer::Renderer() {
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        exit(1);
    }
    std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bg_vertices), bg_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bg_indices), bg_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    bg_shader = CreateShader(bg_vertex_shader_path, bg_fragment_shader_path);
    cube_shader = CreateShader(cube_vertex_shader_path, cube_fragment_shader_path);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &cube_vao_id);
    glBindVertexArray(cube_vao_id);

    glGenBuffers(1, &cube_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &cube_ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::update_texture(const cv::Mat& frame) const {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::Render() const {
    glClearColor(0.1, 0.1, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(bg_shader);
    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


    // Set up perspective projection
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

    // Set up view matrix
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.5f));

    // Pass matrices to shader
    glUseProgram(cube_shader);
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view));

    // Render the cube
    glBindVertexArray(cube_vao_id);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

#endif //CV_LESSONS_RENDERER_H