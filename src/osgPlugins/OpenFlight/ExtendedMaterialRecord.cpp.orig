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

#include <vector>
#include <assert.h> // LINUX
#include <osg/Notify>
#include <osg/StateSet>
#include <osg/Texture>
#include <osg/TexEnv>
#include <osg/Group>
#include <osg/StateSet>
#include "Registry.h"
#include "Document.h"
#include "RecordInputStream.h"
#include "Record.h"
#include "ExtendedMaterial.h"

namespace flt {


REGISTER_FLTRECORD(ExtendedMatrlHdr, EXTENDED_MATRL_OP)

class EMAmbientMap : public ExtendedMatrlHdr {
public:

	EMAmbientMap() {}

    META_Record(EMAmbientMap)

protected:

    virtual ~EMAmbientMap() {}

    virtual void readRecord(RecordInputStream& in, Document& document)
    {

    	float32 red = in.readFloat32();
    	float32 green = in.readFloat32();
    	float32 blue = in.readFloat32();

    	int32 textureidx = in.readInt32();
    	int32 uvidx = in.readInt32();

    	OSG_WARN << "Ambient Map base_color " << red << " " << green << " " << blue << std::endl;
    	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
    	for(int i =0; i < 4; ++i) {
    		OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
    		if(textureidx >=0 ) {
    			dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
    		}
    		// Read the next texture map info
    		textureidx = in.readInt32();
    		uvidx = in.readInt32();
    	}



    }
};
    REGISTER_FLTRECORD(EMAmbientMap, EM_AMBIENT_MAP_OP)

    class EMDiffuseMap : public ExtendedMatrlHdr {
    public:

    	EMDiffuseMap() {}

        META_Record(EMDiffuseMap)

    protected:

        virtual ~EMDiffuseMap() {}

        virtual void readRecord(RecordInputStream& in, Document& document)
        {

        	float32 red = in.readFloat32();
        	float32 green = in.readFloat32();
        	float32 blue = in.readFloat32();

        	int32 textureidx = in.readInt32();
        	int32 uvidx = in.readInt32();

        	OSG_WARN << "Diffuse Map base_color " << red << " " << green << " " << blue << std::endl;
        	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
        	for(int i =0; i < 4; ++i) {
        		OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
        		if(textureidx >=0 ) {
        			dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
        		}

        		textureidx = in.readInt32();
        		uvidx = in.readInt32();
        	}


        }
    };
        REGISTER_FLTRECORD(EMDiffuseMap, EM_DIFFUSE_MAP_OP)

    class EMSpecularMap : public ExtendedMatrlHdr {



     public:

     	EMSpecularMap() {}

         META_Record(EMSpecularMap)


     protected:

         virtual ~EMSpecularMap() {}

         virtual void readRecord(RecordInputStream& in, Document& document)
         {
        	 osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;

        	 float32 shinyness = in.readFloat32();
        	 float32 red = in.readFloat32();
        	 float32 green = in.readFloat32();
        	 float32 blue = in.readFloat32();

        	 int32 textureidx = in.readInt32();
        	 int32 uvidx = in.readInt32();

        	 OSG_WARN << "Specular Map base_color " << red << " " << green << " " << blue << std::endl;
        	 OSG_WARN << "Shininess " << shinyness << std::endl;
        	 OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
        	 // Add up to four specular maps, merge with parent stateset
        	 for(int i =0; i < 4; ++i) {
        		 OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
        		 if(textureidx >=0 ) {
        			 dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
        		 }

        		 textureidx = in.readInt32();
        		 uvidx = in.readInt32();
        	 }



         }
//         virtual void dispose(Document& document)
//         {
//        	 OSG_WARN << "Extended Material Dispose() called. " << std::endl;
//         }
     };
         REGISTER_FLTRECORD(EMSpecularMap, EM_SPECULAR_MAP_OP)

     class EMEmissiveMap : public ExtendedMatrlHdr {
        public:

        	EMEmissiveMap() {}

            META_Record(EMEmissiveMap)

        protected:

            virtual ~EMEmissiveMap() {}

            virtual void readRecord(RecordInputStream& in, Document& document)
            {

            	float32 red = in.readFloat32();
            	float32 green = in.readFloat32();
            	float32 blue = in.readFloat32();

            	int32 textureidx = in.readInt32();
            	int32 uvidx = in.readInt32();

            	OSG_WARN << "Emissive Map base_color " << red << " " << green << " " << blue << std::endl;
            	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
            	for(int i =0; i < 4; ++i) {
            		OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
            		if(textureidx >=0 ) {
            			dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
            		}
            		// Read the next texture map info
            		textureidx = in.readInt32();
            		uvidx = in.readInt32();
            	}


            }
        };
            REGISTER_FLTRECORD(EMEmissiveMap, EM_EMISSIVE_MAP_OP)

         class EMAlphaMap : public ExtendedMatrlHdr {
         public:

         	EMAlphaMap() {}

             META_Record(EMAlphaMap)

         protected:

             virtual ~EMAlphaMap() {}

