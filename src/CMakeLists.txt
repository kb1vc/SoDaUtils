
########### next target ###############

set(UTILS_SRCS
	Split.cxx
	Options.cxx
	Format.cxx
	)


IF(MACOSX) 
  add_library(sodautils SHARED ${UTILS_SRCS})
ELSE()
  add_library(sodautils STATIC ${UTILS_SRCS})
ENDIF()


install(TARGETS sodautils DESTINATION lib)

# Export header files that are required to build loadable modules

install(DIRECTORY ../include/
		  DESTINATION "include/SoDa"
		  FILES_MATCHING PATTERN "*.hxx")


