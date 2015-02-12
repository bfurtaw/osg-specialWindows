SET(TARGET_SRC osgviewer.cpp )

find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
	message(Error "GLEW not found.")
endif(NOT GLEW_FOUND)

SET(TARGET_ADDED_LIBRARIES  ${GLEW_LIBRARIES} )

SETUP_APPLICATION(osgviewer)
