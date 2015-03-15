/***************************************************************************
*                                                                         *
* Originally created by J.S. Graulich May 2010                            *
*                                                                         *
***************************************************************************/

#include "EpicsClient.h"

using namespace MICE;
extern std::vector<PVClient *> gEPICSClients;
extern DATEPVDescription *lDescription;
extern bool EPICSClientsConnected;
extern double lReadWriteTimeout;
extern double lMonitorTimeout;
extern DATEStatusServerDescription lServerDescription;
extern DATEStatusServerDescriptionXmlReader lDescriptionReader;
extern DATEStatusClient *lDATEStatusClient;

extern unsigned int nBytsRecirded;

void ArmEpicsClient( char * parPtr ) {

  DO_INFO {
    sprintf( message, "Arming EpicsClient" );
    INFO( message );
    infoLog_f( LOG_FACILITY, LOG_ERROR, "Arming EpicsClient" );
  }

  char * DateSiteConfFilesPath;

  DateSiteConfFilesPath = getenv ("DATE_SITE_CONFIG");
  lDescriptionReader.ReadFile( std::string(DateSiteConfFilesPath) +
                               std::string("/DATEStatusServerDescription.xml"),
                               lServerDescription);
  lDATEStatusClient = new DATEStatusClient(lServerDescription, lReadWriteTimeout, lMonitorTimeout);

  lDescription = new DATEPVDescription();
  DATEPVDescriptionXmlReader lPVDescriptionReader;
  lPVDescriptionReader.ReadFile( std::string(DateSiteConfFilesPath) +
                               std::string("/DATEPVDescription.xml"),
                               *lDescription);

  sprintf(message, "NumPVs: %ld", lDescription->GetNumPVs());
  INFO( message );

  sprintf(message, "ReadTimeout: %f", lDescription->GetReadTimeout());
  INFO( message );

  sprintf(message, "WriteTimeout: %f", lDescription->GetWriteTimeout());
  INFO( message );

  for(u32 lIndex = 0; lIndex < lDescription->GetNumPVs(); lIndex++) {
    sprintf( message, "-- PV %ld : %s : %s", lIndex, lDescription->GetPVName(lIndex).c_str(), lDescription->GetPVTitle(lIndex).c_str());
    INFO( message );
  }

  try {
    gEPICSClients.resize(lDescription->GetNumPVs(), NULL);
    for(u32 lIndex = 0; lIndex < gEPICSClients.size(); lIndex++) {
      gEPICSClients[lIndex] = new PVClient(lDescription->GetPVName(lIndex),
                                           lDescription->GetReadTimeout(),
                                           lDescription->GetWriteTimeout());
    }

    EPICSClientsConnected = true;
    sprintf( message,"EPICS Client Connected");
    INFO( message );
  } catch(MICEException & lExc) {
    std::cerr << lExc.What() << std::endl;
    std::cerr << lExc.History() << std::endl;
    FATAL("FATAL error in EPICSClient: server is not responding.");
    readList_error = SYNC_ERROR;
    return;
  } catch(std::exception & lExc) {
    std::cerr << lExc.what() << std::endl;
    FATAL("FATAL error in EPICSClient: server is not responding.");
    readList_error = SYNC_ERROR;
    return;
  } catch(...) {
    std::cerr << "Unknown exception occurred..." << std::endl;
    FATAL("FATAL error in EPICSClient: server is not responding.");
    readList_error = SYNC_ERROR;
    return;
  }

  u32 lTempInt = DAQCONTROL->runNumber;
  int lIndex = lDescription->FindPVClientByTitle("RunNumber");
  if (lIndex!=-1 && EPICSClientsConnected) {
    gEPICSClients[lIndex]->Write(lTempInt);
    //gEPICSClients[lIndex]->Read(lTempInt);
    sprintf( message, "RunNumber is %d", (int)lTempInt);
    INFO( message );
  } else {
    sprintf( message, "PV RunNumber is not set.");
    ERROR( message );
  }
}

void DisArmEpicsClient( char * parPtr ) {

  DO_INFO {
    sprintf( message, "Disarming EpicsClient" );
    INFO( message );
  }

  if (EPICSClientsConnected) {
    try {
      for(u32 lIndex = 0; lIndex < gEPICSClients.size(); lIndex++) {
        delete gEPICSClients[lIndex];
      }
    }
    catch(MICEException & lExc)
    {
      std::cerr << lExc.What() << std::endl;
      std::cerr << lExc.History() << std::endl;
    }
    catch(std::exception & lExc)
    {
      std::cerr << lExc.what() << std::endl;
    }
    catch(...)
    {
      std::cerr << "Unknown exception occurred..." << std::endl;
    }
  }
}

void AsynchReadEpicsClient( char * parPtr ){}

