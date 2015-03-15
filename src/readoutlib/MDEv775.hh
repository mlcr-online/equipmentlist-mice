#ifndef _MDEV775N
#define _MDEV775N 1


#define V775Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(BErrEnable) \
  NEXT(BlkEndEnable)

#include "MiceDAQEquipment.hh"
#include <vector>

class MDEv775 : public MiceDAQEquipment {
 public:
  MDEv775();
  ~MDEv775() {}

  bool Arm();
  bool DisArm();
  int ReadEvent();
  int EventArrived();

  bool boardReset();
  bool AcqTestMode();

  int getMaxMemUsed(int nEvents);
  int doBLTRead(MDE_Pointer *data=0);

  int getGEO();
  int getFirmwareV();
  int getStatus();
  std::string Status1ToString(short stReg);
  std::string Status2ToString(short stReg);
  short getStatusReg1();
  short getStatusReg2();
  bool bitSetReg1(short reg1);
  bool bitClearReg1(short reg1);
  bool setGEO(long32 geo);
  bool bitSetReg2(short reg2);
  bool bitClearReg2(short reg2);
  bool evntCounterReset();
  bool dataReady(short reg);
  bool boardBusy(short reg);
  bool bufferFull(short reg);
  bool bufferEmpty(short reg);
  bool setControlReg(short cr);
  bool setFulScaleRange(short fsr);

  IMPLEMENT_PARAMS(V775Params, MDEv775)
};

#endif


