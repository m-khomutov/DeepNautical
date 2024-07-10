#version 330

layout (location=0) in vec3 WavePosition;
layout (location=1) in vec2 Texcoord;
layout (location=2) in vec3 SparklePosition;
layout (location=3) in vec3 SparkleColor;

out vec2 TexCoord;
out vec3 Sparkle;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float DrawSparkles;

void main() {
    if( DrawSparkles > 0.0 )
    {
        Sparkle = SparkleColor;
        gl_Position = Projection * View * Model * vec4(SparklePosition, 1.0);
    }
    else 
    {
        Sparkle = vec3(0.0);
        TexCoord = vec2(Texcoord.x, 1.0 - Texcoord.y);
        gl_Position = Projection * View * Model * vec4(WavePosition, 1.0);
    }
}
