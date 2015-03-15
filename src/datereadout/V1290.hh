#ifndef _V1290_H_
#define _V1290_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "V1290Def.hh"

typedef struct {
    long32 * WinWidth;
    long32 * WinOffset;
    long32 * SwMargin;
    long32 * RejMargin;
    char   * BaseAddress;
  //    short  * EqId;
    short *  DetectionEdge;
    short *  TriggerMode;
    char *   ModuleType;
    char *   ChannelConfig;
    char *   SubstTrigger;
    char *   LSBCode;
    char *   DeadtimeCode;
    char *   TDCHeader;
    short *  EventSizeCode;
    long32 * GEO;
    char *   Init;
} V1290_ParType;

int OPCodeWrite( unsigned long ba, short OPCode, unsigned int nw, short* microData);
int OPCodeRead(  unsigned long ba, short OPCode, unsigned int nw, short* microData);

void ArmV1290( char * );
void DisArmV1290( char * );
void AsynchReadV1290( char * );
int ReadEventV1290( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1290( char * );

void PauseArmV1290( char *);
void PauseDisArmV1290( char *);

void ArmV1290N( char * );
void DisArmV1290N( char * );
void AsynchReadV1290N( char * );
int ReadEventV1290N( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1290N( char * );

void PauseArmV1290N( char *);
void PauseDisArmV1290N( char *);

#endif



