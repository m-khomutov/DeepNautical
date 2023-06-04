#version 330

struct FogParameters
{
    vec4 color;
    float density;
};

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec4 distance;
    in float amplitude;
    in float wake;
} fs_in;

layout (location = 0) out vec4 Color;

uniform vec3 LightColor;
uniform sampler2D Texture;
uniform sampler2D AirTexture;
uniform sampler2D FoamTexture;
uniform FogParameters FogParams;

float FogFactor(FogParameters params) {
    float fogCoord = abs(fs_in.distance.z / fs_in.distance.w);
    float factor = pow(params.density * fogCoord, 2);
    return clamp(exp(-factor), 0.0, 1.0);
}

vec3 FoamColor(sampler2D foam, vec2 surfaceUV ) {
    vec2 scaledUV = surfaceUV * 0.1;
    float r = texture(foam, scaledUV - vec2(0.2, cos(surfaceUV.x))).r;
    float b = texture(foam, scaledUV * 0.5 + vec2(sin(surfaceUV.y), 0.2)).b;
    float gray = clamp(pow((r + b) * 0.95, 2), 0, 1);

    return clamp(vec3(gray), 0.0, 1.0);
}

vec4 Ambient() {
    float Ia = 0.7;
    vec4 ambient = mix(texture( Texture, fs_in.N.xy ), texture( AirTexture, vec2(fs_in.N.x, 1 - fs_in.N.y) ), 0.2) * Ia;
    return ambient;
}

vec4 Diffuse() {
    float Id = 0.9;
    vec3 Kd = vec3(0.392, 0.706, 0.983);
    float diff = max(dot(fs_in.N, fs_in.L), 0.0) * Id;
    return vec4(diff * Kd * LightColor, 0.0);
}

vec4 Specular() {
    vec4 specular = Diffuse();
    if( any(greaterThan(specular, vec4(0.0, 0.0, 0.0, 0.0))) )
    {
        float Is = 0.5;
        vec3 Ks = vec3(1.0, 1.0, 1.0);
        vec3 R = reflect(-fs_in.L, fs_in.N);
        float spec = pow(max(dot(R, fs_in.V), 0.0), 128) * Is;
        specular += vec4(spec * Ks * LightColor, 0.0);
    }
    return Ambient() + specular;
}

void main() {
    float factor = FogFactor(FogParams);
    Color = mix(FogParams.color, Specular(), factor);

    bvec3 toDiscard = lessThan(Color.rgb, mix(FogParams.color, vec4(0.4), factor).rgb);
    if( all(toDiscard) )
    {
        Color = FogParams.color;
    }
    if( fs_in.wake > 0.0 || fs_in.amplitude > 0.02 || fs_in.amplitude < -0.02 )
    {
        Color.rgb = mix(Color.rgb, Color.rgb + FoamColor(FoamTexture, fs_in.N.xy), 0.4);
    }
}
