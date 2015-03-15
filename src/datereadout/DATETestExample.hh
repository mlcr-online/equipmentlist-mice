#ifndef _DATETESTEXAMPLE_H_
#define _DATETESTEXAMPLE_H_ 1

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

/********************* Function prototype definition **********/
typedef struct {
  long32 *evIntervalPtr;
} TimerParType;

void ArmTimer( char *);
void PauseArmTimer( char *);
void DisArmTimer( char *);
void PauseDisArmTimer( char *);
void AsynchReadTimer( char *);
int  ReadEventTimer( char *, struct eventHeaderStruct *,
                             struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedTimer( char *);


typedef struct {
  long32 *eventMinSizePtr;
  long32 *eventMaxSizePtr;
  short *eqIdPtr;
  short *triggerPattern;
} RandParType;

void ArmRand( char *);
void PauseArmRand( char *);
void DisArmRand( char *);
void PauseDisArmRand( char *);
void AsynchReadRand( char *);
int  ReadEventRand( char *, struct eventHeaderStruct *,
                    struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedRand( char *);


typedef struct {
  long32 * errorCasePtr;
} ErrGenParType;

void ArmErrGen( char *);
void PauseArmErrGen( char *);
void DisArmErrGen( char *);
void PauseDisArmErrGen( char *);
void AsynchReadErrGen( char *);
int  ReadEventErrGen( char *, struct eventHeaderStruct *,
                      struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedErrGen( char *);


typedef struct {
  long32 *burstModulusPtr;
  long32 *calibModulusPtr;
  long32 *burstIntervalPtr;
} CalBurstGenParType;

void ArmCalBurstGen( char *);
void PauseArmCalBurstGen( char *);
void DisArmCalBurstGen( char *);
void PauseDisArmCalBurstGen( char *);
void AsynchReadCalBurstGen( char *);
int  ReadEventCalBurstGen( char *, struct eventHeaderStruct *,
                           struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedCalBurstGen( char *);

#endif


