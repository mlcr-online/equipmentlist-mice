#include "MDEv1290.hh"
#include "V1290Def.hh"
#include "MiceDAQMessanger.hh"
#include <math.h>

IMPLEMENT_FUNCTIONS(V1290Params, MDEv1290)

MDEv1290::MDEv1290()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V1290Params)
  n_data_words_recorded_ = 0;
  mde_name_ = "V1290";
}

int MDEv1290::getMaxMemUsed(int nEvents) {
  mde_vmeStatus_ = OPCodeRead (V1290_READ_FIFO_SIZE, 1, &mde_data_16_ );
  int memsize = this->getNumDataWords()*4 + 1;
  return memsize;
}

bool MDEv1290::Arm() {

  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  mde_base_address_ = (*this)["BaseAddress"];

  if( (*this)["DoInit"] )
    this->Init();
  else
    mde_messanger_->sendMessage(this, "Module Reset and Reconfiguration skipped", MDE_INFO);


  int control_reg_mask = 0;
  if( (*this)["UseEventFIFO"])
    control_reg_mask |= V1290_ControlEventFifo;
  if( (*this)["UseExtendedTTT"])
    control_reg_mask|= V1290_ControlTriggerTimeTag;

  if ( (mde_vmeStatus_ == cvSuccess) &&
       this->softwareClear() &&
       this->setGEO( (*this)["GEO"] ) &&
       this->channelsEnable((*this)["ChannelMask"]) &&
       this->setControlReg(control_reg_mask) ) {

    MESSAGESTREAM << "Arming successful.";
    mde_messanger_->sendMessage(MDE_INFO);
    return true;
  } else {
    mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
    return false;
  }
}

bool MDEv1290::DisArm() {
  // Checks if there is data
  mde_current_address_ = mde_base_address_ + V1290_STATUS;
  short status;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_, &status);
  if ( mde_vmeStatus_ != cvSuccess ) {
    return false;
  }
  if( ( mde_vmeStatus_ != cvSuccess ) && !!( status & 0x0001 ) )
    mde_messanger_->sendMessage(this, "V1290 still contains data! ", MDE_WARNING);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "DisArming failed. \n", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "DisArming successful. \n", MDE_INFO);
  return true;
}

int MDEv1290::ReadEvent() {
  int dataStored = 0;
  short status;
  int nWordLeft = 0;
  int nbread;
  int npages;
  int ipage;
  mde_base_address_ = (*this)["BaseAddress"];

  //Checks if there is some data ready
  mde_current_address_ = mde_base_address_ + V1290_STATUS;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_, &status);
  if ( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Cannot read V1290 Status Register, Error: " << getVmeStatusCodeName( mde_vmeStatus_);
    mde_messanger_->sendMessage(MDE_ERROR);
    return false;
  }

  // From now on, we stop checking mde_vmeStatus_.
  if( status & (V1290_StatusFull | V1290_StatusTriggerLost) ) {
    MESSAGESTREAM << "TDC is full and/or Trigger has been lost. Status: " << std::hex
                  << getVmeStatusCodeName( mde_vmeStatus_) << std::dec
                  << " \n Clearing the board!";
    mde_messanger_->sendMessage(MDE_ERROR);
    this->softwareClear();
    return false;
  }

  if( status & V1290_StatusDataReady ) {
    // Get the number of data words.
    if (!n_data_words_recorded_)
      this->getNumDataWords();

    nbread = 0;
    int nBLTCycle=0;
    npages =  n_data_words_recorded_/ ( V1290_PAGECAPACITY );

    if ( n_data_words_recorded_ ) {
      for ( ipage = 0 ; ipage < npages ; ipage++ ) {
        mde_vmeStatus_ = VME_READ_BLT_32(mde_base_address_, mde_dataPtr_, V1290_PAGESIZE, &nbread);
        dataStored   += nbread;
        mde_dataPtr_ += nbread/sizeof(MDE_Pointer);
        MESSAGESTREAM << "BLT cycle number " << ++nBLTCycle << ". " << nbread << " bites read. (" << dataStored << ")";;
        mde_messanger_->sendMessage(MDE_DEBUGGING);
      }

      //MESSAGESTREAM << "Number of BLT cycles: " << nBLTCycle << ".  bites stored: " << dataStored
      //mde_messanger_->sendMessage(MDE_WARNING);

      nWordLeft = n_data_words_recorded_ % (V1290_PAGESIZE/V1290_READOUTWORDSIZE);
      if ( nWordLeft ) {
        mde_vmeStatus_ = VME_READ_BLT_32(mde_base_address_, mde_dataPtr_, nWordLeft * V1290_READOUTWORDSIZE, &nbread);
        dataStored += nbread;
        MESSAGESTREAM << "BLT cycle number " << ++nBLTCycle << ". " << nbread << " bites read. (" << dataStored << ")";;
        mde_messanger_->sendMessage(MDE_DEBUGGING);
      }
    }
  }

  mde_current_address_ = mde_base_address_ + V1290_STATUS;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_, &status);

  if(  !(status & V1290_StatusPurged)  )
    mde_messanger_->sendMessage(this, "Board still contains data after readout!", MDE_WARNING );

  MESSAGESTREAM << "Data stored: " << dataStored;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  n_data_words_recorded_ = 0;
  return dataStored;
}

