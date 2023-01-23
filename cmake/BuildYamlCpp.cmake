message("In build yaml-cpp cmake")
include(GNUInstallDirs)

# SoDaUtils depends on yaml-cpp if we're going to
# provide the property tree library. 

include(ExternalProject)

# If we find the package, yippeeee!
FIND_PACKAGE(yaml-cpp QUIET)
message("yaml-cpp_FOUND = ${yaml-cpp_FOUND}")
# can we find a static version
if(yaml-cpp_FOUND)
  message("yaml-cpp_FOUND = [${yaml-cpp_FOUND}]")
  message("YAML_CPP_LIBRARIES [${YAML_CPP_LIBRARIES}]")
  message("YAML_CPP_INCLUDE_DIR = [${YAML_CPP_INCLUDE_DIR}]")
  message("YAML_CPP_LIBRARY_DIR [${YAML_CPP_LIBRARY_DIR}]")
  find_library(YAML_CPP_LIB_STATIC 
    NAMES ${YAML_CPP_LIBRARIES}.a
    #NO_DEFAULT_PATH
    PATH ${YAML_CPP_LIBRARY_DIR})
  message("YAML_CPP_LIB_STATIC [${YAML_CPP_LIB_STATIC}]")
  if(YAML_CPP_LIB_STATIC-NOTFOUND)
    message("\n\n!!!! Couldn't find a static version of yaml-cpp YAML_CPP_LIB_STATIC = [${YAML_CPP_LIB_STATIC}]\n\n")
    set(yaml_cpp_FOUND 0)
    message("after set to zero yaml-cpp_FOUND = [${yaml-cpp_FOUND}]")    
  else()
    set(YAML_CPP_LIBRARIES libyaml-cpp.a)
    message("\n\n\n###### YAML LIB [${YAML_CPP_LIBRARIES}]")
  endif()
endif()

message("before test for local build yaml-cpp_FOUND = [${yaml-cpp_FOUND}]")      
IF(yaml-cpp_FOUND)
  # we need to create the lib dir for yaml-cpp
  message("YAML_CPP_LIBRARIES [${YAML_CPP_LIBRARIES}]")
  # alas YAML_CPP_LIB_DIR isn't defined in the config file (!)
  # we need to create it from YAML_CPP_CMAKE_DIR (!)
  set(YAML_CPP_LIB_DIR "${YAML_CPP_CMAKE_DIR}/../../")
ELSE()
  message("\n\n****Building yaml-cpp package***\n\n")
  # The SoDa::Utils package hasn't been installed.
  # Get it and build it as an external package.
  # get sodaformat
  ExternalProject_Add(
    yaml-cpp
    #  PREFIX ${PROJECT_BINARY_DIR}/sodaformat-kit
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 1b50109f7  # did all the testing against this one.
    SOURCE_DIR yaml-kit
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>" "-DYAML_BUILD_SHARED_LIBS=OFF"
  )

  ExternalProject_Get_Property(yaml-cpp INSTALL_DIR)
  message("About to set yaml-cpp stuff... to ${INSTALL_DIR}")
  set(YAML_CPP_ROOT ${INSTALL_DIR})
  set(YAML_CPP_INCLUDE_DIR ${YAML_CPP_ROOT}/include)
  set(YAML_CPP_LIBRARIES ${YAML_CPP_ROOT}/${CMAKE_INSTALL_LIBDIR}/libyaml-cpp.a)
  set(YAML_CPP_LIB_DIR ${YAML_CPP_ROOT}/${CMAKE_INSTALL_LIBDIR})
  set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/libyaml-cpp)
  set(yaml-cpp_FOUND 1)
  message("####YAML_CPP_LIBRARIES [${YAML_CPP_LIBRARIES}]")
ENDIF()


  
