#ifndef _MDEVCB
#define _MDEVCB 1


#define VCBParams(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(N_FEBs)

#include "MiceDAQEquipment.hh"
// int debug;
// int FEBID;
// int outreg;           /* Output register */
// int addr_conn_out;    /* */
// int addr_conn_in;     /* */
// int mar_mask[829];    /* */
// int32_t BHandle;      /* */

class MDEvCB : public MiceDAQEquipment {
 public:
  MDEvCB();
  ~MDEvCB() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

  bool LoadMaskFromFile(std::string fname);
  void LoadMask(const char* mask);

  bool configureFEB(unsigned int feb_id);

 private:

  int encode_reg_mar(int febid);
  void clear_bit(int *dataptr, int mode_bit);
  void set_bit(int *dataptr, int mode_bit);
  void clock_bus_pulse();
  void regin_up();
  void regin_down();
  void clkreg_clk();
  void send_on_bus_mus(int febid, int address, int data);

  int mar_mask_[829];

  IMPLEMENT_PARAMS(VCBParams, MDEvCB)
};

#endif


