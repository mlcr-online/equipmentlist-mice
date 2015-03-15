#ifndef _EVENTBUILDMANAGER_
#define _EVENTBUILDMANAGER_ 1

#include "equipmentList_common.hh"
#include "DAQBinaryFile.hh"

typedef void (* UserCallback)(int i);

class EventBuildManager {
 public:
  EventBuildManager();
  virtual ~EventBuildManager();

  void SetDataPtr(MDE_Pointer *dataPtr);

  void Go();

  void SetOnStartOfRunDo(UserCallback userFunc)   {_onStartOfRunDo   = userFunc;}
  void SetOnEndOfRunDo(UserCallback userFunc)     {_onEndOfRunDo     = userFunc;}
  void SetOnStartOfSpillDo(UserCallback userFunc) {_onStartOfSpillDo = userFunc;}
  void SetOnEndOfSpillDo(UserCallback userFunc)   {_onEndOfSpillDo   = userFunc;}


  int GetSpillCount()  const {return spill_count_;}
  int GetRunCount()    const {return run_count_;}
  void SetRunCount(int r)  {run_count_ = r;}
 private:

  int spill_count_;
  int run_count_;
  DAQBinaryFile binary_file_;

  UserCallback _onStartOfRunDo;
  UserCallback _onEndOfRunDo;
  UserCallback _onStartOfSpillDo;
  UserCallback _onEndOfSpillDo;

};

#endif

