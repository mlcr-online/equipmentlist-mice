/***************************************************************************
 *                                                                         *
 * $Log: Trailer.c,v $
 * Revision 1.2  2007/06/15 14:05:27  root
 * Implement CM18 Trigger connection scheme
 *
 * Revision 1.1  2007/06/08 17:17:56  daq
 * Initial revision
 *
 *                                                                         *
 * equipmentList functions for the Trailer                                 *
 * Trailer is a virtual equipment called always at the end of the list     *
 * for Arm and ReadEvent and called always at the begining of the list     *
 * for DisArm.                                                             *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#include "Trailer.h"

using namespace MICE;
extern bool EPICSClientsConnected;
extern DATEStatusClient *lDATEStatusClient;

void ArmTrailer( char * parPtr ) {

  // Trailer is the last to arm
  // Set the DAQ Alive
  vmeStatus = CAENVME_SetOutputRegister( V2718_Handle, cvOut0Bit );

  if( vmeStatus != cvSuccess ){
    DO_ERROR {
      sprintf( message, "Error Arming Trailer: %d", vmeStatus );
      ERROR( message );
    }
    readList_error = VME_ERROR;
  }

  if(EPICSClientsConnected)
    lDATEStatusClient->Write("ARMED");

  DO_INFO {
    sprintf( message, "End of Arming Trailer" );
    INFO( message );
  }
}

void DisArmTrailer( char * parPtr ) {
  // This Equipment is the first to be disarmed
  // Clear the DAQ alive
  vmeStatus = CAENVME_ClearOutputRegister( V2718_Handle, cvOut0Bit );
  if( vmeStatus != cvSuccess ){
    DO_ERROR{
      sprintf( message, "Error disarming Trailer. Error # %d", vmeStatus );
      ERROR( message );
    }
  } else DO_INFO {
    sprintf( message, "End of Disarming Trailer" );
    INFO( message );
  }

  if(EPICSClientsConnected)
    lDATEStatusClient->Write("RUN_COMPLETE");
}

void AsynchReadTrailer( char * parPtr ){}

int  ReadEventTrailer( char * parPtr, struct eventHeaderStruct * header_ptr,
                                    struct equipmentHeaderStruct * eq_header_ptrs, datePointer * data_ptr ) {
  // This ReadEvent is always called at the end of the equipment List
  int dataStored = 0;
  short data = 0;
  DO_DETAILED {
    sprintf( message, "Entering ReadEventTrailer on %s", 
             getenv("DATE_ROLE_NAME") );
    INFO(message);
  }
  switch( triggerReceiverBoard ) {
  case TR_V977 :

    switch (header_ptr->eventType) {
    case START_OF_BURST:
      // Nothing to do here
      // input mask set by the trigger receiver is still valid
      // the SOS busy will be reset by the EOS
      break;
     case PHYSICS_EVENT:
      // mask the busy for PHYS events
      addr = V977_BaseAddress +  V977_OUTPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      data |= TR_PHYSICS_EVENT_BUSY;
      vmeStatus = VME_WRITE_16( addr, data );

      break;
    case END_OF_BURST: 
      // no break here
    case CALIBRATION_EVENT:
      // unmask inputs for CALIBRATION events
      addr = V977_BaseAddress +  V977_INPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      data &= ~(TR_CALIBRATION_EVENT_REQUEST);
      vmeStatus |= VME_WRITE_16( addr, data );
      // reset the busy: clear all outputs but the one set by software
      addr =  V977_BaseAddress + V977_CLEAR_OUTPUT;
      data =0;
      vmeStatus = VME_WRITE_16( addr, data );
      // Reset the output mask
      addr =  V977_BaseAddress + V977_OUTPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      data &= ~TR_TRIGGER_MASK;
      vmeStatus = VME_WRITE_16( addr, data );
     break;
   default:
      DO_ERROR {
        sprintf( message, "Unexpected event Type. Fatal !");
        ERROR(message);
      }
      readList_error = GENERIC_ERROR;
      break;
    }
    break;
  case TR_V2718 :
    //  reset the busy
    vmeStatus = CAENVME_ClearOutputRegister( V2718_Handle, cvOut4Bit );
    break;
  default :
    DO_ERROR {
      sprintf( message, "Unknown Trigger Receiver type in ReadEventTrailer");
      ERROR( message );
    }
    readList_error = GENERIC_ERROR;
    break;
  }

  return dataStored;
}

int  EventArrivedTrailer( char * parPtr ) { 
  return 0;
}

void PauseArmTrailer( char *) {}

void PauseDisArmTrailer( char *) {}
