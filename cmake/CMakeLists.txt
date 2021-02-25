
CONFIGURE_FILE (
  SoDaUtilsConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/SoDaUtilsConfigVersion.cmake"
  @ONLY
  )  

INSTALL(FILES ${PROJECT_BINARY_DIR}/SoDaUtilsConfigVersion.cmake DESTINATION "lib/cmake/SoDaUtils")
INSTALL(FILES SoDaUtilsConfig.cmake DESTINATION "lib/cmake/SoDaUtils")

#INSTALL(FILES FindSoDaUtils.cmake DESTINATION "lib/cmake/SoDaUtils")

