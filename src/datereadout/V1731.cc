#include "V1731.hh"

#include "MDEv1731.hh"
#include "V1731Def.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

std::vector<MDEv1731 *> fadc1731 (30);

void ArmV1731 (char *parPtr) {

  V1731_ParType * V1731 = (V1731_ParType*) parPtr;
  int geo = *V1731->GEO;
  if(!fadc1731[geo])
    fadc1731[geo] = new MDEv1731();
    else {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream(fadc1731[geo])) << "The Geo Number " << geo << " is not unique.";
    messanger->sendMessage(MDE_FATAL);
    return;
  }

  fadc1731[geo]->setParams("GEO",                    geo)
                          ("BaseAddress",            getBA( V1731->BaseAddress ))
                          ("PostTriggerOffset",      *V1731->PostTriggerOffset)
                          ("BufferOrganizationCode", V1731_Buffer_Number_1024)
                          ("BlockTransfEventNum",    1024)
                          ("ChannelMask",            0xff)
                          ("TriggerOverlapping",     1)
                          ("WordsPerEvent",          *V1731->BufferSize) //32)
                          ("UseSoftwareTrigger",     0)
                          ("UseExternalTrigger",     1)
                          ("DualEdgeSampling",       0)
                          ("ZSThreshold",            *V1731->ZSThreshold);

  if ( !fadc1731[geo]->Arm() )
    readList_error = VME_ERROR;

}

void DisArmV1731 (char *parPtr) {
  V1731_ParType * V1731 = ( V1731_ParType *) parPtr;
  int geo = *V1731->GEO;
  fadc1731[geo]->DisArm();
}

void AsynchReadV1731(char *parPtr) {}

int ReadEventV1731(char *parPtr, struct eventHeaderStruct *header_ptr,
                   struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {
  int dataStored = 0;
  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT) {

    V1731_ParType *V1731 = (V1731_ParType*) parPtr;
    int geo = *V1731->GEO;
    MDEv1731 *this_fadc = fadc1731[geo];
    // Setting equipment header values
    eq_header_ptr->equipmentBasicElementSize = 4;
    eq_header_ptr->equipmentId = geo;

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

int EventArrivedV1731(char *parPtr) {
    return 0;
}

void PauseArmV1731(char *parPtr) {}

void PauseDisArmV1731(char *parPtr) {}
