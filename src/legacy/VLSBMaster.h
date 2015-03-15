#ifndef _VLSBMASTER_H_
#define _VLSBMASTER_H_

#include "equipmentList_common.h"
#include "V2718.h"

#include "VLSBDef.h"
#include <math.h>

int ConvertTime2Ticks(int t_usec);
int CodeSpillDuration(int sd);
int CodeTriggerSettings(int tp, int me);
int CodeVetoSettings(int vgd, int vgp, int vw);

typedef struct {
  char *baseAddr;
  int  *spillDuration;
  int  *triggerPeriod;
  int  *maxEvents;
  int  *vetoGateDelay;
  int  *vetoGatePeriod;
  int  *vetoWindow;
} VLSBMaster_ParType;

void ArmVLSBMaster( char * );
void DisArmVLSBMaster( char * );
void AsynchReadVLSBMaster( char * );
int  ReadEventVLSBMaster( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedVLSBMaster( char * );

void PauseArmVLSBMaster( char *);
void PauseDisArmVLSBMaster( char *);
#endif
