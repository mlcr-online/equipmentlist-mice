#include <time.h>

#include "MDprocessManager.h"

#include "MDEv2718.hh"
#include "MDEv977.hh"
#include "MDEvRB.hh"
#include "MDEDBB.hh"
#include "MDEv1731.hh"
#include "DAQManager.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQRunStats.hh"
#include "MiceDAQSpillStats.hh"
#include "equipmentList_common.hh"

void addvrb(DAQManager &myDAQ,MDEvRB &vrb,MDEDBB &dbb1,MDEDBB &dbb2,
                                          MDEDBB &dbb3,MDEDBB &dbb4,
                                          MDEDBB &dbb5,MDEDBB &dbb6,
                                          int vrbBA,int vrbgeo,int geodbb1);

time_t rawtime;

MiceDAQRunStats   runStats   = *MiceDAQRunStats::Instance();
MiceDAQSpillStats spillStats = *MiceDAQSpillStats::Instance();

using namespace std;
void dumpEventVRB(MDE_Pointer *data, int nbr);

int main(int argc, char** argv) {

  int argii = 1;
  string help_str;

  int nEvents = 10;
  if (argc > argii ) {
    stringstream ss;
    ss << argv[argii];
    ss >> nEvents;
  }
  help_str += " nEvents";
  argii++;

  int spillPeriod = 1000000; // in micro seconds
  if (argc > argii ) {
    spillPeriod = atoi(argv[argii]);
  }
  help_str += " spillPeriod";
  argii++;

  int dbbDebugMode = 1; // otherwise data is saved in files
  if (argc > argii ) {
    dbbDebugMode = atoi(argv[argii]);
  }
  help_str += " dbbDebugMode";
  argii++;

  int vrb1BA = 0x55ff0000;
  if (argc > argii ) {
    vrb1BA = getBA(argv[argii]);
  }
  help_str += " vrb1BA";
  argii++;

  int firstDbbId = 0;
  if (argc > argii ) {
    firstDbbId = atoi(argv[argii]);
  }
  help_str += " firstDbbId";
  argii++;

  int testDbbId = 1;
  if (argc > argii ) {
    testDbbId = atoi(argv[argii]);
  }
  help_str += " testDbbId";
  argii++;

  if (argc==2 && std::string(argv[1]) == "--help") {
    printf("\n\nusage: %s%s\n\n\n",argv[0],help_str.data());
    return 1;
  } else if (dbbDebugMode==1 && argc!=argii) {
    printf("\n\n ERROR: not enough arguments\n\n\n");
    return 0;
  } else if (dbbDebugMode==0 && argc!=4) {
    printf("\n\n ERROR: not enough arguments\n\n\n");
    return 0;
  }

  MiceDAQMessanger::Instance()->setVerbosity(3);
  MiceDAQMessanger::Instance()->setThrowExceptionLevel(4);
  MDprocessManager proc_manager;

  try {
    DAQManager myDAQ;

/////////////////////////////////////////////////////////////////////////////////////////

    MDEv2718 controler;
    controler.setParams("BNumber",    0)
                       ("BLink",      0)
                       ("MaxNumEvts", 100);

   myDAQ.addToArmList(&controler);

/////////////////////////////////////////////////////////////////////////////////////////

    MDEvRB vrb1,vrb2,vrb3,vrb4,vrb5,vrb6,vrb7,vrb8;
    MDEDBB dbb1, dbb2, dbb3, dbb4, dbb5, dbb6;
    MDEDBB dbb7, dbb8, dbb9, dbb10,dbb11,dbb12;
    MDEDBB dbb13,dbb14,dbb15,dbb16,dbb17,dbb18;
    MDEDBB dbb19,dbb20,dbb21,dbb22,dbb23,dbb24;
    MDEDBB dbb25,dbb26,dbb27,dbb28,dbb29,dbb30;
    MDEDBB dbb31,dbb32,dbb33,dbb34,dbb35,dbb36;
    MDEDBB dbb37,dbb38,dbb39,dbb40,dbb41,dbb42;
    MDEDBB dbb43,dbb44,dbb45,dbb46,dbb47,dbb48;

  if (dbbDebugMode) {

    int mountNdbbs = 1;
    if (firstDbbId!=testDbbId) mountNdbbs=2;
    vrb1.setParams("GEO",               1)
                  ("BaseAddress",       vrb1BA)
                  ("DBB_FV",            8)
                  ("N_DBBs",            mountNdbbs)
                  ("ChannelMask0_DBB1", 0xffffffff)
                  ("ChannelMask1_DBB1", 0xffffffff)
                  ("Geo_DBB1",          firstDbbId)
                  ("ChannelMask0_DBB2", 0xffffffff)
                  ("ChannelMask1_DBB2", 0xffffffff)
                  ("Geo_DBB2",          testDbbId);
    dbb1.setParams("GEO",             firstDbbId)
                  ("BaseAddressVRB",  vrb1BA)
                  ("IdInVRB",         1);
    dbb2.setParams("GEO",             testDbbId)
                  ("BaseAddressVRB",  vrb1BA)
                  ("IdInVRB",         2);

    myDAQ.addToArmList(&vrb1);
    myDAQ.addToReadList(&vrb1);

    myDAQ.addToArmList(&dbb1);
    myDAQ.addToReadList(&dbb1);

    myDAQ.addToArmList(&dbb2);
    myDAQ.addToReadList(&dbb2);

  } else {

    addvrb(myDAQ,vrb1,dbb1, dbb2, dbb3, dbb4, dbb5, dbb6, 0x11ff0000,1,0);
    addvrb(myDAQ,vrb2,dbb7, dbb8, dbb9, dbb10,dbb11,dbb12,0x22ff0000,2,6);
    addvrb(myDAQ,vrb3,dbb13,dbb14,dbb15,dbb16,dbb17,dbb18,0x33ff0000,3,12);
    addvrb(myDAQ,vrb4,dbb19,dbb20,dbb21,dbb22,dbb23,dbb24,0x44ff0000,4,18);
    addvrb(myDAQ,vrb5,dbb25,dbb26,dbb27,dbb28,dbb29,dbb30,0x55ff0000,5,24);
    addvrb(myDAQ,vrb6,dbb31,dbb32,dbb33,dbb34,dbb35,dbb36,0x66ff0000,6,30);
    addvrb(myDAQ,vrb7,dbb37,dbb38,dbb39,dbb40,dbb41,dbb42,0x77ff0000,7,36);
    addvrb(myDAQ,vrb8,dbb43,dbb44,dbb45,dbb46,dbb47,dbb48,0x88ff0000,8,42);

  }

/////////////////////////////////////////////////////////////////////////////////////////


    MDEv1731 fadc1;
    fadc1.setParams("GEO",                    21)
                   ("BaseAddress",            0xE00E0000)
                   ("PostTriggerOffset",      96)//32)
                   ("BufferOrganizationCode", 0xA)
                   ("BlockTransfEventNum",    1024)//512)
                   ("ChannelMask",            0xffff)
                   ("TriggerOverlapping",     1)
                   ("WordsPerEvent",          384)//128)
                   ("UseSoftwareTrigger",     0)
                   ("UseExternalTrigger",     1)
                   ("DualEdgeSampling",       0)
                   ("ZSThreshold",            2);

    MDEv1731 fadc2(fadc1);
    fadc2.setParams("GEO",                    22)
                   ("BaseAddress",            0xE00F0000);

    MDEv1731 fadc3(fadc1);
    fadc3.setParams("GEO",                    23)
                   ("BaseAddress",            0xE0100000);

    MDEv1731 fadc4(fadc1);
    fadc4.setParams("GEO",                    24)
                   ("BaseAddress",            0xE0110000);

    MDEv1731 fadc5(fadc1);
    fadc5.setParams("GEO",                    25)
                   ("BaseAddress",            0xE0120000);

    MDEv1731 fadc6(fadc1);
    fadc6.setParams("GEO",                    26)
                   ("BaseAddress",            0xE0130000);

    myDAQ.addToArmList(&fadc1);
    myDAQ.addToReadList(&fadc1);

    myDAQ.addToArmList(&fadc2);
    myDAQ.addToReadList(&fadc2);

    myDAQ.addToArmList(&fadc3);
    myDAQ.addToReadList(&fadc3);

    myDAQ.addToArmList(&fadc4);
    myDAQ.addToReadList(&fadc4);

    myDAQ.addToArmList(&fadc5);
    myDAQ.addToReadList(&fadc5);

    myDAQ.addToArmList(&fadc6);
    myDAQ.addToReadList(&fadc6);


/////////////////////////////////////////////////////////////////////////////////////////

    if ( !myDAQ.Arm() )
      return false;

/////////////////////////////////////////////////////////////////////////////////////////

    MDEv977 ioReg;
    ioReg.setParams("GEO", 3)("BaseAddress", 0xe0020000);

    if ( !ioReg.ArmTriggerReceiver() )
      return false;

    if ( !ioReg.ArmPartTriggerSelector("GVA1") )
      return false;

    if ( !ioReg.ArmTrailer() )
      return false;

/////////////////////////////////////////////////////////////////////////////////////////

    int mem_size = 1024*1024;
    MDE_Pointer *dataPtr = new MDE_Pointer[mem_size];
    std::cout << "Memory alocated: " << mem_size << std::endl;
    myDAQ.setDataPtr(dataPtr);
    vrb1.setDataPtr(dataPtr);

    char hn[64];
    hn[63] = '\0';
    gethostname(hn, sizeof(hn));
    int size = std::string(hn).find(".");
    std::string hostname(hn, size);

    usleep(500000);


    int spillCount=0, eventCount=0;

//     struct timeval t1, t2;
//     gettimeofday(&t1, NULL);

    while (spillCount<nEvents) {
      ioReg.setOutput(0x2000);
      ioReg.unsetOutput(0x2000);
      cout << "\n \nSpill gate " << spillCount+1 << " generated ..." << endl;

      usleep(spillPeriod);

      stringstream ss;
      ss << "../data_tmp/" << eventCount << "_" << hostname << ".data";

      if (dbbDebugMode) {
        int dumpDbbIdInVRB=1;
        if (firstDbbId!=testDbbId) dumpDbbIdInVRB=2;
        bool data_OK = vrb1.StartDBBReadout(dumpDbbIdInVRB);
        if (data_OK) {
          int nbr = vrb1.GetDBBData(dumpDbbIdInVRB);
          dumpEventVRB(dataPtr, nbr);
        }
      } else {
//       gettimeofday(&t1, NULL);
      int ntr = fadc1.getNEventsStored();
      int nbr = myDAQ.ReadEvent();
      cout << "Number of Triggers: "<< ntr << endl;

//       gettimeofday(&t2, NULL);
//       double diff_t;
//       diff_t = t2.tv_usec - t1.tv_usec;
//       cout << "time myDAQ.ReadEvent(): " << diff_t << endl;

        if (ntr) {
          eventCount++;
          std::ofstream outfile (ss.str().c_str(), std::ofstream::binary);
          outfile.write( (char*)dataPtr, nbr);
          outfile.close();
          cout << "Event " << eventCount << " is recorded (" << nbr << " bytes)\n";
        }
      }
      spillCount++;
    }

    myDAQ.DisArm();
    delete dataPtr;

  } catch (MiceDAQException e) {
    cout << "DAQ exception"    << endl;
    cout << e.GetLocation()    << endl;
    cout << e.GetDescription() << endl;
    return 0;
  }
  catch ( MDexception & e) {
    cout << "Unpacking exception" << endl;
    cout <<  e.GetDescription()   << endl;
    return 0;
  }
  catch(std::exception & e) {
    cout << "Standard exception" << endl;
    cout << e.what() << endl;
    return 0;
  }

  return 1;
}

