message(STATUS "legacy ... ")

#CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/Config.h")

# Make a library.
ADD_LIBRARY(MICEReadout             DATETestExample.cpp
                                    equipmentList_common.cpp
                                    EpicsClient.cpp
                                    PartTriggerSelector.cpp
                                    DBB.cpp
                                    Trailer.cpp
                                    TriggerReceiver.cpp
                                    V1290.cpp
                                    V1724.cpp
                                    V1731.cpp
                                    V2718.cpp
                                    V830.cpp
                                    VLSBBank.cpp
                                    VLSB.cpp
                                    VLSBCosmicTrgManager.cpp
                                    VLSBMaster.cpp
                                    VRB.cpp)

TARGET_LINK_LIBRARIES(MICEReadout   ${CAENVMELIB_LIBS}
                                    ${DATE_RL_LIBS})


# Create the readout executable.
# First declare the DATE object files.
SET(DATE_OBJECT_FILES               $ENV{DATE_READOUT_BIN}/readout.o
                                    $ENV{DATE_READOUT_BIN}/gdcSelection.o
                                    $ENV{DATE_READLIST_BIN}/readList_equipment.o)

SET_SOURCE_FILES_PROPERTIES(${DATE_OBJECT_FILES}    PROPERTIES
                                                    EXTERNAL_OBJECT true
                                                    GENERATED true)

# Now create the executable using the DATE object files.
ADD_EXECUTABLE(readout              equipmentList_MICE.cpp
                                    ${DATE_OBJECT_FILES})

SET_TARGET_PROPERTIES(readout      PROPERTIES
                                   LINKER_LANGUAGE CXX)

# Link the executable.
TARGET_LINK_LIBRARIES(readout       MICEReadout
                                    ${DATE_RL_LIBS}
                                    EPICSInterface
                                    ${MYSQL_LIBS})

INSTALL (TARGETS   readout          DESTINATION    $ENV{DATE_SITE}/miceraid4-LDC)
