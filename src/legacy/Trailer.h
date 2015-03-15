/***************************************************************************
 *                                                                         *

 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/
#ifndef _TRAILER_H_
#define _TRAILER_H_


#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "TriggerReceiver.h"

void ArmTrailer( char * );
void DisArmTrailer( char * );
void AsynchReadTrailer( char * );
int  ReadEventTrailer( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedTrailer( char * );

void PauseArmTrailer( char *);
void PauseDisArmTrailer( char *);
#endif
