#include <math.h>

#include "MDETestLib.hh"
#include "DAQManager.hh"

int nbStored;
MiceDAQSpillStats spillStats = *MiceDAQSpillStats::Instance();
MiceDAQRunStats   runStats   = *MiceDAQRunStats::Instance();

void SetDefaultTrigger(MDEv1495 *trigger){
  trigger->setParams("SGOpenDelay",      722050)
                    ("SGCloseDelay",     1023050)
                    ("SggCtrl",          0x1F0F)
                    ("TOF0Mask",         0xfffff)
                    ("TOF1Mask",         0xfffff)
                    ("TOF2Mask",         0xfffff)
                    ("PartTrVetoLenght", 150);
}

void SetDefaultV830(MDEv830 *scaler) {
  scaler->setParams("ChannelMask", 0xffffffff)
                   ("TriggerMode", 1)
                   ("UseHeader",   1)
                   ("UseNarrow",   1)
                   ("AutoReset",   1);
}

void SetDefaultV1290(MDEv1290 *tdc) {
  tdc->setParams("ChannelMask",    0xffff)
                ("UseEventFIFO",   1)
                ("UseExtendedTTT", 1)
                ("DoInit",         1)
                ("TriggerMode",    1)
                ("WinWidth",       1200)
                ("WinOffset",     -28)
                ("SwMargin",       8)
                ("RejMargin",      2)
                ("TTSubtraction",  1)
                ("DetectionEdge",  2)
                ("TDCHeader",      1)
                ("LSBCode",        3)
                ("DeadtimeCode",   0);
}

void SetDefaultV1731(MDEv1731 *fadc) {
  fadc->setParams("PostTriggerOffset",      42)
                 ("BufferOrganizationCode", 0x0A)
                 ("BlockTransfEventNum",    1024)
                 ("ChannelMask",            0xffff)
                 ("TriggerOverlapping",     1)
                 ("WordsPerEvent",          64)
                 ("UseSoftwareTrigger",     1)
                 ("UseExternalTrigger",     1)
                 ("DualEdgeSampling",       0);

  char* zs_cut_char = getenv("ZSThresholdV1731");
  if(zs_cut_char==NULL)
    fadc->setParams("ZSThreshold",          3);
  else {
    int zs_cut_int;

    stringstream ss(zs_cut_char ,ios_base::out|ios_base::in );
    ss >> zs_cut_int;
    if (ss.fail()) {
      MiceDAQMessanger *mde_messanger = MiceDAQMessanger::Instance();
      *(mde_messanger->getStream(fadc)) << "ERROR in SetDefaultV1731 : The ZSThresholdV1731("
                  << zs_cut_char << ") does not match an integer";
      mde_messanger->sendMessage(MDE_FATAL);
      return;
    }
    fadc->setParams("ZSThreshold",          zs_cut_int);
  }
}

void SetDefaultV1724(MDEv1724 *fadc) {
  fadc->setParams("PostTriggerOffset",      42)
                 ("BufferOrganizationCode", 0x0A)
                 ("BlockTransfEventNum",    0xff)
                 ("ChannelMask",            0xffff)
                 ("TriggerOverlapping",     1)
                 ("WordsPerEvent",          64)
                 ("UseSoftwareTrigger",     1)
                 ("UseExternalTrigger",     1);

  char* zs_cut_char = getenv("ZSThresholdV1724");
  if(zs_cut_char==NULL)
    fadc->setParams("ZSThreshold",          30);
  else {
    int zs_cut_int;

    stringstream ss(zs_cut_char ,ios_base::out|ios_base::in );
    ss >> zs_cut_int;
    if (ss.fail()) {
      MiceDAQMessanger *mde_messanger = MiceDAQMessanger::Instance();
      *(mde_messanger->getStream(fadc)) << "ERROR in SetDefaultV1724 : The ZSThresholdV1724("
                  << zs_cut_char << ") does not match an integer";
      mde_messanger->sendMessage(MDE_FATAL);
      return;
    }
    fadc->setParams("ZSThreshold",          zs_cut_int);
  }
}

