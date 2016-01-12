#include <iostream>
#include <sstream>

#include "MDEv2718.hh"
#include "MDEv977.hh"

#include "MiceDAQMessanger.hh"
#include "equipmentList_common.hh"

// Tracker HW
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

#define N_VLSB 8
#define N_BANK 4

using namespace std;
const std::vector<uint32_t> VLSBAddresses{0x10000000, 0x20000000, 0x30000000, 0x40000000,
    0x18000000, 0x28000000, 0x38000000, 0x48000000};

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

    std::vector<MDE_Pointer> bankData(10 * 130944);
    std::vector<MDEVLSB> VLSBs;
    std::vector<MDEVLSBBank> VLSBBanks;

    const uint32_t VLSBOffset (8);
    // Construct and Set parameters.
    // Note that the last board in the set is set as master (by convention).
    for (int i = 0; i < N_VLSB; i++) {

      int VLSB_id = i + VLSBOffset;
      VLSBs.push_back(MDEVLSB());
      VLSBs.back().setParams("GEO", VLSB_id * 4)
	("BaseAddress", VLSBAddresses.at(i))
	("Master", i == (N_VLSB - 1))
	("UseInternalTrigger", 1);
      VLSBs.back().setDataPtr(&bankData[0]);

      for (int j = 0; j < N_BANK; j++) {
	VLSBBanks.push_back(MDEVLSBBank());
	VLSBBanks.back().setParams("GEO", VLSB_id * 4 + j)
	  ("BaseAddress", VLSBAddresses.at(i))
	  ("VLSBid", VLSB_id)
	  ("VLSBBank", j);
	VLSBBanks.back().setDataPtr(&bankData[0]);
      }
    }
    
    MDEVLSB & VLSBMaster = VLSBs.back();
    
    for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it) it->Arm();
    for (auto it = VLSBBanks.begin(); it != VLSBBanks.end(); ++it) it->Arm();

    TH2D adc_th2d("adcs","adcs", 8192, -0.5, 8191.5, 256, -0.5, 255.5);
    MDfragmentVLSB vlsbfragment;

    VLSBMaster.SetupTriggerWindow(0, 0);
    VLSBMaster.SetupInternalTrigger(0x800, 40);
    VLSBMaster.SetupSpillLength(0x3FFFF);

    VLSBMaster.DisableTrigger();
    for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it) it->DisableLVDS();

    for (auto i (0); i<100; i++)
      {
	std::cout << "-------------------------------------------------" << std::endl;
	// Do some readout proces...
	for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it) it->EnableLVDS();
	VLSBMaster.EnableTrigger();

	sleep (1);

	VLSBMaster.DisableTrigger();
	for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it) it->DisableLVDS();

	std::cout << "Triggers: " << VLSBMaster.GetTriggerCount() << std::endl;

	for (auto it = VLSBBanks.begin(); it != VLSBBanks.end(); ++it)
	  {
	    const int bankUID = it->getParam("GEO");
	    it->setDataPtr(&(bankData[0]));
	    int nwords = it->ReadEvent();

	    std::cout << "bankUID:" << bankUID << "  nwords:" << nwords << std::endl;
	    
	    // Extract Data:
	    if (nwords>0)
	      {
		// Process the data fragment..
		vlsbfragment.SetDataPtr(&(bankData[0]), nwords*4);
		for (int i=0; i<nwords; i++)
		  {
		    int ChannelUID = bankUID*128 + vlsbfragment.GetChannel(i);
		    adc_th2d.Fill(ChannelUID, vlsbfragment.GetAdc(i));
		  }
	      }
	  }
      }

    adc_th2d.SaveAs("test.C");

  for (auto it = VLSBBanks.begin(); it != VLSBBanks.end(); ++it)
    it->DisArm();

  // Perform Arming Process VLSBs:
  for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it)
    it->DisArm();

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
