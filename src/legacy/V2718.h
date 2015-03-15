/***************************************************************************
 *                                                                         *
 * $Log: V2718.h,v $
 * Revision 1.2  2007/06/30 18:47:48  daq
 * Added VME_WRITE_32 macro
 *
 * Revision 1.1  2007/06/08 17:17:34  daq
 * Initial revision
 *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#ifndef _V2718_H_
#define _V2718_H_

#include "DATEIncludes.h"
#include "equipmentList_common.h"
// #include "V2718Def.h"

/**************** Global Variables ***************************/
extern long V2718_Handle; // V2718 handle
extern int  vmeStatus;
extern unsigned long addr;

typedef struct {
    short *BLink;
    short *BNumber;
    short *EquipmentId;
} V2718_ParType;

int _Write16( unsigned long ad, short dt, int err );

void ArmV2718( char *);
void DisArmV2718( char *);
void AsynchReadV2718( char *);
int  ReadEventV2718( char *, struct eventHeaderStruct *,
                     struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedV2718( char *);

void PauseArmV2718( char *);
void PauseDisArmV2718( char *);

#endif


