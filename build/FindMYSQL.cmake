MESSAGE(STATUS "\n Looking for MYSQL...")

FIND_PROGRAM(MYSQL_CONFIG_EXECUTABLE NAMES mysql_config PATHS
                                                        /usr/bin/
                                                        NO_DEFAULT_PATH)

if (MYSQL_CONFIG_EXECUTABLE)

  set(MYSQL_FOUND         TRUE)

  EXEC_PROGRAM(${MYSQL_CONFIG_EXECUTABLE} ARGS  "--libs_r"  OUTPUT_VARIABLE  MYSQL_LIBS)
  EXEC_PROGRAM(${MYSQL_CONFIG_EXECUTABLE} ARGS  "--version" OUTPUT_VARIABLE  MYSQL_VERSION)

  MESSAGE(STATUS "MySQL found. Version : ${MYSQL_VERSION}\n")

else (MYSQL_CONFIG_EXECUTABLE)

  MESSAGE(STATUS "Could NOT find MySQL. \n")

endif (MYSQL_CONFIG_EXECUTABLE)
