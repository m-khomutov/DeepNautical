#version 330

in vec2 TexCoord;
layout (location = 0) out vec4 Color;

uniform Circle
{
    vec4 InnerColor;
    vec4 OuterColor;
    float InnerRadius;
    float OuterRadius;
} circle;

void main() {
    vec2 center = TexCoord - 0.5f;
    float dist = sqrt(center.x * center.x + center.y * center.y);
    Color = mix(circle.InnerColor,
                circle.OuterColor,
                smoothstep(circle.InnerRadius, circle.OuterRadius, dist));
}
