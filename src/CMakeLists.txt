
########### next target ###############

set(UTILS_SRCS
	Split.cxx
	Options.cxx
	Format.cxx
	UtilsBase.cxx
	Barrier.cxx
)


if(yaml-cpp_FOUND)
  message("\n\nYAML_CPP_LIBRARIES = [${YAML_CPP_LIBRARIES}]\n\n")
  message("Making libsodaprops with yaml lib [${YAML_CPP_LIBRARIES}]")
  set(PROPS_SRCS
    PropertyTree.cxx    
    PropertyTreeYAML.cxx)    
  add_library(sodaprops_pre STATIC ${PROPS_SRCS})
  add_library(sodaprops_pre::sodaprops_pre ALIAS sodaprops_pre)
  target_link_libraries(sodaprops_pre PRIVATE ${YAML_CPP_LIBRARIES})
  set_target_properties(sodaprops_pre PROPERTIES VERSION ${SoDaUtils_VERSION})
  set_target_properties(sodaprops_pre PROPERTIES
    VERSION ${SoDaUtils_VERSION}
    SOVERSION "${SoDaUtils_VERSION_MAJOR}.${SoDaUtils_VERSION_MINOR}"
    PROJECT_LABEL "SoDaUtils"
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
  include(ArchiveMerge.cmake)
  LIST(APPEND proplibs libsodaprops_pre.a ${YAML_CPP_LIB_DIR}/${YAML_CPP_LIBRARIES})
  message("proplibs [${proplibs}]")
  combine_archives(sodaprops "${proplibs}" "sodaprops_pre")
  add_library(sodaprops::sodaprops ALIAS sodaprops)
  set_target_properties(sodaprops PROPERTIES VERSION ${SoDaUtils_VERSION})
endif()	

add_library(sodautils STATIC ${UTILS_SRCS})
add_library(sodautils::sodautils ALIAS sodautils)
set_target_properties(sodautils PROPERTIES VERSION ${SoDaUtils_VERSION})
target_link_libraries(sodautils Threads::Threads)

message("CMAKE_INSTALL_INCLUDEDIR = [${CMAKE_INSTALL_INCLUDEDIR}]")

target_include_directories(sodautils
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  #$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/SoDa>
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
