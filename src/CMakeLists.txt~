
########### next target ###############

set(COMMAND_SRCS
	Command.cxx
	)


IF(MACOSX) 
  add_library(sodacommand SHARED ${COMMAND_SRCS})
ELSE()
  add_library(sodacommand STATIC ${COMMAND_SRCS})
ENDIF()

install(TARGETS sodacommand DESTINATION lib)

# Export header files that are required to build loadable modules

install(FILES ../include/Command.hxx DESTINATION "include/SoDa")


