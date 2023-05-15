#version 330

struct FogParameters
{
    vec4 color;
    float start;
    float end;
};

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
} fs_in;

layout (location = 0) out vec4 Color;

uniform vec3 LightColor;
uniform sampler2D Texture;
uniform sampler2D AirTexture;
uniform FogParameters FogParams;

float FogFactor(FogParameters params, float coord) {
    return (params.end - coord) / (params.end - params.start);
}

vec4 Diffuse() {
    float diff = max(dot(fs_in.N, fs_in.L), 0.0);
    float Id = 0.9;
    vec3 Kd = vec3(0.392, 0.706, 0.983);
    return vec4(LightColor * diff * Kd * Id, 0.0f);
}
vec4 Ambient() {
    float Ia = 0.7;
    vec4 ambient = mix(texture( Texture, fs_in.N.xy ), texture( AirTexture, vec2(fs_in.N.x, 1 - fs_in.N.y) ), 0.2) * Ia;
    //vec4 ambient = texture( Texture, fs_in.N.xy ) * Ia;
    return Diffuse() + ambient;
}
vec4 Specular() {
    float Is = 1.0;
    vec3 R = reflect(-fs_in.L, fs_in.N);
    float spec = pow(max(dot(R, fs_in.V), 0.0), 128);
    vec3 specular = LightColor * spec * Is;
    return Ambient() + vec4(specular, 0.0);
}

void main() {
    float coord = abs(gl_FragCoord.z / gl_FragCoord.w);
    Color = mix(Specular(), FogParams.color, FogFactor(FogParams, coord));
}
