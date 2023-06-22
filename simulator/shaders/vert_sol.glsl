#version 330

layout (location=0) in vec3 Position;
layout (location=1) in vec2 Texcoord;

out vec2 TexCoord;
out vec4 Distance;

uniform vec3 Offset;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 CameraPosition;

void main() {
    TexCoord = Texcoord;
    Distance = View * Model * vec4(Position, 1.0);
    gl_Position = Projection * View * Model * vec4(Position + Offset, 1.0);
}
