#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> //  our new library
#include <string>
#include <time.h> // For checking spills

#include "MDEv977.hh"
#include "MDEVLSB.hh"
#include "MDEVLSBBank.hh"
#include "MDEv2718.hh"
#include "VLSBDef.hh"

#include "MDfragmentVLSB.h"
#include <TH2D.h>
#include <TH1D.h>
#include <TFile.h>
#include <TVectorD.h>

/**************************************************************************************
 * Configuration:
 */
#define N_VLSB 8
#define N_BANK 4

// Timeout if no spill in this many seconds.
const int spill_timeout_s (10);

// Address of the trigger reciever
const uint32_t triggerRecieverAddress(0x50000000);

// Addresses of the VLSBs
const std::vector<uint32_t> VLSBAddresses{0x10000000, 0x20000000, 0x30000000, 0x40000000,
    0x18000000, 0x28000000, 0x38000000, 0x48000000};


/**************************************************************************************
 * Interrupt Function: - To abort recording date on ctl-c
 */
volatile sig_atomic_t flag_interrupt = 0;
void function_interrupt(int sig){
  flag_interrupt = 1;
}

/***************************************************************************************
 * Main function, args is as follows:
 *   [0] = Executable Name
 *   [1] = Output Filename
 *   [2] = Number of spills
 *   [3] = BankUID offset (=32 for downstream detector)
 */
