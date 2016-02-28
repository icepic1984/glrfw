#version 330
in vec3 VertexPosition;
in vec3 VertexColor;

out vec3 Color;

uniform mat4 model_view;

uniform mat4 projection;

void main()
{
    Color = VertexColor;
    gl_Position = model_view * projection * vec4(VertexPosition, 1.0);
}

