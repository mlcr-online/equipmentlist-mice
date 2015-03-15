#include "V830.h"

using namespace std;

using namespace MICE;
extern  DATEStatusClient *lDATEStatusClient;
extern std::vector<PVClient *> gEPICSClients;
extern DATEPVDescription *lDescription;
extern bool EPICSClientsConnected;

unsigned long ba;
static int emptyEventCount(0);
static int overflowCount(0);
int nChannelsSet;
extern int maxNevents;

void ArmV830 ( char * parPtr ) {
  V830_ParType * V830 = ( V830_ParType *) parPtr;
  ba = getBA( V830->BaseAddress ); //0x50000000;
  int data = 0;
  int channels;

  DO_INFO {
    sprintf( message, "Arming V830" );
    INFO( message );
  }
  //Reset module
  addr = ba + V830_MODULE_RESET;
  vmeStatus = VME_WRITE_16( addr, data ); 
  /*
  //Clear module
  if( vmeStatus == cvSuccess ) {
    addr = ba + V830_SOFTWARE_CLEAR; 
    vmeStatus = VME_WRITE_16( addr, data ); 
  }
  */
  addr = ba +  V830_GEO_REGISTER;
  data = (int)*V830->GEO;
  vmeStatus = VME_WRITE_16( addr, data ); 

  //Enable Channels
  if( vmeStatus == cvSuccess ) {
    addr = ba + V830_CHANNEL_ENABLE;
    channels = channelPattern( V830->ChannelConfig );
    nChannelsSet = bitCount( channels );
    vmeStatus = VME_WRITE_32( addr, channels ); 
  }
  vmeStatus = VME_READ_32( addr, &channels );
  sprintf( message, "Enabled Channel Pattern = 0x%x", channels );
  INFO( message );

  // Set trigger mode
  if( vmeStatus == cvSuccess ) {
    int tmp = *V830->TriggerMode;
    addr = ba + V830_CONTROL_BIT_SET;
    vmeStatus = VME_WRITE_16( addr, tmp );
    DO_INFO {
      sprintf( message, "Trigger = %d", tmp );
      INFO( message );
    }
  }

  //Set Header
  if( vmeStatus == cvSuccess ) {
    if ( decodeChar( V830->UseHeader )) {
      addr = ba + V830_CONTROL_BIT_SET;
    } else {
      addr = ba + V830_CONTROL_BIT_CLEAR;
    }
    data = V830_ControlHeaderEnable;
    vmeStatus = VME_WRITE_16( addr, data );
  }

  //Set Data (32 or 26 bits)
  if( vmeStatus == cvSuccess ) {
    if ( decodeChar(V830->UseNarrow) ) {
      addr = ba + V830_CONTROL_BIT_SET;
    } else {
      addr = ba + V830_CONTROL_BIT_CLEAR;
    }
    data = V830_ControlDataFormat;
    vmeStatus = VME_WRITE_16( addr, data );
  }

  //Set AutoReset
  if( vmeStatus == cvSuccess ) {
    if (decodeChar( V830->AutoReset )) {
      addr = ba + V830_CONTROL_BIT_SET;
    } else {
      addr = ba + V830_CONTROL_BIT_CLEAR;
    }
    data = V830_ControlAutoReset;
    vmeStatus = VME_WRITE_16( addr, data );
  }

  //Get the contents of the Control Register
  addr = ba + V830_CONTROL_REGISTER;
  vmeStatus= VME_READ_16( addr, &data );
  if( vmeStatus == cvSuccess ) {
    sprintf( message, "Arming V830 successful. Control Register is 0x%x, nChannelsSet = %d", data, nChannelsSet );
    INFO (message);
  } else {
    sprintf( message, "Communication error while arming V830." );
    INFO (message);

    readList_error = VME_ERROR;
  }
}

void DisArmV830 ( char * parPtr ) {
  DO_INFO {
      sprintf( message, "Disarming V830 successful." );
      INFO( message );
    }
}

void AsynchReadV830( char *parPtr) {}

