#ifndef CV_LESSONS_RENDERER_H
#define CV_LESSONS_RENDERER_H

#include <iostream>
#include "GL/glew.h"
#include "opencv2/core.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

#define PI 3.1415

const char* bg_vertex_shader_path = "../lab5/shaders/vertex.glsl";
const char* bg_fragment_shader_path = "../lab5/shaders/fragment.glsl";

const char* cube_vertex_shader_path = "../lab5/shaders/cube_vertex.glsl";
const char* cube_fragment_shader_path = "../lab5/shaders/cube_fragment.glsl";

#define RED 1.0, 0.0, 0.0
#define GREEN 0.0, 1.0, 0.0
#define BLUE 0.0, 0.0, 1.0
#define YELLOW 1.0f, 1.0f, 0.0f
#define PURPLE 1.0f, 0.0f, 1.0f
#define CYAN 0.0f, 1.0f, 1.0f

const float CUBE_SCALE = 0.3;

GLfloat quad_vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
};

GLuint quad_indices[] = {
        0, 1, 2,
        2, 3, 0
};


GLfloat cube_vertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f,   RED,  // Bottom-left
        0.5f, -0.5f, 0.5f,   RED,  // Bottom-right
        0.5f,  0.5f, 0.5f,   RED,  // Top-right
        -0.5f,  0.5f, 0.5f,   RED,  // Top-left

        // Back face
        -0.5f, -0.5f, -0.5f,  GREEN,  // Bottom-left
        0.5f, -0.5f, -0.5f,  GREEN,  // Bottom-right
        0.5f,  0.5f, -0.5f,  GREEN,  // Top-right
        -0.5f,  0.5f, -0.5f,  GREEN,  // Top-left

        // Top face
        -0.5f,  0.5f, -0.5f,  BLUE,  // Front-left
        0.5f,  0.5f, -0.5f,  BLUE,  // Front-right
        0.5f,  0.5f,  0.5f,  BLUE,  // Back-right
        -0.5f,  0.5f,  0.5f,  BLUE,  // Back-left

        // Bottom face
        -0.5f, -0.5f, -0.5f,  YELLOW,  // Front-left
        0.5f, -0.5f, -0.5f,  YELLOW,  // Front-right
        0.5f, -0.5f,  0.5f,  YELLOW,  // Back-right
        -0.5f, -0.5f,  0.5f,  YELLOW,  // Back-left

        // Right face
        0.5f, -0.5f, -0.5f,  PURPLE,  // Front-bottom
        0.5f,  0.5f, -0.5f,  PURPLE,  // Front-top
        0.5f,  0.5f,  0.5f,  PURPLE,  // Back-top
        0.5f, -0.5f,  0.5f,  PURPLE,  // Back-bottom

        // Left face
        -0.5f, -0.5f, -0.5f,  CYAN,  // Front-bottom
        -0.5f,  0.5f, -0.5f,  CYAN,  // Front-top
        -0.5f,  0.5f,  0.5f,  CYAN,  // Back-top
        -0.5f, -0.5f,  0.5f,  CYAN   // Back-bottom
};

GLuint cube_indices[] = {
        0, 1, 2,  0, 2, 3,   // Front face
        4, 5, 6,  4, 6, 7,   // Back face
        8, 9, 10, 8, 10, 11, // Top face
        12, 13, 14, 12, 14, 15, // Bottom face
        16, 17, 18, 16, 18, 19, // Right face
        20, 21, 22, 20, 22, 23  // Left face
};


void setup_opengl_projection(const cv::Mat& camera_matrix, int width, int height, glm::mat4 &projection_mat,
                                  double near_plane = 0.1, double far_plane = 100) {
    auto fx = camera_matrix.at<double>(0, 0);
    auto fy = camera_matrix.at<double>(1, 1);
    double cx = camera_matrix.at<double>(0, 2);
    double cy = camera_matrix.at<double>(1, 2);

    double aspect_ratio = static_cast<double>(width) / height;
    double fovy = 2.0 * atan(height / (2.0 * fy)) * 180.0 / PI;

    // Construct the projection matrix
    projection_mat = glm::perspective(glm::radians(fovy), aspect_ratio, near_plane, far_plane);
}



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

    glm::mat4 projection_mat = {};

public:
    Renderer(int width, int height, const cv::Mat &cam_mat);
    void render_img(const cv::Mat& frame) const;
    void render_cube(const cv::Vec<double, 3> &pos, const cv::Vec<double, 3> &rot);
};

Renderer::Renderer(int width, int height, const cv::Mat &cam_mat) {
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        exit(1);
    }
    std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    glClearColor(1, 1, 1, 1);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

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

    setup_opengl_projection(cam_mat, width, height, projection_mat);
}



void Renderer::render_img(const cv::Mat& frame) const {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

    glUseProgram(bg_shader);
    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::render_cube(const cv::Vec<double, 3> &pos, const cv::Vec<double, 3> &rot) {
    std::cout << "Render cube on " << pos << " "<< rot << std::endl;

    auto model = glm::mat4(1.0f);
//    model[1][1] *= -1;
//    model[2][2] *= -1;
    model = glm::translate(model, glm::vec3(pos[0], -pos[1], -pos[2]));


    model = glm::rotate(model, static_cast<float>(rot[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(rot[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(rot[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(CUBE_SCALE));

    glm::mat4 transform_mat = model * scale;
//    transform_mat = glm::inverse(transform_mat);

    glUseProgram(cube_shader);
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_mat));
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "view_matrix"), 1, GL_FALSE, glm::value_ptr(transform_mat));
    glBindVertexArray(cube_vao_id);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

#endif //CV_LESSONS_RENDERER_H