#include "MDEVLSB.hh"

/*! Constructor */
MDEVLSB::MDEVLSB() {}

bool MDEVLSB::Arm() {
	/*! Sets up the address map */
	controller.initialise(parameters["BaseAddress"]);
        
	// Set readout mode to clar fifo when spill starts and Acquire is enabled.
	controller.setReadoutMode();
        controller.setDataMode();

        return true;
}

int MDEVLSB::ReadEvent(std::string mode) {
	int dataStored = 0;	
	/*! ACQUIRE mode puts LVDS into data accept
	* should happen before any triggers arrive
	* called at START_OF_BURST DAQ event */
	if (mode == "ACQUIRE") {
		controller.setDataMode();
	}
	/*! READOUT mode stops LVDS acquire and opens VME access
	* called at end of spill gate by PHYSICS_EVENT DAQ event */
	else if (mode == "READOUT") {
		controller.setReadoutMode();
	}
	/*! VLSB doesn't return data - the banks do*/
	return dataStored;
}

int MDEVLSB::EventArrived() {
  return 0;
}

bool MDEVLSB::DisArm() {
  return true;
}

void MDEVLSB::SetParams(std::string name, int value) {
	parameters.insert(std::pair<std::string,int>(name,value));
}
