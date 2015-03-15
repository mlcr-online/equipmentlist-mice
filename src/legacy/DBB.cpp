#include "DBB.h"

using namespace std;

void ArmDBB( char * parPtr)
{
  // Get DBB parameters 
  DBB_ParType* dbbParam = (DBB_ParType*) parPtr;

  DO_INFO {
    sprintf( message, "Arming DBB with following parameters:" );
    INFO( message );
    sprintf( message, "VRB Base Addr.: %s, Geo: %d",dbbParam->VRBBaseAddress, *dbbParam->GEO);
    INFO( message );
  }
  // Nothing to do: the DBB has been reseted by the VRB
}

void DisArmDBB( char * )
{
  DO_INFO {
    sprintf( message, "DBB disarmed" );
    INFO( message );
  }
}

/////////////////////////////////////////////////////////////////////
void AsynchReadDBB( char * ) {}

/////////////////////////////////////////////////////////////////////
int ReadEventDBB( char *parPtr, struct eventHeaderStruct *header_ptr,
                  struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr)
{
  //int nBytesRead=0;
  int nWordsRead=0;
  short nEvt = 0;

  // Get DBB parameters 
  DBB_ParType* dbbParam = (DBB_ParType*) parPtr;
  unsigned long ba = getBA( dbbParam->VRBBaseAddress );
  int boardId = *(dbbParam->GEO);

  uint16_t data(0);
  int addr;

  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT ){
  //if ( header_ptr->eventType == CALIBRATION_EVENT ){
    eq_header_ptr->equipmentId = boardId ;

    // read status
    ExecuteCommand(ba, boardId, DBB_CMD_STATUS);
    addr = ba + VRB_MEMORY_B;
    VME_READ_16_A24( addr, &data);
    sprintf( message, "Status in DBB %d, status is 0x%x", boardId, data);
    INFO( message );

    // read trigger count quickly (no check)
    vmeStatus = ExecuteCommand(ba, boardId, DBB_CMD_TRIGGER_COUNT);
    addr = ba + VRB_MEMORY_B + DBB_WORD_SIZE ;
    vmeStatus = VME_READ_16_A24( addr, &data);
    nEvt = data & DBB_Trigger_Count_Reply_Mask;
    sprintf( message, "DBB %d : Number of triggers is : %d", boardId, nEvt);
    INFO(message );

    // Send Read Event Command
    if (nEvt) {
      vmeStatus = ExecuteCommand(ba, boardId, DBB_CMD_SEND_DATA);
      sprintf( message, "DBB %d : data transfer started", boardId);
      ERROR( message );
      // Read data from memory Bank B until we get 0xFFFF or reach max size    
      addr = ba + VRB_MEMORY_B;
      bool timeToBreak = false;
      while( nWordsRead < DBB_BANK_SIZE/DBB_WORD_SIZE ) {
        VME_READ_16_A24( addr, &data );
        if( data==0xFFFF ) break;
        *data_ptr = (data << 16)&0xFFFF0000;
        nWordsRead++;
        addr += DBB_WORD_SIZE;
        VME_READ_16_A24( addr, &data );
        if ( data==0xFFFF ) timeToBreak=true; // make sure the data size is always a multiple of 4
        *data_ptr = *data_ptr | (data & 0xFFFF);
        if (nWordsRead <20) { 
          sprintf( message, "DBB %d : data = 0x%x", boardId, (int)*data_ptr );
          INFO( message );
        }
        nWordsRead++;
        data_ptr++;
        addr += DBB_WORD_SIZE;
        if (timeToBreak) break;
      }

      DO_INFO{
        sprintf( message, "DBB %d data size : %d", boardId, nWordsRead*DBB_WORD_SIZE );
        INFO( message );
      }
    }

    ExecuteCommand(ba, boardId, DBB_CMD_STATUS);
    addr = ba + VRB_MEMORY_B;
    VME_READ_16_A24( addr, &data);
    sprintf( message, "Status in DBB %d after readout, status is 0x%x ", boardId, data);
    INFO( message );
  }

  if ( header_ptr->eventType == START_OF_BURST ){
    ExecuteCommand(ba, boardId, DBB_CMD_STATUS);
    addr = ba + VRB_MEMORY_B;
    VME_READ_16_A24( addr, &data);
    sprintf( message, "Status in DBB %d in SOS: 0x%x ", boardId, data);
    INFO( message );
  }

  if ( header_ptr->eventType == END_OF_BURST ){
    ExecuteCommand(ba, boardId, DBB_CMD_STATUS);
    addr = ba + VRB_MEMORY_B;
    VME_READ_16_A24( addr, &data);
    sprintf( message, "Status in DBB %d in EOS: 0x%x ", boardId, data);
    INFO( message );
  }


  return nWordsRead*DBB_WORD_SIZE;
}

/////////////////////////////////////////////////////////////////////
int EventArrivedDBB( char * ) {
  return 0;
}

/////////////////////////////////////////////////////////////////////
void PauseArmDBB( char *) {}

/////////////////////////////////////////////////////////////////////
void PauseDisArmDBB( char *) {}





