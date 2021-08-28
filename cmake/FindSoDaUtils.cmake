# Find the SoDaUtils library
# This defines:

#  SoDaUtils_INCLUDE_DIR contains SoDa/*.h
#  SoDaUtils_LIBRARIES points to libsodautils...
#  SoDaUtils_FOUND true if we found it. 

find_path(SoDaUtils_INCLUDE_DIR
  NAMES SoDa/Format.hxx
  )

find_library(SoDaUtils_LIBRARY sodautils)


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SoDaUtils
  REQUIRED_VARS SoDaUtils_LIBRARY SoDaUtils_INCLUDE_DIR)

if(SoDaUtils_FOUND)
  set( SoDaUtils_LIBRARIES ${SoDaUtils_LIBRARY} )
endif()

mark_as_advanced(SoDaUtils_INCLUDE_DIR SoDaUtils_LIBRARY)
