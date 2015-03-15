#include "MDEv2718.hh"
#include "MDEvCB.hh"
#include "MiceDAQMessanger.hh"

using namespace std;

int main(int argc, char** argv) {

  int baseAddress = 0x220000;
  if (argc > 1 ) {
    baseAddress = getBA(argv[1]);
  }
  int nFEBs = 2;
  if (argc > 2 ) {
    stringstream ss;
    ss << argv[2];
    ss >> nFEBs;
  }

  try {
    MiceDAQMessanger::Instance()->setVerbosity(0);

/////////////////////////////////////////////////////////////////////////////////////////

    MDEv2718 controler;
//     controler.setParams("BNumber",    0)
//                        ("BLink",      0)
//                        ("MaxNumEvts", 100);
    if (!controler.Arm())
      return 0;

/////////////////////////////////////////////////////////////////////////////////////////

    MDEvCB vcb1;
    vcb1.setParams("GEO",             0)
                  ("BaseAddress",     baseAddress)
                  ("N_FEBs",          nFEBs);

    if (!vcb1.Arm())
      return 0;;

/////////////////////////////////////////////////////////////////////////////////////////

    vcb1.DisArm();
    controler.DisArm();

  } catch (MiceDAQException e) {
    cout << "DAQ exception"    << endl;
    cout << e.GetLocation()    << endl;
    cout << e.GetDescription() << endl;
    return 0;
  }
  catch(std::exception & e) {
    cout << "Standard exception" << endl;
    cout << e.what() << endl;
    return 0;
  }

  return 1;
}

