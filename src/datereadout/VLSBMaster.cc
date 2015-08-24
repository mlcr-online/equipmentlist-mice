#include "VLSB.hh"
#include "VLSBMaster.hh"
#include "MiceDAQMessanger.hh"
MDEVLSB *master = new MDEVLSB();

void ArmVLSBMaster(char *parPtr) {
	
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	messanger->sendMessage("Arming VLSBMaster",MDE_INFO);

	/*! Get the parameter pointer */
	VLSBMaster_ParType *vmParam = (VLSBMaster_ParType*) parPtr;
	/*! Fill the master with the parameters */
        master->setParams("GEO", 0);
	master->setParams("BaseAddress", getBA(vmParam->baseAddr));
        master->setParams("VLSBid", 0);
        master->setParams("Master", 1);
        master->setParams("UseInternalTrigger", 0);
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
                short nEvts = master->GetTriggerCount();
                if ( !master->processMismatch(nEvts) ) {
                      readList_error = SYNC_ERROR;
                }
                short trigger_data_tdc = master->GetTriggerDataTDC();
                *data_ptr  = 0xA0000000 | (nEvts << 16 ) |  trigger_data_tdc;
		//std::cout << "Data Ptr: " << std::hex << *data_ptr << std::endl;
		std::cout << "== TriggerDataTDC ==  " << std::dec << trigger_data_tdc << std::endl;
                dataStored += 4;
		/*! Disable the vlsb master triggers */
		master->DisableTrigger();
	}

	else if (header_ptr->eventType == START_OF_BURST) {
		master->EnableTrigger();
	}

	return dataStored;
}

int EventArrivedVLSBMaster(char *parPtr) {return 0;}

void PauseArmVLSBMaster(char *parPtr) {}

void PauseDisArmVLSBMaster(char *parPtr) {}
