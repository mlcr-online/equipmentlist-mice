#include "DATETestExample.hh"

#define min(x,y) ((x)>(y)?(y):(x))
#define max(x,y) ((x)>(y)?(x):(y))

/*************************** Timer **************************/

void ArmTimer( char *parPtr) {

  TimerParType *timerPar = (TimerParType *)parPtr;

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_INFO,
                    "Arming Timer: interval = %d", 
                    *timerPar->evIntervalPtr);
  }
}

void PauseArmTimer( char *parPtr) {
}

void DisArmTimer( char *parPtr) {

 DO_INFO {infoLog_f(LOG_FACILITY,LOG_INFO,
          "Disarming Timer");
 }
}

void PauseDisArmTimer( char *parPtr) {
}

void AsynchReadTimer( char *parPtr) {
}

int ReadEventTimer( char *parPtr, struct eventHeaderStruct *header_ptr,
		    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {

  /* Simulate the experiment event number 
     by taking a counter incremented by readout */
  if (DAQCONTROL->colliderModeFlag == 1) {
    ZERO_EVENT_ID(header_ptr->eventId);
    EVENT_ID_SET_ORBIT (header_ptr->eventId, DAQCONTROL->eventCount);
    SET_SYSTEM_ATTRIBUTE( header_ptr->eventTypeAttribute, ATTR_ORBIT_BC );
  } else {
    EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId, DAQCONTROL->eventCount);
  }
  return 1;
}

int EventArrivedTimer( char *parPtr) {
  /* Returns a value > 0 when a trigger has occured */
  /* wait for a trigger in useconds */

  TimerParType *timerPar = (TimerParType *)parPtr;

  if (*timerPar->evIntervalPtr > 0)
    usleep (*timerPar->evIntervalPtr);

  return 1;
}

/************************** Rand ***************************/

void ArmRand( char *parPtr) {

  RandParType *randPar = (RandParType *)parPtr;

  /* Check pagedDataFlag: for Rand it must be not set */
  if (DAQCONTROL->pagedDataFlag == 1) {
    readList_error = 501;
    DO_ERROR {infoLog_f(LOG_FACILITY,LOG_ERROR,
			"Arming Rand: Error %d, pagedData flag must be off",
			readList_error);
    }
    return;
  }

  if ((*randPar->triggerPattern < EVENT_TRIGGER_ID_MIN) || 
      (*randPar->triggerPattern > EVENT_TRIGGER_ID_MAX) ) {
    readList_error = 502;
    DO_ERROR {infoLog_f(LOG_FACILITY,LOG_ERROR,
			"Arming Rand: Error %d, triggerPatter %d is out of range",
			readList_error, 
			*randPar->triggerPattern);
    }
    return;
  }

  if (*randPar->eventMinSizePtr < 0) *randPar->eventMinSizePtr = 0;
  if (*randPar->eventMaxSizePtr < *randPar->eventMinSizePtr) {
    *randPar->eventMaxSizePtr = *randPar->eventMinSizePtr;
  }
  if (*randPar->eventMaxSizePtr + sizeof( struct eventHeaderStruct) >
      DAQCONTROL->maxEventSize) {
      *randPar->eventMaxSizePtr = DAQCONTROL->maxEventSize -
	  sizeof( struct eventHeaderStruct);
  }

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_INFO,
		     "Arming Rand: id = %d, min = %d, max = %d, trigger pattern = %d",
		     *randPar->eqIdPtr,
		     *randPar->eventMinSizePtr,
		     *randPar->eventMaxSizePtr,
		     *randPar->triggerPattern);
  }
}

void PauseArmRand( char *parPtr) {
}

void DisArmRand( char *parPtr) {

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_INFO,
		     "Disarming Rand");
  }
}

void PauseDisArmRand( char *parPtr) {
}

void AsynchReadRand( char *parPtr) {
}

