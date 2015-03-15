/***************************************************************************
 *                                                                         *
 * $Log: V830Def.h,v $
 * Revision 1.2  2007/07/20 17:01:02  daq
 * *** empty log message ***
 *
 * Revision 1.1  2007/07/20 17:00:11  daq
 * Initial revision
 *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#ifndef _V830DEF_H_
#define _V830DEF_H_

typedef enum  V830TriggerMode {
  V830_TriggerDisabled        = 0x0,
  V830_TriggerRandom          = 0x1,
  V830_TriggerPeriodical      = 0x2
} V830TriggerMode;

typedef enum  V830ControlMask {
  V830_ControlAcqModeLow      = 0x01,
  V830_ControlAcqModeHigh     = 0x02,
  V830_ControlDataFormat      = 0x04,
  V830_ControlTestMode        = 0x08,
  V830_ControlBerrEnable      = 0x10,
  V830_ControlHeaderEnable    = 0x20,
  V830_ControlClearMeb        = 0x40,
  V830_ControlAutoReset       = 0x80
} V830ControlMask;

typedef enum V830StatusMask {
  V830_DReady                 = 0x01,
  V830_AlmostFull             = 0x02,
  V830_Full                   = 0x04,
  V830_GlobalDReady           = 0x08,
  V830_GlobalBusy             = 0x10,
  V830_TermOn                 = 0x20,
  V830_TermOff                = 0x40,
  V830_BErrFlag               = 0x80
} V830StatusMask;

/* Register addresses */
#define V830_MEB                     0x0000
#define V830_COUNTER                 0x1000
#define V830_TESTREG                 0x1080
#define V830_CHANNEL_ENABLE          0x1100
#define V830_DWELL_TIME              0x1104
#define V830_CONTROL_REGISTER        0x1108
#define V830_CONTROL_BIT_SET         0x110A
#define V830_CONTROL_BIT_CLEAR       0x110C
#define V830_STATUS_REGISTER         0x110E
#define V830_GEO_REGISTER            0x1110
#define V830_MODULE_RESET            0x1120
#define V830_SOFTWARE_CLEAR          0x1122
#define V830_SOFTWARE_TRIGGER        0x1124
#define V830_TRIGGER_COUNTER         0x1128
#define V830_BLT_EVENT_NUMBER        0x1130
#define V830_MEB_EVENT_NUMBER        0x1134

#endif

