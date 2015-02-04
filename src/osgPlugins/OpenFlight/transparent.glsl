
// Written by Brian Furtaw (bfurtaw@csc.com)
//
// Fragment Shader to support model rendering in Virtual Ship vers 9
// This version incorporates transparency.
//
//
// 1. Need reflection cubemap (1-face, align camera) for reflection and self shadow
#version 420

in vec3 position;
in vec3 norm;
in vec2 UVs;
in vec3 lightdir;

out vec4 fragColor;


vec3 gammaCorrection( in vec3 color)
{
	const float gammaValue = 2.2;
	return pow( color, vec3(1.0/gammaValue) );
}

void main()
{
	// Not USED with bumpmapping Phong lit ONLY    float NdotL = dot(norm, lightdir);
	float HdotL = pow( max( dot(norm,  lightdir), 0.0), 40.0);
	vec3 diffuse = vec3(0.607843, 0.423529, 0.0823529); // White base color
	vec3 ambient = vec3(0.32549, 0.223529, 0.0431373);  // Dark grey ambient contrib
	vec3 specular = vec3(0.87451, 0.607843, 0.117647) * HdotL;  // Specular has a tint color

	float lambert = max(dot(lightdir, norm), 0.0);

	float alpha = 1.0;

	vec3 pcolor = ambient  + diffuse * lambert + specular;
	fragColor = vec4(gammaCorrection(pcolor), alpha);
}
