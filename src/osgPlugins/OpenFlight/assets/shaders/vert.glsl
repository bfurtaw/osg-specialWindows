// Written by Brian Furtaw (bfurtaw@csc.com)
//
// Vertex Shader to support model rendering in Virtual Ship vers 9
//
#version 420
#extension GL_ARB_separate_shader_objects : enable
#extension GL_NV_bindless_texture : require
#extension GL_NV_gpu_shader5 : require

layout (location = 0) in vec4 osg_Vertex;
layout (location = 1) in vec3 osg_Normal;
layout (location = 3) in vec2 osg_MultiTexCoord0; //texCoord0;
layout (location = 5) in vec4 sxty_textures;

uniform vec3 g_lightPos;
uniform mat4 g_ModelViewMatrix;
uniform mat4 g_ProjectionMatrix;
uniform mat3 g_NormalMatrix;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 norm;
layout (location = 2) out vec2 UVs;
layout (location = 3) out vec3 lightdir;
layout (location = 4) flat out ivec4 counts;
void main()
{
	position =  vec3(g_ModelViewMatrix * vec4(osg_Vertex.xyz, 1.0));
	gl_Position = g_ProjectionMatrix * g_ModelViewMatrix * vec4(osg_Vertex.xyz, 1.0);
	//mat4 cNormalMatrix = transpose(inverse(g_ModelViewMatrix));
	//norm = vec3(cNormalMatrix * vec4(osg_Normal, 0.0));
	norm =  normalize(g_NormalMatrix * osg_Normal);
	lightdir = normalize(g_lightPos - position);
	UVs = osg_MultiTexCoord0; //texCoord0;
	counts.x = int(sxty_textures.x);
	counts.y = int(sxty_textures.y);
	counts.z = int(sxty_textures.z);
	counts.w = int(sxty_textures.w);

}
