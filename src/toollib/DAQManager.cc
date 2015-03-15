#include "MiceDAQMessanger.hh"
#include "DAQManager.hh"
#include "MDfragment.h"


DAQManager::DAQManager() :status_(0), dataPtr_(NULL), memBankSize_(0) {
  // Enable all equipments by default
  equipMap_t::iterator it = MDequipMap::begin();
  while ( it != MDequipMap::end() ){
//     cout << " Enable equipment ID " << it->first << endl;
    enableMap_[it->first] = true;
    ++it;
  }
}

DAQManager::~DAQManager() {
  equipment_arm_list_.resize(0);
  equipment_read_list_.resize(0);
  equipment_trigger_list_.resize(0);

  if (memBankSize_)
    delete[] dataPtr_;
}

void DAQManager::MemBankInit(unsigned int memsize) {

  MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
  *(messanger->getStream()) << "DAQManager: memory bank alocated (" << memsize  << " bytes)";
  messanger->sendMessage(MDE_INFO);

  memBankSize_ = memsize;
  dataPtr_ = new MDE_Pointer[memsize/sizeof(MDE_Pointer)];
  *(messanger->getStream()) << "DAQManager: data ptr: " << dataPtr_;
  messanger->sendMessage(MDE_DEBUGGING);
}


void DAQManager::addToArmList(MiceDAQEquipment* eq) {
  equipment_arm_list_.push_back(eq);
}

void DAQManager::addToReadList(MiceDAQEquipment* eq) {
  equipment_read_list_.push_back(eq);
}

void DAQManager::addToTriggerList(MiceDAQEquipment* eq) {
  equipment_trigger_list_.push_back(eq);
}

bool DAQManager::Arm() {
  int el_size = equipment_arm_list_.size();

  if (!el_size) {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream()) << "DAQManager: Arm List has 0 elements";
    messanger->sendMessage(MDE_WARNING);
    return false;
  }

  for(int i=0;i<el_size;i++)
    if ( !equipment_arm_list_[i]->Arm() )
      return false;

  return true;
}

bool DAQManager::DisArm() {
  int el_size = equipment_arm_list_.size();
  for(int i=el_size-1;i>-1;i--)
    if ( !equipment_arm_list_[i]->DisArm() )
      return false;

  return true;
}

int DAQManager::ReadEvent() {
  int nbRead = 0;
  EventHeaderStruct *evHeader = (EventHeaderStruct*)dataPtr_;
  evHeader->fragmentCount = equipment_read_list_.size();
  evHeader->eventType = 0;
  evHeader->eventId = 0;
  unsigned int eqHeaderSize = sizeof(EventHeaderStruct);
  unsigned int dataPtrSize = sizeof(MDE_Pointer);
  nbRead += eqHeaderSize;
  EqHeaderStruct *eqHeader;

  int el_size = equipment_read_list_.size();
  for(int i=0;i<el_size;i++) {
    eqHeader = (EqHeaderStruct*)(dataPtr_ + nbRead/dataPtrSize);
    equipment_read_list_[i]->setDataPtr(dataPtr_ + (eqHeaderSize + nbRead)/dataPtrSize);
//     struct timeval t1, t2;
//     double diff_t;
//     gettimeofday(&t1, NULL);

    unsigned int eqSize = equipment_read_list_[i]->ReadEvent();

//     gettimeofday(&t2, NULL);
//     diff_t = t2.tv_usec - t1.tv_usec;
//     cout << "time to ReadEvent for " << equipment_read_list_[i]->getName() << ": " << diff_t << endl;

    eqHeader->equipmentSize = eqSize + eqHeaderSize;
    eqHeader->equipmentType = MDequipMap::GetType( std::string(equipment_read_list_[i]->getName()) );

    eqHeader->equipmentId = (*equipment_read_list_[i])["GEO"];
    nbRead += eqSize + eqHeaderSize;
//     std::cout << i << " " << equipment_read_list_[i]->getName() << "    geo: " << eqHeader->equipmentId
//               << "   size: " << eqSize << "  nbr: " << nbRead << std::endl;
  }
  evHeader->eventSize = nbRead;
  return nbRead;
}

