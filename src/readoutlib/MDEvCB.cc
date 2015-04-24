#include "MDEvCB.hh"
#include "VRBDef.hh"
#include "MiceDAQMessanger.hh"
#include "maroc.h"

IMPLEMENT_FUNCTIONS(VCBParams, MDEvCB)

MDEvCB::MDEvCB()
: MiceDAQEquipment::MiceDAQEquipment() {
  SET_ALL_PARAMS_TO_ZERO(VCBParams)
  mde_name_ = "VCB";
}

bool MDEvCB::Arm() {
  mde_messanger_->sendMessage(this, "Going to arm a board with the following parameters:", MDE_INFO);
  this->getInfo();

  mde_base_address_ = (*this)["BaseAddress"];

  bool config_OK = true;
  int nFebs = (*this)["N_FEBs"];

  std::string path( EL_MICE_DIR );
  path = path + "/src/readoutlib/";
  std::string fname("MAROCconfig.txt");
  if ( !this->LoadMaskFromFile( (path + fname).c_str() ) )
    return false;

  for (int xFeb=0; xFeb<nFebs; xFeb++) {
    config_OK = this->configureFEB(xFeb);
    if (!config_OK) continue;
  }

  if ( !config_OK ) {
    mde_messanger_->sendMessage(this, "Arming failed. \n", MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Arming successful.";
  mde_messanger_->sendMessage(MDE_INFO);

  return true;
}

bool MDEvCB::DisArm() {
  MESSAGESTREAM << "Disarming successful. \n";
  mde_messanger_->sendMessage(MDE_INFO);
  return true;
}

int MDEvCB::ReadEvent() {
  return 0;
}

int MDEvCB::EventArrived() {
  return 0;
}


bool MDEvCB::configureFEB(unsigned int feb_id) {

  send_on_bus_mus(feb_id,3,10);
  int err = encode_reg_mar(feb_id);
  send_on_bus_mus(feb_id,3,1);

  if(err) {
    MESSAGESTREAM << "Unable to configure maroc on FEB "<< feb_id
                  << " (" << err << " errors)";
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  MESSAGESTREAM << "Maroc configured on FEB "<< feb_id;
  mde_messanger_->sendMessage(MDE_INFO);
  return true;
}

bool MDEvCB::LoadMaskFromFile(std::string fname) {

  std::ifstream maskFile(fname.c_str());
  if ( maskFile.fail() ) {
    MESSAGESTREAM << "Unable to open mask file "<< fname;
    mde_messanger_->sendMessage(MDE_FATAL);
    return false;
  }

  //char xMaskChar[1024];
  //maskFile.getline(xMaskChar,830);
  //xMaskChar[830] = '\0';
  std::string xMaskStr;
  char line[100];
  while (maskFile.getline(line,100)){
    int paramVal,paramNbits;
    if (line[0]=='#')continue;
    sscanf(line,"%*d %d %d",&paramVal,&paramNbits);
    std::string paramBitStr;
    int ii=paramVal;
    int i=0;
    while(ii!=0){
      char bit;
      sprintf(&bit,"%d",ii%2);
      paramBitStr+=bit;
      ii=ii/2;
      i++;
    }
    for (int j=0; j<paramNbits-i; j++) paramBitStr+="0";
    std::string paramBitStrInv(paramBitStr);
    for (int j=0; j<paramNbits; j++) paramBitStrInv.at(j)=paramBitStr.at(paramNbits-j-1);
    xMaskStr+=paramBitStrInv;
  }

  MESSAGESTREAM << "FEB mask is:\n" << xMaskStr << std::endl;
  mde_messanger_->sendMessage(MDE_DEBUGGING);

  LoadMask(xMaskStr.c_str());
  maskFile.close();

  return true;
}

void MDEvCB::LoadMask(const char* mask) {
  for (int i=0; i<829; i++) {
    sscanf(mask,"%1u",&mar_mask_[i]);
    mask++;
//     fprintf(stdout,"%d",mar_mask_[i]);

  }
}


int MDEvCB::encode_reg_mar(int febid) {
  int ibit;
  int idata;
  int iloop;
//  int config_add;
//  int readback_add;
  int outreg_data_in;
  int readback_bit;
  int nbitperchip;
  int nbit_tot;
  int ibad;
  int outreg_data;

  nbitperchip = 680;
  nbit_tot = 829;
  ibad = 0;

  mde_current_address_ = mde_base_address_ + 0xe000;
  mde_vmeStatus_ = VME_READ_16_A24(mde_current_address_, &outreg_data);
  if( mde_vmeStatus_ != cvSuccess ) {
      MESSAGESTREAM << "Unable to read from address " << std::hex << mde_current_address_ << std::dec;
      mde_messanger_->sendMessage(MDE_FATAL);
      return 1;
    }

//   read_vme(&outreg_data,config_add);

  /* send data in */
  for (iloop = 0; iloop<1; iloop++){
    for (ibit=0; ibit<nbit_tot; ibit++) {
      /* define the mask for a given bit */
      idata = mar_mask_[ibit];
      /* set data */
      if (idata == 1) {
        regin_up();
      } else if (idata == 0) {
        regin_down();
      } else {
        fprintf(stderr,"encode_reg_mar ERROR-1 >>> quit...\n");
        return (99);
      }
      clkreg_clk();
    }
  }
  /* re-send the bits and check */
  for (iloop = 0; iloop<1; iloop++) {
    for (ibit=0; ibit<nbit_tot; ibit++) {
      idata = mar_mask_[ibit];
      if (idata == 1) {
        regin_up();
      } else if (idata == 0) {
        regin_down();
      } else {
        fprintf(stderr,"encode_reg_mar ERROR-2 >>> quit...\n");
        return (99);
      }

      mde_current_address_ = mde_base_address_ + 0xf000;
      VME_READ_16_A24(mde_current_address_, &outreg_data_in);
//       read_vme(&outreg_data_in,readback_add);

      if (febid<4||(febid>7&&febid<12)) {
        readback_bit= (outreg_data_in >> (4*febid)) & 0x1;
      } else {
        readback_bit= (outreg_data_in >> (4*(febid-4))) & 0x1;
      }
//       fprintf(stdout, "encode_maroc ->  bit %d original %d read %d outreg_data_in 0x%x \n",
//                          ibit, mar_mask_[ibit], readback_bit,outreg_data_in);
      clkreg_clk();
      if (readback_bit !=  mar_mask_[ibit]) {
        ibad ++;
//         fprintf(stdout, "encode_maroc ->  bit %d original %d read %d\n",
//                 ibit, mar_mask_[ibit], readback_bit);
      }
    }
  }
  return ibad;
}

/* Clear one bit */
void MDEvCB::clear_bit(int *dataptr, int mode_bit) {
  *dataptr = *dataptr & (~(1<<mode_bit)); 
}

/* Set one bit */
void MDEvCB::set_bit(int *dataptr, int mode_bit) {
  *dataptr = *dataptr | (1<<mode_bit); 
}

void MDEvCB::clock_bus_pulse() {
  int local_reg;

  mde_current_address_ = mde_base_address_ + 0xe000;
  VME_READ_16_A24(mde_current_address_, &local_reg);
//   read_vme(&local_reg,addr_conn_out);

  clear_bit(&local_reg,0);

  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);

  set_bit(&local_reg,0);

  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);

  clear_bit(&local_reg,0);

  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);
}

