#include "MDEvRB.hh"
#include "VRBDef.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(VRBParams, MDEvRB)

MDEvRB::MDEvRB()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(VRBParams)
  mde_name_ = "VRB";
}

bool MDEvRB::Arm() {
  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  this->getInfo();

  mde_base_address_ = (*this)["BaseAddress"];

  if ( this->SetAllDBBGeos()  && // This function has to be called first !!!
       this->resetAllDBBs() &&
       this->checkAllDBBFV() &&
       this->SetAllDBBChEnableMasks() ) {
    MESSAGESTREAM << "Arming successful.";
    mde_messanger_->sendMessage(MDE_INFO);

    return true;
  } else {
    mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
    return false;
  }
}

bool MDEvRB::DisArm() {
  MESSAGESTREAM << "Disarming successful.";
  mde_messanger_->sendMessage(MDE_INFO);
  return true;
}

int MDEvRB::ReadEvent() {

  this->StartDBBReadout();

//  int nDBBs = (*this)["N_DBBs"];
//  for (int dbbId=1; dbbId<nDBBs+1; dbbId++) {
//    this->StartDBBReadout(dbbId);
//     usleep(2);
//     nbRead += this->GetDBBData(dbbId);
//  }

  return 0;
}

bool MDEvRB::StartDBBReadout(unsigned int dbbId) {
  // Make sure that there is no active data transmission between the VRB and the DBBs.
  while (this->getTransmissionStatus() == 1) {};

  if (dbbId != ALL_DBBS) {

    if (getDBBStatus(dbbId));
      return false;

//     if (this->getNTriggers(dbbId) == 0) {
//       MESSAGESTREAM << "No Triggers in DBB" << dbbId;
//       mde_messanger_->sendMessage(MDE_WARNING);
//       this->resetDBB(dbbId);
//       return false;
//     }

    mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_SEND_REQ;
    int d=1;
    mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, d);
    usleep(2);
    if( mde_vmeStatus_ == cvSuccess ) {
      MESSAGESTREAM << "Data requested for DBB" << dbbId;
      mde_messanger_->sendMessage(MDE_DEBUGGING);

      while (this->getTransmissionStatus() == 1) {};
      this->getNumDataWords(dbbId);

    } else {
      MESSAGESTREAM << "Unable to send data request for DBB" << dbbId;
      mde_messanger_->sendMessage(MDE_ERROR);
//       return false; //!!! FIX IT !!!
    }

  } else {
    int id=1, nDBBs = (*this)["N_DBBs"];
    int nTriggers_dbb1 = this->getNTriggers(1);
//     if (nTriggers_dbb1 == 0) {
//       MESSAGESTREAM << "No Triggers in DBB1";
//       mde_messanger_->sendMessage(MDE_WARNING);
//
//       return false;
//     }

    MESSAGESTREAM << "Number of Triggers in DBB1: " << nTriggers_dbb1;
    mde_messanger_->sendMessage(MDE_DEBUGGING);

    while(id<(nDBBs+1)) {
      if (getDBBStatus(id))
        return false;

      if(id!=1) {
        int nTriggers = this->getNTriggers(id);
        if (nTriggers != nTriggers_dbb1) {
          MESSAGESTREAM << "Number of triggers mismatch in DBB" << id
                        << " (" << nTriggers << " != " << nTriggers_dbb1 << ")";
          mde_messanger_->sendMessage(MDE_ERROR);
          nTriggers = this->getNTriggers(id);
          MESSAGESTREAM << "Number of triggers (2nd) : " << nTriggers;
          mde_messanger_->sendMessage(MDE_ERROR);
        }
      }
      id++;
    }

    mde_current_address_ = mde_base_address_ + START_CHAIN_TRANSFER;
    int d=1;
    mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, d);
    usleep(2);
    MESSAGESTREAM << "Chain data transfer requested.";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  return true;
}

