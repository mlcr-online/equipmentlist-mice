#ifndef _VLSBDEF_H_
#define _VLSBDEF_H_

// Definitions for VLSBs

/* Register addresses */
// see note by D.Adey: 
// http://micewww.pp.rl.ac.uk/documents/73

// Amount of data in each banks FIFO
#define VLSB_BANK0_LENGTH 0x00
#define VLSB_BANK1_LENGTH 0x04
#define VLSB_BANK2_LENGTH 0x08
#define VLSB_BANK3_LENGTH 0x0C

// Control registers
#define VLSB_TRIGGER_CTL 0x10
#define VLSB_SPILL_LENGTH 0x14
#define VLSB_N_GEN_TRIGGERS 0x18
#define VLSB_ACC_WINDOW 0x1C
#define VLSB_ENCODED_TRIGGERS 0x20
#define VLSB_DATA_ENABLE 0x24
#define VLSB_FIRMWARE_VERS 0x28
#define VLSB_FIFO_FLAGS 0x2C
#define VLSB_MEM_FULL 0x30
#define VLSB_TRIGGER_TDC 0x34

// Data Registers:
// Note these numbers are different from DA's note!
// I assume the note is wrong becuase these were extracted
// from working code!
#define VLSB_BANK0_DATA 0x1000000
#define VLSB_BANK1_DATA 0x1100000
#define VLSB_BANK2_DATA 0x1200000
#define VLSB_BANK3_DATA 0x1300000


// Register bits:

// For trigger control register - VLSB_TRIGGER_CTL
typedef enum  VLSBTriggerCtl {
  VLSB_FastClear                = 0x1,
  VLSB_FastClearDone            = 0x2,
  VLSB_SpillEnable              = 0x4,
  VLSB_SpillDone                = 0x8,
  VLSB_ContinuosSpill           = 0x10,
  VLSB_ExternalTrigger          = 0x20,
  VLSB_NoZeroSuppression        = 0x40,
  VLSB_AcceptanceAllwaysOpen    = 0x80
} VLSBTriggerCtl;

typedef enum  VLSBDataEnable {
  VLSB_LVDS_DISABLE = 0x00,
  VLSB_LVDS_ENABLE = 0x40
} VLSBDataEnable;


typedef enum  VLSBFIFOStatus {
  VLSB_Bank0FIFOEmpty           = 0x1,
  VLSB_Bank1FIFOEmpty           = 0x2,
  VLSB_Bank2FIFOEmpty           = 0x4,
  VLSB_Bank3FIFOEmpty           = 0x8,
  VLSB_Bank0FIFOFull            = 0x10,
  VLSB_Bank1FIFOFull            = 0x20,
  VLSB_Bank2FIFOFull            = 0x40,
  VLSB_Bank3FIFOFull            = 0x80,
  VLSB_Bank0DataTimeout         = 0x100,
  VLSB_Bank1DataTimeout         = 0x200,
  VLSB_Bank2DataTimeout         = 0x400,
  VLSB_Bank3DataTimeout         = 0x800,
  VLSB_Bank0CRCError            = 0x1000,
  VLSB_Bank1CRCError            = 0x2000,
  VLSB_Bank2CRCError            = 0x4000,
  VLSB_Bank3CRCError            = 0x8000,
  VLSB_Bank0ClockPhaseError     = 0x10000,
  VLSB_Bank1ClockPhaseError     = 0x20000,
  VLSB_Bank2ClockPhaseError     = 0x40000,
  VLSB_Bank3ClockPhaseError     = 0x80000

} VLSBFIFOStatus;

typedef enum  VLSBFIFOGeneralStatus {
  VLSB_FIFOEmpty           = 0x1,
  VLSB_FIFOFull            = 0x10,
  VLSB_DataTimeout         = 0x100,
  VLSB_CRCError            = 0x1000,
  VLSB_ClockPhaseError     = 0x10000
} VLSBFIFOGeneralStatus;


#endif
