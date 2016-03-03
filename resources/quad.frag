#version 330

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D tex;

void main()
{
    Color = texture(tex,TexCoords);
    //Color = vec4(1.0f,1.0f,0.0f,1.0f);
}
