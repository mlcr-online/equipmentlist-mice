#include "VLSBMasterTrailer.hh"
#include "VLSBMaster.hh"

//MDEVLSBMaster *master = new MDEVLSBMaster();
extern MDEVLSBMaster *master;

void ArmVLSBMasterTrailer(char *parPtr) {	
	VLSBMaster_ParType *vmParam = (VLSBMaster_ParType*) parPtr;
	master->setParams("BaseAddress", getBA(vmParam->BaseAddress));
	master->Arm();
}

void DisArmVLSBMasterTrailer(char *parPtr) {}
void AsynchReadVLSBMasterTrailer(char *parPtr) {}

int ReadEventVLSBMasterTrailer(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptrs, datePointer *data_ptr) {

	int dataStored = 0;
	if (header_ptr->eventType == START_OF_BURST) {
		master->ReadEventVLSBMasterTrailer();
	}
	return dataStored;
}

int EventArrivedVLSBMasterTrailer(char *parPtr) {
  return 0;
}

void PauseArmVLSBMasterTrailer(char *parPtr) {}
void PauseDisArmVLSBMasterTrailer(char *parPtr) {}