int MDEvRB::GetDBBData(unsigned int dbbId) {
  int nbRead = 0;

  // Make sure that the data transmission between the VRB and the DBBs
  // is complate.
  while (this->getTransmissionStatus() == 1) {};

  if (getDBBStatus(dbbId))
    return 0;

  if (this->getNTriggers(dbbId) == 0) {
    MESSAGESTREAM << "No Triggers in DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
    return 0;
  }

  int ndw = this->getNumDataWords(dbbId);

  std::stringstream ss;
  ss << "Geo_DBB" << dbbId;
  int geo = (*this)[ss.str().c_str()];

  mde_current_address_ = (mde_base_address_ & 0xFF000000) + (geo << 16);
  mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, mde_dataPtr_, ndw*4, &nbRead );
  usleep(2);

  if (mde_vmeStatus_ != cvSuccess ) {
    // This is a serious error. Stop reading.
    MESSAGESTREAM << "Error in the BLT cycle. The VME error code is: " <<  getVmeStatusCodeName( mde_vmeStatus_ );
    mde_messanger_->sendMessage(MDE_ERROR);
  }

  return nbRead;
}

int MDEvRB::getTransmissionStatus() {
  int st=0;
  mde_current_address_ = mde_base_address_ + ADDR_TSM_ST;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &st);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to get the Transmission Status";
    mde_messanger_->sendMessage(MDE_ERROR);
    return -1;
  }

  MESSAGESTREAM << "Transmission Status is: " << st;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return st;
}

int MDEvRB::getDBBStatus(unsigned int dbbId) {
  int st=0;
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_STATUS;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &st);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to get DBB" << dbbId << " status";
    mde_messanger_->sendMessage(MDE_ERROR);
    return 1;
  }

  if (st==0) {
    MESSAGESTREAM << "DBB" << dbbId << " status is 0x00 ";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  } else {
    MESSAGESTREAM << "DBB" << dbbId << " status is: " << this->DbbStatusToString(st)
                  << " (0x" << std::hex << st << std::dec << ")";
    mde_messanger_->sendMessage(MDE_ERROR);
    mde_vmeStatus_ = VME_READ_32(mde_current_address_, &st);
    usleep(2);
    MESSAGESTREAM << "DBB" << dbbId << " status (2nd) is: " << this->DbbStatusToString(st)
                  << " (0x" << std::hex << st << std::dec << ")";
    mde_messanger_->sendMessage(MDE_ERROR);
  }

  return st;
}

std::string MDEvRB::DbbStatusToString(int st) {
  int tso = st & ST_TIMESTAMP_OVERFLOW;
  int l1fifo_o = st & ST_L1_FIFO_OVERFLOW;
  int l2fifo_o = st & ST_L2_FIFO_OVERFLOW;
  int cwb = st & ST_COMMAND_WHILE_BUSY;
  int dwb = st & ST_DATA_WHILE_BUSY;
  int cds = st & ST_COMMAND_DURING_SPILLGATE;

  std::string status_message;
  if (tso) status_message += " **Timestamp overflow**";
  if (l1fifo_o) status_message += " **L1 FIFO overflow**";
  if (l2fifo_o) status_message += " **L2 FIFO overflow**";
  if (cwb) status_message += " **New command received while busy**";
  if (dwb) status_message += " **Data received while busy**";
  if (cds) status_message += " **Command received during spillgate**";

  return status_message;
}

bool MDEvRB::resetDBB(unsigned int dbbId) {
  int r=0;
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_RESET;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &r);
  usleep(200);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to reset DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  MESSAGESTREAM << "Reset sent to DBB" << dbbId;
  mde_messanger_->sendMessage(MDE_DEBUGGING);


  if (getDBBStatus(dbbId))
    return false;

  return true;
}

bool MDEvRB::resetAllDBBs() {
  bool status=true;
  int nDBBs = (*this)["N_DBBs"];
  int i=1;
  while (i<=nDBBs && status) {
    status = this->resetDBB(i);
    i++;
  }

  return status;
}

bool MDEvRB::fastResetDBB(unsigned int dbbId) {
  int r=0;
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_FAST_RESET;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &r);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to reset (fast) DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  MESSAGESTREAM << "Fast reset sent to DBB" << dbbId;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

