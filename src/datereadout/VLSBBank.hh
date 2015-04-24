
#ifndef VLSBBANK_H
#define VLSBBANK_H

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "MDEVLSBBank.hh"

/*! The struct containing the relevant information about the equipment
* retrieved from the DATE database */
typedef struct {	
	long32 *BankNum;
	long32 *VLSBid;
	char *VLSBaddr;	
} VLSBBank_ParType;


/*! Prepare the equipment at the start of the run
* nothing needs to be done for VLSBBanks here */
void ArmVLSBBank( char * );

/*! Clean up at the end of the run, again nothing
* For VLSBBanks here */
void DisArmVLSBBank( char * );

/*! Placeholder - never used */
void AsynchReadVLSBBank( char * );

/*! Readout of data from the VLSB memory banks
* \param parameterPointer - pointer to paramter description
* \param eventHeader - pointer to event header
* \param equipmentHeader - pointer to equipment header
* \param dataPointer - pointer to the current location of data 
* \return numberOfBytesRead  - need to increment dataPointer by this/4 */
int ReadEventVLSBBank( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);

/*! Not used for VLSBBank equipment type */
int EventArrivedVLSBBank( char * );

/*! Placeholder - never used */
void PauseArmVLSBBank( char *);

/*! Placeholder - never used */
void PauseDisArmVLSBBank( char *);

#endif


