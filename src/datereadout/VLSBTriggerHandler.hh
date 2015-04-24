

#ifndef VLSBMASTERTRAILER_H
#define VLSBMASTERTRAILER_H

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

/*! The struct containing the relevant information about the equipment
* retrieved from the DATE database */
typedef struct {
	char *baseAddr;
	long32 *GEO;
} VLSBMasterTriggerHandler_ParType;

/*! Arm the VLSBMaster - sets up addresses - no hardware action */
void ArmVLSBMasterTriggerHandler( char * );

/*! Clean up at the end of the run, again nothing
* for VLSBMaster here */
void DisArmVLSBMasterTriggerHandler( char * );

/*! Placeholder - never used */
void AsynchReadVLSBMasterTriggerHandler( char * );

/*! Hardware actions upon receipt of DAQ trigger
* if START_OF_BURST enable trigger distribution
* \param parameterPointer - pointer to paramter description
* \param eventHeader - pointer to event header
* \param equipmentHeader - pointer to equipment header
* \param dataPointer - pointer to the current location of data 
* \return numberOfBytesRead */
int ReadEventVLSBMasterTriggerHandler( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);

/*! Not used by VLSBMaster type */
int EventArrivedVLSBMasterTriggerHandler( char * );

/*! Placeholder - never used */
void PauseArmVLSBMasterTriggerHandler( char *);

/*! Placeholder - never used */
void PauseDisArmVLSBMasterTriggerHandler( char *);

#endif


