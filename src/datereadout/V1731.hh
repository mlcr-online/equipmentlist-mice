
#ifndef _V1731_H_
#define _V1731_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

typedef struct {
  char    *BaseAddress;
  long32  *GEO;
  char    *ChannelConfig;
  long32  *PostTriggerOffset;
  long32  *BufferSize;
  long32  *ZSThreshold;
} V1731_ParType;


void ArmV1731( char * );
void DisArmV1731( char * );
void AsynchReadV1731( char * );
int ReadEventV1731( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1731( char * );

void PauseArmV1731( char *);
void PauseDisArmV1731( char *);

#endif