             virtual void readRecord(RecordInputStream& in, Document& document)
             {

             	float32 alpha = in.readFloat32();

             	int32 textureidx = in.readInt32();
             	int32 uvidx = in.readInt32();

             	OSG_WARN << "Alpha Map base_transparency " << alpha << std::endl;
             	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
            	for(int i =0; i < 4; ++i) {
            		OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
            		if(textureidx >=0 ) {
            			dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
            		}
            		// Read the next texture map info
            		textureidx = in.readInt32();
            		uvidx = in.readInt32();
            	}

             	int32 quality = in.readInt32();
             	OSG_WARN << "Sample Quality " << (quality == 1 ? "Alpha Blend" : "Multisample Alpha") << std::endl;


             }
         };
             REGISTER_FLTRECORD(EMAlphaMap, EM_ALPHA_MAP_OP)

            class EMLightMap : public ExtendedMatrlHdr {
            public:

            	EMLightMap() {}

                META_Record(EMLightMap)

            protected:

                virtual ~EMLightMap() {}

                virtual void readRecord(RecordInputStream& in, Document& document)
                {

                	float32 intensity = in.readFloat32();

                	int32 textureidx = in.readInt32();
                	int32 uvidx = in.readInt32();

                	OSG_WARN << "Light Map base_intensity " << intensity << std::endl;
                	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;

                	OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
                	if(textureidx >=0 ) {
                		dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
                	}



                }
            };
                REGISTER_FLTRECORD(EMLightMap, EM_LIGHT_MAP_OP)

             class EMNormalMap : public ExtendedMatrlHdr {
             public:

             	EMNormalMap() {}

                 META_Record(EMNormalMap)

             protected:

                 virtual ~EMNormalMap() {}

                 virtual void readRecord(RecordInputStream& in, Document& document)
                 {

                 	int32 textureidx = in.readInt32();
                 	int32 uvidx = in.readInt32();

                 	OSG_WARN << "Normal Map  " << std::endl;
                 	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
                	OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
                	if(textureidx >=0 ) {
                		dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
                	}


                 }
             };
                 REGISTER_FLTRECORD(EMNormalMap, EM_NORMAL_MAP_OP)

                class EMBumpMap : public ExtendedMatrlHdr {
                public:

                	EMBumpMap() {}

                    META_Record(EMBumpMap)

                protected:

                    virtual ~EMBumpMap() {}

                    virtual void readRecord(RecordInputStream& in, Document& document)
                    {

                    	int32 textureidx = in.readInt32();
                    	int32 uvidx = in.readInt32();

                    	OSG_WARN << "Bump Map  " << std::endl;
                    	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
                    	OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
                    	if(textureidx >=0 ) {
                    		dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
                    	} // W A R N I N G Handle remain UVSets Need all three.
                    	uvidx = in.readInt32();
                       	OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
                       	uvidx = in.readInt32();
                       	OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;


                    }
                };
                    REGISTER_FLTRECORD(EMBumpMap, EM_BUMP_MAP_OP)

                 class EMShadowMap : public ExtendedMatrlHdr {
                 public:

                 	EMShadowMap() {}

                     META_Record(EMShadowMap)

                 protected:

                     virtual ~EMShadowMap() {}

                     virtual void readRecord(RecordInputStream& in, Document& document)
                     {
                    	float32 mintensity = in.readFloat32();
                     	int32 textureidx = in.readInt32();
                     	int32 uvidx = in.readInt32();

                     	OSG_WARN << "Shadow Map  max intensity " << mintensity << std::endl;
                     	OSG_WARN << "  TEXTURE_ID  UV_INDEX" << std::endl;
                    	OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
                    	if(textureidx >=0 ) {
                    		dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
                    	}// W A R N I N G Handle remain UVSets Need all three.
                     	uvidx = in.readInt32();
                        OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;
                        uvidx = in.readInt32();
                        OSG_WARN << "  " << textureidx << "   " << uvidx << std::endl;


                     }
                 };
                     REGISTER_FLTRECORD(EMShadowMap, EM_SHADOW_MAP_OP)

                    class EMReflectionMap : public ExtendedMatrlHdr {
                    public:

                    	EMReflectionMap() {}

                        META_Record(EMReflectionMap)

                    protected:

                        virtual ~EMReflectionMap() {}

                        virtual void readRecord(RecordInputStream& in, Document& document)
                        {
                        	float32 red = in.readFloat32();
                        	float32 green = in.readFloat32();
                        	float32 blue = in.readFloat32();
                        	int32 textureidx = in.readInt32();
                        	int32 uvidx = in.readInt32();
                        	int32 baseidx = in.readInt32();
                        	/*int32 reserved =*/ in.readInt32();

                        	OSG_WARN << "Reflection Map  tint color " << red << " " << green << " " << blue << std::endl;
                        	OSG_WARN << "  MASK_ID  UV_INDEX    BASE_ID" << std::endl;
                        	OSG_WARN << "  " << textureidx << "   " << uvidx << "    " << baseidx << std::endl;
                        	if(textureidx >=0 ) {
                        		dynamic_cast<ExtendedMatrlHdr *>(document.getCurrentExtendedMaterial())->_textureLayers.push_back(TextureUVs(textureidx, uvidx));
                        	}
                        	// W A R N I N G Does not handle baseidx yet fix code.


                        }
                    };
                        REGISTER_FLTRECORD(EMReflectionMap, EM_REFLECTION_MAP_OP)

}

