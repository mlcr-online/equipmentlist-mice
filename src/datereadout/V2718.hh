#ifndef _V2718_H_
#define _V2718_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

typedef struct {
  short *BLink;
  short *BNumber;
  long32 *MaxNumOfEvts;
} V2718_ParType;

int _Write16( unsigned long ad, short dt, int err );

void ArmV2718( char *);
void DisArmV2718( char *);
void AsynchReadV2718( char *);
int  ReadEventV2718( char *, struct eventHeaderStruct *,
                     struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedV2718( char *);

void PauseArmV2718( char *);
void PauseDisArmV2718( char *);

#endif