void dumpEventVRB(MDE_Pointer *data, int nbr) 
{
  int type = (*data & 0xf0000000) >> 28;
  int id   = (*data & 0x0fc00000) >> 22;
  int hit  = *data & 0x1fff;
//   int zero = (*data & 0x3f0000) >> 16;
  cout << "----------------------------------------------------------------" << endl;
  cout << "header type: "<< hex << type << dec << "    board id: " << id << "    spill number: " << hit << endl;
  data++;
  int tr_count = (*data & 0x3ff0000) >> 16;
  int h_count  = *data & 0xffff;
  int hc = (nbr/4) - 4;
  cout << "trigger count: "<< tr_count << "    hit count:" << h_count << " (" << hc << ")" << endl;
  cout << "----------------------------------------------------------------" << endl;
  data++;
  for (int i=0; i<hc; i++) {
   type = (*data & 0xf0000000) >> 28;
   id   = (*data & 0x0fc00000) >> 22;
   hit  = *data & 0x3fffff;
   cout << i << "    hit type: " << hex << type << dec << "    ch id: " << id << "    hit time: " << hit << endl;
   data++;
  }
  type    = (*data & 0xf0000000) >> 28;
  id      = (*data & 0x0fc00000) >> 22;
  int status  = (*data & 0x3f0000) >> 16;
  hit      = *data & 0xffff;
  cout << "----------------------------------------------------------------" << endl;
  cout << "trailer type: " << hex << type   << dec << "    board id: "     << id
       << "    status: "   << hex << status << dec << "    spill number: " << hit << endl;
  data++;
  cout << "spill width: " << *data << endl;
  cout << "----------------------------------------------------------------" << endl << endl;
}

