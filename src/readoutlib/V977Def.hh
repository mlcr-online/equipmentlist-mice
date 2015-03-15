/***************************************************************************
 *                                                                         *
 * $Log: V977Def.h,v $
 * Revision 1.1  2007/06/08 17:17:34  daq
 * Initial revision
 *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov June 2007          *
 *                                                                         *
 ***************************************************************************/

#ifndef _V977DEF_H_
#define _V977DEF_H_

typedef enum  V977Channels {
  V977_CH00  = 0x0001,
  V977_CH01  = 0x0002,
  V977_CH02  = 0x0004,
  V977_CH03  = 0x0008,
  V977_CH04  = 0x0010,
  V977_CH05  = 0x0020,
  V977_CH06  = 0x0040,
  V977_CH07  = 0x0080,
  V977_CH08  = 0x0100,
  V977_CH09  = 0x0200,
  V977_CH10  = 0x0400,
  V977_CH11  = 0x0800,
  V977_CH12  = 0x1000,
  V977_CH13  = 0x2000,
  V977_CH14  = 0x4000,
  V977_CH15  = 0x8000

}  V977Channels;

//**************** Registers **********************

#define AM_A24_NON_PRIVILIGED           0x39
#define AM_A24_SUPERVISORY              0x3D
#define AM_A32_NON_PRIVILIGED           0x09
#define AM_A32_SUPERVISORY              0x0D

#define V977_INPUT_SET                  0x0000 // A24/A32 D16 R/W
#define V977_INPUT_MASK                 0x0002 // A24/A32 D16 R/W
#define V977_INPUT_READ                 0x0004 // A24/A32 D16 RO
#define V977_SINGLEHIT_READ             0x0006 // A24/A32 D16 RO
#define V977_MULTIHIT_READ              0x0008 // A24/A32 D16 RO
#define V977_OUTPUT_SET                 0x000A // A24/A32 D16 R/W
#define V977_OUTPUT_MASK                0x000C // A24/A32 D16 R/W
#define V977_INTERRUPT_MASK             0x000E // A24/A32 D16 R/W
#define V977_CLEAR_OUTPUT               0x0010 // A24/A32 D16 RO
#define V977_SINGLEHIT_READ_CLEAR       0x0016 // A24/A32 D16 RO
#define V977_MULTIHIT_READ_CLEAR        0x0018 // A24/A32 D16 RO
#define V977_TEST_CONTROL_REGISTER      0x001A // A24/A32 D16 R/W
#define V977_INTERRUPT_LEVEL            0x0020 // A24/A32 D16 R/W
#define V977_INTERRUPT_VECTOR           0x0022 // A24/A32 D16 R/W
#define V977_SERIAL_NUMBER              0x0024 // A24/A32 D16 RO
#define V977_FIRMWARE_VERSION           0x0026 // A24/A32 D16 RO
#define V977_CONTROL_REGISTER           0x0028 // A24/A32 D16 R/W
#define V977_DUMMY_REGISTER             0x002A // A24/A32 D16 R/W
#define V977_SOFTWARE_RESET             0x002E // A24/A32 D16 WO

typedef struct {
    short *TriggerPattern;
    short *EquipmentId;
} V977_ParType;

//  TriggerReceiver
typedef enum  TriggerReceiverChannelBit {
  TR_PHYSICS_EVENT_REQUEST     = 0x01,
  TR_PHYSICS_EVENT_BUSY        = 0x02,
  TR_CALIBRATION_EVENT_REQUEST = 0x04,
  TR_CALIBRATION_EVENT_BUSY    = 0x08,
  TR_SOS_EVENT_REQUEST         = 0x10,
  TR_SOS_EVENT_BUSY            = 0x20,
  TR_EOS_EVENT_REQUEST         = 0x40,
  TR_EOS_EVENT_BUSY            = 0x80
} TriggerReceiverChannelBit;

typedef enum  TriggerReceiverMask {
  TR_TRIGGER_MASK        = 0x00FF,
  TR_TRIGGER_REQUEST     = 0x0055,
  TR_TRIGGER_ACK         = 0x0055,
  TR_TRIGGER_RECEIVED    = 0x00aa,
  TR_TRIGGER_BUSY        = 0x00aa,
  TR_PHYSICS_EVENT       = 0x0003,
  TR_CALIBRATION_EVENT   = 0x000c,
  TR_SOS_EVENT           = 0x0030,
  TR_EOS_EVENT           = 0x00c0,
  TR_VALID_PHYSICS_EVENT = 0x0033, // A physics event is only valid if it arrives after SOS
  TR_VALID_EOS_EVENT     = 0x00F3  // A EOS event is only valid if it arrives after SOS and Physics event
} TriggerReceiverMask;

typedef enum  PartTriggerSelectorChannelBit {
  PTS_GVA1   = 0x100,
  PTS_TOF0   = 0x200,
  PTS_TOF1   = 0x400,
  PTS_PULSER = 0x800
} PartTriggerSelectorChannelBit ;

#endif



