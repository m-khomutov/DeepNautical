#version 330

struct Fog
{
    vec4 color;
    float density;
};

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec2 tc;
    in vec4 distance;
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
uniform Fog fog;
uniform sampler2D Texture;

float FogFactor(in float density, in float distance) {
    float fogCoord = abs(fs_in.distance.z / fs_in.distance.w);
    float factor = pow(density * fogCoord, 2);
    return clamp(exp(-factor), 0.0, 1.0);
}

vec3 Ambient() {
    float Ia = 0.4;
    vec3 ambient = mtl.Ka * Ia;
    return ambient;
}

vec3 Diffuse() {
    float Id = 0.9;
    float diff = max(dot(fs_in.N, fs_in.L), 0.0) * Id;
    return diff * mtl.Kd * LightColor;
}

vec3 Specular() {
    vec3 specular = Diffuse();
    if( any(greaterThan(specular, vec3(0.0, 0.0, 0.0))) )
    {
        float Is = 1.0;
        vec3 R = reflect(-fs_in.L, fs_in.N);
        float spec = pow(max(dot(R, fs_in.V), 0.0), mtl.Ns) * Is;
        specular += spec * mtl.Ks * LightColor;
    }
    return specular;
}

void main() {
    switch( mtl.illum )
    {
    case 0:
        Color = vec4(Ambient(), mtl.d);
        break;
    case 1:
        Color = vec4(Ambient() + Diffuse(), mtl.d);
        break;
    default:
        Color = vec4(Ambient() + Specular(), mtl.d);
        break;
    }

    Color *= mix(fog.color, vec4(texture(Texture, fs_in.tc)), FogFactor(fog.density, fs_in.distance));
}
