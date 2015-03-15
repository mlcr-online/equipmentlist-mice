
#include "VLSBMaster.h"
#include <iostream>

using namespace std;

int ConvertTime2Ticks(int t_usec) {
  return (int)rint(t_usec*VLSBMASTER_TIME_UNIT/VLSBMASTER_PERIOD);
}
int CodeSpillDuration(int sd) {
  return ConvertTime2Ticks(sd);
}

int CodeTriggerSettings(int triggerPeriod, int maxEvents) { 
  int tp = ConvertTime2Ticks(triggerPeriod);
  if (tp > 0xFFFF) { 
    sprintf( message, "In VLSBMaster::CodeTriggerSettings, trigger period exeeds its maximum value. Set it to 0xFFFF" );
    INFO( message );
    tp = 0xFFFF;
}
  return ( (tp&0xFFFF) + ((maxEvents<<16)&0xFFFF0000) );
}

int CodeVetoSettings(int vetoGateDelay, int vetoGatePeriod, int vetoWindow) { 
  if ( (vetoGateDelay > 0xFF) || (vetoGatePeriod > 0xFF) || (vetoWindow > 0xFF) ) { 
    sprintf( message, "In VLSBMaster::CodeVetoSettings, trigger period parameters exeed maximum values" );
    ERROR( message );
  }
  return ( (vetoGateDelay&0xFF) + ((vetoGatePeriod<<8)&0xFF00) + ((vetoWindow<<16)&0xFF0000) ); 
}

void ArmVLSBMaster( char *parPtr ){
  VLSBMaster_ParType* par = (VLSBMaster_ParType*)parPtr;
  unsigned long addr;
  unsigned long data=0;
  int baseAddr = strtoul(par->baseAddr, NULL, 16);
  char m[128];

  // reset control reg (disable triggers)
  data=0;
  addr = baseAddr + VLSB_CONTROL;
  vmeStatus = VME_WRITE_32(addr,data);

  // set the Spill and Trigger settings regs
  addr = baseAddr + VLSB_SPILL_DURATION;
  data = CodeSpillDuration(*par->spillDuration);
  vmeStatus += VME_WRITE_32(addr,data);

  addr = baseAddr + VLSB_TRIGGER_SETTINGS;
  // data = 0x0001FFFF; // no internal triggering in external mode
  data = CodeTriggerSettings(*par->triggerPeriod, *par->maxEvents);
  vmeStatus += VME_WRITE_32(addr,data);

  addr = baseAddr + VLSB_VETO_SETTINGS ;
  data = CodeVetoSettings( *par->vetoGateDelay, *par->vetoGatePeriod, *par->vetoWindow);
  vmeStatus += VME_WRITE_32(addr,data);

  usleep(50000);

  sprintf(m, "Arm of VLSBMaster - Spill Enabled");
    INFO(m); 
}

void DisArmVLSBMaster( char *parPtr ){
  VLSBMaster_ParType* par = (VLSBMaster_ParType*)parPtr;
  unsigned long addr;
  unsigned long data= 0x0;
  int baseAddr = strtoul(par->baseAddr, NULL, 16);

  // reset control reg (disable triggers)
  addr = baseAddr + VLSB_CONTROL;
  vmeStatus = VME_WRITE_32(addr,data);

}

void AsynchReadVLSBMaster( char *parPtr ){}

int  EventArrivedVLSBMaster( char *parPtr ){
  // The VLSBMaster does not generate triggers
  return 0;
}

int  ReadEventVLSBMaster( char *parPtr, struct eventHeaderStruct *evhdr, 
			  struct equipmentHeaderStruct *eqhdr, datePointer *data_ptr){
  VLSBMaster_ParType* par = (VLSBMaster_ParType*)parPtr;
  unsigned long addr;
  unsigned long data=0;
  int baseAddr = strtoul(par->baseAddr, NULL, 16);

  // There is only one VLSBMaster per crate and its ID should be 0
  eqhdr->equipmentId = 0;

  if ( evhdr->eventType == PHYSICS_EVENT || evhdr->eventType == CALIBRATION_EVENT ){
    /* Put into readout mode by disabling triggers */
    data = 0x0; // disable all of trigger register
    addr = baseAddr + VLSB_CONTROL;
    vmeStatus = VME_WRITE_32(addr,data);
    // the triggers are re-enabled either by the next Start of Spill event or by the VLSBCosmicTrgManager

    // TODO
    // We should read the number of particle triggers received here and put it in the 
    // equipment header attribute and in the event header attribute so that we
    // can check it is the same as for other equipments 

  } else if ( evhdr->eventType == START_OF_BURST ) { 
    // This corresponds to the MICE Start Of Spill signal, received about 20 ms before the DAQ Spill Gate
    /* Put into data accept mode mode by enabling triggers */
    addr = baseAddr + VLSB_CONTROL;
    data = VLSB_CosmicTrigger;
    vmeStatus = VME_WRITE_32(addr,data);
  }

  return 0; // No data is written to the data stream

}

void PauseArmVLSBMaster( char *) {}

void PauseDisArmVLSBMaster( char *) {}

