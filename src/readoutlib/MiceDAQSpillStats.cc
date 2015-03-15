#include "MiceDAQSpillStats.hh"
#include "MiceDAQMessanger.hh"

//MiceDAQSpillStats* MiceDAQSpillStats::instance_ = NULL;

IMPLEMENT_FUNCTIONS(SpillStatsParams, MiceDAQSpillStats)

MiceDAQSpillStats::MiceDAQSpillStats() {
  SET_ALL_PARAMS_TO_ZERO(SpillStatsParams)

  mde_messanger_   = MiceDAQMessanger::Instance();
  (*this)["SpillNumber"] = -1;
}

MiceDAQSpillStats* MiceDAQSpillStats::Instance() {
/*
  if(!instance_) {
    instance_ = new MiceDAQSpillStats();
  }

  return instance_;
*/
  static MiceDAQSpillStats instance;
  return &instance;
}

MiceDAQSpillStats::~MiceDAQSpillStats() {
//  delete instance_;
}

int& MiceDAQSpillStats::operator[](const char* pName) {
  return getParam(pName);
}

