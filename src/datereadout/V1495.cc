#include "V1495.hh"
#include "MDEv1495.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

MDEv1495 *trigger;

void ArmV1495 ( char * parPtr ) {

  trigger = new MDEv1495();
  V1495_ParType * V1495 = ( V1495_ParType *) parPtr;

  trigger->setParams( "GEO",             (int)*V1495->GEO )
                    ( "BaseAddress",     getBA(V1495->BaseAddress) )
                    ("SGOpenDelay",      (int)*V1495->SGOpenDelay )
                    ("SGCloseDelay",     (int)*V1495->SGCloseDelay )
                    ("SggCtrl",          channelPattern(V1495->SggCtrl) )
                    ("TriggerLogicCtrl", channelPattern(V1495->TriggerLogicCtrl) )
                    ( "TOF0Mask",        channelPattern(V1495->TOF0Mask) )
                    ( "TOF1Mask",        channelPattern(V1495->TOF1Mask) )
                    ( "TOF2Mask",        channelPattern(V1495->TOF2Mask) )
                    ("PartTrVetoLenght", (int)*V1495->PartTrVetoLenght );

#ifdef EPICS_FOUND

  std::string trCondition("");
  int trCond_int(0);
  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();

  try {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if ( epics_instance->isConnected() ) {
      double xSpillGate = (*trigger)["SGCloseDelay"] - (*trigger)["SGOpenDelay"];
      xSpillGate *= 1e-2;
      (*epics_instance)["SpillGate"]->write(xSpillGate);
//      (*epics_instance)["SpillGate"]->read(xSpillGate);
//      *(messanger->getStream(trigger)) << "Spill Gate: " << xSpillGate;
//      messanger->sendMessage(MDE_WARNING);

      (*epics_instance)["TriggerCondition"]->read(trCondition);
      if ( trCondition == "GVA1" ) {
        // set output for GVA1
        trCond_int = TRIGGER_GVA;
      } else if ( trCondition == "TOF0" ) {
        // set output for TOF0
        trCond_int = TRIGGER_TOF0_OR;
      } else if ( trCondition == "TOF1" ) {
        // set output TOF1
        trCond_int = TRIGGER_TOF1_OR;
      } else if ( trCondition == "TOF2" ) {
        // set output TOF2
        trCond_int = TRIGGER_TOF2_OR;
      } else if ( trCondition == "PULSER" ) {
        // set output TOF1
        trCond_int = TRIGGER_PULS_200KHz;
      } else {
        // send warning message
        messanger->sendMessage("TriggerSelector: Unknown trigger condition: " + trCondition + ".", MDE_WARNING);
        messanger->sendMessage("TriggerSelector: The possible values are GVA1, TOF0, TOF1 or PULSER.", MDE_WARNING);
        messanger->sendMessage("TriggerSelector: Setting TOF1 trigger", MDE_WARNING);
      // set default = TOF1
        trCond_int = TRIGGER_TOF1_OR;
      }

      trigger->setParams("TriggerLogicCtrl", trCond_int);
    }
  } catch(MiceDAQException lExc) {
    messanger->sendMessage("V1495: Unable to get trigger condition.", MDE_FATAL);
    messanger->sendMessage( lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage( lExc.GetDescription(), MDE_FATAL);
    readList_error = SYNC_ERROR;
    return;
  }

#endif

  if ( !trigger->Arm() )
    readList_error = VME_ERROR;
}

void DisArmV1495 ( char * parPtr ) {
  trigger->DisArm();
  delete trigger;
}

void AsynchReadV1495( char *parPtr) {}

int ReadEventV1495( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {

  V1495_ParType * V1495 = ( V1495_ParType *) parPtr;
  int dataStored = 0;

  eq_header_ptr->equipmentId = (int)*V1495->GEO;
  eq_header_ptr->equipmentBasicElementSize = 4;
  if ( header_ptr->eventType == PHYSICS_EVENT && trigger->isGoodEvent() ) {
    // Cast the 64b DATE pointer into 32b.
    MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
    trigger->setDataPtr(data_ptr_32);

    //read the spill.
    dataStored += trigger->ReadEvent();

    int npart_t = trigger->getNTriggers();

#ifdef EPICS_FOUND

  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();
  i32 ntr = npart_t;
  u32 xSpill = DAQCONTROL->eventCount;
  try {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if ( epics_instance->isConnected() ) {
      (*epics_instance)["V830Ch0"]->write(ntr);
      (*epics_instance)["DAQEventCount"]->write(xSpill);
    }
  } catch(MiceDAQException lExc) {
    messanger->sendMessage("V1495: Unable to get trigger condition.", MDE_FATAL);
    messanger->sendMessage( lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage( lExc.GetDescription(), MDE_FATAL);
    readList_error = SYNC_ERROR;
    return 0;
  }

#endif

//    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
//    *(messanger->getStream(trigger)) << "Number of Particle Triggers is " << npart_t;
//    messanger->sendMessage(MDE_INFO);
    trigger->processMismatch(npart_t);

    ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
    return dataStored;
  } else if ( header_ptr->eventType == START_OF_BURST ) {
    // Clear the Trigger
//    trigger->softwareClear();
  } else if ( header_ptr->eventType == END_OF_BURST ) {
    // Get the LDCs busy times
//    int busy;
//    for (int xLDC=0; xLDC<6; xLDC++)
//      busy = trigger->getPhysBusy(xLDC);
  }

  return 0;
}

int EventArrivedV1495( char *parPtr ) {
  return 0;
}

void PauseArmV1495( char *parPtr) {}

void PauseDisArmV1495( char *parPtr) {}
