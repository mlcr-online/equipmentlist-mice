
#include "VLSB.h"
#include <iostream>

using namespace std;

int zeroVLSBbankLoop(int ba){
  unsigned long addr;
  unsigned long data=0;
  int memAdd = 0;
  addr =  ba + VLSB_BANK_BASE_0;

  for (int bank=0; bank < VLSB_N_BANKS ; bank++){
    memAdd=0;
    for (int j=0; j<VLSB_BANK_LENGTH; j++){
      vmeStatus = VME_WRITE_32(addr + bank*VLSB_BANK_OFFSET + memAdd, data);
      memAdd+=4;
    }
  }
  return vmeStatus;
}

int fastClear(int ba){
  unsigned long addr = ba + VLSB_CONTROL;
  unsigned long data;
  unsigned long orig;

  vmeStatus = VME_READ_32(addr, &orig );
  data = VLSB_FastClear;
  vmeStatus += VME_WRITE_32(addr, data );
  usleep(50);
  vmeStatus += VME_WRITE_32(addr, orig );
  return vmeStatus;
}

void ArmVLSB(char *parPtr){
  VLSB_ParType* par = (VLSB_ParType*)parPtr;
  unsigned long addr;
  unsigned long data=0;
  int baseAddr = strtoul(par->baseAddr, NULL, 16);

  // Set No zero suppression
  addr = baseAddr + VLSB_CONTROL;
  data = VLSB_NoZeroSuppress;
  vmeStatus = VME_WRITE_32(addr,data);

  // Set data accept mode
  addr = baseAddr + VLSB_TRIGGER_CONTROL;
  data = VLSB_Trigger_Data;
  vmeStatus += VME_WRITE_32(addr,data);

}

void DisArmVLSB( char *parPtr ){
  VLSB_ParType* par = (VLSB_ParType*)parPtr;
  unsigned long addr;
  unsigned long data=0;
  int baseAddr = strtoul(par->baseAddr, NULL, 16);

  // Disable triggers
  addr = baseAddr + VLSB_TRIGGER_CONTROL;
  data = VLSB_Trigger_Disable;
  vmeStatus += VME_WRITE_32(addr,data);

}

void AsynchReadVLSB( char *parPtr ){}

int  EventArrivedVLSB( char *parPtr ){
  return 0; 
}

int  ReadEventVLSB( char *parPtr, struct eventHeaderStruct *evhdr, 
                    struct equipmentHeaderStruct *eqhdr, datePointer *data_ptr){

  VLSB_ParType* par = (VLSB_ParType*)parPtr; 
  int baseAddr = strtoul(par->baseAddr, NULL, 16);
  unsigned long addr;
  unsigned long data;

  int id = *par->id;  
  eqhdr->equipmentId = id;

  if ( evhdr->eventType == PHYSICS_EVENT || evhdr->eventType == CALIBRATION_EVENT ){
    /* Put into readout mode by disabling triggers */
    addr = baseAddr + VLSB_TRIGGER_CONTROL;
    data = VLSB_Trigger_Disable;
    vmeStatus += VME_WRITE_32(addr,data);
    // That's it, the data is read by the VLSBBank equipments

  } else if ( evhdr->eventType == START_OF_BURST ) { 
    // This corresponds to the MICE Start Of Spill signal, received about 20 ms before the DAQ Spill Gate
    /* Put into data accept mode mode by enabling triggers */
    addr = baseAddr + VLSB_TRIGGER_CONTROL;
    data = VLSB_Trigger_Data;
    vmeStatus += VME_WRITE_32(addr,data);
    // That's it, no data to read
  }

  return 0;
}

void PauseArmVLSB( char *) {}

void PauseDisArmVLSB( char *) {}

