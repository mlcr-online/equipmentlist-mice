#ifndef _MDEV792N
#define _MDEV792N 1


#define V792Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(BErrEnable) \
  NEXT(BlkEndEnable)

#include "MiceDAQEquipment.hh"
#include <vector>

class MDEv792 : public MiceDAQEquipment {
 public:
  MDEv792();
  ~MDEv792() {}

  bool Arm();
  bool DisArm();
  int ReadEvent(MDE_Pointer *data=0);
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
  bool enableOV();
  int DataPtrTest(MDE_Pointer *data, int evNum);
  
  IMPLEMENT_PARAMS(V792Params, MDEv792)
};

#endif


