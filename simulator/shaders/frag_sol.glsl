#version 330

in vec2 TexCoord;
in vec4 distance;
layout (location = 0) out vec4 Color;

struct Fog
{
    vec4 color;
    float density;
};

uniform Circle
{
    vec3 InnerColor;
    vec3 OuterColor;
    float InnerRadius;
    float OuterRadius;
} circle;

uniform Fog fog;
uniform sampler2D Texture;

float FogFactor(in float density) {
    float fogCoord = abs(distance.z / distance.w);
    float factor = pow(density * fogCoord, 2);
    return clamp(exp(-factor), 0.0, 1.0);
}

void main() {
    vec2 center = TexCoord - 0.5f;
    float dist = sqrt(center.x * center.x + center.y * center.y);
    vec4 sol = mix(vec4(circle.InnerColor, 1.0),
                   vec4(circle.OuterColor, 1.0),
                   smoothstep(circle.InnerRadius, circle.OuterRadius, dist));
    Color = mix(fog.color, texture(Texture, TexCoord), FogFactor(fog.density));
}
