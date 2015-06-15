#include "VLSBMaster.hh"
#include "MiceDAQMessanger.hh"
MDEVLSBMaster *master = new MDEVLSBMaster();

void ArmVLSBMaster(char *parPtr) {
	
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	messanger->sendMessage("Arming VLSBMaster",MDE_INFO);

	/*! Get the parameter pointer */
	VLSBMaster_ParType *vmParam = (VLSBMaster_ParType*) parPtr;
	/*! Fill the master with the parameters */
	master->SetParams("BaseAddress", getBA(vmParam->baseAddr));
	/*! Call the arm (setup addresses) function */
	*(messanger->getStream()) << "Master base address " << getBA(vmParam->baseAddr);
	messanger->sendMessage(MDE_INFO);
	master->Arm();
}

void DisArmVLSBMaster(char *parPtr) {
	master->DisArm();
}

void AsynchReadVLSBMaster(char *parPtr) {}

int ReadEventVLSBMaster(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptrs, datePointer *data_ptr) {

	int dataStored = 0;
	/*! Check the DAQ Event type */
	if (header_ptr->eventType == PHYSICS_EVENT) {
                short nEvts = master->getNTriggers();
                if ( !master->processMismatch(nEvts) ) {
                      readList_error = SYNC_ERROR;
                }
                short trigger_data_tdc = master->getTriggerDataTDC();
                *data_ptr  = 0xA0000000 | (nEvts << 16 ) |  trigger_data_tdc;
		//std::cout << "Data Ptr: " << std::hex << *data_ptr << std::endl;
		std::cout << "== TriggerDataTDC ==  " << std::dec << trigger_data_tdc << std::endl;
                dataStored += 4;
		/*! Enable the vlsb master triggers */
		master->ReadEventVLSBMaster();
	}
	/*! No data returned */
	return dataStored;
}

int EventArrivedVLSBMaster(char *parPtr) {return 0;}

void PauseArmVLSBMaster(char *parPtr) {}

void PauseDisArmVLSBMaster(char *parPtr) {}
