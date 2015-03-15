#ifndef MICEDAQRUNSTATS
#define MICEDAQRUNSTATS 1

#include "MiceDAQParamHolder.hh"

#define RunStatsParams(FIRST, NEXT) \
  FIRST(NumberOfSpills) \
  NEXT(DataRecorded) \
  NEXT(NumberOfErrorFlags)

class MiceDAQMessanger;

class MiceDAQRunStats : public MiceDAQParamHolder {
 public:
  ~MiceDAQRunStats();

  static MiceDAQRunStats* Instance();

  int& operator[](const char* pName);

  void printDataRecorded();

 private:
  MiceDAQRunStats();

  //static MiceDAQRunStats* instance_;
  MiceDAQMessanger* mde_messanger_;

  IMPLEMENT_PARAMS(RunStatsParams, MiceDAQRunStats)
};

#endif



