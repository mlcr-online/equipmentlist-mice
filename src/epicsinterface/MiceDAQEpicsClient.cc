#include "MiceDAQEpicsClient.hh"
#include <cadef.h>
#include <fstream>


//MiceDAQEpicsClient* MiceDAQEpicsClient::instance_ = NULL;

MiceDAQEpicsClient::MiceDAQEpicsClient()
: connected_(false) {

  messanger_ =  MiceDAQMessanger::Instance();
}

bool MiceDAQEpicsClient::Connect(std::string configFile) {
  // Establish the connection.
  if(connected_)
    return true;

  if( !getenv("EPICS_CA_ADDR_LIST") ) {
    messanger_->sendMessage("MiceDAQEpicsClient: env EPICS_CA_ADDR_LIST is not set.", MDE_INFO);

    std::string epics_addr_list, str;
    std::string filename( EPICS_PATH);
    filename	 += "/EPICS_CA_ADDR_LIST";
    std::ifstream file(filename);
    if ( !file.is_open() ) {
      std::string lMessage("Can't open file " + filename + ".");
      std::string lLocation("DAQPVClientsMap::load(const char* fileName)");
      throw MiceDAQException(lMessage, lLocation, MDE_FATAL);
    }

    while (std::getline(file, str))
      epics_addr_list += str + " ";

    setenv("EPICS_CA_ADDR_LIST", epics_addr_list.c_str(), 1);
    setenv("EPICS_CA_AUTO_ADDR_LIST", "NO", 1);
  }

  try {
    if (configFile.size()<2) {
    // Get the path to the configuration files.
      std::string DateSiteConfFilesPath( getenv ("DATE_SITE_CONFIG") );
      //std::string DateSiteConfFilesPath("/home/daq/Software/DATE/equipmentlist-mice/src/epicsinterface/");
      PVmap_.load(DateSiteConfFilesPath + "/DAQPVClientsMap.txt");
    } else {
      PVmap_.load(configFile);
    }
  } catch(MiceDAQException lExc) {
    messanger_->sendMessage("MiceDAQEpicsClient: Unable to load the PV list.", MDE_FATAL);
    messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),       MDE_FATAL);
    messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(),    MDE_FATAL);
    return false;
  }

  try {
    EPICS_PVClients_.resize( PVmap_.size() );

    // Initialize
    int PVStatus = ca_context_create(ca_disable_preemptive_callback);
    if(PVStatus != ECA_NORMAL) {
      std::string lMessage("Failed to create context.");
      std::string lLocation("void MiceDAQEpicsClient::Connect()");
      throw MiceDAQException(lMessage, lLocation, MDE_ERROR);
    }

    for(int lIndex = 0; lIndex < PVmap_.size(); lIndex++) {
      messanger_->sendMessage("MiceDAQEpicsClient: adding PV " + PVmap_.getPVName(lIndex) + ".", MDE_INFO);
      EPICS_PVClients_[lIndex] = new DAQPVClient( PVmap_.getPVName(lIndex) );
    }

    connected_ = true;
    messanger_->sendMessage("MiceDAQEpicsClient: EPICS Client is Connected.", MDE_INFO);
  } catch(MiceDAQException lExc) {
    messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
    messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
    messanger_->sendMessage("MiceDAQEpicsClient: Unable to connect.",       MDE_FATAL);
    return false;
  }

  return connected_;
}

void MiceDAQEpicsClient::addPV(std::string pvName, std::string pvTitle) {
  try {
    messanger_->sendMessage("MiceDAQEpicsClient: adding PV " + pvName + ".", MDE_INFO);
    PVmap_.addPV(pvName, pvTitle);
    EPICS_PVClients_.push_back( new DAQPVClient(pvName) );
    connected_ = true;
  } catch(MiceDAQException lExc) {
    messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),    MDE_FATAL);
    messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(), MDE_FATAL);
    messanger_->sendMessage("MiceDAQEpicsClient: Unable to add PV " + pvName + ".", MDE_FATAL);
    connected_ = false;
  }
}

