#include "MDEv977.hh"
#include "V977Def.hh"
#include "MiceDAQMessanger.hh"
#include <iomanip>

IMPLEMENT_FUNCTIONS(V977Params, MDEv977)

MDEv977::MDEv977()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V977Params)
  mde_name_ = "V977";
}

bool MDEv977::ArmTriggerReceiver() {
  short ctl_reg;
  mde_messanger_->sendMessage(this, "Going to arm Trigger Receiver. Board parameters:", MDE_INFO);
  this->getInfo();

  mde_base_address_ = (*this)["BaseAddress"];

//  triggerReceiverBoard = TR_V977;

 if ( this->softwareReset() &&  // SOFTWARE RESET
      this->setInputMask(TR_EOS_EVENT_REQUEST | TR_PHYSICS_EVENT_REQUEST ) && // Mask the EOS and the Physics event trigger input.
      this->clearOutputs() && // Clear all outputs (except those set by software)
      this->setOutput(TR_TRIGGER_BUSY) ) {  // Set all the busy outputs.

    // Set the first call flag.
    first_call_ = true;

    mde_current_address_ = mde_base_address_ + V977_CONTROL_REGISTER;
    mde_vmeStatus_ = VME_READ_16(mde_current_address_,&ctl_reg);
  }

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Arming Trigger Receiver failed. \n", MDE_FATAL);
    return false;
  }

  sleep(3);

  MESSAGESTREAM << "Arming successful. Control Reg: " << std::hex << ctl_reg << std::dec << std::endl;
  mde_messanger_->sendMessage(MDE_INFO);
  return true;
}

bool MDEv977::DisArmTriggerReceiver() {
  // The TriggerReceiver is the last to be disarmed
  // Free all the busy outputs.
  this->unsetOutput(TR_TRIGGER_MASK);
  mde_messanger_->sendMessage(this, "DisArming Trigger Receiver successful. \n", MDE_INFO);
  return true;
}

int MDEv977::ReadEventTriggerReceiver() {
  // This ReadEvent is always called at the beginning of the equipment List
  int dataStored = 0;
  int inputData = 0;

    static unsigned int spillNb = 0;
    static int nbInSpill = 0;


  // Read the inputs and Flip flop Q status and send them to data stream
  mde_current_address_ = mde_base_address_ + V977_INPUT_READ;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_,&inputData);
  mde_current_address_ = mde_base_address_ + V977_SINGLEHIT_READ;
  mde_vmeStatus_ |= VME_READ_16(mde_current_address_,mde_dataPtr_);
  //MESSAGESTREAM << "RETR-> inp: " << inputData << "  ff: " <<*mde_dataPtr_;
  //mde_messanger_->sendMessage(MDE_INFO);
  *mde_dataPtr_ = (*mde_dataPtr_ & 0x0000FFFF) | (inputData << 16);


  dataStored = sizeof(*mde_dataPtr_);

  if ( inputData & TR_SOS_EVENT_BUSY ) {
    // Acknowledge: mask the output of the request
    // that will reset the input of the busy
    this->addOutputMask(TR_SOS_EVENT_REQUEST);
    mde_messanger_->sendMessage(this, "SOS Trigger Received.", MDE_DEBUGGING);
    spillNb++;
    nbInSpill = 0 ;
    this->setEventType( START_OF_BURST );
    return dataStored;
  }

  if ( inputData & TR_PHYSICS_EVENT_BUSY ) {
    // Acknowledge: mask the output of the request
    this->addOutputMask(TR_PHYSICS_EVENT_REQUEST);
    mde_messanger_->sendMessage(this, "PHYSICS Trigger Received.", MDE_DEBUGGING);
    nbInSpill++ ;
    this->setEventType( PHYSICS_EVENT );
    return dataStored;
  }

  if ( inputData  & TR_EOS_EVENT_BUSY ) {
    // Acknowledge: mask the output of the request
    this->addOutputMask(TR_EOS_EVENT_REQUEST);
    mde_messanger_->sendMessage(this, "EOS Trigger Received.", MDE_DEBUGGING);
    this->setEventType( END_OF_BURST );
    return dataStored;
  }

  if ( inputData & TR_CALIBRATION_EVENT_BUSY ) {
    // Acknowledge: mask the output of the request
    this->addOutputMask(TR_CALIBRATION_EVENT_REQUEST);
    mde_messanger_->sendMessage(this, "CALIBRATION Trigger Received.", MDE_DEBUGGING);
    nbInSpill++ ;
    this->setEventType( CALIBRATION_EVENT );
    return dataStored;
  }

  mde_messanger_->sendMessage(this, "Invalid event trigger ", MDE_ERROR);
  return dataStored;
}

