#ifndef _TRAILER_H_
#define _TRAILER_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "TriggerReceiver.hh"

typedef struct {
  long32 *myParam;
} TrailerParType;

void ArmTrailer( char * );
void DisArmTrailer( char * );
void AsynchReadTrailer( char * );
int  ReadEventTrailer( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedTrailer( char * );

void PauseArmTrailer( char *);
void PauseDisArmTrailer( char *);
#endif
