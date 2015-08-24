message(STATUS "datereadout ... ")

# Create the readout executable.
# First declare the DATE object files.
SET(DATE_OBJECT_FILES               $ENV{DATE_READOUT_BIN}/readout.o
                                    $ENV{DATE_READOUT_BIN}/gdcSelection.o
                                    $ENV{DATE_READLIST_BIN}/readList_equipment.o)

SET_SOURCE_FILES_PROPERTIES(${DATE_OBJECT_FILES}    PROPERTIES
                                                    EXTERNAL_OBJECT true
                                                    GENERATED true)

# Now create the executable using the DATE object files.
ADD_EXECUTABLE(readout              equipmentList_MICE.cc
                                    ${DATE_OBJECT_FILES}
                                    DATETestExample.cc
                                    DATEMessage.cc
                                    V2718.cc
                                    TriggerReceiver.cc
                                    Trailer.cc
                                    EpicsInterface.cc
                                    PartTriggerSelector.cc
                                    V830.cc
                                    V1290.cc
                                    V1724.cc
                                    V1731.cc
                                    VRB.cc
                                    VCB.cc
                                    DBB.cc
                                    DBBChain.cc
                                    V1495.cc
                                    VLSB.cc
                                    VLSBBank.cc
                                    VLSBMaster.cc
                                    VLSBMasterTriggerHandler.cc
)

SET_TARGET_PROPERTIES(readout       PROPERTIES
                                    LINKER_LANGUAGE CXX)

# Link the executable.
if (EPICS_FOUND)

  TARGET_LINK_LIBRARIES(readout       ${CAENVMELIB_LIBS}
                                      ${DATE_RL_LIBS}
                                      EPICSInterface
                                      ${MYSQL_LIBS}
                                      MICEreadout)

else (EPICS_FOUND)

  TARGET_LINK_LIBRARIES(readout       ${CAENVMELIB_LIBS}
                                      ${DATE_RL_LIBS}
                                      ${MYSQL_LIBS}
                                      MICEreadout)

endif (EPICS_FOUND)

INSTALL (TARGETS   readout          DESTINATION    $ENV{DATE_SITE}/miceraid4-LDC)