void setDefaultVRB(MDEvRB *vrb, int geodbb1) {

  vrb->setParams("N_DBBs",            6)
                ("ChannelMask0_DBB1", 0xffffffaa)
                ("ChannelMask1_DBB1", 0xbbffffff)
                ("Geo_DBB1",          geodbb1)
                ("ChannelMask0_DBB2", 0xffffffff)
                ("ChannelMask1_DBB2", 0xfffccfff)
                ("Geo_DBB2",          geodbb1+1)
                ("ChannelMask0_DBB3", 0xffffffff)
                ("ChannelMask1_DBB3", 0xffdfffdf)
                ("Geo_DBB3",          geodbb1+2)
                ("ChannelMask0_DBB4", 0xfefffeff)
                ("ChannelMask1_DBB4", 0xff0fffff)
                ("Geo_DBB4",          geodbb1+3)
                ("ChannelMask0_DBB5", 0xfffff4ff)
                ("ChannelMask1_DBB5", 0xf3ffffff)
                ("Geo_DBB5",          geodbb1+4)
                ("ChannelMask0_DBB6", 0xfffff67f)
                ("ChannelMask1_DBB6", 0xff88ffff)
                ("Geo_DBB6",          geodbb1+5);
}

void addvrb(DAQManager *myDAQ,
            MDEvRB *vrb,
            MDEDBB *dbb1,
            MDEDBB *dbb2,
            MDEDBB *dbb3,
            MDEDBB *dbb4,
            MDEDBB *dbb5,
            MDEDBB *dbb6,
            int vrbBA,
            int vrbgeo,
            int geodbb1) {
  vrb->setParams("GEO",               vrbgeo)
                ("BaseAddress",       vrbBA)
                ("DBB_FV",            8)
                ("N_DBBs",            6)
                ("ChannelMask0_DBB1", 0xffffffff)
                ("ChannelMask1_DBB1", 0xffffffff)
                ("Geo_DBB1",          geodbb1)
                ("ChannelMask0_DBB2", 0xffffffff)
                ("ChannelMask1_DBB2", 0xffffffff)
                ("Geo_DBB2",          geodbb1+1)
                ("ChannelMask0_DBB3", 0xffffffff)
                ("ChannelMask1_DBB3", 0xffffffff)
                ("Geo_DBB3",          geodbb1+2)
                ("ChannelMask0_DBB4", 0xffffffff)
                ("ChannelMask1_DBB4", 0xffffffff)
                ("Geo_DBB4",          geodbb1+3)
                ("ChannelMask0_DBB5", 0xffffffff)
                ("ChannelMask1_DBB5", 0xffffffff)
                ("Geo_DBB5",          geodbb1+4)
                ("ChannelMask0_DBB6", 0xffffffff)
                ("ChannelMask1_DBB6", 0xffffffff)
                ("Geo_DBB6",          geodbb1+5);

  dbb1->setParams("GEO",             geodbb1)
                ("BaseAddressVRB",  vrbBA)
                ("IdInVRB",         1);
  dbb2->setParams("GEO",             geodbb1+1)
                ("BaseAddressVRB",  vrbBA)
                ("IdInVRB",         2);
  dbb3->setParams("GEO",             geodbb1+2)
                ("BaseAddressVRB",  vrbBA)
                ("IdInVRB",         3);
  dbb4->setParams("GEO",             geodbb1+3)
                ("BaseAddressVRB",  vrbBA)
                ("IdInVRB",         4);
  dbb5->setParams("GEO",             geodbb1+4)
                ("BaseAddressVRB",  vrbBA)
                ("IdInVRB",         5);
  dbb6->setParams("GEO",             geodbb1+5)
                ("BaseAddressVRB",  vrbBA)
                ("IdInVRB",         6);

  myDAQ->addToArmList(vrb);
  myDAQ->addToReadList(vrb);

}

