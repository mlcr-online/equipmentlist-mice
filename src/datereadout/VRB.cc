#include "VRB.hh"
#include "MDEvRB.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

std::vector<MDEvRB *> vrb (10);

void ArmVRB(char *parPtr) {

  VRB_ParType *VRB = (VRB_ParType*) parPtr;
  int geo = *VRB->GEO;
  if(!vrb[geo])
    vrb[geo] = new MDEvRB();
    else {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream(vrb[geo])) << "The Geo Number " << geo << " is not unique.";
    messanger->sendMessage(MDE_FATAL);
    return;
  }

  int geodbb1 = *VRB->DBBfirstID;
  vrb[geo]->setParams("GEO",               geo)
                     ("BaseAddress",       getBA(VRB->BaseAddress) )
                     ("DBB_FV",            8 /*decodeChar(VRB->FWversion)*/ )
                     ("N_DBBs",            *VRB->nDBB)
                     ("ChannelMask0_DBB1", 0xffffffff)
                     ("ChannelMask1_DBB1", 0xffffffff)
                     ("Geo_DBB1",          geodbb1)
                     ("ChannelMask0_DBB2", 0xffffffff)
                     ("ChannelMask1_DBB2", 0xffffffff)
                     ("Geo_DBB2",          geodbb1+1)
                     ("ChannelMask0_DBB3", 0xffffffff)
                     ("ChannelMask1_DBB3", 0xffffffff)
                     ("Geo_DBB3",          geodbb1+2)
                     ("ChannelMask0_DBB4", 0xffffffff)
                     ("ChannelMask1_DBB4", 0xffffffff)
                     ("Geo_DBB4",          geodbb1+3)
                     ("ChannelMask0_DBB5", 0xffffffff)
                     ("ChannelMask1_DBB5", 0xffffffff)
                     ("Geo_DBB5",          geodbb1+4)
                     ("ChannelMask0_DBB6", 0xffffffff)
                     ("ChannelMask1_DBB6", 0xffffffff)
                     ("Geo_DBB6",          geodbb1+5);

  if ( !vrb[geo]->Arm() )
    readList_error = VME_ERROR;

}

void DisArmVRB(char * parPtr) {
  VRB_ParType *VRB = ( VRB_ParType *) parPtr;
  int geo = *VRB->GEO;
  vrb[geo]->DisArm();
}

void AsynchReadVRB(char *parPtr) {}

int ReadEventVRB(char *parPtr, struct eventHeaderStruct *header_ptr,
                 struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {

  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT) {

    VRB_ParType *VRB = (VRB_ParType*) parPtr;
    int geo = *VRB->GEO;
    MDEvRB *this_vrb = vrb[geo];

    if( !this_vrb->isGoodEvent() ){
      return 0;
    }

    int nEvts = this_vrb->getNTriggers(1);

    if ( !this_vrb->processMismatch(nEvts) ) {
      readList_error = SYNC_ERROR;
    }

   vrb[geo]->StartDBBReadout();
  }

  return 0;
}

int EventArrivedVRB(char *parPtr) {
  return 0;
}

void PauseArmVRB(char *parPtr) {}
void PauseDisArmVRB(char *parPtr) {}
