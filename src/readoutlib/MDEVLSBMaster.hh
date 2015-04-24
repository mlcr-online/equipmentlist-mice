#include "MiceDAQEquipment.hh"
#include "TrVLSBMasterController.hh"
#include <map>

#ifndef MDEVLSBMASTER
#define MDEVLSBMASTER

class MDEVLSBMaster : public MiceDAQEquipment {
	private:
		/*! The controller which understands all
		* the registers in the VLSBMaster */
		TrVLSBMasterController controller;
		std::map<std::string, int> parameters;

	public:
		MDEVLSBMaster() {};
		~MDEVLSBMaster() {};
		
 		bool Arm();
 		bool DisArmVLSBMaster();
		int ReadEventVLSBMaster();
  		int EventArrivedVLSBMaster();
	
  		bool DisArmVLSBMasterTrailer();
  		int ReadEventVLSBMasterTrailer();
  		int EventArrivedVLSBMasterTrailer();

		void SetParams(std::string, int value);

};

#endif
