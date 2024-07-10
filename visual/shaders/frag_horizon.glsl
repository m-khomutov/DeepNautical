#version 330

in vec2 TexCoord;
layout (location = 0) out vec4 Color;

uniform sampler2D Texture;

void main() {
    Color = texture( Texture, TexCoord );
}
