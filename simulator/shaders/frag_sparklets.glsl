#version 330

in vec2 TexCoord;
in vec3 Sparkle;

layout (location = 0) out vec4 Color;

uniform sampler2D Texture;
uniform sampler2D AirTexture;
uniform float Time;
uniform vec2 Surge[2];  // amplitude, frequency

float perturb( vec2 surge )
{
    float rad = (TexCoord.r + TexCoord.s + 1.0 + Time) * surge[1];
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
    bvec3 drawSparkle = greaterThan(Sparkle, vec3(0.0) );
    if( all(drawSparkle) )
    {
        Color = vec4(Sparkle, 1.0);
    }
    else
    {
        vec2 off = vec2(perturb(Surge[0]), perturb(Surge[1]));
        Color = mix(texture2D( Texture, off + TexCoord ), texture2D(AirTexture, off + TexCoord), 0.7);
    }
}
