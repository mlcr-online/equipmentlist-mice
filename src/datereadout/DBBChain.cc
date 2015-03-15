#include "DBBChain.hh"
#include "MDEvRB.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

std::vector<MDEDBB *> dbb_in_chain(60);

void ArmDBBChain(char *parPtr) {

//   MiceDAQMessanger::Instance()->sendMessage("This is ArmDBBChain", MDE_ERROR);

  DBBChain_ParType *DBBchain = (DBBChain_ParType*) parPtr;

  int first_geo = *DBBchain->FirstDbbGEO;
  int ndbbs     = *DBBchain->nDBBs;

  for (int i=0; i<ndbbs; i++) {
    int geo = first_geo + i;

    if(!dbb_in_chain[geo])
      dbb_in_chain[geo] = new MDEDBB();
    else {
      MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
      *(messanger->getStream(dbb_in_chain[geo])) << "The Geo Number " << geo << " is not unique.";
      messanger->sendMessage(MDE_FATAL);
      return;
    }

    dbb_in_chain[geo]->setParams("GEO",             geo)
                                ("BaseAddressVRB",  getBA(DBBchain->VRBBaseAddress) )
                                ("IdInVRB",         i+1);

    if ( !dbb_in_chain[geo]->Arm() ) {
      readList_error = VME_ERROR;
      return;
    }
  }

}

void DisArmDBBChain(char *parPtr) {
  DBBChain_ParType *DBBchain = (DBBChain_ParType*) parPtr;

  int first_geo = *DBBchain->FirstDbbGEO;
  int ndbbs     = *DBBchain->nDBBs;

  for (int i=0; i<ndbbs; i++) {
    int geo = first_geo + i;

    if( dbb_in_chain[geo] )
      dbb_in_chain[geo]->DisArm();
  }
}

void AsynchReadDBBChain(char *parPtr) {}

int ReadEventDBBChain(char *parPtr, struct eventHeaderStruct *header_ptr,
                 struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  int dataStored = 0;
  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT) {
  DBBChain_ParType *DBBchain = (DBBChain_ParType*) parPtr;

    int first_geo = *DBBchain->FirstDbbGEO;
    int ndbbs     = *DBBchain->nDBBs;

    // Cast the 64b DATE pointer into 32b.
    MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);

    for (int i=0; i<ndbbs; i++) {
      int geo = first_geo + i;
      MDEDBB *this_dbb = dbb_in_chain[geo];

      this_dbb->setDataPtr(data_ptr_32);
      int nbRead = this_dbb->ReadEvent();
      data_ptr_32 += nbRead/sizeof(MDE_Pointer);
      dataStored  += nbRead;

//       int ndw = this_dbb->getNumDataWords();
//       MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
//       *(messanger->getStream(this_dbb)) << "data Stored (chain): " <<  dataStored
//                                         << "\nndw (chain): " <<  ndw
//                                         << "\ndata_ptr_32: " << data_ptr_32;
//       messanger->sendMessage(MDE_ERROR);
    }
  }

  ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
  return dataStored;
}

int EventArrivedDBBChain(char *parPtr) {
  return 0;
}

void PauseArmDBBChain(char *parPtr) {}

void PauseDisArmDBBChain(char *parPtr) {}


