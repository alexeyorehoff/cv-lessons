#version 330 core

in vec2 texture_coords;
out vec4 fragment_color;

uniform sampler2D texture_sampler;

void main() {
    fragment_color = texture(texture_sampler, texture_coords);
}