#version 330

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D tex;

void main()
{
    // scale depth map 
    float z = texture(tex,TexCoords);
    float near = 0.1f;
    float far = 1000.0f;
    float c = (2.0 * near) / (far + near - z * (far - near));
    Color = vec4(c);
    
}

