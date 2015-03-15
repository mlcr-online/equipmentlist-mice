/***************************************************************************
 *                                                                         *
 * $Log: V830.h,v $
 * Revision 1.1  2007/07/20 17:00:04  daq
 * Initial revision
 *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#ifndef _V830_H_
#define _V830_H_

#include "DATEIncludes.h"
#include "equipmentList_common.h"
#include "V2718.h"

#include "V830Def.h"

typedef struct {
    long32 * GEO;
    char   * BaseAddress;
    char   * ChannelConfig;   //default 'all'
    short  * TriggerMode;     //default 1
    //short  * SpillGateChannel;
    char   * UseHeader;       //default 1
    char   * UseNarrow;       //use 26 bit counter so default is 1
    char   * AutoReset;       //default 1 
} V830_ParType;

extern int nChannelsSet; //Number of enabled channels

bool CallEPICS(V830_ParType *, datePointer *data_ptr);
bool IsGoodEvent(V830_ParType * V830,
                 struct equipmentHeaderStruct *eq_header_ptr,
                 datePointer *data_ptr);

void ArmV830( char * );
void DisArmV830( char * );
void AsynchReadV830( char * );
int ReadEventV830( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV830( char * );

void PauseArmV830( char *);
void PauseDisArmV830( char *);

#endif


