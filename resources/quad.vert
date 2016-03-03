#version 330

in vec3 in_Position;
out vec2 TexCoords;

void main() 
{
    TexCoords = vec2(in_Position) * vec2(0.5f, 0.5f) + vec2(0.5f, 0.5f);
    gl_Position = vec4(in_Position,1.0f);
}

    
