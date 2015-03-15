#include "V830.hh"
#include "MDEv830.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"

  static int overflowCount(0);
#endif

MDEv830 *scaler;

static int emptyEventCount(0);

void ArmV830 ( char * parPtr ) {

  scaler = new MDEv830();
  V830_ParType * V830 = ( V830_ParType *) parPtr;

  scaler->setParams( "GEO",          (int)*V830->GEO )
                   ( "BaseAddress",  getBA(V830->BaseAddress) )
                   ( "ChannelMask",  channelPattern(V830->ChannelConfig) )
                   ( "TriggerMode",  *V830->TriggerMode )
                   ( "UseHeader",    decodeChar(V830->UseHeader) )
                   ( "UseNarrow",    decodeChar(V830->UseNarrow) )
                   ( "AutoReset",    decodeChar(V830->AutoReset) );

  if ( !scaler->Arm() )
    readList_error = VME_ERROR;
}

void DisArmV830 ( char * parPtr ) {
  scaler->DisArm();
  delete scaler;
}

void AsynchReadV830( char *parPtr) {}

int ReadEventV830( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {

  V830_ParType * V830 = ( V830_ParType *) parPtr;
  int dataStored = 0;

  eq_header_ptr->equipmentId = (int)*V830->GEO;
  eq_header_ptr->equipmentBasicElementSize = 4;
  if ( header_ptr->eventType == PHYSICS_EVENT && scaler->isGoodEvent() ) {
    // Cast the 64b DATE pointer into 32b.
    MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
    scaler->setDataPtr(data_ptr_32);

    //read the status register
    dataStored += scaler->ReadEvent();

    int npart_t = data_ptr_32[decodeChar( V830->UseHeader )]& 0x3FFFFFF;
    //MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    //*(messanger->getStream(scaler)) << "Number of Particle Triggers is " << npart_t;
    //messanger->sendMessage(MDE_INFO);
    scaler->processMismatch(npart_t);
    if ( npart_t < 2 ) {
      emptyEventCount++;
      //scaler->setGoodEvent(false);
    } else emptyEventCount = 0;

#ifdef EPICS_FOUND
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if( epics_instance->isConnected() ) {
      if ( emptyEventCount>10 ) {
        (*epics_instance)["DATEState"]->write(DS_ERROR);
      } else {
        CallEPICS(V830, data_ptr_32);
      }
    }
#endif

    // Clear the scalars
    scaler->softwareClear();

    ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
    return dataStored;
  } else if ( header_ptr->eventType == START_OF_BURST ) {
    // Clear the scalars
    scaler->softwareClear();

    return 0;
  }

  return 0;
}

int EventArrivedV830( char *parPtr ) {
    return 0;
}

void PauseArmV830( char *parPtr) {}

void PauseDisArmV830( char *parPtr) {}

#ifdef EPICS_FOUND

bool CallEPICS(V830_ParType * V830, MDE_Pointer *data_ptr) {
  //spill_g = data_ptr[V830->SpillGateChannel + decodeChar( V830->UseHeader )];
  int clock_ch = 13;
  double spill_g = double(data_ptr[clock_ch] & 0x3FFFFFF);
  //int spill_g = scaler->getCount(int chNum);

  u32 lTmpInt;
  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();
  try {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    (*epics_instance)["SpillGate"]->write(spill_g);

    lTmpInt = DAQCONTROL->eventCount;
    (*epics_instance)["DAQEventCount"]->write(lTmpInt);

    for(int chNum=0; chNum<12; chNum++) {
      std::stringstream ss;
      ss << "V830Ch" << chNum;
      lTmpInt = data_ptr[chNum + decodeChar( V830->UseHeader )] & 0x3FFFFFF;
      const char* pvName = ss.str().c_str();
      (*epics_instance)[pvName]->write(lTmpInt);
    }

    overflowCount= 0;
    //emptyEventCount = 0;
    (*epics_instance)["DATEState"]->write(DS_TAKING_DATA);

  }catch(MiceDAQException lExc) {
    messanger->sendMessage( lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage( lExc.GetDescription(), MDE_FATAL);
    messanger->sendMessage(scaler, "EPICS Client is disconnected", MDE_FATAL);
    readList_error = SYNC_ERROR;
    return false;
  }

  return true;
}

#endif


