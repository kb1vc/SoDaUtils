
########### next target ###############

set(UTILS_SRCS
	Split.cxx
	Options.cxx
	Format.cxx
	)


add_library(sodautils SHARED ${UTILS_SRCS})
set_target_properties(sodautils PROPERTIES VERSION ${SoDaUtils_VERSION})

install(TARGETS sodautils DESTINATION ${LIBDIR})

# Export header files that are required to build loadable modules

install(DIRECTORY ../include/
		  DESTINATION "include/SoDa"
		  FILES_MATCHING PATTERN "*.hxx")


