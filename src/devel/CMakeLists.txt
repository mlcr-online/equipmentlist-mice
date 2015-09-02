message(STATUS "devel ... ")

ADD_EXECUTABLE(febconfig                 FEB_config.cc)
TARGET_LINK_LIBRARIES(febconfig          MICEreadout)

if (DATE_FOUND)

  ADD_EXECUTABLE(messagetest             date_messages_test.cc
                                         ../datereadout/DATEMessage.cc)
  TARGET_LINK_LIBRARIES(messagetest      ${DATE_RL_LIBS}
                                         MICEreadout)

endif (DATE_FOUND)


if (ROOT_FOUND AND MICEDATA_FOUND AND UNPACKING_FOUND)

  ADD_EXECUTABLE(emrtest                   EMR_test.cc)
  TARGET_LINK_LIBRARIES(emrtest            MICEreadout
                                           DAQTools
                                           MDUnpack)

  ADD_EXECUTABLE(eventbuild                eventbuild.cc)
  TARGET_LINK_LIBRARIES(eventbuild         MICEreadout
                                           DAQTools
                                           MDUnpack
                                           ${ROOT_LIBRARIES}
                                           ${MICEDATA_LIBS})

  ADD_EXECUTABLE(vrbdebug                  vrb_debug.cc)
  TARGET_LINK_LIBRARIES(vrbdebug           MICEreadout)

endif (ROOT_FOUND AND MICEDATA_FOUND AND UNPACKING_FOUND)

if (UNPACKING_FOUND AND ROOT_FOUND)

  ADD_EXECUTABLE(develtest                 develTest.cc)
  TARGET_LINK_LIBRARIES(develtest          MICEreadout MDUnpack ${ROOT_LIBRARIES})

# Note this is an executable from davids orginal code,
# its no longer supported.
#  ADD_EXECUTABLE(trackerTest                 trackerTest.cc)
#  TARGET_LINK_LIBRARIES(trackerTest          trackerReadout MICEreadout MDUnpack ${ROOT_LIBRARIES})


   ADD_EXECUTABLE(trackerReadout          trackerReadout.cc)
   TARGET_LINK_LIBRARIES(trackerReadout       MICEreadout MDUnpack ${ROOT_LIBRARIES} rt)

  ADD_EXECUTABLE(triggerdebug                  v1495_debug.cc)
#  ADD_EXECUTABLE(triggerdebug              triggerTest.cc)
  TARGET_LINK_LIBRARIES(triggerdebug       MICEreadout  MDUnpack ${ROOT_LIBRARIES})

endif (UNPACKING_FOUND AND ROOT_FOUND)
