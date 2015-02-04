#include "GL/glew.h"
#include <osg/Notify>
#include <vector>
#include <stdio.h>
#ifdef __linux
#include <GL/glx.h>
#endif
#include <string.h>
#include <malloc.h>

#include "NV/NvAssetLoader.h"
#include "NV/NvGLSLProgram.h"
#include <windows.h>
//#include <shlwapi.h>

//#pragma comment(lib,"shlwapi32.lib")

namespace glcode {

typedef void (*funcPtr)(GLint);
funcPtr g_findActives = NULL;

static void findActivesNewWay( GLint prog) {
		  GLint numActiveAttribs = 0;
		  GLint numActiveUniforms = 0;


		  glGetProgramInterfaceiv(prog, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
		  glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveAttribs);

		  std::vector<GLchar> nameData(256);
		  std::vector<GLenum> properties;
		  properties.push_back(GL_NAME_LENGTH);
		  properties.push_back(GL_TYPE);
		  properties.push_back(GL_ARRAY_SIZE);
		  std::vector<GLint> values(properties.size());
		  for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
		  {
					 glGetProgramResourceiv(prog, GL_PROGRAM_INPUT, attrib, properties.size(),
										  &properties[0], values.size(), NULL, &values[0]);

					 nameData.resize(properties[0]); //The length of the name.
					 glGetProgramResourceName(prog, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
					 std::string name((char*)&nameData[0], nameData.size() - 1);
				//	 OSG_WARN << "Active Attribute Name Location " << 
				//	 	glGetAttribLocation(prog, reinterpret_cast<const char *>(name.c_str())) << "(" << name << ")" << std::endl;
		  }
		  for(int uniform = 0; uniform < numActiveUniforms; ++uniform)
		  {
					 glGetProgramResourceiv(prog, GL_UNIFORM, uniform, properties.size(),
									  &properties[0], values.size(), NULL, &values[0]);

					 nameData.resize(properties[0]); //The length of the name.
					 glGetProgramResourceName(prog, GL_UNIFORM, uniform, nameData.size(), NULL, &nameData[0]);
					 std::string name((char*)&nameData[0], nameData.size() - 1);
				//	 OSG_WARN << "Active Uniform Name Location " << 
				//	 	glGetUniformLocation(prog,reinterpret_cast<const char *>(name.c_str())) << "(" << name << ")" << std::endl;
		  }
}

static void findActivesOldWay( GLint prog) {
		  GLint numActiveAttribs = 0;
		  GLint numActiveUniforms = 0;
		  glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
		  glGetProgramiv(prog, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

		  std::vector<GLchar> nameData(256);
		  for(int unif = 0; unif < numActiveUniforms; ++unif)
		  {
					 GLint arraySize = 0;
					 GLenum type = 0;
					 GLsizei actualLength = 0;
					 glGetActiveUniform(prog, unif, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
					 std::string name((char*)&nameData[0], actualLength );
				//	 OSG_WARN << "Active Uniform location " << 
				//	   glGetUniformLocation(prog,reinterpret_cast<const char *>(name.c_str())) << "(" << name << ")" << std::endl;
		  }
		  GLint maxAttribNameLength = 0;
		  glGetProgramiv(prog, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
		  //std::vector<GLchar> nameData(maxAttribNameLength);
		  for(int attrib = 0; attrib < numActiveAttribs; ++attrib)
		  {
					 GLint arraySize = 0;
					 GLenum type = 0;
					 GLsizei actualLength = 0;
					 glGetActiveAttrib(prog, attrib, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
					 std::string name((char*)&nameData[0], actualLength );
					 //OSG_WARN << "Active Attrib location " << 
					 //  glGetAttribLocation(prog, reinterpret_cast<const char *>(name.c_str())) << "(" << name << ")" << std::endl;
		  }
}

struct Vector3 {
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

struct UBOContainer {
	public:
		Vector3 LightDir;
		Vector3 LightColor;
		Vector3 Kd;
		Vector3 Ka;
		Vector3 Ks;
		GLuint map_Ka;
		GLuint map_Kd;
		GLuint map_Ks;
		GLuint map_Kn;
		GLuint map_Kl;
		GLfloat shine;
		GLfloat Tr;
};

void createUBO (GLuint progHandle, GLint count) {
	//Get UBO information garuntees buffer size is correct.
	GLuint blockIndex = glGetUniformBlockIndex(progHandle, "Material");
	GLint blockSize;
	UBOContainer ubo;
	ubo.map_Ka = (4);  // Landing gear texture F16c
	ubo.map_Kd = (1);  // Diffuse
	ubo.map_Kn = (2);  // Bumpmap
	ubo.map_Ks = (3);  // Specular
	
	glGetActiveUniformBlockiv(progHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLubyte *blockBuffer = (GLubyte *) malloc(blockSize);
	if(blockSize != sizeof(UBOContainer) ) {
		OSG_WARN << "createUBO size mismatch " << blockSize << " vs " << sizeof(UBOContainer) << std::endl;
	}
	// Move the data to the GPU
	// blf  memcpy(blockBuffer, ubo, blockSize);
   
	GLuint UBOhandle;
	glGenBuffers(1, &UBOhandle);
	glBindBuffer(GL_UNIFORM_BUFFER, UBOhandle);
	// blf something is wrong here   glBufferStorage(GL_UNIFORM_BUFFER, blockBuffer, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT); // OpenGL 4.4+ way, pointer to data always valid.
	// Make UBO data visible to shaders
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, UBOhandle);
}

void drawTextureThumbnails(std::vector<GLint> texUnits) {
	GLint depth;
	glGetIntegerv(GL_DEPTH_FUNC, &depth); // write depth values on top....
	glDepthFunc(GL_ALWAYS);
	//std::for_each(texUnits.begin(), texUnits.end(), drawTile);
	glDepthFunc(depth); // restore state....

}
void fixOpenGlState() {
	// This function will turn -off all the fixed function state activated by OSG, none of it is needed.
		  //glDisable(GL_CULL_FACE);
		  glDepthFunc(GL_LEQUAL);
		  // Not all the VertexAttribArrays are enabled...
		  glEnableVertexAttribArray(0);
		  glEnableVertexAttribArray(1);
		 // glEnableVertexAttribArray(2);
		  glEnableVertexAttribArray(3);
		 // glEnableVertexAttribArray(5);
}

void printGLSLVersions() {
		  // Print out all the GLSL shader language versions supported
		  GLint numGLSLVers;
		  glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &numGLSLVers);

		  char GLSLbuffer[1024];
		  GLint count;
		  count = sprintf(GLSLbuffer, "GLSL supported vers (%d) ", numGLSLVers);
		  for(unsigned int k = 0; k < numGLSLVers ; ++k) {

					 count += sprintf(GLSLbuffer + count, "%s, ", glGetStringi(GL_SHADING_LANGUAGE_VERSION, k));
		  }
		  OSG_WARN << GLSLbuffer << std::endl;
}

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	void* userParam){

	OSG_WARN << "---------------------opengl-callback-start------------" << std::endl;
	OSG_WARN << "message: " << message << std::endl;
	OSG_WARN << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		OSG_WARN << "ERROR";
		abort();
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		OSG_WARN << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		OSG_WARN << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		OSG_WARN << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		OSG_WARN << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		OSG_WARN << "OTHER";
		break;
	}
	OSG_WARN << std::endl;

	OSG_WARN << "id: " << id << " ";    OSG_WARN << "severity: ";
	switch (severity){
	case GL_DEBUG_SEVERITY_LOW:
		OSG_WARN << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		OSG_WARN << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		OSG_WARN << "HIGH";
		break;
	}
	OSG_WARN << std::endl;
	OSG_WARN << "---------------------opengl-callback-end--------------" << std::endl;
}

void GLCode(GLint context_id) {
static GLuint64 *m_textureHandles = new GLuint64[256];
static GLuint m_numTextures = 0;
static  NvGLSLProgram*                m_shader = NULL;
        GLint prog;
		  GLint major, minor;
		  GLuint dlist[] = {2,3,4,9,10,11,12,13,14,15,16,17,18,19,20,21};

		  glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
		  OSG_WARN << "Number of Resident Textures: " << m_numTextures << std::endl;
		  if (!g_findActives) { // DO ONCE
			  glewExperimental = GL_TRUE;
			  glewInit();

#if 0 // _DEBUG
			  if (glDebugMessageCallback){
				  OSG_WARN << "_DEBUG CODE: Register OpenGL debug callback " << std::endl;
				  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				  glDebugMessageCallback((GLDEBUGPROC)(openglCallbackFunction), nullptr);
				  GLuint unusedIds = 0;
				  glDebugMessageControl(GL_DONT_CARE,
					  GL_DONT_CARE,
					  GL_DONT_CARE,
					  0,
					  &unusedIds,
					  true);
			  }
			  else
				  OSG_WARN << "_DEBUG CODE: glDebugMessageCallback not available" << std::endl;
#endif
			  if (glewIsExtensionSupported("ARB_program_interface_query"))
				  g_findActives = (funcPtr)findActivesNewWay;
			  else {
				  g_findActives = (funcPtr)findActivesOldWay;
				  //	OSG_WARN << "ARB_program_interface_query NOT SUPPORTED" << std::endl;
			  }
			  glGetIntegerv(GL_MAJOR_VERSION, &major);
			  glGetIntegerv(GL_MINOR_VERSION, &minor);
			  OSG_WARN << "OpenGL-" << major << "." << minor << " Visual" << std::endl;
			  GLint red, green, blue, alpha, depth, stencil;
			  glGetIntegerv(GL_RED_BITS, &red);
			  glGetIntegerv(GL_GREEN_BITS, &green);
			  glGetIntegerv(GL_BLUE_BITS, &blue);
			  glGetIntegerv(GL_ALPHA_BITS, &alpha);
			  glGetIntegerv(GL_DEPTH_BITS, &depth);
			  glGetIntegerv(GL_STENCIL_BITS, &stencil);
			  OSG_WARN << "Visual traits (RGBA) " << red << "," << green << "," << blue << ","
				  << alpha << " (Depth,Stencil) " << depth << "," << stencil << std::endl;

			  printGLSLVersions();

			  // take out to restore drawing til this works fully. createUBO(prog,1);
			  const int BUFSIZE = 1024;
			  char buffer[BUFSIZE];
			  short retval;
			  retval = GetCurrentDirectory(BUFSIZE, buffer);
			  //retval = GetModuleFileName(NULL, buffer, BUFSIZE);
			  
			  //if(retval) PathRemoveFileSpec(buffer);
			  //abort();
			  NvAssetLoaderAddSearchPath(buffer);

			  // Create our pixel and vertex shader

			  if (glewIsExtensionSupported("GL_NV_bindless_texture")) {
				  m_shader = NvGLSLProgram::createFromFiles("shaders/vert.glsl", "shaders/frag.glsl");
				  m_textureHandles[0] = 0L; // Empty texture VOID(0)
				  m_numTextures++;
				  for (GLuint hh = 1; hh <= 240; ++hh) // W AR N I N G: hh <= 240 will not work for array maximum 256, OUT_OF_BOUNDS
						if (glIsTexture(hh)) {
							glBindTexture(GL_TEXTURE_2D, hh);
							m_textureHandles[hh] = glGetTextureHandleARB(hh);
							glMakeTextureHandleResidentARB(m_textureHandles[hh]);
							 m_numTextures++;
							//OSG_WARN << "Count 'em " << m_numTextures << std::endl;
						}
					//OSG_WARN << "Done " << m_numTextures << std::endl;
			  }
			  else { // This fallback path uses glBindTexture and diffuse/specular/bump/luminance map uniforms
				  m_shader = NvGLSLProgram::createFromFiles("shaders/simple_vert.glsl", "shaders/simple_frag.glsl");
			  }
		  } // END OF DO ONCE
		  fixOpenGlState();

		  //OSG_WARN <<  context_id << " CURRENT = " << prog << std::endl;
		  if(prog != m_shader->getProgram()) {
		  		m_shader->enable();
		  		prog = m_shader->getProgram();

				if (glewIsExtensionSupported("GL_NV_bindless_texture")) {
					GLuint samplersLocation(m_shader->getUniformLocation("samplers"));
					glUniform1ui64vNV(samplersLocation, m_numTextures, m_textureHandles);
					GLuint uniformLocation(m_shader->getUniformLocation("lightPos"));
					glUniform3f(uniformLocation, 0.0, 0.0, 1.0);
					OSG_WARN << "Locations (l/s)" << uniformLocation << " " << samplersLocation <<  std::endl;
					
				}
				else {
					GLuint uniformLocation(m_shader->getUniformLocation("lightPos"));
					glUniform3f(uniformLocation, 0.0, 0.0, 1.0);
					
#if 0
					GLint diffuseLocation(m_shader->getUniformLocation("diffuseMap"));
					if (diffuseLocation > -1) glUniform1i(diffuseLocation, 0); // base Index TEXUNIT
					GLint bumpLocation = m_shader->getUniformLocation("bumpMap");
					if (bumpLocation > -1) glUniform1i(bumpLocation, 6); // 6 Index TEXUNIT
					GLint specularLocation = m_shader->getUniformLocation("specularMap");
					if(specularLocation > -1) glUniform1i(specularLocation, 0/*7*/); // 7 Index TEXUNIT

					
					OSG_WARN << "Locations " << diffuseLocation << " " << bumpLocation << " " << specularLocation << std::endl;
#endif
				}
				//glDisableVertexAttribArray(5);
				//glVertexAttrib4f(5, 2,2,3,0);
		}
#if 0
		  GLint diffuseLocation(m_shader->getUniformLocation("diffuseMap"));
		  if (diffuseLocation > -1) glUniform1i(diffuseLocation, 0); // base Index TEXUNIT
		  GLint bumpLocation = m_shader->getUniformLocation("bumpMap");
		  if (bumpLocation > -1) glUniform1i(bumpLocation, 6); // 6 Index TEXUNIT
		  GLint specularLocation = m_shader->getUniformLocation("specularMap");
		  if (specularLocation > -1) glUniform1i(specularLocation, 0/*7*/); // 7 Index TEXUNIT
		  OSG_WARN << "Shader Prog " << prog << " Locations (d/b/s)" << diffuseLocation << " " << bumpLocation << " " << specularLocation << std::endl;
#endif

#if 0
					 for(unsigned int list=0; list < sizeof(dlist)/sizeof(GLuint); ++list)
					 			glCallList(dlist[list]);
#endif
#if 0
		  //Query all the active uniforms identify textures and texture units.
		  GLint textureUnits = 16;
		  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
		  std::vector<GLint> tunits;
		  int i, saveT;
		  for(i = 0; i < textureUnits; ++i) {
					 // If active texture unit has valid texture, draw it, draw matching uniform.
					 glActiveTexture(GL_TEXTURE0 + i);
					 glGetIntegerv(GL_TEXTURE_BINDING_2D, &saveT);
					 tunits.push_back(saveT);
					 //if(saveT > 0)
					 //	OSG_WARN << "Program " << prog << " TexUnit " << i << " TexBind " << saveT << std::endl;
		  }
#endif
		  g_findActives(prog);
#if 0
		  OSG_WARN << "Are textures {1,2,3} Resident?? " <<  (glIsTextureHandleResidentARB(glGetTextureHandleARB(1)) ? 1 : 0) 
		  << " , " << (glIsTextureHandleResidentARB(glGetTextureHandleARB(2)) ? 1 : 0) 
		  << " , " << (glIsTextureHandleResidentARB(glGetTextureHandleARB(3)) ? 1 : 0) << " Yes or No" <<std::endl;
#endif

		  //OSG_WARN << std::hex  << glXGetCurrentContext() << std::dec << " is GL_Context ID." << std::endl;
}


} // end-namespace glcode
