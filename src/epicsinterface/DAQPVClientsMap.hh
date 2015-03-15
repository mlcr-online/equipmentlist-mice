#ifndef DAQPVCLIENTSMAP
#define DAQPVCLIENTSMAP 1

#include "equipmentList_common.hh"

class DAQPVClientsMap {
 public:
  DAQPVClientsMap() : size_(0) {};
  DAQPVClientsMap(std::string fileName);
  ~DAQPVClientsMap();

  int load(std::string fileName);
  int findByTitle(const char* pvTitle);
  int findByName(const char* pvName);
  int size() const {return size_;}
  void addPV(std::string pvName, std::string pvTitle);

  const std::string getPVName(int i) const   {return PVName_[i];}
  const std::string getPVTitle(int i) const  {return PVTitle_[i];}

  void dump();

 private:
  std::vector<std::string> PVName_;
  std::vector<std::string> PVTitle_;
  int size_;
};


#endif



