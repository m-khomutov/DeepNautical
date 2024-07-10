#version 330

in vec2 TexCoord;
layout (location = 0) out vec4 Color;

uniform sampler2D Texture;
uniform float Waterline;

void main() {
    if(TexCoord.y > Waterline)
        discard;

    Color = texture( Texture, TexCoord );
    bvec3 toDiscard = greaterThan( vec3(Color.rgb), vec3(0.2, 0.2, 0.2) );
    if( all(toDiscard) )
        discard;
}