int  ReadEventEpicsClient( char * parPtr, struct eventHeaderStruct * header_ptr,
                           struct equipmentHeaderStruct * eq_header_ptrs, datePointer * data_ptr ) {
  int nBytesStored(0);
  /*
  sprintf( message, "Entering ReadEventEpicsClient on %s, event type is %d" ,
                    getenv("DATE_ROLE_NAME"),  header_ptr->eventType);
  ERROR(message);
  */
  /*
  if ( header_ptr->eventType == START_OF_BURST) {
    if(EPICSClientsConnected) {
      int lIndex = lDescription->FindPVClientByTitle("EventSize");
      if(lIndex != -1) {
        u32 lTempInt = DAQCONTROL->bytesRecorded - nBytsRecirded;
        gEPICSClients[lIndex]->Write(lTempInt);
        //gEPICSClients[lIndex]->Read(lTempInt);
        sprintf( message, "Event size : %d.", (int)lTempInt);
        INFO(message);
        nBytsRecirded = DAQCONTROL->bytesRecorded;
      }
    }
  }
  */
  if ( header_ptr->eventType == START_OF_RUN ||  header_ptr->eventType == END_OF_RUN ) {
    if(EPICSClientsConnected) {
      u32 lTempInt=0;
      std::string lTempStr("");
      try {
        int lIndex = lDescription->FindPVClientByTitle("RunNumber");
        if(lIndex != -1) {
          gEPICSClients[lIndex]->Read(lTempInt);
          sprintf( message, "ReadEventEpicsClient RN : %d", (int)lTempInt);
          ERROR(message);
          int nByts = SavePVInt(data_ptr, "RunNumber", lTempInt);
          data_ptr += nByts;
          nBytesStored += nByts;
        }

        lIndex = lDescription->FindPVClientByTitle("TriggerCondition");
        if(lIndex != -1) {
          gEPICSClients[lIndex]->Read(lTempStr);
          std::string message = "ReadEventEpicsClient  TC: " + lTempStr;
          ERROR(message.c_str());
          int nByts = SavePVString(data_ptr, "TriggerCondition", lTempStr);
          data_ptr += nByts;
          nBytesStored += nByts;
        }
      } catch(MICEException & lExc)
      {
        std::cerr << lExc.What() << std::endl;
        std::cerr << lExc.History() << std::endl;
        FATAL("FATAL error in EpicsClient: EPICS Client is disconnected");
        readList_error = SYNC_ERROR;
        return 0;
      } catch(std::exception & lExc) {
        std::cerr << lExc.what() << std::endl;
        FATAL("FATAL error in EpicsClient: EPICS Client is disconnected");
        readList_error = SYNC_ERROR;
        return 0;
      } catch(...) {
        std::cerr << "Unknown exception occurred..." << std::endl;
        FATAL("FATAL error in EpicsClient: EPICS Client is disconnected");
        readList_error = SYNC_ERROR;
        return 0;
      }
    }
  }

  return nBytesStored;
}

int  EventArrivedEpicsClient( char * parPtr ) {
  return 0;
}

void PauseArmEpicsClient( char *parPtr) {}

void PauseDisArmEpicsClient( char *parPtr) {}


int SavePVDouble(datePointer * data_ptr, std::string pvTitle, double pv)
{
  int lIndex = lDescription->FindPVClientByTitle(pvTitle);
  if(lIndex != -1) {
    u32 data_uint = (lIndex & PV_TITLE_MASK) << PV_TITLE_SHIFT;
    data_uint |= (PV_TYPE_INT << PV_TYPE_SHIFT);
    *data_ptr = data_uint;
    data_ptr++;
    memcpy(data_ptr, &pv, sizeof(double));
    return sizeof(double) + sizeof(int);
  }
  return 0;
}

int SavePVInt(datePointer * data_ptr, std::string pvTitle, u32 pv)
{
  int lIndex = lDescription->FindPVClientByTitle(pvTitle);
  if(lIndex != -1) {
    u32 data_uint = (lIndex & PV_TITLE_MASK) << PV_TITLE_SHIFT;
    data_uint |= (PV_TYPE_INT << PV_TYPE_SHIFT);
    data_uint |= pv;
    *data_ptr = data_uint;
    return sizeof(int);
  }
  return 0;
}

int SavePVString(datePointer * data_ptr, std::string pvTitle, std::string pv)
{
  int lIndex = lDescription->FindPVClientByTitle(pvTitle);
  int len = pv.size();
  if(lIndex != -1) {
    u32 data_uint = (lIndex & PV_TITLE_MASK) << PV_TITLE_SHIFT;
    data_uint |= (PV_TYPE_STRING << PV_TYPE_SHIFT);
    data_uint |= len;
    data_ptr++;
    memcpy(data_ptr, pv.c_str(), len);
    return len + sizeof(int);
  }
  return 0;
}


