#include "PartTriggerSelector.hh"
#include "MDEv977.hh"
#include "MiceDAQMessanger.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

extern MDEv977 *ioRegister; // The original declaration is in TriggerRecever.cc .
void ArmPartTriggerSelector (char *parPtr)
{
  std::string lTempStr("");

#ifdef EPICS_FOUND

  try {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if ( epics_instance->isConnected() ) {
      (*epics_instance)["TriggerCondition"]->read(lTempStr);
      //(*epics_instance)["TriggerCondition"]->write<std::string>(lTempStr);
    } else {
      PartTriggerSelector_ParType* params = ( PartTriggerSelector_ParType *) parPtr;
      lTempStr = std::string( params->TriggerCondition );
    }
  } catch(MiceDAQException lExc) {
    MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();
    messanger->sendMessage("PartTriggerSelector: Unable to get trigger condition.", MDE_FATAL);
    messanger->sendMessage( lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage( lExc.GetDescription(), MDE_FATAL);
    readList_error = SYNC_ERROR;
    return;
  }

#else

  PartTriggerSelector_ParType *params = (PartTriggerSelector_ParType*) parPtr;
  lTempStr = std::string( params->TriggerCondition );

#endif

  ioRegister->ArmPartTriggerSelector(lTempStr);
}

void DisArmPartTriggerSelector(char *parPtr) {
  ioRegister->DisArmPartTriggerSelector();
}

void AsynchReadPartTriggerSelector(char *parPtr) {}

int ReadEventPartTriggerSelector(char *parPtr, struct eventHeaderStruct *header_ptr,
                                 struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {
  return 0;
}

int EventArrivedPartTriggerSelector(char *parPtr) {
  return 0;
}

void PauseArmPartTriggerSelector(char *parPtr) {}

void PauseDisArmPartTriggerSelector(char *parPtr) {}

