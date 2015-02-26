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

#ifndef _EXTENDED_MATERIAL_H_
#define _EXTENDED_MATERIAL_H_ 1

#include <osg/StateSet>
#include <osg/TexEnv>
#include <vector>
#include "Registry.h"
#include "Document.h"
#include "RecordInputStream.h"
#include "Record.h"

namespace flt {
class ExtendedMatrlHdr : public Record {
public:

	ExtendedMatrlHdr() : _index(-1) {}
	ExtendedMatrlHdr(const ExtendedMatrlHdr &A_copy) {
		_index = A_copy._index;
		std::vector<TextureUVs>::const_iterator iter = A_copy._textureLayers.begin();
		for( ; iter != A_copy._textureLayers.end(); ++iter) {
			_textureLayers.push_back(TextureUVs(*iter)); // Use Texture UVs copy constructor
		}
	}
	// Remove extended matrl created on the Heap in this way. No memory leaks.
	void destroy() const { delete(this); }

    META_Record(ExtendedMatrlHdr)

protected:

    virtual ~ExtendedMatrlHdr() {
	// OSG_WARN << "Extended Material Out-of-scope" << std::endl;
	 }

    virtual void readRecord(RecordInputStream& in, Document& document)
    {
    	uint32 matrlIndex = in.readUInt32();
    	uint32 flags= in.readUInt32();
    	std::string buffer(13, '\0');
    	in.read(&buffer[0], 12);
    	uint32 shadeModel = in.readUInt32();
    	//OSG_WARN << "Extended_matrl- index " << matrlIndex << " used- " << flags << " shade_model "
    	//		<< (shadeModel == 1 ? "vertex" : "pixel") << std::endl;
    	_index = matrlIndex; // save extended material index

    	document.pushExtendedMaterial(new ExtendedMatrlHdr(*this)); // Create the ExtendedMatrlRecord on the Heap
    	//OSG_WARN << "Pushed ExtendedMaterialDef " << _index << " Level " << document.level() << std::endl;
    }
public:

    enum Layers { UNIMPLEMENTED = -1,
	 			COLOR_TEXTURE =1,
				SPECULAR_TEXTURE,
				NORMAL_TEXTURE,
				LUMINOSITY_TEXTURE
	};

    struct TextureUVs {
    	int textureIndex;
    	int UVSetIndex;
    	int layer;
		TextureUVs () : textureIndex(-1), UVSetIndex(-1), layer(-1) {};
    	TextureUVs (const TextureUVs &A_copy) {
    		textureIndex = A_copy.textureIndex;
    		UVSetIndex = A_copy.UVSetIndex;
			layer = A_copy.layer;
    	}
    	TextureUVs (int A_textureIndex, int A_layer, int A_UVSetIndex=-1) { textureIndex = A_textureIndex; layer = A_layer; UVSetIndex= A_UVSetIndex; }
    };

    std::vector<TextureUVs> _textureLayers;
    int _index;

    virtual void applyMerge(Document &document) const {
    	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
    	if(stateset) {
    		unsigned int layer;
    		std::vector<TextureUVs>::const_iterator iter = _textureLayers.begin();
			OSG_WARN << "This EMatrl index " << _index;

    		for( layer = 1; iter != _textureLayers.end(); ++layer, ++iter) {
    			osg::ref_ptr<osg::StateSet> texturePoolStateset = document.getOrCreateTexturePool()->get(iter->textureIndex);
    			if (stateset.valid() && texturePoolStateset.valid()) {
    				// Apply texture from texture pool.
    				osg::Texture* texture = dynamic_cast<osg::Texture*>(texturePoolStateset->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
    				if (texture && (iter->layer >=0)) {
						// Associate OSG Texture object with extended material
					   //OSG_WARN << " added Texture l[" << iter->layer << "] (" << /*iter->textureIndex*/texture->getUniqueID() << ")";
    					stateset->setTextureAttributeAndModes(iter->layer,texture,osg::StateAttribute::ON);
    				}
    			}
    		}
			OSG_WARN << std::endl;
    	}

    	// Add to extendedMaterial pool.
    	ExtendedMaterialPool* emp = document.getOrCreateExtendedMaterialPool();
    	assert(emp);
    	(*emp)[_index] = stateset;
    	// END OF APPLY FUNCTION
    }

};


} // end namespace

#endif // _EXTENDED_MATERIAL_H_