void MDEvCB::regin_up() {
  unsigned short mask;
  int bit_reg;
  int outreg_data;
  bit_reg = 5;

  mde_current_address_ = mde_base_address_ + 0xe000;
  VME_READ_16_A24(mde_current_address_, &outreg_data);
//   read_vme(&outreg_data,addr_conn_out);
  mask = 0x1 << bit_reg;
  outreg_data = outreg_data | mask;

  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
}

void MDEvCB::regin_down() {
  unsigned short mask;
  int bit_reg;
  int outreg_data;
  bit_reg = 5;

  mde_current_address_ = mde_base_address_ + 0xe000;
  VME_READ_16_A24(mde_current_address_, &outreg_data);
//   read_vme(&outreg_data,addr_conn_out);

  mask = ~(0x1 << bit_reg);
  outreg_data = outreg_data & mask;

  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
}

void MDEvCB::clkreg_clk() {
  unsigned short mask;
  int bit_reg;
  int outreg_data;
  bit_reg = 4;

  mde_current_address_ = mde_base_address_ + 0xe000;
  VME_READ_16_A24(mde_current_address_, &outreg_data);
//   read_vme(&outreg_data,addr_conn_out);

  mask = 0x1 << bit_reg;
  outreg_data = outreg_data | mask;

  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);


  mask = ~(0x1 << bit_reg);
  outreg_data = outreg_data & mask;

  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
  VME_WRITE_16_A24(mde_current_address_, outreg_data);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
//   write_vme(outreg_data,addr_conn_out);
}

void MDEvCB::send_on_bus_mus(int febid, int address, int data) {
  int local_reg;
  int ibit;
  int xbit;
  int local_addr;

  local_addr = address | ((febid & 0xf) << 8);

  mde_current_address_ = mde_base_address_ + 0xe000;
  mde_vmeStatus_ = VME_READ_16_A24(mde_current_address_, &local_reg);
  if( mde_vmeStatus_ != cvSuccess ) {
      MESSAGESTREAM << "Unable to read from address 0x" << std::hex << mde_current_address_ << std::dec;
      mde_messanger_->sendMessage(MDE_FATAL);
      return;
    }

//   read_vme(&local_reg,addr_conn_out);

  clear_bit(&local_reg,1);

  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);

  set_bit(&local_reg,1);

  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);

  set_bit(&local_reg,2);

  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);

  clock_bus_pulse();
  clock_bus_pulse();
  for (ibit=0; ibit<16; ibit++) {
    xbit = (local_addr >> (15-ibit)) & 0x1;
    if (xbit==1) {
      set_bit(&local_reg,2);
      VME_WRITE_16_A24(mde_current_address_, local_reg);
//       write_vme(local_reg,addr_conn_out);
    } else {
      clear_bit(&local_reg,2);
      VME_WRITE_16_A24(mde_current_address_, local_reg);
//       write_vme(local_reg,addr_conn_out);
    }
    clock_bus_pulse();
  }
  for (ibit=0; ibit<16; ibit++) {
    xbit = (data >> (15-ibit)) & 0x1;
    if (xbit==1) {
      set_bit(&local_reg,2);
//       write_vme(local_reg,addr_conn_out);
      VME_WRITE_16_A24(mde_current_address_, local_reg);
    } else {
      clear_bit(&local_reg,2);
      VME_WRITE_16_A24(mde_current_address_, local_reg);
//       write_vme(local_reg,addr_conn_out);
    }
    clock_bus_pulse();
  }
  clear_bit(&local_reg,1);
  VME_WRITE_16_A24(mde_current_address_, local_reg);
//   write_vme(local_reg,addr_conn_out);
  clock_bus_pulse();
}
