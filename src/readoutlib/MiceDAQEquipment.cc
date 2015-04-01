#include "MiceDAQEquipment.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQException.hh"

int             MiceDAQEquipment::mde_V2718_Handle_    = 0;
eventTypeType   MiceDAQEquipment::mde_eventType_       = START_OF_RUN;
bool            MiceDAQEquipment::mde_event_is_good_   = true;
int             MiceDAQEquipment::mde_num_of_evts_     = 0;
int             MiceDAQEquipment::mde_max_num_of_evts_ = 0;

MiceDAQEquipment::MiceDAQEquipment()
: mde_base_address_(0), mde_current_address_(0), mde_vmeStatus_(0),
  mde_data_16_(0), mde_data_32_(0), mde_name_("")
  {

  mde_messanger_   = MiceDAQMessanger::Instance();
}

bool MiceDAQEquipment::checkStatus(std::string action) {

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << action << " - failed!  Status is " << cvSuccess;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, action + " - done.", MDE_INFO);
  return true;
}

bool MiceDAQEquipment::processMismatch(int nEvts) {

  if (mde_num_of_evts_ == 0) {
    // First call of this function in this spill.
    // Set mde_num_of_evts_. This is a static data member.
    mde_num_of_evts_ = nEvts;
    return true;
  }

  if ( nEvts > mde_max_num_of_evts_ ) {
    // Set the static flag.
    this->setGoodEvent(false);
    *(mde_messanger_->getStream(this)) << "Number if Particle Triggers is " << nEvts
                                       << " (>" << mde_max_num_of_evts_ << ")";
    mde_messanger_->sendMessage(MDE_FATAL);
    mde_messanger_->sendMessage(this,
                           "Too many triggers! \nReduce target dip depth or Spill Gate Width!",
                           MDE_FATAL );

    mde_messanger_->sendMessage(this, "Automatic run stop.", MDE_FATAL );
  }

  if (nEvts != mde_num_of_evts_) {
    // Set the static flag.
    this->setGoodEvent(false);
    int iMis = abs(nEvts - mde_num_of_evts_);
    std::stringstream *ss = mde_messanger_->getStream(this);
    (*ss) << "Trigger Mismatch (nEvts " << nEvts << "!=" << mde_num_of_evts_ << ").\n";
    if(iMis>1) {
      // Stop the Run.
      (*ss) << "Automatic run stop.";
      mde_messanger_->sendMessage(MDE_FATAL);

      return false;
    } else {
      (*ss) << "Ignoring the spill.";
      mde_messanger_->sendMessage(MDE_WARNING);
      return true;
    }
  }
  return true;
}

std::string MiceDAQEquipment::PrintVMEaddr() {
  std::stringstream ss;
  ss << "addr: 0x" << std::hex << mde_current_address_ << std::dec;
  return ss.str();
}


