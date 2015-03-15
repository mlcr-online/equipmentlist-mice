#include "Trailer.hh"
#include "MDEv977.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"
#include "MiceDAQRunStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
  static std::string EventSizePVName;
#endif

extern MDEv977 *ioRegister; // The original declaration is in TriggerRecever.cc .


void ArmTrailer(char * parPtr ) {
  // Trailer is the last to arm
  if ( !ioRegister->ArmTrailer() )
    readList_error = VME_ERROR;

  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();

#ifdef EPICS_FOUND
  try {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if( epics_instance->isConnected() ) {
      (*epics_instance)["DATEState"]->write(DS_ARMED);
    }
    EventSizePVName = "EvtSize-" + std::string( getenv("DATE_HOSTNAME") );
  } catch(MiceDAQException lExc) {
    messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
  }
#endif

  messanger->setCallVerbosity(1);

}

void DisArmTrailer(char * parPtr) {
  // This Equipment is the first to be disarmed
  MiceDAQRunStats::Instance()->getInfoShort();

  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();
  messanger->setCallVerbosity(2);

  ioRegister->DisArmTrailer();

#ifdef EPICS_FOUND
  try {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if( epics_instance->isConnected() )
      (*epics_instance)["DATEState"]->write(DS_RUN_COMPLETE);
  } catch(MiceDAQException lExc) {
    MiceDAQMessanger *messanger =  MiceDAQMessanger::Instance();
    messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
  }
#endif
}

void AsynchReadTrailer(char *parPtr) {}

int  ReadEventTrailer(char *parPtr, struct eventHeaderStruct *header_ptr,
                      struct equipmentHeaderStruct *eq_header_ptrs, datePointer *data_ptr) {
  // This ReadEvent is always called at the end of the equipment list.
  int dataStored = 0;
  dataStored += ioRegister->ReadEventTrailer();
  //ioRegister->dump();
  MiceDAQSpillStats *spillStats = MiceDAQSpillStats::Instance();
  MiceDAQRunStats *runStats = MiceDAQRunStats::Instance();
  (*spillStats)["DataRecorded"] += dataStored;
  (*runStats)["DataRecorded"] += (*spillStats)["DataRecorded"];

  if(ioRegister->getEventType() == PHYSICS_EVENT) {
    (*runStats)["NumberOfSpills"] ++;
    (*spillStats)["ParticleTriggers"] = ioRegister->getNumOfEvts();
    spillStats->getInfoShort();
    //spillStats->getInfo();
    //runStats->printDataRecorded();

#ifdef EPICS_FOUND
    try {
      MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
      if( epics_instance->isConnected()  ) {
        if (ioRegister->getNumOfEvts() > ioRegister->getMaxNumOfEvts())
          (*epics_instance)["DATEState"]->write(DS_TAKING_DATA);

        u32 lTmpInt = (*spillStats)["DataRecorded"];
        (*epics_instance)[EventSizePVName.c_str()]->write(lTmpInt);
      }
    }   catch(MiceDAQException lExc) {
      MiceDAQMessanger *messanger =  MiceDAQMessanger::Instance();
      messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
      messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
    }
#endif
  }

  if ( !ioRegister->isGoodEvent() )
    (*runStats)["NumberOfErrorFlags"] ++;

  return dataStored;
}

int  EventArrivedTrailer(char *parPtr) {
  return 0;
}

void PauseArmTrailer(char *parPtr) {}

void PauseDisArmTrailer(char *parPtr) {}
