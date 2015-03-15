#ifndef _EUIPMENTLIST_COMMON_H_
#define _EUIPMENTLIST_COMMON_H_ 1

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h> // needed for gettimeofday

/**************** CMAKE Auto-Generated Include ******************************/
#include "config.h"


// **************** MICESoftware Includes ******************************
#include "DATEStatusClient.hh"
#include "DATEStatusServerDescription.hh"
#include "DATEStatusServerDescriptionXmlReader.hh"
#include "DATEPVDescription.h"
#include "DATEPVDescriptionXmlReader.h"
#include "PVClient.hh"

/*================= MICE Start ==================================*/
#define SET_PARTICLE_TRIGGERS(t, npt)  (p[0] = npt | 0xFFFF0000)
#define GET_PARTICLE_TRIGGERS(t)       (p[0] & 0x0000FFFF )
/*================= MICE Ends= ==================================*/


#define min(x,y) ((x)>(y)?(y):(x))
#define max(x,y) ((x)>(y)?(x):(y))

/* Definition of errors of ErrGen */
#define ARM_ERROR 1
#define DISARM_ERROR 2
#define READEVENT_ERROR 3
#define READEVENT_CRASH 4   /* after 10 events */
#define ARM_CRASH 5
#define NB_IN_RUN_INCR 11   /* after 10 events */
#define NB_IN_RUN_DECR 12   /* after 10 events */
#define NB_IN_BURST_INCR 15 /* after 10 events */
#define NB_IN_BURST_DECR 16 /* after 10 events */
#define BURST_NB_INCR 17    /* after 10 events */
#define BURST_NB_DECR 18    /* after 10 events */

/* Definition of readlist_error values used in MICE readout */
/* (setting readList_error to non-zero value will stop the run) */
#define GENERIC_ERROR     101 // used by default
#define VME_ERROR         102 // communication error on the VME bus
#define SYNC_ERROR        103 // Event consistency error
#define PARAM_ERROR       104 // Event consistency error


/**************** CAEN Includes and defs ******************************/
#include "CAENVMEtypes.h"
#include "CAENVMElib.h"

#define VME_WRITE_16(add,dat) \
CAENVME_WriteCycle( V2718_Handle, add, &dat, cvA32_U_DATA, cvD16 )

#define VME_WRITE_32(add,dat) \
CAENVME_WriteCycle( V2718_Handle, add, &dat, cvA32_U_DATA, cvD32 )

#define VME_READ_16(add,dat_ptr) \
CAENVME_ReadCycle( V2718_Handle, add, dat_ptr, cvA32_U_DATA, cvD16 )

#define VME_READ_32(add,dat_ptr) \
CAENVME_ReadCycle( V2718_Handle, add, dat_ptr, cvA32_U_DATA, cvD32 )

#define VME_READ_BLT_32(add,dat_ptr,nb,nbr_ptr) \
CAENVME_BLTReadCycle( V2718_Handle, add, dat_ptr, nb, cvA32_U_BLT, cvD32, nbr_ptr)

#define VME_WRITE_16_A24(add,dat) \
CAENVME_WriteCycle( V2718_Handle, add, &dat, cvA24_U_DATA, cvD16 )

#define VME_READ_16_A24(add,dat_ptr) \
CAENVME_ReadCycle( V2718_Handle, add, dat_ptr, cvA24_U_DATA, cvD16 )


int decodeBinStr( char * BStr ); // Decodes binary string to int (e.g. '1011' = 11) 
int decodeChar( char * Ch ); //Converts chars 1,0,Y,y,N or n to int 1 or 0
long32 channelPattern( char * channelConfig ); //recognizes "all", "none" or hexadecimal representation of channel mask and converts it to integer
int bitCount( int Number ); //Gives the number of the "1" bits in an int
long32 getBA( char * BA );

int getHighestBitNumber( long32 Word ); // returns the highest bit set in 32 bit word

#endif


