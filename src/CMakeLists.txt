
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
set_target_properties(sodautils PROPERTIES VERSION ${SoDaUtils_VERSION})
target_link_libraries(sodautils Threads::Threads ${YAML_CPP_LIBRARIES})

install(TARGETS sodautils DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Export header files that are required to build loadable modules

install(DIRECTORY ../include/
		  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/SoDa"
		  FILES_MATCHING PATTERN "*.hxx")


