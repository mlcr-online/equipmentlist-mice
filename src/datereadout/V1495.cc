#include "V1495.hh"
#include "MDEv1495.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

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
