#include "MDEv1718.hh"
#include "MiceDAQMessanger.hh"

MDEv1718::MDEv1718()
: MDEv2718::MDEv2718() {
  SET_ALL_PARAMS_TO_ZERO(V2718Params)
  mde_name_ = "V1718";
}

bool MDEv1718::Arm() {
  //Initialize V1718
  mde_messanger_->sendMessage(this, "Going to arm a board with following parameters:", MDE_INFO);
  getInfo();

  if (this->Init() && this->Configure()) {
    mde_messanger_->sendMessage(this, "Arming successful. \n", MDE_INFO);
    return true;
  }

  mde_messanger_->sendMessage(this," Arming failed. \n", MDE_FATAL);
  return false;
}

bool MDEv1718::Init() {
  mde_vmeStatus_ = CAENVME_Init( cvV1718,
                                 0,
                                 this->getParam("BNumber"),
                                 &mde_V2718_Handle_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to initialize. Status: " << mde_vmeStatus_;
    mde_messanger_->sendMessage(MDE_FATAL);
    mde_messanger_->sendMessage(this, "Check that the crate is ON and the controller is connected.",
                                  MDE_FATAL);
    return false;
  }

  return true;
}

