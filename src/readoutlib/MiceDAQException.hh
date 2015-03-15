#ifndef MICEDAQEXCEPTION
#define MICEDAQEXCEPTION 1

#include "equipmentList_common.hh"

class MiceDAQException {
 public:
  MiceDAQException(const std::string &ErrorDescr = "", 
                   const std::string &ErrorLocation = "",
                   const int Svr = 3 );

  // Default destructor has nothing to do
  // except be declared virtual and non-throwing.
  virtual ~MiceDAQException() throw() {}

  std::string GetDescription() const {return ">>> " + errorDescription_ + " <<<";}
  std::string GetLocation()    const {return "ERROR in " + errorLocation_;}
  int getSeverit()             const {return severity_;}


 private:
  std::string errorDescription_;
  std::string errorLocation_;
  int severity_;
};

#endif

