#include "MDEv2718.hh"
#include "MDEv1495.hh"
#include "MiceDAQMessanger.hh"
#include "MDdataWordV1495.h"
#include "MDfragmentV1495.h"

using namespace std;
void Dump(MDE_Pointer *data, int nbr);
// MDEv977 *ioReg;

int main(int argc, char** argv) {

  int nEvents = 0;
  if (argc == 2 ) {
    stringstream ss;
    ss << argv[1];
    ss >> nEvents;
  }

  try {
    MiceDAQMessanger::Instance()->setVerbosity(0);
    MiceDAQMessanger::Instance()->setThrowExceptionLevel(2);

/////////////////////////////////////////////////////////////////////////////////////////

    MDEv2718 controler;
    controler.setParams("GEO",        0)
                       ("BNumber",    0)
                       ("BLink",      0)
                       ("FIFOMode",   1)
                       ("MaxNumEvts", 100);

    if ( !controler.Arm() )
      return 0;


/////////////////////////////////////////////////////////////////////////////////////////


    MDEv1495 trigger;
    trigger.setParams("GEO",               2)
                     ("BaseAddress",       0x11020000)
//                      ("SGOpenDelay",       0x2FFFF)
//                      ("SGCloseDelay",      0x3FFFF)
//                     ("SGOpenDelay",        722050)
//                     ("SGCloseDelay",      1023050)
                     ("SGOpenDelay",       1200000)
                     ("SGCloseDelay",      1500000)
                     ("SggCtrl",           0x00F)
//                     ("TriggerLogicCtrl",  TRIGGER_PULS_500KHz | RAND2)
                     ("TriggerLogicCtrl",  TRIGGER_PULS_20KHz)
//                     ("TriggerLogicCtrl",  TRIGGER_TOF1_OR)
                     ("TOF0Mask",          0xfffff)
                     ("TOF1Mask",          0xfffff)
                     ("TOF2Mask",          0xfffff)
                     ("PartTrVetoLenght",  150);

    if ( !trigger.Arm() )
      return 0;

/////////////////////////////////////////////////////////////////////////////////////////

    int mem_size = 1024*62;
    MDE_Pointer *dataPtr = new MDE_Pointer[mem_size];
    std::cout << "\nMemory alocated: " << mem_size << std::endl << std::endl;
//     trigger.setDataPtr(dataPtr);

    MiceDAQMessanger::Instance()->setVerbosity(0);

    int spillCount=0;
    while (spillCount<nEvents) {
      cout << "\n \nGenerating Spill gate " << spillCount << " ..." << endl;
      trigger.softwareStart(true);

      trigger.setDataPtr(dataPtr);
      int nbr = trigger.ReadEvent();
      Dump(dataPtr, nbr);

//       usleep(50000);
      sleep(1);
      spillCount++;
    }

    trigger.DisArm();
    controler.DisArm();

    delete dataPtr;

  } catch (MiceDAQException e) {
    cout << "DAQ exception"    << endl;
    cout << e.GetLocation()    << endl;
    cout << e.GetDescription() << endl;
    return 0;
  } catch ( MDexception & e) {
    cout << "Unpacking exception" << endl;
    cout << e.GetDescription()   << endl;
    return 0;
  } catch(std::exception & e) {
    cout << "Standard exception" << endl;
    cout << e.what() << endl;
    return 0;
  }

  return 1;
}

void Dump(MDE_Pointer *data, int nbr) {
  MDfragmentV1495 df;
  df.SetDataPtr(data, nbr);
  df.Dump();

}


