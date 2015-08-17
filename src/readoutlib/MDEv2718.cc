#include "MDEv2718.hh"
#include "MiceDAQMessanger.hh"

IMPLEMENT_FUNCTIONS(V2718Params, MDEv2718)

MDEv2718::MDEv2718()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(V2718Params)
  mde_name_ = "V2718";
}

bool MDEv2718::Arm() {
  //Initialize V2718
  mde_messanger_->sendMessage(this, "Going to arm a board with following parameters:", MDE_INFO );
  getInfo();

  if (this->Init() && this->Configure()) {
    mde_messanger_->sendMessage(this, "Arming successful. \n", MDE_INFO);
    return true;
  }

  mde_messanger_->sendMessage("V2718: Arming failed. \n", MDE_FATAL);
  return false;
}

bool MDEv2718::DisArm() {
  // This will be the last equipment to be disarmed
  mde_vmeStatus_ = CAENVME_End( mde_V2718_Handle_ );
  if( mde_vmeStatus_ != cvSuccess ){
    MESSAGESTREAM << "Unable to End. Status: " << mde_vmeStatus_;
    mde_messanger_->sendMessage(MDE_ERROR);
    mde_messanger_->sendMessage(this, "DisArming failed. \n", MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, "Disarming successful. \n", MDE_INFO);

  return true;
}

int MDEv2718::ReadEvent() {
  return 0;
}

int MDEv2718::EventArrived() {
  return 0;
}

bool MDEv2718::Init() {
  mde_vmeStatus_ = CAENVME_Init( cvV2718,
                                 this->getParam("BLink"),
                                 this->getParam("BNumber"),
                                 &mde_V2718_Handle_ );

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to initialize. Status: " << mde_vmeStatus_;
    mde_messanger_->sendMessage(MDE_ERROR);
    mde_messanger_->sendMessage(this, "Check that the crate is ON and the controller is connected.",
                                  MDE_FATAL);
    return false;
  }

  return true;
}

bool MDEv2718::Configure() {
  // Set the static data member max event number.
  this->setMaxNumOfEvts( this->getParam("MaxNumEvts") );

  if (this->SetFIFOMode())
    return true;

  return false;
}

bool MDEv2718::SetFIFOMode() {
  if ( this->getParam("FIFOMode") )
    mde_vmeStatus_ = CAENVME_SetFIFOMode(mde_V2718_Handle_, 1);
  else
    mde_vmeStatus_ = CAENVME_SetFIFOMode(mde_V2718_Handle_, 0);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set the FIFO mode";
    mde_messanger_->sendMessage(MDE_FATAL);

    return false;
  }

  MESSAGESTREAM << "FIFO mode is set to " << this->getParam("FIFOMode");
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv2718::SetOutputConf() {

    mde_vmeStatus_ = CAENVME_SetOutputConf(mde_V2718_Handle_, 
                                           cvOutput0,
                                           cvDirect, 
                                           cvActiveHigh,
                                           cvMiscSignals);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to configure the output 0. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Output 0 cofigured ", MDE_DEBUGGING);
  MESSAGESTREAM << "Output 0 cofigure status:" << mde_vmeStatus_ ;

  mde_vmeStatus_ = CAENVME_SetOutputConf(mde_V2718_Handle_, 
                                         cvOutput1,
                                         cvDirect, 
                                         cvActiveHigh,
                                         cvMiscSignals);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to configure the output 1. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Output 1 cofigured ", MDE_DEBUGGING);
  MESSAGESTREAM << "Output 1 cofigure status:" << mde_vmeStatus_ ;
  return true;
}

bool MDEv2718::SetInputConf() {
  //  Configure input 0
  mde_vmeStatus_ = CAENVME_SetInputConf( mde_V2718_Handle_,  // Handle
                                         cvInput0,           // CVInputSelect
                                         cvDirect,           // CVIOPolarity 
                                         cvActiveHigh);      // CVLEDPolarity

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to configure input 0. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  //  Configure input 1
  mde_vmeStatus_ = CAENVME_SetInputConf( mde_V2718_Handle_,
                                         cvInput1,
                                         cvDirect,
                                         cvActiveHigh);

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to configure input 1. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Input 1 cofigured ", MDE_DEBUGGING);
  MESSAGESTREAM << "Input 1 cofigure status:" << mde_vmeStatus_ ;
  return true;
}


bool MDEv2718::SetOutputSignal() {
  mde_vmeStatus_ = CAENVME_SetOutputRegister ( mde_V2718_Handle_, cvPulsAStartBit );
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set output signal. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Output signal sent ", MDE_DEBUGGING);
  MESSAGESTREAM << "Output signal status:" << mde_vmeStatus_ ;
  return true;
}


