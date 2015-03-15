#include "VCB.hh"
#include "MDEvCB.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

std::vector<MDEvCB *> vcb (10);

void ArmVCB(char *parPtr) {

  VCB_ParType *VCB = (VCB_ParType*) parPtr;
  int geo = *VCB->GEO;
  if(!vcb[geo])
    vcb[geo] = new MDEvCB();
    else {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream(vcb[geo])) << "The Geo Number " << geo << " is not unique.";
    messanger->sendMessage(MDE_FATAL);
    return;
  }

  vcb[geo]->setParams("GEO",         geo)
                     ("BaseAddress", getBA(VCB->BaseAddress) )
                     ("N_FEBs",      16);

  if ( !vcb[geo]->Arm() )
    readList_error = VME_ERROR;
}

void DisArmVCB(char * parPtr) {
  VCB_ParType *VCB = ( VCB_ParType *) parPtr;
  int geo = *VCB->GEO;
  vcb[geo]->DisArm();
  delete vcb[geo];
}

void AsynchReadVCB(char *parPtr) {}

int ReadEventVCB(char *parPtr, struct eventHeaderStruct *header_ptr,
                 struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {
  return 0;
}

int EventArrivedVCB(char *parPtr) {
  return 0;
}

void PauseArmVCB(char *parPtr) {}
void PauseDisArmVCB(char *parPtr) {}
