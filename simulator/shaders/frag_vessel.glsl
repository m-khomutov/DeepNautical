#version 330

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec2 tc;
} fs_in;

layout (location = 0) out vec4 Color;

uniform Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Ns;
    float Ni;  // optical density
    float d;   // dissolve (transparency)
    int illum;
}  mtl;

void main() {
    vec3 suncolor = vec3(1.0f, 1.0f, 0.75f);

    float diff = max(dot(fs_in.N, fs_in.L), 0.0);
    float Id = 0.5;
    vec3 diffuse = suncolor * diff * mtl.Kd * Id;

    if( mtl.illum == 0 )
    {
        Color = vec4(diffuse, 1.0);
    }
    else
    {
        float Ia = 0.4;
        vec3 ambient = mtl.Ka * Ia;
        if( mtl.illum == 1 )
        {
            Color = vec4((ambient + diffuse), 1.0);
        }
        else
        {
            // specular 
            float Is = 1.0;
            vec3 R = reflect(-fs_in.L, fs_in.N);
            float spec = pow(max(dot(R, fs_in.V), 0.0), mtl.Ns);
            vec3 specular = suncolor * spec * mtl.Ks * Is;
            Color = vec4((ambient + diffuse + specular), 1.0);
        }
    }
}