int main(int argc, char** argv) {


  // Welcome message and argument parsing:
  std::cout << "trackerReadout " << std::endl
	    << "Tracker standalone DAQ code for testing" << std::endl;

  if (argc < 4)
    {
      std::cout << std::endl
		<< "ERROR: Invalid arguments, please run with the following:" << std::endl
		<< "<Output Filename> <Number of Spills> <VLSB Offset>" << std::endl
		<< " note that <VLSB Offset> is 0 for upstream and 8 for the downstream" << std::endl;
      return -1;
    }

  // Read in options:
  const std::string root_filename (argv[1]);
  const uint32_t spills(std::stoi(argv[2]));
  const uint32_t VLSBOffset (std::stoi(argv[3]));

  // Attach interrupt to the interrupt function, which will set the flag:
  // we also use a mask to inhibit interrupts during readout..
  sigset_t intmask;
  sigemptyset(&intmask);
  sigaddset(&intmask, SIGINT);
  sigaddset(&intmask, SIGWINCH);
  signal(SIGINT, function_interrupt);

  // Setup VME Controller:
  MDEv2718 controler;
  controler.setParams("BNumber", 0)("BLink", 0);

  // Setup Trigger Reciever:
  MDEv977 triggerReciever;
  triggerReciever.setParams("GEO", 0)
    ("BaseAddress", triggerRecieverAddress);
  std::vector<MDE_Pointer> triggerData(1024);
  triggerReciever.setDataPtr(&triggerData[0]);

  // Setup the VLSBs for readout:
  // There are 4 banks in each VLSB which must be initilised.
  std::vector<MDE_Pointer> bankData(10 * 130944);
  std::vector<MDEVLSB> VLSBs;
  std::vector<MDEVLSBBank> VLSBBanks;

  // Construct and Set parameters.
  // Note that the last board in the set is set as master (by convention).
  for (int i = 0; i < N_VLSB; i++) {

    int VLSB_id = i + VLSBOffset;
    VLSBs.push_back(MDEVLSB());
    VLSBs.back().setParams("GEO", VLSB_id * 4)
      ("BaseAddress", VLSBAddresses.at(i))
      ("Master", i == (N_VLSB - 1))
      ("UseInternalTrigger", 0);
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

  // Define a reference to the master....
  MDEVLSB & VLSBMaster = VLSBs.back();

  /**************************************************************************************
   * Detector arming sequence.
   */

  std::cout << std::endl
	    << " ------------------------------------------------------- " << std::endl
	    << "  Beginning to arm detectors" << std::endl
	    << " ------------------------------------------------------- " << std::endl << std::endl;

  bool status(true);

  try
    {
      // Check the controller got armed.
      if (!controler.Arm())
	{
	  std::cout << "Controller Arming Failed." << std::endl;
	  throw;
	}

      // Arm the trigger reciever:
      if (!triggerReciever.ArmTriggerReceiver() || !triggerReciever.ArmTrailer())
	{
	  std::cout << "Trigger Reciever Arming Failed." << std::endl;
	  throw;
	}

      // Perform Arming Process VLSBs:
      for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it)
	if (!it->Arm())
	  {
	    std::cout << "VLSB Arm failed." << std::endl;
	    throw;
	  }

      // Perform Arming Process for VLSB Banks:
      for (auto it = VLSBBanks.begin(); it != VLSBBanks.end(); ++it)
	if (!it->Arm())
	  {
	    std::cout << "VLSBBank Arm failed." << std::endl;
	    throw;
	  }
    }
  catch(int e)
    {
      status = false;
    }

  /**************************************************************************************
   * Readout stuff
   */
  std::cout << std::endl
	    << " ------------------------------------------------------- " << std::endl
	    << "  Beginning readout sequence..." << std::endl
	    << " ------------------------------------------------------- " << std::endl << std::endl;


  uint32_t spills_complete(0), triggers(0), total_triggers(0), trigger_tdc;
  timespec lastop_time, this_time;
  clock_gettime(CLOCK_MONOTONIC, &lastop_time);
  time_t now = time(0);
  MDfragmentVLSB vlsbfragment;

  /**************************************************************************************
   * ROOT File/Histogram objects:
   * */
  TFile output_tfile(root_filename.c_str(),"RECREATE","TrackerHistograms", 9);

  TH2D adc_th2d("adcs","adcs", 8192, -0.5, 8191.5, 256, -0.5, 255.5);
  TH2D discr_th2d("discrs","discrs", 8192, -0.5, 8191.5, 2, -0.5, 1.5);
  TH2D tdc_th2d("tdcs","tdcs", 8192, -0.5, 8191.5, 256, -0.5, 255.5);

  TH1D data_volume_th1d("data_volume","data_volume", 64, -0.5, 63.5);
  TH1D fifo_empty_th1d("fifo_empty","fifo_empty", 64, -0.5, 63.5);
  TH1D fifo_full_th1d("fifo_full","fifo_full", 64, -0.5, 63.5);
  TH1D data_timeout_th1d("data_timeout","data_timeout", 64, -0.5, 63.5);
  TH1D crc_error_th1d("crc_error","crc_error", 64, -0.5, 63.5);
  TH1D clkphase_error_th1d("clkphase_error","clkphase_error", 64, -0.5, 63.5);

  TH1D channel_entries_th1d("channel_entries","channel_entries", 8192, -0.5, 8191.5);
  TH1D triggers_spill_th1d("triggers_spill","triggers_spill", 301, -0.5, 300.5);
  TH1D triggers_tdc_th1d("triggers_tdc","triggers_tdc", 301, -0.5, 300.5);

  TVectorD start_time(1);
  start_time[0] = now;
  start_time.Write("_DAQ_start");


  // Loop while run is true, note that Ctl-C + spill counts end the running
  bool run(status);
  while(run)
    {
      // Perform readout
      try
	{
	  if ( triggerReciever.EventArrivedTriggerReceiver() )
	    {

	      // Check trigger reciever event:
	      triggerReciever.ReadEventTriggerReceiver();
	      std::cout << "Event Arrived. Event type is " << triggerReciever.getEventTypeAsString() << std::endl;

	      clock_gettime(CLOCK_MONOTONIC, &lastop_time);

	      /**************************************************************************************
	       * Check for start of burst: */
	      if ( triggerReciever.getEventType() == START_OF_BURST )
		{
		  // Begin Data Taking:
		  for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it)
		    {
		      it->EnableLVDS();
		    }
		  VLSBMaster.EnableTrigger();
		}

	      /**************************************************************************************
	       * Check for Physics Event - Readout phase */
	      if ( triggerReciever.getEventType() == PHYSICS_EVENT )
		{
		  // Block signals from interrupting this process
		  // signals during readout seem to segfault...
		  // maybe something involving the caen controller/multireads.
		  sigprocmask(SIG_BLOCK, &intmask, NULL);

		  // Disable Triggers and LVDS:
		  VLSBMaster.DisableTrigger();
		  for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it) {
		    it->DisableLVDS();
		  }

		  // Retreve/Record number of recorded triggers from master:
		  triggers = VLSBMaster.GetTriggerCount();
		  total_triggers += triggers;
		  triggers_spill_th1d.Fill(triggers);

		  // Record the time from trigger to data arrival on the master.
		  trigger_tdc = VLSBMaster.GetTriggerDataTDC();
		  triggers_tdc_th1d.Fill(trigger_tdc);

		  // Read Data Banks:
		  for (auto it = VLSBBanks.begin(); it != VLSBBanks.end(); ++it)
		    {
		      const int bankUID = it->getParam("GEO");

		      // Update the Histograms with errors from the FIFOs:
		      const int FIFOStatus (it->ReadFIFOStatus());
		      if (FIFOStatus & VLSB_FIFOEmpty) fifo_empty_th1d.Fill(bankUID);
		      if (FIFOStatus & VLSB_FIFOFull) fifo_full_th1d.Fill(bankUID);
		      if (FIFOStatus & VLSB_DataTimeout) data_timeout_th1d.Fill(bankUID);
		      if (FIFOStatus & VLSB_CRCError) crc_error_th1d.Fill(bankUID);
		      if (FIFOStatus & VLSB_ClockPhaseError) clkphase_error_th1d.Fill(bankUID);

		      if (!it->FIFOOK())
		      {
		      		MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
		      		messanger->sendMessage("FIFO not reported as OK, deleting bank data", MDE_ERROR);
		      }

		      // Read the Bank Data - Update NWords read..
		      it->setDataPtr(&(bankData[0]));
		      int nwords = it->ReadEvent();
		      data_volume_th1d.Fill(bankUID,nwords);

		      if (nwords>0)
			{
			  // Process the data fragment..
			  vlsbfragment.SetDataPtr(&(bankData[0]), nwords*4);
			  for (int i=0; i<nwords; i++)
			    {
			      int ChannelUID = bankUID*128 + vlsbfragment.GetChannel(i);
			      channel_entries_th1d.Fill(ChannelUID);
			      adc_th2d.Fill(ChannelUID, vlsbfragment.GetAdc(i));
			      discr_th2d.Fill(ChannelUID, vlsbfragment.GetDiscriBit(i));
			      tdc_th2d.Fill(ChannelUID, vlsbfragment.GetTdc(i));
			    }
			}
		    }

		  // Unblock signals.
		  sigprocmask(SIG_UNBLOCK, &intmask, NULL);

		}

	      /**************************************************************************************
	       * Check for end of burst.. */
	      if ( triggerReciever.getEventType() == END_OF_BURST )
		{
		  now = time(0);
		  std::cout << "Spill " << spills_complete << " Completed, " << spills - spills_complete - 1 << " remain" << std::endl
			    << "Triggers/Total: " << triggers << "/" << total_triggers << std::endl
			    << "Time: " << ctime(&now)
			    << "----------------------------------------------------" << std::endl;

		  spills_complete++;

		  // Check spill counter:
		  if (spills_complete >= spills)
		    {
		      std::cout << "Spills complete" << std::endl;
		      run = false;
		    }

		  // Catch Keyboard interrupt:
		  if (flag_interrupt == 1)
		    {
		      std::cout << "Caught Exit Flag" << std::endl;
		      run = false;
		    }

		}

	      triggerReciever.ReadEventTrailer();
	    }

	  clock_gettime(CLOCK_MONOTONIC, &this_time);
	  long int dt = this_time.tv_sec - lastop_time.tv_sec;
	  if ( dt > spill_timeout_s)
	    {
	      std::cout << "No Spill in " << spill_timeout_s
			<<" seconds, Timing out" << std::endl;
	      run = false;
	    }


	  usleep(1000);

	}
      catch (int e)
	{

	}


    }

  std::cout << std::endl
	    << " ------------------------------------------------------- " << std::endl
	    << "  Ending readout sequence..." << std::endl
	    << " ------------------------------------------------------- " << std::endl << std::endl;


  // ____________________________________________________________________________
  // Perform Disarming Process for VLSB Banks:
  for (auto it = VLSBBanks.begin(); it != VLSBBanks.end(); ++it)
    it->DisArm();

  // Perform Arming Process VLSBs:
  for (auto it = VLSBs.begin(); it != VLSBs.end(); ++it)
    it->DisArm();

  //triggerReciever.DisArmTriggerReceiver();
  triggerReciever.DisArmTrailer();
  triggerReciever.DisArmTriggerReceiver();

  // Disarm Equipment:
  controler.DisArm();

  std::cout << "Disarming complete" << std::endl;

  // Count number of CRC/Phase errors
  uint32_t crc_errors = crc_error_th1d.GetEntries();
  uint32_t phase_errors = clkphase_error_th1d.GetEntries();

  // Write ROOT Data:
  if (spills_complete > 0)
    {
      TVectorD stop_time(1);
      now = time(0);
      stop_time[0] = now;
      stop_time.Write("_DAQ_stop");
      output_tfile.Write();

      // Output error logic:
      if (crc_errors>spills_complete+2)
	return 100;
      else if (phase_errors>spills_complete+2)
	return 10;
      else
	return 0;
    }
  else
    {
      return 1000;
    }
}
