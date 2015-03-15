#include "TriggerReceiver.hh"
#include "MDEv977.hh"
#include "MiceDAQSpillStats.hh"
#include "MiceDAQRunStats.hh"
#include "MiceDAQMessanger.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

MDEv977 *ioRegister;

void ArmTriggerReceiver(char *parPtr) {

  ioRegister = new MDEv977();

  // Get Trigger Receiver parameters
  TriggerReceiver_ParType *trParam = (TriggerReceiver_ParType*) parPtr;
  int V977_BaseAddress = getBA( trParam->BaseAddress );
  ioRegister->setParams("GEO", 0)("BaseAddress", V977_BaseAddress);

  if( !ioRegister->ArmTriggerReceiver() )
    readList_error = VME_ERROR;
}

void DisArmTriggerReceiver(char *parPtr) {
  ioRegister->DisArmTriggerReceiver();
}

void AsynchReadTriggerReceiver(char *parPtr) {}

int  ReadEventTriggerReceiver(char *parPtr, struct eventHeaderStruct *header_ptr,
                              struct equipmentHeaderStruct *eq_header_ptrs, datePointer *data_ptr) {

  int dataStored = 0;
  static unsigned int nbInSpill;

  // Get Trigger Receiver parameters.
  TriggerReceiver_ParType *trParam = (TriggerReceiver_ParType*) parPtr;
  int V977_BaseAddress = getBA( trParam->BaseAddress );
  ioRegister->setParams("GEO", 0)("BaseAddress", V977_BaseAddress);

  // Cast the 64b DATE pointer into 32b.
  MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
  ioRegister->setDataPtr(data_ptr_32);

  dataStored += ioRegister->ReadEventTriggerReceiver();
  unsigned int xEvType = ioRegister->getEventType();
  header_ptr->eventType = xEvType;

  MiceDAQSpillStats *spill = MiceDAQSpillStats::Instance();
  (*spill)["DataRecorded"] += dataStored;

  if (xEvType == START_OF_BURST) {

#ifdef EPICS_FOUND
    if ((*spill)["SpillNumber"] < 0 ) {
      MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
      if( epics_instance->isConnected() )
        (*epics_instance)["DATEState"]->write(DS_TAKING_DATA);
    }
#endif

    (*spill)["SpillNumber"] ++;
    nbInSpill = 0;
    EVENT_ID_SET_BURST_NB( header_ptr->eventId, ( (*spill)["SpillNumber"] ) & 0xFFF );
    EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, (unsigned int)(*spill)["SpillNumber"] );
    EVENT_ID_SET_NB_IN_BURST( header_ptr->eventId, nbInSpill);
  }

  if ( xEvType == PHYSICS_EVENT ||
       xEvType == END_OF_BURST  ||
       xEvType == CALIBRATION_EVENT ) {

    nbInSpill++;
    EVENT_ID_SET_BURST_NB( header_ptr->eventId, ( (*spill)["SpillNumber"] ) & 0xFFF );
    EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, (unsigned int)(*spill)["SpillNumber"] );
    EVENT_ID_SET_NB_IN_BURST( header_ptr->eventId, nbInSpill);
  }

  // Set the static flag "Good Event" and the static "Number of events". Because
  // these are static data members the value is the same for all instances (objects)
  // inheriting from the base clase MiceDAQEquipment.
  ioRegister->setGoodEvent(true);
  ioRegister->setNumOfEvts(0);

  ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
  return dataStored;
}

int  EventArrivedTriggerReceiver(char *parPtr) {
  // Get Trigger Receiver parameters
  //MiceDAQMessanger::Instance()->sendMessage("EventArrivedTriggerReceiver", MDE_INFO);
  TriggerReceiver_ParType *trParam = (TriggerReceiver_ParType*) parPtr;
  int V977_BaseAddress = getBA( trParam->BaseAddress );
  ioRegister->setParams("GEO", 0)("BaseAddress", V977_BaseAddress);

  int xTrigger = ioRegister->EventArrivedTriggerReceiver();
  // Note that the static data member mde_eventType_ is set inside EventArrivedTriggerReceiver()
  // Because this is a static data member the value is the same for all instances (objects)
  // inheriting from the base clase MiceDAQEquipment.

  if (xTrigger) {
    // Reset the Spill stats.
    MiceDAQSpillStats::Instance()->setParams("ParticleTriggers", 0)
                                            ("DataRecorded", 0)("ErrorFlag", 0);
  }
  return xTrigger;
}

void PauseArmTriggerReceiver(char *parPtr) {}

void PauseDisArmTriggerReceiver(char *parPtr) {}
