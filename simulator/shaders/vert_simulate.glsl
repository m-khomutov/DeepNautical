#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec2 texcoord;

out vec2 TexCoord;

uniform float offset;

void main() {
    TexCoord = texcoord;
    gl_Position = vec4(position.x + offset, position.y, position.z, 1.0);
}
