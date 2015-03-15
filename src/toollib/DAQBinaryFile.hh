#ifndef _DAQBINARYFILE_
#define _DAQBINARYFILE_ 1

#include "equipmentList_common.hh"

class DAQBinaryFile {

 public:
  DAQBinaryFile();
  virtual ~DAQBinaryFile();

  void SetDataPtr(MDE_Pointer *dataPtr);
  bool Open(std::string filename, unsigned int maxTime);
  bool Read();

 private:

  std::ifstream file_;
  unsigned int current_file_size_;
  std::string current_file_name_;
  char* dataPtr_char_;
};

#endif


