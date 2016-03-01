#version 330

in vec3 in_Position;
in vec3 in_Normals;

out vec3 Color;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

void main()
{
	Color = vec3(1.0f,0.0f,0.0f);
	gl_Position = vec4(in_Position,1.0f);//modelviewMatrix * projectionMatrix * vec4(in_Position, 1.0);
}


