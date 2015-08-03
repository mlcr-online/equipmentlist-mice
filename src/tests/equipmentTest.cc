#include "MDETestLib.hh"
#include "MDexception.h"

int nEvents=1;
extern int nbStored;
extern MiceDAQRunStats   runStats;
extern MiceDAQSpillStats spillStats;

int main(int argc, char** argv) {

  try {
    MiceDAQMessanger::Instance()->setVerbosity(1);
  MiceDAQMessanger::Instance()->setThrowExceptionLevel(4);
    //runStats->getInfo();
    //spillStats->getInfo();

    if (argc!=3) {
      std::cout << "Usage:     equipmentTest  <equipment name>  <base address> \n"
                << "Examples: ./equipmentTest V1290 0x21100000 \n"
                << "          ./equipmentTest V977 21020000 \n"
                << "          ./equipmentTest V1724 0x11120000 \n"
                << "          ./equipmentTest V1731 11140000 \n"
                << "          ./equipmentTest VCB 0xE00A0000 \n"
                << "          ./equipmentTest V830 11030000" << std::endl;
      return 0;
    }

    std::string eqName(argv[1]);

    spillStats.setParams("ParticleTriggers", 0)("DataRecorded", 0)("ErrorFlag", 0);

    MDEv2718 controler;
    controler.setParams("BNumber", 0)("BLink", 0);
    if ( !controler.Arm() )
      return 0;

    bool testOK = false;
    if (eqName == "V977")  testOK = testV977( getBA(argv[2]), 10);
    if (eqName == "V830")  testOK = testV830( getBA(argv[2]) );
    if (eqName == "V1290") testOK = testV1290( getBA(argv[2]) );
    if (eqName == "V1724") testOK = testV1724( getBA(argv[2]) );
    if (eqName == "V1731") testOK = testV1731( getBA(argv[2]) );
    if (eqName == "V1495") testOK = testV1495( getBA(argv[2]) );
    if (eqName == "VCB")   testOK = testVCB( getBA(argv[2]) );
    if (eqName == "VRB")   testOK = testVRB( getBA(argv[2]) );
    if (eqName == "Integrated" || eqName == "integrated") {
      if (std::string(argv[2]) == "miceacq14") testOK = testmiceacq14();
      if (std::string(argv[2]) == "miceacq20") testOK = testmiceacq20();
    }

    if (eqName == "V977" || eqName == "Integrated" ) runStats.getInfo();
    else spillStats.getInfo();

    if( !testOK )
      return 0;

    if ( !controler.DisArm() )
      return 0;

  } catch(MiceDAQException e) {
    std::cout << e.GetLocation() << std::endl;
    std::cout << e.GetDescription() << std::endl;
    return 0;
  } catch(MDexception e) {
    std::cout << e.GetDescription() << std::endl;
    return 0;
  }

  return 1;
}








