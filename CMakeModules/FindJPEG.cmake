# - Find JPEG 
# Added to support Windows build but should work anywhere.
# After looking in UNIX standard places
#
# defines cache variables
#  JPEG_INCLUDE_DIR, where to find headers
#  JPEG_LIBRARIES, list of release link libraries
#  JPEG_DEBUG_LIBRARIES, list of debug link libraries
#  JPEG_FOUND, If != "YES", do not try to use JPEG

FIND_PATH(JPEG_INCLUDE_DIR jpeglib.h
  ${ACTUAL_3RDPARTY_DIR}/jpeg
  /usr/local/include
  /usr/include
)

FIND_LIBRARY(JPEG_LIBRARY 
    NAMES jpeg libjpeg
    PATHS
    ${ACTUAL_3RDPARTY_DIR}/jpeg/Release
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    /usr/freeware/lib64
)

SET(JPEG_FOUND "NO")
IF(JPEG_INCLUDE_DIR AND JPEG_LIBRARY)
	SET(JPEG_FOUND "YES")
	MESSAGE("Found another jpeg .lib")
ENDIF(JPEG_INCLUDE_DIR AND JPEG_LIBRARY)


MARK_AS_ADVANCED(JPEG_INCLUDE_DIR JPEG_LIBRARIES )

