
// Written by Brian Furtaw (bfurtaw@csc.com)
//
// Fragment Shader to support model rendering in Virtual Ship vers 9
//
//
// 1. Need reflection cubemap (1-face, align camera) for reflection and self shadow
#version 420

in vec3 position;
in vec3 norm;
in vec2 UVs;
in vec3 lightdir;

out vec4 fragColor;

layout ( binding = 1 ) uniform sampler2D diffuseMap;
layout ( binding = 3 ) uniform sampler2D bumpMap;
layout ( binding = 2 ) uniform sampler2D specularMap;

vec3 gammaCorrection( in vec3 color)
{
	const float gammaValue = 2.2;
	return pow( color, vec3(1.0/gammaValue) );
}

void main()
{
	// Not USED with bumpmapping Phong lit ONLY    float NdotL = dot(norm, lightdir);
	float HdotL = pow( max( dot(norm,  lightdir), 0.0), 6.0);
	vec3 diffuse = texture2D(diffuseMap, UVs).rgb; // White base color
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
