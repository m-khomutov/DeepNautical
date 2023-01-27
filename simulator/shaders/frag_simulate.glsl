#version 330

in vec4 vert_Color;
out vec4 color;
uniform vec4 vertexColor;

void main() {
    color = vert_Color + vertexColor;
}
