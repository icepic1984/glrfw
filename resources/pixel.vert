#version 330

precision highp float;

//Vertex-attributes
layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normals;


out vec3 normal;
out vec3 lightdir;
out vec3 epos;

uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

uniform vec3 lightpos = vec3(0.0,0.0,5.0);


void main(void) {
	 
    
	//Transform normals to eyespace (use normal matrix
	//to avoid problems with non-uniform scaling)
	//Pass it to fragment shader
    normal = normalize(normalMatrix * in_Normals);

	//Transform vertex to eye-space
    vec4 position = modelviewMatrix * in_Position;

	//Pass position to fragment-shader
	//Interpolate thought the polygon
    epos = position.xyz;

    //Calculate vector from surface to light
	//and pass it to fragment-shader
    lightdir = normalize(lightpos - position.xyz);

	//Transform vertex to clip-space
    gl_Position = projectionMatrix * position;
}
