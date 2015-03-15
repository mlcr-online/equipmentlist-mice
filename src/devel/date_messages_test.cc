#include "equipmentList_common.hh"
#include "DATEIncludes.hh"
#include "DATEMessage.hh"  //doDATEMessage from here.
#include "MiceDAQMessanger.hh"
#include "MiceDAQException.hh"
#include "MiceDAQSpillStats.hh"

MiceDAQMessanger *messanger;

void MyPrint(std::stringstream &ss) {
  // This will be printed when calling getInfo()
  ss << "\n  *** This is MyPrint. *** \n\n";
}

int readList_error;
int main(int argc, char** argv) {

  // Test User Print function.
  messanger =  MiceDAQMessanger::Instance();
  MiceDAQSpillStats *spillStats= MiceDAQSpillStats::Instance();
  spillStats->setUserPrint(&MyPrint);
  spillStats->getInfo();

/*
  infoLogger_msg_xt_here(-1, LOG_INFO, INFOLOGLEVEL_DAQ, "This is info.");
  infoLogger_msg_xt_here(-1, LOG_WARNING, INFOLOGLEVEL_DAQ, "This is warning.");
  infoLogger_msg_xt_here(-1, LOG_ERROR, INFOLOGLEVEL_DAQ, "This is error!");
  infoLogger_msg_xt_here(-1, LOG_FATAL, INFOLOGLEVEL_DAQ, "This is fatal!!!");
*/

  messanger->setMessageFunc(&doDATEMessage);

  messanger->sendMessage(" DATE message test",MDE_INFO);
  messanger->sendMessage(" DATE message test",MDE_WARNING);
  messanger->sendMessage(" DATE message test",MDE_ERROR);
  messanger->sendMessage(" DATE message test",MDE_FATAL);

  return 0;
}