bool DAQManager::compareNTriggers(int &nTr) {
  int el_size = equipment_read_list_.size();
  nTr = equipment_read_list_[0]->getNTriggers();

  MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();

  for(int i=1;i<el_size;i++) {
    int nTr_this = equipment_read_list_[i]->getNTriggers();
    if (nTr != nTr_this) {
      *(messanger->getStream()) << "DAQManager: Trigger Mismatch in "
                                << equipment_read_list_[i]->getName() << " GEO:" << (*equipment_read_list_[i])["GEO"]
                                << " (nEvts " << nTr_this << "!=" << nTr << ").\n";
      messanger->sendMessage(MDE_ERROR);
//      return false;
    }
  }

  *(messanger->getStream()) << "DAQManager: " << nTr << " triggers received in all boards.";
  messanger->sendMessage(MDE_INFO);

  return true;
}

void DAQManager::Process() {
  int nbRead = 0;
  EventHeaderStruct *evHeader = (EventHeaderStruct*)dataPtr_;
  unsigned int eqHeaderSize = sizeof(EventHeaderStruct);
  unsigned int dataPtrSize = sizeof(MDE_Pointer);
  nbRead += eqHeaderSize;
//   std::cout << "\nevent size: " << evHeader->eventSize << "  fragment count: " << evHeader->fragmentCount << std::endl;
  EqHeaderStruct *eqHeader;
  MDfragment* myFragPtr;
  uint32_t nPart(0);

  for(unsigned int i=0;i<evHeader->fragmentCount;i++) {
    eqHeader = (EqHeaderStruct*)(dataPtr_ + nbRead/dataPtrSize);
    unsigned int eqSize = eqHeader->equipmentSize;
    unsigned int eqType = eqHeader->equipmentType;
    unsigned int eqId = eqHeader->equipmentId;
//     std::cout << "nbRead: " << nbRead << "  type: " << eqType << "  id: " << eqId << "  size: " <<  eqSize << std::endl;
    // Check if the user wants this equipment to be decoded
    if (IsEnabled(eqType)) {
        // Get a pointer to the dynamically assigned pointer from the map
      myFragPtr = MDequipMap::GetFragmentPtr(eqType);
      // Get the data pointer
      //unsigned char* myFragDataPtr = theEquipmentHeader.EquipmentDataPtr();
      // Get the data size
      // uint32_t myFragDataSize = theEquipmentHeader.EquipmentDataSize();
//       cout << " Equipment " << MDequipMap::GetName(eqType) << " found." << endl;
      if (myFragPtr && eqSize) { // the equipment is defined in the map and filled with data
        try {
          myFragPtr->SetDataPtr(dataPtr_ + (eqHeaderSize + nbRead)/dataPtrSize, eqSize - eqHeaderSize);
          myFragPtr->SetBoardID(eqId);
          nPart = myFragPtr->InitPartEventVector();
        }
        catch (MDexception lExc)
        {
          stringstream ss;
          ss << lExc.GetDescription() << endl;
          ss << "*** Unpacking exception in DAQManager::Process :"<< endl;
          ss << "while initializing the fragment";
          throw MDexception( ss.str() );
        }
        if (fragmentProc_[eqType]) {
          try {
            status_ = fragmentProc_[eqType]->Process(myFragPtr);
          }
          catch(MDexception lExc){
            stringstream ss;
            ss << lExc.GetDescription() << endl;
            ss << "*** Unpacking exception in DAQManager::Process : "<< endl;
            ss << "while processing fragment in board " << MDequipMap::GetName(eqType);
            ss << " Geo " << eqId;
            throw MDexception( ss.str() );
          }
        }
//         if (status_ != 0) {
//           stringstream ss;
//           ss << "ERROR in DAQManager::Process : "<< endl;
//           ss << "while processing fragment in board " << MDequipMap::GetName(eqType);
//           ss << " Geo " << eqId << endl;
//           ss << "The status is : " << status_;
//           throw MDexception( ss.str() );
//         }
        // loop over particle events if there are
        for (uint32_t iPart=0 ; iPart<nPart ; iPart++) {
          //_partEventNumber = iPart;
          if (partEventProc_[eqType]) {
            try {
              status_ = partEventProc_[eqType]->Process(myFragPtr->GetPartEventPtr(iPart));
            }
            catch(MDexception lExc){
              stringstream ss;
              ss << lExc.GetDescription() << endl;
              ss << "*** Unpacking exception in DAQManager::Process : "<< endl;
              ss << "while processing particle event in board ";
              ss << MDequipMap::GetName( eqType ) << " Geo " << eqId; 
              throw MDexception( ss.str() );
            }
          }
          if (status_ != 0) {
            stringstream ss;
            ss << "ERROR in DAQManager::Process : "<< endl;
            ss << "while processing particle event in board ";
            ss << MDequipMap::GetName( eqType ) << " Geo " << eqId << endl;
            ss << "The status is : " << status_;
            throw MDexception( ss.str() );
          }
        }
      }
    }

    nbRead += eqSize;
  }
}

