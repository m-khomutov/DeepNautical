#version 330

layout (location=0) in vec4 Position;
layout (location=1) in vec2 Texcoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec4 Offset;

out vec2 fTexCoord;
out vec4 fDistance;

void main(void)
{
    mat4 vm = View * Model;

    fTexCoord = Texcoord;
    fDistance = vm * Position;
    gl_Position = Projection * vm * (Position + Offset);
}
