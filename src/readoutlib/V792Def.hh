#ifndef _V792DEF_H_
#define _V792DEF_H_

/* Register addresses */
#define V792_OUT_BUFFER                0x0
#define V792_OUT_BUFFER_MAX_SIZE       4352
#define V792_FIRMWARE_REV	       0x1000
#define V792_GEO_REGISTER	       0x1002
#define V792_GEO_MASK                  0x1F
#define V792_BIT_SET_REGISTER_1        0x1006
#define V792_BIT_CLEAR_REGISTER_1      0x1008
#define V792_STATUS_REGISTER_1         0x100E
#define V792_STATUS_REGISTER_2         0x1022
#define V792_BIT_SET_REGISTER_2        0x1032
#define V792_BIT_CLEAR_REGISTER_2      0x1034
#define V792_W_MEMORY_TEST             0x1036
#define V792_MEM_TEST_WORD_HIGH        0x1038
#define	V792_MEM_TEST_WORD_LOW         0x103A
#define V792_CRATE_SELECT              0x103C
#define V792_R_MEMORY_TEST             0x1064
#define V792_SW_COMM                   0x1068
#define V792_OUPUT_BUFFER              0x0000
#define V792_EVENT_COUNT_HIGH          0x1024
#define V792_EVENT_COUNT_LOW           0x1026
#define V792_FCLR_WINDOW               0x102E
#define V792_TEST_EVNT_WRITE           0x103E
#define V792_EVNT_COUNT_RESET          0x1040
#define V792_AAD_VALUE                 0x1070
#define V792_BAD_VALUE                 0x1072
#define	V792_SS_RESET_REG              0x1016
#define V792_CONTROL_REG               0x1010

#define CR_BLKEND_ENABLE                0x4
#define CR_PROG_RESET                   0x10
#define CR_BERR_ENABLE                  0x20
#define CR_ALIGN_ENABLE                 0x40

#define REG1_SOFT_RESET                 0x80
#define REG2_ACQ_TEST_MODE              0x40
#define REG2_OVER_FLOW                  0x8

#define STATUS1_DREADY                   0x1
#define STATUS1_GLOBAL_DREADY            0x2
#define STATUS1_BUSY                     0x4
#define STATUS1_GLOBAL_BUSY              0x8
#define STATUS1_AMNESIA                  0x10
#define STATUS1_PURGED                   0x20
#define STATUS1_TERM_ON                  0x40
#define STATUS1_TERM_OFF                 0x80
#define STATUS1_EVRDY                    0x100

#define STATUS2_BUFFER_EMPTY             0x2
#define STATUS2_BUFFER_FULL              0x4

#endif
