#ifndef ATB3500_IO_H_
#define ATB3500_IO_H_

#define ATB3500_IO_PRV_SIZE    24

#define ATB3500_IO_AMOUNT   11
/*
  depends on:
  communication
  LED_0_0, LED_0_1, LED_0_2,
  LED_1_0, LED_1_1, LED_1_2,
  LED_2_0, LED_2_1, LED_2_2,
  RELAY_1, RELAY_2
*/
typedef struct {
    hdl_module_t module;
    PRIVATE(bldl, ATB3500_IO_PRV_SIZE);
} atb3500_io_t;

typedef enum {
    ATB3500_IO_LED_0_0 = 0x00000001,
    ATB3500_IO_LED_0_1 = 0x00000002,
    ATB3500_IO_LED_0_2 = 0x00000004,
    ATB3500_IO_LED_1_0 = 0x00000008,
    ATB3500_IO_LED_1_1 = 0x00000010,
    ATB3500_IO_LED_1_2 = 0x00000020,
    ATB3500_IO_LED_2_0 = 0x00000040,
    ATB3500_IO_LED_2_1 = 0x00000080,
    ATB3500_IO_LED_2_2 = 0x00000100,
    ATB3500_IO_RELAY_1 = 0x00000200,
    ATB3500_IO_RELAY_2 = 0x00000400,
    ATB3500_IO_ALL =     ATB3500_IO_LED_0_0 | ATB3500_IO_LED_0_1 | ATB3500_IO_LED_0_2 |
                         ATB3500_IO_LED_1_0 | ATB3500_IO_LED_1_1 | ATB3500_IO_LED_1_2 |
                         ATB3500_IO_LED_2_0 | ATB3500_IO_LED_2_1 | ATB3500_IO_LED_2_2 |
                         ATB3500_IO_RELAY_1 | ATB3500_IO_RELAY_2
} atb3500_io_port_t;

hdl_module_state_t atb3500_io(void *desc, uint8_t enable);

void atb3500_io_set(atb3500_io_t *desc, atb3500_io_port_t port, uint8_t active);

uint32_t atb3500_io_proto_rx_size(void);
void atb3500_io_proto_set_map_rx(atb3500_io_t *desc, proto_map_mem_t *map);
uint32_t atb3500_io_proto_tx_size(void);
void atb3500_io_proto_set_map_tx(atb3500_io_t *desc, proto_map_mem_t *map);

#endif //ATB3500_IO_H_