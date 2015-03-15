#include "MiceDAQRunStats.hh"
#include "MiceDAQMessanger.hh"

//MiceDAQRunStats* MiceDAQRunStats::instance_ = NULL;

IMPLEMENT_FUNCTIONS(RunStatsParams, MiceDAQRunStats)

MiceDAQRunStats::MiceDAQRunStats() {
  SET_ALL_PARAMS_TO_ZERO(RunStatsParams)

  mde_messanger_   = MiceDAQMessanger::Instance();
}

MiceDAQRunStats* MiceDAQRunStats::Instance() {
/*
  if(!instance_) {
    instance_ = new MiceDAQRunStats();
  }

  return instance_;
*/
  static MiceDAQRunStats instance;
  return &instance;
}

MiceDAQRunStats::~MiceDAQRunStats() {
//  delete instance_;
}

int& MiceDAQRunStats::operator[](const char* pName) {
  return getParam(pName);
}

void MiceDAQRunStats::printDataRecorded() {
  double dataSize = double( (*this)["DataRecorded"])/1048576.;
  *(mde_messanger_->getStream()) << "MiceDAQRunStats:\n Data recorded: "
                                 << int(1e2*dataSize)/100. << " M.";
  mde_messanger_->sendMessage(MDE_INFO);
}

