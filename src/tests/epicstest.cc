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
    epics_instance->addPV("MICE-OL-DATE-01:WFTestFloat", "TestFloat");
    epics_instance->addPV("MICE-OL-DATE-01:WFTestString", "TestString");
    //epics_instance->getDetectorNamesFromDB();
    //epics_instance->getActiveLDCs();

    if ( epics_instance->isConnected() ) {

      i32 lTmpInt_in = 109;
      i32 lTmpInt_out = 10;
      DAQPVClient *RN_Client = (*epics_instance)["RunNumber"];
      RN_Client->write(lTmpInt_in);
      //(*epics_instance)["RunNumber"]->putPV(&lTmpDouble);


      RN_Client->read(lTmpInt_out);
      std::cout << "\nafter reading " << RN_Client->getPVName()
                << ": " << lTmpInt_out << "\n\n";

      const char *lTempCh_in = "TOF1";
      str40 lTempCh_out = "blah";
      DAQPVClient *TC_CLient = (*epics_instance)["TriggerCondition"];
      TC_CLient->write(*lTempCh_in);
      TC_CLient->read(lTempCh_out);
      std::cout << "after reading " << TC_CLient->getPVName()
                <<": " << lTempCh_out << "\n\n";

//      std::string lTempStr_out1 = "blah";
//      TC_CLient->read(lTempStr_out1);
//      std::cout << lTempStr_out1 << "  size: " <<  lTempStr_out1.size() << std::endl;

#ifdef DATE_FOUND

      std::string lTempStr_in( DATE_VERSION );

#else

      std::string lTempStr_in( "7.40" );

#endif

      std::string lTempStr_out("");
      DAQPVClient *DV_Client = (*epics_instance)["DATEversion"];
      DV_Client->write(lTempStr_in);
      DV_Client->read(lTempStr_out);
      std::cout << "after reading "<< DV_Client->getPVName()
                << ": " << lTempStr_out << "\n\n";


      //const int n=10; float *test_f_in = new float[n]; for(int i=0; i<n; i++) test_f_in[i]=1+i;
      float test_f_in[10] = {12.222, 32.1, 32.2, 32.3, 22.4, 22.5, 22.6, 22.7, 22.8, 22.9};
      float test_f_out[10] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};

      DAQPVClient * TF_Client = (*epics_instance)["TestFloat"];
      TF_Client->write(*test_f_in);
      TF_Client->read(*test_f_out);
      std::cout << "after reading (array) "<<TF_Client->getPVName()
                << ": " << test_f_out[0] << ", " << test_f_out[1] << ", " << test_f_out[2]
                << ", ..., " << test_f_out[9] << "\n\n";

      std::vector<float> test_vf_in;
      test_vf_in.resize(10);
      for(int i=0;i<10;i++) test_vf_in[i] = test_f_in[i] + 1;
      TF_Client->write(test_vf_in);
      std::vector<float> test_vf_out;
      TF_Client->read(test_vf_out);
      std::cout << "after reading (std::vector) "<<TF_Client->getPVName()
                << ": " << test_vf_out[0] << ", " << test_vf_out[1] << ", " << test_vf_out[2]
                << ", ..., " << test_vf_out[9] << "\n\n";

      str40 test_str_in[10] = {"strtest0", "strtest1", "strtest2", "strtest3", "strtest4", "strtest5",
                               "strtest6", "strtest7", "strtest8", "strtest9"};
      str40 test_str_out[10] = {"blah", "blah", "blah", "blah", "blah", "blah", "blah", "blah", "blah", "blah"};
      DAQPVClient * TS_Client = (*epics_instance)["TestString"];
      TS_Client->write(*test_str_in);
      TS_Client->read(*test_str_out);
      std::cout << "after reading "<<(*epics_instance)["TestString"]->getPVName()
                << ": " << test_str_out[0] << ", " << test_str_out[1] << ", "
                << test_str_out[2] << ", ..., " << test_str_out[9] << std::endl;


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

