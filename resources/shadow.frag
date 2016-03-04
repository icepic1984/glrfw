#version 330

in vec3 epos;
in vec3 lightdir;
in vec3 normal;
in vec4 shadow_coord;

out vec4 out_color;

uniform sampler2DShadow ShadowMap;
uniform vec4 colorAmbient = vec4(1.0f,1.0f,1.0f,0.1f);
uniform vec4 colorDiffuse = vec4(0.7f,0.7f,0.7f,0.7f);
uniform vec4 colorSpecular = vec4(1.0f,1.0f,1.0f,0.5f);
uniform float shininess = 100.0f;



void main(void) {
	
    //Reflexion coefficients
    float ka = colorAmbient.w; //0.1f
    float kd = colorDiffuse.w; //0.7
    float ks = colorSpecular.w ;//0.9;

	//Color 
    vec3 ambient = vec3(colorAmbient);
    vec3 diffuse = vec3(colorDiffuse);
    vec3 specular = vec3(colorSpecular);

	//Vector from surface to eye.
	//Eyeposition is by default at (0,0,0)
	//Just negate transformed position
    vec3 eye = normalize(-epos);

	//Reflection vector. (reflect requires the incident
	//lightvector, vector from lightsource to surface)
    vec3 r = normalize(reflect(-lightdir,normal));

	//Diffuse contribution
	//Independend from eye-point
    float nDotL = max(dot(lightdir,normal),0.0);

	//Specular contribution
	//amount of light that get reflected in direction
	//of the eye
    float nDotR = max(dot(eye,r),0.0);
    float spec;

	//Simulate shininiess
    if(nDotR == 0.0)
        spec = 0.0;
    else 
        spec = pow(nDotR,shininess);

    float shadow = textureProj(ShadowMap,shadow_coord);

     out_color = vec4(ambient * ka + shadow * diffuse * nDotL * kd +
                      specular * spec * ks * shadow, 1);
}
