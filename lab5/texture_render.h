#ifndef CV_LESSONS_RENDERER_H
#define CV_LESSONS_RENDERER_H

#include <iostream>
#include "GL/glew.h"
#include "opencv2/core.hpp"
#include "shader.h"

const char* vertex_shader_path = "../lab5/shaders/vertex.glsl";
const char* fragment_shader_path = "../lab5/shaders/fragment.glsl";

GLfloat vertices[] = {
        // Positions    // Texture Coords
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
};

GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
};

class Renderer {
private:
    GLuint vao_id;
    GLuint vbo_id;
    GLuint ebo_id;
    GLuint shader_program;
    GLuint texture;

public:
    Renderer();
    void Render();
    void update_texture(const cv::Mat& frame);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader_program = CreateShader(vertex_shader_path, fragment_shader_path);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::update_texture(const cv::Mat& frame) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::Render() {
    glClearColor(0.1, 0.1, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shader_program);
    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#endif //CV_LESSONS_RENDERER_H