#ifndef _DBBDEF_H_
#define _DBBDEF_H_

#define DBB_BANK_SIZE                0x1000 // 4096
#define DBB_WORD_SIZE                2


#define DBB_CMD_TYPE                 0x5000
#define DBB_ARGUMENT_TYPE            0x6000
#define DBB_TRIGGER_COUNT_TYPE       0x8000
#define DBB_SPILL_WIDTH_TYPE         0x9000
#define DBB_FIRMWARE_VERSION_TYPE    0xA000
#define DBB_STATUS_TYPE              0xB000
#define DBB_ENABLE_MASK_TYPE         0xC000
#define DBB_HEADER_TYPE              0xD000
#define DBB_TRAILER_TYPE             0xE000

#define BOARDID_ALL                  0x3F

#define DBB_CMD_RESET                0x3C
#define DBB_CMD_SEND_DATA            0x21
#define DBB_CMD_TRIGGER_COUNT        0x23
#define DBB_CMD_SPILL_WIDTH          0x25
#define DBB_CMD_FIRMWARE             0x27
#define DBB_CMD_STATUS               0x29
#define DBB_CMD_SET_ENABLE_MASK      0x2A
#define DBB_CMD_READ_ENABLE_MASK     0x2B
#define DBB_CMD_FAILSAFE_RESET       0x3E

typedef enum DBBCommandMask{
  DBB_Command_Type_Mask  = 0xF000,
  DBB_Board_Id_Mask      = 0xFC0,
  DBB_Command_Mask       = 0x3F
};

typedef enum DBBCmdReplyMask{
  DBB_Trigger_Count_Reply_Mask  = 0x3FF /*,
  DBB_Spill_Width_Reply_Mask    = 0xFFFFF */
};

#endif









