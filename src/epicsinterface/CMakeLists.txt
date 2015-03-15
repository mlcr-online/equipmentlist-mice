message(STATUS "epicsinterface ... ")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)

ADD_LIBRARY(EPICSInterface             ${SOURCE_FILES})

if (DATE_FOUND)

  TARGET_LINK_LIBRARIES(EPICSInterface   ${EPICS_LIBS}
                                         ${MYSQL_LIBS}
                                         ${DATE_RL_LIBS})
else (DATE_FOUND)

  TARGET_LINK_LIBRARIES(EPICSInterface   ${EPICS_LIBS})

endif (DATE_FOUND)

#ADD_EXECUTABLE(simplecaget             simplecaget.cc
#                                       DAQPVClient.cc)
#TARGET_LINK_LIBRARIES(simplecaget      ${EPICS_LIBS}
#                                       MICEreadout)

#ADD_EXECUTABLE(simplecagetcb           simplecagetcb.cc)
#TARGET_LINK_LIBRARIES(simplecagetcb    ${EPICS_LIBS}
#                                       MICEreadout)