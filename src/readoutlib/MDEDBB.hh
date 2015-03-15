#ifndef _MDE_DBB_
#define _MDE_DBB_ 1


#define DBBParams(FIRST, NEXT) \
  FIRST(GEO) \
  NEXT(BaseAddressVRB) \
  NEXT(IdInVRB)

#include "MiceDAQEquipment.hh"
#include "MDEvRB.hh"

class MDEDBB : public MiceDAQEquipment {
 public:
  MDEDBB();
  ~MDEDBB() {}

  bool Arm();
  bool DisArm();

  int ReadEvent();
  int EventArrived();

//   void setDataPtr(MDE_Pointer *p);

  int getNumDataWords();

 private:

  MDEvRB vrb_;

  IMPLEMENT_PARAMS(DBBParams, MDEDBB)
};

#endif

