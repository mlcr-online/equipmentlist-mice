#ifndef _VLSB_H_
#define _VLSB_H_

#include "equipmentList_common.h"
#include "V2718.h"

#include "VLSBDef.h"

typedef struct {
  int *id;
  char *baseAddr;
} VLSB_ParType;

int zeroVLSBbankLoop(int ba);
int fastClear(int ba);

void ArmVLSB(char*);
void DisArmVLSB(char*);
void AsynchReadVLSB(char*);
int  ReadEventVLSB(char*, struct eventHeaderStruct*, struct equipmentHeaderStruct*, datePointer *);
int  EventArrivedVLSB(char*);

void PauseArmVLSB( char *);
void PauseDisArmVLSB( char *);

#endif
