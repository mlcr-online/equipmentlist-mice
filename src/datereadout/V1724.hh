#ifndef _V1724_H_
#define _V1724_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

typedef struct {
  char   *BaseAddress;
  long32 *GEO;
  char   *ChannelConfig;
  long32 *PostTriggerOffset;
  long32 *BufferSize;
  long32 *ZSThreshold;
} V1724_ParType;


void ArmV1724( char * );
void DisArmV1724( char * );
void AsynchReadV1724( char * );
int ReadEventV1724( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1724( char * );

void PauseArmV1724( char *);
void PauseDisArmV1724( char *);

#endif

