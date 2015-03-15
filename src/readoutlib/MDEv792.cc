#include <vector>
#include <sstream>

#include "MDEv792.hh"
#include "V792Def.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(V792Params, MDEv792)

MDEv792::MDEv792()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V792Params)
  mde_name_ = "V792";
}

int MDEv792::getMaxMemUsed(int nEvents) {
  int memPerEvent = 34*4;

  return memPerEvent*nEvents+1;
}

bool MDEv792::Arm() {

  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  getInfo();

  mde_base_address_ = (*this)["BaseAddress"];

  short xCtrlReg(0);
  if ((*this)["BErrEnable"])
    xCtrlReg |= CR_BERR_ENABLE;

  if ((*this)["BlkEndEnable"])
    xCtrlReg |= CR_BLKEND_ENABLE;

  if ( this->boardReset() &&
       this->setGEO( (*this)["GEO"] ) &&
       this->setControlReg(xCtrlReg) &&
       this->enableOV() ) {

    MESSAGESTREAM << "Arming successful." << std::endl;
    mde_messanger_->sendMessage(MDE_INFO);
    return true;
  }

  mde_messanger_->sendMessage(this, "Arming failed! \n", MDE_FATAL);
  return false;
}


bool MDEv792::DisArm() {
  mde_messanger_->sendMessage(this, "DisArming successful. \n", MDE_INFO);
  return true;
}


bool MDEv792::boardReset() {
  if ( !this->bitSetReg1(REG1_SOFT_RESET) )
    return false;

  if ( !this->bitClearReg1(REG1_SOFT_RESET) )
    return false;

  return true;
}

bool MDEv792::bitSetReg1(short reg1) {
  mde_current_address_ = mde_base_address_ + V792_BIT_SET_REGISTER_1;
  mde_data_16_ = reg1;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Reg1 failed to set.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Reg1 is set to: " << reg1;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}


bool MDEv792::bitClearReg1(short reg1) {
  mde_current_address_ = mde_base_address_ + V792_BIT_CLEAR_REGISTER_1;
  mde_data_16_ = reg1;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_);
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Reg1 failed to reset.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Reg1 is reset (from " << reg1 << ")";
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv792::bitSetReg2(short reg2) {
  mde_current_address_ =  mde_base_address_ + V792_BIT_SET_REGISTER_2;
  mde_data_16_ =  reg2  ; 
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );
  if( mde_vmeStatus_ !=cvSuccess ){
    mde_messanger_->sendMessage(this, "Bit Set Reg 2 failed.", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "Bit Set Reg 2 set.", MDE_DEBUGGING);
  return true;
}

bool MDEv792::bitClearReg2(short reg2) {
  mde_current_address_ = mde_base_address_ + V792_BIT_CLEAR_REGISTER_2;
  mde_data_16_ =  reg2;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ );
  if( mde_vmeStatus_ !=cvSuccess ) {
    mde_messanger_->sendMessage(this, "Bit Clear Reg 2 failed.", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "Bit Clear Reg 2 set.", MDE_DEBUGGING);
  return true;
}

bool MDEv792::setControlReg(short cr){
  mde_current_address_ = mde_base_address_ + V792_CONTROL_REG ;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, cr );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Control Register!", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Control Register is set to " << std::hex << cr << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

int MDEv792::getGEO() {
  mde_current_address_ = mde_base_address_ + V792_GEO_REGISTER ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_);
  if ( mde_vmeStatus_ != cvSuccess ){
    mde_messanger_->sendMessage(this, "Unable to get the GEO Address!", MDE_FATAL);
    return false;
  }

  int geo = mde_data_16_ & V792_GEO_MASK;
  MESSAGESTREAM << "GEO Address is " << geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return geo;
}

bool MDEv792::setGEO(int geo) {
  mde_current_address_ = mde_base_address_ + V792_GEO_REGISTER;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, geo );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to set the Geo Address.", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "GEO Address is set to " << geo;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return true;
}

