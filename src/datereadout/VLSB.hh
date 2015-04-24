
#ifndef VLSB_H
#define VLSB_H

#include "MDEVLSB.hh"
#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

/*! The struct containing the relevant information about the equipment
* retrieved from the DATE database */
typedef struct {
	long32 *id;	
	char *baseAddr;
	short zeroSuppress;
} VLSB_ParType;

/*! Prepare the equipment at the start of the run
* nothing needs to be done for VLSBs here */
void ArmVLSB( char * );

/*! Clean up at the end of the run, again nothing
* For VLSBs here */
void DisArmVLSB( char * );

/*! Placeholder - never used */
void AsynchReadVLSB( char * );

/*! Communication with VLSB at DAQ trigger
* if event type = START_OF_BURST put VLSBs into data acquire mode
* if event type = PHYSICS_EVENT put VLSBs into readout mode 
* \param parameterPointer - pointer to paramter description
* \param eventHeader - pointer to event header
* \param equipmentHeader - pointer to equipment header
* \param dataPointer - pointer to the current location of data 
* \return numberOfBytesRead */
int ReadEventVLSB( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);

/*! Not used for VLSB equipment type */
int EventArrivedVLSB( char * );

/*! Placeholder - never used */
void PauseArmVLSB( char *);

/*! Placeholder - never used */
void PauseDisArmVLSB( char *);

#endif


