#ifndef _VLSBCOSMICTRGMANAGER_H_
#define _VLSBCOSMICTRGMANAGER_H_

#include "equipmentList_common.h"
#include "V2718.h"
#include "V977Def.h"
#include "VLSBDef.h"
#include <math.h>

typedef struct {
  int  *evInterval;
  char *baseAddrOutputReg;
  int  *channelOutputReg;
  char *baseAddrVLSBMaster;
  char *baseAddrVLSBFirst;
  int  *nVLSB;
  char *shiftAddrVLSB;
} VLSBCosmicTrgManager_ParType;

void ArmVLSBCosmicTrgManager( char * );
void DisArmVLSBCosmicTrgManager( char * );
void AsynchReadVLSBCosmicTrgManager( char * );
int  ReadEventVLSBCosmicTrgManager( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedVLSBCosmicTrgManager( char * );

void PauseArmVLSBCosmicTrgManager( char *);
void PauseDisArmVLSBCosmicTrgManager( char *);

#endif