int MDEv977::EventArrivedTriggerReceiver() {
  unsigned short eventType;

  if ( first_call_) {
    // Reset the busy, keeping the other output set.
    this->unsetOutput(TR_TRIGGER_MASK);
    first_call_ = false;
    return 0;
  }

  // Get the Input register.
  mde_current_address_ = mde_base_address_ + V977_INPUT_READ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &eventType);

  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Error: Cannot read V977_INPUT_READ register", MDE_FATAL);
    return 0;
  }

  if ( eventType & TR_SOS_EVENT_BUSY ) {
    mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
    mde_vmeStatus_ |= VME_READ_16( mde_current_address_, &mde_data_16_ );
    // mask inputs for CALIBRATION and SOS events
    mde_data_16_ |= TR_CALIBRATION_EVENT_REQUEST |
                    TR_SOS_EVENT_REQUEST |
                    TR_EOS_EVENT_REQUEST;
    // unmask inputs for PHYSICS
    mde_data_16_ &= ~(TR_PHYSICS_EVENT_REQUEST );
    mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );
    //gettimeofday(&timetag0,NULL);
    if ( mde_vmeStatus_ == cvSuccess )
      return 1;
  }

  if ( eventType & TR_PHYSICS_EVENT_BUSY ) {
    mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
    mde_vmeStatus_ |= VME_READ_16( mde_current_address_, &mde_data_16_ );
    // mask inputs for CALIBRATION, PHYSICS and SOS events
    mde_data_16_ |= TR_CALIBRATION_EVENT_REQUEST |
                    TR_SOS_EVENT_REQUEST | 
                    TR_PHYSICS_EVENT_REQUEST;
    // unmask inputs for EOS
    mde_data_16_ &= ~(TR_EOS_EVENT_REQUEST);
    mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );
    //gettimeofday(&timetag0,NULL);
    if ( mde_vmeStatus_ == cvSuccess )
      return 1;
  }

  if ( eventType & TR_EOS_EVENT_BUSY ) {
    mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
    mde_vmeStatus_ |= VME_READ_16( mde_current_address_, &mde_data_16_ );
    // mask inputs for CALIBRATION, PHYSICS and EOS events
    mde_data_16_ |= TR_CALIBRATION_EVENT_REQUEST |
                    TR_EOS_EVENT_REQUEST | 
                    TR_PHYSICS_EVENT_REQUEST;
    // unmask inputs for SOS
    mde_data_16_ &= ~(TR_SOS_EVENT_REQUEST);
    mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );
    //gettimeofday(&timetag0,NULL);
    if ( mde_vmeStatus_ == cvSuccess )
      return 1;
  }

  if ( eventType & TR_CALIBRATION_EVENT_BUSY ) {
    mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
    mde_vmeStatus_ |= VME_READ_16( mde_current_address_, &mde_data_16_ );
    // mask inputs for CALIBRATION, PHYS and EOS events
    mde_data_16_ |= TR_CALIBRATION_EVENT_REQUEST |
                    TR_EOS_EVENT_REQUEST | 
                    TR_PHYSICS_EVENT_REQUEST;
    // unmask inputs for SOS events
    mde_data_16_ &= ~(TR_SOS_EVENT_REQUEST);
    mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );
    //gettimeofday(&timetag0,NULL);
    if ( mde_vmeStatus_ == cvSuccess )
      return 1;
  }

  return 0;
}

bool MDEv977::ArmTrailer() {

  mde_messanger_->sendMessage(this, "Going to arm Trailer. Board parameters:", MDE_INFO);
  this->getInfo();
  // Trailer is the last to arm
  // Set the DAQ Alive
  mde_vmeStatus_ = CAENVME_SetOutputRegister( mde_V2718_Handle_, cvOut0Bit );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Arming Trailer failed. \n", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "Arming Trailer successful. \n", MDE_INFO);
  return true;
}

bool MDEv977::DisArmTrailer() {
  // This Equipment is the first to be disarmed
  // Set all the busy outputs.
  this->setOutput(TR_TRIGGER_BUSY);

  // Clear the DAQ alive
  mde_vmeStatus_ = CAENVME_ClearOutputRegister( mde_V2718_Handle_, cvOut0Bit );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "DisArming Trailer failed. \n", MDE_FATAL);
    return false;
  }

//  if(EPICSClientsConnected)
//    lDATEStatusClient->Write("RUN_COMPLETE");

  mde_messanger_->sendMessage(this, "DisArming Trailer successful. \n", MDE_INFO);
  sleep(3);

  return true;
}

