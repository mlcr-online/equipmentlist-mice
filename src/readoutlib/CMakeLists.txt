message(STATUS "readoutlib ... ")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
# message(STATUS "${SOURCE_FILES}")
ADD_LIBRARY(MICEreadout                ${SOURCE_FILES})

# ADD_LIBRARY(MICEreadout                MiceDAQEquipment.cc
#                                        equipmentList_common.cc
#                                        MiceDAQParamHolder.cc
#                                        MiceDAQMessanger.cc
#                                        MiceDAQException.cc
#                                        MiceDAQSpillStats.cc
#                                        MiceDAQRunStats.cc
#                                        MDEv2718.cc
#                                        MDEv1290.cc
#                                        MDEv1731.cc
#                                        MDEv1724.cc
#                                        MDEv830.cc
#                                        MDEv977.cc
#                                        MDEvRB.cc
#                                        MDEvCB.cc
#                                        MDEDBB.cc
#                                        MDEv1495.cc)

TARGET_LINK_LIBRARIES(MICEreadout      ${CAENVMELIB_LIBS})
