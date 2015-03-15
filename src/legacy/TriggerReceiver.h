/***************************************************************************
 *                                                                         *
 * $Log: TriggerReceiver.h,v $
 * Revision 1.2  2007/06/15 14:07:33  root
 * Introduce TriggerReceiverMask enum
 *
 * Revision 1.1  2007/06/08 17:17:34  daq
 * Initial revision
 *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/
#ifndef _TRIGGER_RECEIVER_H_
#define _TRIGGER_RECEIVER_H_

// #include <sdgstd.h> //for strlwr()

#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "V2718.h"
#include "V977Def.h"


extern struct timeval timetag0,timetag1;
extern int firstCall;
extern int V977_BaseAddress;

typedef enum TriggerReceiverBoardType {
  TR_V977    = 0,
  TR_V2718   = 1,
  TR_UNKNOWN = -1
} TriggerReceiverBoardType ;

extern TriggerReceiverBoardType triggerReceiverBoard;

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


