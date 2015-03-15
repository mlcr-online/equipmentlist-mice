#ifndef _V513DEF_H_ 
#define _V513DEF_H_ 

#define V513_INTERUPT_VECTOR_REG      0x00
#define V513_INTERUPT_LEVEL_REG       0x02
#define V513_IO_REG                   0x04
#define V513_STROBE_REG               0x06
#define V513_INT_MASK_REG             0x08
#define V513_CHANNEL_STATUS_REG       0x10
#define V513_CLEAR_VME_INTERUPT       0x40
#define V513_MODULE_RESET             0x42
#define V513_CLEAR_STROBE_BIT         0x44
#define V513_INIT_STATUS_REGS         0x46
#define V513_CLEAR_INPUT_REG          0x48

#define STB_NEG_POL                0x1
#define STB_IRQ_ENABLE             0x2

#endif

// #define V513_STATUS_OUTPUT            0x0
// #define V513_STATUS_INPUT             0x1
// #define V513_POLARITY_NEG             0x0
// #define V513_POLARITY_POS             0x2
// #define V513_INP_MODE_GLITCHED        0x0
// #define V513_INP_MODE_NORMAL          0x4
// #define V513_TRANSF_MODE_VME          0x0
// #define V513_TRANSF_MODE_EXT          0x8
