message(STATUS "\n Looking for CAENVME ...")

FIND_PATH(CAENVME_LIB_DIR NAMES "libCAENVME.so"  PATHS
                                              /usr/lib/
                                              NO_DEFAULT_PATH)
#SET(CAENVMELIB_DIR "/usr/lib/")

if (CAENVME_LIB_DIR)

  MESSAGE(STATUS "CAENVME found in ${CAENVME_LIB_DIR}\n")
  SET(CAENVME_FOUND TRUE)
  SET(CAENVMELIB_LIBS  "-lCAENVME")

else (MYSQL_CONFIG_EXECUTABLE)

  message(FATAL_ERROR "Could NOT find CAENVME!  FATAL! \n")

endif (CAENVME_LIB_DIR)