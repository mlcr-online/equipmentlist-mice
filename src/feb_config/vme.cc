#include "equipmentList_common.hh"
#include "globals.h"
#include "vme.h"
#include "maroc.h"

extern int32_t BHandle;
extern int addr_conn_out;

void InitVme()
{
  fprintf(stdout,"\n-----------------\nInit VME\n---\n");
  std::cout << "addr_conn_out: " << addr_conn_out << std::endl;
  CVBoardTypes  VMEBoard;
  short         Link;
  short         Device;
  
  // Initialize the Board
  VMEBoard = cvV2718;
  Link = 0;
  Device = 0;
  if( CAENVME_Init(VMEBoard, Link, Device, &BHandle) != cvSuccess ) {
    printf("\n\n Error opening the device\n");
    exit(1); }
  else fprintf(stdout,"---\nVME initialised\n-----------------\n");
}

/* Routine to read from VME bus (24 bit addr and 16 bit data) */
unsigned short read_vme(int *data, int address) 
{
  /*
  unsigned short value_16;
  int read_size;
  if (lseek(ifile_map,(off_t)address,SEEK_SET)==-1) {
    perror("read_vme lseek(): ");
    exit(EXIT_FAILURE);
  }
  if ((read_size=read(ifile_map,(void*)(&value_16),2))==-1) {
    perror("read_vme read(): ");
    exit(EXIT_FAILURE);
  }
  *data_ptr = value_16;
  */
  CVErrorCodes ret;
  ret = CAENVME_ReadCycle(BHandle,(uint32_t)address,data,cvA24_U_DATA,cvD16);
  if (ret==cvSuccess) ;//printf(" Cycle(s) completed normally\n");
  else if (ret==cvBusError){ printf(" Bus Error !!!"); return(1);}
  else if (ret==cvCommError){ printf(" Communication Error !!!"); return(1);}
  else{ printf(" Unknown Error !!!");  return(1);}  
  return(0);
}

/* Routine to write on VME bus (24 bit addr and 16 bit data) */
unsigned short write_vme(int data, int address) 
{
  /*
  unsigned short value_16=data;
  int written_size;
  if (lseek(ifile_map,(off_t)address,SEEK_SET)==-1) {
    perror("write_vme lseek(): ");
    exit(EXIT_FAILURE);
  }
  if ((written_size=write(ifile_map,&value_16,2))==-1) {
    perror("write_vme write(): ");
    exit(EXIT_FAILURE);
  }  
  */  
  CVErrorCodes ret;
  ret = CAENVME_WriteCycle(BHandle,(uint32_t)address,&data,cvA24_U_DATA,cvD16);
  if (ret==cvSuccess) ;//printf(" Cycle(s) completed normally\n");
  else if (ret==cvBusError){ printf(" Bus Error !!!"); return(1);}
  else if (ret==cvCommError){ printf(" Communication Error !!!"); return(1);}
  else{ printf(" Unknown Error !!!");  return(1);}  
  return(0);
}

/* Clear one bit */
void clear_bit(int *dataptr, int mode_bit)
{
  *dataptr = *dataptr & (~(1<<mode_bit)); 
}

/* Set one bit */
void set_bit(int *dataptr, int mode_bit)
{
  *dataptr = *dataptr | (1<<mode_bit); 
}

void clock_bus_pulse() 
{
  int local_reg;
  read_vme(&local_reg,addr_conn_out);
  clear_bit(&local_reg,0);
  write_vme(local_reg,addr_conn_out);
  set_bit(&local_reg,0);
  write_vme(local_reg,addr_conn_out);
  clear_bit(&local_reg,0);
  write_vme(local_reg,addr_conn_out);
}

void regin_up()
{
  unsigned short mask;
  int bit_reg;
  int outreg_data;
  bit_reg = 5;
  read_vme(&outreg_data,addr_conn_out);
  mask = 0x1 << bit_reg;  
  outreg_data = outreg_data | mask; 
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
}

void regin_down()
{
  unsigned short mask;
  int bit_reg;
  int outreg_data;
  bit_reg = 5;
  read_vme(&outreg_data,addr_conn_out);
  mask = ~(0x1 << bit_reg);
  outreg_data = outreg_data & mask;
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
}

void clkreg_clk()
{
  unsigned short mask;
  int bit_reg;
  int outreg_data;
  bit_reg = 4;
  read_vme(&outreg_data,addr_conn_out);
  mask = 0x1 << bit_reg;
  outreg_data = outreg_data | mask;
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
  mask = ~(0x1 << bit_reg);
  outreg_data = outreg_data & mask;
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
  write_vme(outreg_data,addr_conn_out);
}

void send_on_bus_mus(int febid, int address, int data) 
{
  int local_reg;
  int ibit;
  int xbit;
  int local_addr;

  local_addr = address | ((febid & 0xf) << 8);

  read_vme(&local_reg,addr_conn_out);
  clear_bit(&local_reg,1);
  write_vme(local_reg,addr_conn_out);
  set_bit(&local_reg,1);
  write_vme(local_reg,addr_conn_out);
  set_bit(&local_reg,2);
  write_vme(local_reg,addr_conn_out);
  clock_bus_pulse();
  clock_bus_pulse();
  for (ibit=0; ibit<16; ibit++) {
    xbit = (local_addr >> (15-ibit)) & 0x1;
    if (xbit==1) {
      set_bit(&local_reg,2);
      write_vme(local_reg,addr_conn_out);
    } else {
      clear_bit(&local_reg,2);
      write_vme(local_reg,addr_conn_out);
    }
    clock_bus_pulse();
  }
  for (ibit=0; ibit<16; ibit++) {
    xbit = (data >> (15-ibit)) & 0x1;
    if (xbit==1) {
      set_bit(&local_reg,2);
      write_vme(local_reg,addr_conn_out);
    } else {
      clear_bit(&local_reg,2);
      write_vme(local_reg,addr_conn_out);
    }
    clock_bus_pulse();
  }
  clear_bit(&local_reg,1);
  write_vme(local_reg,addr_conn_out);
  clock_bus_pulse();
}


