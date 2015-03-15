#include "MDEv2718.hh"
#include "MDEv977.hh"
#include "MDEvRB.hh"
#include "MDEDBB.hh"
#include "MDEv1731.hh"
#include "DAQManager.hh"
#include "MiceDAQMessanger.hh"

using namespace std;
void dumpEventVRB(MDE_Pointer *data, int nbr);
MDEv977 *ioReg;

int main(int argc, char** argv) {

  int nEvents = 0;
  if (argc == 2 ) {
    stringstream ss;
    ss << argv[1];
    ss >> nEvents;
  }

  try {
    MiceDAQMessanger::Instance()->setVerbosity(3);
    MiceDAQMessanger::Instance()->setThrowExceptionLevel(3);

/////////////////////////////////////////////////////////////////////////////////////////

    MDEv2718 controler;
    controler.setParams("BNumber",    0)
                       ("BLink",      0)
                       ("MaxNumEvts", 100);

    if ( !controler.Arm() )
      return 0;

/////////////////////////////////////////////////////////////////////////////////////////

    ioReg = new MDEv977;
    ioReg->setParams("GEO", 3)("BaseAddress", 0xaaaa0000);
    if ( !ioReg->ArmTriggerReceiver() )
      return false;

    if ( !ioReg->ArmPartTriggerSelector("GVA1") )
      return false;

    if ( !ioReg->ArmTrailer() )
      return false;

/////////////////////////////////////////////////////////////////////////////////////////


    MDEvRB vrb1;
    vrb1.setParams("GEO",               1)
                  ("BaseAddress",       0x11ff0000)
                  ("DBB_FV",            1)
                  ("N_DBBs",            6)

                  ("ChannelMask0_DBB1", 0xffffffff)
                  ("ChannelMask1_DBB1", 0xffffffff)
                  ("Geo_DBB1",          0)

                  ("ChannelMask0_DBB2", 0xffffffff)
                  ("ChannelMask1_DBB2", 0xffffffff)
                  ("Geo_DBB2",          1)

                  ("ChannelMask0_DBB3", 0xffffffff)
                  ("ChannelMask1_DBB3", 0xffffffff)
                  ("Geo_DBB3",          2)

                  ("ChannelMask0_DBB4", 0xffffffff)
                  ("ChannelMask1_DBB4", 0xffffffff)
                  ("Geo_DBB4",          3)

                  ("ChannelMask0_DBB5", 0xffffffff)
                  ("ChannelMask1_DBB5", 0xffffffff)
                  ("Geo_DBB5",          4)

                  ("ChannelMask0_DBB6", 0xffffffff)
                  ("ChannelMask1_DBB6", 0xffffffff)
                  ("Geo_DBB6",          5);


    if ( !vrb1.Arm() )
      return 0;

/////////////////////////////////////////////////////////////////////////////////////////


//     vrb1.updateBA();
//     vrb1.SetAllDBBGeos();
//     vrb1.GetDBBFV(1);
//     vrb1.GetDBBFV(1);
//     vrb1.GetDBBFV(2);
//     vrb1.GetDBBFV(2);
//     vrb1.GetDBBFV(3);
//     vrb1.GetDBBFV(3);
//     vrb1.GetDBBFV(4);
//     vrb1.GetDBBFV(4);
//     vrb1.GetDBBFV(5);
//     vrb1.GetDBBFV(5);

//     vrb1.updateBA();
//     vrb1.SetAllDBBGeos();
// //     vrb1.resetAllDBBs();
//     vrb1.GetDBBFV(1);
//     vrb1.getDBBStatus(2);
//     vrb1.GetDBBFV(1);
//     vrb1.getDBBStatus(2);
//     vrb1.GetDBBFV(1);

    int mem_size = 1024*62;
    MDE_Pointer *dataPtr = new MDE_Pointer[mem_size];
    std::cout << "\nMemory alocated: " << mem_size << std::endl << std::endl;
    vrb1.setDataPtr(dataPtr);

//     MiceDAQMessanger::Instance()->setVerbosity(0);

    int spillCount=0;
    while (spillCount<nEvents) {

      ioReg->setOutput(0x2000);
      ioReg->unsetOutput(0x2000);
      if( !(spillCount%100) )
        cout << "\n \nSpill gate " << spillCount << " generated ..." << endl;

      usleep(25000);
//       sleep(1);
//       vrb1.MemClear(0x000000, 512);
//       vrb1.MemClear(0x010000, 512);
//       vrb1.MemClear(0x020000, 512);
//       vrb1.MemClear(0x030000, 512);
//       vrb1.MemClear(0x040000, 512);
//       vrb1.MemClear(0x050000, 512);
//       vrb1.StartDBBReadout();

      for (int dbbId=1; dbbId<7; dbbId++) {
        bool data_OK = vrb1.StartDBBReadout(dbbId);
        usleep(100);
        if (data_OK) {
          int nbr = vrb1.GetDBBData(dbbId);
          cout << "Event " << spillCount << " is recorded (" << nbr << " bytes)\n";
//           dumpEventVRB(dataPtr, nbr);
        }
      }

      spillCount++;
    }

    vrb1.DisArm();
    controler.DisArm();

    delete dataPtr;
    delete ioReg;

  } catch (MiceDAQException e) {
    ioReg->setOutput(0x1000);
    ioReg->unsetOutput(0x1000);
    cout << "DAQ exception"    << endl;
    cout << e.GetLocation()    << endl;
    cout << e.GetDescription() << endl;
    return 0;
  }
//   catch ( MDexception & e) {
//     cout << "Unpacking exception" << endl;
//     cout << e.GetDescription()   << endl;
//     return 0;
//   }
  catch(std::exception & e) {
    cout << "Standard exception" << endl;
    cout << e.what() << endl;
    return 0;
  }

  return 1;
}


void dumpEventVRB(MDE_Pointer *data, int nbr) {

  int type = (*data & 0xf0000000) >> 28;
  int id   = (*data & 0x0fc00000) >> 22;
  int hit  = *data & 0x1fff;
//  int zero = (*data & 0x3f0000) >> 16;
  cout << "----------------------------------------------------------------" << endl;
  cout << "header type: "<< hex << type << dec << "    board id: " << id << "    spill number: " << hit << endl;
//   cout << "zeros: " << zero << endl;
  data++;

  int tr_count = (*data & 0x3ff0000) >> 16;
  int h_count  = *data & 0xffff;
  int hc = (nbr/4) - 4;
  cout << "trigger count: "<< tr_count << "    hit count:" << h_count
            << " (" << hc << ")" << endl;
  cout << "----------------------------------------------------------------" << endl;
  data++;
  //for (int i=0; i<(h_count*2); i++) {
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
  cout << "trailer type: "<< hex << type << dec << "    board id: " << id
       << "    status: " << hex << status << dec << "    spill number: " << hit << endl;
  data++;

  cout << "spill width: " << *data << endl;
  cout << "----------------------------------------------------------------" << endl << endl;
}