int ReadEventRand( char *parPtr, struct eventHeaderStruct *header_ptr,
		   struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  /* Called after a trigger has arrived */
  /* Inserts header fields data into the area  pointed by header_ptr */
  /* Inserts raw data into the area  pointed by data_ptr */
  /* Returns the number of bytes of raw data actually readout.*/

  RandParType *randPar = (RandParType *)parPtr;
  datePointer* firstWord = data_ptr;
  int dataSize;
  char m0[512];
  char m1[512];
  char m2[512];
  int i;

  /* SOB and EOB treatment: no data */
  if ( (header_ptr->eventType != PHYSICS_EVENT) &&
       (header_ptr->eventType != CALIBRATION_EVENT) ) {
      eq_header_ptr->equipmentId = *randPar->eqIdPtr;
      return ((datePointer)data_ptr - (datePointer)firstWord );
  }
  /* Physics or calibration data */
  
  /* set event trigger pattern to a constant value */
  SET_TRIGGER_IN_PATTERN (header_ptr->eventTriggerPattern,
			  *randPar->triggerPattern);
  VALIDATE_TRIGGER_PATTERN(header_ptr->eventTriggerPattern);

  /* fill the equipment header and the raw data */  
  eq_header_ptr->equipmentId = *randPar->eqIdPtr;
  eq_header_ptr->equipmentBasicElementSize = 4;

  dataSize = (*randPar->eventMinSizePtr + 
	      (((int)rand() / RAND_MAX) *
	       (*randPar->eventMaxSizePtr - *randPar->eventMinSizePtr)))/sizeof(long);
  
  for (i = 0; i < dataSize; i++) {
    *data_ptr++ = (long)i;
  }

  DO_DETAILED {
    /* dump the first and the last 10 words of raw data */
    sprintf (m1," ");
    for (i = 0; i < min (10, dataSize); i++) {
      sprintf (m0, "0x%lx - ", *(firstWord + i));
      strcat (m1, m0);
    }
    sprintf (m2," ");
    if (dataSize > 10) {
	for (i= max (dataSize - 10, 10); i < dataSize; i++) {
	    sprintf (m0, "0x%lx - ", *(firstWord + i));
	    strcat (m2, m0);
	}
    }
    infoLog_f(LOG_FACILITY,LOG_ERROR,
	      "Rand: "
	      "type = %d, id = %hd, basic element size = %d, "
	      "data size in longwords = %d, returned data size in bytes = %ld, "
	      "first 10 data words: %s, "
	      "last 10 data words: %s",
	      eq_header_ptr->equipmentType,
	      eq_header_ptr->equipmentId,
	      eq_header_ptr->equipmentBasicElementSize,
	      dataSize, (datePointer)data_ptr - (datePointer)firstWord,
	      m1,
	      m2);
  }

  return ((datePointer)data_ptr - (datePointer)firstWord );
}

int EventArrivedRand( char *parPtr) {
  return 0;
}

/*************************** ErrGen **************************/

/* Equipment used to test the error propagation */

long32 errorCase;

void ArmErrGen( char *parPtr) {

  ErrGenParType *ErrGenPar = (ErrGenParType *)parPtr;
  int *wrongPointer = NULL;
  
  errorCase = *ErrGenPar->errorCasePtr;

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_ERROR,
		     "Arming ErrGen: error = %d", errorCase);
  }

  if (errorCase == ARM_ERROR) {
    readList_error = 101;
  }
  if (errorCase == ARM_CRASH) {
    *wrongPointer = 0;
  }
}

void PauseArmErrGen( char *parPtr) {
}

void DisArmErrGen( char *parPtr) {

  ErrGenParType *ErrGenPar = (ErrGenParType *)parPtr;

  errorCase = *ErrGenPar->errorCasePtr;

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_ERROR,
		     "Disarming ErrGen");
  }

  if (errorCase == DISARM_ERROR) {
    readList_error = 102;
  }
}

void PauseDisArmErrGen( char *parPtr) {
}

void AsynchReadErrGen( char *parPtr) {
}

int ReadEventErrGen( char *parPtr, struct eventHeaderStruct *header_ptr,
		     struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {

  ErrGenParType *ErrGenPar = (ErrGenParType *)parPtr;
  int *wrongPointer = NULL;
    
  errorCase = *ErrGenPar->errorCasePtr;
  
  if (errorCase == READEVENT_ERROR) {
    readList_error = 103;
  }
  if (DAQCONTROL->eventCount >= 10) {
    
    switch (errorCase) {
    case READEVENT_CRASH:
      *wrongPointer = 0;
      break;
    case NB_IN_RUN_INCR:
      EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId,
			      EVENT_ID_GET_NB_IN_RUN(header_ptr->eventId) + 1 );
      break;
    case NB_IN_RUN_DECR: 
      EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId,
			      EVENT_ID_GET_NB_IN_RUN(header_ptr->eventId) - 1 );
      break;
    case NB_IN_BURST_INCR: 
      EVENT_ID_SET_NB_IN_BURST (header_ptr->eventId,
			        EVENT_ID_GET_NB_IN_BURST(header_ptr->eventId) + 1 );
      break;
    case NB_IN_BURST_DECR: 
      EVENT_ID_SET_NB_IN_BURST (header_ptr->eventId,
			        EVENT_ID_GET_NB_IN_BURST(header_ptr->eventId) - 1 );
      break;
    case BURST_NB_INCR:
      EVENT_ID_SET_BURST_NB (header_ptr->eventId,
			     EVENT_ID_GET_BURST_NB(header_ptr->eventId) + 1 );
      break;
    case BURST_NB_DECR:
      EVENT_ID_SET_BURST_NB (header_ptr->eventId,
			     EVENT_ID_GET_BURST_NB(header_ptr->eventId) - 1 );
      break;
    }
  }
  return 0;
}