int ReadEventV830( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  V830_ParType * V830 = ( V830_ParType *) parPtr;
  //unsigned long ba =  getBA( V830->BaseAddress ); //0x50000000; //= *V830->BaseAddress;
  //unsigned long addr;
  short status;
  short nEvt = 0;
  short nWords;
  int data = 0;
  int dataStored = 0;
  int nbread = 0;
  //static int errorFlag(0);
  //static int overflowFlag(0);

  eq_header_ptr->equipmentId = (int)*V830->GEO;
  eq_header_ptr->equipmentBasicElementSize = 4;
  if ( header_ptr->eventType == PHYSICS_EVENT ) {
    //read the status register
    addr = ba + V830_STATUS_REGISTER;
    vmeStatus = VME_READ_16( addr, &status );
    //  sprintf( message, "Starts V830 readout. Status reg = %X", status );
    //  INFO( message );
    //Check if the scaler is not busy
    // Commented out: for some reason the busy is always set !
    //  int cnt = 5;
    //  while( (status & V830_GlobalBusy) && cnt ){
    //    --cnt;
    //    usleep( 1 );	
    //  }

    if( status & V830_DReady ){
      //Get the number of the words per event
      nWords = nChannelsSet + decodeChar( V830->UseHeader );
      //Get the number of the events (usually it is only 1)
      addr = ba + V830_MEB_EVENT_NUMBER;
      vmeStatus = VME_READ_16( addr, &nEvt );
      DO_DETAILED {
        sprintf( message, "Events in V830 MEB: %d, each of size %d words", nEvt, nWords );
        INFO( message );
      }
      if (nEvt != 1) {
        sprintf( message, "More than 1 Event in V830 MEB" );
        ERROR( message );
      }

     //Readout the MEB
      addr = ba + V830_MEB;//Equals BA

      while( nEvt-- ) {
        vmeStatus = VME_READ_BLT_32( addr, data_ptr, nWords * 4, &nbread );
        dataStored += nbread;

        if (nEvt == 0) {
          if ( IsGoodEvent(V830, eq_header_ptr, data_ptr) ) {
            if (EPICSClientsConnected) {
              if (!CallEPICS(V830, data_ptr))
                return 0;
            }
          } else {
            if (overflowCount<2 && emptyEventCount<10) {
              if (EPICSClientsConnected) {
                if (!CallEPICS(V830, data_ptr))
                  return 0;
              }
            }
            else {
              if(EPICSClientsConnected)
                lDATEStatusClient->Write("ERROR");
            }
          }
        }
      }
    }
    DO_DETAILED {
      sprintf( message, "Data stored in V830: %d", dataStored );
      INFO( message );
    }
    // Clear the scalars
    addr = ba + V830_SOFTWARE_CLEAR; 
    vmeStatus = VME_WRITE_16( addr, data ); // whatever data is

    return dataStored;
  } else if ( header_ptr->eventType == START_OF_BURST ) {
    // Clear the scalars
    addr = ba + V830_SOFTWARE_CLEAR;
    vmeStatus = VME_WRITE_16( addr, data ); // whatever data is
    return 0;
  }
  return 0;
}

int EventArrivedV830( char *parPtr ) {
    return 0;
}

void PauseArmV830( char *) {}

void PauseDisArmV830( char *) {}


bool CallEPICS(V830_ParType * V830, datePointer *data_ptr) {
  //spill_g = data_ptr[V830->SpillGateChannel + decodeChar( V830->UseHeader )];
  int clock_ch = 13;
  int spill_g = data_ptr[clock_ch] & 0x3FFFFFF;
  int lIndex;
  u32 lTempInt;
  try {
    lIndex = lDescription->FindPVClientByTitle("SpillGate");
    if(lIndex != -1) {
      //sprintf( message, "Spill Gate : %d  miliseconds", spill_g);
      //INFO( message );
      lTempInt = spill_g;
      gEPICSClients[lIndex]->Write(lTempInt);
    }

    lIndex = lDescription->FindPVClientByTitle("DAQEventCount");
    if(lIndex != -1) {
      lTempInt = DAQCONTROL->eventCount;
      sprintf( message, "DAQEventCount : %d", (int)lTempInt);
      INFO( message );
      gEPICSClients[lIndex]->Write(lTempInt);
    }

    for(int chNum=0; chNum<12; chNum++) {
      stringstream ss;
      ss << "V830Ch" << chNum;
      lIndex = lDescription->FindPVClientByTitle(ss.str());
      if (lIndex != -1) {
        lTempInt = data_ptr[chNum + decodeChar( V830->UseHeader )] & 0x3FFFFFF;;
        gEPICSClients[lIndex]->Write(lTempInt);
      }
    }

    overflowCount= 0;
    emptyEventCount = 0;
    lDATEStatusClient->Write("TAKING_DATA");

  }catch(MICEException & lExc) {
    std::cerr << lExc.What() << std::endl;
    std::cerr << lExc.History() << std::endl;
    FATAL("FATAL error in V830: EPICS Client is disconnected");
    readList_error = SYNC_ERROR;
    return false;
  }
  catch(std::exception & lExc) {
    std::cerr << lExc.what() << std::endl;
    FATAL("FATAL error in V830: EPICS Client is disconnected");
    readList_error = SYNC_ERROR;
    return false;
  }
  catch(...) {
    std::cerr << "Unknown exception occurred..." << std::endl;
    FATAL("FATAL error in V830: EPICS Client is disconnected");
    readList_error = SYNC_ERROR;
    return false;
  }

  return true;
}

bool IsGoodEvent(V830_ParType * V830, 
                 struct equipmentHeaderStruct *eq_header_ptr,
                 datePointer *data_ptr) {

  int npart_t = data_ptr[decodeChar( V830->UseHeader )]& 0x3FFFFFF;
  eq_header_ptr->equipmentTypeAttribute[0]= npart_t;

  sprintf( message, "Number of particle triggers in V830: %d", npart_t );
  INFO( message );

  if ( npart_t > maxNevents ) {
    sprintf( message, "Too many triggers for the online monitoring buffer - Reduce target dip depth or Spill Gate Width!" );
    ERROR( message );

    sprintf( message, "FATAL. Ignoring the spill; setting to the number of triggers to 0");
    ERROR( message );

    eq_header_ptr->equipmentTypeAttribute[0]= 0;
    overflowCount++;
    return false;
  }

  if ( npart_t < 2 ) {
    emptyEventCount++;
    return false;
  }

  return true;
}

