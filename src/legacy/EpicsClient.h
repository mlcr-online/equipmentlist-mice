/***************************************************************************
*                                                                         *
* Originally created by J.S. Graulich in May 2010                         *
*                                                                         *
***************************************************************************/

#ifndef _EPICSCLIENT_H_
#define _EPICSCLIENT_H_


#include "DATEIncludes.h"
#include "equipmentList_common.h"

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

#include <iostream>
#include <string>

#include "DATEPVDescription.h"
#include "DATEPVDescriptionXmlReader.h"
#include "PVClient.hh"

int SavePVInt(datePointer * data_ptr, std::string pvTitle, u32 pv);
int SavePVDouble(datePointer * data_ptr, std::string pvTitle, double pv);
int SavePVString(datePointer * data_ptr, std::string pvTitle, std::string pv);

void ArmEpicsClient( char * );
void DisArmEpicsClient( char * );
void AsynchReadEpicsClient( char * );
int  ReadEventEpicsClient( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int  EventArrivedEpicsClient( char * );

void PauseArmEpicsClient( char *);
void PauseDisArmEpicsClient( char *);

#endif

