// Copyright (C) 2014 Brian Furtaw (bfurtaw@csc.com)
//
// Support for extended material definitions for OpenFlight v 16.4
//

#include <osg/Notify>
#include "Registry.h"
#include "Document.h"
#include "RecordInputStream.h"
#include "Record.h"

namespace flt {
class ExtendedMatrlHdr : public Record {
public:

	ExtendedMatrlHdr() {}

    META_Record(ExtendedMatrlHdr)

protected:

    virtual ~ExtendedMatrlHdr() {}

    virtual void readRecord(RecordInputStream& in, Document& document)
    {

    	uint32 matrlIndex = in.readUInt32();
    	uint32 flags= in.readUInt32();
    	std:string buffer(13, '\0');
    	in.read(&buffer[0], 12);
    	uint32 shadeModel = in.readUInt32();
    	OSG_WARN << "EXTENDED_MATRL index " << matrlIndex << " flags " << flags << " shade_model "
    			<< shadeModel << " NamedMatrl " << buffer << std:endl;
    }
};
    REGISTER_FLTRECORD(ExtendedMatrlHdr, EXTENDED_MATRL_OP)
}

