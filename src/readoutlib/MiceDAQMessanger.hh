#ifndef MICEDAQMESSANGER
#define MICEDAQMESSANGER 1

#include "equipmentList_common.hh"

class MiceDAQEquipment;
class MiceDAQSpillStats;

class MiceDAQMessanger {
 public:
  static MiceDAQMessanger* Instance();
  virtual ~MiceDAQMessanger() {};

  void setVerbosity(int v);
  int  getVerbosity() {return verbosity_;}
  void setThrowExceptionLevel(int el);

  void sendMessage(MiceDAQEquipment* eq, std::string message, int severity);
  void sendMessage(std::string message, int severity);
  void sendMessage(int severity);

  void processMessage(std::string message, int severity);

  std::stringstream* getStream(MiceDAQEquipment* eq=NULL);
  std::string severityToString(int severity);

  void dumpToFile(const char* filename="readout_log.txt");
  void setMessageFunc(MessageCall msFunc)   { messageCall_= msFunc;}
  void setCallVerbosity(int v);

 private:
  MiceDAQMessanger();
  //static MiceDAQMessanger* instance_;

  MiceDAQSpillStats *spillStats_;
  std::streambuf *psbuf_, *backup_;
  std::ofstream filestr_;

  std::stringstream* ss_;
  int verbosity_;
  int throw_exception_level_;

  MessageCall messageCall_;
  int call_verbosity_;
};

#endif


