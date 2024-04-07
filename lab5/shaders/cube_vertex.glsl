#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 color;

uniform mat4 projection_matrix;
uniform mat3 rotation_matrix;
uniform vec4 translation_vector;

uniform vec3 rad_dist;
uniform vec3 tang_dist;

void main() {
    gl_Position = projection_matrix * (mat4(rotation_matrix) * vec4(vertex_position, 1.0) + translation_vector);

    float x = gl_Position.x;
    float y = gl_Position.y;

    float r2 = x * x + y * y;
    float r4 = r2 * r2;
    float r6 = r4 * r2;

    float rad_dist = 1.0 + rad_dist.x * r2 + rad_dist.y * r4 + rad_dist.z * r6;

    float dx = 2.0 * tang_dist[0] * x * y + tang_dist[1] * (r2 + 2.0 * x * x);
    float dy = 2.0 * tang_dist[1] * x * y + tang_dist[0] * (r2 + 2.0 * y * y);

    gl_Position.x = (gl_Position.x + dx) * rad_dist;
    gl_Position.y = (gl_Position.y + dy) * rad_dist;

    color = vertex_color;
}