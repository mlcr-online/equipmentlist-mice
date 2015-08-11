#include "VLSBBank.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

std::vector<MDEVLSBBank *> bank (64);// = new MDEVLSBBank();

void ArmVLSBBank(char *parPtr) {
	
//	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	//messanger->sendMessage("Arming VLSBBank",MDE_INFO);
	/*! Get the parameter pointer */
	VLSBBank_ParType *vbParam = (VLSBBank_ParType*) parPtr;
	/*! Fill the parameters */
	int geo = (*vbParam->VLSBid*4) + *vbParam->BankNum;
	
	if (!bank[geo]) {
	//	messanger->sendMessage("Making new MDE BANK",MDE_INFO);
		bank[geo] = new MDEVLSBBank();
	}
	bank[geo]->SetParams("BaseAddress", getBA(vbParam->VLSBaddr));
	bank[geo]->SetParams("BankNumber", int(*vbParam->BankNum));



	
	/*! Call the MDEVLSBBank Arm */
	bank[geo]->Arm();
}

int ReadEventVLSBBank(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {

	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();		
//       *(messanger->getStream()) << "Read Event VLSBBank ";
	//messanger->sendMessage(MDE_INFO);

	VLSBBank_ParType *vbParam = (VLSBBank_ParType*) parPtr;
	int geo = (*vbParam->VLSBid*4) + *vbParam->BankNum;

//	std::cout << " ==== geo: " << geo << std::endl;

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
		
       	//	*(messanger->getStream()) << "About to MDEBank ReadEvent ";
	//	messanger->sendMessage(MDE_INFO);

		dataStored += bank[geo]->ReadEvent();
		
       	//	*(messanger->getStream()) << "MDEBank ReadEvent ";
	//	messanger->sendMessage(MDE_INFO);

//           *(messanger->getStream()) << "Bank " << geo << " read " << dataStored << " words of data";
//           messanger->sendMessage(MDE_INFO);

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
