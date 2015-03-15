/***************************************************************************
 *                                                                         *
 * $Log: V1290Def.h,v $
 * Revision 1.1  2007/07/20 16:59:34  daq
 * Initial revision
 *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#ifndef _V1290DEF_H_
#define _V1290DEF_H_

#define V1290_DATAWORDSIZE      4      // data word size in Bytes
#define V1290_READOUTWORDSIZE   4      // readout data word size in Bytes

typedef enum  V1290ModuleType {
  V1290_V1290A                  = 0x0001,
  V1290_V1290N                  = 0x0002,
  V1290_UNKNOWN_MODULE          = 0xFFFF
} V1290ModuleType ;


typedef enum  V1290ControlBit {
  V1290_ControlBerr             = 0x0001,
  V1290_ControlTerm             = 0x0002,
  V1290_ControlTermSW           = 0x0004,
  V1290_ControlEmptyEvent       = 0x0008,
  V1290_ControlAlign64          = 0x0010,
  V1290_ControlCompensation     = 0x0020,
  V1290_ControlTestFifo         = 0x0040,
  V1290_ControlReadCompensation = 0x0080,
  V1290_ControlEventFifo        = 0x0100,
  V1290_ControlTriggerTimeTag   = 0x0200
} V1290ControlBit ;

typedef enum  V1290StatusBit {
  V1290_StatusDataReady         = 0x0001,
  V1290_StatusAlmostFull        = 0x0002,
  V1290_StatusFull              = 0x0004,
  V1290_StatusTriggerMatching   = 0x0008,
  V1290_StatusTdcHeader         = 0x0010,
  V1290_StatusTermOn            = 0x0020,
  V1290_StatusErrorTdc0         = 0x0040,
  V1290_StatusErrorTdc1         = 0x0080,
  V1290_StatusErrorTdc2         = 0x0100,
  V1290_StatusErrorTdc3         = 0x0200,
  V1290_StatusBusErrorFlag      = 0x0400,
  V1290_StatusPurged            = 0x0800,
  V1290_StatusResolutionMask    = 0x3000,
  V1290_StatusPairMode          = 0x4000,
  V1290_StatusTriggerLost       = 0x8000
} V1290StatusBit ;

typedef enum  V1290EdgeDet {
  V1290_Pair                 = 0x0,
  V1290_Trailing             = 0x1,
  V1290_Leading              = 0x2,
  V1290_TrailingAndLeading   = 0x3
} V1290EdgeDet ; 

typedef enum  V1290HandShake {
  V1290_MicroWrite         = 0x1,
  V1290_MicroRead          = 0x2,
  V1290_MicroRWMask        = 0x3
} V1290HandShake ; 

/* Register addresses */
#define V1290_OUTPUTBUFFER      0x0000
#define V1290_OUTPUTBUFFERLAST  0x0FFC
#define V1290_CONTROL           0x1000
#define V1290_STATUS            0x1002
#define V1290_INTERRUPTLEVEL    0x100A
#define V1290_INTERRUPTVECTOR   0x100C
#define V1290_GEO_REGISTER	0x100E
#define V1290_MODULERESET       0x1014
#define V1290_SOFTWARECLEAR     0x1016
#define V1290_SOFTWARETRIGGER   0x101A
#define V1290_EVENTCOUNTER      0x101C
#define V1290_EVENTSTORED       0x1020
#define V1290_ALMOSTFULLLEVEL   0x1022
#define V1290_BLTEVENTNUMBER    0x1024
#define V1290_FIRMWAREREVISION  0x1026
#define V1290_TESTREG           0x1028
#define V1290_OUTPUTPROGCONTROL 0x102C
#define V1290_MICRO             0x102E
#define V1290_MICROHANDSHAKE    0x1030
#define V1290_SELECTFLASH       0x1032
#define V1290_FLASHMEMORY       0x1034
#define V1290_SRAMPAGE          0x1036
#define V1290_EVENTFIFO         0x1038
#define V1290_EVENTFIFOSTORED   0x103C
#define V1290_EVENTFIFOSTATUS   0x103E
#define V1290_DUMMY32           0x1200
#define V1290_DUMMY16           0x1204
#define V1290_CONFIG_ROM        0x4000
#define V1290_COMPENSATIONSRAM  0x8000


/* Operating Codes */
#define V1290_TRG_MATCH               0x0000
#define V1290_CONT_STOR               0x0100
#define V1290_READ_ACQ_MODE           0x0200
#define V1290_LOAD_DEF_CONFIG         0x0500
#define V1290_SAVE_USER_CONFIG        0x0600
#define V1290_LOAD_USER_CONFIG        0x0700
#define V1290_AUTOLOAD_USER_CONFIG    0x0800
#define V1290_AUTOLOAD_DEF_CONFIG     0x0900
#define V1290_SET_WIN_WIDTH           0x1000
#define V1290_SET_WIN_OFFS            0x1100
#define V1290_SET_SW_MARGIN           0x1200
#define V1290_SET_REJ_MARGIN          0x1300
#define V1290_EN_SUB_TRG              0x1400
#define V1290_DIS_SUB_TRG             0x1500
#define V1290_READ_TRG_CONF           0x1600
#define V1290_SET_DETECTION           0x2200
#define V1290_READ_DETECTION          0x2300
#define V1290_SET_TR_LEAD_LSB         0x2400
#define V1290_SET_PAIR_RES            0x2500
#define V1290_READ_RES                0x2600
#define V1290_SET_DEAD_TIME           0x2800
#define V1290_READ_DEAD_TIME          0x2900
#define V1290_EN_HEAD                 0x3000
#define V1290_DIS_HEAD                0x3100
#define V1290_READ_HEAD               0x3200
#define V1290_READ_EVENT_SIZE         0x3400
#define V1290_READ_ERROR_TYPES        0x3A00
#define V1290_READ_FIFO_SIZE          0x3C00
#define V1290_EN_CHANNEL              0x4000
#define V1290_DIS_CHANNEL             0x4100
#define V1290_EN_ALL_CH               0x4200
#define V1290_DIS_ALL_CH              0x4300
#define V1290_WRITE_EN_PATTERN        0x4400
#define V1290_READ_EN_PATTERN         0x4500
#define V1290_READ_GLOB_OFFS          0x5000
#define V1290_READ_ADJUST_CH          0x5300
#define V1290_READ_RC_ADJ             0x5500
#define V1290_READ_TDC_ID             0x6000
#define V1290_READ_MICRO_REV          0x6100
#define V1290_READ_SETUP_REG          0x7100
#define V1290_READ_ERROR_STATUS       0x7400
#define V1290_READ_DLL_LOCK           0x7500
#define V1290_READ_STATUS_STREAM      0x7600
#define V1290_READ_REV_DATE_MICRO_FW  0xC200
#define V1290_READ_SETUP_SCANPATH     0xC900

#define V1290_PAGESIZE ( V1290_OUTPUTBUFFERLAST - V1290_OUTPUTBUFFER + V1290_DATAWORDSIZE )
#define V1290_PAGECAPACITY ( ( V1290_PAGESIZE ) / V1290_READOUTWORDSIZE )

#endif



