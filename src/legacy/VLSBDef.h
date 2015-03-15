#ifndef _VLSBDEF_H_
#define _VLSBDEF_H_

#define VLSB_WORD_SIZE                   4 // bytes
#define VLSB_N_BANKS                     4 
#define VLSB_BANK_LENGTH                 0x20000 // words (word = 4 bytes)

#define VLSB_BANK_OFFSET                 0x100000

#define VLSBMASTER_PERIOD    18.8  // nano seconds
#define VLSBMASTER_TIME_UNIT 1000  // nano seconds

typedef enum  VLSBControlBit {
  VLSB_FastClear                        = 0x01,
  VLSB_EndOfSpill                       = 0x04, // TODO rename this with the proper definition
  VLSB_ContSpill                        = 0x10,
  VLSB_TriggerMode                      = 0x20,
  VLSB_TriggerInternal                  = 0x44,
  VLSB_NoZeroSuppress			= 0x40,
  VLSB_CosmicTrigger			= 0xF4
} VLSBControlBit;

typedef enum  VLSBTriggerSetting {
  VLSB_Trigger_Disable                  = 0x0000,
  VLSB_Trigger_Generic                  = 0xF002,
  VLSB_Trigger_Data                     = 0x40 
} VLSBTriggerSetting;


//**************** Registers **********************

#define VLSB_EVENT_LENGTH_0                  0x0000 
#define VLSB_EVENT_LENGTH_1                  0x0004 
#define VLSB_EVENT_LENGTH_2                  0x0008 
#define VLSB_EVENT_LENGTH_3                  0x000C 

#define VLSB_CONTROL                         0x0010
#define VLSB_SPILL_DURATION                  0x0014 
#define VLSB_TRIGGER_SETTINGS                0x0018
#define VLSB_VETO_SETTINGS                   0x001C

#define VLSB_TRIGGER_DATA_MASK               0x0020
#define VLSB_TRIGGER_CONTROL                 0x0024
#define VLSB_MODULE_INFO                     0x0028
#define VLSB_INPUT_FIFO_STATUS               0x002C // now monitoring register
#define VLSB_MMODE_DATA                      0x0030
#define VLSB_MONITOR_STAT                    0x0034
#define VLSB_MONITOR_STAT_HIST               0x0038
#define VLSB_TRIGGER_NUM                     0x0020
#define VLSB_BANK_BASE_0                     0x1000000
#define VLSB_BANK_BASE_1                     0x1100000
#define VLSB_BANK_BASE_2                     0x1200000
#define VLSB_BANK_BASE_3                     0x1300000


#endif