int DAQManager::EventArrived() {
  int el_size = equipment_trigger_list_.size();
  for(int i=0;i<el_size;i++)
    if (equipment_trigger_list_[i]->EventArrived() )
      return i+1;

  return 0;
}

unsigned int DAQManager::armListSize() {
  return equipment_arm_list_.size();
}

MiceDAQEquipment* DAQManager::getArmEquipment(unsigned int eq) {
  return equipment_arm_list_[eq];
}

unsigned int DAQManager::readListSize() {
  return equipment_read_list_.size();
}

MiceDAQEquipment* DAQManager::getReadEquipment(unsigned int eq) {
  return equipment_read_list_[eq];
}

unsigned int DAQManager::triggerListSize() {
  return equipment_trigger_list_.size();
}

MiceDAQEquipment* DAQManager::getTriggerEquipment(unsigned int eq) {
  return equipment_read_list_[eq];
}

void DAQManager::SetFragmentProc(unsigned int aType,MDprocessor* aProc){
  if (MDequipMap::IsDefined(aType)){
    fragmentProc_[aType] = aProc;
    //aProc->SetProcessManager(this);
  } else {
    stringstream ss;
    ss << "WARNING in DAQManager::SetFragmentProc : Type " ;
    ss << dec << aType << " not found in the map, ignored." << endl;
    throw MDexception( ss.str() );
  }
}

void DAQManager::SetFragmentProc(string aName,MDprocessor* aProc){
  SetFragmentProc(MDequipMap::GetType(aName), aProc);
}

void DAQManager::SetPartEventProc(unsigned int aType,MDprocessor* aProc){
  if (MDequipMap::IsDefined(aType)){
    partEventProc_[aType] = aProc;
    //Proc->SetProcessManager(this);
  } else {
    stringstream ss;
    ss << "WARNING in DAQManager::SetPartEventProc : Type " ;
    ss << dec << aType << " not found in the map, ignored." << endl;
    throw MDexception( ss.str() );
  }
}

void DAQManager::SetPartEventProc(string aName,MDprocessor* aProc) {
  SetPartEventProc(MDequipMap::GetType(aName), aProc);
}

void DAQManager::DumpProcessors() {
  procMap_t::iterator it;

  cout<<"+++++++ Dump Processors +++++++"<<endl;

  for ( it=fragmentProc_.begin() ; it != fragmentProc_.end(); it++ ){
    cout << "Fragment processor for equipment " << MDequipMap::GetName((*it).first) << " is active,  ptr : "
         << (*it).second << endl;
  }
  cout << fragmentProc_.size() << " active fragment processors "<< endl;

  for ( it=partEventProc_.begin() ; it != partEventProc_.end(); it++ ){
    cout << "Particle event processor for equipment "<<  MDequipMap::GetName((*it).first) << " is active,  ptr : "
         << (*it).second <<  endl;
  }
  cout << partEventProc_.size() << " active Particle event processors "<< endl;
  cout << "+++++++++++++++++++++++++++++++" << endl;
}