int MDEv1290::EventArrived() {

  return 0;
}

int MDEv1290::getNumDataWords() {

  if( (*this)["UseEventFIFO"] ) {
    //Get the number of events
    mde_current_address_ = mde_base_address_ + V1290_EVENTFIFOSTORED;
    mde_vmeStatus_ = VME_READ_16(mde_current_address_,&mde_data_16_);

    short fifoWordCount = 0;
    short fifoEventCount = 0;
    int nEvt = mde_data_16_;
    MESSAGESTREAM << "ReadEventV1290: Number of Events in FIFO = " << nEvt;
    mde_messanger_->sendMessage(MDE_DEBUGGING);

    mde_current_address_ = mde_base_address_ + V1290_EVENTFIFO;
    int nDataWords = 0;
    // compute the total number of data words.
    for(int current_event = 0; current_event < nEvt; ++current_event ){
      mde_vmeStatus_ = VME_READ_32( mde_current_address_, &mde_data_32_ );
      fifoWordCount = ( short )( mde_data_32_ & 0x0000FFFF );
      fifoEventCount = ( short )( ( mde_data_32_ >> 16 ) & 0x0000FFFF );
      nDataWords += fifoWordCount;
    }

    MESSAGESTREAM << "ReadEventV1290: Number of Words in FIFO = " << nDataWords;
    mde_messanger_->sendMessage(MDE_DEBUGGING);

    n_data_words_recorded_ = nDataWords;
    return n_data_words_recorded_;
  } else {
    mde_messanger_->sendMessage(this, "Unable to get the number of data words recorded. EventFIFO is disabled.", MDE_ERROR);
    return 0;
  }
}

bool MDEv1290::softwareTrigger() {
  mde_data_16_ = 1;
  mde_current_address_ = mde_base_address_ + V1290_SOFTWARETRIGGER;
  mde_vmeStatus_ = VME_WRITE_16(mde_current_address_, mde_data_16_);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to generate software trigger.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software Trigger generated.", MDE_INFO);
  return true;
}

bool MDEv1290::softwareClear() {
  mde_data_16_ = 1;
  mde_current_address_ = mde_base_address_ + V1290_SOFTWARECLEAR;
  mde_vmeStatus_ = VME_WRITE_16(mde_current_address_, mde_data_16_);

  //MESSAGESTREAM << " MDEv1290::softwareClear() address: " << std::hex << mde_current_address_ << std::dec;
  //mde_messanger_->sendMessage(MDE_WARNING);
  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do software clear.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software clear done.", MDE_DEBUGGING);
  return true;
}

int MDEv1290::getTotNTriggers() {
  mde_current_address_ = mde_base_address_ + V1290_EVENTCOUNTER;
  mde_vmeStatus_ = VME_READ_32(mde_current_address_, &mde_data_32_);

  MESSAGESTREAM << "The Number of Triggers is " << mde_data_32_;
  mde_messanger_->sendMessage(MDE_WARNING);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do read the Event Counter Register.", MDE_ERROR);
    return 0;
  }

  return mde_data_32_;
}

int MDEv1290::getNEventsStored() {
  mde_current_address_ = mde_base_address_ + V1290_EVENTSTORED;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_, &mde_data_16_);

  MESSAGESTREAM << "The Number of Events is " << mde_data_32_;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do read the Event Stored Register.", MDE_ERROR);
    return 0;
  }

  return mde_data_16_;
}

