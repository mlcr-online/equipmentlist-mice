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

#include "TH1I.h"
#include "TCanvas.h"
#include "TImage.h"
#include <bitset>

TH1I *hh, *hv;
MDE_Pointer *data_tdc, *data_tr1;
void Process() {

}

void DumpTrigger(MDE_Pointer *data, int nbr) {
  MDfragmentV1495 df;
  df.SetDataPtr(data, nbr);
  df.Dump();
  int nTr = df.GetNumTriggers();
  for (int xTr=0; xTr<nTr; xTr++) {
    unsigned int hitsMask = df.GetPatternTOF1(xTr);
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
void dumpEventV1724(int* dataPtr);
int dumpEventV1290(MDE_Pointer* dataPtr);
int nbStored;

int dumpEventV1290(MDE_Pointer* dataPtr, int nEv) {

  for (int xPe=0; xPe<nEv; xPe++) {
    cout << "\n\n";
    MDpartEventV1290 pe(dataPtr);
    pe.Dump();
    dataPtr += pe.GetSize()/4;
  }

  return 0;
}

#include <time.h>
clock_t startm, stopm;
#define START if ( (startm = clock()) == -1) {printf("Error calling clock");exit(1);}
#define STOP if ( (stopm = clock()) == -1) {printf("Error calling clock");exit(1);}
#define PRINTTIME printf( "%6.3f microseconds used by the processor.", ((double)stopm-startm)/(1e6*CLOCKS_PER_SEC));

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
    //char *test = "blah";
    //bool *test_b;
    fadc1.setParams("GEO",                     21)
                   ("BaseAddress",             0x110A0000)
                   ("PostTriggerOffset",       42)
                   ("BUfferOrganizationCode",  V1724_OutputBufferSize_1K)
                   ("BlockTransfEventNum",     1024)
                   ("ChannelMask",             0xffff)
                   ("TriggerOverlapping",      1)
                   ("WordsPerEvent",           32)
                   ("UseSoftwareTrigger",      1)
                   ("UseExternalTrigger",      1)
                   ("TriggerIOLevel",          1)
                   ("ZSThreshold",             100);
//                    ("ZSThreshold", 0);

//     if ( !fadc1.Arm() )
//       return 0;

    int memSizeFadc = fadc1.getMaxMemUsed(50);
    MDE_Pointer *data_fadc = new MDE_Pointer[memSizeFadc/4];
    fadc1.setDataPtr(data_fadc);
    std::cout << "Memory alocated (fADC): " << memSizeFadc << std::endl;

//////////////////////////////////////////////////////////////////////////////

    MDEv1290 tdc;
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
//                      ("SGOpenDelay",       0x1FF)
//                      ("SGCloseDelay",      0x3FF)
//                      ("SggCtrl",           0x10F)
                     ("SggCtrl",           0x110)
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
    data_tr1 = new MDE_Pointer[memSizeTr/4];
    trigger.setDataPtr(data_tr1);
    std::cout << "Memory alocated (Trigger): " << memSizeTr << std::endl;
//////////////////////////////////////////////////////////////////////////////
//     int xEv(0);
//     while (xEv<nEvents) {
//       fadc1.softwareTrigger();
//       tdc.softwareTrigger();
//       usleep(1);
//       xEv++;
//     }
// 
//     ioReg.setOutput(0x4000);
// //     MiceDAQMessanger::Instance()->setVerbosity(0);
//     nbStored += tdc.ReadEvent();
// //     nbStored += fadc1.ReadEvent();
//     ioReg.unsetOutput(0x4000);

//     ioReg.setOutput(0x800);
    MiceDAQMessanger::Instance()->setVerbosity(1);
    TCanvas *c1 = new TCanvas;
    c1->Divide(2,1);
    TImage *img = TImage::Create();

    hh = new TH1I("horizontal", "", 10, -0.5, 9.5);
    hv = new TH1I("vertical"  , "", 10, -0.5, 9.5);

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
//             std::cout << "TDC events: " << tdc.getNEventsStored() << "    "
//                       << "fADC events: " << fadc1.getNEventsStored() << std::endl;
            trigger.softwareClear();
            spillStats["SpillNumber"]++;
            runStats["NumberOfSpills"]++;
          } else if (ioReg.getEventType() == PHYSICS_EVENT) {
            std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
//             std::cout << "TDC events: "  << tdc.getNEventsStored()   << std::endl;
            int nEv = tdc.getNEventsStored();
            nbStored = tdc.ReadEvent();
            if (nbStored)
              dumpEventV1290(data_tdc, nEv);
//             std::cout << "fADC events: " << fadc1.getNEventsStored() << std::endl;
//             nbStored += fadc1.ReadEvent();
//             trigger.getNSpils();
            nbStored = trigger.ReadEvent();
        if (nbStored>8) DumpTrigger(data_tr1, nbStored);
//             usleep(1);
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

//       trigger.setDataPtr(dataPtr);
//       int nbr = trigger.ReadEvent();
//       Dump(dataPtr, nbr);

//       usleep(150000);
//       sleep(1);
      nbStored = 0;
      if ( !(spillCount%4) ) {
        cout << "### Update ###\n";
        c1->cd(1);
        hh->Draw();
        c1->cd(2);
        hv->Draw();
        img->FromPad(c1);
        img->WriteImage("canvas.png");
      }
      spillCount++;
    }

//     nbStored = 0;
//     bool done = false;
// //     int duration = 20;
// //     time_t t1=time(NULL), t2;
//     while (!done) {
// //       t2=time(NULL);
// //       if ( (t2-t1) > duration ) done = true;
//       if ( ioReg.EventArrivedTriggerReceiver() ){
//         ioReg.ReadEventTriggerReceiver();
//         if ( ioReg.getEventType() == START_OF_BURST ) {
//           std::cout << "\n---------------- \n"  << "time: " << t2-t1 << "s" << std::endl;
//           std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
// //           std::cout << "TDC events: " << tdc.getNEventsStored() << "    "
// //                     << "fADC events: " << fadc1.getNEventsStored() << std::endl;
//           spillStats["SpillNumber"] ++;
//           runStats["NumberOfSpills"] ++;
//         } else if (ioReg.getEventType() == PHYSICS_EVENT ) {
//           std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
// //           std::cout << "TDC events: " << tdc.getNEventsStored() << "    "
// //                     << "fADC events: " << fadc1.getNEventsStored() << std::endl;
// //           nbStored += tdc.ReadEvent();
// //           nbStored += fadc1.ReadEvent();
//           nbStored += ioReg.ReadEventTrailer();
//         } else if ( ioReg.getEventType() == END_OF_BURST ){
//           std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
// //           std::cout << "TDC events: " << tdc.getNEventsStored() << "    "
// //                     << "fADC events: " << fadc1.getNEventsStored() << std::endl;
//           ioReg.ReadEventTrailer();
//         }
//       }
//     }
    /*
    std::cout << "\nData recorded: " << nbStored/1e3 << " KB\n";

    int* data_tmp = data_fadc;
    for(int j=0;j<nEvents;j++) {
      dumpEventV1724(data_tmp);
      int evSize = (*data_tmp) & 0xFFFFFFF;
      data_tmp += evSize;
    }

    data_tmp = data_tdc;
    for(int j=0;j<nEvents;j++) {
      int nw = dumpEventV1290(data_tmp);
      data_tmp += nw;
    }
    */
    //free(data_tdc);
    free(data_fadc);
    free(data_tr);
//     if( !fadc1.DisArm() )
//       return 0;

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


