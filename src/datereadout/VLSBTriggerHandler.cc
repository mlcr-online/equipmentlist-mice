#include "VLSBMasterTriggerHandler.hh"
#include "VLSBMaster.hh"
#include "MiceDAQMessanger.hh"
//MDEVLSBMaster *master = new MDEVLSBMaster();
//extern MDEVLSBMaster *master;

void ArmVLSBMasterTriggerHandler(char *parPtr) {	
	
	//MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	//messanger->sendMessage("Arming VLSB trigger handler",MDE_INFO);
	//VLSBMasterTriggerHandler_ParType *vmParam = (VLSBMasterTriggerHandler_ParType*) parPtr;
	//master->setParams("BaseAddress", getBA(vmParam->baseAddr));
	//master->Arm();
}

void DisArmVLSBMasterTriggerHandler(char *parPtr) {}
void AsynchReadVLSBMasterTriggerHandler(char *parPtr) {}

int ReadEventVLSBMasterTriggerHandler(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptrs, datePointer *data_ptr) {

	//int dataStored = 0;
	//if (header_ptr->eventType == START_OF_BURST) {
	//	master->ReadEventVLSBMasterTrailer();
	//}
	//return dataStored;
	return 0;
}

int EventArrivedVLSBMasterTriggerHandler(char *parPtr) {
  return 0;
}

void PauseArmVLSBMasterTriggerHandler(char *parPtr) {}
void PauseDisArmVLSBMasterTriggerHandler(char *parPtr) {}
