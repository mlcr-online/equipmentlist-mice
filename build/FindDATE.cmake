MESSAGE(STATUS "\n Looking for DATE...")

FIND_PROGRAM(DATE_CONFIG_EXECUTABLE NAMES date-config PATHS
                                                      /opt/date/.commonScripts/
                                                      NO_DEFAULT_PATH)

set(DATE_ENV $ENV{DATE_COMMON_DEFS})

if (DATE_CONFIG_EXECUTABLE)

  if (DATE_ENV)

    set(DATE_FOUND         TRUE)
    EXEC_PROGRAM(${DATE_CONFIG_EXECUTABLE} ARGS  "--version"      OUTPUT_VARIABLE  DATE_VERSION)
    EXEC_PROGRAM(${DATE_CONFIG_EXECUTABLE} ARGS  "--cflags"       OUTPUT_VARIABLE  DATE_CFLAGS)
    EXEC_PROGRAM(${DATE_CONFIG_EXECUTABLE} ARGS  "--ldflags"      OUTPUT_VARIABLE  DATE_LINKER_FLAGS)
    #EXEC_PROGRAM(${DATE_CONFIG_EXECUTABLE} ARGS  "--readlistlibs" OUTPUT_VARIABLE  DATE_RL_LIBS)
    SET(DATE_RL_LIBS  "-lproducer -lDb -lFifo -lDateBufferManager -lBanksManager -lEquipmentConfig -lInfo")
    EXEC_PROGRAM(${DATE_CONFIG_EXECUTABLE} ARGS  "--monitorlibs"  OUTPUT_VARIABLE  DATE_MON_LIBS)

    SET(dataPtrType $ENV{DATE_POINTER})
    SET(DATE_SITE_CONFIG_FILES $ENV{DATE_SITE_CONFIG})
    MESSAGE(STATUS "DATE found. Version : ${DATE_VERSION}\n")

  else (DATE_ENV)

    MESSAGE(STATUS "DATE found, but DATE environment not loaded! \n")

  endif (DATE_ENV)

else (DATE_CONFIG_EXECUTABLE)

  message(STATUS "Could not find DATE! \n")

endif (DATE_CONFIG_EXECUTABLE)