int EventArrivedErrGen( char *parPtr) {
  return 0;
}

/*************************** CalBurstGen **************************/

/* Equipment used to generate bursts and calibration events */

long32 eventCounter;
long32 burstCounter;
long32 calibCounter;
long32 calibFlag;
long32 outBurstFlag;
long32 calNbInBurst;
long32 evNbInBurst;

void ArmCalBurstGen( char *parPtr) {

  eventCounter = 0;
  calibCounter = 0;
  burstCounter = 0;
  calibFlag = 0;
  outBurstFlag = 0;

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_ERROR,
		     "Arming CalBurstGen: ");
  }
}

void PauseArmCalBurstGen( char *parPtr) {
}

void DisArmCalBurstGen( char *parPtr) {

  DO_INFO {infoLog_f(LOG_FACILITY,LOG_ERROR,
		     "Disarming DisArmCalBurstGen");
  }
}

void PauseDisArmCalBurstGen( char *parPtr) {
}

void AsynchReadCalBurstGen( char *parPtr) {
}

int ReadEventCalBurstGen( char *parPtr, struct eventHeaderStruct *header_ptr,
			  struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {

    CalBurstGenParType *CalBurstGenPar = (CalBurstGenParType *)parPtr;

    if ( *CalBurstGenPar->burstModulusPtr != 0 && (outBurstFlag != 2) &&
	 (outBurstFlag == 1 || eventCounter == 0) ) {
	/* Start of burst record */
	if ( *CalBurstGenPar->burstIntervalPtr > 0 )
	    usleep (*CalBurstGenPar->burstIntervalPtr);
	outBurstFlag = 2;
	calNbInBurst = 0;
	evNbInBurst = 0;
	header_ptr->eventType = START_OF_BURST;
        burstCounter++;
	EVENT_ID_SET_BURST_NB (header_ptr->eventId, burstCounter);
	EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId, 0);
    } else if ( *CalBurstGenPar->burstModulusPtr != 0 &&
		(outBurstFlag == 0) &&
		(eventCounter % *CalBurstGenPar->burstModulusPtr == 0 ) ) {
	/* End of burst record */
	outBurstFlag = 1;
	header_ptr->eventType = END_OF_BURST;
	EVENT_ID_SET_BURST_NB (header_ptr->eventId, burstCounter);
	EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId, 0);
    } else if ( *CalBurstGenPar->calibModulusPtr != 0 &&
		!calibFlag && eventCounter != 0 &&
		(eventCounter % *CalBurstGenPar->calibModulusPtr == 0 ) ) {
	/* Calibration record */
	calibFlag = 1;
	header_ptr->eventType = CALIBRATION_EVENT;
	calibCounter++;
	EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId, (unsigned int)calibCounter);
	if (DAQCONTROL->burstPresentFlag == TRUE) {
	    calNbInBurst++;
	    EVENT_ID_SET_NB_IN_BURST (header_ptr->eventId, calNbInBurst);
	    EVENT_ID_SET_BURST_NB (header_ptr->eventId, burstCounter);
	}
    } else {
	/* Normal event */
	outBurstFlag = 0;
	calibFlag = 0;
	eventCounter++;
	EVENT_ID_SET_NB_IN_RUN (header_ptr->eventId, (unsigned int)eventCounter);
	if (DAQCONTROL->burstPresentFlag == TRUE) {
	    evNbInBurst++;
	    EVENT_ID_SET_NB_IN_BURST (header_ptr->eventId, evNbInBurst);
	    EVENT_ID_SET_BURST_NB (header_ptr->eventId, burstCounter);
	}
 
        /* Set event trigger pattern for edm testing */
	if (eventCounter%5 == 0) { /* true every fifth time */
	  SET_TRIGGER_IN_PATTERN (header_ptr->eventTriggerPattern, 1);
	} else {
	  SET_TRIGGER_IN_PATTERN (header_ptr->eventTriggerPattern, 2);
	}
    }
    return 0;
}

int EventArrivedCalBurstGen( char *parPtr) {
  return 0;
}

