#version 330

layout (location=0) in vec3 position;
layout (location=1) in vec2 texcoord;
layout (location=2) in vec3 normal;

out VS_OUT {
    out vec3 N;
    out vec3 L;
    out vec3 V;
    out vec2 tc;
} vs_out;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main() {
    // vertex in world coords
    vec3 World = (View * Model * vec4(position, 1.0)).xyz;
    // normal in world coords
    mat4 nm = transpose(inverse(View * Model));
    vs_out.N = (nm * vec4(normal, 1.0)).xyz;

    // diffuse 
    vec3 lightPos = vec3(0.0, 0.8, -10.0);
    vs_out.L = lightPos - World;

    // specular 
    vs_out.V = -World;

    //texels
    vs_out.tc = texcoord;

    gl_Position = Projection * View * Model * vec4(position, 1.0);
}
