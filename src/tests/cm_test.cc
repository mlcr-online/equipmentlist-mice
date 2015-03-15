#include "MiceDAQEpicsClient.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQRunStats.hh"
#include "MiceDAQSpillStats.hh"
#include "equipmentList_common.hh"

int main(int argc, char** argv) {

  try {

    MiceDAQMessanger   *messanger = MiceDAQMessanger::Instance();
    messanger->setVerbosity(2);

    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    //epics_instance->dumpPVMap();
    epics_instance->Connect("../src/epicsinterface/DAQPVClientsMap.txt");

    if ( epics_instance->isConnected() ) {

      DAQPVClient *DATEStateClient = (*epics_instance)["DATEState"];
      DATEStateClient->write(DS_NOT_RUNNING);
      u32 ds=22;
      DATEStateClient->read(ds);
      std::cout << "\n1. DATE State is now: " << ds << std::endl;
      sleep(2);

      //////////////////////////////////////////////////////////////////////////

      DATEStateClient->write(DS_ARMING);

      u32 lTmpInt = 4444;
      (*epics_instance)["RunNumber"]->read(lTmpInt);

      DATEStateClient->read(ds);
      std::cout << "2. DATE State is now: " << ds << std::endl;
      sleep(2);

      //////////////////////////////////////////////////////////////////////////

      DATEStateClient->write(DS_ARMED);
      DATEStateClient->read(ds);
      std::cout << "3. DATE State is now: " << ds << std::endl;
      sleep(2);

      //////////////////////////////////////////////////////////////////////////

      int n_daq_events=10;
      int i=0;
      while (i<n_daq_events) {

        DATEStateClient->write(DS_TAKING_DATA);
        if (i==0) {
          double lTmpDouble = 3333.33;
          (*epics_instance)["SpillGate"]->write(lTmpDouble);
        }

        lTmpInt = i;
        (*epics_instance)["DAQEventCount"]->write(lTmpInt);

        for(int chNum=0; chNum<12; chNum++) {
          std::stringstream ss;
          ss << "V830Ch" << chNum;
          lTmpInt = i*10 + chNum;
          const char* pvName = ss.str().c_str();
          (*epics_instance)[pvName]->write(lTmpInt);
        }

        lTmpInt = 2000 + i*55;
        (*epics_instance)["EvtSize-miceacq07"]->write(lTmpInt);
        lTmpInt -= 33;
        (*epics_instance)["EvtSize-miceacq10"]->write(lTmpInt);

        DATEStateClient->read(ds);
        std::cout << "4." << i << " DATE State is now: " << ds << std::endl;
        sleep(2);
        i++;
      }

      //////////////////////////////////////////////////////////////////////////

      sleep(2);
      DATEStateClient->write(DS_RUN_COMPLETE);
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

