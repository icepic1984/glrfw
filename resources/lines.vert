#version 330

in vec3 in_Position;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;


void main()
{
    gl_Position = projectionMatrix * modelviewMatrix * vec4(in_Position,1.0f);
}