int MDEv1290::getStatusReg() {
  mde_current_address_ = mde_base_address_ + V1290_STATUS;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_, &mde_data_16_);

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do read the Status Register.", MDE_ERROR);
    return 0;
  }

  MESSAGESTREAM << "Status Register: " << std::hex << mde_data_16_ << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return mde_data_16_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int MDEv1290::MicroPollingLoop(unsigned long ad, unsigned short mask) {
  int timeOut = MICROCONTROLER_TIMEOUT;
  short data;
  do {
    usleep(10000);
    timeOut--;
    mde_vmeStatus_ = VME_READ_16( ad , &data );
    if (mde_vmeStatus_ != cvSuccess) {
      MESSAGESTREAM << "MicroPollingLoop  - Error " << mde_vmeStatus_;
      mde_messanger_->sendMessage(MDE_WARNING);
      return mde_vmeStatus_;
    }

  } while ( ( !(data & mask)) && timeOut && (mde_vmeStatus_==cvSuccess) );
  if ( timeOut == 0 ) {
    mde_messanger_->sendMessage(this, "In MicroPollingLoop(): Handshaking with Controler timed out", MDE_ERROR);
    return -1;
  }

  return 0;
}

int MDEv1290::OPCodeCycle(unsigned short mask,
                          short OPCode,
                          unsigned int nw,
                          short* microData) {
  unsigned int iw;
  short data;
  mde_vmeStatus_ = VME_READ_16( mde_base_address_ + V1290_MICROHANDSHAKE , &data );
  if (mde_vmeStatus_ != cvSuccess)
    return mde_vmeStatus_;

  // Check for pending operation
  if ( (data & V1290_MicroRWMask) !=  V1290_MicroWrite ) {
    // There is a pending Operation
    MESSAGESTREAM << "Error in OPCodeCycle(): The Micro Controler is busy with a pending IO operation (mask = " << mask << ")";
    mde_messanger_->sendMessage(MDE_ERROR);
    return cvGenericError;
  }

  // Send the OPCode
  mde_current_address_ = mde_base_address_ + V1290_MICRO;
  data = OPCode;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, data );
  if (mde_vmeStatus_ != cvSuccess)
    return mde_vmeStatus_;

  // loop over the words to be written
  for ( iw = 0 ; iw < nw ; iw++ ){
     // poll the handshake Write OK
    //  Don't allow more than timeOut cycles
    if ( MicroPollingLoop( mde_base_address_ + V1290_MICROHANDSHAKE, mask) ) {
      MESSAGESTREAM << "Error in OPCodeCycle(): for word" << iw << "/" << nw;
      mde_messanger_->sendMessage(MDE_ERROR);
      return cvCommError;
    }

    mde_current_address_ = mde_base_address_ + V1290_MICRO;
    switch (mask) {
    case V1290_MicroWrite:
      mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, microData[iw] ); break;
    case V1290_MicroRead:
      mde_vmeStatus_ = VME_READ_16( mde_current_address_, &microData[iw] ); break;
    default:
      mde_messanger_->sendMessage(this, "Error in OPCodeCycle(): Wrong parameter", MDE_ERROR);
      return cvGenericError;
    }
    if (mde_vmeStatus_ != cvSuccess)
     return mde_vmeStatus_;
  }

  // poll the handshake until it's back to normal
  if ( MicroPollingLoop( mde_base_address_ + V1290_MICROHANDSHAKE, V1290_MicroWrite  ) )
    return cvCommError;

  return cvSuccess;
}

int MDEv1290::OPCodeWrite( short OPCode,
                           unsigned int nw,
                           short* microData) {
  unsigned int iw;
  short data;
  mde_vmeStatus_ = VME_READ_16( mde_base_address_ + V1290_MICROHANDSHAKE , &data );
  if (mde_vmeStatus_ != cvSuccess)
    return mde_vmeStatus_;

  // Check for pending operation.
  if ( (data & V1290_MicroRWMask) !=  V1290_MicroWrite ) {
    // There is a pending Operation.
    mde_messanger_->sendMessage(this, "Error in OPCodeWrite(): The Micro Controler is busy with a pending IO operation.",
                                      MDE_ERROR);
    return cvGenericError;
  }

  // Send the OPCode.
  mde_current_address_ = mde_base_address_ + V1290_MICRO;
  data = OPCode;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, data );
  if (mde_vmeStatus_ != cvSuccess)
    return mde_vmeStatus_;

  // Loop over the words to be written.
  for ( iw = 0 ; iw < nw ; iw++ ) {
    // Poll the handshake Write OK.
    // Don't allow more than timeOut cycles.
    if ( MicroPollingLoop( mde_base_address_ + V1290_MICROHANDSHAKE, V1290_MicroWrite) ) {
      MESSAGESTREAM << "Error in OPCodeWrite(): for word" << iw << "/" << nw;
      mde_messanger_->sendMessage(MDE_ERROR);
      return cvCommError;
    }
    mde_current_address_ = mde_base_address_ + V1290_MICRO;
    data = microData[iw];
    mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, data );
    if (mde_vmeStatus_ != cvSuccess)
      return mde_vmeStatus_;
  }

  // Poll the handshake until it's back to normal.
  if ( MicroPollingLoop( mde_base_address_ + V1290_MICROHANDSHAKE, V1290_MicroWrite  ) )
    return cvCommError;

  return cvSuccess;
}

