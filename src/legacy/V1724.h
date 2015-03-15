/***************************************************************************
 *                                                                         *
 * $Log: V1724.h,v $
 *                                                                         *
 * equipmentList functions for the CAEN fADC V1724                         *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov July 2007          *
 *                                                                         *
 ***************************************************************************/ 
#ifndef _V1724_H_
#define _V1724_H_

#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "V2718.h"

#include "V1724Def.h"

typedef struct {
  char   * BaseAddress;
  long32 * GEO;
  char   * ChannelConfig;
  long32 * PostTriggerOffset;
  long32   * BufferSize; // not used
} V1724_ParType;


void ArmV1724( char * );
void DisArmV1724( char * );
void AsynchReadV1724( char * );
int ReadEventV1724( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1724( char * );

void PauseArmV1724( char *);
void PauseDisArmV1724( char *);

#endif

