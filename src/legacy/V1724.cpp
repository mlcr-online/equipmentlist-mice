/***************************************************************************
 *                                                                         *
 * $Log: V1724.c,v $
 *                                                                         *
 * equipmentList functions for the CAEN fADC V1724                         *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov July 2007          *
 *                                                                         *
 ***************************************************************************/
#include "V1724.h"

using namespace std;

using namespace MICE;
extern  DATEStatusClient *lDATEStatusClient;
extern bool EPICSClientsConnected;

extern int maxNevents;

int buffer_size;

void ArmV1724 ( char * parPtr ) {
  int data;
  V1724_ParType * V1724 = ( V1724_ParType *) parPtr;
  unsigned long ba = getBA( V1724->BaseAddress ); 
  DO_INFO {
    sprintf( message, "Arming V1724 with following parameters: BA = %lx", ba );
    INFO( message );
    sprintf( message, "Buffer size (not implemented): = %d",  *V1724->BufferSize );
    INFO( message );
  }
  data=0;

  //Reset the board
  addr = ba + V1724_SW_RESET;
  vmeStatus = VME_WRITE_32( addr, data );
  if ( vmeStatus != cvSuccess ) {
   readList_error = VME_ERROR;
    DO_ERROR {
      sprintf( message, "The board failed to reset. Communication error." );
      FATAL( message );
      return ;
    } 
  }
  // Poll ACQ Status Register until the board is ready
  data=0;
  addr = ba +  V1724_ACQ_STATUS;
  int ploop=0;
  while ( (vmeStatus == cvSuccess) && !(data &  V1724_BoardReady) ) {
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
  // Firmware version
  DO_DETAILED {

    data=0;
    addr = ba +  V1724_ROC_FIRMWARE_REVISION;
    vmeStatus = VME_READ_32( addr, &data );
    sprintf( message, "ROC Firmware Revision %d .%d (%d/%d/%d) [0x%x]",
             (data&0xFF00)>>8, (data&0xFF), (data&0xFF0000)>>16,
             (data&0xF000000)>>24, (data&0xF0000000)>>28, data );
    ERROR( message );

    for ( int ich=0; ich<8; ich++){
      data=0;
      addr = ba +  V1724_CHANNEL_FIRMWARE_REVISION + ich*0x0100;
      vmeStatus = VME_READ_32( addr, &data );
      sprintf( message, "Channel %d Firmware Revision %d .%d (%d/%d/%d) [0x%x]",
               ich, (data&0xFF00)>>8,(data&0xFF),(data&0xFF0000)>>16,
               (data&0xF000000)>>24,(data&0xF0000000)>>28, data );
      ERROR( message );
    }
  }

  //VME control
  addr = ba + V1724_VME_CONTROL;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "VME Control = %X", data );
  INFO( message );
  // Enable bus error emission during BLT cycle
  data |= V1724_BERR;
  vmeStatus |= VME_WRITE_32( addr, data );
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "VME Control after setting = %X", data );
  INFO( message );

  //Set BLT Event Number 
  addr = ba + V1724_BLT_EVENT_NUMBER;
  //TODO Should be a parameter
  data = 0xff;
  vmeStatus |= VME_WRITE_32( addr, data );

  //Acquisition control
  addr = ba + V1724_ACQ_CONTROL;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "ACQ Control = %X", data );
  INFO( message );
  data = V1724_AcquisitonRun | V1724_CountAllTriggers;
  vmeStatus |= VME_WRITE_32( addr, data );
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "ACQ Control after Setting = %X", data );
  INFO( message );
  //Acquisition status
  addr = ba + V1724_ACQ_STATUS;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "ACQ Status = %X", data );
  INFO( message );

  //Set GEO address     
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_BOARD_ID;
    data = *V1724->GEO;
    vmeStatus = VME_WRITE_32( addr, data );
  }
  if ( vmeStatus != cvSuccess ) {
    sprintf( message, "Can not set Geo Address to %d", data );
    ERROR( message); 
  }

  //Enable Channels
  addr = ba + V1724_CHANNEL_ENABLE;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "Channel Mask (not implemented): 0x%X (%d)", data, data );
  INFO( message );
  //TODO implement channel mask
  data = 0xFF;
  //Enable all channels
  vmeStatus |= VME_WRITE_32( addr, data );

  //Set the post trigger offset by [data] samples
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_POST_TRIGGER_SETTING;
    data = *V1724->PostTriggerOffset;
    vmeStatus |= VME_WRITE_32( addr, data );
  }

  //Set Output Buffer Size
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_BUFFER_ORGANIZATION;

    data = V1724_OutputBufferSize_1K;
    vmeStatus |= VME_WRITE_32( addr, data ); 
    buffer_size = 1 << (10 - data); //Get the size of the output buffer in Kb 
    sprintf( message, " Output Buffer size = %d (%d kB)", data, buffer_size );
    INFO( message );
  }

  //Set custom number of sample words per events
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_BUFFER_CUSTOM_SIZE;
    // temporary: should be a parameter TODO 
    //   data = 256;
    // Changed on 16.09.2009 data = 128;
   data = 64;
   vmeStatus = VME_WRITE_32( addr, data ); 
  }

  //Set channels configuration
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_CHANNEL_CONFIG_BIT_CLEAR;//check this
    data = 0xFF; 
    vmeStatus = VME_WRITE_32( addr, data ); 
  }
  addr = ba + V1724_CHANNEL_CONFIG;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "CHANNEL CONFIG = %X", data );
  INFO( message );
  // Enable overlapping triggers
  // temporary: should be a parameter TODO
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_CHANNEL_CONFIG_BIT_SET;
    data = V1724_TriggerOverlapping | V1724_MemorySequentialAccess;
    vmeStatus = VME_WRITE_32( addr, data ); 
  }
  addr = ba + V1724_CHANNEL_CONFIG;
  vmeStatus |= VME_READ_32( addr, &data );
  sprintf( message, "CHANNEL CONFIG after setting = %X", data );
  INFO( message );

  //Set channels threshold
  //int i = 0;
  /*
  i = 0;
  while ( ( i  < 1 )  && ( vmeStatus == cvSuccess ) )
  {
    addr = ba + (i * 0x100) + V1724_CHANNEL_THRESHOLD; 
    data = 8600;
    vmeStatus = VME_WRITE_32( addr, data );
    sprintf( message, "Setting threshold to %d for channel %d (addr = %X)", data, i+1, addr );
    INFO( message );
    addr += 4;
    data = 6;
    vmeStatus = VME_WRITE_32( addr, data );
    }
    ++i;
  }
  */

  //Set Trigger Source
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_TRIGGER_SOURCE_ENABLE_MASK;
    data = V1724_TriggerSourceExternal;
    vmeStatus |= VME_WRITE_32( addr, data ); 
  }

  //Print some information for debug purpose
  if( vmeStatus == cvSuccess ) {
    DO_INFO {
      sprintf( message, "Arming V1724 successful." );
      INFO( message );
    }
  } else {
    readList_error = VME_ERROR;
    DO_ERROR {
      sprintf( message, "Arming V1724 failed" );
      ERROR( message );
    }
  }
}

