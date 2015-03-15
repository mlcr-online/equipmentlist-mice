
/***************************************************************************
 *                                                                         *
 * $Log: V1724Def.h,v $
 *                                                                         *
 * equipmentList functions for the CAEN fADC V1724                         *
 *                                                                         *
 * Originally created by J.S. Graulich and V. Verguilov July 2007          *
 *                                                                         *
 ***************************************************************************/
#ifndef _V1290DEF_H_
#define _V1290DEF_H_

typedef enum  V1724ChannelConfigBit {
  V1724_SingleShotGate                 = 0x01,
  V1724_TriggerOverlapping             = 0x02,
  V1724_TestPatternGeneration          = 0x08,
  V1724_MemorySequentialAccess         = 0x10,
  V1724_TriggerOnFallingEdge           = 0x40,
  V1724_AnalogMonitor                  = 0x80
} V1724ChannelConfigBit;

typedef enum  V1724ChannelStatusBit {
  V1724_MemoryFull                     = 0x01,
  V1724_MemoryEmpty                    = 0x02,
  V1724_DACBusy                        = 0x04,
  V1724_ChEnabled                      = 0x08,
  V1724_NextChannelEnabled             = 0x10,
  V1724_BufferFreeError                = 0x20
} V1724ChannelStatusBit;

typedef enum  V1724AcqControlBit {
  V1724_AcquisitonRun                  = 0x04,
  V1724_CountAllTriggers               = 0x08,
  V1724_DownSampling                   = 0x10,
  V1724_AlwaysKeepOneBufferFree        = 0x20
} V1724AcqControlBit;

typedef enum  V1724AcqStatusBit {
  V1724_RunOn                          = 0x004,
  V1724_EventReady                     = 0x008,
  V1724_EventFull                      = 0x010,
  V1724_ExternalClock                  = 0x020,
  V1724_BypassPLL                      = 0x040,
  V1724_PLLLock                        = 0x080,
  V1724_BoardReady                     = 0x100
} V1724AcqStatusBit;

typedef enum  V1724VMEControlBit {
  V1724_InterruptLevel                 = 0x07,
  V1724_OpticalLinkInterrupt           = 0x08,
  V1724_BERR                           = 0x10,
  V1724_ALIGN64                        = 0x20,
  V1724_RELOC                          = 0x40,
  V1724_ROAK                           = 0x80
} V1724VMEControlBit;

typedef enum  V1724VMEStatusBit {
  V1724_DataReady                      = 0x01,
  V1724_BERRFlag                       = 0x04
} V1724VMEStatusBit;

typedef enum  V1724OutputBufferSize {
  V1724_OutputBufferSize_1024K	       = 0x0000,
  V1724_OutputBufferSize_512K          = 0x0001,
  V1724_OutputBufferSize_256K          = 0x0002,
  V1724_OutputBufferSize_128K          = 0x0003,
  V1724_OutputBufferSize_64K           = 0x0004,
  V1724_OutputBufferSize_32K           = 0x0005,
  V1724_OutputBufferSize_16K           = 0x0006,
  V1724_OutputBufferSize_8K            = 0x0007,
  V1724_OutputBufferSize_4K            = 0x0008,
  V1724_OutputBufferSize_2K            = 0x0009,
  V1724_OutputBufferSize_1K            = 0x000A
} V1724OutputBufferSize;

typedef enum  V1724TriggerSourceBit {
  V1724_TriggerSourceChannel_0         = 0x00000001,
  V1724_TriggerSourceChannel_1         = 0x00000002,
  V1724_TriggerSourceChannel_2         = 0x00000004,
  V1724_TriggerSourceChannel_3         = 0x00000008,
  V1724_TriggerSourceChannel_4         = 0x00000010,
  V1724_TriggerSourceChannel_5         = 0x00000020,
  V1724_TriggerSourceChannel_6         = 0x00000040,
  V1724_TriggerSourceChannel_7         = 0x00000080,
  V1724_TriggerSourceExternal          = 0x40000000,
  V1724_TriggerSourceSoftware          = 0x80000000
} V1724TriggerSourceBit;

#define V1724_CHANNELS_NUMBER               8

#define V1724_READOUT_BUFFER_SIZE           0x1000

#define V1724_BUFFER_OUTPUT_SIZE_1024K      0x0000
#define V1724_BUFFER_OUTPUT_SIZE_512K       0x0001
#define V1724_BUFFER_OUTPUT_SIZE_256K       0x0002
#define V1724_BUFFER_OUTPUT_SIZE_128K       0x0003
#define V1724_BUFFER_OUTPUT_SIZE_64K        0x0004
#define V1724_BUFFER_OUTPUT_SIZE_32K        0x0005
#define V1724_BUFFER_OUTPUT_SIZE_16K        0x0006
#define V1724_BUFFER_OUTPUT_SIZE_8K         0x0007
#define V1724_BUFFER_OUTPUT_SIZE_4K         0x0008
#define V1724_BUFFER_OUTPUT_SIZE_2K         0x0009
#define V1724_BUFFER_OUTPUT_SIZE_1K         0x000A

/* Register addresses */
#define V1724_EVENT_READOUT_BUFFER          0x0000
#define V1724_CHANNEL_THRESHOLD             0x1080
#define V1724_CHANNEL_TIME_OU_THRESHOLD     0x1084
#define V1724_CHANNEL_STATUS                0x1088
#define V1724_CHANNEL_FIRMWARE_REVISION     0x108C
#define V1724_CHANNEL_BUFFER_OCCPANCY       0x1094
#define V1724_CHANNEL_DAC                   0x1098
#define V1724_CHANNEL_ADC_CONFIG            0x109C
#define V1724_CHANNEL_CONFIG                0x8000
#define V1724_CHANNEL_CONFIG_BIT_SET        0x8004
#define V1724_CHANNEL_CONFIG_BIT_CLEAR      0x8008
#define V1724_BUFFER_ORGANIZATION           0x800C
#define V1724_BUFFER_FREE                   0x8010
#define V1724_BUFFER_CUSTOM_SIZE            0x8020
#define V1724_ACQ_CONTROL                   0x8100
#define V1724_ACQ_STATUS                    0x8104
#define V1724_SW_TRIGGER                    0x8108
#define V1724_TRIGGER_SOURCE_ENABLE_MASK    0x810C
#define V1724_POST_TRIGGER_SETTING          0x8114
#define V1724_IO_CONTROL                    0x811C
#define V1724_CHANNEL_ENABLE                0x8120
#define V1724_ROC_FIRMWARE_REVISION         0x8124
#define V1724_DOWNSAMPLE_FACTOR             0x8128
#define V1724_EVENT_STORED                  0x812C
#define V1724_SET_MONITOR_DAC               0x8138
#define V1724_BOARD_INFO                    0x8140
#define V1724_MONITOR_MODE                  0x8144
#define V1724_VME_CONTROL                   0xEF00
#define V1724_VME_STATUS                    0xEF04
#define V1724_BOARD_ID                      0xEF08
#define V1724_BLT_EVENT_NUMBER              0xEF1C
#define V1724_SCRATCH                       0xEF20
#define V1724_SW_RESET                      0xEF24
#define V1724_SW_CLEAR                      0xEF28

#define V1724_USER_ZS                    0x7000000
#endif


