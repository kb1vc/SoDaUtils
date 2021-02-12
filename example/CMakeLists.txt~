CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0) # this lets out really old distros like CentOS 6

IF(CMAKE_VERSION VERSION_GREATER 3.0.0)
  CMAKE_POLICY(SET CMP0048 NEW)
  SET(CMAKE_CXX_STANDARD 11)
ENDIF()

PROJECT(SoDaCommandExample)

# optimizations
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

SET(CMAKE_CXX_FLAGS_DEBUG "-g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")

# This is all you need to do if the SoDaCommand stuff
# was installed correctly. 
FIND_PACKAGE(SoDaCommand REQUIRED)

########### next target ###############

# Export header files that are required to build loadable modules

add_executable(CommandExample CommandExample.cxx)
target_link_libraries(CommandExample ${SoDaCommand_LIBRARIES})
target_include_directories(CommandExample PUBLIC ${SoDaCommand_INCLUDE_DIRS})



