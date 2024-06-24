#ifndef ATB3500_IO_H_
#define ATB3500_IO_H_

#define ATB3500_IO_PRV_SIZE    20

#define ATB3500_IO_AMOUNT   22
/*
  depends on:
  communication
  LED_0_0, LED_0_1, LED_0_2,
  LED_1_0, LED_1_1, LED_1_2,
  LED_2_0, LED_2_1, LED_2_2,
  RELAY_1, RELAY_2, PCI_SWITCH,
  LTE_RST, MOD_ADDR1, MOD_ADDR2,
  MOD_ADDR3, MOD_ADDR4, MOD_ADDR5,
  EXT_IRQ1, EXT_IRQ2, EXT_OUT1,
  EXT_OUT2 
*/
typedef struct {
    hdl_module_t module;
    PRIVATE(bldl, ATB3500_IO_PRV_SIZE);
} atb3500_io_t;

typedef enum {
    ATB3500_IO_LED_0_0    = 0x00000001,
    ATB3500_IO_LED_0_1    = 0x00000002,
    ATB3500_IO_LED_0_2    = 0x00000004,
    ATB3500_IO_LED_1_0    = 0x00000008,
    ATB3500_IO_LED_1_1    = 0x00000010,
    ATB3500_IO_LED_1_2    = 0x00000020,
    ATB3500_IO_LED_2_0    = 0x00000040,
    ATB3500_IO_LED_2_1    = 0x00000080,
    ATB3500_IO_LED_2_2    = 0x00000100,
    ATB3500_IO_RELAY_1    = 0x00000200,
    ATB3500_IO_RELAY_2    = 0x00000400,

    ATB3500_IO_PCI_SWITCH = 0x00000800,
    ATB3500_IO_LTE_RST    = 0x00001000,
    
    ATB3500_IO_MOD_ADDR1  = 0x00002000,
    ATB3500_IO_MOD_ADDR2  = 0x00004000,
    ATB3500_IO_MOD_ADDR3  = 0x00008000,
    ATB3500_IO_MOD_ADDR4  = 0x00010000,
    ATB3500_IO_MOD_ADDR5  = 0x00020000,
    ATB3500_IO_EXT_IRQ1   = 0x00080000,
    ATB3500_IO_EXT_IRQ2   = 0x00100000,

    ATB3500_IO_EXT_OUT1   = 0x00200000,
    ATB3500_IO_EXT_OUT2   = 0x00400000,
    
    ATB3500_IO_ALL_OUT    = ATB3500_IO_LED_0_0 | ATB3500_IO_LED_0_1 | ATB3500_IO_LED_0_2 |
                            ATB3500_IO_LED_1_0 | ATB3500_IO_LED_1_1 | ATB3500_IO_LED_1_2 |
                            ATB3500_IO_LED_2_0 | ATB3500_IO_LED_2_1 | ATB3500_IO_LED_2_2 |
                            ATB3500_IO_RELAY_1 | ATB3500_IO_RELAY_2 | ATB3500_IO_PCI_SWITCH | 
                            ATB3500_IO_LTE_RST | ATB3500_IO_EXT_OUT1 | ATB3500_IO_EXT_OUT2,
    ATB3500_IO_ALL_IN     = ATB3500_IO_MOD_ADDR1 | ATB3500_IO_MOD_ADDR2 | ATB3500_IO_MOD_ADDR3 |
                            ATB3500_IO_MOD_ADDR4 | ATB3500_IO_MOD_ADDR5 | ATB3500_IO_EXT_IRQ1 |
                            ATB3500_IO_EXT_IRQ1,
    ATB3500_IO_ALL        = ATB3500_IO_ALL_OUT | ATB3500_IO_ALL_IN
                          
} atb3500_io_port_t;

hdl_module_state_t atb3500_io(void *desc, uint8_t enable);

void atb3500_io_set(atb3500_io_t *desc, atb3500_io_port_t port, uint8_t active);

uint32_t atb3500_io_proto_rx_size(void);
void atb3500_io_proto_set_map_rx(atb3500_io_t *desc, proto_map_mem_t *map);
uint32_t atb3500_io_proto_tx_size(void);
void atb3500_io_proto_set_map_tx(atb3500_io_t *desc, proto_map_mem_t *map);

#endif //ATB3500_IO_H_