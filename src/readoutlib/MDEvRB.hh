#ifndef _MDEVRB
#define _MDEVRB 1


#define VRBParams(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(DBB_FV) \
  NEXT(N_DBBs) \
  NEXT(ChannelMask0_DBB1) \
  NEXT(ChannelMask1_DBB1) \
  NEXT(Geo_DBB1) \
  NEXT(ChannelMask0_DBB2) \
  NEXT(ChannelMask1_DBB2) \
  NEXT(Geo_DBB2) \
  NEXT(ChannelMask0_DBB3) \
  NEXT(ChannelMask1_DBB3) \
  NEXT(Geo_DBB3) \
  NEXT(ChannelMask0_DBB4) \
  NEXT(ChannelMask1_DBB4) \
  NEXT(Geo_DBB4) \
  NEXT(ChannelMask0_DBB5) \
  NEXT(ChannelMask1_DBB5) \
  NEXT(Geo_DBB5) \
  NEXT(ChannelMask0_DBB6) \
  NEXT(ChannelMask1_DBB6) \
  NEXT(Geo_DBB6)

#define ALL_DBBS 99

#include "MiceDAQEquipment.hh"

class MDEvRB : public MiceDAQEquipment {
 public:
  MDEvRB();
  ~MDEvRB() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

  int getNTriggers() {return this->getNTriggers(1);} 


  int getNTriggers(unsigned int dbbId);

  bool StartDBBReadout(unsigned int dbbId = ALL_DBBS);
  int GetDBBData(unsigned int dbbId);

  bool softwareClear();
  int getMaxMemUsed(int nEvents=1);

  int getSpillWidth(unsigned int dbbId);
  int getNumDataWords(unsigned int dbbId);
  int getTransmissionStatus();

  bool resetDBB(unsigned int dbbId);
  bool resetAllDBBs();
  bool fastResetDBB(unsigned int dbbId);
  bool fastResetAllDBBs();
  int getDBBStatus(unsigned int dbbId);
  std::string DbbStatusToString(int st);

  bool testEM();
  bool testFV();
  bool MemClear(unsigned int memaddr, unsigned int memsize);

//  private:

  bool SetDBBChEnableMask(unsigned int dbbId, int mask0, int mask1);
  bool SetAllDBBChEnableMasks();

  bool SetDBBGeo(unsigned int dbbId, unsigned int Geo);
  bool SetNDBBS(unsigned int nDBBs);
  bool SetAllDBBGeos();
  unsigned int GetDBBFV(unsigned int dbbId);
  bool checkAllDBBFV();

  IMPLEMENT_PARAMS(VRBParams, MDEvRB)
};

#endif


