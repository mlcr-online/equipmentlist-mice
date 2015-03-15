#include "DAQPVClient.hh"
#include "MiceDAQException.hh"
#include "MiceDAQMessanger.hh"

DAQPVClient::DAQPVClient(const std::string & aPVName) 
: PVName_(aPVName),
  connected_(false),
  numElements_(0){

  Connect();
}

DAQPVClient::~DAQPVClient() {
  if (connected_)
    Disconnect();
}

void DAQPVClient::Connect() {

  // Search for the channel
  PVStatus_ = ca_create_channel(PVName_.c_str(), NULL, NULL, CA_PRIORITY_DEFAULT, &channelID_);
  if( PVStatus_!= ECA_NORMAL) {
    std::string lMessage("Failed when searching channel for PV " + PVName_);
    std::string lLocation("void DAQPVClient::Connect()");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  // Send the request and wait for channel to be found
  PVStatus_ = ca_pend_io(TIMEOUT);
  if(PVStatus_ == ECA_TIMEOUT) {
    std::string lMessage("Channel *" + PVName_ + "* not found. Timeout.");
    std::string lLocation("void DAQPVClient::Connect()");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  // Get PVs native data type
  nativeType_ = ca_field_type(channelID_);

  // Get number of elements of PV
  numElements_ = ca_element_count(channelID_);
  if(numElements_ < 1) {
    std::string lMessage("PV " + PVName_ + " has no data elements");
    std::string lLocation("void DAQPVClient::Connect()");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  connected_ = true;
}

void DAQPVClient::Disconnect() {

  PVStatus_ = ca_clear_channel(channelID_);
  if(PVStatus_ != ECA_NORMAL) {
    std::string lMessage("Failed to clear channel for PV " + PVName_);
    std::string lLocation("void DAQPVClient::Disconnect()");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  connected_ = false;
}

void DAQPVClient::read(std::string &pvValue) {
  //pvValue.resize(MAX_STRING_SIZE);
  //this->getPV<char>(&pvValue[0]);
  str40 pvValue_char;
  this->getPV<char>(pvValue_char);
  pvValue = std::string(pvValue_char);
}


void DAQPVClient::write(const std::string &pvValue) {
  this->putPV<char>(&pvValue[0]);
}

void DAQPVClient::dump() {
  MiceDAQMessanger* messanger = MiceDAQMessanger::Instance();
  std::stringstream* ss = messanger->getStream();
  (*ss) << " DAQPVClient:" << "\n";
  (*ss) << "  PV name: "  << PVName_      << "\n";
  (*ss) << "  type: "     << nativeType_  << "\n";
  (*ss) << "  size: "     << numElements_ << "\n";
  messanger->sendMessage(MDE_INFO);
}










