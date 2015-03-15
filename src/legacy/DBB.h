#ifndef _DBB_H_
#define _DBB_H_


#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "DBBDef.h"
#include "VRBDef.h"
#include "VRB.h"
#include "V2718.h"

typedef struct {
  char   * VRBBaseAddress;
  long32 * GEO;
} DBB_ParType;

void ArmDBB( char * );
void DisArmDBB( char * );
void AsynchReadDBB( char * );
int ReadEventDBB( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedDBB( char * );

void PauseArmDBB( char *);
void PauseDisArmDBB( char *);

#endif









