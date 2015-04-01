#include <climits>
#include <limits.h>

#include "MDEv1724.hh"
#include "V1724Def.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(V1724Params, MDEv1724)

MDEv1724::MDEv1724()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V1724Params)
  mde_name_ = "V1724";
}

int MDEv1724::getMaxMemUsed(int nEvents) {
  int memPerEvent = 16 + 64*(*this)["WordsPerEvent"];

  return memPerEvent*nEvents+1;
}

int MDEv1724::getMaxMemUsedBLT(int nEvents) {
  int memUsed = this->getMaxMemUsed(nEvents);
  return (int(memUsed/4096) + 1) * 4096 + 1;
}

bool MDEv1724::Arm() {

  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  mde_data_32_=0;
  mde_base_address_ = (*this)["BaseAddress"];
  length_           = (*this)["WordsPerEvent"];
  ZSThreshold_      = (*this)["ZSThreshold"];

  int chConfigMask = V1724_MemorySequentialAccess;
  if ( (*this)["TriggerOverlapping"] )
    chConfigMask |= V1724_TriggerOverlapping;

  int triggerSource = 0;
  if ( (*this)["UseSoftwareTrigger"] )
    triggerSource |= V1724_TriggerSourceSoftware;
  if ((*this)["UseExternalTrigger"] )
    triggerSource |= V1724_TriggerSourceExternal;

  if ( this->boardReset() &&
       this->enableEEinBLT() &&
       this->setNEventsForBLT( (*this)["BlockTransfEventNum"] ) &&
       this->setAcqControlReg( V1724_AcquisitonRun | V1724_CountAllTriggers ) &&
       this->setGEO((*this)["GEO"]) &&
       this->channelsEnable((*this)["ChannelMask"] ) &&
       this->setPostTriggerOffset( (*this)["PostTriggerOffset"] ) &&
       this->setNOutBuffers((*this)["BufferOrganizationCode"]) &&
       //this->setNOutBuffers(V1724_OutputBufferSize_1K) &&
       this->setWordsPerEvent((*this)["WordsPerEvent"]) &&
       this->setChConfigReg( chConfigMask ) &&
       this->setTriggerSource( triggerSource ) &&
       this->setTriggerIOLevel((*this)["TriggerIOLevel"]) ) {

    mde_messanger_->sendMessage(this,"Arming successful. \n", MDE_INFO);
    return true;
  } else {
    mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
    return false;
  }
}

bool MDEv1724::DisArm() {

  mde_current_address_ = mde_base_address_ + V1724_ACQ_CONTROL;
  mde_data_32_ = V1724_AcquisitonRun;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_);

  if (mde_vmeStatus_ == cvSuccess) {
    mde_messanger_->sendMessage(this, "DisArming successful. \n", MDE_INFO);
    return true;
  } else {
    mde_messanger_->sendMessage(this, "Unable to DisArm. \n", MDE_FATAL);
    return false;
  }
}

int MDEv1724::ReadEvent() {
  int nbStored   = 0;
  if ( (*this)["ZSThreshold"] > 0)
    nbStored += ReadEventZS();

  else
    nbStored += ReadEventNoZS();

  return nbStored;
}

int MDEv1724::ReadEventNoZS() {

  int nbStored   = 0;
  mde_data_32_ = 0;
  mde_base_address_ = (*this)["BaseAddress"];

  mde_current_address_ = mde_base_address_ + V1724_ACQ_STATUS;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &mde_data_32_ );
  if ( mde_data_32_ & V1724_EventFull ) {
    mde_messanger_->sendMessage(this, "The max. number of events is exceeded.", MDE_ERROR);
  }
  if ( mde_data_32_ & V1724_EventReady ) {
    // Get the number of events.
    int nEvt =  getNEventsStored();
    //MESSAGESTREAM << "number of events to read: "<< nEvt;
    //mde_messanger_->sendMessage(MDE_DEBUGGING);

    nbStored += doBLTRead(mde_dataPtr_, nEvt);
  }

  //Clear the board memory buffer.
  this->softwareClear();

  MESSAGESTREAM << "Data stored: " << nbStored;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return nbStored;
}


