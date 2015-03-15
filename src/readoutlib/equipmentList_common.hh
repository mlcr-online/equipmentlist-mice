#ifndef _EUIPMENTLIST_COMMON_H_
#define _EUIPMENTLIST_COMMON_H_ 1

#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <stdio.h> 
#include <stdint.h>
#include <unistd.h> 
#include <stdint.h>

#include <sys/time.h> // needed for gettimeofday

/**************** CMAKE Auto-Generated Include ******************************/
#include "config.hh"


typedef uint32_t MDE_Pointer;
typedef void(*UserPrintCall)(std::stringstream &ss);

/**************** MiceDAQMessanger and MiceDAQException ************************/
#define MESSAGESTREAM \
*(mde_messanger_->getStream(this))
typedef void(*MessageCall)(std::string, int);

enum MDESeverity {
  MDE_DEBUGGING=0,
  MDE_INFO=1,
  MDE_WARNING=2,
  MDE_ERROR=3,
  MDE_FATAL=4
};

/*================= MICE Start ==================================*/
#define SET_PARTICLE_TRIGGERS(t, npt)  (p[0] = npt | 0xFFFF0000)
#define GET_PARTICLE_TRIGGERS(t)       (p[0] & 0x0000FFFF )
/*================= MICE Ends= ==================================*/


// #define min(x,y) ((x)>(y)?(y):(x))
// #define max(x,y) ((x)>(y)?(x):(y))

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


/**************** CAEN includes and defs ******************************/
#include "CAENVMEtypes.h"
#include "CAENVMElib.h"

#define VME_WRITE_16(add,dat) \
CAENVME_WriteCycle( mde_V2718_Handle_, add, &dat, cvA32_U_DATA, cvD16 )

#define VME_WRITE_32(add,dat) \
CAENVME_WriteCycle( mde_V2718_Handle_, add, &dat, cvA32_U_DATA, cvD32 )

#define VME_WRITE_BLT_32(add,dat,nb,nbr_ptr) \
CAENVME_BLTWriteCycle( mde_V2718_Handle_, add, &dat, nb, cvA32_U_BLT, cvD32, nbr_ptr)

#define VME_READ_16(add,dat_ptr) \
CAENVME_ReadCycle( mde_V2718_Handle_, add, dat_ptr, cvA32_U_DATA, cvD16 )

#define VME_READ_32(add,dat_ptr) \
CAENVME_ReadCycle( mde_V2718_Handle_, add, dat_ptr, cvA32_U_DATA, cvD32 )

#define VME_MULTIREAD_32(addr_ptr, dat_ptr, n_cycles) \
{ \
uint32_t *dat_ptr_tmp = static_cast<uint32_t*>(dat_ptr); \
std::vector<CVAddressModifier>  AMs(n_cycles, cvA32_U_DATA); \
std::vector<CVDataWidth>        DWs(n_cycles, cvD32); \
std::vector<CVErrorCodes>       ECs(n_cycles, cvSuccess); \
mde_vmeStatus_ = \
CAENVME_MultiRead( mde_V2718_Handle_, addr_ptr, dat_ptr_tmp, n_cycles, &AMs[0], &DWs[0], &ECs[0]); \
}

#define VME_READ_BLT_32(add,dat_ptr,nb,nbr_ptr) \
CAENVME_BLTReadCycle( mde_V2718_Handle_, add, dat_ptr, nb, cvA32_U_BLT, cvD32, nbr_ptr)

#define VME_WRITE_16_A24(add,dat) \
CAENVME_WriteCycle( mde_V2718_Handle_, add, &dat, cvA24_U_DATA, cvD16 )

#define VME_READ_16_A24(add,dat_ptr) \
CAENVME_ReadCycle( mde_V2718_Handle_, add, dat_ptr, cvA24_U_DATA, cvD16 )

#define VME_WRITE_8_A24(add,dat) \
CAENVME_WriteCycle( mde_V2718_Handle_, add, &dat, cvA24_U_DATA, cvD8 )

#define VME_READ_8_A24(add, dat_ptr) \
CAENVME_ReadCycle( mde_V2718_Handle_, add, dat_ptr, cvA24_U_DATA, cvD8 )





#ifdef DATE_FOUND

  #include "event.h"

#else

  //typedef int  long32;
  //typedef long datePointer;
  // The type deffinitions are now cxx flags. See  ${CMAKE_CXX_FLAGS} in src/CMakeLists.txt .

  /* ----------DATE Event types ---------- */
  typedef unsigned int eventTypeType;
  #define START_OF_RUN                    ((eventTypeType) 1)
  #define END_OF_RUN                      ((eventTypeType) 2)
  #define START_OF_BURST                  ((eventTypeType) 5)
  #define END_OF_BURST                    ((eventTypeType) 6)
  #define PHYSICS_EVENT                   ((eventTypeType) 7)
  #define CALIBRATION_EVENT               ((eventTypeType) 8)

#endif

#ifdef EPICS_FOUND
  // **************** EPICS Interface Includes and typedefs ******************************
  #include "EpicsInterfaceTypes.hh"

#endif

int decodeBinStr( char * BStr );               // Decodes binary string to int (e.g. '1011' = 11)
int decodeChar( char * Ch );                   //Converts chars 1,0,Y,y,N or n to int 1 or 0
long32 channelPattern( char * channelConfig ); //recognizes "all", "none" or hexadecimal representation of channel mask and converts it to integer
int bitCount( int Number );                    //Gives the number of the "1" bits in an int
long32 getBA( char * BA );
int getHighestBitNumber( long32 Word );        // returns the highest bit set in 32 bit word
std::string getVmeStatusCodeName(int status);


#endif