bool MiceDAQEpicsClient::Disconnect() {

  if ( this->isConnected() ) {
    messanger_->sendMessage("MiceDAQEpicsClient: Disconnectiong.", MDE_INFO);
    try {
      for(unsigned int lIndex = 0; lIndex < EPICS_PVClients_.size(); lIndex++)
        delete EPICS_PVClients_[lIndex];

      EPICS_PVClients_.resize(0);

      // Clese the comunication.
      ca_context_destroy();

      connected_ = false;
    } catch(MiceDAQException lExc) {
      messanger_->sendMessage("MiceDAQEpicsClient: Unable to disconnect.", MDE_FATAL);
      messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetLocation(),      MDE_FATAL);
      messanger_->sendMessage("MiceDAQEpicsClient: " + lExc.GetDescription(),   MDE_FATAL);
      return false;
    }
  }
  return true;
}

MiceDAQEpicsClient* MiceDAQEpicsClient::Instance() {
/*
  if (!instance_)
    instance_ = new MiceDAQEpicsClient(); // Only allow one instance of class to be generated.

  return instance_;
*/
 static MiceDAQEpicsClient instance;
 return &instance;

}

MiceDAQEpicsClient::~MiceDAQEpicsClient() {

  Disconnect();
  //delete instance_;
}

DAQPVClient* MiceDAQEpicsClient::operator[](const char* pvTitle) {
  int clNum = PVmap_.findByTitle(pvTitle);
  return EPICS_PVClients_[clNum];
}

#ifdef DATE_FOUND
  void MiceDAQEpicsClient::getDetectorNamesFromDB() {

    int status, i;
    //char msg[1024], *p, partitions[512];

    if ( (status = dbLoadRoles()) != DB_LOAD_OK ) {
      messanger_->sendMessage("MiceDAQEpicsClient: Unable to load Roles list from DB.", MDE_FATAL);
    }
    nDetectors=0;
    for ( i = 0; i != dbSizeRolesDb; i++ ) {
        std::cout << "*&%$  " << i << " " << dbRolesDb[i].name << std::endl;

      if (dbRolesDb[i].role == dbRoleDetector) {
/*
        trncpy(detectorNames[nDetectors],dbRolesDb[i].name,sizeof(detectorNames[nDetectors]));
        UPPER (detectorNames[nDetectors]);
        if (strncmp(detectorNames[nDetectors],"HLT",3) != 0) {
          nDetectors++;
        }
*/
      }
    }
  }

  void MiceDAQEpicsClient::getActiveLDCs() {
/*
    dbLdcPatternType LDC;
    int i,j,r;
    dbRoleType rtype=dbRoleUnknown;
    int status;

    if ( (status = dbLoadRoles()) != DB_LOAD_OK ) {
      messanger_->sendMessage("MiceDAQEpicsClient: Unable to load Roles list from DB.", MDE_FATAL);
    }

    r=dbGet_activeLDCs(&LDC);
    if (r==0) {
      rtype=dbRoleLdc;
      for (i=0;i<HOST_ID_MAX;i++) {
        if (DB_TEST_BIT(LDC,i)) {
          j=dbRolesFindId(i,rtype);
          if (j>=0) {
            printf("LDC %d : %s\n",i,dbRolesDb[j].name);
          }
        }
      }
    } else {
      printf("Failed to get active LDCs : %d\n",r);
    }
*/
  }

  void MiceDAQEpicsClient::getActiveGDCs() {

  }

#endif









/*
bool MDEEpicsInterface::Arm() {

  epicsClient_ = MiceDAQEpicsClient::Instance();
  epicsClient_->Connect();
  if (epicsClient_->isConnected())
    return true;
  else {
    mde_messanger_->sendMessage(this, "FATAL error in EPICSClient: server is not responding.", MDE_FATAL);
    //readList_error = SYNC_ERROR;
    return false;
  }
}

bool MDEEpicsInterface::DisArm() {
  return true;
}

int MDEEpicsInterface::ReadEvent() {

  int nBytesStored(0);


  return nBytesStored;
}

int  MDEEpicsInterface::EventArrived() {
  return 0;
}

*/


