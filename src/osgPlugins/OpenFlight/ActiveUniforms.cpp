#include <osg/Notify>
#include <osg/Texture>
#include "ActiveUniforms.h"
namespace flt {

GLuint ActiveUniforms::getHandle(osg::StateSet *emat, GLuint layer, GLuint contextID) const {
	osg::Texture *texture = dynamic_cast<osg::Texture*>(emat->getTextureAttribute(layer, osg::StateAttribute::TEXTURE));
	osg::Texture::TextureObject *texobj;

	if(texture && (texobj = texture->getTextureObject(contextID)) ) {
		OSG_WARN << "l(" << layer << ") hD[" << texobj->id()-1 << "]" ;
		return texobj->id()-1; 
	} else
		OSG_WARN << "l(" << layer << ") hD[0]" ;
		return 0;
}

void ActiveUniforms::apply(osg::State& state) const
{
		  const unsigned int contextID = state.getContextID();
		  const osg::GL2Extensions* extensions = osg::GL2Extensions::Get(contextID,true);

		  if( ! extensions->isGlslSupported() ) return;

		  OSG_WARN << " Frame # " << state.getFrameStamp()->getFrameNumber() << " EMatrl type " << _matrl_type << std::endl;

		  //Make sure the UBER shader is loaded.
		  glcode::GLCode(contextID);
		  //glGetIntegerv(GL_CURRENT_PROGRAM, &prog); fix if it is not.
		  //use glVertexAttrib4f to set the state properly.      EXTensions->glVertexAttrib4f(5, _id-1, specular, bumper, luminosity);
		  // make sure Attrib number 5 is enabled.
		  if(_base_texture) {
					 //OSG_WARN << "Base Texture ID " << _base_texture->getUniqueID() << std::endl;
					 osg::Texture::TextureObject *texobj = _base_texture->getTextureObject(contextID);
					 if(texobj) {
								OSG_WARN << "Base Texture Handle " << texobj->id() << std::endl;
#if 0
								if(_base_texture->getUniqueID() == 256)
										  extensions->glVertexAttrib4f(5, texobj->id()-1, 3-1, 4-1, 0);
								else
#endif
										  extensions->glVertexAttrib4f(5, texobj->id()-1, 0, 0, 0);
					 }
		  } else if(_extended_matrl) {
					 extensions->glVertexAttrib4f(5, getHandle(_extended_matrl, 1, contextID), 
										  getHandle(_extended_matrl, 2, contextID), 
										  getHandle(_extended_matrl, 3, contextID),
										  getHandle(_extended_matrl, 4, contextID) );
					OSG_WARN << " applied textures." << std::endl;
		  }
}

// Make a node visitor that finds all Statesets investigates all layer textures checks there usage grab there textureObject->getID()
// Then set the texture to OFF to save state changes.
// Render a bunch of VBO's
// Make sure the Swap buffer callback class Profiler makes it to Steve's VisFed.
// Figure out how to set the sun position and sun color.
// Make methods in to update the uniforms for sun position
// Make methods for fog color and distance (0..1) and height (0..1)
// Walk the scene graph everytime it changes. Need a watch node or somthing to detect new StateSets
// Have to test this with

} // end-of-namespace
