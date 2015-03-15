message(STATUS "feb_config ... ")

ADD_EXECUTABLE(febconf                      febdaq.cc
                                         maroc.cc
                                         vme.cc)

TARGET_LINK_LIBRARIES(febconf            ${CAENVMELIB_LIBS})

ADD_EXECUTABLE(febconfig                 FEB_config.cc)
TARGET_LINK_LIBRARIES(febconfig          MICEreadout)
