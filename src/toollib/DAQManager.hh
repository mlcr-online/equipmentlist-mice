#ifndef _DAQMANAGER_
#define _DAQMANAGER_ 1

#include "MiceDAQEquipment.hh"
#include "MDequipMap.h"
#include "MDprocessor.h"

typedef unsigned int   SizeType;
typedef unsigned int   TypeType;
typedef unsigned int   IdType;
typedef unsigned int   CountType;

typedef map<uint32_t,MDprocessor*> procMap_t;
typedef map<uint32_t,bool> enableMap_t;

struct EqHeaderStruct {
  SizeType             equipmentSize;
  TypeType             equipmentType;
  IdType               equipmentId;
};

struct EventHeaderStruct {
  SizeType             eventSize;
  TypeType             eventType;
  IdType               eventId;
  CountType            fragmentCount;
};
class DAQManager {

 public:
  DAQManager();
  virtual ~DAQManager();

  bool Arm();
  bool DisArm();
  int  ReadEvent();
  int  EventArrived();

  void Process();
  bool compareNTriggers(int &nTr);

  void setDataPtr(MDE_Pointer *p)        {dataPtr_ = p;}
  MDE_Pointer* getDataPtr() const        {return dataPtr_;}

  void addToArmList(MiceDAQEquipment* eq);
  void addToReadList(MiceDAQEquipment* eq);
  void addToTriggerList(MiceDAQEquipment* eq);

  unsigned int armListSize();
  MiceDAQEquipment* getArmEquipment(unsigned int eq);

  unsigned int readListSize();
  MiceDAQEquipment* getReadEquipment(unsigned int eq);

  unsigned int triggerListSize();
  MiceDAQEquipment* getTriggerEquipment(unsigned int eq);

  void SetFragmentProc(unsigned int aType,MDprocessor* aProc);
  void SetFragmentProc(string aName,MDprocessor* aProc);
  void SetPartEventProc(unsigned int aType,MDprocessor* aProc);
  void SetPartEventProc(string aName,MDprocessor* aProc);

  void Disable(unsigned int aType)    { enableMap_[aType] = false; }
  void Enable(unsigned int aType)     { enableMap_[aType] = true; }
  void Disable(string aName)          { Disable(MDequipMap::GetType(aName)); }
  void Enable(string aName)           { Enable(MDequipMap::GetType(aName)); }
  bool IsEnabled(unsigned int aType)  { return enableMap_[aType]; }
  bool IsEnabled(string aName)        { return IsEnabled(MDequipMap::GetType(aName)); }

  void MemBankInit(unsigned int memsize);

  void     DumpProcessors();

 private:

  int      status_;

  MDE_Pointer   *dataPtr_;
  unsigned int  memBankSize_;
  procMap_t     fragmentProc_;
  procMap_t     partEventProc_;

  enableMap_t   enableMap_;

  std::vector<MiceDAQEquipment*>    equipment_arm_list_;
  std::vector<MiceDAQEquipment*>    equipment_read_list_;
  std::vector<MiceDAQEquipment*>    equipment_trigger_list_;
};

#endif