void addvrb(DAQManager &myDAQ,
            MDEvRB &vrb,
            MDEDBB &dbb1,
            MDEDBB &dbb2,
            MDEDBB &dbb3,
            MDEDBB &dbb4,
            MDEDBB &dbb5,
            MDEDBB &dbb6,
            int vrbBA,
            int vrbgeo,
            int geodbb1) 
{
     vrb.setParams("GEO",               vrbgeo)
                  ("BaseAddress",       vrbBA)
                  ("DBB_FV",            8)
                  ("N_DBBs",            6)
                  ("ChannelMask0_DBB1", 0xffffffff)
                  ("ChannelMask1_DBB1", 0xffffffff)
                  ("Geo_DBB1",          geodbb1)
                  ("ChannelMask0_DBB2", 0xffffffff)
                  ("ChannelMask1_DBB2", 0xffffffff)
                  ("Geo_DBB2",          geodbb1+1)
                  ("ChannelMask0_DBB3", 0xffffffff)
                  ("ChannelMask1_DBB3", 0xffffffff)
                  ("Geo_DBB3",          geodbb1+2)
                  ("ChannelMask0_DBB4", 0xffffffff)
                  ("ChannelMask1_DBB4", 0xffffffff)
                  ("Geo_DBB4",          geodbb1+3)
                  ("ChannelMask0_DBB5", 0xffffffff)
                  ("ChannelMask1_DBB5", 0xffffffff)
                  ("Geo_DBB5",          geodbb1+4)
                  ("ChannelMask0_DBB6", 0xffffffff)
                  ("ChannelMask1_DBB6", 0xffffffff)
                  ("Geo_DBB6",          geodbb1+5);
    dbb1.setParams("GEO",             geodbb1)
                  ("BaseAddressVRB",  vrbBA)
                  ("IdInVRB",         1);
    dbb2.setParams("GEO",             geodbb1+1)
                  ("BaseAddressVRB",  vrbBA)
                  ("IdInVRB",         2);
    dbb3.setParams("GEO",             geodbb1+2)
                  ("BaseAddressVRB",  vrbBA)
                  ("IdInVRB",         3);
    dbb4.setParams("GEO",             geodbb1+3)
                  ("BaseAddressVRB",  vrbBA)
                  ("IdInVRB",         4);
    dbb5.setParams("GEO",             geodbb1+4)
                  ("BaseAddressVRB",  vrbBA)
                  ("IdInVRB",         5);
    dbb6.setParams("GEO",             geodbb1+5)
                  ("BaseAddressVRB",  vrbBA)
                  ("IdInVRB",         6);

    myDAQ.addToArmList(&vrb);
    myDAQ.addToReadList(&vrb);

    myDAQ.addToArmList(&dbb1);
    myDAQ.addToReadList(&dbb1);

    myDAQ.addToArmList(&dbb2);
    myDAQ.addToReadList(&dbb2);

    myDAQ.addToArmList(&dbb3);
    myDAQ.addToReadList(&dbb3);

    myDAQ.addToArmList(&dbb4);
    myDAQ.addToReadList(&dbb4);

    myDAQ.addToArmList(&dbb5);
    myDAQ.addToReadList(&dbb5);

    myDAQ.addToArmList(&dbb6);
    myDAQ.addToReadList(&dbb6);
}

