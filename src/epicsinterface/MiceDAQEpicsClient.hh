#ifndef _MICE_EPICS_CLIENT
#define _MICE_EPICS_CLIENT  1

#include "DAQPVClientsMap.hh"
#include "MiceDAQEquipment.hh"
#include "MiceDAQMessanger.hh"
#include "DAQPVClient.hh"

#ifdef DATE_FOUND

  #ifdef __cplusplus
   extern "C" {
  #endif

  #include "dateDb.h"
  #include "dbConfig.h"

  #ifdef __cplusplus
  }
  #endif

#endif

typedef std::vector<DAQPVClient*> EpicsPVList;

/*
enum DATEStatusValues{
  NO_DIIM_SERVICE=0,
  DATE_NOT_RUNNING=1,
  ARMED=2,
  TAKING_DATA=3,
  IDLE=4,
  ERROR=5,
  RUN_COMPLETE=6
};
*/

#define    DS_NO_DIIM_SERVICE   (i32)0
#define    DS_NOT_RUNNING       (i32)1
#define    DS_ARMING            (i32)2
#define    DS_ARMED             (i32)3
#define    DS_TAKING_DATA       (i32)4
#define    DS_RUN_COMPLETE      (i32)5
#define    DS_ERROR             (i32)6
#define    DS_IDLE              (i32)7

class MiceDAQEpicsClient {
 public:
  static MiceDAQEpicsClient* Instance();
  ~MiceDAQEpicsClient();

  bool Connect(std::string configFile="");
  bool Disconnect();
  bool isConnected() const {return connected_;}

  EpicsPVList         getPVList() const            {return EPICS_PVClients_;}
  DAQPVClient*        getPV(int i) const           {return EPICS_PVClients_[i];}

  int loadPVClients(const char* fileName);
  void addPV(std::string pvName, std::string pvTitle);

  void dumpPVMap()  {PVmap_.dump();}
  int FindPVByTitle(const char* pvName) {return PVmap_.findByName(pvName);}

  DAQPVClient* operator[](const char* pvTitle);

 private:
  MiceDAQEpicsClient();

  //static MiceDAQEpicsClient* instance_;

  MiceDAQMessanger *messanger_;

  EpicsPVList EPICS_PVClients_;
  DAQPVClientsMap PVmap_;

  bool connected_;

#ifdef DATE_FOUND

  char detectorNames[64][32];
  int nDetectors;

 public:
  void getDetectorNamesFromDB();
  void getActiveLDCs();
  void getActiveGDCs();

#endif
};








/*
  class MDEEpicsInterface : public MiceDAQEquipment {
   public:
    MDEEpicsInterface() {}
    virtual ~MDEEpicsInterface() {};

    bool Arm();
    bool DisArm();

    int ReadEvent();
    int EventArrived();

   private:
    MiceDAQEpicsClient *epicsClient_;
  };
*/

#endif



