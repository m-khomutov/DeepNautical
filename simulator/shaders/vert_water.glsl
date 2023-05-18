#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec3 normals;

out VS_OUT {
    out vec3 N;
    out vec3 L;
    out vec3 V;
    out float distance;
} vs_out;


uniform vec3 Offset;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

void main() {
    // vertex in world coords
    vec3 worldcoords = (View * Model * vec4(position, 1.0)).xyz;
    // normal in world coords
    vs_out.N = normalize((inverse(transpose(Model)) * vec4(normals, 1.0)).xyz);
    // diffuse 
    vs_out.L = normalize(LightPosition - worldcoords);
    // specular 
    vs_out.V = normalize(-worldcoords);
    // distance from camera
    vs_out.distance = length(worldcoords - CameraPosition);

    gl_Position = Projection * View * Model * vec4(position + Offset, 1.0);
}
