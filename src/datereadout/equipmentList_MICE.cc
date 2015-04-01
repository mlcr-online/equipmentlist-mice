/***************************************************************************
 *                                                                         *                                                                   *
 * Generic equipmentList file for MICE                                     *
 * Based on equipmentList_TEST provided by DATE                            *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#include "equipmentList_common.hh"

#ifdef __cplusplus
 extern "C" {
#endif

#define DESCRIPTION "DATE equipment list"
char equipmentListIdent[]="@(#)""" __FILE__ """: """ DESCRIPTION \
""" """ DATE_VERSION """ compiled """ __DATE__ """ """ __TIME__;

/**************** DATE Includes ******************************/
#include "readList_equipment.h"
#include "DATEIncludes.hh"

#ifdef __cplusplus
}
#endif

#include "DATETestExample.hh"
#include "V2718.hh"
#include "TriggerReceiver.hh"
#include "Trailer.hh"
#include "EpicsInterface.hh"
#include "PartTriggerSelector.hh"
#include "V830.hh"
#include "V1290.hh"
#include "V1724.hh"
#include "V1731.hh"
#include "V1495.hh"
#include "VRB.hh"
#include "VCB.hh"
#include "DBB.hh"
#include "DBBChain.hh"

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
  EQUIPMENTDESCR( Trailer ),
  EQUIPMENTDESCR( EpicsClient ),
  EQUIPMENTDESCR( PartTriggerSelector ),
  EQUIPMENTDESCR( V830 ),
  EQUIPMENTDESCR( V1290 ),/*
  EQUIPMENTDESCR( V1290N ),*/
  EQUIPMENTDESCR( V1724 ),
  EQUIPMENTDESCR( V1731 ),/*,
  EQUIPMENTDESCR( VLSBMaster ),
  EQUIPMENTDESCR( VLSB ),
  EQUIPMENTDESCR( VLSBBank ),
  EQUIPMENTDESCR( VLSBCosmicTrgManager ),*/
  EQUIPMENTDESCR( V1495 ),
  EQUIPMENTDESCR( VRB ),
  EQUIPMENTDESCR( VCB ),
  EQUIPMENTDESCR( DBB ),
  EQUIPMENTDESCR( DBBChain )
};

int nbEquipDescr = sizeof( equipmentDescrTable) / sizeof( equipmentDescrTable[0]);

