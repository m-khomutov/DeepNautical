#version 330

layout (location=0) in vec3 Position;
layout (location=1) in vec2 Texcoord;

out vec2 TexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main() {
    TexCoord = Texcoord;
    gl_Position = Projection * View * Model * vec4(Position, 1.0);
}
