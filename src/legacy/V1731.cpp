/***************************************************************************
 *                                                                         *
 * $Log: V1731.c,v $
 *                                                                         *
 * equipmentList functions for the CAEN fADC V1731                         *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov July 2007          *
 *                                                                         *
 ***************************************************************************/
#include "V1731.h"

using namespace std;

using namespace MICE;
extern  DATEStatusClient *lDATEStatusClient;
extern bool EPICSClientsConnected;

extern int maxNevents;

void ArmV1731 ( char * parPtr ) {
  int data;
  V1731_ParType * V1731 = ( V1731_ParType *) parPtr;
  unsigned long ba = getBA( V1731->BaseAddress ); 
  DO_INFO {
    sprintf( message, "Arming V1731 with following parameters: BA = %lx", ba );
    INFO( message );
    sprintf( message, "Buffer size (not implemented): = %d",  *V1731->BufferSize );
    INFO( message );
  }
  data=0;

  //Reset the board
  addr = ba + V1731_SW_RESET;
  vmeStatus = VME_WRITE_32( addr, data );
  if ( vmeStatus != cvSuccess ) {
   readList_error = VME_ERROR;
    DO_ERROR {
      sprintf( message, "The board failed to reset. Exiting with error." );
      ERROR( message );
      return ;
    }
  }
  // Poll ACQ Status Register until the board is ready
  data=0;
  addr = ba +  V1731_ACQ_STATUS;
  int ploop=0;
  while ( (vmeStatus == cvSuccess) && !(data &  V1731_BoardReady) ) {
    INFO("Waiting for the board to be ready after reset...");
    usleep(200000);
    vmeStatus = VME_READ_32( addr, &data );
    ploop++;
    if (ploop > 10) {
      sprintf( message, "The board failed to reset. Timeout error." );
      FATAL( message );
      vmeStatus=cvGenericError;
      readList_error = VME_ERROR;
      return ;
    }
  }

  //Calibration
  for (int ich=0; ich<V1731_CHANNELS_NUMBER ; ich++){
    addr = ba + V1731_CHANNEL_ADC_CONFIG + 0x0100*ich;
    data = V1731_ADCCalibration;
    vmeStatus = VME_WRITE_32( addr, data );
    data = 0;
    vmeStatus = VME_WRITE_32( addr, data );
  }
  for (int ich=0; ich<V1731_CHANNELS_NUMBER ; ich++){
    addr = ba + V1731_CHANNEL_STATUS + 0x0100*ich;
    data = 0;
    int iteration=0;
    do {
      iteration++;
      vmeStatus = VME_READ_32( addr, &data );
      if ( vmeStatus!=cvSuccess || iteration>10000) {
        data= 0xFFFF;
        sprintf( message, "V1731: Error during calibration of channel %d",ich );
        ERROR( message );
      }
    } while ( !(data & V1731_CalibrationDone) );
    sprintf( message, "V1731: calibration of channel %d done.",ich );
    INFO( message );
  }

  //VME control
  addr = ba + V1731_VME_CONTROL;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "VME Control = %X", data );
  INFO( message );
  // Enable bus error emission during BLT cycle
  data |= V1731_BERR;
  vmeStatus |= VME_WRITE_32( addr, data );
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "VME Control after setting = %X", data );
  INFO( message );

  //Set BLT Event Number 
  addr = ba + V1731_BLT_EVENT_NUMBER;
  //TODO Should be a parameter
  data = 0xff;
  vmeStatus |= VME_WRITE_32( addr, data );

  //Acquisition control
  addr = ba + V1731_ACQ_CONTROL;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "ACQ Control = %X", data );
  INFO( message );
  data = V1731_AcquisitonRun | V1731_CountAllTriggers;
  vmeStatus |= VME_WRITE_32( addr, data );
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "ACQ Control after Setting = %X", data );
  INFO( message );
  //Acquisition status
  addr = ba + V1731_ACQ_STATUS;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "ACQ Status = %X", data );
  INFO( message );

  //Set GEO address     
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_BOARD_ID;
    data = *V1731->GEO;
    vmeStatus = VME_WRITE_32( addr, data );
  }
  if ( vmeStatus != cvSuccess ) {
    sprintf( message, "Can not set Geo Address to %d", data );
    ERROR( message);
  }

  //Enable Channels
  addr = ba + V1731_CHANNEL_ENABLE;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "Channel Mask (not implemented): 0x%X (%d)", data, data );
  INFO( message );
  //TODO implement channel mask
  data = 0xFF; 
  //Enable all channels
  vmeStatus |= VME_WRITE_32( addr, data );

  //Set the post trigger offset by [data] samples
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_POST_TRIGGER_SETTING;
    data = *V1731->PostTriggerOffset;
    vmeStatus |= VME_WRITE_32( addr, data );
  }

  //Set Output Buffer Size 
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_BUFFER_ORGANIZATION;
    data = V1731_Buffer_Number_1024;
    vmeStatus |= VME_WRITE_32( addr, data ); 
  }

  //Set custom number of sample words per events
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_BUFFER_CUSTOM_SIZE;
    // temporary: should be a parameter TODO 
    data = 32;
    vmeStatus = VME_WRITE_32( addr, data ); 
  }

  //Set channels configuration
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_CHANNEL_CONFIG_BIT_CLEAR;//check this
    data = 0xFF; 
    vmeStatus = VME_WRITE_32( addr, data ); 
  }
  addr = ba + V1731_CHANNEL_CONFIG;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "CHANNEL CONFIG = %X", data );
  INFO( message );
  // Enable overlapping triggers
  // temporary: should be a parameter TODO 
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_CHANNEL_CONFIG_BIT_SET;
    data = V1731_TriggerOverlapping | V1731_MemorySequentialAccess;
    vmeStatus = VME_WRITE_32( addr, data ); 
  }
  addr = ba + V1731_CHANNEL_CONFIG;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "CHANNEL CONFIG after setting = %X", data );
  INFO( message );

  //Set Trigger Source
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_TRIGGER_SOURCE_ENABLE_MASK;
    data = V1731_TriggerSourceExternal;
    vmeStatus |= VME_WRITE_32( addr, data ); 
  }

  //Print some information for debug purpose  
  if( vmeStatus == cvSuccess ) {
    DO_INFO {
      sprintf( message, "Arming V1731 successful." );
      INFO( message );
    }
  } else {
    readList_error = VME_ERROR;
    DO_ERROR {
      sprintf( message, "Arming V1731 failed" );
      ERROR( message );
    }
  }
}

