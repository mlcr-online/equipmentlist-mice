#ifndef _MDEV513N
#define _MDEV513N 1


#define V513Params(FIRST, NEXT) \
FIRST(BaseAddress) \
NEXT(STB_NegPolarity) \
NEXT(IRQ_Enable) \
NEXT(IRQ_Level) \
NEXT(GEO)

#include "MiceDAQEquipment.hh"
#include <vector>

class MDEv513 : public MiceDAQEquipment {
public:
  MDEv513();
  ~MDEv513() {}

  bool Arm();
  bool DisArm();

  bool BoardReset();
  bool SetupStrobe();
  bool ReadSTBReg();
  bool STBBitClear();
  bool SetIRQLevel();
  bool SetIRQVector();
  bool IRQClear();

  IMPLEMENT_PARAMS(V513Params, MDEv513)
};

#endif