///////////////////////////////////// Trigger //////////////////////////////////
#include "MDfragmentV1495.h"

bool processSpill(MDE_Pointer *dataPtr, unsigned int nTr) {
  MDfragmentV1495 spill;
  spill.SetDataPtr(dataPtr, nbStored);
  spill.Dump();
  if (spill.GetNumTriggers() != nTr)
    return false;

  return true;
}

bool testV1495(int ba) {

  int nEvents = 4;

  MDEv977 ioReg;
  ioReg.setParams("GEO", 3)("BaseAddress", 0x11150000);

  if ( !ioReg.ArmTriggerReceiver() )
    return false;

  if ( !ioReg.ArmTrailer() )
    return false;

  MDE_Pointer *data_io = new MDE_Pointer[256];
  ioReg.setDataPtr(data_io);

  MDEv1495 trigger;
  trigger.setParams("GEO",               1)
//                   ("BaseAddress",       0x11020000)
                   ("BaseAddress",       ba)
                   ("TriggerLogicCtrl",  TRIGGER_TOF1_OR);
  SetDefaultTrigger(&trigger);

  if ( !trigger.Arm() )
    return 0;

  int memSizeTr = trigger.getMaxMemUsed();
  MDE_Pointer *data_tr = new MDE_Pointer[memSizeTr/4];
  trigger.setDataPtr(data_tr);

  int spillCount=0;
  while (spillCount<nEvents) {
    std::cout << "\n \n Generating Spill gate " << spillCount << " ...\n";

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
          int nTr = trigger.getNTriggers();
          if (nTr)
            return false;
      
          nbStored = trigger.ReadEvent();
          if( !processSpill(data_tr, nTr) )
            return false;

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

  delete data_tr;
  delete data_io;

//  trigger.setParams("TriggerLogicCtrl",  TRIGGER_TOF1_OR);
//  if( !trigger.Arm() )
//    return false;

  return true;
}


/////////////////////////////////////  TDC //////////////////////////////////
#include "MDpartEventV1290.h"

bool processEventV1290(int* dataPtr, int &wc, bool v) {
  MDpartEventV1290 pe;
  pe.SetDataPtr(dataPtr);
  wc = pe.GetWordCount();
  int nHits = pe.GetNHits();

  if (v) pe.Dump();

  if(nHits!=0)
    return false;

  return true;
}

bool testV1290(int ba) {

  int nEvents  = 500;
  MDEv1290 tdc;
  tdc.setParams("GEO", 3)
               ("BaseAddress", ba);
  SetDefaultV1290(&tdc);

  if ( !tdc.Arm() )
    return false;

  for(int j=0;j<nEvents;j++) {
    usleep(100);
    tdc.softwareTrigger();
  }

  int nTr = tdc.getNTriggers();
  int nEvSt = tdc.getNEventsStored();
  if (nTr!=nEvents || nEvSt!=nEvents)
    return false;

  tdc.getStatusReg();
  std::cout << "NTriggers = " << nTr <<
               "  NEventsStored = " << nEvSt << std::endl;

  int memSize = tdc.getMaxMemUsed(nEvents);
  std::cout << "Memory allocated: " << memSize << std::endl;

  MDE_Pointer* data = (MDE_Pointer*) malloc (memSize);
  tdc.setDataPtr(data);

  nbStored = tdc.ReadEvent();
  spillStats["DataRecorded"] += nbStored;

  int* data_tmp = (int*)data;

  for(int j=0;j<nEvents;j++) {
    int nw; bool v(false);
    if (j<10) v=true;
    if ( !processEventV1290(data_tmp, nw, v) )
      return false;

    data_tmp += nw;
  }

  if ( !tdc.DisArm() )
    return false;

  free(data);

  return true;
}

///////////////////////////////////// IO-Register ////////////////////////////////
bool testV977(int ba, int duration) {

  MiceDAQMessanger::Instance()->setVerbosity(1);
  MDEv977 ioReg;
  ioReg.setParams("GEO", 2)("BaseAddress", ba);

  if ( !ioReg.ArmTriggerReceiver() )
    return false;

  if ( !ioReg.ArmTrailer() )
    return false;

  MDE_Pointer* data = (MDE_Pointer*) malloc (1024);
  ioReg.setDataPtr(data);

  bool done = false;
  time_t t1=time(NULL), t2;
  int nExp = static_cast<int>(duration*0.79+.5);
  std::cout << "V977 test duration: " << duration << " s"
            << "\nExpected number of spills: " << nExp << "+-1" << std::endl;

  while (!done) {
    t2=time(NULL);
    if ( (t2-t1) > duration ) done = true;
    if ( ioReg.EventArrivedTriggerReceiver() ){
      ioReg.ReadEventTriggerReceiver();
      if ( ioReg.getEventType() == START_OF_BURST ) {
        std::cout << "\n---------------- \n"  << "time: " << t2-t1 << "s" << std::endl;
        runStats["NumberOfSpills"] ++;
      }
      std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
      ioReg.ReadEventTrailer();
    }
  }


  if( !ioReg.DisArmTrailer() )
    return false;

  if (!ioReg.DisArmTriggerReceiver() )
    return false;

  free(data);

  if (fabs(runStats["NumberOfSpills"] - nExp) > 2)
    return false;

  return true;
}



//////////////////////////// Scaler /////////////////////////////////////
#include "MDfragmentV830.h"

bool processEventV830(int* dataPtr, int nbStored) {

  MDfragmentV830 df;
  df.SetDataPtr(dataPtr, nbStored);

  df.Dump();
  for (unsigned int xCh=0; xCh<32; xCh++) {
    if (df.GetChannel(xCh)!=xCh)
      return false;

    if (df.GetMeasurement(xCh)!=0)
      return false;
  }

  return true;
}


bool testV830(int ba) {

  MDEv830 scaler;
  scaler.setParams("GEO", 11)
                  ("BaseAddress", ba);
  SetDefaultV830(&scaler);

  if ( !scaler.Arm() )
    return false;

  scaler.softwareTrigger();
  int memSizeScaler = scaler.getMaxMemUsed(1);
  std::cout << "Memory allocated: " << memSizeScaler << std::endl;

  MDE_Pointer* data = (MDE_Pointer*) malloc (memSizeScaler);
  scaler.setDataPtr(data);
  nbStored = scaler.ReadEvent();
  spillStats["DataRecorded"] += nbStored;
  int* data_tmp = (int*)data;
  if ( !processEventV830(data_tmp, nbStored) )
    return false;

  free(data);

  if( !scaler.DisArm() )
    return false;

  return true;
}


/////////////////////// Flash ADC V1731 ///////////////////////////////////
#include "MDpartEventV1731.h"

bool processEventV1731(int* dataPtr) {

  MDpartEventV1731 pe;
  pe.SetDataPtr(dataPtr);
  pe.Dump();
  if (pe.GetWordCount() > 12)
    return false;

  return true;
}

bool testV1731(int ba) {

  int nEvents = 32;
  MDEv1731 fadc;
  fadc.setParams("GEO", 21)
                 ("BaseAddress", ba);
  SetDefaultV1731(&fadc);

  if ( !fadc.Arm() )
    return false;

  int memSizeFadc1 = fadc.getMaxMemUsedBLT(nEvents);
  MDE_Pointer* data = (MDE_Pointer*) malloc (memSizeFadc1);
  std::cout << "Memory alocated: " << memSizeFadc1 << "(" << fadc.getMaxMemUsed(nEvents) << ")" << std::endl;
  fadc.setDataPtr(data);

  for(int j=0;j<nEvents;j++) {
    usleep(1000);
    fadc.softwareTrigger();
  }

  nbStored = fadc.ReadEventZS();
  spillStats["DataRecorded"] += nbStored;

  int* data_tmp = (int*)data;
  for(int j=0;j<nEvents;j++) {
    if ( !processEventV1731(data_tmp) )
      return false;

    int evSize = (*data_tmp) & 0xFFFFFFF;
    data_tmp += evSize;
  }

  free(data);
  if( !fadc.DisArm() )
    return false;

  return true;
}


/////////////////////// Flash ADC V1724 ///////////////////////////////////
#include "MDpartEventV1724.h"

bool processEventV1724(int* dataPtr) {

  MDpartEventV1724 pe;
  pe.SetDataPtr(dataPtr);
  pe.Dump();
  if (pe.GetWordCount() > 12)
    return false;

  return true;
}

bool testV1724(int ba) {

  int nEvents = 32;
  MDEv1724 fadc;
  fadc.setParams("GEO", 22)
                 ("BaseAddress", ba);
  SetDefaultV1724(&fadc);

  if ( !fadc.Arm() )
    return false;

  int memSizeFadc2 = fadc.getMaxMemUsed(nEvents);
  MDE_Pointer* data2 = (MDE_Pointer*) malloc (memSizeFadc2);
  std::cout << "Memory alocated: " << memSizeFadc2 << std::endl;
  fadc.setDataPtr(data2);

  for(int j=0;j<nEvents;j++) {
    usleep(1000);
    fadc.softwareTrigger();
  }

  nbStored = fadc.ReadEventZS();
  spillStats["DataRecorded"] += nbStored;

  int* data_tmp = (int*)data2;
  for(int j=0;j<nEvents;j++) {
    if ( !processEventV1724(data_tmp) )
      return false;

    int evSize = (*data_tmp) & 0xFFFFFFF;
    data_tmp += evSize;
  }

  free(data2);
  if( !fadc.DisArm() )
    return false;

  return true;
}

//////////////////////  VCB  ///////////////////////////////////

bool testVCB(int ba) {
  MDEvCB vcb;
  vcb.setParams("GEO",           0)
               ("BaseAddress",  ba)
               ("N_FEBs",       16);

  if (!vcb.Arm())
    return false;

  return true;
}

//////////////////////  VRB  ///////////////////////////////////

bool testVRB(int ba) {
  MDEvRB vrb;
  vrb.setParams("GEO",               0)
               ("BaseAddress",       ba)
               ("DBB_FV",            8);

  int vrbNum = ( (ba & 0xf000000) >> 24 ) - 1;
  setDefaultVRB(&vrb, vrbNum*6);

  if (!vrb.Arm())
    return false;

  for (int dbbId=1; dbbId<7; dbbId++) {
    if (vrb.getNTriggers(dbbId))
     return false;

    if (vrb.getNumDataWords(dbbId))
      return false;
  }

  if (!vrb.DisArm())
    return false;

  usleep(1000);
  return true;
}

/////////////////////// miceacq14 Integration test ///////////////////////////////////

bool testmiceacq14() {

  DAQManager myDAQ;

  MDEv977 ioReg;
  ioReg.setParams("GEO", 2)("BaseAddress", 0x11150000);

  if ( !ioReg.ArmTriggerReceiver() )
    return false;

  if ( !ioReg.ArmTrailer() )
    return false;
  
  MDE_Pointer* data_io = new MDE_Pointer[1024];

  int mem_size = 8*1024*1024;
  myDAQ.MemBankInit(mem_size);
  ioReg.setDataPtr(data_io);

  // Trigger
  MDEv1495 trigger;
  trigger.setParams("GEO",               1)
                   ("BaseAddress",       0x11020000)
                   ("TriggerLogicCtrl",  TRIGGER_PULS_50KHz);
  SetDefaultTrigger(&trigger);
  myDAQ.addToArmList(&trigger);
  myDAQ.addToReadList(&trigger);
/*
  // Scaler
  MDEv830 scaler;
  scaler.setParams("GEO",                2)
                  ("BaseAddress",        0x11030000);
  SetDefaultV830(&scaler);
  myDAQ.addToArmList(&scaler);
  myDAQ.addToReadList(&scaler);
*/
  // TDCs
  MDEv1290 tdc1;
  tdc1.setParams("GEO",                  3)
               ("BaseAddress",           0x11060000);
  SetDefaultV1290(&tdc1);
  myDAQ.addToArmList(&tdc1);
  myDAQ.addToReadList(&tdc1);

  MDEv1290 tdc2;
  tdc2.setParams("GEO",                  4)
               ("BaseAddress",           0x11070000);
  SetDefaultV1290(&tdc2);
  myDAQ.addToArmList(&tdc2);
  myDAQ.addToReadList(&tdc2);

  MDEv1290 tdc3;
  tdc3.setParams("GEO",                  5)
               ("BaseAddress",           0x11080000);
  SetDefaultV1290(&tdc3);
  myDAQ.addToArmList(&tdc3);
  myDAQ.addToReadList(&tdc3);

  MDEv1290 tdc4;
  tdc4.setParams("GEO",                  6)
               ("BaseAddress",           0x11090000);
  SetDefaultV1290(&tdc4);
  myDAQ.addToArmList(&tdc4);
  myDAQ.addToReadList(&tdc4);

  MDEv1724 fadc1;
  fadc1.setParams("GEO",                21)
                 ("BaseAddress",        0x110a0000);
  SetDefaultV1724(&fadc1);
  myDAQ.addToArmList(&fadc1);
  myDAQ.addToReadList(&fadc1);

  MDEv1724 fadc2;
  fadc2.setParams("GEO",                22)
                 ("BaseAddress",        0x110b0000);
  SetDefaultV1724(&fadc2);
  myDAQ.addToArmList(&fadc2);
  myDAQ.addToReadList(&fadc2);

  MDEv1724 fadc3;
  fadc3.setParams("GEO",                23)
                 ("BaseAddress",        0x110c0000);
  SetDefaultV1724(&fadc3);
  myDAQ.addToArmList(&fadc3);
  myDAQ.addToReadList(&fadc3);

  MDEv1724 fadc4;
  fadc4.setParams("GEO",                24)
                 ("BaseAddress",        0x110d0000);
  SetDefaultV1724(&fadc4);
  myDAQ.addToArmList(&fadc4);
  myDAQ.addToReadList(&fadc4);

  MDEv1724 fadc5;
  fadc5.setParams("GEO",                25)
                 ("BaseAddress",        0x110e0000);
  SetDefaultV1724(&fadc5);
  myDAQ.addToArmList(&fadc5);
  myDAQ.addToReadList(&fadc5);

  MDEv1724 fadc6;
  fadc6.setParams("GEO",                26)
                 ("BaseAddress",        0x110f0000);
  SetDefaultV1724(&fadc6);
  myDAQ.addToArmList(&fadc6);
  myDAQ.addToReadList(&fadc6);

  MDEv1724 fadc7;
  fadc7.setParams("GEO",                27)
                 ("BaseAddress",        0x11100000);
  SetDefaultV1724(&fadc7);
  myDAQ.addToArmList(&fadc7);
  myDAQ.addToReadList(&fadc7);

  MDEv1724 fadc8;
  fadc8.setParams("GEO",                28)
                 ("BaseAddress",        0x11110000);
  SetDefaultV1724(&fadc8);
  myDAQ.addToArmList(&fadc8);
  myDAQ.addToReadList(&fadc8);

  MDEv1724 fadc9;
  fadc9.setParams("GEO",                29)
                 ("BaseAddress",        0x11120000);
  SetDefaultV1724(&fadc9);
  myDAQ.addToArmList(&fadc9);
  myDAQ.addToReadList(&fadc9);

  MDEv1731 fadc10;
  fadc10.setParams("GEO",               30)
                 ("BaseAddress",        0x11140000);
  SetDefaultV1731(&fadc10);
  myDAQ.addToArmList(&fadc10);
  myDAQ.addToReadList(&fadc10);


  if ( !myDAQ.Arm() )
    return false;

  int spillCount=0;
  while (spillCount<20000) {
    std::cout << "\n \n Generating Spill gate " << spillCount << " ...\n";

    bool done = false;     
    while (!done) {
      if ( ioReg.EventArrivedTriggerReceiver() ) {
        ioReg.ReadEventTriggerReceiver();
        if (ioReg.getEventType() == START_OF_BURST) {
          std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
//          trigger.softwareClear();
          ioReg.ReadEventTrailer();
          std::cout << "Event Arrived end\n.";
        } else if (ioReg.getEventType() == PHYSICS_EVENT) {
          std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
          int nTr;
          if ( !myDAQ.compareNTriggers(nTr) )
            return false;
          int nbr = myDAQ.ReadEvent();
          std::cout << "Event size: " << nbr << std::endl;

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


  if ( !myDAQ.DisArm() )
    return false;

  delete data_io;

  return true;  
}


/////////////////////// miceacq20 Integration test ///////////////////////////////////
#include "MDfragmentDBB.h"

bool processEventDBB(MDE_Pointer *dataPtr, int nTr) {

  if (nTr==0 && nbStored==0)
    return true;

  MDfragmentDBB df;
  df.SetDataPtr(dataPtr, nbStored);
//  df.Dump();

  int tr_count  = df.GetTriggerCount();
  int hit_count = df.GetHitCount();
//  cout << tr_count << " " << hit_count << endl;
  if (tr_count != hit_count)
    return false;

  if(tr_count != nTr)
    return false;

  return true;
}

bool testmiceacq20() {

  DAQManager myDAQ;

  MDEv977 ioReg;
  ioReg.setParams("GEO", 2)("BaseAddress", 0xe0020000);

  if ( !ioReg.ArmTriggerReceiver() )
    return false;

  if ( !ioReg.ArmTrailer() )
    return false;
  
  MDE_Pointer* data_io  = new MDE_Pointer[1024];
  MDE_Pointer* data_dbb = new MDE_Pointer[1024*1024];

  int mem_size = 8*1024*1024;
  myDAQ.MemBankInit(mem_size);
  ioReg.setDataPtr(data_io);

  MDEvRB vrb1,vrb2,vrb3,vrb4,vrb5,vrb6,vrb7,vrb8;

  std::vector <MDEDBB*> dbbs;

  for (int i=0;i<48;i++) {
    dbbs.push_back(new MDEDBB);
    dbbs[i]->setDataPtr(data_dbb);
  }

  addvrb(&myDAQ,&vrb1,dbbs[0],  dbbs[1],  dbbs[2],  dbbs[3],  dbbs[4],  dbbs[5],  0x11ff0000,1,0);
  addvrb(&myDAQ,&vrb2,dbbs[6],  dbbs[7],  dbbs[8],  dbbs[9],  dbbs[10], dbbs[11], 0x22ff0000,2,6);
  addvrb(&myDAQ,&vrb3,dbbs[12], dbbs[13], dbbs[14], dbbs[15], dbbs[16], dbbs[17], 0x33ff0000,3,12);
  addvrb(&myDAQ,&vrb4,dbbs[18], dbbs[19], dbbs[20], dbbs[21], dbbs[22], dbbs[23], 0x44ff0000,4,18);
  addvrb(&myDAQ,&vrb5,dbbs[24], dbbs[25], dbbs[26], dbbs[27], dbbs[28], dbbs[29], 0x55ff0000,5,24);
  addvrb(&myDAQ,&vrb6,dbbs[30], dbbs[31], dbbs[32], dbbs[33], dbbs[34], dbbs[35], 0x66ff0000,6,30);
  addvrb(&myDAQ,&vrb7,dbbs[36], dbbs[37], dbbs[38], dbbs[39], dbbs[40], dbbs[41], 0x77ff0000,7,36);
  addvrb(&myDAQ,&vrb8,dbbs[42], dbbs[43], dbbs[44], dbbs[45], dbbs[46], dbbs[47], 0x88ff0000,8,42);


  MDEv1731 fadc0;
  fadc0.setParams("GEO",            0)
                 ("BaseAddress",   0xe00e0000);
  SetDefaultV1731(&fadc0);
  myDAQ.addToArmList(&fadc0);
  myDAQ.addToReadList(&fadc0);

  MDEv1731 fadc1;
  fadc1.setParams("GEO",           1)
                 ("BaseAddress",   0xe00f0000);
  SetDefaultV1731(&fadc1);
  myDAQ.addToArmList(&fadc1);
  myDAQ.addToReadList(&fadc1);

  MDEv1731 fadc2;
  fadc2.setParams("GEO",            2)
                 ("BaseAddress",   0xe0100000);
  SetDefaultV1731(&fadc2);
  myDAQ.addToArmList(&fadc2);
  myDAQ.addToReadList(&fadc2);

  MDEv1731 fadc3;
  fadc3.setParams("GEO",           3)
                 ("BaseAddress",   0xe0110000);
  SetDefaultV1731(&fadc3);
  myDAQ.addToArmList(&fadc3);
  myDAQ.addToReadList(&fadc3);

  MDEv1731 fadc4;
  fadc4.setParams("GEO",            4)
                 ("BaseAddress",   0xe0120000);
  SetDefaultV1731(&fadc4);
  myDAQ.addToArmList(&fadc4);
  myDAQ.addToReadList(&fadc4);

  MDEv1731 fadc5;
  fadc5.setParams("GEO",           5)
                 ("BaseAddress",   0xe0130000);
  SetDefaultV1731(&fadc5);
  myDAQ.addToArmList(&fadc5);
  myDAQ.addToReadList(&fadc5);

  for (unsigned int i=0; i<dbbs.size(); i++) {
    myDAQ.addToArmList(dbbs[i]);
    //myDAQ.addToReadList(dbbs[i]);    
  }

  if ( !myDAQ.Arm() )
    return false;

//  for (unsigned int i=0; i<dbbs.size(); i++)
//    dbbs[i]->Arm();

//  sleep(5);
  int spillCount=0;
  while (spillCount<20) {
    std::cout << "\n \n Generating Spill gate " << spillCount << " ...\n";

    bool done = false;     
    while (!done) {
      if ( ioReg.EventArrivedTriggerReceiver() ) {
        ioReg.ReadEventTriggerReceiver();
        if (ioReg.getEventType() == START_OF_BURST) {
          std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
//          trigger.softwareClear();
          ioReg.ReadEventTrailer();
        } else if (ioReg.getEventType() == PHYSICS_EVENT) {
          std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
          int nTriggers;
          if ( !myDAQ.compareNTriggers(nTriggers) )
            return false;

          int nbr = myDAQ.ReadEvent();
          for (unsigned int i=0; i<dbbs.size(); i++) {
            nbStored = dbbs[i]->ReadEvent();
            nbr += nbStored;
            if( !processEventDBB(data_dbb, nTriggers) )
              return false;
          }
          std::cout << "Event size: " << nbr << std::endl;

          ioReg.ReadEventTrailer();
        } else if (ioReg.getEventType() == END_OF_BURST) {
          std::cout << "Event Arrived. Event type is " << ioReg.getEventTypeAsString() << std::endl;
          ioReg.ReadEventTrailer();
          done =true;
        }
      }
    }
    spillCount++;
  }


  if ( !myDAQ.DisArm() )
    return false;

  delete data_io;
  delete data_dbb;
  dbbs.clear();

  return true;  
}

