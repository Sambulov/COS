#ifndef HDL_SD_DEF_H_
#define HDL_SD_DEF_H_

#define HDL_SD_ACMD41                     41

#define HDL_SD_ACMD41_ARG_HIGH_CAPACITY   0x40000000UL /* High capasity support */
#define HDL_SD_ACMD41_ARG_XPC             0x10000000UL /* XPC=0 0,36W(Power Saving), XPC=1 0,54W(Max Performance) */
#define HDL_SD_ACMD41_ARG_S1V8            0x01000000UL /* 1V8 signaling support */

#define HDL_SD_ACMD41_RESP_BUSY           0x80000000UL
#define HDL_SD_ACMD41_RESP_HIGH_CAPACITY  0x40000000UL
#define HDL_SD_ACMD41_RESP_UHS            0x20000000UL
#define HDL_SD_ACMD41_RESP_S1V8           0x01000000UL /* 1V8 signaling support */

#define HDL_SD_ACMD41_OCR                 0x00FFFF00UL

#define HDL_SD_OCR_VW_2V7_3V6             0x00FF8000UL

#define HDL_SD_ACMD6_ARG_BUS_WIDTH1       0x00000000UL
#define HDL_SD_ACMD6_ARG_BUS_WIDTH4       0x00000002UL


/* R1 response */
#define HDL_SD_STA_ADDR_OUT_OF_RANGE                   0x80000000U
#define HDL_SD_STA_ADDR_MISALIGNED                     0x40000000U
#define HDL_SD_STA_BLOCK_LEN_ERR                       0x20000000U
#define HDL_SD_STA_ERASE_SEQ_ERR                       0x10000000U
#define HDL_SD_STA_BAD_ERASE_PARAM                     0x08000000U
#define HDL_SD_STA_WRITE_PROT_VIOLATION                0x04000000U
#define HDL_SD_STA_LOCKED                              0x02000000U
#define HDL_SD_STA_LOCK_UNLOCK_FAILED                  0x01000000U
#define HDL_SD_STA_COM_CRC_FAILED                      0x00800000U
#define HDL_SD_STA_ILLEGAL_CMD                         0x00400000U
#define HDL_SD_STA_CARD_ECC_FAILED                     0x00200000U
#define HDL_SD_STA_CC_ERROR                            0x00100000U
#define HDL_SD_STA_GENERAL_UNKNOWN_ERROR               0x00080000U
#define HDL_SD_STA_STREAM_READ_UNDERRUN                0x00040000U
#define HDL_SD_STA_STREAM_WRITE_OVERRUN                0x00020000U
#define HDL_SD_STA_CID_CSD_OVERWRITE                   0x00010000U
#define HDL_SD_STA_WP_ERASE_SKIP                       0x00008000U
#define HDL_SD_STA_CARD_ECC_DISABLED                   0x00004000U
#define HDL_SD_STA_ERASE_RESET                         0x00002000U

#define HDL_SD_STA_STATE_MASK                          0x00001E00U
#define HDL_SD_STA_STATE_IDLE                          0x00000000U
#define HDL_SD_STA_STATE_READY                         0x00000200U
#define HDL_SD_STA_STATE_IDENT                         0x00000400U
#define HDL_SD_STA_STATE_STBY                          0x00000600U
#define HDL_SD_STA_STATE_TRAN                          0x00000800U
#define HDL_SD_STA_STATE_DATA                          0x00000A00U
#define HDL_SD_STA_STATE_RCV                           0x00000C00U
#define HDL_SD_STA_STATE_PRG                           0x00000E00U
#define HDL_SD_STA_STATE_DIS                           0x00001000U

#define HDL_SD_STA_READY_FOR_DATA                      0x00000100U
#define HDL_SD_STA_FX_EVENT                            0x00000040U
#define HDL_SD_STA_APP_CMD                             0x00000020U
#define HDL_SD_STA_AKE_SEQ_ERROR                       0x00000008U

#define HDL_SD_STA_ERRORBITS                           0xFDFFE008U

#endif /* HDL_SD_DEF_H_ */
