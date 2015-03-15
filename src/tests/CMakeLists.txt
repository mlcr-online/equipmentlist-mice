message(STATUS "testslib ... ")

ADD_LIBRARY(MDEtests                  MDETestLib.cc)
TARGET_LINK_LIBRARIES(MDEtests        MICEreadout MDUnpack DAQTools)

ADD_EXECUTABLE(equipmentTest          equipmentTest.cc)
TARGET_LINK_LIBRARIES(equipmentTest   MDEtests)

if (EPICS_FOUND)

  ADD_EXECUTABLE(epicstest               epicstest.cc)
  TARGET_LINK_LIBRARIES(epicstest        MICEreadout
                                         EPICSInterface)

  ADD_EXECUTABLE(cmtest                  cm_test.cc)
  TARGET_LINK_LIBRARIES(cmtest           MICEreadout
                                         EPICSInterface)

endif (EPICS_FOUND)
