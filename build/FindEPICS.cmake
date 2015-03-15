MESSAGE(STATUS "\n Looking for EPICS...")

FIND_PATH(EPICS_LIB_DIR        NAMES libregistryIoc.a   PATHS
                                                        $ENV{EPICS_BASE}/lib/$ENV{EPICS_HOST_ARCH}
                                                        NO_DEFAULT_PATH)

FIND_PATH(EPICS_INCLUDE_DIR    NAMES epicsTimer.h       PATHS
                                                        $ENV{EPICS_BASE}/include
                                                        NO_DEFAULT_PATH)

if (EPICS_LIB_DIR AND EPICS_INCLUDE_DIR)

  set(EPICS_FOUND         TRUE)
  set(EPICS_PATH "$ENV{EPICS_BASE}/..")
  SET(EPICS_LIBS  "-lca -lCom")
  SET(EPICS_INCLUDE_OS_DIR  "${EPICS_INCLUDE_DIR}/os/Linux")

  MESSAGE(STATUS "EPICS found. Version : ${EPICS_VERSION}\n")

else (EPICS_LIB_DIR AND EPICS_INCLUDE_DIR)

  MESSAGE(STATUS "Could NOT find EPICS! \n")

endif (EPICS_LIB_DIR AND EPICS_INCLUDE_DIR)
