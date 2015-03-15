
#include "VLSBCosmicTrgManager.h"
#include <iostream>

using namespace std;

void ArmVLSBCosmicTrgManager( char *parPtr ){
  VLSBCosmicTrgManager_ParType* par = (VLSBCosmicTrgManager_ParType*)parPtr;

  sprintf(message, "Arming of VLSBCosmicTrgManager with the following parameters ");
    INFO(message); 
  sprintf(message, "Time Interval: %d (microseconds) ", *par->evInterval);
    INFO(message); 
  sprintf(message, "Output Register: base address: 0x%s , channel: %d ",
	  par->baseAddrOutputReg, *par->channelOutputReg);
    INFO(message); 
  sprintf(message, "VLSBMaster: base address: 0x%s ",
	  par->baseAddrVLSBMaster);
    INFO(message); 
  sprintf(message, "VLSBs: first address: 0x%s, number of boards: %d , address shift: 0x%s ",
	  par->baseAddrVLSBFirst, *par->nVLSB, par->shiftAddrVLSB);
    INFO(message); 

    // TODO test communication with all boards
}

void DisArmVLSBCosmicTrgManager( char *parPtr ){
  DO_INFO {
    sprintf(message, "Disarming VLSBCosmicTrgManager");
    INFO(message);
  }
}

void AsynchReadVLSBCosmicTrgManager( char *parPtr ){
  VLSBCosmicTrgManager_ParType* par = (VLSBCosmicTrgManager_ParType*)parPtr;
  unsigned long addr;
  unsigned long data=0;
  int ba = strtoul(par->baseAddrVLSBFirst, NULL, 16);
  int shift = strtoul(par->shiftAddrVLSB, NULL, 16);
  int nBoards = (*par->nVLSB);

  // Set the VLSB boards to accept data
  for (int ib=0; ib<nBoards; ib++) {
    addr= ba + ib*shift + VLSB_TRIGGER_CONTROL ;
    data = VLSB_Trigger_Data;
    vmeStatus = VME_WRITE_32(addr,data);
  }

  // Enable triggers in the VLSB Master
  ba = strtoul(par->baseAddrVLSBMaster, NULL, 16);
  addr = ba + VLSB_CONTROL;
  data = VLSB_CosmicTrigger;
  vmeStatus = VME_WRITE_32(addr,data);

  // The VLSBCosmicTrgManager does not generate trigger himself
  // It gnerate a signal on the output register that should be 
  // connected to the CALIBRATION Event input of the TriggerReceiver (ch2)
  // Wait for the time to come 
  // In the meanwhile, the VLSBs are acquiring data
  if ( *par->evInterval > 50000 ) {
    usleep ( *par->evInterval );
  } else {
    usleep ( 50000 );
  }
  // Set the output channel
  ba = strtoul(par->baseAddrOutputReg, NULL, 16);
  addr = ba + V977_OUTPUT_SET;
  vmeStatus = VME_READ_16( addr, &data );
  data |= (1<<(*par->channelOutputReg)); // channel between 0 and 15
  vmeStatus |= VME_WRITE_16( addr, data );
  // reset the output channel
  data &= ~(1<<(*par->channelOutputReg)); // channel between 0 and 15
  vmeStatus |= VME_WRITE_16( addr, data );
  // Let the trigger be handled by the trigger receiver 
  return;
}

int  EventArrivedVLSBCosmicTrgManager( char *parPtr ){
  return 0;
}

int  ReadEventVLSBCosmicTrgManager( char *parPtr, struct eventHeaderStruct *evhdr, 
			  struct equipmentHeaderStruct *eqhdr, datePointer *data_ptr){
  // The VLSBCosmicTrgManager does not generate data
  return 0;
}

void PauseArmVLSBCosmicTrgManager( char *) {}

void PauseDisArmVLSBCosmicTrgManager( char *) {}


