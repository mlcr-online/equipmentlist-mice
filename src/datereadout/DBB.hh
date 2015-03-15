#ifndef _DBB_H_
#define _DBB_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

#include "VRBDef.hh"
#include "MDEvRB.hh"
#include "MDEDBB.hh"

typedef struct {
  char   * VRBBaseAddress;
  long32 * GEO;
  long32 * IdInVRB;
} DBB_ParType;

void ArmDBB( char * );
void DisArmDBB( char * );
void AsynchReadDBB( char * );
int ReadEventDBB( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedDBB( char * );

void PauseArmDBB( char *);
void PauseDisArmDBB( char *);

#endif

