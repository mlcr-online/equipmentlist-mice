
/***************************************************************************
 *                                                                         *
 * $Log: V1731Def.h,v $
 *                                                                         *
 * equipmentList functions for the CAEN fADC V1731                         *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov July 2007          *
 *                                                                         *
 ***************************************************************************/

#ifndef _V1290DEF_H_
#define _V1290DEF_H_

typedef enum  V1731ChannelConfigBit {
  V1731_TriggerOverlapping             = 0x0002,
  V1731_TestPatternGeneration          = 0x0008,
  V1731_MemorySequentialAccess         = 0x0010,
  V1731_TriggerOnFallingEdge           = 0x0040,
  V1731_DualEdgeSampling               = 0x1000
} V1731ChannelConfigBit;

typedef enum  V1731ChannelADCConfigBit {
  V1731_ADCPowerDown                   = 0x0001,
  V1731_ADCCalibration                 = 0x0002
} V1731ChannelADCConfigBit;

typedef enum  V1731ZeroSuppression {
  V1731_ZeroSuppr_None                 = 0x00000,
  V1731_ZeroSuppr_ZLE                  = 0x20000,
  V1731_ZeroSuppr_ZS_AMP               = 0x30000,
} V1731ZeroSuppression;

typedef enum  V1731ChannelStatusBit {
  V1731_MemoryFull                     = 0x01,
  V1731_MemoryEmpty                    = 0x02,
  V1731_DACBusy                        = 0x04,
  V1731_ChEnabled                      = 0x08,
  V1731_NextChannelEnabled             = 0x10,
  V1731_BufferFreeError                = 0x20,
  V1731_CalibrationDone                = 0x40
} V1731ChannelStatusBit;

typedef enum  V1731AcqControlBit {
  V1731_S_INControledlMode             = 0x01,
  V1731_S_INGateMode                   = 0x02,
  V1731_MultiBoardSyncMode             = 0x03,
  V1731_AcquisitonRun                  = 0x04,
  V1731_CountAllTriggers               = 0x08,
} V1731AcqControlBit;

typedef enum  V1731AcqStatusBit {
  V1731_RunOn                          = 0x004,
  V1731_EventReady                     = 0x008,
  V1731_EventFull                      = 0x010,
  V1731_ExternalClock                  = 0x020,
  V1731_BypassPLL                      = 0x040,
  V1731_PLLLock                        = 0x080,
  V1731_BoardReady                     = 0x100
} V1731AcqStatusBit;

typedef enum  V1731VMEControlBit {
  V1731_InterruptLevel                 = 0x07,
  V1731_OpticalLinkInterrupt           = 0x08,
  V1731_BERR                           = 0x10,
  V1731_ALIGN64                        = 0x20,
  V1731_RELOC                          = 0x40,
  V1731_ROAK                           = 0x80
} V1731VMEControlBit;

typedef enum  V1731VMEStatusBit {
  V1731_DataReady                      = 0x01,
  V1731_BERRFlag                       = 0x04
} V1731VMEStatusBit;

typedef enum  V1731BufferNumber {
  V1731_Buffer_Number_1                = 0x0000,
  V1731_Buffer_Number_2                = 0x0001,
  V1731_Buffer_Number_4                = 0x0002,
  V1731_Buffer_Number_8                = 0x0003,
  V1731_Buffer_Number_16               = 0x0004,
  V1731_Buffer_Number_32               = 0x0005,
  V1731_Buffer_Number_64               = 0x0006,
  V1731_Buffer_Number_128              = 0x0007,
  V1731_Buffer_Number_256              = 0x0008,
  V1731_Buffer_Number_512              = 0x0009,
  V1731_Buffer_Number_1024             = 0x000A
} V1731BufferNumber;


typedef enum  V1731TriggerSourceBit {
  V1731_TriggerSourceChannel_0         = 0x00000001,
  V1731_TriggerSourceChannel_1         = 0x00000002,
  V1731_TriggerSourceChannel_2         = 0x00000004,
  V1731_TriggerSourceChannel_3         = 0x00000008,
  V1731_TriggerSourceChannel_4         = 0x00000010,
  V1731_TriggerSourceChannel_5         = 0x00000020,
  V1731_TriggerSourceChannel_6         = 0x00000040,
  V1731_TriggerSourceChannel_7         = 0x00000080,
  V1731_TriggerSourceExternal          = 0x40000000,
  V1731_TriggerSourceSoftware          = 0x80000000
} V1731TriggerSourceBit;

#define V1731_CHANNELS_NUMBER                   8

#define V1731_READOUT_BUFFER_SIZE               0x1000

/* Register addresses */
#define V1731_EVENT_READOUT_BUFFER              0x0000
#define V1731_CHANNEL_ZS_THRES                  0x1024
#define V1731_CHANNEL_ZS_NSAMP                  0x1028
#define V1731_CHANNEL_THRESHOLD                 0x1080
#define V1731_CHANNEL_THRESHOLD                 0x1080
#define V1731_CHANNEL_TIME_OU_THRESHOLD         0x1084
#define V1731_CHANNEL_STATUS                    0x1088
#define V1731_CHANNEL_AMC_FPGA_FW_REV           0x1088
#define V1731_CHANNEL_BUFFER_OCCPANCY           0x1094
#define V1731_CHANNEL_DAC                       0x1098
#define V1731_CHANNEL_ADC_CONFIG                0x109C
#define V1731_CHANNEL_CONFIG                    0x8000
#define V1731_CHANNEL_CONFIG_BIT_SET            0x8004
#define V1731_CHANNEL_CONFIG_BIT_CLEAR          0x8008
#define V1731_BUFFER_ORGANIZATION               0x800C
#define V1731_BUFFER_FREE                       0x8010
#define V1731_BUFFER_CUSTOM_SIZE                0x8020
#define V1731_ACQ_CONTROL                       0x8100
#define V1731_ACQ_STATUS                        0x8104
#define V1731_SW_TRIGGER                        0x8108
#define V1731_TRIGGER_SOURCE_ENABLE_MASK        0x810C
#define V1731_FRONT_PANEL_TRG_OUT_ENABLE_MASK   0x8110
#define V1731_POST_TRIGGER_SETTING              0x8114
#define V1731_FRONT_PANEL_IO_DATA               0x8118
#define V1731_FRONT_PANEL_IO_CONTROL            0x811C
#define V1731_CHANNEL_ENABLE                    0x8120
#define V1731_CHANNEL_ROC_FPGA_FW_REV           0x8124
#define V1731_EVENT_STORED                      0x812C
#define V1731_SET_MONITOR_DAC                   0x8138
#define V1731_BOARD_INFO                        0x8140
#define V1731_MONITOR_MODE                      0x8144
#define V1731_VME_CONTROL                       0xEF00
#define V1731_VME_STATUS                        0xEF04
#define V1731_BOARD_ID                          0xEF08
#define V1731_BLT_EVENT_NUMBER                  0xEF1C
#define V1731_SCRATCH                           0xEF20
#define V1731_SW_RESET                          0xEF24
#define V1731_SW_CLEAR                          0xEF28

#define V1731_USER_ZS                        0x7000000

#endif


