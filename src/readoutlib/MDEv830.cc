#include "MDEv830.hh"
#include "V830Def.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(V830Params, MDEv830)

MDEv830::MDEv830()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V830Params)
  mde_name_ = "V830";
}

int MDEv830::getMaxMemUsed(int nEvents) {
  int memPerEvent = ( (*this)["UseHeader"] + bitCount( (*this)["ChannelMask"] ) )*4;

  return memPerEvent*nEvents + 1;
}

bool MDEv830::Arm() {

  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  mde_base_address_ = (*this)["BaseAddress"];
  mde_data_32_=0;

  if ( this->boardReset() &&
       this->setGEO( (*this)["GEO"] ) &&
       this->channelsEnable((*this)["ChannelMask"]) &&
       this->setTriggerMode() &&
       this->setUseHeader() &&
       this->setDataFormat() &&
       this->setAutoReset()) {
    //Get the contents of the Control Register
    mde_current_address_ = mde_base_address_ + V830_CONTROL_REGISTER;
    int control_reg;
    mde_vmeStatus_ = VME_READ_16( mde_current_address_, &control_reg );

    MESSAGESTREAM << "Arming successful. Control reg: " << control_reg << std::endl;
    mde_messanger_->sendMessage(MDE_INFO);
    return true;
  } else {
    mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
    return false;
  }
}

bool MDEv830::DisArm() {

  mde_messanger_->sendMessage(this, "DisArming successful. \n", MDE_INFO);
  return true;
}

int MDEv830::ReadEvent() {
  short status;
  short nEvt = 0;
  short nWords;
  int dataStored = 0;
  int nbread = 0;
  mde_base_address_ = (*this)["BaseAddress"];

  int nChannelsSet = bitCount( (*this)["ChannelMask"] );
  //read the status register.
  mde_current_address_ = mde_base_address_ + V830_STATUS_REGISTER;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &status );

  if( status & V830_DReady ){
    //Get the number of the words per event.
    nWords = nChannelsSet + (*this)["UseHeader"];
    //Get the number of the events.
    mde_current_address_ = mde_base_address_ + V830_MEB_EVENT_NUMBER;
    mde_vmeStatus_ = VME_READ_16( mde_current_address_, &nEvt );

    MESSAGESTREAM << "Events in V830 MEB: "<< nEvt << ", each of size " << nWords << ",  nCh: " << nChannelsSet;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
    if (nEvt != 1)
      mde_messanger_->sendMessage(this, "More than 1 Event in V830 MEB", MDE_WARNING);

    //Readout the MEB
    mde_current_address_ = mde_base_address_ + V830_MEB;//Equals BA
    while( nEvt-- ) {
      mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, mde_dataPtr_, nWords * 4, &nbread );
      dataStored += nbread;
      nTriggers_ = mde_dataPtr_[ (*this)["UseHeader"] ]& 0x3FFFFFF;
      MESSAGESTREAM << "Nunber of triggers: " << nTriggers_;
      mde_messanger_->sendMessage(MDE_DEBUGGING);
    }
  }

  MESSAGESTREAM << "Data stored: " << dataStored;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return dataStored;
}

int MDEv830::EventArrived() {

  return 0;
}

int MDEv830::getCount(int chNum) {
  int n_word = chNum + (*this)["UseHeader"];
  int dataWord = mde_dataPtr_[n_word];
  int ch_data = (dataWord & 0xF8000000) >> 27;
  if (ch_data != chNum) {
    mde_messanger_->sendMessage(this, "Error in MDEv830::getCount. Channel number doesn't match.", MDE_ERROR);
    return -1;
  }
  return (dataWord & 0x3ffffff);
}

bool MDEv830::softwareTrigger() {
  mde_data_32_ = 1;
  mde_current_address_ = mde_base_address_ + V830_SOFTWARE_TRIGGER;
  mde_vmeStatus_ |= VME_WRITE_16(mde_current_address_, mde_data_32_);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to generate software trigger.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software Trigger generated.", MDE_INFO);
  return true;
}

bool MDEv830::softwareClear() {
  mde_data_32_ = 1;
  mde_current_address_ = mde_base_address_ + V830_SOFTWARE_CLEAR;
  mde_vmeStatus_ |= VME_WRITE_16(mde_current_address_, mde_data_32_);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do software clear.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software clear done.", MDE_DEBUGGING);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

// PRIVATE METHODS

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::boardReset() {
  mde_current_address_ = mde_base_address_ + V830_MODULE_RESET;
  mde_data_32_=0;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_32_ );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "The board failed to reset.", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "Reset done.", MDE_DEBUGGING);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::setGEO(int geo) {
  mde_current_address_ = mde_base_address_ + V830_GEO_REGISTER;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, geo );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Geo Address.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "GEO set to " << geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::channelsEnable(int mask) {

  mde_current_address_ = mde_base_address_ + V830_CHANNEL_ENABLE;
  mde_vmeStatus_ |= VME_WRITE_32( mde_current_address_, mask );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to apply channel mask.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Channel mask set to " << std::hex << mask << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::setTriggerMode() {
  int tmp = (*this)["TriggerMode"];
  if ( tmp ) {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_SET;
  } else {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_CLEAR;
  }
  mde_data_32_ = V830_TriggerRandom;
  mde_vmeStatus_ = VME_WRITE_16(mde_current_address_ , mde_data_32_ );
  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set Trigger mode.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Trigger mode set to " << tmp;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::setUseHeader() {
  int tmp = (*this)["UseHeader"];
  if ( tmp ) {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_SET;
  } else {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_CLEAR;
  }
  mde_data_32_ = V830_ControlHeaderEnable;
  mde_vmeStatus_ = VME_WRITE_16(mde_current_address_ , mde_data_32_ );
  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set Use header.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Use header set to " << tmp;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::setDataFormat() {
  int tmp = (*this)["UseNarrow"];
  std::string format;
  if ( tmp) {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_SET;
    format = "26 bit.";
  } else {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_CLEAR;
    format = "32 bit.";
  }
  mde_data_32_ = V830_ControlDataFormat;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_32_ );
  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set Data format.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Data format set to " << tmp;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv830::setAutoReset() {

  int tmp = (*this)["AutoReset"];
  if (tmp) {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_SET;
  } else {
    mde_current_address_ = mde_base_address_ + V830_CONTROL_BIT_CLEAR;
  }
  mde_data_32_ = V830_ControlAutoReset;
  mde_vmeStatus_ = VME_WRITE_16(mde_current_address_ , mde_data_32_ );
  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set AutoReset.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "AutoReset set to " << tmp;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}