int MDEv1290::OPCodeRead( short OPCode,
                          unsigned int nw,
                          short* microData) {
  unsigned int iw;
  short data;
  mde_vmeStatus_ = VME_READ_16( mde_base_address_ + V1290_MICROHANDSHAKE , &data );
  if (mde_vmeStatus_ != cvSuccess)
    return mde_vmeStatus_;

  // Check for pending operation.
  if ( (data & V1290_MicroRWMask) !=  V1290_MicroWrite ) {
  // There is a pending Operation.
    mde_messanger_->sendMessage(this, "Error in OPCodeWrite(): The Micro Controler is busy with a pending IO operation.",
                                      MDE_ERROR);
    return cvGenericError;
  }
  // Send the OPCode.
  mde_current_address_ = mde_base_address_ + V1290_MICRO;
  data = OPCode;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, data );
  if (mde_vmeStatus_ != cvSuccess)
    return mde_vmeStatus_;

  // Loop over the words to be written.
  for ( iw = 0 ; iw < nw ; iw++ ) {
    // Poll the handshake Read OK.
    // Don't allow more than timeOut cycles.
    if ( MicroPollingLoop( mde_base_address_ + V1290_MICROHANDSHAKE, V1290_MicroRead) ){
      MESSAGESTREAM << "Error in OPCodeRead(): for word" << iw << "/" << nw;
      mde_messanger_->sendMessage(MDE_ERROR);
      return cvCommError;
    }
    mde_current_address_ = mde_base_address_ + V1290_MICRO;
    mde_vmeStatus_ = VME_READ_16( mde_current_address_, &microData[iw] );
    if (mde_vmeStatus_ != cvSuccess) 
      return mde_vmeStatus_;
  }

  // Poll the handshake until it's back to normal.
  if ( MicroPollingLoop( mde_base_address_ + V1290_MICROHANDSHAKE,  V1290_MicroWrite) )
    return cvCommError;

  return cvSuccess;
}


