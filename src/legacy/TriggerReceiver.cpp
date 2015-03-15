/***************************************************************************
 *                                                                         *
 * $Log: TriggerReceiver.c,v $
 * Revision 1.3  2007/07/20 16:53:24  daq
 * Minor changes.
 *
 * Revision 1.2  2007/06/15 14:05:27  root
 * Implement CM18 Trigger connection scheme
 *
 * Revision 1.1  2007/06/08 17:17:56  daq
 * Initial revision
 *
 *                                                                         *
 * equipmentList functions for the TriggerReceiver                         *
 * TriggerReceiver is the only equpiment generating a DAQ Trigger, that is *
 * the only one for which EventArrive() is not dummy.                      *
 * It generates data giving additional information on the trigger          *
 * condition and it sets the EventType in the event header.                *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/
#include "TriggerReceiver.h"

using namespace std;

TriggerReceiverBoardType triggerReceiverBoard;

struct timeval timetag0,timetag1;
int firstCall;
int V977_BaseAddress;

TriggerReceiver_ParType *trParam;

void ArmTriggerReceiver( char * parPtr ) {
  short data=0;
  // Get Trigger Receiver parameters
  trParam = (TriggerReceiver_ParType*) parPtr;

  //lDescriptionReader.ReadFile(lDescriptionFile, lServerDescription);
  //lDATEStatusClient = new DATEStatusClient(lServerDescription, lReadWriteTimeout, lMonitorTimeout);

  V977_BaseAddress = getBA( trParam->BaseAddress );
  sprintf( message,
           "TriggerReceiver Base Address: (H%X D%d S%s) (%s %s)",
           V977_BaseAddress, V977_BaseAddress, trParam->BaseAddress,
           trParam->BoardType, trParam->TriggerCondition );

  INFO( message );
  if ( strcmp(trParam->BoardType,"V977" ) == 0) {
    triggerReceiverBoard = TR_V977;   
    //Initialize V977
    //1.) SOFTWARE RESET
    sprintf( message, "Going to reset the board..." );
    INFO( message );
    addr =  V977_BaseAddress +  V977_SOFTWARE_RESET;
    vmeStatus = VME_WRITE_16( addr, data );
    //2.) mask the EOS and the Physics event trigger input
    addr =  V977_BaseAddress +  V977_INPUT_MASK;
    data = TR_EOS_EVENT_REQUEST | TR_PHYSICS_EVENT_REQUEST ;
    vmeStatus |= VME_WRITE_16( addr, data );
    //3.) Clear all outputs (except those set by software)
    addr =  V977_BaseAddress + V977_CLEAR_OUTPUT;
    data = 0;
    vmeStatus |= VME_WRITE_16( addr, data );
    //4.) Set all the busy outputs
    addr =  V977_BaseAddress +  V977_OUTPUT_SET;
    vmeStatus |= VME_READ_16( addr, &data );
    data |= TR_TRIGGER_BUSY;
    vmeStatus |= VME_WRITE_16( addr, data );

   } else if (strcmp( trParam->BoardType, "V2718") == 0 ) {
     triggerReceiverBoard = TR_V2718;
     // Set the busy (on output 4)
     vmeStatus = CAENVME_SetOutputRegister ( V2718_Handle, cvOut4Bit );
  } else {
    triggerReceiverBoard = TR_UNKNOWN;
    readList_error = PARAM_ERROR;    
    DO_ERROR {
      sprintf( message, "TriggerReceiver Type unknown: %s", trParam->BoardType );
      ERROR(message);
    }
    return;
  }

  if( vmeStatus != cvSuccess ){
    DO_ERROR {
      sprintf( message, "Error Arming TriggerReceiver: %d", vmeStatus );
      ERROR(message);
      readList_error = VME_ERROR;
    }
  } else
    DO_INFO {
    sprintf( message, "Arming TriggerReceiver successful: Module is %s ", trParam->BoardType );
    INFO(message);
  }
  // Set the first call flag 
  firstCall = 1; 
}

void DisArmTriggerReceiver( char * parPtr )
{
  //This will be the first to disarm (after the interface)
  DO_INFO {
    sprintf( message, "Disarming TriggerReceiver" );
    INFO(message);
  }
}

void AsynchReadTriggerReceiver( char * parPtr ){}

int  ReadEventTriggerReceiver( char * parPtr, struct eventHeaderStruct * header_ptr,
                                    struct equipmentHeaderStruct * eq_header_ptrs, datePointer * data_ptr )
{
  //  sprintf( message, "ENTERRING  ReadEventTriggerReceiver () for event %d for %s", DAQCONTROL->eventCount, getenv("DATE_ROLE_NAME"));
  //  ERROR(message);
    // This ReadEvent is always called at the beginning of the equipment List
    int dataStored = 0;    
    //trParam = (TriggerReceiver_ParType*) parPtr;

    int data = 0;
    int inputData = 0;

    static unsigned int spillNb = 0;
    static int nbInSpill = 0;
    //   EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, DAQCONTROL->eventCount );

    switch( triggerReceiverBoard ) {
    case TR_V977 :
      // Read the inputs and Flip flop Q status and send them to data stream 
      addr =  V977_BaseAddress + V977_INPUT_READ;
      vmeStatus = VME_READ_16(addr,&inputData);
      addr =  V977_BaseAddress + V977_SINGLEHIT_READ;
      vmeStatus |= VME_READ_16(addr,data_ptr);
      *data_ptr = (*data_ptr & 0x0000FFFF) | (inputData << 16);

      dataStored = sizeof(*data_ptr);
      eq_header_ptrs->equipmentBasicElementSize = 4;

      //      sprintf( message, "Trigger Reg is 0x%X", *data_ptr);
      //     ERROR(message);

      /*
      // the first 8 bits are reserved for DAQ triggers
      int nbits=0, ibit;
      for (ibit=0; ibit<8; ibit++){
	nbits += (*data_ptr>>ibit) & 0x1 ;
      }
      if (nbits > 1) 
	DO_INFO {
        sprintf( message, "%d different DAQ trigger type received ", nbits );
	// Not yet implemented
        INFO(message);
      }
      */
      if ( inputData & TR_SOS_EVENT_BUSY ){
        // Acknowledge: mask the output of the request
        // that will reset the input of the busy
        addr =  V977_BaseAddress + V977_OUTPUT_MASK;
        vmeStatus = VME_READ_16( addr, &data );
        data |= TR_SOS_EVENT_REQUEST;
        vmeStatus = VME_WRITE_16( addr, data );
        DO_DETAILED {
          sprintf( message, "SOS Trigger Received "); 
          INFO(message);
        }
        spillNb++;
        nbInSpill = 0 ;
        EVENT_ID_SET_BURST_NB( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_BURST( header_ptr->eventId, nbInSpill );

        //VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
        header_ptr->eventType = START_OF_BURST;
        return dataStored;
      }

      if ( inputData & TR_PHYSICS_EVENT_BUSY ){
        // Acknowledge: mask the output of the request
        addr =  V977_BaseAddress + V977_OUTPUT_MASK;
        vmeStatus = VME_READ_16( addr, &data );
        data |= TR_PHYSICS_EVENT_REQUEST;
        vmeStatus = VME_WRITE_16( addr, data );
        DO_DETAILED {
          sprintf( message, "PHYSICS Trigger Received "); 
          INFO(message);
        }
        nbInSpill++ ;
        EVENT_ID_SET_BURST_NB( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_BURST( header_ptr->eventId, nbInSpill );

//      VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
        header_ptr->eventType = PHYSICS_EVENT;
        return dataStored;
      }

      if ( inputData  & TR_EOS_EVENT_BUSY ){
        // Acknowledge: mask the output of the request
        addr =  V977_BaseAddress + V977_OUTPUT_MASK;
        vmeStatus = VME_READ_16( addr, &data );
        data |= TR_EOS_EVENT_REQUEST;
        vmeStatus = VME_WRITE_16( addr, data );
        DO_DETAILED {
          sprintf( message, "EOS Trigger Received "); 
          INFO(message);
        }
        //nbInSpill++ ;
        EVENT_ID_SET_BURST_NB( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, spillNb );
        //EVENT_ID_SET_NB_IN_BURST( header_ptr->eventId, nbInSpill );

//      VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
        header_ptr->eventType = END_OF_BURST;
        return dataStored;
      }
      if ( inputData & TR_CALIBRATION_EVENT_BUSY ){
        // Acknowledge: mask the output of the request
        addr =  V977_BaseAddress + V977_OUTPUT_MASK;
        vmeStatus = VME_READ_16( addr, &data );
        data |= TR_CALIBRATION_EVENT_REQUEST;
        vmeStatus = VME_WRITE_16( addr, data );
        DO_ERROR {
          sprintf( message, "CALIBRATION Trigger Received "); 
          INFO(message);
        }
        nbInSpill++ ;
        EVENT_ID_SET_BURST_NB( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_RUN( header_ptr->eventId, spillNb );
        EVENT_ID_SET_NB_IN_BURST( header_ptr->eventId, nbInSpill );

//      VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
        header_ptr->eventType = CALIBRATION_EVENT;
        return dataStored;
      }
      sprintf( message, "Warning: Invalid event trigger "); 
      ERROR(message);

	
      /*
      switch ( *data_ptr & TR_TRIGGER_MASK ) {
      case TR_VALID_PHYSICS_EVENT :
	VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
	header_ptr->eventType = PHYSICS_EVENT;
	// UnMasking the EOS event trigger inputs
	addr =  V977_BaseAddress +  V977_INPUT_MASK;
	data = TR_CALIBRATION_EVENT_REQUEST | TR_PHYSICS_EVENT_REQUEST;
	VME_WRITE_16( addr, data );
	break;
      case TR_PHYSICS_EVENT :
	sprintf( message, "Warning: Invalid event trigger "); 
	ERROR(message);
	header_ptr->eventType = PHYSICS_EVENT;
	// UnMasking the EOS event trigger inputs
	addr =  V977_BaseAddress +  V977_INPUT_MASK;
	data = TR_CALIBRATION_EVENT_REQUEST | TR_PHYSICS_EVENT_REQUEST;
	VME_WRITE_16( addr, data );
	break;
      case TR_CALIBRATION_EVENT :
	header_ptr->eventType = CALIBRATION_EVENT;
	VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
	break;
      case TR_SOS_EVENT :
	// That is Start of Spill (Start of Burst in DATE terminology)
	header_ptr->eventType = START_OF_BURST;
	VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
	break;
      case TR_VALID_EOS_EVENT :
	VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
      case TR_EOS_EVENT :
	// That is End of Spill (End of Burst in DATE terminology)
	header_ptr->eventType = END_OF_BURST;
	break;
      case (TR_CALIBRATION_EVENT|TR_SOS_EVENT):
	header_ptr->eventType = START_OF_BURST;
	sprintf( message, "Trigger clash (Oreg = %#lX). We should clean up the boards (TODO) ", 
		 (unsigned long) *data_ptr );
	ERROR(message);
	addr =  V977_BaseAddress + V977_CLEAR_OUTPUT;
	vmeStatus = VME_WRITE_16( addr, data );
	// Reset the output mask
	addr =  V977_BaseAddress + V977_OUTPUT_MASK;
	data = 0;
	vmeStatus = VME_WRITE_16( addr, data );
	// set the output like it was a normal SOS trigger
	addr =  V977_BaseAddress + V977_INPUT_SET;
	data = TR_SOS_EVENT_REQUEST;
	vmeStatus = VME_WRITE_16( addr, data );
	data = 0;
	vmeStatus = VME_WRITE_16( addr, data );
	addr =  V977_BaseAddress + V977_OUTPUT_MASK;
	data = TR_SOS_EVENT_REQUEST;
	vmeStatus = VME_WRITE_16( addr, data );
	break;
      default:
	DO_ERROR {
	  sprintf( message, "Unexpected trigger received (Oreg = %#lX). Fatal ! ",
		   (unsigned long) *data_ptr );
	  ERROR(message);
	}
	readList_error = SYNC_ERROR;	
	break;
      } 
      break;
      */
    case TR_V2718 :
      SET_TRIGGER_IN_PATTERN( header_ptr->eventTriggerPattern, 1 );
//      VALIDATE_TRIGGER_PATTERN( header_ptr->eventTriggerPattern );
      break;
    default :
      DO_ERROR {
        sprintf( message, "Unknown Trigger Receiver type in ReadEventTriggerReceiver");
        ERROR(message);
      }
      break;
    }
    return dataStored;
}

int  EventArrivedTriggerReceiver( char * parPtr ) { 
  unsigned short regData;
  unsigned long addr;
  short data=0;
  //trParam = (TriggerReceiver_ParType*) parPtr;

  switch( triggerReceiverBoard ) {
  case TR_V977 :
    if ( firstCall == 1 ){ // Reset the busy, keeping the other output set
      addr =  V977_BaseAddress +  V977_OUTPUT_SET;
      vmeStatus = VME_READ_16( addr, &data );
      data &= ~TR_TRIGGER_MASK;
      vmeStatus = VME_WRITE_16( addr, data );
      // unmask all inputs
      //      addr =  V977_BaseAddress +  V977_INPUT_MASK;
      //      data = 0;
      //      vmeStatus = VME_WRITE_16( addr, data );
      firstCall = 0;
      return 0;
     } 

    addr =  V977_BaseAddress + V977_INPUT_READ;
    vmeStatus = VME_READ_16( addr, &data);
    if ( vmeStatus != cvSuccess ) {
      DO_ERROR{
        sprintf( message, "Error: Cannot read V977_INPUT_READ register" );
        ERROR( message );
        readList_error = VME_ERROR;
      }
      return 0;
    }

    if ( data & TR_SOS_EVENT_BUSY ){
      addr = V977_BaseAddress +  V977_INPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      // mask inputs for CALIBRATION and SOS events
      data |= TR_CALIBRATION_EVENT_REQUEST | 
              TR_SOS_EVENT_REQUEST | 
              TR_EOS_EVENT_REQUEST;
      // unmask inputs for PHYSICS
      data &= ~(TR_PHYSICS_EVENT_REQUEST );
      vmeStatus |= VME_WRITE_16( addr, data );
      gettimeofday(&timetag0,NULL);
      return 1;
    }
    if ( data & TR_PHYSICS_EVENT_BUSY ){
      addr = V977_BaseAddress +  V977_INPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      // mask inputs for CALIBRATION, PHYSICS and SOS events
      data |= TR_CALIBRATION_EVENT_REQUEST |
              TR_SOS_EVENT_REQUEST | 
              TR_PHYSICS_EVENT_REQUEST;
      // unmask inputs for EOS
      data &= ~(TR_EOS_EVENT_REQUEST);
      vmeStatus |= VME_WRITE_16( addr, data );
      gettimeofday(&timetag0,NULL);
      return 1;
    }
    if ( data & TR_EOS_EVENT_BUSY ){
      addr = V977_BaseAddress +  V977_INPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      // mask inputs for CALIBRATION, PHYSICS and EOS events
      data |= TR_CALIBRATION_EVENT_REQUEST |
              TR_EOS_EVENT_REQUEST | 
              TR_PHYSICS_EVENT_REQUEST;
      // unmask inputs for SOS
      data &= ~(TR_SOS_EVENT_REQUEST);
      vmeStatus |= VME_WRITE_16( addr, data );
      gettimeofday(&timetag0,NULL);
      return 1;
    }
    if ( data & TR_CALIBRATION_EVENT_BUSY ){

      addr = V977_BaseAddress +  V977_INPUT_MASK;
      vmeStatus |= VME_READ_16( addr, &data );
      // mask inputs for CALIBRATION, PHYS and EOS events
      data |= TR_CALIBRATION_EVENT_REQUEST |
              TR_EOS_EVENT_REQUEST | 
              TR_PHYSICS_EVENT_REQUEST;
      // unmask inputs for SOS events
      data &= ~(TR_SOS_EVENT_REQUEST);
      vmeStatus |= VME_WRITE_16( addr, data );
      gettimeofday(&timetag0,NULL);
      return 1;
    }
    return 0;

    /*
    if ( data & TR_TRIGGER_RECEIVED ){
      // trigger has arrived on at least one of the trigger channels
      // the corrsponding outputs are set automatically by the board -> busy
     // Acknowledge Trigger accordingly by masking the corresponding output
      switch ( data & TR_TRIGGER_RECEIVED ) {
      case TR_PHYSICS_EVENT_BUSY:
//	addr =  V977_BaseAddress + V977_OUTPUT_SET;
//	data = TR_PHYSICS_EVENT_BUSY;
//	vmeStatus = VME_WRITE_16( addr, data);


	//addr =  V977_BaseAddress + V977_SINGLEHIT_READ;
	// vmeStatus = VME_READ_16( addr, &data);
	//	sprintf( message, "We are expecting 0x%X and we get 0x%X for event %d in %s", 
	//		 TR_VALID_PHYSICS_EVENT,  data, 
	//	 DAQCONTROL->eventCount+1, getenv("DATE_ROLE_NAME"));
	//       ERROR(message);
	addr = V977_BaseAddress +  V977_OUTPUT_MASK;
	vmeStatus = VME_READ_16( addr, &data );
	data |= TR_PHYSICS_EVENT_REQUEST;

	break;
      case TR_CALIBRATION_EVENT_BUSY:
	//	sprintf( message, "Calibration Trigger received for event %d in %s", 
	//		 DAQCONTROL->eventCount+1, getenv("DATE_ROLE_NAME"));
	//	ERROR(message);
//	addr =  V977_BaseAddress +  V977_INPUT_MASK;
//	data = TR_TRIGGER_REQUEST ;
        addr = V977_BaseAddress +  V977_OUTPUT_MASK;
        vmeStatus = VME_READ_16( addr, &data );
	data = TR_CALIBRATION_EVENT_REQUEST;
	break;
      case TR_SOS_EVENT_BUSY:
	// Masking the calibration event trigger and unmasking the physics event trigger inputs
	// addr =  V977_BaseAddress +  V977_INPUT_MASK;
	// data = TR_EOS_EVENT_REQUEST | TR_CALIBRATION_EVENT_REQUEST ;
	// VME_WRITE_16( addr, data );
        // data = TR_SOS_EVENT_REQUEST;
	//	sprintf( message, "SOS Trigger received for event %d in %s", 
	//	 DAQCONTROL->eventCount+1, getenv("DATE_ROLE_NAME"));
	//	ERROR(message);
        addr = V977_BaseAddress +  V977_OUTPUT_MASK;
        vmeStatus = VME_READ_16( addr, &data );
        data = TR_SOS_EVENT_REQUEST;
	break;
      case TR_EOS_EVENT_BUSY:
 	// addr =  V977_BaseAddress + V977_SINGLEHIT_READ;
	// vmeStatus = VME_READ_16( addr, &data);
	//	sprintf( message, "We are expecting 0x%X and we get 0x%X for event %d in %s", 
	//		 TR_VALID_EOS_EVENT, data, 
	//	 DAQCONTROL->eventCount, getenv("DATE_ROLE_NAME"));
	//	ERROR(message);
	addr = V977_BaseAddress +  V977_OUTPUT_MASK;
	vmeStatus = VME_READ_16( addr, &data );
	data |= TR_EOS_EVENT_REQUEST;
	break;
      default :

	if ( data & (TR_CALIBRATION_EVENT_BUSY | TR_SOS_EVENT_BUSY) ) {
	  // Start of spill arrive while the system is busy with calibration
	  sprintf( message, "SOS request has arrived while busy with calibration event. (Oreg = %#lX) ", 
		   (unsigned long) (data  & TR_TRIGGER_MASK));
	  ERROR(message);     
	  sprintf( message, "Trying to ignore, hoping the SOS will last long enough.");
	  ERROR(message);	
	  addr = V977_BaseAddress +  V977_OUTPUT_MASK;
	  data = TR_CALIBRATION_EVENT_REQUEST;
	  vmeStatus = VME_WRITE_16( addr, data );	  
	  return 0;
	}
	readList_error = SYNC_ERROR;

	DO_ERROR {
	  sprintf( message, "Trigger pattern is not valid (Oreg = %#lX) ", 
		   (unsigned long) (data  & TR_TRIGGER_MASK));
	  ERROR(message);
	}
	return 0;	
	break;
      } 
      addr = V977_BaseAddress +  V977_OUTPUT_MASK;
      vmeStatus = VME_WRITE_16( addr, data );
      // time tag for DAQ readout time measurement
      gettimeofday(&timetag0,NULL);
      return 1;
      */

    break;
  case TR_V2718 : 
    if ( firstCall == 1 ){ // Reset the busy
      vmeStatus = CAENVME_ClearOutputRegister( V2718_Handle, cvOut4Bit );
      firstCall = 0;
      return 0;
    }
    vmeStatus = CAENVME_ReadRegister( V2718_Handle, cvInputReg, &regData );
    if ( vmeStatus != cvSuccess ) {
      DO_ERROR{
        sprintf( message, "Error: Cannot read the VME Input Register! Error # %d", vmeStatus );
        ERROR( message );
      }
      return 0;
    }

    if (( cvIn0Bit & regData ) == 1 ){ // event has arrived 
      // Set the busy
      vmeStatus = CAENVME_SetOutputRegister( V2718_Handle, cvOut4Bit );
      // time tag for DAQ readout time measurment
      gettimeofday(&timetag0,NULL);
      return 1;
    }
    break;
  default :
    DO_ERROR {
      sprintf( message, "Unknown Trigger Receiver type in EventArrivedTriggerReceiver");
      ERROR(message);
    }
    break;
  }

  return 0;
}

void PauseArmTriggerReceiver( char *) {}

void PauseDisArmTriggerReceiver( char *) {}
