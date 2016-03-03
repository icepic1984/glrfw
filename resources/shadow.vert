#version 330

in vec4 in_Position;
in vec3 in_Normals;

out vec3 normal;
out vec3 lightdir;
out vec3 epos;
out vec4 shadow_coord;

uniform sampler2DShadow ShadowMap;
uniform mat4 modelviewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform mat4 shadowMatrix;

uniform vec3 lightpos;// = vec3(0.0,0.0,5.0);


void main(void) {
	 
    
	//Transform normals to eyespace (use normal matrix
	//to avoid problems with non-uniform scaling)
	//Pass it to fragment shader
    normal = normalize(normalMatrix * in_Normals);

	//Transform vertex to eye-space
    vec4 position = modelviewMatrix * in_Position;
    
    vec4 ml = modelviewMatrix * vec4(lightpos,1.0f);

	//Pass position to fragment-shader
	//Interpolate thought the polygon
    epos = position.xyz;

    //Calculate vector from surface to light
	//and pass it to fragment-shader
    //lightdir = normalize(lightpos - position.xyz);
    lightdir = normalize(ml.xyz - position.xyz);

    shadow_coord = shadowMatrix * in_Position;

    //Transform vertex to clip-space
    gl_Position = projectionMatrix * position;
}

