#ifndef _V1495_H_
#define _V1495_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

typedef struct {
  char   *BaseAddress;
  long32 *GEO;
  long32 *SGOpenDelay;
  long32 *SGCloseDelay;
  char   *SggCtrl;
  char   *TriggerLogicCtrl;
  char   *TOF0Mask;
  char   *TOF1Mask;
  char   *TOF2Mask;
  long32 *PartTrVetoLenght;
} V1495_ParType;


void ArmV1495( char * );
void DisArmV1495( char * );
void AsynchReadV1495( char * );
int ReadEventV1495( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV1495( char * );

void PauseArmV1495( char *);
void PauseDisArmV1495( char *);

#endif