bool MDEvRB::fastResetAllDBBs() {
  bool status=true;
  int nDBBs = (*this)["N_DBBs"];
  int i=1;
  while (i<=nDBBs && status) {
    status = this->fastResetDBB(i);
    i++;
  }

  return status;
}

bool MDEvRB::testEM() {
  int err_count=0;

  for(unsigned int i=0;i<5000;i++){
    std::cout << i << std::endl;
    bool ok = this->SetDBBChEnableMask(i%6+1, 0x34ff06ed+i,
                                0x34ff06ed-i);
    if (!ok)
      err_count++;
    if (err_count>0)
      return false;
    usleep(20000);
  }

  return true;
}

bool MDEvRB::testFV() {
  int err_count=0;
  int fv;

  for(unsigned int i=0;i<5000;i++){
    int dbbId = i%6+1;
    //if (dbbId==1 || dbbId==2) continue;3
    fv = this->GetDBBFV(dbbId);
    std::cout << i << " DbbID: " << dbbId << " fv: " << fv << std::endl;
    if (fv!=8)
      err_count++;
    //    if (err_count>100)
    //return false;
    usleep(10000);
    //sleep(1);
  }

  return true;
}

int MDEvRB::EventArrived() {
  return 0;
}

bool MDEvRB::softwareClear() {
  return true;
}

int MDEvRB::getMaxMemUsed(int nEvents) {
  return 0;
}

int MDEvRB::getNTriggers(unsigned int dbbId) {
  unsigned int tr_count;
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_TRIG_COUNT;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &tr_count);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to get the Trigger count for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return 0;
  }

  MESSAGESTREAM << "DBB" << dbbId << " Trigger count: " << tr_count;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return tr_count;
}

int MDEvRB::getSpillWidth(unsigned int dbbId) {
  unsigned int sw;
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_SPILL_WIDTH;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &sw);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to get the Spill width for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return 0;
  }

  MESSAGESTREAM << "DBB" << dbbId << " Spill width: " << sw;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return sw;
}

unsigned int MDEvRB::GetDBBFV(unsigned int dbbId) {
  unsigned int fv;
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_FM_VERSION;
  usleep(2);
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &fv);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to get the firmware version for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return 0;
  }

  MESSAGESTREAM << "DBB" << dbbId << " firmware version: " << fv;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return fv;
}

bool MDEvRB::checkAllDBBFV() {
  bool status=true;
  int nDBBs = (*this)["N_DBBs"];
  int supported_fv = (*this)["DBB_FV"];
  int this_fv;
  int i=1;
  while (i<=nDBBs && status) {
    this_fv = this->GetDBBFV(i);
    status = (supported_fv == this_fv);
    i++;
  }

  if (status == false) {
    MESSAGESTREAM << "Unsupported firmware version (" << this_fv << ") in DBB" << i-1;
    mde_messanger_->sendMessage(MDE_ERROR);
  }

  return status;
}

bool MDEvRB::SetDBBChEnableMask(unsigned int dbbId, int mask0, int mask1) {

  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_CHMASK1;
  mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, mask1);
  usleep(2);
  if( mde_vmeStatus_ == cvSuccess ) {
    mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_CHMASK0;
    mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, mask0);
    usleep(2);
  }

  int mask0_set, mask1_set;
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set the channel enable mask for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_CHMASK1;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mask1_set);
  usleep(2);
  mde_current_address_ = mde_base_address_ + (dbbId << 8) + DBB_CHMASK0;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mask0_set);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to get the channel enable mask for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  if ( (mask0_set != mask0) || (mask1_set != mask1) ) {
    MESSAGESTREAM << "Channel enable mask mismatch for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);

    if (mask1_set != mask1) {
      MESSAGESTREAM << "mask1: " << std::hex << mask1 << "!=" << mask1_set << std::dec;
      mde_messanger_->sendMessage(MDE_ERROR);
    }

    if (mask0_set != mask0) {
      MESSAGESTREAM << "mask0: " << std::hex << mask0 << "!=" << mask0_set << std::dec;
      mde_messanger_->sendMessage(MDE_ERROR);
    }
    return false;
  }

  MESSAGESTREAM << "Channel enable mask for DBB" << dbbId << " is now: "
                << std::hex << mask1_set << " " << mask0_set << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEvRB::SetAllDBBChEnableMasks() {
  bool status=true;
  int nDBBs = (*this)["N_DBBs"];
  int i=1;
  while (i<=nDBBs && status) {
    std::stringstream ss1, ss0;
    ss0 << "ChannelMask0_DBB" << i;
    ss1 << "ChannelMask1_DBB" << i;
    int mask0 = (*this)[ss0.str().c_str()];
    int mask1 = (*this)[ss1.str().c_str()];
    status = this->SetDBBChEnableMask(i, mask0, mask1);
    i++;
  }
  return status;
}

