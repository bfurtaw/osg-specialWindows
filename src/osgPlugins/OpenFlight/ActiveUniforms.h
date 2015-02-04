#include <osg/StateAttribute>
#include <osg/Object>
#include <osg/State>
#include "GLCode.h"

namespace flt {

class ActiveUniforms : public osg::StateAttribute
{
	public:
		ActiveUniforms() : _matrl_type(0) {}
		ActiveUniforms(unsigned int matrl_type) : _matrl_type(matrl_type) {}
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
		~ActiveUniforms() {}
		unsigned int _matrl_type;
};

} // namespace flt
