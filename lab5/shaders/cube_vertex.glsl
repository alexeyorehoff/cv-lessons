#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 color;

uniform mat4 projection_matrix;
uniform mat4 rotation_matrix;
uniform vec4 translation_vector;

void main() {
    gl_Position = projection_matrix * (rotation_matrix * vec4(vertex_position, 1.0) + translation_vector);
    color = vertex_color;
}