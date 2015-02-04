SET(TARGET_SRC
	 ActiveUniforms.cpp
    AncillaryRecords.cpp
    AttrData.cpp
    ControlRecords.cpp
    DataInputStream.cpp
    DataOutputStream.cpp
    Document.cpp
    expAncillaryRecords.cpp
    expControlRecords.cpp
    expGeometryRecords.cpp
    expPrimaryRecords.cpp
    ExportOptions.cpp
    ExtendedMaterialRecord.cpp
    FltExportVisitor.cpp
    GeometryRecords.cpp
	 GLCode.cpp
    LightPointRecords.cpp
    LightSourcePaletteManager.cpp
    MaterialPaletteManager.cpp
    PaletteRecords.cpp
    Pools.cpp
    PrimaryRecords.cpp
    ReaderWriterATTR.cpp
    ReaderWriterFLT.cpp
    Record.cpp
    RecordInputStream.cpp
    Registry.cpp
    ReservedRecords.cpp
    RoadRecords.cpp
    TexturePaletteManager.cpp
    Vertex.cpp
    VertexPaletteManager.cpp
    VertexRecords.cpp
	 NV/NvAssetLoader.cpp
	 NV/NvTimers.cpp
	 NV/NvGLSLProgram.cpp
)

SET(TARGET_H
	 ActiveUniforms.h
    AttrData.h
    ConvertToVertexAttributeArrays.h
    DataInputStream.h
    DataOutputStream.h
    ExportOptions.h
    ExtendedMaterial.h
    FltExportVisitor.h
    FltWriteResult.h
	 GLCode.h
    Document.h
    LightSourcePaletteManager.h
    MaterialPaletteManager.h
    Pools.h
    Record.h
    RecordInputStream.h
    Registry.h
    TexturePaletteManager.h
    Vertex.h
    VertexPaletteManager.h
    Opcodes.h
    Types.h
    Utils.h
	 NV/NvFoundation.h
	 NV/NvAssetLoader.h
	 NV/NvGLSLProgram.h
	 NV/NvLogs.h
	 NV/NvStopWatch.h
	 NV/NvTimers.h
)
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
	message(Error "GLEW not found.")
endif(NOT GLEW_FOUND)

SET(TARGET_ADDED_LIBRARIES osgSim ${GLEW_LIBRARIES} )

#### end var setup  ###
SETUP_PLUGIN(openflight)
