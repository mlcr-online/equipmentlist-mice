#ifndef TRVLSBCONTROLLER_HH
#define TRVLSBCONTROLLER_HH

#include <map>
#include <vector>
#include "TrVMEDevice.hh"
#include "equipmentList_common.hh"
class TrVLSBController : public TrVMEDevice {

	protected:
		/* Common VLSB parameters */
		std::map<std::string, int> vlsbParameters;
		std::map<std::string, int> addresses;
		std::map<std::string, int> triggerSetting;
		std::map<std::string, int> lvdsSetting;
		std::map<int, std::string> fifoStatus;

		/* Specific values and variables */
		std::vector<int> bankLengths;
		int id;
 	public:
 		TrVLSBController();
		void initialise(int);
		void setReadoutMode();
		void setDataMode();
		void getBankLengths();
		int readBank(int, int, MDE_Pointer*);
		int moveData(int);
		int* readAllBanks();
		int getTotalDataVolume();
		int verifyBankSpill(int*, int, int);
		int calculateCRC(int*, int);
		int getID();	
};

#endif
