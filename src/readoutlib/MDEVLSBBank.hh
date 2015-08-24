#ifndef _MDEVLSBBANK
#define _MDEVLSBBANK 1


#define VLSBBankParams(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddress) \
  NEXT(VLSBid) \
  NEXT(VLSBBank)

#include "MiceDAQEquipment.hh"


class MDEVLSBBank : public MiceDAQEquipment {

 public:
  MDEVLSBBank();
  ~MDEVLSBBank() {}

  bool Arm();
  int ReadEvent();

  // Bank IO:
  int ReadBank();
  int ReadFIFOStatus();
  uint32_t ReadBankLength();
  bool ReadBankData(MDE_Pointer *dataPtr, uint32_t BankLength);

  // Read the status of the FIFO, after the data was readout.
  bool FIFOOK();

 private:

  IMPLEMENT_PARAMS(VLSBBankParams, MDEVLSBBank)

};

#endif
