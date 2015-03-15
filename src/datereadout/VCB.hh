#ifndef _VCB_H_
#define _VCB_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "V2718.hh"

typedef struct {
  char   * BaseAddress;
  long32 * GEO;
} VCB_ParType;


void ArmVCB( char * );
void DisArmVCB( char * );
void AsynchReadVCB( char * );
int ReadEventVCB( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedVCB( char * );

void PauseArmVCB( char *);
void PauseDisArmVCB( char *);

#endif

