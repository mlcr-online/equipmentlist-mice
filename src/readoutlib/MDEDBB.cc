#include "MDEDBB.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(DBBParams, MDEDBB)

MDEDBB::MDEDBB() {
  SET_ALL_PARAMS_TO_ZERO(DBBParams)

  mde_name_ = "DBB";
}

bool MDEDBB::Arm() {
  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  this->getInfo();

  // Set the VRB Base Address and the Geo number of the DBB.
  std::stringstream ss;
  ss << "Geo_DBB" << (*this)["IdInVRB"] ;
  const char* vrb_baram_name = ss.str().c_str();

  vrb_.setParams("BaseAddress",   (*this)["BaseAddressVRB"])
                (vrb_baram_name,  (*this)["GEO"]);  // Geo number of the DBB.
  vrb_.updateBA();  // This is very important.

  return true;
}

bool MDEDBB::DisArm() {
  return true;
}

int MDEDBB::ReadEvent() {
  // Set the data ptr of the VRB using the data ptr of the DBB.
  // No special memory allocated for the VRB object.
  vrb_.setDataPtr( this->getDataPtr() );
  int nbRead = vrb_.GetDBBData( (*this)["IdInVRB"] );

  MESSAGESTREAM << "Data stored: " << nbRead;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return nbRead;
}

int MDEDBB::EventArrived() {
  return 0;
}

int MDEDBB::getNumDataWords() {
  int ndw = vrb_.getNumDataWords( (*this)["IdInVRB"] );
  return ndw;
}

