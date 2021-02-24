
CONFIGURE_FILE (
  SoDaUtilsConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/SoDaUtilsConfigVersion.cmake"
  @ONLY
  )  

INSTALL(FILES ${PROJECT_BINARY_DIR}/SoDaUtilsConfigVersion.cmake DESTINATION "share/cmake/Modules")
INSTALL(FILES SoDaUtilsConfig.cmake DESTINATION "share/cmake/Modules")

INSTALL(FILES FindSoDaUtils.cmake DESTINATION "share/cmake/Modules")

