#ifndef _MDEV1495_DEF
#define _MDEV1495_DEF 1

//  -- Register Address Map

// #define V1495_BOARD_ID        0x8008
#define V1495_BOARD_ID        0x102C
#define V1495_BOARD_RESET     0x800A
#define V1495_BOARD_FV        0x800C

#define V1495_STATUS          0x1030
#define V1495_N_TRIGGERS      0x1034
#define V1495_N_DW            0x1038
#define V1495_N_SPILLS        0x103C
#define V1495_SWR_SG          0x1040
#define V1495_USER_FV         0x1008

#define V1495_PHYS_BUSY_01    0x1060
#define V1495_PHYS_BUSY_23    0x1064
#define V1495_PHYS_BUSY_45    0x1068

#define V1495_PT_VETO_LENGHT  0x100C
#define V1495_SG_OPEN_DELAY   0x1010
#define V1495_SG_CLOSE_DELAY  0x1014
#define V1495_SGG_CTRL        0x1018
#define V1495_TOF0_MASK       0x101C
#define V1495_TOF1_MASK       0x1020
#define V1495_TOF2_MASK       0x1024
#define V1495_PTG_CTRL        0x1028
// #define V1495_    0x102c

#define V1495_MEB             0x0000

//  -- Trigger ctrl

#define TRIGGER_TOF0_OR       0x0003
#define TRIGGER_TOF0_AND      0x0007
#define TRIGGER_TOF1_OR       0x0018
#define TRIGGER_TOF1_V        0x0008
#define TRIGGER_TOF1_H        0x0010
#define TRIGGER_TOF1_AND      0x0038
#define TRIGGER_TOF2_OR       0x00C0
#define TRIGGER_TOF2_AND      0x01C0
#define TRIGGER_GVA           0x0200
#define TRIGGER_PULS_500KHz   0x3C00
#define TRIGGER_PULS_200KHz   0x3400
#define TRIGGER_PULS_100KHz   0x2C00
#define TRIGGER_PULS_50KHz    0x2400
#define TRIGGER_PULS_20KHz    0x1C00
#define TRIGGER_PULS_10KHz    0x1400
#define TRIGGER_PULS_5KHz     0x0C00
#define TRIGGER_PULS_2KHz     0x0400
#define RAND1                 0x8000
#define RAND2                 0xC000

#define EXTERNALVETO_ENABLE   0x10000000


#endif