bool MDEvRB::SetNDBBS(unsigned int nDBBs) {

  mde_current_address_ = mde_base_address_ + ADDR_NUM_DBBS;
  mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, nDBBs);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set the number of DBBs";
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  unsigned int xNDBBs= 999;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &xNDBBs);
  usleep(2);
  if (nDBBs!=xNDBBs) {
    MESSAGESTREAM << "Number of DBB mismatch " << nDBBs << "!=" << xNDBBs;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  MESSAGESTREAM << "Number of DBBs is set to: " << nDBBs;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEvRB::SetDBBGeo(unsigned int dbbId, unsigned int Geo) {

  mde_current_address_ = mde_base_address_ + 0xff00 + dbbId*4;
  mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, Geo);
  usleep(2);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set the Geo number for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  unsigned int xGeo= 999;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &xGeo);
  usleep(2);
  if (Geo!=xGeo) {
    MESSAGESTREAM << "Geo number mismatch for DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    MESSAGESTREAM << Geo << "!=" << xGeo;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }


  MESSAGESTREAM << "Geo number for DBB" << dbbId << " is set to: " << Geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEvRB::SetAllDBBGeos() {
  bool status=true;
  int nDBBs = (*this)["N_DBBs"];
  this->SetNDBBS(nDBBs);
//   this->SetNDBBS(1);
  int i=1;
  while (i<=nDBBs && status) {
    std::stringstream ss1, ss0;
    ss0 << "Geo_DBB" << i;
    int geo = (*this)[ss0.str().c_str()];
    status = this->SetDBBGeo(i, geo);
    i++;
  }
  return status;

  return true;
}

int MDEvRB::getNumDataWords(unsigned int dbbId) {
  int ndw=0;
  if (dbbId==1)
    mde_current_address_ = mde_base_address_ + ADDR_DBB1_DSIZE;

  else if (dbbId==2)
    mde_current_address_ = mde_base_address_ + ADDR_DBB2_DSIZE;

  else if (dbbId==3)
    mde_current_address_ = mde_base_address_ + ADDR_DBB3_DSIZE;

  else if (dbbId==4)
    mde_current_address_ = mde_base_address_ + ADDR_DBB4_DSIZE;

  else if (dbbId==5)
    mde_current_address_ = mde_base_address_ + ADDR_DBB5_DSIZE;

  else if (dbbId==6)
    mde_current_address_ = mde_base_address_ + ADDR_DBB6_DSIZE;

  else {
    MESSAGESTREAM << "Wrong DBB id: " << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &ndw);
  usleep(2);

  if( mde_vmeStatus_ == cvSuccess ) {
    MESSAGESTREAM << "Number of data words in DBB" << dbbId <<": " << ndw
                  << " (" << ndw*4 << " bytes)";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  } else {
    MESSAGESTREAM << "Unable to get the number of data words in DBB" << dbbId;
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  return ndw;
}

bool MDEvRB::MemClear(unsigned int memaddr, unsigned int memsize) {
  int nb;
  int zero_mask[memsize/4];
  for (unsigned int i=0;i<memsize/4;i++)
    zero_mask[i] = 0;
  mde_current_address_ = (mde_base_address_ & 0xFF000000) + memaddr;
  mde_vmeStatus_ = VME_WRITE_BLT_32(mde_current_address_, zero_mask, memsize, &nb);
  return true;
}