int MDEv1724::ReadEventZS() {

  int nbStored = 0;
  int nbRead   = 0;
  mde_base_address_ = (*this)["BaseAddress"];

  // Allocate the necessary amount of memory.
  int nEvt =  getNEventsStored();
  //MESSAGESTREAM << "number of events to read: "<< nEvt;
  //mde_messanger_->sendMessage(MDE_INFO);
  int memSizeFadc = this->getMaxMemUsed(nEvt);
  MDE_Pointer *dataPtr_tmp = new MDE_Pointer[memSizeFadc/sizeof(MDE_Pointer)];

  mde_current_address_ = mde_base_address_ + V1724_ACQ_STATUS;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &mde_data_32_ );
  if ( mde_data_32_ & V1724_EventFull ) {
    mde_messanger_->sendMessage(this, "The max. number of events is exceeded.", MDE_ERROR);
  }
  if ( mde_data_32_ & V1724_EventReady ) {
    this->doBLTRead(dataPtr_tmp, nEvt);

    //Clear the board memory buffer.
    this->softwareClear();

    int evSize = (*dataPtr_tmp) & 0xFFFFFFF;

    // Get a copy of the data pointer. Use only this copy below.
    MDE_Pointer *dataPtr = this->getDataPtr();
    for (int ev=0; ev<nEvt; ev++) {

      nbRead = ZS_processEvent(dataPtr, dataPtr_tmp + ev*evSize);
      dataPtr += nbRead/sizeof(MDE_Pointer);
      nbStored += nbRead;
    }
  }

  // Free the allocated memory.
  delete[] dataPtr_tmp;

  MESSAGESTREAM << "Data stored after ZS: " << nbStored;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return nbStored;
}
int MDEv1724::EventArrived() {

  return 0;
}


bool MDEv1724::softwareTrigger() {

  mde_data_32_ = 1;
  mde_current_address_ = mde_base_address_ + V1724_SW_TRIGGER;
  mde_vmeStatus_ |= VME_WRITE_32(mde_current_address_, mde_data_32_);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to generate software trigger.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software Trigger generated.", MDE_INFO);
  return true;
}

bool MDEv1724::softwareClear() {
  mde_data_32_ = 1;
  mde_current_address_ = mde_base_address_ + V1724_SW_CLEAR;
  mde_vmeStatus_ |= VME_WRITE_32(mde_current_address_, mde_data_32_);

  //MESSAGESTREAM << " MDEv1724::softwareClear() address: " << std::hex << mde_current_address_ << std::dec;
  //mde_messanger_->sendMessage(MDE_WARNING);
  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do software clear.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software clear done.", MDE_DEBUGGING);
  return true;
}

int MDEv1724::getNEventsStored() {

  mde_current_address_ = mde_base_address_ + V1724_EVENT_STORED;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

  MESSAGESTREAM << "The Number of Events is " << mde_data_32_;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do read the Event Stored Register.", MDE_ERROR);
    return 0;
  }

  return mde_data_32_;
}


