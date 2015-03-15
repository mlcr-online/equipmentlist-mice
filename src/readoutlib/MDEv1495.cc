#include "MDEv1495.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(V1495Params, MDEv1495)

using std::hex;
using std::dec;

MDEv1495::MDEv1495() {
  SET_ALL_PARAMS_TO_ZERO(V1495Params)
  mde_name_ = "V1495";
}

bool MDEv1495::Arm() {
  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  this->getInfo();
  mde_base_address_ = (*this)["BaseAddress"];

  if ((*this)["SGOpenDelay"] >= (*this)["SGCloseDelay"]) {
    mde_messanger_->sendMessage(this, "The Spill Gate time is negative. \n", MDE_FATAL); 
  }

  if (this->reset() &&
      this->setGEO((*this)["GEO"]) &&
      this->setSGOpenDelay((*this)["SGOpenDelay"]) &&
      this->setSGCloseDelay((*this)["SGCloseDelay"]) &&
      this->setSggCtrl((*this)["SggCtrl"]) &&
      this->setTrCtrl((*this)["TriggerLogicCtrl"]) &&
      this->setTOF0Mask((*this)["TOF0Mask"]) &&
      this->setTOF1Mask((*this)["TOF1Mask"]) &&
      this->setTOF2Mask((*this)["TOF2Mask"]) &&
      this->setPTVetoLenght((*this)["PartTrVetoLenght"])) {

    mde_messanger_->sendMessage(this,"Arming successful. \n", MDE_INFO);
    return true;
  } else {
    mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
    return false;
  }
}

bool MDEv1495::DisArm() {
  return true;
}

int MDEv1495::ReadEvent() {
  int nbRead = 0;
  mde_data_32_ = 0;
  mde_base_address_ = (*this)["BaseAddress"];
  int nDW = this->getNDW();
//   int nTr = this->getNTriggers();

  if (!nDW)
    return 0;

  mde_current_address_ = mde_base_address_ + V1495_MEB;
  mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, mde_dataPtr_, nDW*4, &nbRead );
  if ( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "BLT cycle status: " << getVmeStatusCodeName(mde_vmeStatus_)
                  << "! Read " << nbRead << " bytes.";
    mde_messanger_->sendMessage(MDE_ERROR);
  }

  MESSAGESTREAM << "Data stored: " << nbRead;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return nbRead;
}

int MDEv1495::EventArrived() {
  return 0;
}

bool MDEv1495::reset() {
  //Reset the board
  mde_current_address_ = mde_base_address_ + V1495_BOARD_RESET;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to reset the board.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Board reset.";
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

int MDEv1495::getNSpils() {
  int nSpils(0);
  //Get the Number of Spills
  mde_current_address_ = mde_base_address_ + V1495_N_SPILLS;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &nSpils);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to get the Number of Spills.", MDE_FATAL);
    return 0;
  }

  MESSAGESTREAM << "The Number of Spills is " << nSpils;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return nSpils;
}

int MDEv1495::getNDW() {
  int nDW(0);
  //Get the Number of Data Words
  mde_current_address_ = mde_base_address_ + V1495_N_DW;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &nDW);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to get the Number of Data Words.", MDE_FATAL);
    return 0;
  }

  MESSAGESTREAM << "The Number of Data Words is " << nDW;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return nDW;
}

int MDEv1495::getNTriggers() {
  int nTr(0);
  //Get the Number of Triggers
  mde_current_address_ = mde_base_address_ + V1495_N_TRIGGERS;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &nTr);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to get the Number of Triggers.", MDE_FATAL);
    return 0;
  }

  MESSAGESTREAM << "The Number of Triggers is " << nTr;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return nTr;
}

int MDEv1495::softwareStart(bool wait) {
  int nSpils(0), nSpils1(0);
  //Generate new  Spill Gate.
  mde_current_address_ = mde_base_address_ + V1495_SWR_SG;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &nSpils);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to generate new Spill Gate.", MDE_FATAL);
    return 0;
  }

  MESSAGESTREAM << "Start of Software Spill " << nSpils;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  if (wait) {
    // Wait till the sycle is completed.
    mde_current_address_ = mde_base_address_ + V1495_N_SPILLS;
    int n=0;
    while(nSpils != nSpils1) {
      usleep(10);
      mde_vmeStatus_ = VME_READ_32( mde_current_address_, &nSpils1);
      n++;
      if (n>1000) {
        mde_messanger_->sendMessage(this, "Unable to generate new Spill Gate. Timeout.", MDE_WARNING);
        return 0;
      }
    }

    MESSAGESTREAM << "New Spill Gate generated. The Number of Spills is " << nSpils1;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  return nSpils;
}

