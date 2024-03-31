#version 330 core

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_texture_coords;

out vec2 texture_coords;

void main() {
    gl_Position = vec4(-vertex_position, 0.0, 1.0);
    texture_coords = vertex_texture_coords;
}