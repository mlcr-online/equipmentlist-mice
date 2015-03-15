#ifndef _DBB_CHAIN_H_
#define _DBB_CHAIN_H_

#include "DATEIncludes.hh"
#include "equipmentList_common.hh"

#include "VRBDef.hh"
#include "MDEvRB.hh"
#include "MDEDBB.hh"

typedef struct {
  char   * VRBBaseAddress;
  long32 * FirstDbbGEO;
  long32 * nDBBs;
} DBBChain_ParType;

void ArmDBBChain( char * );
void DisArmDBBChain( char * );
void AsynchReadDBBChain( char * );
int ReadEventDBBChain( char *, struct eventHeaderStruct *, struct equipmentHeaderStruct *, datePointer *);
int EventArrivedDBBChain( char * );

void PauseArmDBBChain( char *);
void PauseDisArmDBBChain( char *);

#endif

