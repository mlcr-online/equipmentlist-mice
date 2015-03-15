#include "V1290.h"

#define MICROCONTROLER_TIMEOUT 10000

using namespace std;

using namespace MICE;
extern  DATEStatusClient *lDATEStatusClient;
extern bool EPICSClientsConnected;

extern int maxNevents;

int MicroPollingLoop(unsigned long ad, unsigned short mask){
  int timeOut=MICROCONTROLER_TIMEOUT;
  short data;
  do {
    usleep(10000);
    timeOut--;
    vmeStatus = VME_READ_16( ad , &data );
    if (vmeStatus != cvSuccess) return vmeStatus;
  } while ( ( !(data & mask)) && timeOut && (vmeStatus==cvSuccess) );
  if ( timeOut == 0 ) {
    sprintf( message, "In MicroPollingLoop(): Handshaking with Controler timed out" );
    ERROR( message );
    return -1;
  }
  return 0;
}

int OPCodeCycle(unsigned short mask, unsigned long ba, short OPCode, unsigned int nw, short* microData){
  short data;
  unsigned int iw;
  vmeStatus = VME_READ_16( ba + V1290_MICROHANDSHAKE , &data );
  if (vmeStatus != cvSuccess) return vmeStatus;
  // Check for pending operation
  if ( (data & V1290_MicroRWMask) !=  V1290_MicroWrite ) {
  // There is a pending Operation
    sprintf( message, "Error in OPCodeCycle(): The Micro Controler is busy with a pending IO operation (mask = %X)", mask );
    ERROR( message );
    return cvGenericError;
  }
  // Send the OPCode
  addr = ba + V1290_MICRO;
  data = OPCode;
  vmeStatus = VME_WRITE_16( addr, data );
  if (vmeStatus != cvSuccess) return vmeStatus;
  // loop over the words to be written
  for ( iw = 0 ; iw < nw ; iw++ ){
     // poll the handshake Write OK
    //  Don't allow more than timeOut cycles
    if ( MicroPollingLoop( ba + V1290_MICROHANDSHAKE, mask) ){
      sprintf( message, "Error in OPCodeCycle(): for word %d / %d",iw, nw );
      ERROR( message );
      return cvCommError;
    }
    addr = ba + V1290_MICRO;
    switch (mask) {
    case V1290_MicroWrite:
      vmeStatus = VME_WRITE_16( addr, microData[iw] ); break;
    case V1290_MicroRead:
      vmeStatus = VME_READ_16( addr, &microData[iw] ); break;
    default:
      sprintf( message, "Error in OPCodeCycle(): Wrong parameter" );
      ERROR( message );
      return cvGenericError;
      break;
    }
    if (vmeStatus != cvSuccess) return vmeStatus;
  }
  // poll the handshake until it's back to normal
  if ( MicroPollingLoop( ba + V1290_MICROHANDSHAKE, V1290_MicroWrite  ) ) return cvCommError;

  return cvSuccess;

}

int OPCodeWrite( unsigned long ba, short OPCode, unsigned int nw, short* microData){
  short data;
  unsigned int iw;
  vmeStatus = VME_READ_16( ba + V1290_MICROHANDSHAKE , &data );
  if (vmeStatus != cvSuccess) return vmeStatus;
  // Check for pending operation
  if ( (data & V1290_MicroRWMask) !=  V1290_MicroWrite ) {
  // There is a pending Operation
    sprintf( message, "Error in OPCodeWrite(): The Micro Controler is busy with a pending IO operation." );
    ERROR( message );
    return cvGenericError;
  }
  // Send the OPCode
  addr = ba + V1290_MICRO;
  data = OPCode;
  vmeStatus = VME_WRITE_16( addr, data );
  if (vmeStatus != cvSuccess) return vmeStatus;
  // loop over the words to be written
  for ( iw = 0 ; iw < nw ; iw++ ){
     // poll the handshake Write OK
    //  Don't allow more than timeOut cycles 
    if ( MicroPollingLoop( ba + V1290_MICROHANDSHAKE, V1290_MicroWrite) ){
      sprintf( message, "Error in OPCodeWrite(): for word %d / %d",iw, nw );
      ERROR( message );
      return cvCommError;
    }
    addr = ba + V1290_MICRO;
    data = microData[iw];
    vmeStatus = VME_WRITE_16( addr, data );
    if (vmeStatus != cvSuccess) return vmeStatus;
  }
  // poll the handshake until it's back to normal
  if ( MicroPollingLoop( ba + V1290_MICROHANDSHAKE, V1290_MicroWrite  ) ) return cvCommError;

  return cvSuccess;

}

