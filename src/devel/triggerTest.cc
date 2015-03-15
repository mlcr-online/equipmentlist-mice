#include <sstream>
#include <vector>
#include <bitset>

#include "MDEv2718.hh"
#include "MDEv1290.hh"
#include "MDEv977.hh"
#include "MDEv1495.hh"

#include "MiceDAQMessanger.hh"
#include "equipmentList_common.hh"

#include "MDfragmentV1495.h"
#include "MDfragmentV1290.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1I.h"

MDEv2718 controler;
MDEv977 ioReg;
MDEv1290 tdc;
MDEv1495 trigger;

MDE_Pointer *data_io;
MDE_Pointer *data_tdc;
MDE_Pointer *data_tr;

TTree *dataTree;
TH1I *hh, *hv;
vector<int> ch0hits, ch1hits, ch14hits, ch15hits;
int TDCtimeTag, TrigTimeTag, Tof1mask;

using namespace std;

void ProcessTDC(int nEv, MDE_Pointer* dataPtr) {


}

void ProcessTrigger(int nbr) {
  MDfragmentV1495 df;
  df.SetDataPtr(data_tr, nbr);
//   df.Dump();
//   int nTr = df.GetNumTriggers();
//   for (int xTr=0; xTr<nTr; xTr++) {
//     
//   }
}

void Process(int nEv_tdc, int nbr_tr) {
  MDfragmentV1495 df;
  df.SetDataPtr(data_tr, nbr_tr);
//   df.Dump();
  MDE_Pointer *dataPtr = data_tdc;
  for (int xPe=0; xPe<nEv_tdc; xPe++) {
//     cout <<"###### \n\n";
    MDpartEventV1290 pe(dataPtr);
//     pe.Dump();
    ch0hits  = pe.GetLHitMeasurements(0);
    ch1hits  = pe.GetLHitMeasurements(1);
    ch14hits = pe.GetLHitMeasurements(14);
    ch15hits = pe.GetLHitMeasurements(15);
    TDCtimeTag = pe.GetTriggerTimeTag();
//     cout << "Tr. req. (" << ch0hits.size() << ", " << ch14hits.size() << ")\n";
//     cout << "Tr.      (" << ch1hits.size() << ", " << ch15hits.size() << ")\n";
    TrigTimeTag = df.GetTriggerTime(xPe);
    dataTree->Fill();

    Tof1mask = df.GetPatternTOF1(xPe);
    if (ch15hits.size()==0) {
      cout << "Evt" << xPe << endl;
      std::bitset<20>hits(Tof1mask);
      for(int xBar=0; xBar<10; xBar++) {
        if (hits.test(xBar)) {
          cout << "Hit in V. bar" << xBar << endl;
          hv->Fill(xBar);
        }
      }
      for(int xBar=10; xBar<20; xBar++) {
        if (hits.test(xBar)) {
          cout << "Hit in H. bar" << xBar-10 << endl;
          hh->Fill(xBar-10);
        }
      }
    }

    dataPtr += pe.GetSize()/4;
  }
}



