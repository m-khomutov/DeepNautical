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

uniform sampler2D Texture;

void main() {
    Color = texture( Texture, TexCoord ) * circle.OuterColor;
}
