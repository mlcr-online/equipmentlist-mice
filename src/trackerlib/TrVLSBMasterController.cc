#include "TrVLSBMasterController.hh"
#include <iostream>
#include <time.h>

TrVLSBMasterController::TrVLSBMasterController() {}

int TrVLSBMasterController::setup(int baseAddress) {

	TrVLSBController::initialise(baseAddress);
	triggerModes.insert( std::pair<std::string, int> ("internal",0) );
	triggerModes.insert( std::pair<std::string, int> ("external",1) );
	triggerModes.insert( std::pair<std::string, int> ("internalSuppressed",2) );
	triggerModes.insert( std::pair<std::string, int> ("externalSuppressed",3) );
	return 0;
}

int TrVLSBMasterController::initialise(std::string triggerMode, int numberTriggers = 0, int triggerPeriod = 0) {
	
	//if (triggerModes[triggerMode]) {
	//	setTriggerWindowSettings(0x0, 0xF);
	//}
	//if (! triggerModes[triggerMode]) {
	//setInternalSpillLength();
	//setTriggerRate(numberTriggers, triggerPeriod);
	//}
	//setTriggerMode(triggerMode);	
	return 0;
}

void TrVLSBMasterController::setTriggerMode(std::string mode) {

	try {
		switch(triggerModes[mode]) {

			case 0: // internal triggers
				modeRegisterValue = triggerSetting["internal"];
				modeRegisterValue |= triggerSetting["noZeroSuppression"];
				break;
			case 1: // external
				modeRegisterValue = triggerSetting["external"];
				modeRegisterValue |= triggerSetting["noZeroSuppression"];	
				modeRegisterValue |= triggerSetting["noVeto"];
				modeRegisterValue |= triggerSetting["continuousSpill"];
				break;
			case 2:
				modeRegisterValue = triggerSetting["internal"];
				break;
			case 3:	
				modeRegisterValue = triggerSetting["external"];	
				modeRegisterValue |= triggerSetting["noVeto"];
				modeRegisterValue |= triggerSetting["continuousSpill"];
				break;
			default:
				break; // How did this happen?
		}
	}
	catch(...) {
		std::cout << "Trigger mode exception," << std::endl;
		std::cout << "Fixing by defaulting to internal mode setting" << std::endl;
		modeRegisterValue = triggerSetting["internal"];
	}
	
}

void TrVLSBMasterController::enableTrigger() {
	//modeRegisterValue |= triggerSetting["spillEnable"];
	//modeRegisterValue |= 0x80; // ignore VLSB veto
	modeRegisterValue = 0xF4; // External trigger
	int result = CAENVME_WriteCycle(0, baseAddress + addresses["control"], &modeRegisterValue, addressModifier, dataWidth);

//	std::cout << result << " enable" << std::endl;
//	std::cout << std::hex << (baseAddress + addresses["control"]) << " " << modeRegisterValue << std::endl;
}

void TrVLSBMasterController::disableTrigger() {
//        std::cout << std::hex << (baseAddress + addresses["control"]) << " " << triggerSetting["spillDisable"] << std::endl;
	CAENVME_WriteCycle(0, baseAddress + addresses["control"], &triggerSetting["spillDisable"], addressModifier, dataWidth);
}

void TrVLSBMasterController::setTriggerWindowSettings(int windowOpen, int windowClose) {
	int windowRegisterValue = 0x51F01;//(windowOpen & 0x1F) + ((windowClose & 0x1F) << 8);
	CAENVME_WriteCycle(0, baseAddress + addresses["vetoSettings"], &windowRegisterValue, addressModifier, dataWidth);
}

void TrVLSBMasterController::setInternalSpillLength() {
	int spillLength = 0xFFFF;	
	std::cout << "Setting spill length with " << std::hex << spillLength << std::endl;
	std::cout << "Equal to " << ((spillLength * 18.8) / 1000000.0) << "ms" << std::endl;
	CAENVME_WriteCycle(0, baseAddress + addresses["spillDuration"], &spillLength, addressModifier, dataWidth);
	}
	

void TrVLSBMasterController::setTriggerRate(int triggers, int period) {
	triggers = 10;
	period = 5000;
	int triggerRateRegisterValue = (period & 0xFFFF) + ((triggers & 0xFFFF) << 16);
	std::cout << "Setting trigger rate with " << std::hex << triggerRateRegisterValue << std::endl;
	CAENVME_WriteCycle(0, baseAddress + addresses["triggerSettings"], &triggerRateRegisterValue, addressModifier, dataWidth);
}

int TrVLSBMasterController::getSpillDone() {
	int done(0);
	int triggers(0);
	
	std::cout << clock() << std::endl;
	int fifo(0);
	//while (!done) {
		//CAENVME_ReadCycle(0, baseAddress + addresses["control"], &done, addressModifier, dataWidth);
		CAENVME_ReadCycle(0, baseAddress + addresses["fifoStatus"], &fifo, addressModifier, dataWidth);
		//std::cout << "Done reg " << done << std::endl;
		std::cout << "fifo " << fifo << std::endl;
		//done &= 0x8;
		//done = (done << 4); // Ugly - retrieve bit correspondong to spill done and shift it
	//}
	sleep(1);
	CAENVME_ReadCycle(0, baseAddress + addresses["recordedTriggers"], &triggers, addressModifier, dataWidth);
	std::cout << triggers << " triggers sent" << std::endl;
	
	done = 1;
	std::cout << "Spill done" << std::endl;
	std::cout << clock() << std::endl;
	return done;
}
