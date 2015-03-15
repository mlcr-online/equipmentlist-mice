#ifndef DATEINCLUDES
#define DATEINCLUDES 1

#ifdef __cplusplus
 extern "C" {
#endif

  #define DO_ERROR    if ( LOGLEVEL >= LOG_ERROR_TH )
  #define DO_INFO     if ( LOGLEVEL >= LOG_NORMAL_TH )
  #define DO_DETAILED if ( LOGLEVEL >= LOG_DETAILED_TH )

  /**************** DATE Includes ******************************/

  #include "event.h"
  #include "rcShm.h"
  //#include "readList_equipment.h"

  #define LOG_FACILITY "equipmentList_MICE"
  #include "infoLogger.h"

  #include "banksManager.h"
  #include "dateBufferManager.h"

#ifdef __cplusplus
}
#endif

/**************** Global Variables ***************************/
/* log message buffer */
// char message[512];
extern char message[];
/* Variable for error signalling (and stop the run) */
extern int readList_error;

int checkPartTriggerCount(struct eventHeaderStruct *header, unsigned long32 nPartEvt ); // Check Particle Trigger Count

#endif


