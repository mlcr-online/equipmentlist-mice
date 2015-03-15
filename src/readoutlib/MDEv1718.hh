#ifndef _MDEV1718
#define _MDEV1718 1

#include "MDEv2718.hh"

class MDEv1718 : public MDEv2718 {
 public:
  MDEv1718();
  ~MDEv1718() {}

  bool Arm();

 private:
  bool Init();
};

#endif


