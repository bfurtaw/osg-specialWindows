#include <osg/StateAttribute>
#include <osg/Object>
#include <osg/State>
#include <osg/Texture>
#include "GLCode.h"

namespace flt {

class ActiveUniforms : public osg::StateAttribute
{
	public:
		ActiveUniforms() : _matrl_type(0), _base_texture(NULL), _extended_matrl(NULL) {}
		ActiveUniforms(unsigned int matrl_type) : _matrl_type(matrl_type), _base_texture(NULL), _extended_matrl(NULL) {}
		ActiveUniforms(unsigned int matrl_type, osg::Texture *a_tex) : _matrl_type(matrl_type) , _base_texture(a_tex) {}
		ActiveUniforms(unsigned int matrl_type, osg::StateSet *a_extend) : _matrl_type(matrl_type) , _base_texture(NULL), _extended_matrl(a_extend) {}
		ActiveUniforms(const ActiveUniforms& act, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
		{}

		META_StateAttribute(osg,ActiveUniforms,(static_cast<osg::StateAttribute::Type>(osg::StateAttribute::FRAME_BUFFER_OBJECT+1)))

      /** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs.*/
      int compare(const StateAttribute& sa) const
		{
			COMPARE_StateAttribute_Types(ActiveUniforms,sa);
			if(_matrl_type == rhs._matrl_type)
				return 0;
			else if(_matrl_type < rhs._matrl_type)
				return -1;
			else
				return 1;
		}

		void apply(osg::State& state) const;

	protected:
	   GLuint getHandle(osg::StateSet *, GLuint, GLuint) const ;
		~ActiveUniforms() {}
		unsigned int _matrl_type;
		osg::Texture *_base_texture;
		osg::StateSet *_extended_matrl;

};

} // namespace flt
