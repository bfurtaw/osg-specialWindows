
// Written by Brian Furtaw (bfurtaw@csc.com)
//
// Fragment Shader to support model rendering in Virtual Ship vers 9
//
//
// 1. Need reflection cubemap (1-face, align camera) for reflection and self shadow
#version 420
#extension GL_ARB_separate_shader_objects : enable
#extension GL_NV_bindless_texture : require
#extension GL_NV_gpu_shader5 : require // uint64_t

layout (location=0) in vec3 position;
layout (location=1) in vec3 norm;
layout (location=2) in vec2 UVs;
layout (location=3) in vec3 lightdir;
layout (location=4) flat in ivec4 counts;

//layout ( std140 ) uniform Material {
//	vec3 LightDir;
//	vec3 LightColor;
//	vec3 Kd;
//	vec3 Ka;
//	vec3 Ks;
//	uint64_t map_Ka;
//	uint64_t map_Kd;
//	uint64_t map_Ks;
//	uint64_t map_Kn;
//	uint64_t map_Kl;
//	float shine;
//	float Tr;
//	};

layout (location=0) out vec4 fragColor;

//layout ( binding = 1 ) uniform sampler2D bumpMap;
//layout ( binding = 2 ) uniform sampler2D specularMap;
//layout ( binding = 3 ) uniform sampler2D diffuseMap;

uniform uint64_t samplers[256];

vec3 gammaCorrection( in vec3 color)
{
	const float gammaValue = 2.2;
	return pow( color, vec3(1.0/gammaValue) );
}

// Correct color textures when they are read
vec3 colorTexture( in vec3 color)
{
	const float gammaValue= 2.2;
	return pow(color, vec3(gammaValue) );
}

// Fresnel Effect to enhance specular
float fresnel( in vec3 H, in vec3 view )
{
	float base = 1.0 + dot(view,H);
	float exponential = pow(base, 5.0);
	return max(0, min(1, .2 + 0.8 * exponential) );
}	

void main()
{
	// Not USED with bumpmapping Phong lit ONLY    float NdotL = dot(norm, lightdir);
	sampler2D diffuseMap = sampler2D(samplers[counts.x]);
	vec4 diffuse = texture2D(diffuseMap, UVs); // White base color
	sampler2D specularMap = sampler2D(samplers[counts.y]);
	sampler2D bumpMap = sampler2D(samplers[counts.z]);
	vec3 ambient = colorTexture(diffuse.rgb) * vec3(.15, .15, .15);  // Dark grey ambient contrib
	vec3 specular = gammaCorrection(texture2D(specularMap, UVs).rgb) * 0.5; //* vec3(0.607843, 0.423529, 0.0823529); // Specular has a tint color
	vec3 bump = texture2D(bumpMap, UVs).rgb * 2.0 - 1.0;
	bump.g *= 2.5;
	sampler2D lumMap = sampler2D(samplers[counts.w]);
	vec3 lumin = texture2D(lumMap, UVs).rgb;

	// Compute align TBN space along cameraspace norm(al), use displacements of texcoords0 and cameraspace position.
	vec3 Q1 = dFdx(position);
	vec3 Q2 = dFdy(position);
	vec2 st1 = dFdx(UVs);
	vec2 st2 = dFdy(UVs);

	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	//transpose of texture-to-eye space matrix
	mat3 TBN = mat3(T, B, norm);

	if(counts.z > 0)
	   bump = normalize(bump); // Required step because green * 2.5 un-normalizes bumpvec
	else  // Bumpdata is constant.
		bump = vec3(0.0, 1.0, 0.0); // Uncomment to see the normals on the model.

	vec3 bumpvec =  bump * TBN;
	float HdotL = pow( max( dot(bumpvec,  lightdir), 0.0), 6.0);
	float lambert = max(dot(lightdir, bumpvec), 0.0);

	float alpha = diffuse.a;

	vec3 pcolor = ambient + 0.01 +  colorTexture(diffuse.rgb) * lambert ;

        float R = fresnel(position, bumpvec); 

	if(counts.y > 0)
		pcolor += specular * HdotL * R ;
	else
		pcolor += vec3(0.607843, 0.423529, 0.0823529) * HdotL * R;

	if(position.x > 0.5 && counts.w > 0)
		pcolor *= lumin;

	fragColor = vec4(gammaCorrection(pcolor), alpha);
}
