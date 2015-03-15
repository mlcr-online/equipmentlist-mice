#include "MiceDAQMessanger.hh"
#include "MiceDAQEquipment.hh"
#include "MiceDAQSpillStats.hh"

#ifdef DATE_FOUND
  #include "DATEIncludes.hh"
#endif

//MiceDAQMessanger* MiceDAQMessanger::instance_ = NULL;

MiceDAQMessanger::MiceDAQMessanger() {
  messageCall_ = NULL;
  ss_ = new std::stringstream;
  verbosity_ = 1;
  throw_exception_level_ = MDE_FATAL + 1; // never
  //spillStats_ = MiceDAQSpillStats::Instance(); This is very bad idea!!!
  spillStats_ = NULL;
}

MiceDAQMessanger* MiceDAQMessanger::Instance() {
/*
  if (!instance_)
    instance_ = new MiceDAQMessanger(); // Only allow one instance of class to be generated.

  return instance_;
*/
  static MiceDAQMessanger instance;
  return &instance;
}

void MiceDAQMessanger::dumpToFile(const char* filename) {
  filestr_.open(filename);
  backup_ = std::cout.rdbuf();       // back up cout's streambuf

  psbuf_ = filestr_.rdbuf();         // get file's streambuf
  std::cout.rdbuf(psbuf_);           // assign streambuf to cout
}

std::stringstream* MiceDAQMessanger::getStream(MiceDAQEquipment* eq) {

  (*ss_).str("");

  if (eq) {
    (*ss_) << eq->getName() << "(GEO " << eq->getParam("GEO");

    if (verbosity_ == 0)
      (*ss_) << ", " << eq->PrintVMEaddr();

    (*ss_) << "): ";
  }
  return ss_;
}

void MiceDAQMessanger::setVerbosity(int v) {
  verbosity_ = v;
  this->sendMessage( "MiceDAQMessanger: Verbosity level changed to " +
                     severityToString(v), MDE_INFO);
}

void MiceDAQMessanger::setCallVerbosity(int v) {
  call_verbosity_ = v;
  this->sendMessage( "MiceDAQMessanger: User Call Verbosity level changed to " +
                     severityToString(v), MDE_INFO);
}

void MiceDAQMessanger::setThrowExceptionLevel(int el) {
  throw_exception_level_ = el;
}

std::string MiceDAQMessanger::severityToString(int severity) {
  std::string severity_str("");

  switch (severity) {
    case MDE_DEBUGGING:
      severity_str = "DEBUGGING";
      break;
    case MDE_INFO:
      severity_str = "INFO     ";
      break;
    case MDE_WARNING:
      severity_str = "WARNING! ";
      break;
    case MDE_ERROR:
      severity_str = "ERROR!!  ";
      break;
    case MDE_FATAL:
      severity_str = "FATAL!!! ";
      break;
  }

  return severity_str;
}

void MiceDAQMessanger::sendMessage(int severity) {

  this->processMessage((*ss_).str(), severity);
}

void MiceDAQMessanger::sendMessage(std::string message, int severity) {

  this->processMessage(message, severity);
}

void MiceDAQMessanger::sendMessage(MiceDAQEquipment* eq, std::string message, int severity) {

  std::stringstream full_message;
  full_message << eq->getName() << "(GEO " << eq->getParam("GEO");

  if ( verbosity_ == 0)
    full_message << ", " << eq->PrintVMEaddr();

  full_message << "): " << message;


  this->processMessage(full_message.str(), severity);
}

void MiceDAQMessanger::processMessage(std::string message, int severity) {

  if ( !spillStats_)
    spillStats_ = MiceDAQSpillStats::Instance();

  std::string full_message = severityToString(severity) + " from " + message;

  if(severity >= throw_exception_level_)
    throw MiceDAQException( full_message,
                            "MiceDAQMessanger::processMessage(std::string message, int severity)",
                            severity);

  if (severity >= verbosity_) {
    std::cout << full_message << std::endl;

    if(severity > MDE_WARNING) {
      (*spillStats_)["ErrorFlag"] ++;
      //std::cerr << full_message << std::endl;
    }
  }

  if(messageCall_ && severity >= call_verbosity_)
    (*messageCall_)(full_message, severity);
}





