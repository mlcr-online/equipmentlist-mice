#include "DATEMessage.hh"
#include "DATEIncludes.hh"


void doFatalError() {
  if ( readList_error == 0 )
    readList_error = GENERIC_ERROR;
  //std::cout << "*** doFatalError " << readList_error << " ***" <<std::endl;
}

void doDATEMessage(std::string message, int severity) {

  switch (severity) {
    case MDE_INFO:
      infoLogger_msg_xt_here(-1, LOG_INFO, INFOLOGLEVEL_DAQ, message.c_str());
      break;

    case MDE_WARNING:
      infoLogger_msg_xt_here(-1, LOG_WARNING, INFOLOGLEVEL_DAQ, message.c_str());
      break;

    case MDE_ERROR:
      infoLogger_msg_xt_here(-1, LOG_ERROR, INFOLOGLEVEL_DAQ, message.c_str());
      break;

    case MDE_FATAL:
      infoLogger_msg_xt_here(-1, LOG_FATAL, INFOLOGLEVEL_DAQ, message.c_str());
      doFatalError();
      break;
  }
}

static std::string role_name;
static bool role_set = false;
void doHostNamePrint(std::stringstream &ss) {
  if(!role_set)
    role_name = getenv("DATE_ROLE_NAME");

  ss << " Role: " << role_name << ";  ";
}