int main(int argc, char** argv) {

  int nEvents = 1;
  if (argc == 2 ) {
    stringstream ss;
    ss << argv[1];
    ss >> nEvents;
  }

  TFile file("TriggerTest.root", "RECREATE");
  dataTree = new TTree("TriggerTest", "Tr. test data");
  hh = new TH1I("horizontal", "", 10, -0.5, 9.5);
  hv = new TH1I("vertical"  , "", 10, -0.5, 9.5);

  try {
    MiceDAQMessanger::Instance()->setVerbosity(1);

    controler.setParams("GEO",        0)
                       ("BNumber",    0)
                       ("BLink",      0)
                       ("FIFOMode",   1)
                       ("MaxNumEvts", 100);

    if ( !controler.Arm() )
      return 0;

//////////////////////////////////////////////////////////////////////////////

    ioReg.setParams("GEO", 3)("BaseAddress", 0x11150000);

    if ( !ioReg.ArmTriggerReceiver() )
      return false;

    if ( !ioReg.ArmPartTriggerSelector("TOF1") )
      return false;

    if ( !ioReg.ArmTrailer() )
      return false;

    data_io = new MDE_Pointer[256];
    ioReg.setDataPtr(data_io);

//////////////////////////////////////////////////////////////////////////////

    tdc.setParams("GEO",             11)
                 ("BaseAddress",     0x11050000)
                 ("ChannelMask",     0xffff)
                 ("UseEventFIFO",    1)
                 ("UseExtendedTTT",  1)
                 ("DoInit",          1)
                 ("TriggerMode",     1)
                 ("WinWidth",        80)
                 ("WinOffset",       -28)
                 ("SwMargin",        8)
                 ("RejMargin",       2)
                 ("TTSubtraction",   0)
                 ("DetectionEdge",   2)  //1 only trailing; 2 only leading; 3 trailing and leading.
                 ("TDCHeader",       1)
                 ("LSBCode",         3)
                 ("DeadtimeCode",    0);

    if ( !tdc.Arm() )
      return 0;

//     int memSizeTdc = tdc.getMaxMemUsed(nEvents);
    int memSizeTdc = 2048*16*16;
    data_tdc = new MDE_Pointer[memSizeTdc/4];
    tdc.setDataPtr(data_tdc);
    std::cout << "Memory alocated (TDC): " << memSizeTdc << std::endl;

//////////////////////////////////////////////////////////////////////////////

    trigger.setParams("GEO",               2)
                     ("BaseAddress",       0x11020000)
                     ("SGOpenDelay",       722050)
                     ("SGCloseDelay",      1023050)
//                      ("SGOpenDelay",       0x1FF)
//                      ("SGCloseDelay",      0x3FF)
                      ("SggCtrl",           0x10F)
//                     ("SggCtrl",           0x110)
//                      ("TriggerLogicCtrl",  TRIGGER_PULS_500KHz | RAND2)
//                      ("TriggerLogicCtrl",  TRIGGER_PULS_2KHz)
                     ("TriggerLogicCtrl",  TRIGGER_TOF1_OR)
                     ("TOF0Mask",          0xfffff)
                     ("TOF1Mask",          0xfffff)
                     ("TOF2Mask",          0xfffff)
                     ("PartTrVetoLenght",  150);

    if ( !trigger.Arm() )
      return 0;

    int memSizeTr = trigger.getMaxMemUsed();
    data_tr = new MDE_Pointer[memSizeTr/4];
    trigger.setDataPtr(data_tr);
    std::cout << "Memory alocated (Trigger): " << memSizeTr << std::endl;

//////////////////////////////////////////////////////////////////////////////

    vector<int> *pCh0hits = &ch0hits;
    dataTree->Branch("ch0",  "std::vector<int>", &pCh0hits);

    vector<int> *pCh1hits = &ch1hits;
    dataTree->Branch("ch1",  "std::vector<int>", &pCh1hits);

    vector<int> *pCh14hits = &ch14hits;
    dataTree->Branch("ch14", "std::vector<int>", &pCh14hits);

    vector<int> *pCh15hits = &ch15hits;
    dataTree->Branch("ch15", "std::vector<int>", &pCh15hits);

    dataTree->Branch("TDCTT",     &TDCtimeTag,  "TDCTT/I");
    dataTree->Branch("TrTT",      &TrigTimeTag, "TrTT/I");
    dataTree->Branch("TOF1Mask",  &Tof1mask,    "TOF1Mask/I");

    int spillCount=0;
    while (spillCount<nEvents) {
      std::cout << "\n \nGenerating Spill gate " << spillCount << " ..." << std::endl;
//       trigger.softwareStart();
      bool done = false;
      while (!done) {
        if ( ioReg.EventArrivedTriggerReceiver() ) {
          ioReg.ReadEventTriggerReceiver();
          if (ioReg.getEventType() == START_OF_BURST) {
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
            trigger.softwareClear();
            ioReg.ReadEventTrailer();
          } else if (ioReg.getEventType() == PHYSICS_EVENT) {
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
            int nEv_tdc = tdc.getNEventsStored();
//             int nbr_tdc = tdc.ReadEvent();
            tdc.ReadEvent();
            int nEv_tr  = trigger.getNTriggers();
            int nbr_tr  = trigger.ReadEvent();
            cout << "nEv: " << nEv_tdc << "/" << nEv_tr << " (tdc/tr)\n";
            if (nEv_tdc) {
              Process(nEv_tdc, nbr_tr);
            }
            ioReg.ReadEventTrailer();
          } else if (ioReg.getEventType() == END_OF_BURST){
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
            ioReg.ReadEventTrailer();
            done =true;
          }
        }
      }

      spillCount++;
    }
    file.Write();

    delete data_io;
    delete data_tdc;
    delete data_tr;

    if ( !tdc.DisArm() )
      return 0;
    if ( !trigger.DisArm() )
      return 0;
    if ( !controler.DisArm() )
      return 0;

    return 1;
  } catch (MiceDAQException e) {
    std::cout << e.GetLocation() << std::endl;
    std::cout << e.GetDescription() << std::endl;
    return 0;
  } catch ( MDexception & e) {
    cout << "Unpacking exception: " << endl;
    cout << e.GetDescription()   << endl;
    file.Write();
    return 0;
  } catch(std::exception & e) {
    cout << "Standard exception: " << endl;
    cout << e.what() << endl;
    return 0;
  }

  return 1;
}