int MDEv977::ReadEventTrailer() {
  // This ReadEvent is always called at the end of the equipment List
  int dataStored = 0;

  switch (this->getEventType()) {
    case START_OF_BURST:
      // Nothing to do here
      // input mask set by the trigger receiver is still valid
      // the SOS busy will be reset by the EOS
      break;

     case PHYSICS_EVENT:
      // mask the busy for PHYS events
      this->addOutputMask(TR_PHYSICS_EVENT_BUSY);
      break;

    case END_OF_BURST:
      // no break here
    case CALIBRATION_EVENT:
      // unmask inputs for CALIBRATION events
      this->unmaksInput(TR_CALIBRATION_EVENT_REQUEST);
      // reset the busy: clear all outputs but the one set by software
      this->clearOutputs();
      // Reset the output mask
      this->unmaskOutput(TR_TRIGGER_MASK);
      break;

    default:
      MESSAGESTREAM <<"Unexpected event Type: " << this->getEventType();
      mde_messanger_->sendMessage(MDE_ERROR);
      break;
  }

  return dataStored;
}

int MDEv977::EventArrivedTrailer() {
  return 0;
}

bool MDEv977::ArmPartTriggerSelector(std::string trCondition) {

  mde_messanger_->sendMessage(this, "Going to arm Part. Trigger Selector. Board parameters:", MDE_INFO);
  this->getInfo();
  mde_messanger_->sendMessage("TriggerSelector: Trigger Condition is " + trCondition + ".", MDE_INFO);

  // Set output mask for channels reserved for GVA1, TOF0 and TOF1.
  mde_current_address_ = mde_base_address_ + V977_OUTPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ |= PTS_GVA1 | PTS_TOF0 | PTS_TOF1;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  // clear output channels reserved for GVA1, TOF0 and TOF1.
  mde_current_address_ = mde_base_address_ + V977_OUTPUT_SET;
  mde_vmeStatus_ |= VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ &= ~(PTS_GVA1 | PTS_TOF0 | PTS_TOF1);
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if ( mde_vmeStatus_ == cvSuccess ) {
    mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );

    if ( trCondition == "GVA1" ) {
      // set output for GVA1
      mde_data_16_ |= PTS_GVA1 ;
    } else if ( trCondition == "TOF0" ) {
      // set output for TOF0
      mde_data_16_ |= PTS_TOF0 ;
    } else if ( trCondition == "TOF1" ) {
      // set output TOF1
      mde_data_16_ |= PTS_TOF1 ;
    } else if ( trCondition == "PULSER" ) {
      // set output TOF1
      mde_data_16_ |= PTS_PULSER ;
    } else {
      // send warning message
      mde_messanger_->sendMessage("TriggerSelector: Unknown trigger condition: " + trCondition + ".", MDE_WARNING);
      mde_messanger_->sendMessage("TriggerSelector: The possible values are GVA1, TOF0, TOF1 or PULSER.", MDE_WARNING);
      mde_messanger_->sendMessage("TriggerSelector: Setting TOF1 trigger", MDE_WARNING);
      // set default = TOF1
      mde_data_16_ |= PTS_TOF1 ;
    }
  }

  if ( mde_vmeStatus_ == cvSuccess )
    mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Arming Part. Trigger Selector failed. \n", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "Arming Part. Trigger Selector successful. \n", MDE_INFO);
  return true;

}

bool MDEv977::DisArmPartTriggerSelector() {

  // clear output channels reserved for GVA1, TOF0 and TOF1.
  mde_data_16_ = PTS_GVA1 | PTS_TOF0 | PTS_TOF1;
  this->unsetInput( mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "DisArming Part. Trigger Selector failed. \n", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "DisArming Part. Trigger Selector successful. \n", MDE_INFO);
  return true;
}

