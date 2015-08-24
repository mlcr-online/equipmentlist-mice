# Check if cmake has the required version
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

# Set name of our project
project(equipmentList-MICE)

set(EL_MICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../")

# Load some basic macros which are needed later on

include(${EL_MICE_DIR}/build/FindCAENVME.cmake)
include(${EL_MICE_DIR}/build/FindDATE.cmake)
include(${EL_MICE_DIR}/build/FindMYSQL.cmake)
include(${EL_MICE_DIR}/build/FindEPICS.cmake)
include(${EL_MICE_DIR}/build/FindMICEDATA.cmake)
include(${EL_MICE_DIR}/build/FindUNPACKING.cmake)
include(${EL_MICE_DIR}/build/FindROOT.cmake)

if (DATE_FOUND)

  set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++0x ${DATE_CFLAGS}")
  set(CMAKE_C_FLAGS "${DATE_CFLAGS}")

else (DATE_FOUND)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dlong32=int -DdatePointer=long")

endif (DATE_FOUND)

configure_file(${EL_MICE_DIR}/build/config.h.cmake ${CMAKE_SOURCE_DIR}/readoutlib/config.hh)

set(LIBRARY_OUTPUT_PATH "${EL_MICE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${EL_MICE_DIR}/bin")

if (CAENVME_FOUND)

  INCLUDE_DIRECTORIES(    ${CMAKE_CURRENT_SOURCE_DIR}/devel
                          ${CMAKE_CURRENT_SOURCE_DIR}/readoutlib
                          ${CMAKE_CURRENT_SOURCE_DIR}/trackerlib
                          ${CMAKE_CURRENT_SOURCE_DIR}/feb_config
                          ${CMAKE_CURRENT_SOURCE_DIR}/toollib)
  LINK_DIRECTORIES(       ${EL_MICE_DIR}/lib
                          ${CAENVME_LIB_DIR})


  if (DATE_FOUND)

    INCLUDE_DIRECTORIES(  ${CMAKE_CURRENT_SOURCE_DIR}/datereadout
                          $ENV{DATE_COMMON_DEFS}
                          $ENV{DATE_RC_DIR}
                          $ENV{DATE_MONITOR_DIR}
                          $ENV{DATE_DB_DIR}
                          $ENV{DATE_SIMPLEFIFO_DIR}
                          $ENV{DATE_READLIST_DIR}
                          $ENV{DATE_BANKS_MANAGER_DIR} 
                          $ENV{DATE_BM_DIR}
                          $ENV{DATE_INFOLOGGER_DIR})

    LINK_DIRECTORIES(     $ENV{DATE_MONITOR_BIN}
                          $ENV{DATE_DB_BIN}
                          $ENV{DATE_SIMPLEFIFO_BIN}
                          $ENV{DATE_READLIST_BIN}
                          $ENV{DATE_BANKS_MANAGER_BIN}
                          $ENV{DATE_BM_BIN}
                          $ENV{DATE_INFOLOGGER_BIN}
                          $ENV{DATE_LOGBOOK_BIN})

  endif (DATE_FOUND)

  if (EPICS_FOUND)

    INCLUDE_DIRECTORIES(  ${CMAKE_CURRENT_SOURCE_DIR}/epicsinterface
                          ${EPICS_INCLUDE_DIR}
                          ${EPICS_INCLUDE_OS_DIR})

    LINK_DIRECTORIES(     ${EPICS_LIB_DIR})

  endif (EPICS_FOUND)

  if (ROOT_FOUND)
    INCLUDE_DIRECTORIES(  ${ROOT_INCLUDE_DIR})
    LINK_DIRECTORIES(     ${ROOT_LIBRARY_DIR})
  endif (ROOT_FOUND)

  if (MICEDATA_FOUND)
    INCLUDE_DIRECTORIES(  ${MICEDATA_INCLUDE_DIR})
    LINK_DIRECTORIES(     ${MICEDATA_LIB_DIR})
  endif (MICEDATA_FOUND)

  if (UNPACKING_FOUND)
    INCLUDE_DIRECTORIES(  ${UNPACKING_INCLUDE_DIR})
    LINK_DIRECTORIES(     ${UNPACKING_LIB_DIR})
  endif (UNPACKING_FOUND)

  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g" )

  message(STATUS "C flafs : "      ${CMAKE_C_FLAGS} \n)
  message(STATUS "Cxx flafs : "    ${CMAKE_CXX_FLAGS} \n)
  message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS} \n)

  MESSAGE(STATUS                   "External dependencies : " \n
                                   ${DATE_RL_LIBS} \n
                                   ${MYSQL_LIBS} \n
                                   ${CAENVMELIB_LIBS} \n
                                   ${EPICS_LIBS} \n)


  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/readoutlib)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/devel)

  if (DATE_FOUND)

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/datereadout)

  endif (DATE_FOUND)

  if (EPICS_FOUND)

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/epicsinterface)

  endif (EPICS_FOUND)

  if (UNPACKING_FOUND)

  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/toollib)

  endif (UNPACKING_FOUND)

endif (CAENVME_FOUND)


