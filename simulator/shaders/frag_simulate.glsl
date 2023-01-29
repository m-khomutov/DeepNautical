#version 330

in vec3 TexColor;
layout (location = 0) out vec4 Color;

uniform Circle
{
    vec4 InnerColor;
    vec4 OuterColor;
    float InnerRadius;
    float OuterRadius;
} circle;

void main() {
//    vec2 delta(TexCoord.x - 0.5, TexCoord.y - 0.5);
//    float dist = sqrt(delta.x * delta.x + delta.y * delta.y);
    //float dist = 0.5;
    Color = mix( circle.InnerColor,
                 circle.OuterColor,
                 vec4(TexColor, 1.0f ) ); 
    //             smoothstep( circle.InnerRadius, circle.OuterRadius, dist ) );
}
