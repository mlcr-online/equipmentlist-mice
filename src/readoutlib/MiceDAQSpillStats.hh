#ifndef MICEDAQSPILLSTATS
#define MICEDAQSPILLSTATS 1

#include "MiceDAQParamHolder.hh"

#define SpillStatsParams(FIRST, NEXT) \
  FIRST(SpillNumber) \
  NEXT(ParticleTriggers) \
  NEXT(DataRecorded) \
  NEXT(ErrorFlag)

class MiceDAQMessanger;

class MiceDAQSpillStats : public MiceDAQParamHolder {
 public:
  ~MiceDAQSpillStats();

  static MiceDAQSpillStats* Instance();

  int& operator[](const char* pName); // {return getParam(pName);}

 private:
  MiceDAQSpillStats();

  //static MiceDAQSpillStats* instance_;
  MiceDAQMessanger* mde_messanger_;

  IMPLEMENT_PARAMS(SpillStatsParams, MiceDAQSpillStats)
};

#endif



