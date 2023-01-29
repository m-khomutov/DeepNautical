#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec3 texcolor;

out vec3 TexColor;

void main() {
    TexColor = texcolor;
    gl_Position = vec4(position, 1.0);
}