bool MDEv977::softwareReset() {
  mde_data_16_ = 0;
  mde_current_address_ = mde_base_address_ +  V977_SOFTWARE_RESET;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do software reset.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Software reset done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::setInput(short mask) {

  mde_current_address_ = mde_base_address_ +  V977_INPUT_SET;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_ , &mde_data_16_ );
  mde_data_16_ |= mask;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the input.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Set input done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::unsetInput(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_INPUT_SET;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ &= ~mask;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to unset the iput.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Unset output done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::clearOutputs() {
  mde_data_16_ = 0;
  mde_current_address_ = mde_base_address_ +  V977_CLEAR_OUTPUT;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to do clear the outputs.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Clear outputs done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::setOutput(short mask) {

  mde_current_address_ = mde_base_address_ +  V977_OUTPUT_SET;
  mde_vmeStatus_ = VME_READ_16(mde_current_address_ , &mde_data_16_ );
  mde_data_16_ |= mask; //TR_TRIGGER_BUSY;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the outputs.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Set outputs done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::unsetOutput(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_OUTPUT_SET;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ &= ~mask;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to unset the outputs.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Unset outputs done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::setInputMask(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mask );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to mask the inputs.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Inputs mask is set.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::addInputMask(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ |= mask;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to add inputs mask.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Inputs mask is added.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::unmaksInput(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_INPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ &= ~(mask);
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to unmask the Output.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Unmask Output done.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::setOutputMask(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_OUTPUT_MASK;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mask );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to add Output Mask.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Output Mask is added.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::addOutputMask(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_OUTPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ |= mask;
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to add Output Mask.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Output Mask is added.", MDE_DEBUGGING);
  return true;
}

bool MDEv977::unmaskOutput(short mask) {
  mde_current_address_ = mde_base_address_ +  V977_OUTPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  mde_data_16_ &= ~(mask);
  mde_vmeStatus_ |= VME_WRITE_16( mde_current_address_, mde_data_16_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to unmask the Output.", MDE_ERROR);
    return false;
  }

  mde_messanger_->sendMessage(this, "Unmask Output done.", MDE_DEBUGGING);
  return true;
}

const char* MDEv977::getEventTypeAsString() {
  int evType = this->getEventType();
  const char* evTypeName;
  switch (evType) {
    case START_OF_RUN:
      evTypeName = "START_OF_RUN";
      break;

    case END_OF_RUN:
      evTypeName = "END_OF_RUN";
      break;

    case START_OF_BURST:
      evTypeName = "START_OF_BURST";
      break;

    case END_OF_BURST:
      evTypeName = "END_OF_BURST";
      break;

    case PHYSICS_EVENT:
      evTypeName = "PHYSICS_EVENT";
      break;

    case CALIBRATION_EVENT:
      evTypeName = "CALIBRATION_EVENT";
      break;

    default:
      evTypeName = "UNKNOWN";
  }
  return evTypeName;
}

void MDEv977::dump() {
  // Read the inputs and output.
  short input=0, output=0, single_hit=0, input_mask=0, input_set=0, output_mask=0, output_set=0;
  std::string ch_name, desc_in, desc_out;

  mde_current_address_ = mde_base_address_ + V977_INPUT_READ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &input);

  mde_current_address_ = mde_base_address_ + V977_SINGLEHIT_READ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &single_hit);

  mde_current_address_ = mde_base_address_ + V977_INPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &input_mask);

  mde_current_address_ = mde_base_address_ + V977_INPUT_SET;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &input_set);

  mde_current_address_ = mde_base_address_ + V977_OUTPUT_MASK;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &output_mask);
  output = single_hit & (~output_mask);

  mde_current_address_ = mde_base_address_ + V977_OUTPUT_SET;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &output_set);
  output |= output_set;

  std::cout << std::endl << "          Input    Output" << std::endl << std::endl;
  for(int ch=0;ch<16;ch++) {
    int ch_in  = (input>>ch) & 1;
    int ch_out = (output>>ch) & 1;

    int mask_i = (input_mask>>ch) & 1;
    int set_i  = (input_set>>ch) & 1;
    if(mask_i && set_i)         desc_in = "(ms)";
    else if (mask_i && !set_i)  desc_in = "(m-)";
    else if (!mask_i && set_i)  desc_in = "(-s)";
    else                        desc_in = "(--)";

    int mask_o = (output_mask>>ch) & 1;
    int set_o  = (output_set>>ch) & 1;
    if(mask_o && set_o)         desc_out = "(ms)";
    else if (mask_o && !set_o)  desc_out = "(m-)";
    else if (!mask_o && set_o)  desc_out = "(-s)";
    else                        desc_out = "(--)";

    switch (ch) {
      case 0:
        ch_name = "TR_PHYSICS_EVENT_REQUEST"; break;

      case 1:
        ch_name = "TR_PHYSICS_EVENT_BUSY"; break;

      case 2:
        ch_name = "TR_CALIBRATION_EVENT_REQUEST"; break;

      case 3:
        ch_name = "TR_CALIBRATION_EVENT_BUSY"; break;

      case 4:
        ch_name = "TR_SOS_EVENT_REQUEST"; break;

      case 5:
        ch_name = "TR_SOS_EVENT_BUSY"; break;

      case 6:
        ch_name = "TR_EOS_EVENT_REQUEST"; break;

      case 7:
        ch_name = "TR_EOS_EVENT_BUSY"; break;

      default:
        ch_name = ""; break;
    }
    std::cout << "ch:" << std::setw(3) << ch << "  |  "
              << ch_in << desc_in << "   " << ch_out << desc_out << " |  "
              << ch_name << std::endl;
  }
  std::cout << std::endl;
}