int MDEv1724::doBLTRead(MDE_Pointer *dataPtr, int nEvt) {

  int nbStored = 0;
  int nbRead = 0;
  int nBLTCycle=0;
  int iloop;

  mde_current_address_ = mde_base_address_ + V1724_EVENT_READOUT_BUFFER;
  for ( iloop = 0 ; iloop <= nEvt/(*this)["BlockTransfEventNum"] ; iloop++) {
    while(  mde_vmeStatus_ == cvSuccess ){
      nBLTCycle++;
      mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, dataPtr, V1724_READOUT_BUFFER_SIZE, &nbRead );
      nbStored += nbRead;
      dataPtr += nbRead/sizeof(MDE_Pointer); //Move the data pointer by the number of words read.
      MESSAGESTREAM << "BLT cycle number " << nBLTCycle << ". " << nbRead << " bites read. (" << nbStored << ")  ";
      //              << mde_vmeStatus_ << "   " << iloop;
      mde_messanger_->sendMessage(MDE_DEBUGGING);
    }

    //MESSAGESTREAM << "Number of BLT cycles: " << nBLTCycle << ".  bites stored: " << nbStored << std::endl
    //              << "The VME status code is: " <<  getVmeStatusCodeName( mde_vmeStatus_ );
    //mde_messanger_->sendMessage(MDE_WARNING);

    if (mde_vmeStatus_ < cvBusError) {
      // This is a serious error. Stop reading.
      MESSAGESTREAM << "Error in the BLT cycle. The VME error code is: " <<  getVmeStatusCodeName( mde_vmeStatus_ );
      mde_messanger_->sendMessage(MDE_WARNING);
      return nbStored;
    }
    mde_vmeStatus_ = cvSuccess;
  }

  return nbStored;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::boardReset() {
  //Reset the board
  mde_current_address_ = mde_base_address_ + V1724_SW_RESET;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "The board failed to reset.", MDE_FATAL);
    return false;
  }
  // Poll ACQ Status Register until the board is ready
  mde_data_32_=0;
  mde_current_address_ = mde_base_address_ +  V1724_ACQ_STATUS;
  int ploop=0;
  while ( (mde_vmeStatus_ == cvSuccess) && !(mde_data_32_ &  V1724_BoardReady) ) {
    mde_messanger_->sendMessage(this, "Waiting for the board to be ready after reset...", MDE_DEBUGGING);
    usleep(200000);
    mde_vmeStatus_ = VME_READ_32( mde_current_address_, &mde_data_32_ );
    ploop++;
    if (ploop > 10) {
      mde_messanger_->sendMessage(this, "The board failed to reset. Timeout error.", MDE_FATAL);
      mde_vmeStatus_= cvGenericError;
      return false;
    }
  }

  mde_messanger_->sendMessage(this, "Reset done.", MDE_DEBUGGING);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::enableEEinBLT() {
  // Get the  VME control register.
  int VME_control_reg;
  this->getVMEControlReg(VME_control_reg);

  if (mde_vmeStatus_ == cvSuccess) {
    MESSAGESTREAM << "VME Control = " << std::hex << VME_control_reg << std::dec;
    mde_messanger_->sendMessage(MDE_DEBUGGING);

    // Enable bus error emission during BLT cycle by reseting the VME control register.
    VME_control_reg |= V1724_BERR;
    this->setVMEControlReg(VME_control_reg);
  }

  if (mde_vmeStatus_ == cvSuccess) {
    // Check the result.
    this->getVMEControlReg(VME_control_reg);
  }

  if (mde_vmeStatus_ != cvSuccess) {
    mde_messanger_->sendMessage(this, "Unable to reset VME Control register.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "VME Control after setting = " << std::hex << VME_control_reg
  << " (expected " << V1724_BERR << ")" << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setVMEControlReg(long32 cr) {
  //VME control
  mde_current_address_ = mde_base_address_ + V1724_VME_CONTROL;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, cr );

  if( mde_vmeStatus_ != cvSuccess )
    return false;

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::getVMEControlReg(long32 &cr) {
  //VME control
  mde_current_address_ = mde_base_address_ + V1724_VME_CONTROL;
  mde_vmeStatus_ = VME_READ_32( mde_current_address_, &cr );

  if( mde_vmeStatus_ != cvSuccess )
    return false;

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setGEO(int geo) {
  //Set GEO address
  mde_current_address_ = mde_base_address_ + V1724_BOARD_ID;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, geo );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Geo Address.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "GEO set to " << geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::channelsEnable(int mask) {
  mde_current_address_ = mde_base_address_ + V1724_CHANNEL_ENABLE;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mask );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to apply channel mask.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Channel mask set to " << std::hex << mask << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setPostTriggerOffset(int offset) {
  //Set the post trigger offset by [mde_data_32_] samples
  mde_current_address_ = mde_base_address_ + V1724_POST_TRIGGER_SETTING;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, offset );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Post Trigger Offset.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Post Trigger Offset is set to " << offset;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setNOutBuffers(int mask) {
  //Set the number of Output Buffers. This will set also the size of the buffers.
  mde_current_address_ = mde_base_address_ + V1724_BUFFER_ORGANIZATION;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mask );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to configure the Output Buffers", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Number of Output Buffers is set using mask " << mask;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setWordsPerEvent(int nWords) {
  //Set custom number of sample words per event
  mde_current_address_ = mde_base_address_ + V1724_BUFFER_CUSTOM_SIZE;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, nWords );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Number of Data Words per event", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Number of Data Words per event is set to " << nWords << " (" << nWords*2 << " samples)";
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setNEventsForBLT(int nEvents) {
  mde_current_address_ = mde_base_address_ + V1724_BLT_EVENT_NUMBER;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, nEvents );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the number of events for Block Transfer",
                                      MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Number of events for Block Transfer is set to " << nEvents;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setTriggerSource(int trigger) {
  mde_current_address_ = mde_base_address_ + V1724_TRIGGER_SOURCE_ENABLE_MASK;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, trigger );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Trigger Source.", MDE_FATAL);
    return false;
  }

  std::string  trSource;
  switch (trigger) {
    case V1724_TriggerSourceExternal:
      trSource = "External";
      break;
    case V1724_TriggerSourceSoftware:
      trSource = "Software";
      break;
    case V1724_TriggerSourceChannel_0:
      trSource = "Channel 0";
      break;
    case V1724_TriggerSourceChannel_1:
      trSource = "Channel 1";
      break;
    case V1724_TriggerSourceChannel_2:
      trSource = "Channel 2";
      break;
    case V1724_TriggerSourceChannel_3:
      trSource = "Channel 3";
      break;
    case V1724_TriggerSourceChannel_4:
      trSource = "Channel 4";
      break;
    case V1724_TriggerSourceChannel_5:
      trSource = "Channel 5";
      break;
    case V1724_TriggerSourceChannel_6:
      trSource = "Channel 6";
      break;
    case V1724_TriggerSourceChannel_7:
      trSource = "Channel 7";
      break;
  }

  mde_messanger_->sendMessage(this, "Trigger Source set to "+trSource+".", MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setTriggerIOLevel(int il) {
  mde_current_address_ = mde_base_address_ + V1724_IO_CONTROL;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);
  if (mde_vmeStatus_ == cvSuccess) {
    MESSAGESTREAM << "I/O Config Reg. is " << std::hex << mde_data_32_ << std::dec;
    mde_messanger_->sendMessage(MDE_DEBUGGING);

    // Enable bus error emission during BLT cycle by reseting the VME control register.
    mde_data_32_ &= ~1;
    mde_data_32_ |= il;
    if( this->setIOConfigReg(mde_data_32_) )
      return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setIOConfigReg(int mask) {
  mde_current_address_ = mde_base_address_ + V1724_IO_CONTROL;
  mde_vmeStatus_ = VME_WRITE_32(mde_current_address_, mask);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the I/O Config Reg.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "I/O Config Reg. is set to " << std::hex << mask << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setChConfigReg(int mask) {
  // Set channels configuration.
  if ( (*this)["TriggerOverlapping"] ) {
    MESSAGESTREAM << "TriggerOverlapping: 1";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  } else {
    MESSAGESTREAM << "TriggerOverlapping: 0";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  // First clear the register.
  mde_current_address_ = mde_base_address_ + V1724_CHANNEL_CONFIG_BIT_CLEAR;//check this
  mde_data_32_ = 0xFF;
  mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mde_data_32_ );

  if( mde_vmeStatus_ == cvSuccess ) {
    mde_current_address_ = mde_base_address_ + V1724_CHANNEL_CONFIG;
    mde_vmeStatus_ |= VME_READ_32( mde_current_address_, &mde_data_32_ );
  }

  if( mde_vmeStatus_ == cvSuccess ) {
    MESSAGESTREAM << "Channel Config Register: " << mde_data_32_;
    mde_messanger_->sendMessage(MDE_DEBUGGING);

    mde_current_address_ = mde_base_address_ + V1724_CHANNEL_CONFIG_BIT_SET;
    mde_vmeStatus_ = VME_WRITE_32( mde_current_address_, mask );
  }

  if( mde_vmeStatus_ == cvSuccess ) {
    mde_current_address_ = mde_base_address_ + V1724_CHANNEL_CONFIG;
    mde_vmeStatus_ |= VME_READ_32( mde_current_address_, &mde_data_32_ );
  }


  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Channel Config Register.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Channel Config Register after setting : " << mde_data_32_;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::setAcqControlReg(int mask) {
  // Set the acquisition control register.
  mde_current_address_ = mde_base_address_ + V1724_ACQ_CONTROL;
  mde_vmeStatus_ |= VME_READ_32( mde_current_address_, &mde_data_32_ );
  MESSAGESTREAM << "ACQ Control = " << std::hex << mde_data_32_ << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  if( mde_vmeStatus_ == cvSuccess )
    mde_vmeStatus_ |= VME_WRITE_32( mde_current_address_, mask );

  if( mde_vmeStatus_ == cvSuccess ) {
    mde_vmeStatus_ |= VME_READ_32( mde_current_address_, &mde_data_32_ );
    MESSAGESTREAM << "ACQ Control after setting = " << std::hex << mde_data_32_ << std::dec;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  if( mde_vmeStatus_ == cvSuccess ) {
    //Acquisition status
    mde_current_address_ = mde_base_address_ + V1724_ACQ_STATUS;
    mde_vmeStatus_ |= VME_READ_32( mde_current_address_, &mde_data_32_ );
    MESSAGESTREAM << "ACQ Status is : " << std::hex << mde_data_32_ << " (expected "
    << std::hex << (V1724_BoardReady | V1724_RunOn | V1724_PLLLock) << ")" << std::dec;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the ACQ Control register", MDE_FATAL);
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int MDEv1724::ZS_processEvent(MDE_Pointer *dataPtr, MDE_Pointer *ptr_tmp) {

  int nbStored = 0;

  // Copy the header.
  dataPtr[0] = ptr_tmp[0];
  // Modify  the data encoding mode to MICE Zero Supression.
  dataPtr[1] = ptr_tmp[1] | V1724_USER_ZS;
  dataPtr[2] = ptr_tmp[2];
  dataPtr[3] = ptr_tmp[3];

  dataPtr  += 4;
  nbStored += 4*4;
  for (int ch=0;ch<8;ch++) {
    MDE_Pointer *chPtr = ptr_tmp + 4 + ch*length_;
    // The first word after the header will contain the channel number and 
    // the number of words recorded. In case the channel is ZERO suppressed 
    // the number of words will be 0.
    *dataPtr = 0 | (ch << 12);
    nbStored += 4;
    if ( ZS_scanChannel(chPtr) ) {
      // We have a signal in this channel.
      *dataPtr |= length_;
      dataPtr++;
      // Copy the mesurements.
      for(int w=0;w<length_;w++) {
        *dataPtr = chPtr[w];
        dataPtr++;
        nbStored += 4;
      }

    } else  dataPtr++;
  }

  // Modify the first word of the header and change the event size.
  int nWords = nbStored/4;
  dataPtr -= nWords;
  *dataPtr &= 0xf0000000; // erase the old event size.
  *dataPtr |= nWords;

  MESSAGESTREAM << nWords << " words after ZS  (" << nbStored << "b)";
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return nbStored;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1724::ZS_scanChannel(MDE_Pointer *dataPtr) {

  int xMax, xMin, d;
  xMin = INT_MAX;
  xMax = INT_MIN;
  for(int i=0;i<length_;i++) {
    d = (*dataPtr & 0xFFFF0000) >> 16;
    xMin = (d < xMin)? d : xMin;
    xMax = (d > xMax)? d : xMax;

    d = *dataPtr & 0xFFFF;
    xMin = (d < xMin)? d : xMin;
    xMax = (d > xMax)? d : xMax;

    dataPtr++;
  }

  if (xMax-xMin < ZSThreshold_)
    return false;

  return true;
}
















