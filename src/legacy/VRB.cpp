#include "VRB.h"

using namespace std;

using namespace MICE;
extern  DATEStatusClient *lDATEStatusClient;
extern bool EPICSClientsConnected;

///////////////////////////////////////////////////////////////////////
int SetMemInt( unsigned long ba) {
  int addr;
  short data;
  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_READ_16_A24( addr, &data );
  if( vmeStatus == cvSuccess ){
    //Set memory in internal mode, LSB =1  => INTERNAL MODE
    data |= VRB_Memory_Internal_Mode;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }

  return vmeStatus;
}

///////////////////////////////////////////////////////////////////////
int SetMemExt( unsigned long ba) {
  int addr;
  short data;
  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_READ_16_A24( addr, &data );
  if( vmeStatus == cvSuccess ){
    //Set memory in external mode, LSB =0  => EXTERNAL MODE
    data &= (~VRB_Memory_Internal_Mode);	
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }

  return vmeStatus;
}

///////////////////////////////////////////////////////////////////////
int ClearMemory(unsigned long addr, int nloc) {
  int iloc=0;
  short data=0xFFFF;
  short data_test=0;
  while(iloc<nloc && vmeStatus == cvSuccess ){
    vmeStatus = VME_WRITE_16_A24( addr+iloc, data );
    iloc+=2;
  }

  if( vmeStatus == cvSuccess )
  {
    vmeStatus = VME_READ_16_A24( addr+4, &data_test);
    if(data!=data_test) INFO("Error in ClearMemory ");
  }

  return vmeStatus;
}

///////////////////////////////////////////////////////////////////////
int SetClock( unsigned long ba, int clock) {
  int addr;
  short data;
  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_READ_16_A24( addr, &data );
  if( vmeStatus == cvSuccess ){
    data |= clock;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }

  return vmeStatus;
}

///////////////////////////////////////////////////////////////////////
int GlobalReset( unsigned long ba ) {
  int addr;
  short data;
  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_READ_16_A24( addr, &data );
  if( vmeStatus == cvSuccess ){
    data |= VRB_Global_Reset;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }
  if( vmeStatus == cvSuccess ){
    data &= ~VRB_Global_Reset;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }
  if( vmeStatus == cvSuccess ){
    data |= VRB_Global_Reset;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }

  return vmeStatus;
}


///////////////////////////////////////////////////////////////////////
int Start( unsigned long ba ) {
  int addr;
  short data;
  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_READ_16_A24( addr, &data );
  if( vmeStatus == cvSuccess ){
    data &= ~VRB_Start;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }
  if( vmeStatus == cvSuccess ){
    data |= VRB_Start;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }
  if( vmeStatus == cvSuccess ){
    data &= ~VRB_Start;
    vmeStatus = VME_WRITE_16_A24( addr, data );
  }

  return vmeStatus;
}

///////////////////////////////////////////////////////////////////////
unsigned int MakeBoardId(unsigned int boardId) {
  return (boardId<<6) & DBB_Board_Id_Mask;
}

