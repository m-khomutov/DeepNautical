#version 330

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec2 tc;
} fs_in;

layout (location = 0) out vec4 Color;

uniform Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Ns;
    float Ni;  // optical density
    float d;   // dissolve (transparency)
    int illum;
} mtl;

uniform vec3 LightColor;
uniform sampler2D Texture;

vec3 Diffuse() {
    float diff = max(dot(fs_in.N, fs_in.L), 0.0);
    float Id = 0.9;
    return LightColor * diff * mtl.Kd * Id;
}

vec3 Ambient() {
    float Ia = 0.4;
    vec3 ambient = mtl.Ka * Ia;
    return Diffuse() + ambient;
}

vec3 Specular() {
    float Is = 1.0;
    vec3 R = reflect(-fs_in.L, fs_in.N);
    float spec = pow(max(dot(R, fs_in.V), 0.0), mtl.Ns);
    vec3 specular = LightColor * spec * mtl.Ks * Is;
    return Ambient() + specular;
}

void main() {
    switch( mtl.illum )
    {
    case 0:
        Color = vec4(Diffuse(), 1.0);
        break;
    case 1:
        Color = vec4(Ambient(), 1.0);
        break;
    default:
        Color = vec4(Specular(), 1.0);
        break;
    }
    Color *= vec4(texture(Texture, fs_in.tc));
}
