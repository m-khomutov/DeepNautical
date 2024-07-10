#version 330

in vec2 fTexCoord;
in vec4 fDistance;

layout (location = 0) out vec4 fragColor;

struct FogParams
{
    vec4 color;
    float density;
};
uniform FogParams Fog;
uniform float Time;

uniform sampler2D Texture;

float FogFactor(in float density) {
    float fogCoord = abs(fDistance.z / fDistance.w);
    float factor = pow(density * fogCoord, 2);
    return clamp(exp(-factor), 0.0, 1.0);
}

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


void main(void)
{
    vec2 off = vec2(perturb(vec2(0.004, 2.0)), perturb(vec2(0.003, 2.5)));
    fragColor = mix(Fog.color, texture(Texture, off + fTexCoord), FogFactor(Fog.density));
}
