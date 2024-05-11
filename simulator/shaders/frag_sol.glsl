#version 330

in vec2 fTexCoord;
in vec4 fDistance;

layout (location = 0) out vec4 fragColor;

struct FogParams
{
    vec4 color;
    float density;
};
uniform FogParams Fog;

uniform sampler2D Texture;

float FogFactor(in float density) {
    float fogCoord = abs(fDistance.z / fDistance.w);
    float factor = pow(density * fogCoord, 2);
    return clamp(exp(-factor), 0.0, 1.0);
}


void main(void)
{
    fragColor = mix(Fog.color, texture(Texture, fTexCoord), FogFactor(Fog.density));
}
