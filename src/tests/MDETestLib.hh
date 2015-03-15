#ifndef _MDETESTLIB
#define _MDETESTLIB 1

#include "equipmentList_common.hh"
#include "MDEv2718.hh"
#include "MDEv1290.hh"
#include "MDEv1731.hh"
#include "MDEv1724.hh"
#include "MDEv830.hh"
#include "MDEv977.hh"
#include "MDEv1495.hh"
#include "MDEvRB.hh"
#include "MDEvCB.hh"
#include "MDEDBB.hh"

#include "MiceDAQMessanger.hh"
#include "MiceDAQRunStats.hh"
#include "MiceDAQSpillStats.hh"

bool testV1495(int ba);
bool testV1290(int ba);
bool testV977(int ba, int duration);
bool testV830(int ba);
bool testV1724(int ba);
bool testV1731(int ba);
bool testVRB(int ba);
bool testVCB(int ba);

bool testmiceacq14();
bool testmiceacq20();
#endif



