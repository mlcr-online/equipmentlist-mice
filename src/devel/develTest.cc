#include "MDEv2718.hh"
#include "MDEv1290.hh"
#include "MDEv1731.hh"
#include "MDEv1724.hh"
#include "V1724Def.hh"
#include "MDEv830.hh"
#include "MDEv977.hh"
#include "MDEv1495.hh"

#include "MiceDAQMessanger.hh"
#include "MiceDAQRunStats.hh"
#include "MiceDAQSpillStats.hh"
#include "equipmentList_common.hh"

#include "MDfragmentV1495.h"
#include "MDfragmentV1290.h"
#include "MDfragmentV1724.h"
#include "MDfragmentV1731.h"

#include "TH1I.h"
#include "TCanvas.h"
#include "TImage.h"
#include <bitset>

TH1I *hl, *hi, *hh, *hv, *htr;
MDE_Pointer *data_tdc, *data_tr1;
void Process() {

}

void dumpTrigger(MDE_Pointer *data, int nbr) {
  MDfragmentV1495 df;
  df.SetDataPtr(data, nbr);
  df.Dump();
  int nTr = df.GetNumTriggers();
  for (int xTr=0; xTr<nTr; xTr++) {
    unsigned int hitsMask = df.GetPatternTOF0(xTr);
    std::bitset<20>hits(hitsMask);
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
}


MiceDAQRunStats   runStats   = *MiceDAQRunStats::Instance();
MiceDAQSpillStats spillStats = *MiceDAQSpillStats::Instance();
using namespace std;
int nbStored;

int dumpV1290(MDE_Pointer* dataPtr, int nEv) {

  for (int xPe=0; xPe<nEv; xPe++) {
    cout << "\n\n";
    MDpartEventV1290 pe(dataPtr);
    pe.Dump();
    dataPtr += pe.GetSize()/4;
  }

  return 0;
}

void resetHisto(TH1 *h) {
//  h->Print();
  for(int i=0;i<h->GetNbinsX();i++)
    h->SetBinContent(i+1, 0);
}

int dumpV1724(MDE_Pointer* dataPtr, int nEv) {

  for (int xPe=0; xPe<nEv; xPe++) {
//    cout << "\n\n";
    MDpartEventV1724 pe(dataPtr);
//    pe.Dump();
    for(int xCh=0;xCh<8;xCh++) {
      int n = pe.GetNSamples(xCh);

      if(n) resetHisto(hl);
      for(int xBin=0;xBin<n;xBin++) {
        int xMeas = pe.GetSampleData(xCh, xBin);
        hl->Fill(xBin, xMeas);
        hi->Fill(xBin, xMeas);
      }
    }
    dataPtr += pe.GetSize()/4;
  }

  return 0;
}


int main(int argc, char** argv) {

  int nEvents = 1;
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
                       ("FIFOMode",   1)
                       ("MaxNumEvts", 100);

    if ( !controler.Arm() )
      return 0;

//////////////////////////////////////////////////////////////////////////////

    MDEv977 ioReg;
    ioReg.setParams("GEO", 3)("BaseAddress", 0x11150000);

    if ( !ioReg.ArmTriggerReceiver() )
      return false;

    if ( !ioReg.ArmPartTriggerSelector("TOF1") )
      return false;

    if ( !ioReg.ArmTrailer() )
      return false;

    MDE_Pointer* data_tr = new MDE_Pointer[256];
    ioReg.setDataPtr(data_tr);

//////////////////////////////////////////////////////////////////////////////

    MDEv1724 fadc1;
    fadc1.setParams("GEO",                     21)
                   ("BaseAddress",             0x11140000)
                   ("PostTriggerOffset",       42)
                   ("BufferOrganizationCode",  V1724_OutputBufferSize_1K)
                   ("BlockTransfEventNum",     1024)
                   ("ChannelMask",             0xffff)
                   ("TriggerOverlapping",      1)
                   ("WordsPerEvent",           64)
                   ("UseSoftwareTrigger",      0)
                   ("UseExternalTrigger",      1)
                   ("TriggerIOLevel",          0)
                   ("ZSThreshold",             100);
//                    ("ZSThreshold", 0);

    if ( !fadc1.Arm() )
      return 0;

    int memSizeFadc = fadc1.getMaxMemUsed(50);
    MDE_Pointer *data_fadc1 = new MDE_Pointer[memSizeFadc/4];
    fadc1.setDataPtr(data_fadc1);
    std::cout << "Memory alocated (fADC): " << memSizeFadc << std::endl;

//////////////////////////////////////////////////////////////////////////////

    MDEv1290 tdc;
    tdc.setParams("GEO",             11)
                 ("BaseAddress",     0x11060000)
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
                 ("DetectionEdge",   3)  //1 only trailing; 2 only leading; 3 trailing and leading.
                 ("TDCHeader",       1)
                 ("LSBCode",         3)
                 ("DeadtimeCode",    0);

    if ( !tdc.Arm() )
      return 0;

//     int memSizeTdc = tdc.getMaxMemUsed(nEvents);
    int memSizeTdc = 2048*16;
    data_tdc = new MDE_Pointer[memSizeTdc/4];
    tdc.setDataPtr(data_tdc);
    std::cout << "Memory alocated (TDC): " << memSizeTdc << std::endl;

//////////////////////////////////////////////////////////////////////////////

    MDEv1495 trigger;
    trigger.setParams("GEO",               2)
                     ("BaseAddress",       0x11020000)
                     ("SGOpenDelay",       722050)
                     ("SGCloseDelay",      1023050)
//                     ("SGOpenDelay",       0x1FF)
//                     ("SGCloseDelay",      0x3FF)
                     ("SggCtrl",           0x10F)
//                     ("SggCtrl",           0x110)
//                     ("TriggerLogicCtrl",  TRIGGER_PULS_20KHz | RAND1 | RAND2)
//                     ("TriggerLogicCtrl",  TRIGGER_PULS_2KHz)
//                     ("TriggerLogicCtrl",  TRIGGER_GVA)
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

    MiceDAQMessanger::Instance()->setVerbosity(1);
    TCanvas *c1 = new TCanvas;
    c1->Divide(2,2);
    TImage *img = TImage::Create();

    hi = new TH1I("integrated", "", 128, -0.5, 127.5);
    hl = new TH1I("last"      , "", 128, -0.5, 127.5);

    hh = new TH1I("horizontal", "", 10, -0.5, 9.5);
    hv = new TH1I("vertical"  , "", 10, -0.5, 9.5);

    htr = new TH1I("nTriggers"  , "", 100, 0, 100);

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
//            std::cout << "TDC events: " << tdc.getNEventsStored() << "    "
//                      << "fADC events: " << fadc1.getNEventsStored() << std::endl;
//            trigger.softwareClear();
            spillStats["SpillNumber"]++;
            runStats["NumberOfSpills"]++;
            ioReg.ReadEventTrailer();
          } else if (ioReg.getEventType() == PHYSICS_EVENT) {
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
            std::cout << "Triggers: " << trigger.getNTriggers() << "    "
                      << "TDC events: "  << tdc.getNTriggers() << "    "
                      << "fADC events: " << fadc1.getNTriggers() << std::endl;

            int nEv_tr  = trigger.getNTriggers();
            htr->Fill(nEv_tr);
            int nbr_tr  = trigger.ReadEvent();
            if (nbr_tr)
              dumpTrigger(data_tr, nbr_tr);

            int nEv_tdc = tdc.getNEventsStored();
            int nbr_tdc = tdc.ReadEvent();
            if (nbr_tdc)
              dumpV1290(data_tdc, nEv_tdc);

            int nEv_fadc1 = fadc1.getNEventsStored();
            int nbr_fadc1 = fadc1.ReadEvent();
            if (nbr_fadc1)
              dumpV1724(data_fadc1, nEv_fadc1);

            nbStored += ioReg.ReadEventTrailer();
          } else if (ioReg.getEventType() == END_OF_BURST){
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
//           std::cout << "TDC events: " << tdc.getNEventsStored() << "    "
//                     << "fADC events: " << fadc1.getNEventsStored() << std::endl;
//             int TrNBR = trigger.ReadEvent();
//             DumpTrigger(data_tr1, TrNBR);
//             std::cout << " nTr: " << trigger.getNTriggers() << "  Read " << nbStored << " bytes"
//                       << "  Busy time: " << 0.000025*(trigger.getPhysBusy(0)) << " ms" << std::endl;
//             trigger.getPhysBusy(1);
            ioReg.ReadEventTrailer();
            done =true;
          }
        }
      }

      nbStored = 0;
      if ( !(spillCount%4) ) {
        cout << "### Update ###\n";
        c1->cd(1);
        hh->Draw();
        c1->cd(2);
        hv->Draw();
        c1->cd(3);
        htr->Draw();
        img->FromPad(c1);
        img->WriteImage("canvas.png");
      }
      spillCount++;
    }

    free(data_tdc);
    free(data_fadc1);
    free(data_tr);

    if ( !controler.DisArm() )
      return 0;

    return 1;
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

  return 1;
}


