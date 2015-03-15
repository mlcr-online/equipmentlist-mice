#ifndef _MDEV977
#define _MDEV977 1


#define V977Params(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress)

#define MICROCONTROLER_TIMEOUT 10000

#include "MiceDAQEquipment.hh"

class MDEv977 : public MiceDAQEquipment {
 public:
  MDEv977();
  ~MDEv977() {}

  bool ArmTriggerReceiver();
  bool DisArmTriggerReceiver();

  int ReadEventTriggerReceiver();
  int EventArrivedTriggerReceiver();

  bool ArmTrailer();
  bool DisArmTrailer();

  int ReadEventTrailer();
  int EventArrivedTrailer();

  bool ArmPartTriggerSelector(std::string trCondition);
  bool DisArmPartTriggerSelector();

  bool softwareReset();

  bool setInput(short mask);
  bool unsetInput(short mask);

  bool clearOutputs();
  bool setOutput(short mask);
  bool unsetOutput(short mask);

  bool setInputMask(short mask);
  bool addInputMask(short mask);
  bool unmaksInput(short mask);

  bool setOutputMask(short mask);
  bool addOutputMask(short mask);
  bool unmaskOutput(short mask);

  const char* getEventTypeAsString();
  void dump();

 private:

  bool first_call_;
  IMPLEMENT_PARAMS(V977Params, MDEv977)
};

#endif

