message(STATUS "trackerlib ... ")

AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
message(STATUS "${SOURCE_FILES}")

ADD_LIBRARY(trackerReadout                ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(trackerReadout      ${CAENVMELIB_LIBS})
