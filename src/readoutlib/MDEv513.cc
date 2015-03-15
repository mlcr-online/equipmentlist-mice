#include <vector>
#include <sstream>

#include "MDEv513.hh"
#include "V513Def.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(V513Params, MDEv513)

MDEv513::MDEv513()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V513Params)
  mde_name_ = "V513";
}

bool MDEv513::Arm() {

  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  mde_base_address_ = (*this)["BaseAddress"];

  if ( this->BoardReset()   &&
       this->SetupStrobe()  &&
       this->SetIRQLevel()   ) {

    MESSAGESTREAM << "Arming successful." << std::endl;
    mde_messanger_->sendMessage(MDE_INFO);
    return true;
  }
  mde_messanger_->sendMessage(this, "Arming failed! \n", MDE_FATAL);
  return false;
}

bool MDEv513::DisArm() {
  mde_messanger_->sendMessage(this, "DisArming successful. \n", MDE_INFO);
  return true;
}

bool MDEv513::BoardReset() {

  mde_data_16_ = 0xFFFF;
  mde_current_address_ = mde_base_address_ + V513_MODULE_RESET;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to reset the module. ", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "The module has been reset. " << std::endl;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv513::SetupStrobe() {

  short xSTBReg(0);
  if ( (*this)["STB_NegPolarity"] )
    xSTBReg |= STB_NEG_POL;

  if ( (*this)["IRQ_Enable"] )
    xSTBReg |= STB_IRQ_ENABLE;

  mde_current_address_ = mde_base_address_ + V513_STROBE_REG ;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, xSTBReg);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to set STB. ", MDE_FATAL);
    return false;
  }

  this->ReadSTBReg();
  return true;
}

bool MDEv513::ReadSTBReg() {
  mde_current_address_ = mde_base_address_ + V513_STROBE_REG ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to read STB reg. ", MDE_FATAL);
    return false;
  }
  MESSAGESTREAM << "STB has a value of " << std::hex << mde_data_16_ << std::dec ;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}


bool MDEv513::STBBitClear() {
  mde_data_16_ = 0x4;
  mde_current_address_ = mde_base_address_ + V513_CLEAR_STROBE_BIT;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to clear STB bit. ", MDE_FATAL);
    return false;
  }
  MESSAGESTREAM << "STB Bit Cleared ";
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}


bool MDEv513::SetIRQLevel() {

  short irqLevel = 0x7; //(*this)["IRQ_Level"];
  mde_current_address_ = mde_base_address_ + V513_INTERUPT_LEVEL_REG ;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, irqLevel);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to set INTERUPT Level. ", MDE_FATAL);
    return false;
  }

  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &irqLevel);
  MESSAGESTREAM << "Interupt Level set to " << std::hex << irqLevel << std::dec <<std::endl;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}


bool MDEv513::SetIRQVector() {

  short irqVect = 0x40;
  mde_current_address_ = mde_base_address_ + V513_INTERUPT_LEVEL_REG ;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, irqVect);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to set IQR Vector.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "IRQ Vector set to " << std::hex << irqVect << std::dec << std::endl;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}


bool MDEv513::IRQClear() {

  mde_data_16_ = 0x4;
  mde_current_address_ = mde_base_address_ +  V513_CLEAR_VME_INTERUPT;
  mde_vmeStatus_ = VME_WRITE_16(mde_current_address_, mde_data_16_);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to clear Interupt Request.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Interupt Request Cleared. " << std::endl;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}
