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

const float CUBE_SCALE = 0.2f;

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
        -CUBE_SCALE, -CUBE_SCALE, CUBE_SCALE,   RED,  // Bottom-left
        CUBE_SCALE, -CUBE_SCALE, CUBE_SCALE,   RED,  // Bottom-right
        CUBE_SCALE,  CUBE_SCALE, CUBE_SCALE,   RED,  // Top-right
        -CUBE_SCALE,  CUBE_SCALE, CUBE_SCALE,   RED,  // Top-left

        // Back face
        -CUBE_SCALE, -CUBE_SCALE, -CUBE_SCALE,  GREEN,  // Bottom-left
        CUBE_SCALE, -CUBE_SCALE, -CUBE_SCALE,  GREEN,  // Bottom-right
        CUBE_SCALE,  CUBE_SCALE, -CUBE_SCALE,  GREEN,  // Top-right
        -CUBE_SCALE,  CUBE_SCALE, -CUBE_SCALE,  GREEN,  // Top-left

        // Top face
        -CUBE_SCALE,  CUBE_SCALE, -CUBE_SCALE,  BLUE,  // Front-left
        CUBE_SCALE,  CUBE_SCALE, -CUBE_SCALE,  BLUE,  // Front-right
        CUBE_SCALE,  CUBE_SCALE,  CUBE_SCALE,  BLUE,  // Back-right
        -CUBE_SCALE,  CUBE_SCALE,  CUBE_SCALE,  BLUE,  // Back-left

        // Bottom face
        -CUBE_SCALE, -CUBE_SCALE, -CUBE_SCALE,  YELLOW,  // Front-left
        CUBE_SCALE, -CUBE_SCALE, -CUBE_SCALE,  YELLOW,  // Front-right
        CUBE_SCALE, -CUBE_SCALE,  CUBE_SCALE,  YELLOW,  // Back-right
        -CUBE_SCALE, -CUBE_SCALE,  CUBE_SCALE,  YELLOW,  // Back-left

        // Right face
        CUBE_SCALE, -CUBE_SCALE, -CUBE_SCALE,  PURPLE,  // Front-bottom
        CUBE_SCALE,  CUBE_SCALE, -CUBE_SCALE,  PURPLE,  // Front-top
        CUBE_SCALE,  CUBE_SCALE,  CUBE_SCALE,  PURPLE,  // Back-top
        CUBE_SCALE, -CUBE_SCALE,  CUBE_SCALE,  PURPLE,  // Back-bottom

        // Left face
        -CUBE_SCALE, -CUBE_SCALE, -CUBE_SCALE,  CYAN,  // Front-bottom
        -CUBE_SCALE,  CUBE_SCALE, -CUBE_SCALE,  CYAN,  // Front-top
        -CUBE_SCALE,  CUBE_SCALE,  CUBE_SCALE,  CYAN,  // Back-top
        -CUBE_SCALE, -CUBE_SCALE,  CUBE_SCALE,  CYAN   // Back-bottom
};

GLuint cube_indices[] = {
        0, 1, 2,  0, 2, 3,   // Front face
        4, 5, 6,  4, 6, 7,   // Back face
        8, 9, 10, 8, 10, 11, // Top face
        12, 13, 14, 12, 14, 15, // Bottom face
        16, 17, 18, 16, 18, 19, // Right face
        20, 21, 22, 20, 22, 23  // Left face
};


glm::mat4 setup_opengl_projection(const cv::Mat& camera_matrix, int width, int height,
                             double near_plane = 0.1, double far_plane = 999999) {
    auto fy = camera_matrix.at<double>(1, 1);
    double aspect_ratio = static_cast<double>(width) / height;
    double fovy = 2.0 * atan(height / (2.0 * fy)) * 180.0 / PI;
    return glm::perspective(glm::radians(fovy), aspect_ratio, near_plane, far_plane);
}

cv::Mat RTtoOpenGL(const cv::Vec3d& rotationVector, const cv::Vec3d& translationVector) {
    cv::Mat rotationMatrix;
    cv::Rodrigues(rotationVector * -1, rotationMatrix);

    cv::Mat transformationMatrix = cv::Mat::eye(4, 4, CV_32F);

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            transformationMatrix.at<float>(i, j) = (float) rotationMatrix.at<double>(i, j);
        }
    }

    transformationMatrix.at<float>(0, 3) = -(float)translationVector[0];
    transformationMatrix.at<float>(1, 3) = (float)translationVector[1];
    transformationMatrix.at<float>(2, 3) = (float)translationVector[2];

    cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_32F);
    cvToGl.at<float>(0, 0) = 1;
    cvToGl.at<float>(1, 1) = -1;
    cvToGl.at<float>(2, 2) = -1; 
    cvToGl.at<float>(3, 3) = 1; 
    transformationMatrix = cvToGl * transformationMatrix;
    cv::transpose(transformationMatrix , transformationMatrix);

    return transformationMatrix;
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

    glm::mat4 projection_mat;

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

    projection_mat = setup_opengl_projection(cam_mat, width, height);
}



void Renderer::render_img(const cv::Mat& frame) const {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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

void Renderer::render_cube(const cv::Vec3d &pos, const cv::Vec3d &rot) {
    std::cout << "Render cube on " << pos << " "<< rot << std::endl;

    auto view_matrix = RTtoOpenGL(rot, pos);

    glUseProgram(cube_shader);
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_mat));
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "view_matrix"), 1, GL_FALSE, view_matrix.ptr<float>(0));
    glBindVertexArray(cube_vao_id);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

#endif //CV_LESSONS_RENDERER_H