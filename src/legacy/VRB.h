#ifndef _VRB_H_
#define _VRB_H_

#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "VRBDef.h"
#include "DBBDef.h"
#include "V2718.h"

typedef struct {
  char   * BaseAddress;
  long32 * GEO;
  long32 * nDBB;
  long32 * DBBfirstID;
  char   * FWversion;
} VRB_ParType;

int SetMemInt( unsigned long ba );
int SetMemExt( unsigned long ba );
int ClearMemory( unsigned long ba, int nloc );
int SetClock( unsigned long ba, int clock);
int GlobalReset( unsigned long ba );
int Start( unsigned long ba );

unsigned int MakeBoardId(unsigned int boardId);
int ExecuteCommand(unsigned long ba, unsigned int boardId, unsigned int command);

void ArmVRB( char * );
void DisArmVRB( char * );
void AsynchReadVRB( char * );
int ReadEventVRB( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedVRB( char * );

void PauseArmVRB( char *);
void PauseDisArmVRB( char *);

#endif









