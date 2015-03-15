#include "DAQPVClientsMap.hh"
#include "MiceDAQException.hh"
#include "MiceDAQMessanger.hh"

DAQPVClientsMap::DAQPVClientsMap(std::string fileName) {
  load(fileName);
}

DAQPVClientsMap::~DAQPVClientsMap() {

  PVName_.resize(0);
  PVTitle_.resize(0);
}

int DAQPVClientsMap::load(std::string fileName) {

  std::ifstream file;
  file.open(fileName.c_str());
  if ( !file.is_open() ) {
    std::string lMessage("Can't open file " + fileName + ".");
    std::string lLocation("DAQPVClientsMap::load(const char* fileName)");
    throw MiceDAQException(lMessage, lLocation, MDE_FATAL);
  }

  MiceDAQMessanger* messanger = MiceDAQMessanger::Instance();
  std::stringstream* ss = messanger->getStream();
  (*ss) << " DAQPVClientsMap loading file " << fileName << "\n";
  messanger->sendMessage(MDE_INFO);

  //char *name, *title;
  std::string name, title;
  bool done = false;
  while (!done) {
    file >> name >> title;

    if ( !file.eof() ) {
      PVName_.push_back( name );
      PVTitle_.push_back( title );
    } else done = true;
  }

  if (PVName_.size() != PVTitle_.size()) {
    std::string lMessage("The sizes of input vectors are not equal. Text file is probably corrupted.");
    std::string lLocation("DAQPVClientsMap::load(const char* fileName)");
    throw MiceDAQException(lMessage, lLocation, MDE_FATAL);
  }
  size_ = PVName_.size();
  return size_;
}

void DAQPVClientsMap::addPV(std::string pvName, std::string pvTitle) {
  PVName_.push_back( pvName );
  PVTitle_.push_back( pvTitle );
  size_++;
}

int DAQPVClientsMap::findByTitle(const char* title) {
  int n = PVTitle_.size();
  for (int i=0;i<n;i++) {
    if( title == PVTitle_[i] )
      return i;
  }

  std::string lMessage("No PVClient with title " + std::string(title) + " in the list of client.");
  std::string lLocation("int DAQPVClientsMap::findByTitle(const char* title)");
  throw MiceDAQException(lMessage, lLocation, MDE_FATAL);
  return 1;
}

int DAQPVClientsMap::findByName(const char* name) {
  int n = PVTitle_.size();
  for (int i=0;i<n;i++) {
    //if( strcmp(name, PVName_[i]) == 0 )
    if( name == PVName_[i] )
      return i;
  }

  std::string lMessage("No PVClient with name " + std::string(name) + " in the list of client.");
  std::string lLocation("int DAQPVClientsMap::findByName(const char* title)");
  throw MiceDAQException(lMessage, lLocation, MDE_FATAL);
}

void DAQPVClientsMap::dump() {

  MiceDAQMessanger* messanger = MiceDAQMessanger::Instance();
  std::stringstream* ss = messanger->getStream();
  (*ss) << " DAQPVClientsMap size: " << size_ << "\n";
  for (int i=0;i<size_;i++) {
    (*ss) << "PV_name: " << std::setw(30) << this->getPVName(i)  << "       PV_title: "
          << std::setw(20) << this->getPVTitle(i) << std::endl;
  }
  messanger->sendMessage(MDE_INFO);
}


