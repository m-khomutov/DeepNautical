#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec2 texcoord;

out vec2 TexCoord;
out float distance;

uniform vec3 Offset;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 CameraPosition;

void main() {
    vec3 worldcoords = (View * Model * vec4(position, 1.0)).xyz;
    distance = length(worldcoords - CameraPosition);
    TexCoord = texcoord;

    gl_Position = Projection * View * Model * vec4(position + Offset, 1.0);
}
