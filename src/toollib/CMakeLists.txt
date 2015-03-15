message(STATUS "toollib ... ")

#AUX_SOURCE_DIRECTORY(.                 SOURCE_FILES)
SET(SOURCE_FILES                       "./DAQManager.cc;./DAQBinaryFile.cc;./EventBuildManager.cc;")

if (ROOT_FOUND AND MICEDATA_FOUND)
 SET(SOURCE_FILES ${SOURCE_FILES}      "./DBBDataProcessor.cc;./V1731DataProcessor.cc;")
endif (ROOT_FOUND AND MICEDATA_FOUND)

#message(STATUS "${SOURCE_FILES}")
ADD_LIBRARY(DAQTools                   ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(DAQTools         MICEreadout)

