#ifndef _VLSBBANK_H_
#define _VLSBBANK_H_

#include "equipmentList_common.h"
#include "V2718.h"

#include "VLSBDef.h"

typedef struct {
  int  *bankNum;
  int  *VLSBid;
  char *VLSBaddr;
} VLSBBank_ParType;

void ArmVLSBBank(char*);
void DisArmVLSBBank(char*);
void AsynchReadVLSBBank(char*);
int  ReadEventVLSBBank(char*, struct eventHeaderStruct*, struct equipmentHeaderStruct*, datePointer*);
int  EventArrivedVLSBBank(char*);

void PauseArmVLSBBank( char *);
void PauseDisArmVLSBBank( char *);

#endif
