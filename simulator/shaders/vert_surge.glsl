#version 330

layout (location=0) in vec4 WavePosition;
layout (location=1) in vec2 Texcoord;
layout (location=2) in vec3 SparklePosition;
layout (location=3) in vec3 SparkleColor;

out vec2 fTexCoord;
out vec3 fSparkle;
out vec4 fDistance;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float DrawSparkles;

void main() {
    if( DrawSparkles > 0.0 )
    {
        fSparkle = SparkleColor;
        gl_Position = Projection * View * Model * vec4(SparklePosition, 1.0);
    }
    else
    {
        fSparkle = vec3(0.0);
        fTexCoord = Texcoord;
        fDistance = View * Model * WavePosition;
        gl_Position = Projection * fDistance;
    }
}
