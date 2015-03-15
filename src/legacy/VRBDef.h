#ifndef _VRBDEF_H_
#define _VRBDEF_H_


typedef enum VRBConfigRegisterMask{
  VRB_40MHz_Clock             = 0x18,
  VRB_Memory_Internal_Mode    = 0x1,
  VRB_Global_Reset            = 0x2,
  VRB_Start                   = 0x4,
  VRB_Lock                    = 0x2000,
  VRB_Loop                    = 0x1000
} VRBConfigRegisterMask;

#define VRB_CONFIGURATION_REGISTER   0xC000
#define VRB_COUNTER_REGISTER         0xA000
#define VRB_MEMORY_A                 0x0000
#define VRB_MEMORY_B                 0x2000

#endif