int MDEv792::getFirmwareV(){
  mde_current_address_ = mde_base_address_ + V792_FIRMWARE_REV;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_);
  if ( mde_vmeStatus_ != cvSuccess ){
    mde_messanger_->sendMessage(this, "Unable to get the Firmware version!", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Firmware version is " << std::hex << mde_data_16_ << std::dec;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return mde_data_16_;
}

short MDEv792::getStatusReg1() {
  mde_current_address_ = mde_base_address_ + V792_STATUS_REGISTER_1 ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to get the Status Register1!", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Status Register1 is " << this->Status1ToString(mde_data_16_);
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return mde_data_16_;
}

short MDEv792::getStatusReg2() {
  mde_current_address_ = mde_base_address_ + V792_STATUS_REGISTER_2 ;
  mde_vmeStatus_ = VME_READ_16( mde_current_address_, &mde_data_16_ );
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to get the Status Register2!", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Status Register2 is: " << this->Status2ToString(mde_data_16_);
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return mde_data_16_;
}

std::string MDEv792::Status1ToString(short stReg) {
  int dready   = stReg & STATUS1_DREADY;
  int g_dready = stReg & STATUS1_GLOBAL_DREADY;
  int busy     = stReg & STATUS1_BUSY;
  int g_busy   = stReg & STATUS1_GLOBAL_BUSY;
  int amnesia  = stReg & STATUS1_AMNESIA;
  int purged   = stReg & STATUS1_PURGED;
  int term_on  = stReg & STATUS1_TERM_ON;
  int term_off = stReg & STATUS1_TERM_OFF;
  int evrdy    = stReg & STATUS1_EVRDY;


  std::string status_message;
  if (dready) status_message   += " **Data Ready**";
  if (g_dready) status_message += " **Global Data Ready**";
  if (busy) status_message     += " **Busy**";
  if (g_busy) status_message   += " **Global Busy**";
  if (amnesia) status_message  += " **Amnesia**";
  if (purged) status_message   += " **Purged**";
  if (term_on) status_message  += " **Term On**";
  if (term_off) status_message += " **Term Off**";
  if (evrdy) status_message    += " **Evrdy**";

  return status_message;
}

std::string MDEv792::Status2ToString(short stReg) {
  int empty = stReg & STATUS2_BUFFER_EMPTY;
  int full  = stReg & STATUS2_BUFFER_FULL;
  std::string status_message;
  if (empty)     status_message = "**Buffer Empty**";
  else if (full) status_message = "**Buffer Full**";
  else {
    std::stringstream ss;
    ss << std::hex << stReg << std::dec;
    status_message = ss.str();
  }

  return status_message;
}

int MDEv792::doBLTRead(MDE_Pointer *data) {
  int nbStored = 0;
  int nbRead = 0;
  int nBLTCycle=0;
  if (!data) data = mde_dataPtr_;
  mde_current_address_ = mde_base_address_ + V792_OUT_BUFFER;

  while(  mde_vmeStatus_ == cvSuccess ){
//     mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, dataPtr, V792_OUT_BUFFER_MAX_SIZE, &nbRead);
    mde_vmeStatus_ = VME_READ_BLT_32( mde_current_address_, data, 512, &nbRead);
    nbStored += nbRead;
    data += nbRead/sizeof(MDE_Pointer); //Move the data pointer by the number of words read.
    MESSAGESTREAM << "BLT cycle number " << ++nBLTCycle << ". " << nbRead << " bytes read. (" << nbStored << ")  ";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
  }

//   MESSAGESTREAM << "Number of BLT cycles: " << nBLTCycle << ".  bytes stored: " << nbStored << std::endl
//                 << "The VME status code is: " <<  getVmeStatusCodeName( mde_vmeStatus_ );
//   mde_messanger_->sendMessage(MDE_WARNING);

  if (mde_vmeStatus_ < cvBusError) {
    // This is a serious error. Stop reading.
    MESSAGESTREAM << "Error in the BLT cycle. The VME error code is: " <<  getVmeStatusCodeName( mde_vmeStatus_ );
    mde_messanger_->sendMessage(MDE_WARNING);
    return nbStored;
  }
  mde_vmeStatus_ = cvSuccess;
  return nbStored;
}

int MDEv792::ReadEvent(MDE_Pointer *data) {
  int nbStored = 0;
//   int nbRead   = 0;
  if (!data) data = mde_dataPtr_;
  mde_base_address_ = (*this)["BaseAddress"];

  short stReg1 = this->getStatusReg1();
  short stReg2 = this->getStatusReg2();

  bool full  = this->bufferFull(stReg2);
  if (full) {
    mde_messanger_->sendMessage(this, "The Buffer is Full! Not writing data", MDE_ERROR);
    return 0;
  }

  bool ready = this->dataReady(stReg1);
  bool busy  = this->boardBusy(stReg1);
  if(ready && !busy) {
//     nbStored += this->doBLTRead();

    //Get the header.
    mde_current_address_ = mde_base_address_ + V792_OUT_BUFFER;
    mde_vmeStatus_ = VME_READ_32( mde_current_address_, data);
//       std::cout << " First word is: " << std::hex << *data << "   addr: " << data << std::dec << std::endl;
    unsigned int header = *data;
    int h_type =  ( header >> 24 ) & 0x7;
    if ( h_type!=2 ) {
      MESSAGESTREAM << "First word is not a header! ("
                    << std::hex << *data << std::dec << ")";
      mde_messanger_->sendMessage(MDE_ERROR);
      return 0;
    }

    data++;
    nbStored += sizeof(MDE_Pointer);

    // Get the measurements in the channels.
    int n_dw = (( (header) >> 8 ) & 0x3F ) + 1;
    unsigned int measurement;
    int measurement_i;
//     uint32_t addrss[n_dw];
//     for (int i=0; i< n_dw; i++)
//       addrss[i] = mde_current_address_;
// 
//     VME_MULTIREAD_32(addrss, data, n_dw);
//     nbStored += sizeof(MDE_Pointer)*n_dw;

    for (int i=0; i< n_dw; i++) {
      mde_vmeStatus_ = VME_READ_32( mde_current_address_, data);
      measurement = *data;
      measurement_i = measurement & 0xFFF;
//       std::cout << i << std::dec << " " << measurement_i << std::hex << "  data_ptr " << *data << " addr: "  << std::hex << data << std::dec << std::endl;
      data++;
      nbStored += sizeof(MDE_Pointer);
    }


    if ( (*this)["BlkEndEnable"] == 0) {
      // Get the End of Block.
      mde_vmeStatus_ = VME_READ_32( mde_current_address_, data);
//       std::cout << " " << std::hex << *data << "   addr: " << data << std::dec << std::endl;
      data++;
      nbStored += sizeof(MDE_Pointer);
    }
  }

  MESSAGESTREAM << "Data stored: " << nbStored;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  return nbStored;
}

int MDEv792::EventArrived() {
  short stReg1 = this->getStatusReg1();
  
  if( this->dataReady(stReg1) )
    return 1;
  else
    return 0;
}

bool MDEv792::evntCounterReset() {
  mde_current_address_ = mde_base_address_ + V792_EVNT_COUNT_RESET ;
  mde_data_16_ = 0x0;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ ) ;
  if ( mde_vmeStatus_ != cvSuccess ) {
    mde_messanger_->sendMessage(this, "Unable to reset the Event Count.", MDE_FATAL);
    return false;
  }
  mde_messanger_->sendMessage(this, "Event count reset", MDE_DEBUGGING );

  return true;
}

