#ifndef _MDEV1290
#define _MDEV1290 1


#define V1290Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(ChannelMask) \
  NEXT(UseEventFIFO) \
  NEXT(UseExtendedTTT) \
  NEXT(DoInit) \
  NEXT(TriggerMode) \
  NEXT(WinWidth) \
  NEXT(WinOffset) \
  NEXT(SwMargin) \
  NEXT(RejMargin) \
  NEXT(TTSubtraction) \
  NEXT(DetectionEdge) \
  NEXT(TDCHeader) \
  NEXT(LSBCode) \
  NEXT(DeadtimeCode)

#define MICROCONTROLER_TIMEOUT 10000

#include "MiceDAQEquipment.hh"

class MDEv1290 : public MiceDAQEquipment {
 public:
  MDEv1290();
  ~MDEv1290() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

  int getNTriggers() {return this->getNEventsStored();}

  bool softwareTrigger();
  bool softwareClear();

  int getMaxMemUsed(int nEvents=1);

  int getTotNTriggers();
  int getNEventsStored();
  int getStatusReg();
  int getNumDataWords();

 private:
  int OPCodeWrite( short OPCode, unsigned int nw, short* microData);
  int OPCodeRead(  short OPCode, unsigned int nw, short* microData);
  int OPCodeCycle(unsigned short mask, short OPCode, unsigned int nw, short* microData);
  int MicroPollingLoop(unsigned long ad, unsigned short mask);

  bool Init();
  bool setControlReg(int cr);
  bool setGEO(long32 geo);
  bool channelsEnable(int mask);

  int n_data_words_recorded_;

  IMPLEMENT_PARAMS(V1290Params, MDEv1290)
};

#endif

