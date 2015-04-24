#include <string>
#include <map>
#include "MiceDAQEquipment.hh"
#include "TrVLSBController.hh"

#ifndef MDEVLSB_HH
#define MDEVLSB_HH

/*! MICE data event class for the VSLB. Inherits from MiceDAQEquipment for DATE specific 
* functions, and has a TrVLSBController as a member for VLSB hardware functions */
class MDEVLSB : public MiceDAQEquipment {
	private:
		/*! The controller which understands all
		* the registers in the VLSB */
		TrVLSBController controller;
		std::map<std::string, int> parameters;

	public:
		/*! Constructor */
		MDEVLSB();

		/*! Desctructor */
		~MDEVLSB() {};
		
		/* Arm - Addresses are set up, not VME communication
		* \return True */
 		bool Arm();

		/* Disarm - nothing is done here
		* \return True */
 		bool DisArm();

		/* Spill data has closed and DAQ readout begins
		* VLSB moved from LVDS open "acquire" mode to VME readout
		* \param Mode - options are "ACQUIRE" to accept data or "READOUT" to readout
		* \return NumberOfBytesRead */
		int ReadEvent(std::string);

		/* Never called for VLSB equipment type */
  		int EventArrived();

		void SetParams(std::string, int);
	
};

#endif
