#version 430

layout (location=0) in vec4 position;
layout (location=1) in vec4 color;

out vec4 vert_Color;
uniform float offset;

void main() {
    gl_Position = vec4(position.x + offset, position.y, position.zw);
    vert_Color = color;
}
