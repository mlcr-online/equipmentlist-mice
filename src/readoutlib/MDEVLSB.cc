#include "MDEVLSB.hh"
#include "MiceDAQMessanger.hh"
#include "VLSBDef.hh"

IMPLEMENT_FUNCTIONS(VLSBParams, MDEVLSB)

MDEVLSB::MDEVLSB()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(VLSBParams)
  mde_name_ = "VLSB";
}

// To send message:
// MESSAGESTREAM << "stuff"
// mde_messenger->sendMessage(MDE_***)

// __________________________________________________________________________________
// Arming/Disarming:

// Arm the detectors..
bool MDEVLSB::Arm()
{
  
  // Arm Message:
  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  // Set the base address & other parameters:
  mde_base_address_ = (*this)["BaseAddress"];

  // Perform Setup tasks:
  bool Status = this->DisableLVDS();
  if ( (*this)["Master"] )
    {
      Status &= this->DisableTrigger() ;
      SetupInternalTrigger(0,0);
    }

  // Read back firmware versions:
  MESSAGESTREAM << "FirmwareVersion: " << std::hex << GetFirmwareV()
		<< std::dec << std::endl;
  mde_messanger_->sendMessage(MDE_INFO);


  if ( Status )
    {
      mde_messanger_->sendMessage(this, "Arming successful. \n", MDE_INFO);
      return true;
    } 
  else 
    {
      mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
      return false;
    }
}

// Arm the detectors..
bool MDEVLSB::DisArm()
{

  bool Status = this->DisableLVDS();
  if ( (*this)["Master"] )
    {
      Status &= this->DisableTrigger() ;
    }
             

  if ( Status )
    {
      mde_messanger_->sendMessage(this,"DisArming successful. \n", MDE_INFO);
      return true;
    } 
  else 
    {
      mde_messanger_->sendMessage(this, "Unable to DisArm. \n", MDE_FATAL);
      return false;
    }

   return Status;
}

// __________________________________________________________________________________
// Aquisition enable/disable:
bool MDEVLSB::EnableAquisition()
{
  bool Status = this->EnableLVDS();
  if ( (*this)["Master"] )
    {
      Status &= this->EnableTrigger();
    }

  if ( !Status )
    {
      mde_messanger_->sendMessage(this, "Failed to enter aqusition mode\n", MDE_FATAL);
    }

  return Status;
}

bool MDEVLSB::DisableAquisition()
{
  bool Status = this->DisableLVDS();
  if ( (*this)["Master"] )
    {
      Status &= this->DisableTrigger();
    }

  if ( !Status )
    {
      mde_messanger_->sendMessage(this, "Failed to exit aqusition mode\n", MDE_FATAL);
    }

  return Status;
}


// __________________________________________________________________________________
// LVDS Enable/Disable:
bool MDEVLSB::DisableLVDS()
{
  // Write to the bus to attempt to disable the LVDS
  mde_current_address_ = mde_base_address_ + VLSB_DATA_ENABLE;
  mde_data_32_ = VLSBDataEnable::VLSB_LVDS_DISABLE;

  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );
  //std::cout << "Data Addr: " << std::hex << mde_current_address_ << std::endl;

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "The LVDS links failed to disable", MDE_FATAL);
    return false;
  }
  //mde_messanger_->sendMessage(this,"LVDS Links Disabled \n", MDE_INFO);
  return true;
}
bool MDEVLSB::EnableLVDS()
{
  // Ensure no zero suppression is set to 1:
  mde_current_address_ = mde_base_address_ + VLSB_TRIGGER_CTL;
  mde_data_32_ = VLSBTriggerCtl::VLSB_NoZeroSuppression;

	mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Failed to set no zero suppression is set to 1", MDE_FATAL);
    return false;
  }

  // Write to the bus to attempt to enable the LVDS
  mde_current_address_ = mde_base_address_ + VLSB_DATA_ENABLE;
  mde_data_32_ = VLSBDataEnable::VLSB_LVDS_ENABLE;

  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "The LVDS links failed to enable", MDE_FATAL);
    return false;
  }
  //mde_messanger_->sendMessage(this,"LVDS Links Enabled \n", MDE_INFO);
  return true;
}

// __________________________________________________________________________________
// Trigger enable / Disable 
bool MDEVLSB::DisableTrigger()
{
  mde_current_address_ = mde_base_address_ + VLSB_TRIGGER_CTL;
  mde_data_32_ = 0;

  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "The trigger failed to disable", MDE_FATAL);
    return false;
  }
  //mde_messanger_->sendMessage(this,"Trigger Disabled \n", MDE_INFO);
  return true;
}