///////////////////////////////////////////////////////////////////////
int ExecuteCommand( unsigned long ba, unsigned int boardId, unsigned int command ) {
  uint16_t data_16=0;
  //int   data_32=0;
  int addr;

  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_WRITE_16_A24( addr, data_16);

  if ( vmeStatus == cvSuccess ) {
    // Set lock=1 and loop=0  ?????????
    data_16 |= VRB_Lock;
    data_16 &= ~VRB_Loop;
    vmeStatus = VME_WRITE_16_A24( addr, data_16);
  }

  if ( vmeStatus == cvSuccess )
    vmeStatus =  SetMemInt( ba );     //set A and B in internal  

  if ( vmeStatus == cvSuccess ) {
    //Send the command  
    addr = ba + VRB_MEMORY_A;
    unsigned int ID = MakeBoardId( boardId );
    data_16 = DBB_CMD_TYPE | ID | (command & DBB_Command_Mask);
    //data_16 = 0x50A7;
    //sprintf( message, "Sending VRB command: %d  to board %d,  ID=%d", data_16, boardId,ID);
    //INFO( message );
    vmeStatus = VME_WRITE_16_A24( addr, data_16);
  }  

  if ( vmeStatus == cvSuccess ) {
    //clear space in memory B
    addr = ba + VRB_MEMORY_B;
    vmeStatus =  ClearMemory(addr, 4096);
  }

  if ( vmeStatus == cvSuccess ) {
    //set A and B in external
    vmeStatus =  SetMemExt(ba);
  }

  if ( vmeStatus == cvSuccess ) { 
    //Set bit 5 to use the test option
    data_16|= 1<<5;
    vmeStatus = VME_WRITE_16_A24( addr, data_16);
  }

  if ( vmeStatus == cvSuccess )
    vmeStatus = SetClock(  ba, VRB_40MHz_Clock );	  // Set the clock rate for transfer from A to B

  if ( vmeStatus == cvSuccess )
    vmeStatus = GlobalReset( ba );

  if( vmeStatus == cvSuccess )
    vmeStatus = Start( ba );

  usleep(20000);

  if( vmeStatus == cvSuccess )
    vmeStatus =  SetMemInt(ba);    //set A and B in internal
  /*  
  if( vmeStatus == cvSuccess )
  {
    addr = ba + VRB_MEMORY_B;
    vmeStatus = VME_READ_16_A24( addr, &data_16);
    sprintf( message, "Received VRB reply from board %d is : %d", boardId, data_16);
    INFO( message );
  }  
  */
  return vmeStatus;
}

///////////////////////////////////////////////////////////////////////
void ArmVRB( char * parPtr) {
  // Get VRB parameters 
  VRB_ParType* vrbParam = (VRB_ParType*) parPtr;

  DO_INFO {
    sprintf( message, "Arming VRB with following parameters:" );
    INFO( message );
    sprintf( message, "Base Addr.: %s, Geo: %d",vrbParam->BaseAddress, *vrbParam->GEO);
    INFO( message );
  }

  unsigned long ba = getBA( vrbParam->BaseAddress );
  uint16_t data_16=0;
  //int   data_32=0;
  int addr;

  addr = ba + VRB_CONFIGURATION_REGISTER;
  vmeStatus = VME_WRITE_16_A24( addr, data_16);

  if ( vmeStatus == cvSuccess )
    vmeStatus = VME_READ_16_A24( addr, &data_16);

  if ( vmeStatus == cvSuccess ) {
    // write the number of location to use in mem0ry B
    addr = ba + VRB_COUNTER_REGISTER;
    data_16= 1;
    vmeStatus = VME_WRITE_16_A24( addr, data_16);
  }

  if ( vmeStatus == cvSuccess )
    vmeStatus = ExecuteCommand(ba, BOARDID_ALL, DBB_CMD_RESET);

  if ( vmeStatus == cvSuccess ) {
    DO_INFO {
      sprintf( message, "VRB armed" );
      INFO( message );
    }
  } else{
    DO_ERROR {
      sprintf( message, "Error during arming" );
      INFO( message );
    }
  }
}

void DisArmVRB( char * ) {
  DO_INFO {
    sprintf( message, "VRB disarmed" );
    INFO( message );
  }
}

/////////////////////////////////////////////////////////////////////
void AsynchReadVRB( char * ) {}

