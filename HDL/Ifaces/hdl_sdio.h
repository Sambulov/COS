#ifndef HDL_SDIO_H_
#define HDL_SDIO_H_

typedef enum {
  HDL_SDIO_STATE_BUSY     = 0x01,
  HDL_SDIO_STATE_CANCELED = 0x02,
  HDL_SDIO_STATE_COMPLETE = 0x08,
  HDL_SDIO_STATE          = 0x0F,
  HDL_SDIO_ERROR_TIMEOUT  = 0x10,
  HDL_SDIO_ERROR_CRC      = 0x20,
  HDL_SDIO_ERROR_BAD_ARG  = 0xE0,
  HDL_SDIO_ERROR_INTERNAL = 0xF0,
  HDL_SDIO_ERROR          = 0xF0
} hdl_sdio_msg_status_t;

typedef enum {
  HDL_SDIO_CMD_RESPONSE_NONE  = 0x00,
  HDL_SDIO_CMD_RESPONSE_SHORT = 0x40, 
  HDL_SDIO_CMD_RESPONSE_LONG  = 0xc0,
  HDL_SDIO_CMD_MASK           = 0x3f,
  HDL_SDIO_CMD_RESPONSE_MASK  = 0xc0
} hdl_sdio_cmd_response_t;

typedef struct {
  uint32_t argument;      /* Specifies the HDL_SDIO command argument which is sent
                             to a card as part of a command message. */
  uint32_t response[4];   /* response[0] = [127:96], response[1] = [95:64], response[2]=[63:32], response[3] = [31:0] for long response;
                             response[0] = [39:8] for short; */
  uint8_t cmd;            /* Specifies the command index [0,64] & responce type. 
                             Complete message contains returned [45:40]/[133:128] bits & responce type */
  hdl_sdio_msg_status_t status;
} hdl_sdio_cmd_message_t;

typedef enum {
  HDL_SDIO_DATA_STREAM,
  HDL_SDIO_DATA_BLOCK,
} hdl_sdio_data_transfer_type_t;

typedef enum {
  HDL_SDIO_DATA_TO_DEV,
  HDL_SDIO_DATA_FROM_DEV,
} hdl_sdio_data_dir_t;

typedef struct {
  void *data;
  uint32_t data_length;         /* Specifies the number of data bytes to be transferred. */
  uint32_t data_block_size;     /* Specifies the data block size for block transfer. */
  hdl_sdio_data_dir_t dir; 
  hdl_sdio_data_transfer_type_t mode;
  uint32_t timeout;             /* Data transfer timeout ms
                                   SD spec see 4.6.2:
                                   Data write timeout is 250ms  
                                   Data read timeout is 100ms
                                */
  uint32_t transferred;
  hdl_sdio_msg_status_t status;
} hdl_sdio_data_message_t;

typedef enum {
  HDL_SDIO_BUS_WIDTH_1 = 1,
  HDL_SDIO_BUS_WIDTH_4 = 2,
  HDL_SDIO_BUS_WIDTH_8 = 4,
  HDL_SDIO_BUS_WIDTH_ALL = 7
} hdl_sdio_bus_width_t;

typedef uint8_t (* hdl_sdio_cmd_transfer_t)(const void *desc, hdl_sdio_cmd_message_t *message);
typedef uint8_t (* hdl_sdio_data_transfer_t)(const void *desc, hdl_sdio_data_message_t *message);
typedef uint8_t (* hdl_sdio_cnf_t)(const void *desc, hdl_sdio_bus_width_t *out_width, uint32_t *out_speed);
typedef uint8_t (* hdl_sdio_bus_t)(const void *desc, hdl_sdio_bus_width_t width);
typedef uint8_t (* hdl_sdio_clock_t)(const void *desc, uint32_t speed);

typedef struct{
  hdl_module_initializer_t init;
  hdl_sdio_cmd_transfer_t cmd;
  hdl_sdio_data_transfer_t data;
  hdl_sdio_cnf_t get_bus_clock;
  hdl_sdio_bus_t check_bus;
  hdl_sdio_bus_t set_bus;
  hdl_sdio_clock_t set_clock;
} hdl_sdio_iface_t;

hdl_module_new_t(hdl_sdio_t, 0, void *, hdl_sdio_iface_t);

__STATIC_INLINE uint8_t hdl_sdio_data_transfer(const void *desc, hdl_sdio_data_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_sdio_t *)desc)->iface->data(desc, message);
}

__STATIC_INLINE uint8_t hdl_sdio_cmd_transfer(const void *desc, hdl_sdio_cmd_message_t *message) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_sdio_t *)desc)->iface->cmd(desc, message);
}

__STATIC_INLINE uint8_t hdl_sdio_set_bus(const void *desc, hdl_sdio_bus_width_t width) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_sdio_t *)desc)->iface->set_bus(desc, width);
}

__STATIC_INLINE uint8_t hdl_sdio_bus_supported(const void *desc, hdl_sdio_bus_width_t width) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_sdio_t *)desc)->iface->check_bus(desc, width);
}

__STATIC_INLINE uint8_t hdl_sdio_set_clock(const void *desc, uint32_t speed) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_sdio_t *)desc)->iface->set_clock(desc, speed);
}

__STATIC_INLINE uint8_t hdl_sdio_get_bus_clock(const void *desc, hdl_sdio_bus_width_t *out_width, uint32_t *out_speed) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_sdio_t *)desc)->iface->get_bus_clock(desc, out_width, out_speed);
}

#endif /* HDL_SDIO_H_ */
