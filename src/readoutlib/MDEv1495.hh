#ifndef _MDEV1495
#define _MDEV1495 1

#define V1495Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(SGOpenDelay) \
  NEXT(SGCloseDelay) \
  NEXT(SggCtrl) \
  NEXT(TriggerLogicCtrl) \
  NEXT(TOF0Mask) \
  NEXT(TOF1Mask) \
  NEXT(TOF2Mask) \
  NEXT(PartTrVetoLenght)

#include "MiceDAQEquipment.hh"
#include "V1495Def.hh"

class MDEv1495 : public MiceDAQEquipment {
 public:
  MDEv1495();
  ~MDEv1495() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

  int getNTriggers();
  int getNSpils();
  int getNDW();
  int softwareStart(bool wait=0);
  bool softwareClear();
  unsigned int getPhysBusy(int i);
  int getMaxMemUsed() {return 1024*16+4;}

 private:
  bool reset();
  bool setGEO(int geo);
  bool setSGOpenDelay(int d);
  bool setSGCloseDelay(int d);
  bool setSggCtrl(int c);
  bool setTrCtrl(int c);
  bool setTOF0Mask(int m);
  bool setTOF1Mask(int m);
  bool setTOF2Mask(int m);
  bool setPTVetoLenght(int vl);

  IMPLEMENT_PARAMS(V1495Params, MDEv1495)
};

#endif


