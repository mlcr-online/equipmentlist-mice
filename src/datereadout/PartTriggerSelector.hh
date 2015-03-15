#ifndef _PARTTRIGGERSELECTOR_H_
#define _PARTTRIGGERSELECTOR_H_


#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "V977Def.hh"


typedef struct {
  char * BaseAddress;
  char * TriggerCondition;
} PartTriggerSelector_ParType;

void ArmPartTriggerSelector( char * );
void DisArmPartTriggerSelector( char * );
void AsynchReadPartTriggerSelector( char * );
int ReadEventPartTriggerSelector( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedPartTriggerSelector( char * );

void PauseArmPartTriggerSelector( char *);
void PauseDisArmPartTriggerSelector( char *);

#endif

