#version 330
in vec4 in_Position;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    gl_Position = projectionMatrix * modelviewMatrix * in_Position;
}

    
