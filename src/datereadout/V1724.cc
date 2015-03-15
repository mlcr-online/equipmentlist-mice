#include "V1724.hh"

#include "MDEv1724.hh"
#include "V1724Def.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

std::vector<MDEv1724 *> fadc1724 (30);

extern int maxNevents;

int buffer_size;

void ArmV1724 (char *parPtr) {

  V1724_ParType *V1724 = (V1724_ParType*) parPtr;
  int geo = *V1724->GEO;
  if(!fadc1724[geo])
    fadc1724[geo] = new MDEv1724();
    else {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream(fadc1724[geo])) << "The Geo Number " << geo << " is not unique.";
    messanger->sendMessage(MDE_FATAL);
    return;
  }

  fadc1724[geo]->setParams("GEO",                    *V1724->GEO)
                          ("BaseAddress",            getBA( V1724->BaseAddress ))
                          ("PostTriggerOffset",      *V1724->PostTriggerOffset)
                          ("BUfferOrganizationCode", V1724_OutputBufferSize_1K)
                          ("BlockTransfEventNum",    1024)
                          ("ChannelMask",            0xff)
                          ("TriggerOverlapping",     1)
                          ("WordsPerEvent",          *V1724->BufferSize) //32)
                          ("UseSoftwareTrigger",     0)
                          ("UseExternalTrigger",     1)
                          ("ZSThreshold",            *V1724->ZSThreshold);

  if ( !fadc1724[geo]->Arm() )
    readList_error = VME_ERROR;
}


void DisArmV1724 (char *parPtr) {

  V1724_ParType *V1724 = (V1724_ParType*) parPtr;
  int geo = *V1724->GEO;
  fadc1724[geo]->DisArm();
}

void AsynchReadV1724(char *parPtr) {}

int ReadEventV1724(char *parPtr, struct eventHeaderStruct *header_ptr,
                   struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr)  {

  int dataStored = 0;

  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT) {

    V1724_ParType *V1724 = (V1724_ParType*) parPtr;
    int geo = *V1724->GEO;
    MDEv1724 *this_fadc = fadc1724[geo];

    // Setting equipment header values
    eq_header_ptr->equipmentBasicElementSize = 4;
    eq_header_ptr->equipmentId = *V1724->GEO;

    if( !this_fadc->isGoodEvent() ){
      //Clear memory.
      this_fadc->softwareClear();
      return 0;
    }

    short nEvts = this_fadc->getNEventsStored();
    //MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    //*(messanger->getStream(this_fadc)) << "Number of Particle Triggers is " << nEvts;
    //messanger->sendMessage(MDE_INFO);

    if ( !this_fadc->processMismatch(nEvts) ) {
      readList_error = SYNC_ERROR;

#ifdef EPICS_FOUND
      try {
        MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
        if( epics_instance->isConnected() )
          (*epics_instance)["DATEState"]->write(DS_ERROR);

      } catch(MiceDAQException lExc) {
        MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
        messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
        messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
      }
#endif

      return 0;
    }

    // Cast the 64b DATE pointer into 32b.
    MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
    this_fadc->setDataPtr(data_ptr_32);
    dataStored += this_fadc->ReadEvent();
  }

  ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
  return dataStored;
}

int EventArrivedV1724(char *parPtr) {
    return 0;
}

void PauseArmV1724(char *parPtr) {}

void PauseDisArmV1724(char *parPtr) {}
