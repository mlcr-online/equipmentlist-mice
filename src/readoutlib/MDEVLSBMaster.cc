#include "MDEVLSBMaster.hh"
#include "MiceDAQMessanger.hh"
#include <iomanip>

bool MDEVLSBMaster::Arm() {
	controller.setup(parameters["BaseAddress"]);
	//controller.initialise("external"); /*! External trigger */
}

int MDEVLSBMaster::ReadEventVLSBMaster() {
	controller.disableTrigger();
	return 0;
}

int MDEVLSBMaster::EventArrivedVLSBMaster() {return 0;}
bool MDEVLSBMaster::DisArmVLSBMaster() {}	

bool MDEVLSBMaster::DisArmVLSBMasterTrailer() {}

int MDEVLSBMaster::ReadEventVLSBMasterTrailer() {
	controller.enableTrigger();
	return 0;
	//get number triggers, tdc, crc
}

int MDEVLSBMaster::EventArrivedVLSBMasterTrailer() {return 0;}


void MDEVLSBMaster::SetParams(std::string name, int value) {
	parameters.insert(std::pair<std::string,int>(name,value));
}
