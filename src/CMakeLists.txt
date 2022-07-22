
########### next target ###############

set(UTILS_SRCS
	Split.cxx
	Options.cxx
	Format.cxx
	UtilsBase.cxx
	Barrier.cxx
	)


add_library(sodautils STATIC ${UTILS_SRCS})
set_target_properties(sodautils PROPERTIES VERSION ${SoDaUtils_VERSION})
target_link_libraries(sodautils Threads::Threads)

install(TARGETS sodautils DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Export header files that are required to build loadable modules

install(DIRECTORY ../include/
		  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/SoDa"
		  FILES_MATCHING PATTERN "*.hxx")