bool MDEv1290::setGEO(int geo) {
  mde_current_address_ = mde_base_address_ + V1290_GEO_REGISTER;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, geo );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Geo Address.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "GEO set to " << geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1290::channelsEnable(int mask) {
  mde_vmeStatus_ = cvSuccess;
  int all_en = 0xffffffff;
  int all_dis = 0;

  if (mask == all_en) {
    mde_vmeStatus_ = OPCodeCycle(V1290_MicroWrite, V1290_EN_ALL_CH, 0, NULL );
    mde_messanger_->sendMessage(this, "All Channels enabled.", MDE_DEBUGGING);
  } else if (mask == all_dis) {
    mde_vmeStatus_ = OPCodeCycle(V1290_MicroWrite, V1290_DIS_ALL_CH, 0, NULL );
    mde_messanger_->sendMessage(this, "All Channels disabled.", MDE_DEBUGGING);
  } else {
    for (unsigned int ch=0; ch<32;ch++) {
      if (mask & (1<<ch)) {
        mde_data_16_ = V1290_EN_CHANNEL + ch;
        mde_vmeStatus_ = OPCodeCycle(V1290_MicroWrite, mde_data_16_, 0, NULL );
        MESSAGESTREAM << "Channel " << ch << " is enable.";
        mde_messanger_->sendMessage(MDE_DEBUGGING);
      } else {
        mde_data_16_ = V1290_DIS_CHANNEL + ch;
        mde_vmeStatus_ = OPCodeCycle(V1290_MicroWrite, mde_data_16_, 0, NULL );
        MESSAGESTREAM << "Channel " << ch << " is disable.";
        mde_messanger_->sendMessage(MDE_DEBUGGING);
      }
    }
  }

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to apply Enable channel mask.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Enable channel mask set to " << std::hex << mask << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1290::setControlReg(int cr) {

  mde_current_address_ = mde_base_address_ + V1290_CONTROL;//0x102E
  mde_vmeStatus_ = VME_READ_16(mde_current_address_, &mde_data_16_);

  if (mde_vmeStatus_ == cvSuccess) {
    MESSAGESTREAM << "Control register: " << std::hex << cr << std::dec;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
    mde_data_16_ |= cr;
    mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );
  }

  if (mde_vmeStatus_ != cvSuccess) {
    mde_messanger_->sendMessage(this, "Unable to reset Control register.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Control register after setting: " << std::hex << cr << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MDEv1290::Init() {
  int newConfig = 0;
  short triggerConf[5];

  // Module Reset
  mde_current_address_ = mde_base_address_ + V1290_MODULERESET;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );
  sleep( 1 );
  if (mde_vmeStatus_ == cvSuccess )
    mde_messanger_->sendMessage(this, "ModuleReset done.", MDE_DEBUGGING);

  // Check Acq Mode
  if( mde_vmeStatus_ == cvSuccess ) {
    if ( OPCodeCycle(V1290_MicroRead, V1290_READ_ACQ_MODE, 1, triggerConf ) == cvSuccess ) {
      mde_data_16_ = (*this)["TriggerMode"];
      if ( mde_data_16_ != (triggerConf[0] & 0x1) ) {
        MESSAGESTREAM << "Changing Trigger mode to " << mde_data_16_ <<" (was " << (triggerConf[0] & 0x1) <<")";
        mde_messanger_->sendMessage(MDE_DEBUGGING);
        switch (mde_data_16_) {
          case 0 :
            mde_vmeStatus_ = OPCodeCycle(V1290_MicroWrite, V1290_CONT_STOR, 0, NULL );
            break;
          case 1 :
            mde_vmeStatus_ = OPCodeCycle(V1290_MicroWrite, V1290_TRG_MATCH, 0, NULL );
            break;
          default :
            return false;
            break;
        }
        newConfig = 1;
      }
    }
  }

  // Set leading and trailing edge measurement.
  if( mde_vmeStatus_ == cvSuccess ) {
    mde_data_16_ = (*this)["DetectionEdge"];

    mde_vmeStatus_ = OPCodeWrite(V1290_SET_DETECTION,  1, &mde_data_16_ );
    mde_vmeStatus_ = OPCodeRead (V1290_READ_DETECTION, 1, &mde_data_16_ );
    MESSAGESTREAM << "DetectionEdge set to " << mde_data_16_;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  //Set TDC Header
  mde_data_16_ = (*this)["TDCHeader"];
  if( ( mde_vmeStatus_ == cvSuccess ) && ( mde_data_16_ > -1 ) ) {

    mde_data_16_ =  ( mde_data_16_ ) ? V1290_EN_HEAD : V1290_DIS_HEAD; // 0x3000 ? 0x3100
    mde_vmeStatus_ = OPCodeWrite(mde_data_16_, 0, NULL );
    mde_vmeStatus_ = OPCodeRead (V1290_READ_HEAD, 1, &mde_data_16_ );
    MESSAGESTREAM << "TDCHeader set to " << mde_data_16_;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  //Set LSB code
  if( mde_vmeStatus_ == cvSuccess ) {
    mde_data_16_ = (*this)["LSBCode"];

    mde_vmeStatus_ = OPCodeWrite(V1290_SET_TR_LEAD_LSB, 1, &mde_data_16_);
    mde_vmeStatus_ = OPCodeRead(V1290_READ_RES, 1, &mde_data_16_ );
    MESSAGESTREAM << "LSBCode set to " << mde_data_16_;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  //Set Deadtime code
  if( mde_vmeStatus_ == cvSuccess ) {
    mde_data_16_ = (*this)["DeadtimeCode"];

    mde_vmeStatus_ = OPCodeWrite(V1290_SET_DEAD_TIME, 1, &mde_data_16_ );
    mde_vmeStatus_ = OPCodeRead(V1290_READ_DEAD_TIME, 1, &mde_data_16_ );
    MESSAGESTREAM << "DeadtimeCode set to " << mde_data_16_;
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

  // Check trigger configuration
  if( mde_vmeStatus_ == cvSuccess ) {
    mde_vmeStatus_ = OPCodeRead(V1290_READ_TRG_CONF, 5, triggerConf );

    //Set window width if the de value is different.
    mde_data_16_ = (*this)["WinWidth"];
    if ( (mde_vmeStatus_ == cvSuccess) && (triggerConf[0] != mde_data_16_) ) {
      MESSAGESTREAM << "Changing Window Width to " << mde_data_16_ << " (was " << triggerConf[0] << ")";
      mde_messanger_->sendMessage(MDE_DEBUGGING);
      mde_vmeStatus_ = OPCodeWrite(V1290_SET_WIN_WIDTH , 1, &mde_data_16_);
      newConfig = 1;
    }

    //Set Window Offset if the de value is different.
    mde_data_16_ = (*this)["WinOffset"];
    if ( (mde_vmeStatus_ == cvSuccess) && (triggerConf[1] != mde_data_16_) ) {
      MESSAGESTREAM << "Changing Window Offset to " << mde_data_16_ << " (was " << triggerConf[1] << ")";
      mde_messanger_->sendMessage(MDE_DEBUGGING);
      mde_vmeStatus_ = OPCodeWrite(V1290_SET_WIN_OFFS , 1, &mde_data_16_ );
      newConfig = 1;
    }

    //SwMargin if the de value is different.
    mde_data_16_ = (*this)["SwMargin"];
    if ( (mde_vmeStatus_ == cvSuccess) && (triggerConf[2] != mde_data_16_) ) {
      MESSAGESTREAM << "Changing Sw Margin to " << mde_data_16_ << " (was " << triggerConf[2] << ")";
      mde_messanger_->sendMessage(MDE_DEBUGGING);
      mde_vmeStatus_ = OPCodeWrite(V1290_SET_SW_MARGIN , 1, &mde_data_16_ );
      newConfig = 1;
    }

    //RejMargin if the de value is different.
    mde_data_16_ = (*this)["RejMargin"];
    if ( (mde_vmeStatus_ == cvSuccess) && (triggerConf[3] != mde_data_16_) ) {
      MESSAGESTREAM << "Changing Rejection Margin to " << mde_data_16_ << " (was " << triggerConf[2] << ")";
      mde_messanger_->sendMessage(MDE_DEBUGGING);
      mde_vmeStatus_ = OPCodeWrite(V1290_SET_REJ_MARGIN , 1, &mde_data_16_ );
      newConfig = 1;
    }

    // Enable or disable Trigger time subtraction if the de value is different.
    mde_data_16_ = (*this)["TTSubtraction"];
    if ( ( mde_vmeStatus_ == cvSuccess ) && 
         ( mde_data_16_ > -1 ) &&
         ( (triggerConf[4] & 0x1) != mde_data_16_ ) ) {
      MESSAGESTREAM << "Changing Trigger time subtraction to " << mde_data_16_ << " (was " << triggerConf[3] << ")";
      mde_messanger_->sendMessage(MDE_DEBUGGING);
      mde_data_16_ =  ( mde_data_16_ ) ? V1290_EN_SUB_TRG : V1290_DIS_SUB_TRG  ;
      mde_vmeStatus_ = OPCodeWrite(mde_data_16_, 0, NULL );
      newConfig = 1;
    }

    //Read again the Trigger Config.
    if ( mde_vmeStatus_ == cvSuccess ) {
      mde_vmeStatus_ = OPCodeRead(V1290_READ_TRG_CONF, 5, triggerConf );
      MESSAGESTREAM << "Trigger Configuration is the following" << std::endl
                    << "  Window Width: " << triggerConf[0] << std::endl
                    << "  Window Offset: " << triggerConf[1] << std::endl
                    << "  Sw Margin: " << triggerConf[2] << std::endl
                    << "  Rej Margin: " << triggerConf[3] << std::endl
                    << "  Tr.Ttime Subtraction: " << triggerConf[4] << std::endl;
      mde_messanger_->sendMessage(MDE_DEBUGGING);
    }

    if ( mde_vmeStatus_ == cvSuccess ) {
      if (newConfig) {
        mde_messanger_->sendMessage(this,"Configuration has been changed.", MDE_INFO);

        // save user config
        mde_vmeStatus_ = OPCodeWrite(V1290_SAVE_USER_CONFIG , 0, NULL );
      } else { // newConfig
        mde_messanger_->sendMessage(this,"The TDC configuration has not changed. Consider skipping it.", MDE_INFO);
      }
    }
  }

  if (mde_vmeStatus_ != cvSuccess) {
    mde_messanger_->sendMessage(this, "Unable to Init the board.", MDE_FATAL);
    return false;
  }

  return true;
}





