#ifndef _MDEV830
#define _MDEV830 1


#define V830Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(ChannelMask) \
  NEXT(TriggerMode) \
  NEXT(UseHeader) \
  NEXT(UseNarrow) \
  NEXT(AutoReset)

#include "MiceDAQEquipment.hh"

class MDEv830 : public MiceDAQEquipment {
 public:
  MDEv830();
  ~MDEv830() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

  int getNTriggers() {return nTriggers_;}

  bool softwareTrigger();
  bool softwareClear();

  int getMaxMemUsed(int nEvents=1);

  int getCount(int chNum);

 private:
  bool boardReset();
  bool setGEO(long32 geo);
  bool channelsEnable(int mask);
  bool setTriggerMode();
  bool setUseHeader();
  bool setDataFormat();
  bool setAutoReset();

  int nTriggers_;

  IMPLEMENT_PARAMS(V830Params, MDEv830)
};

#endif



