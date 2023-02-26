#version 330

in vec2 TexCoord;
layout (location = 0) out vec4 Color;

uniform Circle
{
    vec3 InnerColor;
    vec3 OuterColor;
    float InnerRadius;
    float OuterRadius;
} circle;

void main() {
    vec2 center = TexCoord - 0.5f;
    float dist = sqrt(center.x * center.x + center.y * center.y);
    Color = mix(vec4(circle.InnerColor, 1.0),
                vec4(circle.OuterColor, 1.0),
                smoothstep(circle.InnerRadius, circle.OuterRadius, dist));
}
