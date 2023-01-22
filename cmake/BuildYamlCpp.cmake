message("In build yaml-cpp cmake")
include(GNUInstallDirs)

# SoDaUtils depends on yaml-cpp if we're going to
# provide the property tree library. 

include(ExternalProject)

# If we find the package, yippeeee!
FIND_PACKAGE(yaml-cpp QUIET)
message("yaml-cpp_FOUND = ${yaml-cpp_FOUND}")
IF(NOT yaml-cpp_FOUND)
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
  set_property(TARGET yaml-cpp PROPERTY IMPORTED_LOCATION ${YAML_CPP_LIB_DIR}/libyaml-cpp)
  set(yaml-cpp_FOUND 1)
ENDIF()


  
