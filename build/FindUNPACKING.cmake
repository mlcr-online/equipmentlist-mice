MESSAGE(STATUS "\n Looking for UNPACKING...")

FIND_PATH(UNPACKING_LIB_DIR NAMES "libMDUnpack.so"          PATHS
                                                           ../../unpacking-mice/lib/
                                                           NO_DEFAULT_PATH)

FIND_PATH(UNPACKING_INCLUDE_DIR NAMES "MDdataWordV830.h"  PATHS
                                                           ../../unpacking-mice/src/
                                                           NO_DEFAULT_PATH)

if (UNPACKING_LIB_DIR AND UNPACKING_INCLUDE_DIR)

  set(UNPACKING_FOUND         TRUE)

  MESSAGE(STATUS "Unpacking found : ${UNPACKING_LIB_DIR}\n")

else (UNPACKING_LIB_DIR AND UNPACKING_INCLUDE_DIR)

  message(STATUS "Could NOT find Unpacking! \n")

endif (UNPACKING_LIB_DIR AND UNPACKING_INCLUDE_DIR)
