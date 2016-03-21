#version 330

in vec3 epos;
in vec3 lightdir;
in vec3 normal;
in vec4 shadow_coord;

out vec4 out_color;

uniform sampler2DShadow ShadowMap;
uniform vec4 colorAmbient = vec4(0.0f,0.0f,0.9f,0.4f);
uniform vec4 colorDiffuse = vec4(0.0f,0.0f,0.9f,0.3f);
uniform vec4 colorSpecular = vec4(1.0f,1.0f,1.0f,0.0f);
uniform float shininess = 100.0f;
uniform int red_shadow;


vec3 CalcDirLight(vec3 light , vec3 normal)
{
    vec3 lightDir = normalize(-light);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 ambient = vec3(0.1, 0.1, 0.1);
    
    vec3 diffuse  = vec3(0.3,0.3,0.3)  * diff ;
    return (diffuse);
}  



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

    //float shadow = textureProj(ShadowMap, shadow_coord);

    float sum = 0;
    sum += textureProjOffset(ShadowMap, shadow_coord, ivec2(-1,-1));
    sum += textureProjOffset(ShadowMap, shadow_coord, ivec2(-1, 1));
    sum += textureProjOffset(ShadowMap, shadow_coord, ivec2(1, 1));
    sum += textureProjOffset(ShadowMap, shadow_coord, ivec2(1, -1));
    float shadow = sum * 0.25;
    

    vec3 d1 = CalcDirLight(vec3(0,0,100),normal);
    vec3 d2 = CalcDirLight(vec3(0,100,0),normal);
    vec3 d3 = CalcDirLight(vec3(-100,0,0),normal);
    vec3 d4 = CalcDirLight(vec3(100,0,0),normal);

    if ( red_shadow == 0) {
        out_color = vec4(ambient * ka, 0.16f) +
                    vec4(specular * spec * ks, 0.16f) * shadow +
            vec4(shadow * diffuse * nDotL, 0.16f);// + vec4(d1, 0.16f) +
        //                  vec4(d2, 0.16f) + vec4(d3, 0.16f) + vec4(d4, 0.16f);
    } else {

            if (shadow < 1) {
                out_color = vec4(vec3(0.5f, 0.0f, 0.0f), 0.2f) +
                            vec4(specular * spec * ks, 1.0f) + vec4(d1, 1.0f) +
                            vec4(d2, 1.0f) + vec4(d3, 1.0f) + vec4(d4, 1.0f);
            } else {
                out_color = vec4(ambient * ka, 1.0f) +
                            vec4(specular * spec * ks, 1.0f) + vec4(d1, 1.0f) +
                            vec4(d2, 1.0f) + vec4(d3, 1.0f) + vec4(d4, 1.0f);
            }
    }
    

    // if (shadow < 1) {
    //     out_color = vec4(vec3(0.5f, 0.0f, 0.0f),1.0f) +
    //                 vec4(specular * spec * ks, 1.0f) + vec4(d, 1.0f) * 0.7;
    // } else {
    //     out_color = vec4(ambient * ka, 1.0f) + vec4(specular * spec * ks,1.0f) +
    //                 vec4(d, 1.0f);
    // }
    // out_color = vec4(ambient * ka + shadow * diffuse * nDotL * kd +
    //                  specular * spec * ks * shadow,
    //                  1) + vec4(d,1.0f);
}
