//
// Author Brian Furtaw (bfurtaw@csc.com)
// Date Tue Feb 24 17:02:00 EST 2015
//
// Module to allow custom shader application to operate within the OpenSceneGraph. 
//
#ifndef OSG_NUGEOMETRY
#define OSG_NUGEOMETRY 1

#include <osg/Geometry>
#include <osg/Matrix>
#include "GLCode.h"


namespace osg {

class OSG_EXPORT NuGeometry : public Geometry
{
	public:
		NuGeometry(): Geometry() {};
		NuGeometry( const NuGeometry & geometry, const CopyOp& copyop=CopyOp::SHALLOW_COPY): Geometry(geometry, copyop) {};
		NuGeometry( const Geometry & backwards_compat, const CopyOp& copyop=CopyOp::DEEP_COPY_ARRAYS| osg::CopyOp::DEEP_COPY_PRIMITIVES): Geometry(backwards_compat, copyop) {};

		virtual Object* cloneType() const { return new NuGeometry(); }
		virtual Object* clone(const CopyOp& copyop) const { return new NuGeometry(*this, copyop); }
		virtual bool isSameKindAs(const Object * obj) const { return dynamic_cast<const NuGeometry*>(obj)!=NULL; }
		virtual const char* libraryName() const { return "osg"; }
		virtual const char* className() const { return "NuGeometry"; }

		virtual NuGeometry* asGeometry() { return this; }
		virtual const NuGeometry* asGeometry() const { return this; }

		bool empty() const;

		/** Draw Geometry directly ignoring an OpenGL display list which could be attached.
          * This is the internal draw method which does the drawing itself,
          * and is the method to override when deriving from Geometry for user-drawn objects.
        */
        virtual void drawImplementation(RenderInfo& renderInfo) const 
		  {
					 OSG_WARN << "Drawing from the OpenFLight_extended Geometry " << renderInfo.getState()->getLastAppliedProgramObject() << std::endl;
					 osg::GL2Extensions* extensions = osg::GL2Extensions::Get(renderInfo.getState()->getContextID(), true);
					 extensions->glUseProgram(glcode::getProgramHandle()); // get handle from GLCode.cpp
#if 0
					 renderInfo.getState()->setUseModelViewAndProjectionUniforms(1);
					 renderInfo.getState()->loadModelViewMatrix();
#else
					 Matrix mv(renderInfo.getState()->getModelViewMatrix());
					 mv.setTrans(0.0, 0.0, 0.0);

					 Matrix matrix;
					 matrix.invert(mv);

					 Matrix3 normalMatrix(matrix(0,0), matrix(1,0), matrix(2,0),
										       matrix(0,1), matrix(1,1), matrix(2,1),
										       matrix(0,2), matrix(1,2), matrix(2,2));
					 glcode::assignMatrixUniforms(Matrixf( renderInfo.getState()->getModelViewMatrix() ).ptr(), 
										               Matrixf( renderInfo.getState()->getProjectionMatrix() ).ptr(),
										               normalMatrix.ptr());
#endif
					 // Use generic vertex attributes.
					 renderInfo.getState()->setUseVertexAttributeAliasing(1);
					 // Call the original drawImplementation().
					 Geometry::drawImplementation(renderInfo);
					 // Turn shader program OFF
					 extensions->glUseProgram(0);
		  }
	protected:
		  NuGeometry& operator = (const NuGeometry&) { return *this;}

		  virtual ~NuGeometry() {};

};

} // end-o-namespace

#endif // OSG_NUGEOMETRY
