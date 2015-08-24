#ifndef _MDEVLSB
#define _MDEVLSB 1


#define VLSBParams(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(VLSBid) \
  NEXT(Master) \
  NEXT(UseInternalTrigger)

#include "MiceDAQEquipment.hh"


class MDEVLSB : public MiceDAQEquipment {


 public:
  MDEVLSB();
  ~MDEVLSB() {}

  // DAQ Stuff:
  bool Arm();
  bool DisArm();

  // TODO: Implement these functions:
  //int ReadEvent();
  //int EventArrived();
  //int getNTriggers();
  // TODO: Clarify byte/32bit word count. Note that in the
  // date data size size is in bytes.

  // Start/Stop Data Taking Operations:
  bool EnableAquisition();
  bool DisableAquisition();

  // Base Operations:
  bool DisableLVDS();
  bool EnableLVDS();
  bool DisableTrigger();
  bool EnableTrigger();

  // Load the number of encoded triggers:
  int GetTriggerCount();


  //TODO:  Other functions:
  int getFirmwareV();

  IMPLEMENT_PARAMS(VLSBParams, MDEVLSB)

};


#endif