int OPCodeRead( unsigned long ba, short OPCode, unsigned int nw, short* microData){
  short data;
  unsigned int iw;
  vmeStatus = VME_READ_16( ba + V1290_MICROHANDSHAKE , &data );
  if (vmeStatus != cvSuccess) return vmeStatus;
  // Check for pending operation.
  if ( (data & V1290_MicroRWMask) !=  V1290_MicroWrite ) {
  // There is a pending Operation.
    sprintf( message, "Error in OPCodeRead(): The Micro Controler is busy with a pending IO operation." );
    ERROR( message );
    return cvGenericError;
  }
  // Send the OPCode.
  addr = ba + V1290_MICRO;
  data = OPCode;
  vmeStatus = VME_WRITE_16( addr, data );
  if (vmeStatus != cvSuccess) return vmeStatus;
  // loop over the words to be written.
  for ( iw = 0 ; iw < nw ; iw++ ){
     // poll the handshake Read OK.
    //  Don't allow more than timeOut cycles.
    if ( MicroPollingLoop( ba + V1290_MICROHANDSHAKE, V1290_MicroRead) ){
      sprintf( message, "Error in OPCodeRead(): for word %d / %d",iw, nw );
      ERROR( message );
      return cvCommError;
    }
    addr = ba + V1290_MICRO;
    vmeStatus = VME_READ_16( addr, &microData[iw] );
    if (vmeStatus != cvSuccess) return vmeStatus;
  }
  // Poll the handshake until it's back to normal.
  if ( MicroPollingLoop( ba + V1290_MICROHANDSHAKE,  V1290_MicroWrite) ) return cvCommError;

  return cvSuccess;

}


