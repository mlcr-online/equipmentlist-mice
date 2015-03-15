#ifndef _EPICSINTERFACE_H_
#define _EPICSINTERFACE_H_ 1

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

#define EC_DATA_WORD_SIZE   4
#define PV_TITLE_SHIFT     26
#define PV_TYPE_SHIFT      24
#define PV_TYPE_INT         0
#define PV_TYPE_DOUBLE      1
#define PV_TYPE_STRING      2
#define PV_TITLE_MASK    0x3F
#define PV_TYPE_MASK        3

typedef enum  EpicsClientPVTitle {
  EpicsClientPV_RunNumber          = 0x0,
  EpicsClientPV_TriggerCondition   = 0x1,
} EpicsClientPVTitle;

typedef struct {
  long32 * descFile;
} EpicsClient_ParType;

#ifdef EPICS_FOUND

  int SavePVInt(datePointer * data_ptr, std::string pvTitle, u32 pv);
  int SavePVDouble(datePointer * data_ptr, std::string pvTitle, double pv);
  int SavePVString(datePointer * data_ptr, std::string pvTitle, std::string pv);

#endif // EPICS_FOUND

void ArmEpicsClient( char * );
void DisArmEpicsClient( char * );
void AsynchReadEpicsClient( char * );
int  ReadEventEpicsClient( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedEpicsClient( char * );

void PauseArmEpicsClient( char *);
void PauseDisArmEpicsClient( char *);

#endif

