/***************************************************************************
 *                                                                         *
 * $Log: V1731.h,v $
 *                                                                         *
 * equipmentList functions for the CAEN fADC V1731                         *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov July 2007          *
 *                                                                         *
 ***************************************************************************/ 
#ifndef _V1731_H_
#define _V1731_H_

#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "V2718.h"

#include "V1731Def.h"

typedef struct {
  char   * BaseAddress;
  long32 * GEO;
  char   * ChannelConfig;
  long32 * PostTriggerOffset;
  long32   * BufferSize; // not used
} V1731_ParType;


void ArmV1731( char * );
void DisArmV1731( char * );
void AsynchReadV1731( char * );
int ReadEventV1731( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1731( char * );

void PauseArmV1731( char *);
void PauseDisArmV1731( char *);

#endif