void ArmV1290 ( char *parPtr ) 
{
  V1290_ParType* V1290 = ( V1290_ParType *) parPtr;
  unsigned long ba = getBA( V1290->BaseAddress );

  int newConfig = 0;
  short opt;
  //unsigned long addr;
  short data;
  short triggerConf[5];

  DO_INFO {
    sprintf( message, "Arming V1290 with following parameters:" );
    INFO( message );
    sprintf( message, "Win.Width: %d,Win.Offset: %d, Sw.Margin: %d, Rej.Margin: %d" , 
            *V1290->WinWidth, *V1290->WinOffset, *V1290->SwMargin, *V1290->RejMargin  );
    INFO( message );
    sprintf( message, "Base Addr.: %s, Geo: %d, Tr.Edge: %d, Trg.Mode: %d",
             V1290->BaseAddress, *V1290->GEO, *V1290->DetectionEdge, *V1290->TriggerMode );
    INFO( message );
    sprintf( message, "Module Type %s", V1290->ModuleType );
    INFO( message );
    sprintf( message, "Channel Config = %s (not yet implemented)", V1290->ChannelConfig );
    INFO( message );
    sprintf( message, "This process will take some time." );
    INFO( message ); 
  }

  // Module Clear
  addr = ba + V1290_SOFTWARECLEAR ;
  vmeStatus = VME_WRITE_16( addr, data );

  // enable all channel
  vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_EN_ALL_CH, 0, NULL );

  if ( decodeChar( V1290->Init ) > 0 ) {
    // Module Reset
    addr = ba + V1290_MODULERESET;
    vmeStatus = VME_WRITE_16( addr, data );
    sleep( 1 );

    // Check Acq Mode
    if( vmeStatus == cvSuccess ) {
      if ( OPCodeCycle(V1290_MicroRead, ba, V1290_READ_ACQ_MODE , 1, &data ) == cvSuccess ){
        if (*V1290->TriggerMode != (data & 0x1) ) {
          sprintf( message, "Changing Trigger mode to %d (was %d)" ,*V1290->TriggerMode , data & 0x1 );
          INFO( message );
          switch (*V1290->TriggerMode){
          case 0 :
            vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_CONT_STOR, 0, NULL );
            break;
          case 1 :
            vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_TRG_MATCH, 0, NULL );
            break;
          default :
            readList_error = PARAM_ERROR;
            return;
            break;
          }
          newConfig = 1;
        }
      }
    }
    // Check trigger configuration
    if( vmeStatus == cvSuccess ) {
      vmeStatus = OPCodeCycle(V1290_MicroRead, ba, V1290_READ_TRG_CONF, 5, triggerConf );
      //Set window width if it is different then the value in DB
      if ( (vmeStatus == cvSuccess) && (triggerConf[0] != *V1290->WinWidth)) {
        sprintf( message, "Changing Window Width to %d (was %d)" , *V1290->WinWidth, triggerConf[0] );
        INFO( message );
        vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_WIN_WIDTH , 1, (short*)V1290->WinWidth );
        newConfig = 1;
      }
      //Set WINDOW OFFSET if it is different then the value in DB
      if ( (vmeStatus == cvSuccess) && (triggerConf[1] != *V1290->WinOffset)) {
        sprintf( message, "Changing Window Offset to %d (was %d)" , *V1290->WinOffset, triggerConf[1] );
        INFO( message );
        vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_WIN_OFFS , 1, (short*)V1290->WinOffset );
        newConfig = 1;
      }
      //SwMargin
      if ( (vmeStatus == cvSuccess) && (triggerConf[2] != *V1290->SwMargin)) {
        sprintf( message, "Changing Sw Margin to %d (was %d)" , *V1290->SwMargin, triggerConf[2] );
        INFO( message );
        vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_SW_MARGIN , 1, (short*)V1290->SwMargin );
        newConfig = 1;
      }
      //RejMargin
      if ( (vmeStatus == cvSuccess) && (triggerConf[3] != *V1290->RejMargin)) {
        sprintf( message, "Changing Rejection Margin to %d (was %d)" , *V1290->RejMargin, triggerConf[3] );
        INFO( message );
        vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_REJ_MARGIN , 1, (short*)V1290->RejMargin );
        newConfig = 1;
      }
      opt = decodeChar( V1290->SubstTrigger );
      if ( (vmeStatus == cvSuccess) &&  ( opt > -1 ) && ( (triggerConf[3] & 0x1) != opt ) ){
        data =  ( opt ) ? V1290_EN_SUB_TRG : V1290_DIS_SUB_TRG  ;
        vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, data, 0, NULL );
        newConfig = 1;
      }
    }
    // Set leading and trailing edge measurement
    if( vmeStatus == cvSuccess ) {
      vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_DETECTION, 1, V1290->DetectionEdge );
    }
    //Set TDC Header
    opt = decodeChar( V1290->TDCHeader );
    if( ( vmeStatus == cvSuccess ) && ( opt > -1 ) ) {
      data =  ( opt ) ? V1290_EN_HEAD : V1290_DIS_HEAD; // 0x3000 ? 0x3100
      vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, data, 0, NULL );
    }
    //Set LSB code
    if( vmeStatus == cvSuccess ) {
      data = decodeBinStr( V1290->LSBCode );
      sprintf( message, "DecodeBinStr : %s =  %d",V1290->LSBCode, data );
      INFO( message );
      vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_TR_LEAD_LSB, 1, &data );
    }
    //Set Deadtime code
    if( vmeStatus == cvSuccess ) {
      data = decodeBinStr( V1290->DeadtimeCode );
      vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SET_DEAD_TIME, 1, &data );
    }
    if (newConfig) {
      DO_INFO {
        sprintf( message, "Configuration has been changed. Please check" );
        INFO( message );
      }
      //Read Trigger Config
      vmeStatus = OPCodeCycle(V1290_MicroRead, ba, V1290_READ_TRG_CONF, 5, triggerConf );
      if( vmeStatus == cvSuccess ) {
        sprintf( message, "Window Width = %d", triggerConf[0] );
        INFO( message );
        sprintf( message, "Window Offset = %d", triggerConf[1] );
        INFO( message );
        sprintf( message, "Extra Margin = %d", triggerConf[2] );
        INFO( message );
        sprintf( message, "Reject Margin = %d", triggerConf[3] );
        INFO( message );
        sprintf( message, "Trigger Time Substraction = %d", 0x0001 & triggerConf[4] );
        INFO( message );
      }
      // save user config
      vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, V1290_SAVE_USER_CONFIG , 0, NULL );
    } else { // newConfig
      sprintf( message, "The TDC configuration has not changed. Consider skipping it." );
      INFO( message );
    }
  } else  {    //if( ...Init > 0 )...
    sprintf( message, "Module Reset and Reconfiguration skipped" );
    INFO( message );
  }
  DO_INFO {
    sprintf( message, "Please relax. Almost done." );
    INFO( message );
  }

  if( vmeStatus == cvSuccess ) {
    // Set auto load user configuration
    vmeStatus = OPCodeCycle(V1290_MicroWrite, ba,V1290_AUTOLOAD_USER_CONFIG , 0, NULL );
  }

  // Enable Event FIFO
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1290_CONTROL;//0x102E
    vmeStatus = VME_READ_16(addr,&data);
    data |= V1290_ControlEventFifo;
    data |= V1290_ControlTriggerTimeTag;
    vmeStatus = VME_WRITE_16( addr, data );
  }

  //Set GEO address
  if( vmeStatus == cvSuccess ) {
    addr = ba + V1290_GEO_REGISTER;//0x100E
    data = *V1290->GEO;
    vmeStatus = VME_WRITE_16( addr, data );
  }

  ////////////////////////////////////////////////////////////
  // Temporary tmp Disable faulty channels for TDC01
  /*
    if (*V1290->GEO == 1) {
    sprintf( message, "WARNING: Disabling Channel 8-15 of TDC01." );
    ERROR( message ); 
    int disCh;
    for ( disCh=8; disCh < 16 ; disCh++ ){
      data = V1290_DIS_CHANNEL + disCh;
      vmeStatus = OPCodeCycle(V1290_MicroWrite, ba, data, 0, NULL );
    }
  }
  */
  ///////////////////////////////////////////////////////////

  if( vmeStatus != cvSuccess ) {
    sprintf( message, "Error during V1290 initialization" );
    ERROR( message ); 
    readList_error = VME_ERROR;
  }
}

