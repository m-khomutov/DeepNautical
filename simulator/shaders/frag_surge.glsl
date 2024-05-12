#version 330

layout (location = 0) out vec4 Color;

in vec2 fTexCoord;
in vec3 fSparkle;

uniform sampler2D Texture;
uniform sampler2D AirTexture;
uniform float Time;
uniform vec2 Amplitude;
uniform vec2 Frequency;

float perturb( vec2 surge )
{
    float rad = (fTexCoord.r + fTexCoord.s + 1.0 + Time) * surge[1];
    float pi = radians( 180.0 );
    float pi_pi = 2.0 * pi;
    float half_pi = pi / 2.0;

    rad = rad * pi_pi;
    rad = fract(rad);
    rad = rad * pi_pi;

    if( rad > pi ) rad = rad - pi_pi;
    if( rad < -pi ) rad = rad + pi_pi;

    if( rad > half_pi ) rad = pi - rad;
    if( rad < -half_pi ) rad = -pi - rad;

    return sin(rad) * surge[0];
}

void main() {
    bvec3 drawSparkle = greaterThan(fSparkle, vec3(0.0) );
    if( all(drawSparkle) )
    {
        Color = vec4(fSparkle, 1.0);
    }
    else
    {
        vec2 off = vec2(perturb(Amplitude), perturb(Frequency));
        Color = mix(texture(Texture, off + fTexCoord), texture(AirTexture, off + fTexCoord), 0.7);
    }
}
