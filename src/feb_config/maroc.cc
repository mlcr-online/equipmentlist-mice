#include "equipmentList_common.hh"
#include "vme.h"
#include "maroc.h"

extern int debug;
extern int FEBID;
extern int addr_conn_out;    /* */
extern int addr_conn_in;     /* */
extern int mar_mask[829];
extern int encode_reg_mar(int febid);

int encode_reg_mar(int febid)
{
  int ibit;
  int idata;
  int iloop;
  int config_add;
  int readback_add;
  int outreg_data_in;
  int readback_bit;
  int nbitperchip;
  int nbit_tot;
  int bit_local[4096];
  int ibad;
  int outreg_data;

  nbitperchip = 680;
  nbit_tot = 829;
  ibad = 0;

  for (ibit=0; ibit<nbit_tot; ibit++) bit_local[ibit] = mar_mask[ibit];

  config_add = addr_conn_out;

  readback_add = addr_conn_in;

  read_vme(&outreg_data,config_add);

  std::cout << std::hex << "config_add: " << config_add << "   readback_add: " << readback_add
           << std::dec << std::endl;

  /* send data in */
  for (iloop = 0; iloop<1; iloop++){
    for (ibit=0; ibit<nbit_tot; ibit++) {
      /* define the mask for a given bit */
      idata = bit_local[ibit];
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
  for (iloop = 0; iloop<1; iloop++){
    for (ibit=0; ibit<nbit_tot; ibit++) {
      idata = bit_local[ibit];
      if (idata == 1) {
        regin_up();
      } else if (idata == 0) {
        regin_down();
      } else {
        fprintf(stderr,"encode_reg_mar ERROR-2 >>> quit...\n");
        return (99);
      }
      read_vme(&outreg_data_in,readback_add);
      if (febid<4) {
        readback_bit= (outreg_data_in >> (4*febid)) & 0x1;
      } else if (febid>=4) {
        readback_bit= (outreg_data_in >> (4*(febid-4))) & 0x1;
      }
      if (debug) fprintf(stdout, "encode_maroc ->  bit %d original %d read %d outreg_data_in 0x%x \n",
                         ibit, bit_local[ibit], readback_bit,outreg_data_in);
      clkreg_clk();
      if (readback_bit !=  bit_local[ibit]) {
        ibad = 1;
        fprintf(stdout, "encode_maroc ->  bit %d original %d read %d\n",
                ibit, bit_local[ibit], readback_bit);
      }
    }
  }
  return ibad;
}

bool LoadMaroc()
{
  fprintf(stdout,"\n-----------------\nLoad MAROC mask\n---\n");

  FILE *mfile;
  char str[830]={0};
  char* str_ptr;
//   int FEBID;
  int i;
  int err;

  for (i=0; i<829; i++) mar_mask[i] = 0;

  mfile = fopen("../src/feb_config/maroc3.dat","r");

  fgets(str,830,mfile);
  str_ptr = str;

  for (i=0; i<829; i++) {
    sscanf(str_ptr,"%1u",&mar_mask[i]);
    str_ptr++;
    if (debug) fprintf(stdout,"%d",mar_mask[i]);
  }
  fprintf(stdout,"\n");
  fclose(mfile);

  send_on_bus_mus(FEBID,3,10);
  err = encode_reg_mar(FEBID);
  send_on_bus_mus(FEBID,3,1);

  if (err) {
    fprintf(stdout,"---\nMAROC mask NOT loaded on board %d \n-----------------\n",FEBID);
    return false;
  }

  fprintf(stdout,"---\nMAROC mask loaded on board %d \n-----------------\n",FEBID);
  return true;
}




