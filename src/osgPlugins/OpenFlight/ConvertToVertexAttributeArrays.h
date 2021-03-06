// Copyright (C) 2014 Brian Furtaw (bfurtaw@csc.com)
//
// Support for extended material definitions for OpenFlight v 16.4
//

/*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#ifndef _CONVERT_VERTEX_ATTRIBUTE_H_
#define _CONVERT_VERTEX_ATTRIBUTE_H_ 1



#include <osgUtil/ShaderGen>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Program>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "NuGeometry.h"

namespace flt {

class ConvertToVertexAttributeArrays : public osg::NodeVisitor
{
    public:

        typedef std::pair<unsigned int, std::string> AttributeAlias;

        ConvertToVertexAttributeArrays():
            osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            _manualVertexAliasing = false;

            // mappings taken from http://www.opengl.org/registry/specs/NV/vertex_program.txt
            _vertexAlias = AttributeAlias(0, "vertex");
            _normalAlias = AttributeAlias(1, "normal");
            _colorAlias = AttributeAlias(2, "color");
            _secondaryColorAlias = AttributeAlias(6, "secondaryColor");
            _fogCoordAlias = AttributeAlias(7, "fogCoord");
            _texCoordAlias[0] = AttributeAlias(3, "texCoord0");
            _texCoordAlias[1] = AttributeAlias(4, "texCoord1");
            _texCoordAlias[2] = AttributeAlias(5, "texCoord2");
            _texCoordAlias[3] = AttributeAlias(11, "texCoord3");
            _texCoordAlias[4] = AttributeAlias(12, "texCoord4");
            _texCoordAlias[5] = AttributeAlias(13, "texCoord5");
            _texCoordAlias[6] = AttributeAlias(14, "texCoord6");
            _texCoordAlias[7] = AttributeAlias(15, "texCoord7");
        }

        void bindAttribute(osg::Program& program, const AttributeAlias& alias)
        {
                program.addBindAttribLocation(alias.second, alias.first);
        }


        void setVertexShaderAttribs(osg::Program& program)
        {

            // replace ftransform as it only works with built-ins
            //replaceBuiltinWithBuiltin(source, "ftransform()", "gl_ModelViewProjectionMatrix * gl_Vertex");

            bindAttribute(program, _normalAlias);
            bindAttribute(program, _vertexAlias);
            bindAttribute(program, _colorAlias);


            bindAttribute(program, _texCoordAlias[0]); // Model UVs
            bindAttribute(program, _texCoordAlias[1]); // Instancing matrix row 0
            bindAttribute(program, _texCoordAlias[2]); // Instancing matrix row 1
            bindAttribute(program, _texCoordAlias[3]); // Instancing matrix row 2
            bindAttribute(program, _texCoordAlias[4]); // Instancing matrix row 3
            bindAttribute(program, _texCoordAlias[5]); // Bindless texture addrs.
            bindAttribute(program, _texCoordAlias[6]);
            bindAttribute(program, _texCoordAlias[7]);

            // replace built in uniform
            //replaceBuiltInUniform(source, "gl_ModelViewMatrix", "osg_ModeViewMatrix", "uniform mat4 ");
            //replaceBuiltInUniform(source, "gl_ModelViewProjectionMatrix", "osg_ModelViewProjectionMatrix", "uniform mat4 ");
            //replaceBuiltInUniform(source, "gl_ProjectionMatrix", "osg_ProjectionMatrix", "uniform mat4 ");

            // blf don't forget to load shader Uniforms.

        }



        virtual void reset()
        {
            _visited.clear();
        }

        void apply(osg::Node& node)
        {
            if (_visited.count(&node)!=0) return;
            _visited.insert(&node);

            if (node.getStateSet()) apply(*(node.getStateSet()));
            traverse(node);
        }

        void apply(osg::Geode& geode)
        {
            if (_visited.count(&geode)!=0) return;
            _visited.insert(&geode);

            if (geode.getStateSet()) apply(*(geode.getStateSet()));

            for(unsigned int i=0; i<geode.getNumDrawables(); ++i)
            {
                if (geode.getDrawable(i)->getStateSet()) apply(*(geode.getDrawable(i)->getStateSet()));

                osg::NuGeometry* geom = geode.getDrawable(i)->asGeometry();
                if (geom) apply(*geom);
            }
        }

        void apply(osg::Program& program, osg::Shader& shader)
        {
             if (_visited.count(&shader)!=0) return;
            _visited.insert(&shader);

            osg::notify(osg::NOTICE)<<"Shader "<<shader.getTypename()<<" ----before-----------"<<std::endl;
            osg::notify(osg::NOTICE)<<shader.getShaderSource()<<std::endl;



            osg::notify(osg::NOTICE)<<"--after-----------"<<std::endl;
            osg::notify(osg::NOTICE)<<shader.getShaderSource()<<std::endl;
            osg::notify(osg::NOTICE)<<"---------------------"<<std::endl;
        }

        void apply(osg::StateSet& stateset)
        {
             if (_visited.count(&stateset)!=0) return;
            _visited.insert(&stateset);

            return;

            osg::notify(osg::NOTICE)<<"Found stateset "<<&stateset<<std::endl;
            osg::Program* program = dynamic_cast<osg::Program*>(stateset.getAttribute(osg::StateAttribute::PROGRAM));
            if (program)
            {
                osg::notify(osg::NOTICE)<<"   Found Program "<<program<<std::endl;
                for(unsigned int i=0; i<program->getNumShaders(); ++i)
                {
                    apply(*program, *(program->getShader(i)));
                }

            }
       }


        void apply(osg::NuGeometry& geom)
        {
            geom.setUseDisplayList(false);
            // doesn't help geom.setUseVertexBufferObjects(true);

            //if (!_manualVertexAliasing) return;

            osg::notify(osg::NOTICE)<<"Found geometry "<<&geom<<std::endl;
            if (geom.getVertexArray())
            {
                setVertexAttrib(geom, _vertexAlias, geom.getVertexArray(), false, osg::Array::BIND_PER_VERTEX);
                //geom.setVertexArray(0);
            }

            if (geom.getNormalArray())
            {
                setVertexAttrib(geom, _normalAlias, geom.getNormalArray(), true, osg::Array::BIND_PER_VERTEX);
                //geom.setNormalArray(0);
            }else
            {
                osg::notify(osg::NOTICE)<<"Found empty NormalArray" <<std::endl;
            }

            if (geom.getColorArray())
            {
                // blf: Ignore colorArray for now... setVertexAttrib(geom, _colorAlias, geom.getColorArray(), false);
            	OSG_WARN << "ConvertVertexAttributes: Color Array present Ignored!" << std::endl;
                geom.setColorArray(0);
            }

            if (geom.getSecondaryColorArray())
            {
            	// blf: Ignore secondaryColorArray for now...setVertexAttrib(geom, _secondaryColorAlias, geom.getSecondaryColorArray(), false);
                geom.setSecondaryColorArray(0);
            }

            if (geom.getFogCoordArray())
            {
                // should we normalize the FogCoord array? Don't think so...
            	// blf: Ignore FogCoordArray for now...setVertexAttrib(geom, _fogCoordAlias, geom.getFogCoordArray(), false);
                geom.setFogCoordArray(0);
            }

            unsigned int maxNumTexCoords = geom.getNumTexCoordArrays();
            if (maxNumTexCoords>8)
            {
                osg::notify(osg::NOTICE)<<"Warning: Ignoring "<<maxNumTexCoords-8<<" texture coordinate arrays, only 8 are currently supported in vertex attribute conversion code."<<std::endl;
                maxNumTexCoords = 8;
            }
            for(unsigned int i=0; i<maxNumTexCoords; ++i)
            {
                if (geom.getTexCoordArray(i))
                {
                    setVertexAttrib(geom, _texCoordAlias[i], geom.getTexCoordArray(i), false, osg::Array::BIND_PER_VERTEX);
                   // geom.setTexCoordArray(i,0);
                }
                else
                {
                    osg::notify(osg::NOTICE)<<"Found empty TexCoordArray("<<i<<")"<<std::endl;
                }
            }
        }

        void setVertexAttrib(osg::NuGeometry& geom, const AttributeAlias& alias, osg::Array* array, bool normalize, osg::Array::Binding binding = osg::Array::BIND_UNDEFINED)
        {
            unsigned int index = alias.first;
            const std::string& name = alias.second;
            array->setName(name);
            if (binding!=osg::Array::BIND_UNDEFINED) array->setBinding(binding);
            array->setNormalize(normalize);
            geom.setVertexAttribArray(index, array);

            osg::notify(osg::NOTICE)<<"   vertex attrib("<<name<<", index="<<index<<", normalize="<<normalize<<" binding="<<binding<<")"<<std::endl;
        }


        typedef std::set<osg::Object*> Visited;
        Visited         _visited;

        bool           _manualVertexAliasing;
        AttributeAlias _vertexAlias;
        AttributeAlias _normalAlias;
        AttributeAlias _colorAlias;
        AttributeAlias _secondaryColorAlias;
        AttributeAlias _fogCoordAlias;
        AttributeAlias _texCoordAlias[8];
};

}

#endif // _CONVERT_VERTEX_ATTRIBUTE_H_
