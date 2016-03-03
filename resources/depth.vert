#version 330

in vec3 in_Position;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;


void main() 
{
    // Position = (modelviewMatrix * vec4(in_Position,1.0f)).xyz;
    //Position = (projectionMatrix * modelviewMatrix * vec4(in_Position,1.0f)).xyz;
    gl_Position = projectionMatrix * modelviewMatrix * vec4(in_Position,1.0f);
}

