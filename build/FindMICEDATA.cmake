
MESSAGE(STATUS "\n Looking for MICEdata...")

FIND_PATH(MICEDATA_LIB_DIR NAMES "libMICEdata.a"           PATHS
                                                           ../../maus-data/lib/
                                                           NO_DEFAULT_PATH)

FIND_PATH(MICEDATA_INCLUDE_DIR_1 NAMES "KLEvent.hh"        PATHS
                                                           ../../maus-data/src/datalib/
                                                           NO_DEFAULT_PATH)

FIND_PATH(MICEDATA_INCLUDE_DIR_2 NAMES "ReconEnums.hh"     PATHS
                                                           ../../maus-data/src/toolslib/
                                                           NO_DEFAULT_PATH)

#MESSAGE("MICEDATA_LIB_DIR: ${MICEDATA_LIB_DIR}")
#MESSAGE("MICEDATA_INCLUDE_DIR_1: ${MICEDATA_INCLUDE_DIR_1}")
#MESSAGE("MICEDATA_INCLUDE_DIR_2: ${MICEDATA_INCLUDE_DIR_2}")

if (MICEDATA_LIB_DIR AND MICEDATA_INCLUDE_DIR_1 AND MICEDATA_INCLUDE_DIR_2)

  set(MICEDATA_FOUND         TRUE)
  set(MICEDATA_INCLUDE_DIR         ${MICEDATA_INCLUDE_DIR_1}
                                    ${MICEDATA_INCLUDE_DIR_2})

  MESSAGE(STATUS "MICEData found : ${MICEDATA_LIB_DIR}\n")

  set(MICEDATA_LIBS "-lMICEdata -lMAUStools")
else (MICEDATA_LIB_DIR AND MICEDATA_INCLUDE_DIR_1 AND MICEDATA_INCLUDE_DIR_2)

  message(STATUS "Could NOT find MICEData! \n")

endif (MICEDATA_LIB_DIR AND MICEDATA_INCLUDE_DIR_1 AND MICEDATA_INCLUDE_DIR_2)