bool MDEv792::dataReady(short reg){
  if (reg & 0x1)
    return true;

  return false;
}

bool MDEv792::boardBusy(short reg) {
  if (reg >> 2 & 0x1)
    return true;

  return false;
}

bool MDEv792::bufferFull(short reg) {
  if (reg >> 2 & 0x1)
    return true;

  return false;
}

bool MDEv792::bufferEmpty(short reg) {
  if (reg >> 1 & 0x1)
    return true;

  return false;
}


bool MDEv792::AcqTestMode() {
  
  if ( !this->bitSetReg2(REG2_ACQ_TEST_MODE) ) 
    return false;
  
  if ( !this->bitClearReg2(REG2_ACQ_TEST_MODE) )
    return false;
  
  mde_current_address_ = mde_base_address_ + V792_TEST_EVNT_WRITE ;
  mde_data_16_ = 80;
  
  for ( int i=0; i<32; i++ ){
    
    mde_data_16_ += 4;
    mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ ) ;
    if( mde_vmeStatus_ != cvSuccess ){
      mde_messanger_->sendMessage(this, "Writing data failed.", MDE_FATAL);
      return false;
    }
    mde_messanger_->sendMessage(this, " Data word written", MDE_DEBUGGING);
  }
  
  if ( !this->bitSetReg2(REG2_ACQ_TEST_MODE) ) 
    return false;
  
  mde_current_address_ = mde_base_address_ + V792_SW_COMM ;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, mde_data_16_ ) ;
  if( mde_vmeStatus_ != cvSuccess ){
    mde_messanger_->sendMessage(this, "Write to SW_COM failed.", MDE_FATAL);
    return false;  
  }
  mde_messanger_->sendMessage(this, "Writing to SW_COM done.", MDE_DEBUGGING);
  
  if ( !this->bitClearReg2(REG2_ACQ_TEST_MODE) ){
    std::cout << "Can't return to normal mode Reg 2" << std::endl;
    return false;
  }
  return true;
}

bool MDEv792::enableOV(){
  mde_current_address_ = mde_base_address_ + V792_BIT_SET_REGISTER_2;
  short ov = REG2_OVER_FLOW;
  mde_vmeStatus_ = VME_WRITE_16( mde_current_address_, ov );
  
  if (mde_vmeStatus_ !=cvSuccess){
      mde_messanger_->sendMessage(this, "Failed to enable OVER FLOW", MDE_FATAL);
      return false;
  }
  mde_messanger_->sendMessage(this, "Over flow data enabled", MDE_DEBUGGING);
  return true;
}

int MDEv792::DataPtrTest(MDE_Pointer *data, int evNum){
//   0xA2000500 - header word
//   0xA0010085 - channel 1
//   0xA0020715 - channel 2
//   0xA0000000 - geo, no ch, no data
//   0xA4000000 - eob word

     data[0] = 0xA2000500;
     data[1] = 0xA0010085;
     data[2] = 0xA0020715;
     data[3] = 0xA0030235;
     data[4] = 0xA0040681;
     data[5] = 0xA0050093;
     data[6] = 0xA4000000 + evNum ; 
  
     return 1; 
}
