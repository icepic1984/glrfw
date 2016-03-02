#version 330

in vec3 in_Position;
in vec3 in_Normals;


void main()
{
    gl_Position = vec4(in_Position, 1.0);
}


