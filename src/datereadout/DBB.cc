#include "DBB.hh"
#include "MDEvRB.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

std::vector<MDEDBB *> dbb(60);

void ArmDBB(char *parPtr) {
  DBB_ParType *DBB = (DBB_ParType*) parPtr;

  int geo = *DBB->GEO;

  if(!dbb[geo])
    dbb[geo] = new MDEDBB();
    else {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream(dbb[geo])) << "The Geo Number " << geo << " is not unique.";
    messanger->sendMessage(MDE_FATAL);
    return;
  }

  dbb[geo]->setParams("GEO",             geo)
                     ("BaseAddressVRB",  getBA(DBB->VRBBaseAddress) )
                     ("IdInVRB",         *DBB->IdInVRB);

  if ( !dbb[geo]->Arm() )
    readList_error = VME_ERROR;
}

void DisArmDBB(char *parPtr) {
  DBB_ParType *DBB = (DBB_ParType*) parPtr;
  int geo = *DBB->GEO;
  dbb[geo]->DisArm();
}

void AsynchReadDBB(char *parPtr) {}

int ReadEventDBB(char *parPtr, struct eventHeaderStruct *header_ptr,
                 struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  int dataStored = 0;
  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT) {

    DBB_ParType * DBB = (DBB_ParType*) parPtr;
    int geo = *DBB->GEO;

    MDEDBB *this_dbb = dbb[geo];

    // Cast the 64b DATE pointer into 32b.
    MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
    this_dbb->setDataPtr(data_ptr_32);
    dataStored += this_dbb->ReadEvent();
//     MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
//     *(messanger->getStream(this_dbb)) << "data Stored: " <<  dataStored;
//     messanger->sendMessage(MDE_ERROR);

  }

  ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
  return dataStored;
}

int EventArrivedDBB(char *parPtr) {
  return 0;
}

void PauseArmDBB(char *parPtr) {}
void PauseDisArmDBB(char *parPtr) {}