void DisArmV1731 ( char * parPtr ) {
  int data;
  //Set ACQ Run Stop
  V1731_ParType * V1731 = ( V1731_ParType *) parPtr;
  unsigned long ba = getBA( V1731->BaseAddress ); 
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1731_ACQ_CONTROL;
    data = V1731_CountAllTriggers; 
    vmeStatus = VME_WRITE_32( addr, data ); 
  }

  DO_INFO {
      sprintf( message, "Disarming V1731 successful." );
      INFO( message );
    }
}

void AsynchReadV1731( char *parPtr) {}

int ReadEventV1731( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) 
{
  int data;
  V1731_ParType * V1731 = ( V1731_ParType *) parPtr;
  unsigned long ba = getBA( V1731->BaseAddress ); 
  DO_DETAILED {
    sprintf( message, "Entering ReadEventV1731 on %s", 
             getenv("DATE_ROLE_NAME") );
    INFO(message);
  }

  long32 nEvt       = 0;
  int    nbRead     = 0;
  int    nbStored   = 0;
  int    iMis       = 0;

  data = 0;
  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT ){

    // Setting equipment header values
    eq_header_ptr->equipmentBasicElementSize = 4;
    eq_header_ptr->equipmentId = *V1731->GEO;
    DO_DETAILED {
      sprintf( message, "Entering ReadEventV1731 for board %d",  *V1731->GEO );
      INFO( message );
    }

    addr = ba + V1731_ACQ_STATUS;
    vmeStatus = VME_READ_32( addr, &data );
    if ( data & V1731_EventFull ) {
      DO_ERROR {
        sprintf( message, "Buffer is  Full (GEO: %d)", *V1731->GEO);
        ERROR( message );
      }
    }
    if ( data & V1731_EventReady ) {
      // Get the number of events
      addr = ba + V1731_EVENT_STORED;
      vmeStatus = VME_READ_32( addr, &nEvt );
      // DO_ERROR {
      DO_DETAILED {
        sprintf( message, "V1731 number of events to read (GEO: %d): %d", 
                 *V1731->GEO, nEvt );
        INFO( message );
      }
      if ( nEvt> maxNevents)	   {
        ERROR("Error in ReadEventV1731: Too many triggers; Ignoring the spill.");
        addr = ba + V1731_SW_CLEAR;
        data=1;
        vmeStatus = VME_WRITE_32( addr, data );
        nEvt = 0;
        return 0;
      }

      eq_header_ptr->equipmentTypeAttribute[0]=nEvt;

      // all modules of the same ldc should have the same number of particle events
      // The first user attribute is used to store the number of particle triggers
      iMis = checkPartTriggerCount(header_ptr,nEvt);  
      if ( iMis ) {
        sprintf( message,
                 "WARNING from ReadEventV1731(Geo# %d): Automatic run stop in case of Mismatch is enabled for mismatch > 1 ",
                 *V1731->GEO );
        INFO( message );
        if (iMis>1) {
          sprintf( message, "FATAL: Mismatch = %d", iMis );
          FATAL( message );
          readList_error = SYNC_ERROR;

          if(EPICSClientsConnected)
            lDATEStatusClient->Write("ERROR");

          return 0;
        }
      }

      addr = ba + V1731_EVENT_READOUT_BUFFER;
      int nBLTCycle=0;
      int iloop;
      for ( iloop = 0 ; iloop <= nEvt/256 ; iloop++) {
        while(  vmeStatus != cvBusError  ){
          vmeStatus = VME_READ_BLT_32( addr, data_ptr, V1731_READOUT_BUFFER_SIZE, &nbRead );
          nbStored += nbRead;
          data_ptr += nbRead/4; //Move the data pointer by the number of words read
          DO_DETAILED {
            sprintf( message, "BLT cycle number %d (GEO: %d)", 
                     ++nBLTCycle, *V1731->GEO );
            INFO( message );
          }
        }
        vmeStatus = cvSuccess;
      }
    }

    // Temporary: Checking that the memory is empty
    for (int ich=0 ; ich<V1731_CHANNELS_NUMBER ; ich++) {
      addr = ba + V1731_CHANNEL_STATUS + 0x0100*ich;
      vmeStatus = VME_READ_32( addr, &data );
      if ( !(data & V1731_MemoryEmpty) ) {
        DO_ERROR {
          sprintf( message, "Channel %d still contains data after readout (GEO: %d)",ich, *V1731->GEO);
          ERROR( message );
        }
      }
    }

    //Clear memory
    addr = ba + V1731_SW_CLEAR;
    data = 1;
    vmeStatus = VME_WRITE_32( addr, data ); 
    DO_DETAILED {
      sprintf( message, "V1731 Data stored: %d", nbStored );
      INFO( message );
    }
    return nbStored;
  } else {
    return 0;
  }
}

int EventArrivedV1731( char *parPtr ) {
    return 0;
}

void PauseArmV1731( char *) {}

void PauseDisArmV1731( char *) {}
