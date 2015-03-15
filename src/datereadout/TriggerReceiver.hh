#ifndef _TRIGGER_RECEIVER_H_
#define _TRIGGER_RECEIVER_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "V2718.hh"
#include "V977Def.hh"

typedef struct {
  char  *BoardType;
  char  *BaseAddress;
  char  *TriggerCondition;
}TriggerReceiver_ParType;

extern TriggerReceiver_ParType *trParam;

void ArmTriggerReceiver( char * );
void DisArmTriggerReceiver( char * );
void AsynchReadTriggerReceiver( char * );
int  ReadEventTriggerReceiver( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedTriggerReceiver( char * );

void PauseArmTriggerReceiver( char *);
void PauseDisArmTriggerReceiver( char *);

#endif


