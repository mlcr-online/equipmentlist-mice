#ifndef _V830_H_
#define _V830_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"
#include "V2718.hh"

#include "V830Def.hh"

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

bool CallEPICS(V830_ParType *, MDE_Pointer *data_ptr);
bool IsGoodEvent(V830_ParType * V830,
                 struct equipmentHeaderStruct *eq_header_ptr,
                 MDE_Pointer *data_ptr);

void ArmV830( char * );
void DisArmV830( char * );
void AsynchReadV830( char * );
int ReadEventV830( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedV830( char * );

void PauseArmV830( char *);
void PauseDisArmV830( char *);

#endif


