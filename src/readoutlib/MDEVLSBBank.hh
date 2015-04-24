#include "MiceDAQEquipment.hh"
#include "TrVLSBController.hh"

#ifndef MDEVLSBBANK_HH
#define MDEVLSBBANK_HH


/*! MICE Data Event class for the VLSBBank, which reads out tracker VLSB data
* Inherits from MiceDAQEquipment which contains the data pointer
* which must be set */
class MDEVLSBBank : public MiceDAQEquipment {
	private:
		/*! The controller which understands all
		* the registers in the VLSBBank */
		TrVLSBController controller;
		std::map<std::string, int> parameters;
	public:
		MDEVLSBBank();
		~MDEVLSBBank() {};
		
		/*! Calls the controller initialise which sets up addresses
		* \return True */
 		bool Arm();
		
		/*! Placeholder
		* \return True */
 		bool DisArm();

		/*! Read out the event for this bank using the in-built pointer 
		* \return numberBytesRead */
		int ReadEvent();
		
		/*! Not used - only TriggerReceiver has this function
		* \return 0 */
  		int EventArrived();

		void SetParams(std::string, int);
	
};

#endif
