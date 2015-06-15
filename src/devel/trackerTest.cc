#include <iostream>
#include <sstream>


#include "MDEv2718.hh"
#include "MDEv977.hh"

#include "MiceDAQMessanger.hh"
#include "equipmentList_common.hh"

// Tracker HW
#include "MDEVLSBMaster.hh"
#include "MDEVLSB.hh"
#include "MDEVLSBBank.hh"

// headers included for their includes!
#include "MDfragmentV1495.h"
#include "MDfragmentV1290.h"
#include "MDfragmentV1724.h"
#include "MDfragmentV1731.h"
#include "MDfragmentVLSB.h"

#include "TH2I.h"
#include "TCanvas.h"

using namespace std;

int main (int argc, char** argv)
{
	std::cout << "Tracker Test" << std::endl;

	int nEvents = 100;
  if (argc == 2 ) {
    stringstream ss;
    ss << argv[1];
    ss >> nEvents;
  }

  try {
    MiceDAQMessanger::Instance()->setVerbosity(1);

    MDEv2718 controler;
    controler.setParams("GEO",        0)
                       ("BNumber",    0)
                       ("BLink",      0)
                       ("FIFOMode",   0)
                       ("MaxNumEvts", 100);

    if ( !controler.Arm() )
      return 0;

//////////////////////////////////////////////////////////////////////////////

    MDEv977 ioReg;
    ioReg.setParams("GEO", 3)("BaseAddress", 0x50000000);

    if ( !ioReg.ArmTriggerReceiver() )
      return false;

    if ( !ioReg.ArmPartTriggerSelector("TOF1") )
      return false;

    if ( !ioReg.ArmTrailer() )
      return false;

    MDE_Pointer* data_tr = new MDE_Pointer[256];
    ioReg.setDataPtr(data_tr);

//////////////////////////////////////////////////////////////////////////////

    MDEVLSBMaster VLSBMaster;
    VLSBMaster.SetParams("BaseAddress", 0x48000000);
    VLSBMaster.Arm();
    
    MDEVLSB vlsb;
    vlsb.SetParams("BaseAddress", 0x48000000);
    vlsb.Arm();

    MDEVLSBBank bank;
    bank.SetParams("BaseAddress", 0x48000000);
    bank.SetParams("BankNumber", 0);
    bank.Arm();
     


    int memSizeTr = 128*300*1024;
    MDE_Pointer * data_vlsbbank = new MDE_Pointer[memSizeTr];
    bank.setDataPtr(data_vlsbbank);

//////////////////////////////////////////////////////////////////////////////

MDfragmentVLSB vlsbfragment;

//////////////////////////////////////////////////////////////////////////////

TCanvas *c1 = new TCanvas;
c1->cd(0);
TH2I * vlsb_adchist = new TH2I("adchist", "", 128, -0.5, 127.5, 256, -0.5, 255.5); // Smart man!

int spillCount=0;
int nbStored = 0;
    while (spillCount<nEvents) {
      std::cout << "\n \nGenerating Spill gate " << spillCount << " ..." << std::endl;
//       trigger.softwareStart();
      bool done = false;
      while (!done) {
        if ( ioReg.EventArrivedTriggerReceiver() ) {
          ioReg.ReadEventTriggerReceiver();
          if (ioReg.getEventType() == START_OF_BURST) {
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
            VLSBMaster.ReadEventVLSBMasterTrailer();

            ioReg.ReadEventTrailer();
           
          } else if (ioReg.getEventType() == PHYSICS_EVENT) {
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
           
           VLSBMaster.ReadEventVLSBMaster();
	   vlsb.ReadEvent("READOUT");
           nbStored = bank.ReadEvent();

	   //Use the fragment output:
           vlsbfragment.SetDataPtr(data_vlsbbank, nbStored);

	   for (int i=0; i<3*128; i++)
	   {
           	std::cout << std::dec << " E:" << vlsbfragment.GetEventNum(i)
	                  << " C:" << vlsbfragment.GetChannel(i)
                          << " A:" << vlsbfragment.GetAdc(i)
			  << std::endl;
	   }

           for (int i=0; i<nbStored/4; i++)
           {
		vlsb_adchist->Fill(vlsbfragment.GetChannel(i), vlsbfragment.GetAdc(i));
	   }

            nbStored += ioReg.ReadEventTrailer();
          } else if (ioReg.getEventType() == END_OF_BURST){
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;

            vlsb.ReadEvent("ACQUIRE");
            VLSBMaster.ReadEventVLSBMasterTrailer();

            ioReg.ReadEventTrailer();
            done =true;
          }
        }
      }

    vlsb_adchist->Draw("COL");
    spillCount++;
  }


c1->SaveAs("adchist.png");


//////////////////////////////////////////////////////////////////////////////
// Disarming... 


    delete data_vlsbbank;

    if ( !vlsb.DisArm() )
      return 0;

   if ( !VLSBMaster.DisArm() )
      return 0;

   if ( !controler.DisArm() )
      return 0;

} catch (MiceDAQException e) {
    std::cout << e.GetLocation() << std::endl;
    std::cout << e.GetDescription() << std::endl;
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

}