void DisArmV1290( char *parPtr) {
  V1290_ParType* V1290 = ( V1290_ParType *) parPtr;
  unsigned long ba = getBA( V1290->BaseAddress ) ;
  // Checks if there is data
  int addr = ba + V1290_STATUS;
  int status;
  vmeStatus = VME_READ_16(addr,&status);
  if ( vmeStatus != cvSuccess ){
    readList_error = VME_ERROR;
    return;
  }
  if( !!( status & 0x0001 ) ) {
    DO_INFO{
      sprintf( message, "V1290 still contains data! " );
      INFO( message );
    }
  }
  DO_INFO {
    sprintf( message, "V1290 disarmed" );
    INFO( message );
  }
}

void AsynchReadV1290( char *parPtr) {}

int ReadEventV1290( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) 
{
  int dataStored = 0;
  V1290_ParType* V1290 = ( V1290_ParType *) parPtr;
  unsigned long ba = getBA( V1290->BaseAddress );
  short fifoWordCount = 0;
  short fifoEventCount = 0;
  short nEvt = 0;
  short current_event;
  short status; 
  int data; //
  int nDataWord = 0;
  int nWordLeft = 0;
  int nbread;
  int npages;
  int ipage;
  int    iMis       = 0;


  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT ){
    V1290ModuleType moduleType = V1290_UNKNOWN_MODULE ;
    if ( strcmp(V1290->ModuleType, "V1290N") == 0 ) {
      moduleType = V1290_V1290N;
    } else if ( strcmp(V1290->ModuleType, "V1290A") == 0 ) {
      moduleType = V1290_V1290A;
    } else {
      sprintf( message, "ModuleType %s not yet implemented", V1290->ModuleType );
      ERROR( message );
      readList_error = PARAM_ERROR;
    }

    // Setting equipment header values
    eq_header_ptr->equipmentId = (int)*V1290->GEO;
    eq_header_ptr->equipmentBasicElementSize = 4;

    //Checks if there is some data ready
    addr = ba + V1290_STATUS;
    vmeStatus = VME_READ_16(addr,&status);
    if ( vmeStatus != cvSuccess ){
      DO_ERROR{
        sprintf( message, "Error: Cannot read V1290 Status Register Error # %d", vmeStatus );
        // from now on, we stop checking vmeStatus
        ERROR( message );
      }
      readList_error = VME_ERROR;
      return 0;
    }

    if( status & (V1290_StatusFull | V1290_StatusTriggerLost) ) {
      DO_ERROR{
        sprintf( message, "Warning: TDC is full and/or Trigger has been lost: Status =  0x%x", status );
        ERROR( message );
        ERROR ( "Clearing the board" );
        addr = ba + V1290_SOFTWARECLEAR;
        vmeStatus = VME_WRITE_16( addr, data );
        return 0;
      }
    }

    if( status & V1290_StatusDataReady ) {
      //Get the number of events
      addr = ba + V1290_EVENTFIFOSTORED;
      vmeStatus = VME_READ_16(addr,&data);

      nEvt = data;
      if ( nEvt> maxNevents) {
        ERROR("Error in ReadEventV1290: Too many triggers; Ignoring the spill.");
        addr = ba + V1290_SOFTWARECLEAR;
        vmeStatus = VME_WRITE_16( addr, data );
        nEvt = 0;
        return 0;
      }
      eq_header_ptr->equipmentTypeAttribute[0]=nEvt;

      // all modules of the same ldc should have the same number of particle events
      // The first user attribute is used to store the number of particle triggers
     iMis=checkPartTriggerCount(header_ptr,nEvt);
     if ( iMis ) {
       INFO( " WARNING from ReadEventV1290: Automatic run stop in case of Mismatch is  enabled for mismatch > 1 " );
       if (iMis>1) {	
         sprintf( message, "FATAL erroe in V1290 board %d : Mismatch = %d",(int)*V1290->GEO, iMis );
         FATAL( message );
        readList_error = SYNC_ERROR;

        if(EPICSClientsConnected)
          lDATEStatusClient->Write("ERROR");

         return 0;
       }
     }

     // DO_ERROR {
     DO_DETAILED {
       sprintf( message, "ReadEventV1290: Number of Events in FIFO = %d", nEvt );
       INFO( message );
     }
      addr = ba + V1290_EVENTFIFO;
      // compute the total number of events
      for( current_event = 0; current_event < nEvt; ++current_event ){
        vmeStatus = VME_READ_32( addr, &data );
        fifoWordCount = ( short )( data & 0x0000FFFF );
        fifoEventCount = ( short )( ( data >> 16 ) & 0x0000FFFF );
        nDataWord += fifoWordCount;
      }

      nbread = 0;
      npages = nDataWord / ( V1290_PAGECAPACITY );

      if ( nDataWord ) {
        for ( ipage = 0 ; ipage < npages ; ipage++ ) {
          vmeStatus = VME_READ_BLT_32(ba, data_ptr, V1290_PAGESIZE, &nbread);
          dataStored += nbread;
          data_ptr   += nbread/V1290_READOUTWORDSIZE;
        }

        nWordLeft = nDataWord % (V1290_PAGESIZE/V1290_READOUTWORDSIZE);
        if ( nWordLeft ) {
          vmeStatus = VME_READ_BLT_32(ba, data_ptr, nWordLeft * V1290_READOUTWORDSIZE, &nbread);
          dataStored += nbread;
        }
      }
    }

    addr = ba + V1290_STATUS;
    vmeStatus = VME_READ_16(addr,&status);
    if(  !(status & V1290_StatusPurged)  ){
      DO_ERROR{
        sprintf( message, "V1290 still contains data after readout! " );
        ERROR( message );
        //readList_error = SYNC_ERROR;
      }
    }
    DO_DETAILED{
      sprintf( message, "V1290 Data stored: %d", dataStored );
      INFO( message );
    }
    return dataStored;
  } else {
    return 0;
  }
}

int EventArrivedV1290( char *parPtr ) {
    return 0;
}

void PauseArmV1290( char *) {}

void PauseDisArmV1290( char *) {}


/* The only reason why we want V1290N as a separate equipment is because
we need a different equipment Type because the decoding is slightly different from
V1290
*/

void ArmV1290N( char *parPtr) {
  ArmV1290(parPtr);
}
void DisArmV1290N( char * parPtr){
  DisArmV1290(parPtr);
}
void AsynchReadV1290N( char * parPtr){}

int ReadEventV1290N( char *parPtr, struct eventHeaderStruct *header_ptr,
                     struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  return ReadEventV1290(parPtr, header_ptr, eq_header_ptr, data_ptr);
}

int EventArrivedV1290N( char * parPtr){
  return 0;
}

void PauseArmV1290N( char *) {}

void PauseDisArmV1290N( char *) {}
