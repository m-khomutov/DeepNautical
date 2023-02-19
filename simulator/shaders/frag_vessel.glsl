#version 330

in VS_OUT {
    in vec3 N;
    in vec3 L;
    in vec3 V;
    in vec2 tc;
} fs_in;

layout (location = 0) out vec4 Color;

void main() {
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);
    
    // ambient 
    vec3 camb = vec3(0.3);

    // diffuse 
    float diff = max(dot(N, L), 0.0);
    float val = clamp(round(sin( 20 * fs_in.tc.x * 3.14156)), 0, 1);
    vec3 col1 = vec3(1.0, 1.0, 1.0);
    vec3 col2 = vec3(1.0, 0.0, 0.0);
    vec3 Ka = mix(col1, col2, val);
    float Ia = 0.8;
    vec3 cdiff = diff * Ka * Ia;

    // specular 
    vec3 Ks = vec3(1.0, 1.0, 1.0);
    float Is = 1.0;	
    vec3 R = reflect(-L, N);
    float a = 64.0;
    float spec = pow(max(dot(R, V), 0.0), a);
    vec3 cspec = spec * Ks * Is;
    Color = vec4(camb + cdiff + cspec, 1.0);
}
