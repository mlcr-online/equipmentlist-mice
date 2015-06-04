#include "MDEVLSBMaster.hh"
#include "MiceDAQMessanger.hh"
#include <iomanip>

bool MDEVLSBMaster::Arm() {
	controller.setup(parameters["BaseAddress"]);
	//controller.initialise("external"); /*! External trigger */
        controller.enableTrigger();
        return true;
}

int MDEVLSBMaster::ReadEventVLSBMaster() {
	controller.disableTrigger();
	return 0;
}

int MDEVLSBMaster::EventArrivedVLSBMaster() {return 0;}
bool MDEVLSBMaster::DisArmVLSBMaster() {
  return true;
}	

bool MDEVLSBMaster::DisArmVLSBMasterTrailer() {
  return true;
}

int MDEVLSBMaster::ReadEventVLSBMasterTrailer() {
	controller.enableTrigger();
	return 0;
	//get number triggers, tdc, crc
}

int MDEVLSBMaster::EventArrivedVLSBMasterTrailer() {return 0;}


void MDEVLSBMaster::SetParams(std::string name, int value) {
	parameters.insert(std::pair<std::string,int>(name,value));
}

int MDEVLSBMaster::getNTriggers() {
        mde_base_address_ = parameters["BaseAddress"];
	mde_current_address_ = mde_base_address_ + 0x20;
	mde_data_32_ = 0;

	mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

	if ( mde_vmeStatus_ != cvSuccess ) {
		mde_messanger_->sendMessage(this, "Unable to read trigger count", MDE_INFO);
		return 0;
	}

	return mde_data_32_;

}

int MDEVLSBMaster::getTriggerDataTDC() {
        mde_base_address_ = parameters["BaseAddress"];
	mde_current_address_ = mde_base_address_ + 0x34;
	mde_data_32_ = 0;

	mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

	if ( mde_vmeStatus_ != cvSuccess ) {
		mde_messanger_->sendMessage(this, "Unable to read TriggerDataTDC register", MDE_INFO);
		return 0;
	}

	return mde_data_32_;

}
