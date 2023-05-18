#version 330

struct FogParameters
{
    vec4 color;
    float density;
};

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in float distance;
} fs_in;

layout (location = 0) out vec4 Color;

uniform vec3 LightColor;
uniform sampler2D Texture;
uniform sampler2D AirTexture;
uniform FogParameters FogParams;

float FogFactor(FogParameters params) {
    float factor = pow(params.density * fs_in.distance, 2);
    return clamp(exp(-factor), 0.0, 1.0);
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
    float factor = FogFactor(FogParams);
    Color = mix(FogParams.color, Specular(), factor);
    vec4 filter = mix(FogParams.color, vec4(0.4), factor);
    bvec3 toDiscard = lessThan(Color.rgb, filter.rgb);
    if( all(toDiscard) )
        discard;
}
