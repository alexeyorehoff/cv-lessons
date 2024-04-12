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
        0.0, 0.0, 1.0,   1.0f, 0.0f, 0.0f, 
        1.0, 0.0, 1.0,    1.0f, 0.0f, 0.0f, 
        1.0,  1.0, 1.0,    1.0f, 0.0f, 0.0f, 
        0.0,  1.0, 1.0,   1.0f, 0.0f, 0.0f,

        // Back face
        0.0, 0.0, 0.0,  0.0f, 1.0f, 0.0f,
        1.0, 0.0, 0.0,   0.0f, 1.0f, 0.0f,
        1.0,  1.0, 0.0,   0.0f, 1.0f, 0.0f,
        0.0,  1.0, 0.0,  0.0f, 1.0f, 0.0f,
};

GLuint cube_indices[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        4, 5,
        5, 6,
        6, 7,
        7, 4,
        0, 4,
        1, 5,
        2, 6,
        3, 7
};


glm::mat4 setup_opengl_projection(const cv::Mat& camera_matrix, int width, int height,
                             double near_plane = 0.01, double far_plane = 100) {
    auto fx = camera_matrix.at<double>(0, 0);
    auto fy = camera_matrix.at<double>(1, 1);

    double fovy = 2.0 * atan(height / 2.0 / fy);
    double aspect_ratio = (width * fx) / (height * fy);

    return glm::perspective(fovy, aspect_ratio, near_plane, far_plane);
}


glm::mat3 rvec2rotmat(const cv::Vec3d& rotation, float scale) {
    cv::Mat cv_rot_mat;
    cv::Rodrigues(rotation, cv_rot_mat);

    glm::dmat3 rot_mat(0);
    memcpy(glm::value_ptr(rot_mat), cv_rot_mat.data, 9 * sizeof(double));
    rot_mat = glm::transpose(rot_mat);

    glm::dmat3 cv2gl = {scale, 0, 0,
                       0, scale, 0,
                       0, 0, -scale};

    rot_mat = rot_mat * cv2gl;

    return rot_mat;
}


class glRenderer {
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
    glm::vec3 rad_dist;
    glm::vec2 tang_dist;

    float cube_scale;

public:
    glRenderer(int width, int height, const cv::Mat &cam_mat, const cv::Mat &dist_coeffs, float cube_scale);
    void draw_bg(const cv::Mat& frame) const;
    void draw_cube(const cv::Vec3d &pos, const cv::Vec3d &rot);
};


glRenderer::glRenderer(int width, int height, const cv::Mat &cam_mat, const cv::Mat &dist_coeffs, float cube_scale)
     : cube_scale(cube_scale),
       bg_shader(CreateShader(bg_vertex_shader_path, bg_fragment_shader_path)),
       cube_shader(CreateShader(cube_vertex_shader_path, cube_fragment_shader_path)) {

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
    rad_dist = {dist_coeffs.at<double>(0, 0), dist_coeffs.at<double>(0, 1), dist_coeffs.at<double>(0, 4)};
    tang_dist = {dist_coeffs.at<double>(0, 2), dist_coeffs.at<double>(0, 3)};
}


void glRenderer::draw_bg(const cv::Mat& frame) const {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols,
                 frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

    glUseProgram(bg_shader);
    glBindVertexArray(vao_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void glRenderer::draw_cube(const cv::Vec3d &pos, const cv::Vec3d &rot) {
    std::cout << "Render cube on " << pos << " "<< rot << std::endl;

    auto view_matrix = rvec2rotmat(rot, cube_scale);
    glm::vec4 translation = {-pos[0], -pos[1], -pos[2], 1};

    glUseProgram(cube_shader);
    glUniformMatrix4fv(glGetUniformLocation(cube_shader, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_mat));
    glUniformMatrix3fv(glGetUniformLocation(cube_shader, "rotation_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniform4fv(glGetUniformLocation(cube_shader, "translation_vector"), 1, glm::value_ptr(translation));

    glUniform3fv(glGetUniformLocation(cube_shader, "rad_dist"), 1, glm::value_ptr(rad_dist));
    glUniform2fv(glGetUniformLocation(cube_shader, "tang_dist"), 1, glm::value_ptr(tang_dist));


    glBindVertexArray(cube_vao_id);
    glLineWidth(4.0f);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
}

#endif //CV_LESSONS_RENDERER_H