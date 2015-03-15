#include "MiceDAQEpicsClient.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQRunStats.hh"
#include "MiceDAQSpillStats.hh"
#include "equipmentList_common.hh"

int main(int argc, char** argv) {

  try {

    MiceDAQMessanger   *messanger = MiceDAQMessanger::Instance();
    messanger->setVerbosity(0);

    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    //epics_instance->dumpPVMap();
    epics_instance->Connect();

    if ( epics_instance->isConnected() ) {


      DAQPVClient *DATEStateClient = (*epics_instance)["DATEState"];

      i32 ds=22;
      //DATEStateClient->write(ds);
      DATEStateClient->write(DS_NOT_RUNNING);
      sleep(2);
      DATEStateClient->read(ds);
      std::cout << "\n1. DATE State is now: " << ds << std::endl;

      DATEStateClient->write(DS_ARMED);
      sleep(2);
      DATEStateClient->read(ds);
      std::cout << "2. DATE State is now: " << ds << std::endl;

      DATEStateClient->write(DS_TAKING_DATA);
      sleep(2);
      DATEStateClient->read(ds);
      std::cout << "3. DATE State is now: " << ds << std::endl;

      sleep(20);
      DATEStateClient->read(ds);
      std::cout << "4. DATE State is now: " << ds << std::endl;
      DATEStateClient->write(DS_RUN_COMPLETE);
      sleep(2);
      DATEStateClient->read(ds);
      std::cout << "5. DATE State is now: " << ds << "\n";

      std::cout << "\nend.\n";
    }

  } catch(MiceDAQException e) {
    std::cout << e.GetLocation() << std::endl;
    std::cout << e.GetDescription() << std::endl;
  }

  return 0;
}

