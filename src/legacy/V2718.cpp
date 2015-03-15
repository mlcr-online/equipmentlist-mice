/***************************************************************************
 *                                                                         *
 * $Log: V2718.c,v $
 * Revision 1.1  2007/06/08 17:17:56  daq
 * Initial revision
 *
 *                                                                         *
 * equipmentList functions for the VME-PCI interface CAEN V2718            *
 * This equipment should be the very first to be armed (and therefore the  *
 * last to be disarmed). It initializes the interface and provides the     * 
 * handle needed for subsequent VME cycles.                                *
 * This equipment is virtual and doesn't generate data.                    *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#include "V2718.h"

long V2718_Handle; // V2718 handle
int  vmeStatus;
unsigned long addr;

// VME Write 16 bits data for debugging. Use VME_WRITE_16 macro for tested code
int _Write16( unsigned long ad, short dt, int err ){
    unsigned long a = ad;
    short d = dt;
    vmeStatus = CAENVME_WriteCycle( V2718_Handle, a, &d, cvA32_U_DATA, cvD16 );
    if( vmeStatus != cvSuccess )
    {
        DO_ERROR{
            sprintf( message, "Writing error %d. (%d)", err, vmeStatus );
            ERROR( message );
        }
    }
    return vmeStatus;
}

void ArmV2718( char *parPtr ) {
  V2718_ParType * V2718 = (V2718_ParType*) parPtr;
  //Initialize V2718
  vmeStatus = CAENVME_Init( cvV2718, *V2718->BLink, *V2718->BNumber, &V2718_Handle );
  if( vmeStatus != cvSuccess ) {
    readList_error = VME_ERROR;
    DO_ERROR {
      sprintf( message, "Error arming V2718 with following parameters - BLink = %d, BNumber = %d Handle = %#lX. Error # %d", 
      *V2718->BLink, 
      *V2718->BNumber,
      V2718_Handle,
      vmeStatus );
      ERROR(message);
    }
  } else {
    vmeStatus = CAENVME_SetOutputConf( V2718_Handle, cvOutput0, cvDirect , 
                                       cvActiveHigh, cvManualSW );

    vmeStatus |= CAENVME_SetOutputConf( V2718_Handle, cvOutput4, cvDirect , 
                                        cvActiveHigh, cvManualSW );

    if( vmeStatus != cvSuccess ){
      readList_error = VME_ERROR;
      DO_ERROR {
        sprintf( message, "Error arming V2718 - setting output conf. Error # %d", vmeStatus );
        ERROR(message);
      }
    } else {
      DO_INFO {
        sprintf( message, "Arming V2718( %#lX ) successful",V2718_Handle );
        INFO(message);
      }
    }
  }
}

void DisArmV2718( char *parPtr) {
  // This will be the last equipment to be disarmed
  vmeStatus = CAENVME_End( V2718_Handle );
  if( vmeStatus != cvSuccess ){
    DO_ERROR{
      sprintf( message, "Error while closing connection with V2718. Error # %d", vmeStatus );
      ERROR(message);
    }
  } else  DO_INFO {
    sprintf( message, "End of Disarming V2718" );
    INFO(message);
  }
}

void AsynchReadV2718( char *parPtr){}

int ReadEventV2718( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  return 0;
}

int EventArrivedV2718( char *parPtr ) {
  return 0;
}

void PauseArmV2718( char *) {}

void PauseDisArmV2718( char *) {}