bool MDEv1495::softwareClear() {
  int nbRead(0);
  int nbStored(0);
  mde_current_address_ = mde_base_address_ + V1495_MEB;

  while (mde_vmeStatus_ == cvSuccess) {
    mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, mde_dataPtr_, 1024, &nbRead );
    nbStored += nbRead;
  }
  MESSAGESTREAM << "Software Clear completed. "
                << "BLT cycle status: " << getVmeStatusCodeName(mde_vmeStatus_)
                << ". Cleared " << nbStored << " bytes.";
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

bool MDEv1495::setGEO(int geo) {
  //Set GEO address
  mde_current_address_ = mde_base_address_ + V1495_BOARD_ID;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, geo );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Geo Address.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "GEO is set to " << geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setSGOpenDelay(int d) {
  //Set the Spill Gate Open delay
  mde_current_address_ = mde_base_address_ + V1495_SG_OPEN_DELAY;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, d);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Spill Gate Open delay.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Spill Gate Open delay is set to " << d;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setSGCloseDelay(int d) {
  //Set the Spill Gate Close delay
  mde_current_address_ = mde_base_address_ + V1495_SG_CLOSE_DELAY;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, d);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Spill Gate Close delay.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Spill Gate Close delay is set to " << d;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setSggCtrl(int c) {
  //Set the PTG control reg.
  mde_current_address_ = mde_base_address_ + V1495_SGG_CTRL;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, c);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the SGG control reg.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "SGG control reg is set to " << hex << c << dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setTrCtrl(int c) {
  //Set the PTG control reg.
  mde_current_address_ = mde_base_address_ + V1495_PTG_CTRL;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, c);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the PTG control reg.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "PTG control reg is set to " << hex << c << dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setTOF0Mask(int m) {
  //Set the TOF0 mask
  mde_current_address_ = mde_base_address_ + V1495_TOF0_MASK;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, m);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the TOF0 mask.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "TOF0 mask is set to " << hex << m << dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setTOF1Mask(int m) {
  //Set the TOF1 mask
  mde_current_address_ = mde_base_address_ + V1495_TOF1_MASK;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, m);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the TOF1 mask.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "TOF1 mask is set to " << hex << m << dec;;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setTOF2Mask(int m) {
  //Set the TOF2 mask
  mde_current_address_ = mde_base_address_ + V1495_TOF2_MASK;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, m);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the TOF2 mask.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "TOF2 mask is set to " << hex << m << dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv1495::setPTVetoLenght(int vl) {
  //Set the Particle Trigger Veto lenght
  mde_current_address_ = mde_base_address_ + V1495_PT_VETO_LENGHT;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, vl);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Part. Trigger Veto lenght.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Part. Trigger Veto lenght is set to 0x" << hex << vl << dec;;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

unsigned int MDEv1495::getPhysBusy(int i) {
  int mask, shift;
  switch (i) {
    case 0:
      mde_current_address_ = mde_base_address_ + V1495_PHYS_BUSY_01;
      mask = 0x0; shift = 0;
      break;

    case 1:
      mde_current_address_ = mde_base_address_ + V1495_PHYS_BUSY_01;
      mask = 0xFFFF0000; shift = 16;
      break;

    case 2:
      mde_current_address_ = mde_base_address_ + V1495_PHYS_BUSY_23;
      mask = 0x0; shift = 0;
      break;

    case 3:
      mde_current_address_ = mde_base_address_ + V1495_PHYS_BUSY_23;
      mask = 0xFFFF0000; shift = 16;
      break;

    case 4:
      mde_current_address_ = mde_base_address_ + V1495_PHYS_BUSY_45;
      mask = 0x0; shift = 0;
      break;

    case 5:
      mde_current_address_ = mde_base_address_ + V1495_PHYS_BUSY_45;
      mask = 0xFFFF0000; shift = 16;
      break;

    default:
      MESSAGESTREAM << "Busy index " << i << " is out of the range";
      mde_messanger_->sendMessage(MDE_ERROR);
      return 0;
  }

  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &mde_data_32_);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to get the Number of Triggers.", MDE_FATAL);
    return 0;
  }

  unsigned int busy = (*(unsigned long32*)(mde_data_32_) & mask ) >> shift;
  MESSAGESTREAM << "The busy" << i << " length is " << 0.0001*busy << " ms";
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return busy;
}


