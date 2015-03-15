
#include "VLSBBank.h"
#include <iostream>

using namespace std;

void ArmVLSBBank(char *parPtr){
  VLSBBank_ParType* par = (VLSBBank_ParType*)parPtr;
  if (*par->bankNum >= VLSB_N_BANKS ){
    sprintf( message, "Error while Arming VLSBBank: Invalid Bank Number %d for VLSB %d",
             *par->bankNum,*par->VLSBid );
    ERROR( message );
    readList_error = PARAM_ERROR;
  }
}

void DisArmVLSBBank( char *parPtr ){
  // Nothing to do
}

void AsynchReadVLSBBank( char *parPtr ){}

int  EventArrivedVLSBBank( char *parPtr ){
  return 0; 
}

int  ReadEventVLSBBank( char *parPtr, struct eventHeaderStruct *evhdr, 
                        struct equipmentHeaderStruct *eqhdr, datePointer *data_ptr){

  VLSBBank_ParType* par = (VLSBBank_ParType*)parPtr;
  unsigned long addr;
  int ba = strtoul(par->VLSBaddr, NULL, 16);
  int nbyte=0;
  int nbRead=0;
  int dataLength;

  // Bank ID should be unique, start at 0
  int id = *par->bankNum + 4*(*par->VLSBid);  
  eqhdr->equipmentId = id;
  eqhdr->equipmentBasicElementSize = VLSB_WORD_SIZE;

  // Readout only in case of PHYSICS or CALIBRATION Events
  if ( evhdr->eventType == PHYSICS_EVENT || evhdr->eventType == CALIBRATION_EVENT ){
    // read the bank length 
    addr = ba + VLSB_EVENT_LENGTH_0 + (*par->bankNum)*VLSB_WORD_SIZE;
    vmeStatus = VME_READ_32(addr,&dataLength);

    // read the data in the bank
    addr = ba + VLSB_BANK_BASE_0 + (*par->bankNum)*VLSB_BANK_OFFSET;
    /* Version with Block transfer, assuming that the size of the bank matches the BLT capability
      vmeStatus = VME_READ_BLT_32(addr, d_ptr, VLSB_WORD_SIZE*dataLength, &nbRead );
      nbyte += nbRead;
      data_ptr += nbRead;
    */
    nbRead=0;
    for (int j=0; j<dataLength; j++){
      vmeStatus = VME_READ_32(addr+nbRead, data_ptr );
      data_ptr++;
      nbRead += 4;
    } 
    nbyte = nbRead;
  }

  return nbyte;
}

void PauseArmVLSBBank( char *) {}

void PauseDisArmVLSBBank( char *) {}
