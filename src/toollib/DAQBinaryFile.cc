#include <time.h>

#include "DAQBinaryFile.hh"
#include "MiceDAQMessanger.hh"
using namespace std;

DAQBinaryFile::DAQBinaryFile()
 : current_file_size_(0), current_file_name_(""), dataPtr_char_(0) {}

DAQBinaryFile::~DAQBinaryFile() {}

void DAQBinaryFile::SetDataPtr(MDE_Pointer *dataPtr) {
  dataPtr_char_ = (char*)dataPtr;

  MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
  *(messanger->getStream()) << "DAQBinaryFile:   ptr: " << (int*)dataPtr_char_;
  messanger->sendMessage(MDE_DEBUGGING);
}

bool DAQBinaryFile::Open(std::string filename, unsigned int maxTime_s) {

  MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();

  struct timeval t1, t2;
  gettimeofday(&t1, NULL);
  unsigned int diff_us=0, diff_s=0;

  while (diff_s < maxTime_s) {

    gettimeofday(&t2, NULL);
    diff_us = t2.tv_usec - t1.tv_usec;
    diff_s = t2.tv_sec - t1.tv_sec;

    file_.open(filename.c_str(), std::ifstream::binary);
    if ( file_.fail() )
      continue;

    int end;
    int curPos = file_.tellg();
    file_.seekg (0, ios::end);
    end = file_.tellg();
    current_file_size_ = end - curPos;

    if (current_file_size_ == 0) {
//       cerr << " The file is empty!" << endl;
//       remove(ss.str().c_str());
      file_.close();
      continue;
    }

    *(messanger->getStream()) << "DAQBinaryFile: file " << filename
                              << " is open (t=" << diff_s << " sec.)";
    messanger->sendMessage(MDE_DEBUGGING);
    *(messanger->getStream()) << "DAQBinaryFile: file size:" << current_file_size_;
    messanger->sendMessage(MDE_DEBUGGING);
    current_file_name_ = filename;

    return true;
  }

  *(messanger->getStream()) << "DAQBinaryFile: file " << filename
                              << " not found.";
    messanger->sendMessage(MDE_DEBUGGING);
  return false;
}

bool DAQBinaryFile::Read() {

  MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();

  if ( current_file_size_%4 != 0 ) {
    *(messanger->getStream()) << " File size is not multiple of 4. The file "
                              << current_file_name_ << " is rejected!" << endl;
    messanger->sendMessage(MDE_ERROR);
    return false;
  }

  file_.seekg(0, ios::beg); // go back to the begining ( = _curPos )
  file_.read(dataPtr_char_, current_file_size_);
  if (!file_) {
    *(messanger->getStream()) << "DAQBinaryFile: Error while reading file "
                              << current_file_name_;
    messanger->sendMessage(MDE_ERROR);
    return false;
  }

  *(messanger->getStream()) << "DAQBinaryFile: " << current_file_size_
                            << " bytes read from file " << current_file_name_;
  messanger->sendMessage(MDE_INFO);

  file_.close();
  return true;

}







