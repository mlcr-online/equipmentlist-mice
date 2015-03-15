#ifndef _PARTTRIGGERSELECTOR_H_
#define _PARTTRIGGERSELECTOR_H_


#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "V2718.h"
#include "V977Def.h"


typedef struct {
  char * BaseAddress;
  char * TriggerCondition;
} PartTriggerSelector_ParType;


typedef enum  PartTriggerSelectorChannelBit {
  PTS_GVA1 = 0x100,
  PTS_TOF0 = 0x200,
  PTS_TOF1 = 0x400
} PartTriggerSelectorChannelBit ;


void ArmPartTriggerSelector( char * );
void DisArmPartTriggerSelector( char * );
void AsynchReadPartTriggerSelector( char * );
int ReadEventPartTriggerSelector( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedPartTriggerSelector( char * );

void PauseArmPartTriggerSelector( char *);
void PauseDisArmPartTriggerSelector( char *);

#endif

