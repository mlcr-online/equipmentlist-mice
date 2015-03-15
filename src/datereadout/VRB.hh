#ifndef _VRB_H_
#define _VRB_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "V2718.hh"

#include "VRBDef.hh"

typedef struct {
  char   * BaseAddress;
  long32 * GEO;
  long32 * nDBB;
  long32 * DBBfirstID;
  char   * FWversion;
} VRB_ParType;


void ArmVRB( char * );
void DisArmVRB( char * );
void AsynchReadVRB( char * );
int ReadEventVRB( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedVRB( char * );

void PauseArmVRB( char *);
void PauseDisArmVRB( char *);

#endif

