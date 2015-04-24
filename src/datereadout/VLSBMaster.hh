
#ifndef VLSBMASTER_H
#define VLSBMASTER_H

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "MDEVLSBMaster.hh"

/*! The struct containing the relevant information about the equipment
* retrieved from the DATE database */
typedef struct {
	char *baseAddr;
	long32 *spillDuration;
	long32 *triggerReceiver;
	long32 *triggerPeriod;
	long32 *maxEvents;
	long32 *vetoGateDelay;
	long32 *vetoGatePeriod;
	long32 *vetoWindow;
	long32 *triggerMode;
} VLSBMaster_ParType;

/*! Arm the VLSBMaster - sets up addresses - no hardware action */
void ArmVLSBMaster( char * );

/*! Clean up at the end of the run, again nothing
* for VLSBMaster here */
void DisArmVLSBMaster( char * );

/*! Placeholder - never used */
void AsynchReadVLSBMaster( char * );

/*! Hardware actions upon receipt of DAQ trigger
* if PHYSICS_EVENT disable trigger distribution
* \param parameterPointer - pointer to paramter description
* \param eventHeader - pointer to event header
* \param equipmentHeader - pointer to equipment header
* \param dataPointer - pointer to the current location of data 
* \return numberOfBytesRead */
int ReadEventVLSBMaster( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);

/*! Not used by VLSBMaster type */
int EventArrivedVLSBMaster( char * );

/*! Placeholder - never used */
void PauseArmVLSBMaster( char *);

/*! Placeholder - never used */
void PauseDisArmVLSBMaster( char *);

#endif


