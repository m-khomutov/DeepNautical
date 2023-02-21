#version 330

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec2 tc;
} fs_in;

layout (location = 0) out vec4 Color;

void main() {
    vec3 objcolor = vec3(0.9);
    vec3 suncolor = vec3(1.0f, 1.0f, 0.75f);

    vec3 Ka = vec3(0.5);
    vec3 ambient = Ka;

    float diff = max(dot(fs_in.N, fs_in.L), 0.0);
    vec3 Kd = vec3(0.8, 0.8, 0.8);
    float Id = 0.5;
    vec3 diffuse = suncolor * diff * Kd * Id;

    // specular 
    vec3 Ks = vec3(1.0, 1.0, 1.0);
    float Is = 1.0;
    vec3 R = reflect(-fs_in.L, fs_in.N);
    float shininess = 64.0;
    float spec = pow(max(dot(R, fs_in.V), 0.0), shininess);
    vec3 specular = suncolor * spec * Ks * Is;
    Color = vec4((ambient + diffuse + specular) * objcolor, 1.0);
}
