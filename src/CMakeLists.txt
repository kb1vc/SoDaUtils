
########### next target ###############

set(UTILS_SRCS
	Split.cxx
	Options.cxx
	Format.cxx
	UtilsBase.cxx
	Barrier.cxx
)
      
if(yaml-cpp_FOUND)
  list(APPEND UTILS_SRCS
    PropertyTree.cxx    
    PropertyTreeYAML.cxx)    
endif()	

add_library(sodautils STATIC ${UTILS_SRCS})
add_library(sodautils::sodautils ALIAS sodautils)
set_target_properties(sodautils PROPERTIES VERSION ${SoDaUtils_VERSION})
target_link_libraries(sodautils Threads::Threads ${YAML_CPP_LIBRARIES})

target_include_directories(sodautils
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  # $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

target_sources(sodautils
  PRIVATE
  ${UTILS_SRCS})

set_target_properties(sodautils PROPERTIES
  VERSION ${SoDaUtils_VERSION}
  SOVERSION "${SoDaUtils_VERSION_MAJOR}.${SoDaUtils_VERSION_MINOR}"
  PROJECT_LABEL "SoDaUtils"
  DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
