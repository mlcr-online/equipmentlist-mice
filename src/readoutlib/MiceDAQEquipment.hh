#ifndef _MICE_DAQ_EQUIPMENT
#define _MICE_DAQ_EQUIPMENT  1

#include "config.hh"
#include "equipmentList_common.hh"
#include "MiceDAQParamHolder.hh"

class MiceDAQMessanger;

class MiceDAQEquipment : public MiceDAQParamHolder {
 public:
  MiceDAQEquipment();
  virtual ~MiceDAQEquipment() {}

  virtual bool Arm()                     {return false;} //=0;
  virtual bool DisArm()                  {return false;} //=0;

  virtual int ReadEvent()                {return 0;} //=0;
  virtual int EventArrived()             {return 0;} //=0;

  virtual int getNTriggers()             {return 0;} //=0;

  void setDataPtr(MDE_Pointer *p)        {mde_dataPtr_ = p;}
  MDE_Pointer* getDataPtr() const        {return mde_dataPtr_;}

  std::string getName() const            {return mde_name_;}
  void updateBA()                        {mde_base_address_ = (*this)["BaseAddress"];}
  int getEventType() const               {return mde_eventType_;}
  void setEventType(eventTypeType et)    {mde_eventType_ = et;}

  bool checkStatus(std::string action);
  void setGoodEvent(bool eventStatus)    {mde_event_is_good_ = eventStatus;}
  void setNumOfEvts(int nEvts)           {mde_num_of_evts_ = nEvts;}
  int  getNumOfEvts() const              {return mde_num_of_evts_;}
  void setMaxNumOfEvts(int nEvtsMax)     {mde_max_num_of_evts_ = nEvtsMax;}
  int  getMaxNumOfEvts() const           {return mde_max_num_of_evts_;}
  bool isGoodEvent() const               {return mde_event_is_good_;}
  bool processMismatch(int nEvts);
  static int getHandle()                 {return mde_V2718_Handle_;}

  std::string PrintVMEaddr();

 protected:

  MiceDAQMessanger       *mde_messanger_;

  MDE_Pointer            *mde_dataPtr_;

  unsigned int            mde_base_address_;
  unsigned int            mde_current_address_;
  int                     mde_vmeStatus_;
  unsigned char           mde_data_8_;
  short                   mde_data_16_;
  int                     mde_data_32_;
  std::string             mde_name_;
  static int              mde_V2718_Handle_;
  static eventTypeType    mde_eventType_;
  static bool             mde_event_is_good_;
  static int              mde_num_of_evts_;
  static int              mde_max_num_of_evts_;
};

#endif


