#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec2 texcoord;

out vec2 TexCoord;

uniform vec3 Offset;
uniform mat4 Model;

void main() {
    TexCoord = texcoord;
    gl_Position = Model * vec4(position + Offset, 1.0);
}
