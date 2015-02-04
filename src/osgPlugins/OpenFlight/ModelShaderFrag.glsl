// Written by Brian Furtaw (bfurtaw@csc.com)
//
// Fragment Shader to support model rendering in Virtual Ship vers 9
//
//
// 1. Need reflection cubemap (1-face, align camera) for reflection and self shadow
#version 140

uniform mat4 viewMatrix;
uniform mat4 modelmatrix;
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec3 lightPosition;

attribute vec3 position;
attribute vec3 N;
attribute vec2 texcoords;

void main() {

   vec3 color = texture2D(diffuse, texcoords);
	vec3 bumpCoord = texture2D(2DbumpColor, texcoords);
	vec3 specular = texture2D(specularMap, texcoords);
	vec3 ambient = vec3(0.2, 0.2, 0.2);	// passed as a uniform later.
	vec3 diffuse = vec3(1.0, 1.0, 1.0); // diffuse light color, passed as uniform later
	vec3 lightPosition = vec3(1.5, 1.2, 0.5); // Position the sun (SW)....

	const float lambert = max(dot(N, normalize(lightPosition-position), 0);

	const float NdotL = max(dot(N, lightPosition), 0);
	const float NdotH = max(dot(N, halfVector), 0);


	gl_FragColor.rgb = color * ambient + color * NdotL * diffuse /* * selfShadow */ + specular * specular /* * selfShadow */;

}
