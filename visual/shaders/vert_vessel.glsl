#version 330

layout (location=0) in vec3 Position;
layout (location=1) in vec2 Texcoord;
layout (location=2) in vec3 Normal;

out VS_OUT {
    out vec3 N;
    out vec3 L;
    out vec3 V;
    out vec2 tc;
    out vec4 distance;
} vs_out;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 NormalMatrix;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

void main() {
    // distance from camera
    vs_out.distance = View * Model * vec4(Position, 1.0);
    // vertex in world coords
    vec3 worldcoords = vs_out.distance.xyz;
    // normal in world coords
    vs_out.N = normalize(NormalMatrix * Normal);
    // diffuse 
    vs_out.L = normalize(LightPosition - worldcoords);
    // specular 
    vs_out.V = normalize(-worldcoords);
    //texels
    vs_out.tc = Texcoord;

    gl_Position = Projection * View * Model * vec4(Position, 1.0);
}
