#ifndef _MDEV2718
#define _MDEV2718 1

#include "MiceDAQEquipment.hh"
#include "MDEMacros.hh"


#define V2718Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BNumber) \
  NEXT(BLink) \
  NEXT(FIFOMode) \
  NEXT(MaxNumEvts)

class MDEv2718 : public MiceDAQEquipment {
 public:
  MDEv2718();
  ~MDEv2718() {}

  bool Arm();
  bool DisArm();
  int ReadEvent();
  int EventArrived();

  bool SetOutputConf();
  bool SetInputConf();
  bool SetPulserConf();
  bool GetPulserConf();
  bool StartPulser();
  bool StopPulser();
  bool SetOutputSignal();
  bool ClearOutputSignal();
  bool IRQEnable();
  bool IACK();
  bool IRQCheck();
  int IRQWait();

  unsigned int ReadRegister();

 protected:
  bool Init();
  bool Configure();
  bool SetFIFOMode();

  IMPLEMENT_PARAMS(V2718Params, MDEv2718)
};

#endif



