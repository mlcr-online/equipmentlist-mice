/***************************************************************************
 *                                                                         *                                                                   *
 * Generic equipmentList file for MICE                                     *
 * Based on equipmentList_TEST provided by DATE                            *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/
#include "DATEIncludes.h"
#include "equipmentList_common.h"

int checkPartTriggerCount(struct eventHeaderStruct *header, unsigned long32 nPartEvt ) {
  unsigned long32 attribute,nPartEvtAttr;

  if ( nPartEvt==0 ) nPartEvtAttr = 0xFFFFFFFF;
  else nPartEvtAttr = nPartEvt;
  attribute = header->eventTypeAttribute[FIRST_USER_ATTRIBUTE];
  if ( attribute == 0 ) {
    header->eventTypeAttribute[FIRST_USER_ATTRIBUTE] = nPartEvtAttr;
    return 0;
  } else if ( attribute != nPartEvtAttr ) {
    //DO_ERROR{
      //sprintf( message, "Error: Number of particle event mismatch: (%d) != (%d)", attribute, nPartEvtAttr );
      //INFO( message );
    //}
    return abs((long)(nPartEvtAttr-attribute));
  }
  return 0;
}


#ifdef __cplusplus
 extern "C" {
#endif

#define DESCRIPTION "DATE equipment list"
char equipmentListIdent[]="@(#)""" __FILE__ """: """ DESCRIPTION \
""" """ DATE_VERSION """ compiled """ __DATE__ """ """ __TIME__;

/**************** DATE Includes ******************************/
#include "readList_equipment.h"

#ifdef __cplusplus
}
#endif

#include "DATETestExample.h"
#include "V2718.h"
#include "TriggerReceiver.h"
#include "V1290.h"
#include "V830.h"
#include "V1724.h"
#include "V1731.h"
#include "Trailer.h"
#include "EpicsClient.h"
#include "PartTriggerSelector.h"
#include "VLSBMaster.h"
#include "VLSB.h"
#include "VLSBBank.h"
#include "VLSBCosmicTrgManager.h"
#include "VRB.h"
#include "DBB.h"

using namespace std;

double lReadWriteTimeout(1.0);
double lMonitorTimeout(1.0);

using namespace MICE;


DATEStatusServerDescription lServerDescription;
DATEStatusServerDescriptionXmlReader lDescriptionReader;
DATEStatusClient *lDATEStatusClient;
std::vector<PVClient *> gEPICSClients;
DATEPVDescription *lDescription;
bool EPICSClientsConnected = false;

unsigned int nBytsRecirded = 0;
char message[512];
int maxNevents(149);

/*******************************************************************
* equipmentDescrTable is an array shared with readList_equipment
* which contains the description of each type of equipment and
* the references to the functions used to handle them.
********************************************************************/

/* PLEASE UPDATE equipmentDescrTable[] in equipmentDump.c accordingly */
 
equipmentDescrType equipmentDescrTable[] = {
  EQUIPMENTDESCR( Rand),
  EQUIPMENTDESCR( ErrGen),
  EQUIPMENTDESCR( CalBurstGen),
  EQUIPMENTDESCR( Timer),
  EQUIPMENTDESCR( V2718 ),
  EQUIPMENTDESCR( TriggerReceiver ),
  EQUIPMENTDESCR( V1290 ),
  EQUIPMENTDESCR( V1290N ),
  EQUIPMENTDESCR( V830 ),
  EQUIPMENTDESCR( V1724 ),
  EQUIPMENTDESCR( V1731 ),
  EQUIPMENTDESCR( Trailer ),
  EQUIPMENTDESCR( EpicsClient ),
  EQUIPMENTDESCR( PartTriggerSelector ),
  EQUIPMENTDESCR( VLSBMaster ),
  EQUIPMENTDESCR( VLSB ),
  EQUIPMENTDESCR( VLSBBank ),
  EQUIPMENTDESCR( VLSBCosmicTrgManager ),
  EQUIPMENTDESCR( VRB ),
  EQUIPMENTDESCR( DBB )
};

int nbEquipDescr = sizeof( equipmentDescrTable) / sizeof( equipmentDescrTable[0]);

