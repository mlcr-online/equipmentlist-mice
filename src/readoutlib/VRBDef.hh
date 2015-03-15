#ifndef _MDEVRB_DEF
#define _MDEVRB_DEF 1

/* Register addresses */
#define START_CHAIN_TRANSFER  0x05
#define DBB_SEND_REQ          0x0F
#define DBB_TRIG_COUNT        0x1F
#define DBB_SPILL_WIDTH       0x2F
#define DBB_FM_VERSION        0x3F
#define DBB_CHMASK0           0x5F
#define DBB_CHMASK1           0x63
#define DBB_STATUS            0x6F
#define DBB_RESET             0x7F
#define DBB_FAST_RESET        0x8F

#define DBB1_GEO         0xFF04
#define DBB2_GEO         0xFF08
#define DBB3_GEO         0xFF0C
#define DBB4_GEO         0xFF10
#define DBB5_GEO         0xFF14
#define DBB6_GEO         0xFF18

#define VRB_RESET        0xFFFF

#define ADDR_DBB1_DSIZE  0xFF44
#define ADDR_DBB2_DSIZE  0xFF48
#define ADDR_DBB3_DSIZE  0xFF4C
#define ADDR_DBB4_DSIZE  0xFF50
#define ADDR_DBB5_DSIZE  0xFF54
#define ADDR_DBB6_DSIZE  0xFF58

#define ADDR_TSM_ST      0xFFCC
#define ADDR_NUM_DBBS    0xFFEE

#define ST_TIMESTAMP_OVERFLOW        0x01
#define ST_L1_FIFO_OVERFLOW          0x02
#define ST_L2_FIFO_OVERFLOW          0x04
#define ST_COMMAND_WHILE_BUSY        0x08
#define ST_DATA_WHILE_BUSY           0x10
#define ST_COMMAND_DURING_SPILLGATE  0x20

#endif

