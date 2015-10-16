#include "VLSBBank.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

std::vector<MDEVLSBBank *> bank (64);// = new MDEVLSBBank();

void ArmVLSBBank(char *parPtr) {
	
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	messanger->sendMessage("Arming VLSBBank",MDE_INFO);
	/*! Get the parameter pointer */
	VLSBBank_ParType *vbParam = (VLSBBank_ParType*) parPtr;
	/*! Fill the parameters */
	int geo = (*vbParam->VLSBid*4) + *vbParam->BankNum;
	
	if (!bank[geo]) {
		messanger->sendMessage("Making new MDE BANK",MDE_INFO);
		bank[geo] = new MDEVLSBBank();
	}

	messanger->sendMessage("Setting params....",MDE_INFO);

        bank[geo]->setParams("GEO", geo);
	bank[geo]->setParams("BaseAddress", getBA(vbParam->VLSBaddr));
	//bank[geo]->setParams("BankNumber", int(*vbParam->BankNum));
  	bank[geo]->setParams("VLSBid", *vbParam->VLSBid);
  	bank[geo]->setParams("VLSBBank", *vbParam->BankNum);

        messanger->sendMessage("Calling arm...",MDE_INFO);
	/*! Call the MDEVLSBBank Arm */
	bank[geo]->Arm();
        messanger->sendMessage("..Done",MDE_INFO);
}

int ReadEventVLSBBank(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {

	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();		
//       *(messanger->getStream()) << "Read Event VLSBBank ";
	//messanger->sendMessage(MDE_INFO);

	VLSBBank_ParType *vbParam = (VLSBBank_ParType*) parPtr;
	int geo = (*vbParam->VLSBid*4) + *vbParam->BankNum;

	int dataStored = 0;
	/*! Cast the pointer as 32bit because that's what all the data is */
	MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
	/*! Give the data pointer to the VLSBBank */
	bank[geo]->setDataPtr(data_ptr_32);
	/*! Only readout at end of spill gate */
	if (header_ptr->eventType == PHYSICS_EVENT) {
		/*! Call the read bank function from the MDEVLSBBank class */
		eq_header_ptr->equipmentId = geo;
		eq_header_ptr->equipmentBasicElementSize = 4;
		
		dataStored += bank[geo]->ReadEvent();
		

		// DO NOT USE, FIFOOK not reliable.
		// Check the status of the Bank FIFO, where errors
		// between the front end board and buffer board are flagged.
		// There is a simple boolean check for "OK-ness":
		//if (!bank[geo]->FIFOOK())
		//  {
		//    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
		//    messanger->sendMessage("FIFO not reported as OK, deleting bank data", MDE_ERROR);
		//    bank[geo]->setGoodEvent(false);
		//    return 0;
		//  }
		
		/* // Uncomment to destroy data in damaged spills
		// Check that the data is OK, if not - trash the data:
		if( !bank[geo]->isGoodEvent() )
		  {
		  // No Return no data. Data buffers are automatically
		  // cleared when the LVDS links are enabled.
		  return 0;
		  }
		*/
	}
	/*! Returns the number of bytes read - need to increment data pointer accordinly
	* All values are 32bit so must increment by dataStored/4 not sizeof(int) */

        ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;

	return dataStored*4; // note code returns 32 bit entries, not bytes.
}

/*! Nothing is ever done here */
int EventArrivedVLSBBank(char *parPtr) {return 0;}
void PauseArmVLSBBank(char *parPtr) {}
void PauseDisArmVLSBBank(char *parPtr) {}

void DisArmVLSBBank(char *parPtr) {}
void AsynchReadVLSBBank(char *parPtr) {}
