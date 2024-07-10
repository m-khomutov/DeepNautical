#version 330

in vec2 TexCoord;
in vec4 Distance;
layout (location = 0) out vec4 Color;

struct FogParams
{
    vec4 color;
    float density;
};

uniform CircleParams
{
    vec3 InnerColor;
    vec3 OuterColor;
    float InnerRadius;
    float OuterRadius;
} Circle;

uniform FogParams Fog;
uniform sampler2D Texture;

float FogFactor(in float density) {
    float fogCoord = abs(Distance.z / Distance.w);
    float factor = pow(density * fogCoord, 2);
    return clamp(exp(-factor), 0.0, 1.0);
}

void main() {
    vec2 center = TexCoord - 0.5f;
    float dist = sqrt(center.x * center.x + center.y * center.y);
    vec4 sol = mix(vec4(Circle.InnerColor, 1.0),
                   vec4(Circle.OuterColor, 1.0),
                   smoothstep(Circle.InnerRadius, Circle.OuterRadius, dist));
    Color = mix(Fog.color, texture(Texture, TexCoord), FogFactor(Fog.density));
}
