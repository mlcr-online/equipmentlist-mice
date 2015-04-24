#include "VLSBMasterTriggerHandler.hh"
#include "VLSBMaster.hh"
#include "MiceDAQMessanger.hh"
//MDEVLSBMaster *master = new MDEVLSBMaster();
extern MDEVLSBMaster *master;

void ArmVLSBMasterTriggerHandler(char *parPtr) {	
	
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	messanger->sendMessage("Arming VLSB trigger handler",MDE_ERROR);
	VLSBMasterTriggerHandler_ParType *vmParam = (VLSBMasterTriggerHandler_ParType*) parPtr;	
	*(messanger->getStream()) << "Master base address " << getBA(vmParam->baseAddr);
	messanger->sendMessage(MDE_INFO);
	master->setParams("BaseAddress", getBA(vmParam->baseAddr));
	master->Arm();
}

void DisArmVLSBMasterTriggerHandler(char *parPtr) {}
void AsynchReadVLSBMasterTriggerHandler(char *parPtr) {}

int ReadEventVLSBMasterTriggerHandler(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptrs, datePointer *data_ptr) {

	int dataStored = 0;
	if (header_ptr->eventType == END_OF_BURST) {
		
		MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();		
       		*(messanger->getStream()) << "Re-enabling triggers ";
		messanger->sendMessage(MDE_INFO);	
		master->ReadEventVLSBMasterTrailer();
	}
	return dataStored;
}

int EventArrivedVLSBMasterTriggerHandler(char *parPtr) {
  return 0;
}

void PauseArmVLSBMasterTriggerHandler(char *parPtr) {}
void PauseDisArmVLSBMasterTriggerHandler(char *parPtr) {}
