#ifndef _MDEV1724
#define _MDEV1724 1


#define V1724Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(PostTriggerOffset) \
  NEXT(BUfferOrganizationCode) \
  NEXT(BlockTransfEventNum) \
  NEXT(ChannelMask) \
  NEXT(TriggerOverlapping) \
  NEXT(WordsPerEvent) \
  NEXT(UseSoftwareTrigger) \
  NEXT(UseExternalTrigger) \
  NEXT(TriggerIOLevel) \
  NEXT(ZSThreshold)

#include "MiceDAQEquipment.hh"

class MDEv1724 : public MiceDAQEquipment {
 public:
  MDEv1724();
  ~MDEv1724() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

  int getNTriggers() {return this->getNEventsStored();}

  int ReadEventNoZS();
  int ReadEventZS();
  bool softwareTrigger();
  bool softwareClear();

  int getMaxMemUsed(int nEvents=1024);
  int getMaxMemUsedBLT(int nEvents=1024);

  int getNEventsStored();
  int doBLTRead(MDE_Pointer *dataPtr, int nEvt);

 private:
  bool boardReset();
  bool enableEEinBLT();
  bool setGEO(long32 geo);
  bool setPostTriggerOffset(int offset);
  bool setVMEControlReg(int cr);
  bool getVMEControlReg(int &cr);
  bool setAcqControlReg(int mask);
  bool channelsEnable(int mask);
  bool setNOutBuffers(int mask);
  bool setWordsPerEvent(int nWords);
  bool setNEventsForBLT(int nEvents);
  bool setTriggerSource(int mask);
  bool setChConfigReg(int mask);
  bool setTriggerIOLevel(int il);
  bool setIOConfigReg(int mask);

  bool ZS_scanChannel(MDE_Pointer *ptr_tmp);
  int  ZS_processEvent(MDE_Pointer *dataPtr, MDE_Pointer *ptr_tmp);

  int length_;
  int ZSThreshold_;

  IMPLEMENT_PARAMS(V1724Params, MDEv1724)
};

#endif



