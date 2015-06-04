#include "VLSB.hh"
#include "MiceDAQMessanger.hh"
std::vector<MDEVLSB *> vlsb (8);// = new MDEVLSB();

void ArmVLSB(char *parPtr) {
	
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();	
	messanger->sendMessage("Arming VLSB",MDE_INFO);
	/*! Get the parameter pointer */
	VLSB_ParType *vbParam = (VLSB_ParType*) parPtr;
	int geo = *vbParam->id;
       *(messanger->getStream()) << "The Geo Number " << geo;
	messanger->sendMessage(MDE_INFO);
	if (!vlsb[geo]) {
		messanger->sendMessage("Making new MDE",MDE_INFO);
		vlsb[geo] = new MDEVLSB();
	}
	messanger->sendMessage("Made the MDEVLSB",MDE_INFO);
	/*! Use it to set the parameters of the VLSB */
	vlsb[geo]->SetParams("BaseAddress", getBA(vbParam->baseAddr));
	messanger->sendMessage("Set the BA",MDE_INFO);
	/*! Call the VLSB Arm function */
	vlsb[geo]->Arm();
	messanger->sendMessage("Armed VLSB",MDE_INFO);
}

int ReadEventVLSB(char *parPtr, struct eventHeaderStruct *header_ptr, struct equipmentHeaderStruct *eq_header_ptr, datePointer *data_ptr) {
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();		

//       *(messanger->getStream()) << "Read Event VLSB ";
//	messanger->sendMessage(MDE_INFO);

	VLSB_ParType *vbParam = (VLSB_ParType*) parPtr;
	int geo = *vbParam->id;
	
	int dataStored = 0;
	/*! Cast the pointer as 32bit because that's what all the data is 
	* and give it to the VLSB */
	MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
	vlsb[geo]->setDataPtr(data_ptr_32);

//       *(messanger->getStream()) << "Cast pointer ";
//	messanger->sendMessage(MDE_INFO);

	/*! Get what stage of the spill we're in and put the VLSB into the relevent mode */
	if (header_ptr->eventType == END_OF_BURST) {
		
//       	*(messanger->getStream()) << "EOB VLSB ";
//		messanger->sendMessage(MDE_INFO);

		dataStored = vlsb[geo]->ReadEvent("ACQUIRE");
	}
	else if (header_ptr->eventType == PHYSICS_EVENT) {
		
//       	*(messanger->getStream()) << "PE VLSB ";
//		messanger->sendMessage(MDE_INFO);

		eq_header_ptr->equipmentId = geo;
		eq_header_ptr->equipmentBasicElementSize = 4;
		dataStored = vlsb[geo]->ReadEvent("READOUT");
	}else if(header_ptr->eventType == START_OF_BURST) {
		
		//dataStored = vlsb[geo]->ReadEvent("ACQUIRE");
	}
	/*! VLSBs do not return data */
	return dataStored;
}

/*! Never called */
int EventArrivedVLSB(char *parPtr) {return 0;}
void PauseArmVLSB(char *parPtr) {}
void PauseDisArmVLSB(char *parPtr) {}
void DisArmVLSB(char *parPtr) {}
void AsynchReadVLSB(char *parPtr) {}
