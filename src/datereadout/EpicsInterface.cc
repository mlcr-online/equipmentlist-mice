#include "EpicsInterface.hh"
#include "MiceDAQMessanger.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

void ArmEpicsClient( char * parPtr ) {

#ifdef EPICS_FOUND
  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();

  MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
  epics_instance->Connect();
  if ( !epics_instance->isConnected() ) {
    messanger->sendMessage("EpicsClient: server is not responding.\n", MDE_FATAL);
    readList_error = SYNC_ERROR;
    return;
  }

  try {
    (*epics_instance)["DATEState"]->write(DS_ARMING);
    u32 lTmpInt = DAQCONTROL->runNumber;
    (*epics_instance)["RunNumber"]->write(lTmpInt);

    std::string lTempStr( DATE_VERSION );
    (*epics_instance)["DATEversion"]->write(lTempStr);

    messanger->sendMessage("EpicsClient: Arming successful.\n", MDE_INFO);
  } catch(MiceDAQException lExc) {
    messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
    messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
    messanger->sendMessage("EpicsClient: Arming failed.\n", MDE_FATAL);
  }

#endif
}

void DisArmEpicsClient( char * ) {

  MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();

#ifdef EPICS_FOUND
  MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();

  if ( !epics_instance->Disconnect() ) {
    messanger->sendMessage("EpicsClient: failed to Disconnect.\n", MDE_INFO);
    return;
  }

#endif

  messanger->sendMessage("EpicsClient: Disarming successful.\n", MDE_INFO);
}

void AsynchReadEpicsClient( char * ) {}

int  ReadEventEpicsClient( char *parPtr, struct eventHeaderStruct *header_ptr,
                              struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  int nBytesStored(0);

  if ( header_ptr->eventType == START_OF_RUN ||  header_ptr->eventType == END_OF_RUN ) {

#ifdef EPICS_FOUND

    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    if( epics_instance->isConnected() ) {
      i32 lTmpInt=0;
      std::string lTempStr("");
      int nByts=0;
      try {
        (*epics_instance)["RunNumber"]->read<i32>(lTmpInt);
        nByts = SavePVInt(data_ptr, "RunNumber", lTmpInt);
        data_ptr += nByts;
        nBytesStored += nByts;

        (*epics_instance)["TriggerCondition"]->read<std::string>(lTempStr);
        nByts = SavePVString(data_ptr, "TriggerCondition", lTempStr);
        data_ptr += nByts;
        nBytesStored += nByts;

      } catch(MiceDAQException lExc) {
        MiceDAQMessanger *messanger = MiceDAQMessanger::Instance();
        messanger->sendMessage("MiceDAQEpicsClient: EPICS Client is disconnected.", MDE_FATAL);
        messanger->sendMessage( lExc.GetLocation(),    MDE_FATAL);
        messanger->sendMessage( lExc.GetDescription(), MDE_FATAL);
        return 0;
      }
    }

#endif

  }

  return nBytesStored;
}

int  EventArrivedEpicsClient( char * ) {
  return 0;
}

void PauseArmEpicsClient( char *) {}

void PauseDisArmEpicsClient( char *) {}


#ifdef EPICS_FOUND

  int SavePVDouble(datePointer * data_ptr, std::string pvTitle, double pv) {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    int lIndex = epics_instance->FindPVByTitle(pvTitle.c_str());

    u32 data_uint = (lIndex & PV_TITLE_MASK) << PV_TITLE_SHIFT;
    data_uint |= (PV_TYPE_INT << PV_TYPE_SHIFT);
    *data_ptr = data_uint;
    data_ptr++;
    memcpy(data_ptr, &pv, sizeof(double));

    return sizeof(double) + sizeof(int);
  }

  int SavePVInt(datePointer * data_ptr, std::string pvTitle, u32 pv) {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    int lIndex = epics_instance->FindPVByTitle(pvTitle.c_str());

    u32 data_uint = (lIndex & PV_TITLE_MASK) << PV_TITLE_SHIFT;
    data_uint |= (PV_TYPE_INT << PV_TYPE_SHIFT);
    data_uint |= pv;
    *data_ptr = data_uint;

    return sizeof(int);
  }

  int SavePVString(datePointer * data_ptr, std::string pvTitle, std::string pv) {
    MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
    int lIndex = epics_instance->FindPVByTitle(pvTitle.c_str());

    int len = pv.size();

    u32 data_uint = (lIndex & PV_TITLE_MASK) << PV_TITLE_SHIFT;
    data_uint |= (PV_TYPE_STRING << PV_TYPE_SHIFT);
    data_uint |= len;
    data_ptr++;
    memcpy(data_ptr, pv.c_str(), len);

    return len + sizeof(int);
  }

#endif



