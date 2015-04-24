#ifndef TRVLSBMASTERCONTROLLER_HH
#define TRVLSBMASTERCONTROLLER_HH

#include <iostream>
#include "TrVLSBController.hh"

class TrVLSBMasterController : public TrVLSBController {

	private:
		std::map<std::string, int> triggerModes;
		int modeRegisterValue;
	public:
		TrVLSBMasterController();
		void setInternalSpillLength();
		void setTriggerRate(int, int);
		void setVetoPeriod(int);
		void setTriggerMode(std::string);
		void enableTrigger();
		void disableTrigger();
		int getSpillDone();
		int initialise(std::string, int, int);
		int setup(int);
		void setTriggerWindowSettings(int, int);
		
};

#endif