/////////////////////////////////////////////////////////////////////
int ReadEventVRB( char *parPtr, struct eventHeaderStruct *header_ptr,
                  struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  unsigned short data_16;
  unsigned short bad_reply=0;
  int nWordsRead=0;
  int iMis=0;
  short nEvt = 0;

  // Get VRB parameters 
  VRB_ParType* vrbParam = (VRB_ParType*) parPtr;
  unsigned long ba = getBA( vrbParam->BaseAddress );
  int vrbGeo = (int)*vrbParam->GEO;

  // Reset all boards
  /*
  if ( header_ptr->eventType == START_OF_BURST){
    int firstBoard = (int)*vrbParam->DBBfirstID;
    int nBoards = (int)*vrbParam->nDBB;
    int boardId = firstBoard;
    int lastBoard = firstBoard + nBoards;

    while( boardId < lastBoard ){
      vmeStatus = ExecuteCommand(ba, boardId, DBB_CMD_RESET);
      boardId++;
    }
  }
  */

  if ( header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT ){
  //if ( header_ptr->eventType == CALIBRATION_EVENT ){
    eq_header_ptr->equipmentId = vrbGeo;

    int firstBoard = (int)*vrbParam->DBBfirstID;
    int nBoards = (int)*vrbParam->nDBB;
    int boardId = firstBoard;
    int lastBoard = firstBoard + nBoards;
    unsigned int ID = MakeBoardId( boardId );

    // checking status
    while( boardId < lastBoard ){		
      ID = MakeBoardId( boardId );
      vmeStatus = ExecuteCommand(ba, boardId, DBB_CMD_STATUS);
      if( vmeStatus == cvSuccess ){
        addr = ba + VRB_MEMORY_B;
        vmeStatus = VME_READ_16_A24( addr, &data_16);
        if(data_16 & 0x3F){
          DO_ERROR{
            sprintf( message, "Error during reading data in DBB %d, status is %d", boardId, data_16);
            INFO( message );
          }
          boardId++;
          continue;
        }
      }

      // read trigger count
      vmeStatus = ExecuteCommand(ba, boardId, DBB_CMD_TRIGGER_COUNT);
      addr = ba + VRB_MEMORY_B;
      vmeStatus = VME_READ_16_A24( addr, &data_16);
      ID = MakeBoardId( boardId );
      unsigned short expected = DBB_TRIGGER_COUNT_TYPE | ID;
      unsigned short received = data_16 & (DBB_Command_Type_Mask| DBB_Board_Id_Mask);
      bad_reply = (expected!=received);
      if( bad_reply ){
        INFO( " WARNING from ReadEventVRB: Automatic run stop in case of unexpected command reply" );
        sprintf( message, "FATAL: Wrong command reply");
        FATAL( message );
        readList_error = SYNC_ERROR;

        if(EPICSClientsConnected)
          lDATEStatusClient->Write("ERROR");

        return 0;
      }
      addr += DBB_WORD_SIZE;
      vmeStatus = VME_READ_16_A24( addr, &data_16);
      nEvt = data_16 & DBB_Trigger_Count_Reply_Mask;
      sprintf( message, "VRB %d : In DBB %d  number of triggers is : %d", vrbGeo, boardId, nEvt);
      INFO(message );
      eq_header_ptr->equipmentTypeAttribute[0] = nEvt;
      iMis=checkPartTriggerCount(header_ptr,nEvt);
      // all modules of the same ldc should have the same number of particle events
      // The first user attribute is used to store the number of particle triggers
      if ( iMis ) {
        INFO( " WARNING from ReadEventVRB: Automatic run stop in case of Mismatch is  enabled for mismatch > 1 " );
        //lDATEStatusClient->Write("ERROR");
        sprintf( message, "FATAL: Mismatch = %d", iMis );
        FATAL( message );
        readList_error = SYNC_ERROR;
        return 0;
      }

      // read spill width temporary
      unsigned short data1,data2;
      ID = MakeBoardId( boardId );
      vmeStatus = ExecuteCommand(ba, boardId,DBB_CMD_SPILL_WIDTH );
      addr = ba + VRB_MEMORY_B;
      vmeStatus = VME_READ_16_A24( addr, &data1);
      expected = DBB_SPILL_WIDTH_TYPE | ID;
      received = data1 & (DBB_Command_Type_Mask| DBB_Board_Id_Mask);
      data1 = data1&~(DBB_Command_Type_Mask| DBB_Board_Id_Mask);
      bad_reply = (expected!=received);
      addr += DBB_WORD_SIZE;
      vmeStatus = VME_READ_16_A24( addr, &data2);
      sprintf( message,
               "VRB %d : In DBB %d  spill width is w1 : 0x%x  , w2 : 0x%x   reply is bad %d",
               vrbGeo,boardId,data1,data2 ,bad_reply);
      INFO(message );

      sprintf( message, "VRB %d : ready for data transfer from DBB %d", vrbGeo, boardId);
      INFO( message );

      boardId++;
    }
  }
  return nWordsRead*DBB_WORD_SIZE;
}

/////////////////////////////////////////////////////////////////////
int EventArrivedVRB( char * ) {
  return 0;
}

/////////////////////////////////////////////////////////////////////
void PauseArmVRB( char *) {}

/////////////////////////////////////////////////////////////////////
void PauseDisArmVRB( char *) {}



