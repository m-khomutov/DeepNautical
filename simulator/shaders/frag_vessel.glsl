#version 330

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec2 tc;
} fs_in;

layout (location = 0) out vec4 Color;

void main() {
    // object color - strips
    float val = clamp(round(sin( 20 * fs_in.tc.x * 3.14156)), 0, 1);
    vec3 col1 = vec3(1.0, 1.0, 1.0);
    vec3 col2 = vec3(1.0, 0.0, 0.0);
    vec3 objcolor = mix(col1, col2, val);

    vec3 suncolor = vec3(1.0f, 1.0f, 0.75f);

    vec3 ambient = vec3(0.6);
    float diffuse = suncolor * max(dot(fs_in.N, fs_in.L), 0.0);

    // specular 
    vec3 R = reflect(-fs_in.L, fs_in.N);
    float shininess = 64.0;
    vec3 specular = suncolor * pow(max(dot(R, fs_in.V), 0.0), shininess);
    Color = vec4((ambient + diffuse + specular) * objcolor, 1.0);
}
