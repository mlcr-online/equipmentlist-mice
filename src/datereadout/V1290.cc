#include "V1290.hh"
#include "MDEv1290.hh"
#include "MiceDAQMessanger.hh"
#include "MiceDAQSpillStats.hh"

#ifdef EPICS_FOUND
  #include "MiceDAQEpicsClient.hh"
#endif

std::vector<MDEv1290 *> tdc (20);

void ArmV1290 (char *parPtr)
{
  V1290_ParType *V1290 = (V1290_ParType*) parPtr;

  int geo = *V1290->GEO;
  if (!tdc[geo])
    tdc[geo] = new MDEv1290();
  else {
    MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    *(messanger->getStream(tdc[geo])) << "The Geo Number " << geo << " is not unique.";
    messanger->sendMessage(MDE_FATAL);
    return;
  }
  tdc[geo]->setParams( "GEO",            *V1290->GEO )
                     ( "BaseAddress",    getBA( V1290->BaseAddress ) )
                     ( "ChannelMask",    channelPattern(V1290->ChannelConfig) )
                     ( "UseEventFIFO",   1 )
                     ( "UseExtendedTTT", 1 )
                     ( "DoInit",         decodeChar(V1290->Init) )
                     ( "TriggerMode",    *V1290->TriggerMode )
                     ( "WinWidth",       *V1290->WinWidth )
                     ( "WinOffset",      *V1290->WinOffset )
                     ( "SwMargin",       *V1290->SwMargin )
                     ( "RejMargin",      *V1290->RejMargin )
                     ( "TTSubtraction",  decodeChar(V1290->SubstTrigger) )
                     ( "DetectionEdge",  *V1290->DetectionEdge )
                     ( "TDCHeader",      decodeChar(V1290->TDCHeader) )
                     ( "LSBCode",        decodeBinStr( V1290->LSBCode ) )
                     ( "DeadtimeCode",   decodeBinStr( V1290->DeadtimeCode ) );

  if ( !tdc[geo]->Arm() )
    readList_error = VME_ERROR;
}

void DisArmV1290(char *parPtr) {
  V1290_ParType *V1290 = (V1290_ParType*) parPtr;
  int geo = *V1290->GEO;
  tdc[geo]->DisArm();
  delete tdc[geo];
}

void AsynchReadV1290(char *parPtr) {}

int ReadEventV1290(char *parPtr, struct eventHeaderStruct *header_ptr,
                   struct equipmentHeaderStruct *eq_header_ptr,  datePointer *data_ptr) {
  int dataStored = 0;

  if ( (header_ptr->eventType == PHYSICS_EVENT || header_ptr->eventType == CALIBRATION_EVENT) ) {

    V1290_ParType *V1290 = (V1290_ParType*) parPtr;
    int geo = *V1290->GEO;
    // Setting equipment header values
    eq_header_ptr->equipmentId = geo;
    eq_header_ptr->equipmentBasicElementSize = 4;

    if( !tdc[geo]->isGoodEvent() ){
      //Clear memory.
      tdc[geo]->softwareClear();
      return 0;
    }

    short nEvts = tdc[geo]->getNEventsStored();
    //MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
    //*(messanger->getStream( tdc[geo] )) << "Number of Particle Triggers is " << nEvts;
    //messanger->sendMessage(MDE_INFO);

    if ( !tdc[geo]->processMismatch(nEvts) ) {
      readList_error = SYNC_ERROR;

#ifdef EPICS_FOUND
      try {
        MiceDAQEpicsClient *epics_instance = MiceDAQEpicsClient::Instance();
        if( epics_instance->isConnected() )
          (*epics_instance)["DATEState"]->write(DS_ERROR);

      } catch(MiceDAQException lExc) {
        MiceDAQMessanger  *messanger  = MiceDAQMessanger::Instance();
        messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
        messanger->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
      }
#endif

      return 0;
    }

    // Cast the 64b DATE pointer into 32b.
    MDE_Pointer *data_ptr_32 = reinterpret_cast<MDE_Pointer *>(data_ptr);
    tdc[geo]->setDataPtr(data_ptr_32);
    dataStored += tdc[geo]->ReadEvent();
  }

  ( *MiceDAQSpillStats::Instance() )["DataRecorded"] += dataStored;
  return dataStored;
}

int EventArrivedV1290(char *parPtr) {
    return 0;
}

void PauseArmV1290(char *parPtr) {}

void PauseDisArmV1290(char *parPtr) {}

