#ifndef DAQ_PV_CLIENT
#define DAQ_PV_CLIENT 1

#define TIMEOUT 1.0
//#define MAX_STRING_SIZE 256

#include <cadef.h>
#include <alarm.h>

#include "equipmentList_common.hh"

class DAQPVClient {
 public:
  DAQPVClient(const std::string & aPVName);
  ~DAQPVClient();

  // Connection functions
  void Connect();
  void Disconnect();

  // PV Status Access functions
  inline const chid   & getChID()        const {return channelID_;}
  inline const u32    & getNumElements() const {return numElements_;}
  inline const chtype & getNativeType()  const {return nativeType_;}
  inline const std::string & getPVName() const {return PVName_;}

  void dump();


  template <class dataType>
  void read(dataType &pvValue);

  template <class dataType>
  void read(std::vector<dataType> &pvValues);

  void read(std::string &pvValue);


  template <class dataType>
  void write(const dataType &pvValue);

  template <class dataType>
  void write(const std::vector<dataType> &pvValues);

  void write(const std::string &pvValue);


  template <class dataType>
  int getDataTypeCode();

 private:
  // PV name
  std::string PVName_;

  // PVClient status
  int PVStatus_;

  // Connection status
  bool connected_;

  // PV parameters
  chid    channelID_;
  u32     numElements_;
  chtype  nativeType_;
public:
  template <class dataType>
  void getPV(dataType *value);

  template <class dataType>
  void putPV(const dataType *value);

};

#include "DAQPVClient-inc.hh"

#endif