bool MDEv2718::ClearOutputSignal(){
  mde_vmeStatus_ = CAENVME_ClearOutputRegister ( mde_V2718_Handle_, cvOut0Bit );
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to clear output signal. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Output signal cleared ", MDE_DEBUGGING);
  MESSAGESTREAM << "Output signal clear status:" << mde_vmeStatus_ ;
  return true;
}


bool MDEv2718::SetPulserConf() {
   unsigned char period = 3;
   unsigned char width = 2;
   unsigned char nPulses = 1;

   mde_vmeStatus_ = CAENVME_SetPulserConf(mde_V2718_Handle_,
                                          cvPulserA,
                                          period,
                                          width,
                                          cvUnit25ns,
                                          nPulses,
                                          cvManualSW,
                                          cvManualSW );

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set the Pulser mode. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Pulser set ", MDE_DEBUGGING);
  MESSAGESTREAM << "Pulser mode. Status:" << mde_vmeStatus_ ;

  return true;
}

bool MDEv2718::GetPulserConf() {
   unsigned char period;
   unsigned char width;
   unsigned char nPulses;

   CVTimeUnits timeUnit;
   CVIOSources startSrc;
   CVIOSources stopSrc;

   mde_vmeStatus_ = CAENVME_GetPulserConf(mde_V2718_Handle_,
                                          cvPulserA,
                                          &period,
                                          &width,
                                          &timeUnit,
                                          &nPulses,
                                          &startSrc,
                                          &stopSrc );

  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to set the Pulser mode. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Pulser set ", MDE_DEBUGGING);
  return true;
}

bool MDEv2718::StartPulser() {
  mde_vmeStatus_ = CAENVME_StartPulser( mde_V2718_Handle_, cvPulserA );
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to start pulser. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Pulser started ", MDE_DEBUGGING);
  return true;
}

bool MDEv2718::StopPulser(){
  mde_vmeStatus_ = CAENVME_StopPulser( mde_V2718_Handle_, cvPulserA );
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to stop pulser. Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  mde_messanger_->sendMessage(this, " Pulser stopped ", MDE_DEBUGGING);
  return true;
}

unsigned int MDEv2718::ReadRegister() {

  unsigned int data;
  mde_vmeStatus_ = CAENVME_ReadRegister(mde_V2718_Handle_, 
                                        cvInputReg,
                                        &data );
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Unable to read Inp Reg Status:" << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return 0;
  }

  MESSAGESTREAM << "Inp reg: " << data ;
  mde_messanger_->sendMessage( MDE_DEBUGGING);
  return data;
}
bool MDEv2718::IRQEnable(){
  uint32_t Mask = 0;
  mde_vmeStatus_ = CAENVME_IRQEnable(mde_V2718_Handle_, Mask);
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Failed to enable IRQ lines: " << Mask << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }
  return true;
}

int MDEv2718::IRQWait(){
  uint32_t Mask = 0x40;
  mde_vmeStatus_ = CAENVME_IRQWait(mde_V2718_Handle_, Mask, 1);
  if( mde_vmeStatus_ != cvSuccess && mde_vmeStatus_ != cvTimeoutError) {
    MESSAGESTREAM << "IRQ Wait failed at line " << Mask << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return -1;
  }
  else if (mde_vmeStatus_ == cvTimeoutError){
    MESSAGESTREAM << "IRQ Wait Timeout ";
    mde_messanger_->sendMessage(MDE_DEBUGGING);
    return 1;
  }
  MESSAGESTREAM << "IRQ Wait Status: " << mde_vmeStatus_ ;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return 0;
}

bool MDEv2718::IACK(){
  void *Vector(0);
  mde_vmeStatus_ = CAENVME_IACKCycle(mde_V2718_Handle_, cvIRQ7,  Vector, cvD16);
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "IACK Cycle failed at level " << cvIRQ7 << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }
  MESSAGESTREAM << "IACK Cycle returned Vector " << Vector << mde_vmeStatus_ ;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

bool MDEv2718::IRQCheck(){
  mde_vmeStatus_ = CAENVME_IRQCheck( mde_V2718_Handle_, &mde_data_8_);
  if( mde_vmeStatus_ != cvSuccess ) {
    MESSAGESTREAM << "Failed to obtain the active IRQ lines " << mde_vmeStatus_ ;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }
  MESSAGESTREAM << "IRQ line active : " << std::hex << (int)mde_data_8_  << std::dec ;
  mde_messanger_->sendMessage(MDE_DEBUGGING);
  return true;
}

