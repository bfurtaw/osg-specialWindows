// Written by Brian Furtaw (bfurtaw@csc.com)
//
// Vertex Shader to support model rendering in Virtual Ship vers 9
//
#version 400


layout (location = 0) in vec4 osg_Vertex;
layout (location = 1) in vec3 osg_Normal;
layout (location = 3) in vec2 osg_MultiTexCoord0; //texCoord0;

uniform vec3 lightPos;
uniform mat4 osg_ModelViewMatrix;
uniform mat4 osg_ViewMatrix;
uniform mat4 osg_ProjectionMatrix;
uniform mat3 osg_NormalMatrix;

out vec3 position;
out vec3 norm;
out vec2 UVs;
out vec3 lightdir;

void main()
{
	position =  vec3(osg_ModelViewMatrix * vec4(osg_Vertex.xyz, 1.0));
	gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * vec4(osg_Vertex.xyz, 1.0);
	//mat4 NormalMatrix = transpose(inverse(osg_ModelViewMatrix));
	//norm = vec3(NormalMatrix * vec4(osg_Normal, 0.0));
	norm =  normalize(osg_NormalMatrix * osg_Normal);
	lightdir = normalize(lightPos - position);
	UVs = osg_MultiTexCoord0; //texCoord0;

}
