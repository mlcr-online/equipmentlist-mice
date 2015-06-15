#include "MDEVLSBBank.hh"
#include "MiceDAQMessanger.hh"

MDEVLSBBank::MDEVLSBBank() {}

bool MDEVLSBBank::Arm() {
	//controller = TrVLSBController();
	controller.initialise(parameters["BaseAddress"]);
        return true;
}

int MDEVLSBBank::ReadEvent() {
	controller.getBankLengths();
	
	MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();		

//       *(messanger->getStream()) << "Got bank lengths ";
//	messanger->sendMessage(MDE_INFO);

	//int *ptr = static_cast<int*>(mde_dataPtr_);
		
//       *(messanger->getStream()) << "Reading bank ";
//	messanger->sendMessage(MDE_INFO);
	//controller.getBankLengths()
	int dataStored = controller.readBank(parameters["BankNumber"],0,mde_dataPtr_);
	//mde_dataPtr_ = static_cast<MDE_DataPtr*>(ptr);

	if (parameters["BankNumber"] == 0)
	  {
	    *(messanger->getStream()) << "Board:0x" << std::hex << parameters["BaseAddress"]
				      << " DataLength:" << std::dec << dataStored;
	    messanger->sendMessage(MDE_INFO);
	  }

//       *(messanger->getStream()) << "Read bank ";
//	messanger->sendMessage(MDE_INFO);

	return dataStored;
}

int MDEVLSBBank::EventArrived() {
  return 0;
}
bool MDEVLSBBank::DisArm() {
  return true;
}

void MDEVLSBBank::SetParams(std::string name, int value) {
	parameters.insert(std::pair<std::string,int>(name,value));
}
