#version 330

in vec3 in_Position;
in vec3 in_Normals;


void main()
{
    Color = VertexColor;
    gl_Position = vec4(VertexPosition, 1.0);
}


