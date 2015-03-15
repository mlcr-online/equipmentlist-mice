#include "V2718.hh"
#include "MDEv2718.hh"
#include "MDEv1290.hh"
#include "MDEv1724.hh"
#include "MDEv1731.hh"
#include "MDEv977.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"
#include "MiceDAQRunStats.hh"
#include "DATEMessage.hh"

extern std::vector<MDEv1290 *> tdc;       // The original declaration is in V1290.cc. The objects are deleted in DisArmV2718.
extern std::vector<MDEv1724 *> fadc1724;  // The original declaration is in V1724.cc. The objects are deleted in DisArmV2718.
extern std::vector<MDEv1731 *> fadc1731;  // The original declaration is in V1731.cc. The objects are be deleted in DisArmV2718.
extern MDEv977 *ioRegister; // The original declaration is in TriggerReceiver.cc. The object will be deleted in DisArmV2718.
MDEv2718 *controler;

void ArmV2718(char *parPtr ) {

  controler->setUserPrint(&doHostNamePrint);

  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();
  messanger->setCallVerbosity(2);
  messanger->setMessageFunc(&doDATEMessage);
  messanger->dumpToFile();

  MiceDAQSpillStats *spill = MiceDAQSpillStats::Instance();
  spill->setUserPrint(&doHostNamePrint);
  MiceDAQRunStats *run = MiceDAQRunStats::Instance();
  run->setUserPrint(&doHostNamePrint);

  V2718_ParType *V2718 = (V2718_ParType *) parPtr;

  //Initialize V2718
  controler = new MDEv2718();
  controler->setParams("GEO",        0)
                      ("BNumber",    *V2718->BNumber)
                      ("BLink",      *V2718->BLink)
                      ("FIFOMode",   1)
                      ("MaxNumEvts", *V2718->MaxNumOfEvts);

  if ( !controler->Arm() )
    readList_error = VME_ERROR;
}

void DisArmV2718(char *parPtr) {
  // This will be the last equipment to be disarmed.
  controler->DisArm();
  delete controler;
  tdc.resize(0);
  fadc1724.resize(0);
  fadc1731.resize(0);
  delete ioRegister;
}

void AsynchReadV2718(char *parPtr){}

int ReadEventV2718(char *parPtr, struct eventHeaderStruct *header_ptr,
                   struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  return 0;
}

int EventArrivedV2718( char *parPtr ) {
  return 0;
}

void PauseArmV2718(char *parPtr) {}

void PauseDisArmV2718(char *parPtr) {}
