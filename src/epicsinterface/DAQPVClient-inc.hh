#ifndef DAQ_PV_CLIENT_INC
#define DAQ_PV_CLIENT_INC 1

#include <typeinfo>
#include "MiceDAQException.hh"
#include "MiceDAQMessanger.hh"

template <class dataType>
int DAQPVClient::getDataTypeCode() {
  int typeCode = -1;

  if( typeid(dataType) == typeid(char)  ||
      typeid(dataType) == typeid(char*) ||
      typeid(dataType) == typeid(str40) )
    typeCode = DBR_STRING;

  if( typeid(dataType) == typeid(bool))
    typeCode = DBR_ENUM;

  if( typeid(dataType) == typeid(u8) ||
      typeid(dataType) == typeid(i8) )
    typeCode = DBR_CHAR;

  if( typeid(dataType) == typeid(u16) ||
      typeid(dataType) == typeid(i16) )
    typeCode = DBR_SHORT;

  if( typeid(dataType) == typeid(u32) ||
      typeid(dataType) == typeid(i32) )
    typeCode = DBR_LONG;

  if( typeid(dataType) == typeid(float) )
    typeCode = DBR_FLOAT;

  if( typeid(dataType) == typeid(double) )
    typeCode = DBR_DOUBLE;

  if( typeCode == -1 ) {
    std::string lMessage( "Unsupported data type " + std::string( typeid(dataType).name() ) + " used with PV " + PVName_ + ".");
    std::string lLocation("int DAQPVClient::getDataTypeCode()");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  return typeCode;
}

template <class dataType>
void DAQPVClient::read(dataType &pvValue) {

  this->getPV<dataType>(&pvValue);
}

template <class dataType>
void DAQPVClient::read(std::vector<dataType> &pvValues) {
  if(pvValues.size() != 0) {
    MiceDAQMessanger* messanger = MiceDAQMessanger::Instance();
    std::stringstream* ss = messanger->getStream();
    (*ss) << " DAQPVClient: in read(std::vector<dataType>&) size of the input vector is not 0. \n";
    (*ss) << "   Going to resizet to 0!";
    messanger->sendMessage(MDE_WARNING);
    pvValues.resize(0);
  }

  dataType tmp[numElements_];
  this->getPV(tmp);
  pvValues.assign(tmp, tmp+numElements_);
}

template <class dataType>
void DAQPVClient::write(const dataType &pvValue) {

  this->putPV<dataType>(&pvValue);
}

template <class dataType>
void DAQPVClient::write(const std::vector<dataType> &pvValues) {
  if(pvValues.size() != numElements_) {
    std::string lMessage( "Size of the input vector is not equal to the native size for PV " + PVName_);
    std::string lLocation("void AQPVClient::write(const std::vector<dataType> &pvValue)");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  this->putPV(&pvValues[0]);
}

template <class dataType>
void DAQPVClient::getPV(dataType *valuePtr) {

  int typeCode = this->getDataTypeCode<dataType>();
  if (typeCode != nativeType_) {
    std::stringstream ss;
    ss << "Data type mismatch when getting the value of " << PVName_ << ". <<<\n"
       << ">>> Used type is " << typeCode
       << ". Native PV data type is " << nativeType_ <<".";
    std::string lMessage( ss.str() );
    std::string lLocation("void DAQPVClient::getPV(dataType &valuePtr)");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  PVStatus_ = ca_array_get( nativeType_,
                            numElements_,
                            channelID_,
                            const_cast<dataType *>(valuePtr) );

  if(PVStatus_ != ECA_NORMAL) {
    std::string lMessage("Error in ca_array_get for PV " + PVName_ + ": " + ca_message(PVStatus_));
    std::string lLocation("void DAQPVClient::getPV(dataType &valuePtr)");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  // Process get
  PVStatus_ = ca_pend_io(TIMEOUT);
  if(PVStatus_ != ECA_NORMAL) {
    std::string lMessage("Read timeout for PV " + PVName_);
    std::string lLocation("void DAQPVClient::getPV(dataType &valuePtr)");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }
}

template <class dataType>
void DAQPVClient::putPV(const dataType *valuePtr) {
  /*
  MiceDAQMessanger* messanger = MiceDAQMessanger::Instance();
  std::stringstream* ss = messanger->getStream();
  (*ss) << " DAQPVClient putPV: \n  name: " << getPVName() << "\n  value: " << *valuePtr;
  messanger->sendMessage(MDE_INFO);
  */
  int typeCode = this->getDataTypeCode<dataType>();
  if (typeCode != nativeType_) {
    std::stringstream ss;
    ss << "Data type mismatch when setting the value of " << PVName_ << ". <<<\n"
       << ">>> Used type is " << typeCode
       << ". Native PV data type is " << nativeType_ <<".";
    std::string lMessage( ss.str() );
    std::string lLocation("void DAQPVClient::putPV(dataType &valuePtr)");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }

  //Put the value
  PVStatus_ = ca_array_put(nativeType_, numElements_, channelID_, valuePtr);
  if(PVStatus_ != ECA_NORMAL) {
    std::string lMessage("Error in ca_array_put for PV " + PVName_ + ": " + ca_message(PVStatus_));
    std::string lLocation("void DAQPVClient::putPV(dataType valuePtr)");
  }

  // Process put
  PVStatus_ = ca_pend_io(TIMEOUT);
  if(PVStatus_ != ECA_NORMAL) {
    std::string lMessage("Write timeout for PV " + PVName_);
    std::string lLocation("void DAQPVClient::putPV(dataType valuePtr)");
    throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
  }
}

#endif

