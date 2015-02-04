
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

out vec4 fragColor;

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

void main()
{
	// Not USED with bumpmapping Phong lit ONLY    float NdotL = dot(norm, lightdir);
	sampler2D diffuseMap = sampler2D(samplers[0]);
	float HdotL = pow( max( dot(norm,  lightdir), 0.0), 6.0);
	vec3 diffuse = colorTexture(texture2D(diffuseMap, UVs).rgb); // White base color
	sampler2D specularMap = sampler2D(samplers[1]);
	sampler2D bumpMap = sampler2D(samplers[2]);
	vec3 ambient = diffuse * vec3(.2, .2, .2);  // Dark grey ambient contrib
	vec3 specular = texture2D(specularMap, UVs).rgb * HdotL; //* vec3(0.607843, 0.423529, 0.0823529); // Specular has a tint color
	vec3 bump = texture2D(bumpMap, UVs).rgb * 2.0 - 1.0;

	// Compute align TBN space along cameraspace norm(al), use displacements of texcoords0 and cameraspace position.
	vec3 Q1 = dFdx(position);
	vec3 Q2 = dFdy(position);
	vec2 st1 = dFdx(UVs);
	vec2 st2 = dFdy(UVs);

	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
	//transpose of texture-to-eye space matrix
	mat3 TBN = mat3(T, B, norm);
	//bump = vec3(0.0, 1.0, 0.0); // Uncomment to see the normals on the model.

	vec3 bumpvec =  bump * TBN;
	float lambert = max(dot(lightdir, bumpvec), 0.0);

	float alpha = 1.0;

	vec3 pcolor = ambient + 0.01 + diffuse * lambert + specular;
	fragColor = vec4(gammaCorrection(pcolor), alpha);
}
