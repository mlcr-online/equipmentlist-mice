#include "PartTriggerSelector.h"

using namespace std;

extern std::vector<PVClient *> gEPICSClients;
extern DATEPVDescription *lDescription;
extern bool EPICSClientsConnected;

void ArmPartTriggerSelector ( char *parPtr ) 
{
  uint16_t data = 0;
  PartTriggerSelector_ParType* params = ( PartTriggerSelector_ParType *) parPtr;
  unsigned long ba = getBA( params->BaseAddress );

  if (EPICSClientsConnected) {
    std::string lTempStr("");
    int lIndex = lDescription->FindPVClientByTitle("TriggerCondition");
    if(lIndex != -1) {
      //std::string lTempStr( params->TriggerCondition );
      try {
        //gEPICSClients[lIndex]->Write(lTempStr);
        gEPICSClients[lIndex]->Read(lTempStr);
        sprintf( message, "Trigger Condition : %s ", lTempStr.c_str());
        INFO( message );
      } catch (MICEException & lExc) {
        std::cerr << lExc.What() << std::endl;
        std::cerr << lExc.History() << std::endl;
        FATAL("FATAL error in PartTriggerSelector: EPICS Client is disconnected");
        readList_error = SYNC_ERROR;
        return;
      } catch(std::exception & lExc) {
        std::cerr << lExc.what() << std::endl;
        FATAL("FATAL error in PartTriggerSelector: EPICS Client is disconnected");
        readList_error = SYNC_ERROR;
        return;
      } catch(...) {
        std::cerr << "Unknown exception occurred..." << std::endl;
        FATAL("FATAL error in PartTriggerSelector: EPICS Client is disconnected");
        readList_error = SYNC_ERROR;
        return;
      }
    }
  }

  sprintf( message," Setting trigger condition to %s", params->TriggerCondition );
  INFO( message );

  // Set output mask for channels reserved for GVA1, TOF0 and TOF1
  addr = ba + V977_OUTPUT_MASK;
  vmeStatus = VME_READ_16( addr, &data );
  data |= PTS_GVA1 | PTS_TOF0 | PTS_TOF1;
  vmeStatus |= VME_WRITE_16( addr, data );

  // clear output channels reserved for GVA1, TOF0 and TOF1
  addr = ba + V977_OUTPUT_SET;
  vmeStatus |= VME_READ_16( addr, &data );
  data &= ~(PTS_GVA1 | PTS_TOF0 | PTS_TOF1);
  vmeStatus |= VME_WRITE_16( addr, data );

  if ( vmeStatus != cvSuccess ) {
    DO_ERROR {
      sprintf( message, "Error while arming PartTriggerSelectot, Error # %d", vmeStatus );
      ERROR( message );
    }
    readList_error = VME_ERROR;
  }

  if ( strcmp(params->TriggerCondition,"GVA1" ) == 0) {
    // set output for GVA1 
    vmeStatus = VME_READ_16( addr, &data );
    data |= PTS_GVA1 ;
    vmeStatus |= VME_WRITE_16( addr, data );
  } else if ( strcmp(params->TriggerCondition,"TOF0" ) == 0) {
    // set output for TOF0
    vmeStatus = VME_READ_16( addr, &data );
    data |= PTS_TOF0 ;
    vmeStatus |= VME_WRITE_16( addr, data );
  } else if ( strcmp(params->TriggerCondition,"TOF1" ) == 0) {
    // set output TOF1
    vmeStatus = VME_READ_16( addr, &data );
    data |= PTS_TOF1 ;
    vmeStatus |= VME_WRITE_16( addr, data );
  } else {
    // send warning message
    DO_ERROR{
      sprintf( message, "Unknown trigger condition: %s ; Setting TOF1 trigger", params->TriggerCondition );
      ERROR( message );
    }
    // set default = TOF1
    vmeStatus = VME_READ_16( addr, &data );
    data |= PTS_TOF1 ;
    vmeStatus |= VME_WRITE_16( addr, data );
  }
}

void DisArmPartTriggerSelector( char *parPtr) 
{
  uint16_t data = 0;
  PartTriggerSelector_ParType* params = ( PartTriggerSelector_ParType *) parPtr;
  unsigned long ba = getBA( params->BaseAddress );
  // clear output channels reserved for GVA1, TOF0 and TOF1
  addr = ba + V977_OUTPUT_SET;
  vmeStatus |= VME_READ_16( addr, &data );
  data &= ~(PTS_GVA1 | PTS_TOF0 | PTS_TOF1);
  vmeStatus |= VME_WRITE_16( addr, data );
}

void AsynchReadPartTriggerSelector( char *parPtr) {}

int ReadEventPartTriggerSelector( char *parPtr, struct eventHeaderStruct *header_ptr,
                    struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {  
  return 0; }

int EventArrivedPartTriggerSelector( char *parPtr ) {
  return 0;
}

void PauseArmPartTriggerSelector( char *) {}

void PauseDisArmPartTriggerSelector( char *) {}