void DisArmV1724 ( char * parPtr ) {
  int data;
  //Set ACQ Run Stop
  V1724_ParType * V1724 = ( V1724_ParType *) parPtr;
  unsigned long ba = getBA( V1724->BaseAddress ); 
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1724_ACQ_CONTROL;
    data = V1724_CountAllTriggers; 
    vmeStatus = VME_WRITE_32( addr, data ); 
  }

  DO_INFO {
      sprintf( message, "Disarming V1724 successful." );
      INFO( message );
    }
}

void AsynchReadV1724( char *parPtr) {}

int ReadEventV1724( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr)  {
  int data;
  V1724_ParType * V1724 = ( V1724_ParType *) parPtr;
  unsigned long ba = getBA( V1724->BaseAddress );
  DO_DETAILED {
    sprintf( message, "Entering ReadEventV1724 on %s",
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
    eq_header_ptr->equipmentId = *V1724->GEO;
    // DO_ERROR {
    DO_DETAILED {
      sprintf( message, "Entering ReadEventV1724 for board %d",  *V1724->GEO );
      INFO( message );
    }

    addr = ba + V1724_ACQ_STATUS;
    vmeStatus = VME_READ_32( addr, &data );

    //      sprintf( message, "Acq status in %d : 0x%x",  *V1724->GEO, data );
    //      INFO( message );

    if ( data & V1724_EventFull ) {
      DO_ERROR {
        sprintf( message, "Buffer is  Full (GEO: %d)", *V1724->GEO);
        ERROR( message );
      }
    }
    if ( data & V1724_EventReady ) {
      // Get the number of events
      addr = ba + V1724_EVENT_STORED;
      vmeStatus = VME_READ_32( addr, &nEvt );
      // DO_ERROR {
      DO_DETAILED {
        sprintf( message, "V1724 number of events to read (GEO: %d): %d", 
                 *V1724->GEO, nEvt );
        INFO( message );
      }
      if ( nEvt> maxNevents)	   {
        ERROR("Error in ReadEventV1724: Too many triggers; Ignoring the spill.");
        addr = ba + V1724_SW_CLEAR;
        data=1;
        vmeStatus = VME_WRITE_32( addr, data );
        nEvt = 0;
        return 0;
      }

      eq_header_ptr->equipmentTypeAttribute[0]=nEvt;

      // all modules of the same ldc should have the same number of particle events
      // The first user attribute is used to store the number of particle triggers      
      iMis = checkPartTriggerCount(header_ptr,nEvt) ;
      if ( iMis ) {
        sprintf( message, 
                 "WARNING from ReadEventV1724(Geo# %d): Automatic run stop in case of Mismatch is enabled for mismatch > 1 ",
                 *V1724->GEO );
        INFO( message );
        if (iMis>1) {
          sprintf( message, "FATALerror in %d : Mismatch = %d",(int)*V1724->GEO, iMis );
          FATAL( message );
          readList_error = SYNC_ERROR;

          if(EPICSClientsConnected)
            lDATEStatusClient->Write("ERROR");

          return 0;
        }
      }

      addr = ba + V1724_EVENT_READOUT_BUFFER;
      int nBLTCycle=0;
      int iloop;
      for ( iloop = 0 ; iloop <= nEvt/256 ; iloop++) {
        while(  vmeStatus != cvBusError  ){
          vmeStatus = VME_READ_BLT_32( addr, data_ptr, V1724_READOUT_BUFFER_SIZE, &nbRead );
          nbStored += nbRead;
          data_ptr += nbRead/4; //Move the data pointer by the number of words read
          DO_DETAILED {
            sprintf( message, "BLT cycle number %d (GEO: %d)", 
                     ++nBLTCycle, *V1724->GEO );
            INFO( message );
          }
        }
        vmeStatus = cvSuccess;
      }
    }
    // Temporary: Checking that the memory is empty
    for (int ich=0 ; ich<V1724_CHANNELS_NUMBER ; ich++) {
      addr = ba + V1724_CHANNEL_STATUS + 0x0100*ich;
      vmeStatus = VME_READ_32( addr, &data );
      if ( !(data & V1724_MemoryEmpty) ) {
        DO_ERROR {
          sprintf( message, "Channel %d still contains data after readout (GEO: %d)",ich, *V1724->GEO);
          ERROR( message );
        }
      }
    }

    //Clear memory
    addr = ba + V1724_SW_CLEAR;
    data = 1;
    vmeStatus = VME_WRITE_32( addr, data ); 
    DO_DETAILED {
      sprintf( message, "V1724 Data stored: %d", nbStored );
      INFO( message );
    }
    return nbStored;
  } else {
    return 0;
  }
}


int EventArrivedV1724( char *parPtr ) {
    return 0;
}

void PauseArmV1724( char *) {}

void PauseDisArmV1724( char *) {}