bool MDEVLSB::EnableTrigger()
{
  mde_current_address_ = mde_base_address_ + VLSB_TRIGGER_CTL;

  // Determine mode:
  mde_data_32_ = VLSBTriggerCtl::VLSB_SpillEnable | VLSBTriggerCtl::VLSB_NoZeroSuppression;
  if ( (*this)["UseInternalTrigger"] )
    {
      // For internal, do not set ExternalTrigger Bit, Continuos Spill or Acceptence window.
      // mde_data_32_ |= 0x0;
    }
  else
    {
      mde_data_32_ |= VLSBTriggerCtl::VLSB_ContinuosSpill | VLSBTriggerCtl::VLSB_ExternalTrigger
        | VLSBTriggerCtl::VLSB_AcceptanceAllwaysOpen;
    }

  // Perform Write / Check Status:
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );
  if ( mde_vmeStatus_ != cvSuccess ) 
    {
      mde_messanger_->sendMessage(this, "The trigger failed to enable", MDE_FATAL);
      return false;
    }
  else
    {
	  //mde_messanger_->sendMessage(this,"Trigger Enabled \n", MDE_INFO);
      return true;
    }
}

bool MDEVLSB::SetupTriggerWindow(int open, int close)
{
  mde_current_address_ = mde_base_address_ + VLSB_ACC_WINDOW;
  mde_data_32_ = 0x51F01;//(windowOpen & 0x1F) + ((windowClose & 0x1F) << 8);

  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );
  
  if ( mde_vmeStatus_ != cvSuccess ) 
    {
      mde_messanger_->sendMessage(this, "Failed to set trigger window", MDE_FATAL);
      return false;
    }
  else
    {
	    mde_messanger_->sendMessage(this,"Set trigger window", MDE_INFO);
      return true;
    }
}


bool MDEVLSB::SetupSpillLength(unsigned int length)
{
  mde_current_address_ = mde_base_address_ + VLSB_SPILL_LENGTH;
  mde_data_32_ = length; // Spill Length

  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );
  
  if ( mde_vmeStatus_ != cvSuccess ) 
    {
      mde_messanger_->sendMessage(this, "Failed to set spill length", MDE_FATAL);
      return false;
    }
  else
    {
      MESSAGESTREAM << "Set spill length to: " << std::hex <<  mde_data_32_ << " ("
                    << std::dec << ((mde_data_32_ * 18.8) / 1000000.0) << "ms )" << std::endl;
      mde_messanger_->sendMessage(MDE_INFO);
      return true;
    }
}

bool MDEVLSB::SetupInternalTrigger(int period, int triggers)
{
  mde_current_address_ = mde_base_address_ + VLSB_N_GEN_TRIGGERS;

  // Bit shifting triggers by 17, since 16 results in 1/2 as many expected triggers..
  mde_data_32_ = (period & 0xFFFF) + ((triggers & 0xFFFF) << 17);

  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );

  if ( mde_vmeStatus_ != cvSuccess ) 
    {
      mde_messanger_->sendMessage(this, "Failed to setup internal trigger ", MDE_FATAL);
      return false;
    }
  else
    {
      MESSAGESTREAM << "Setup internal trigger to: " << std::hex <<  mde_data_32_ << " ("
                    << std::dec << std::endl;
      mde_messanger_->sendMessage(MDE_INFO);
      return true;
    }
}

int MDEVLSB::GetFirmwareV()
{
	mde_current_address_ = mde_base_address_ + VLSB_FIRMWARE_VERS;

	mde_data_32_ = 0;
	mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

	if ( mde_vmeStatus_ != cvSuccess ) {
		mde_messanger_->sendMessage(this, "Unable to read VLSB Firmware Version", MDE_INFO);
		return 0;
	}

	return mde_data_32_;
}

int MDEVLSB::GetTriggerCount()
{
	mde_current_address_ = mde_base_address_ + VLSB_ENCODED_TRIGGERS;
	mde_data_32_ = 0;

	mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

	if ( mde_vmeStatus_ != cvSuccess ) {
		mde_messanger_->sendMessage(this, "Unable to read trigger count", MDE_INFO);
		return 0;
	}

	return mde_data_32_;
}

int MDEVLSB::GetTriggerDataTDC()
{
	mde_current_address_ = mde_base_address_ + VLSB_TRIGGER_TDC;
	mde_data_32_ = 0;

	mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

	if ( mde_vmeStatus_ != cvSuccess ) {
		mde_messanger_->sendMessage(this, "Unable to read TriggerDataTDC register", MDE_INFO);
		return 0;
	}

	return mde_data_32_;
}
