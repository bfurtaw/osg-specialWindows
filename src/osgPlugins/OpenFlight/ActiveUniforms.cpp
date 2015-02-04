#include <osg/Notify>
#include "ActiveUniforms.h"
namespace flt {


void ActiveUniforms::apply(osg::State& state) const
{
		  const unsigned int contextID = state.getContextID();
		  const osg::GL2Extensions* extensions = osg::GL2Extensions::Get(contextID,true);

		  if( ! extensions->isGlslSupported() ) return;

		  OSG_WARN << " Frame # " << state.getFrameStamp()->getFrameNumber() << " EMatrl type " << _matrl_type << std::endl;

		  glcode::GLCode(contextID);
}

} // end-of-namespace
