#ifndef HDL_SHIFT_REG_H_
#define HDL_SHIFT_REG_H_

#define HDL_SHIFT_REG_PRV_SIZE   0

/* depends on:
  spi (mosi->data, miso->data return, clk->clk, cs->latch)
  gpio -> reset#
  gpio -> output enable
 */
typedef struct {
  hdl_module_t module;

  PRIVATE(hdl, HDL_SHIFT_REG_PRV_SIZE);
} hdl_shift_reg_spi_t;

hdl_module_state_t hdl_shift_reg_spi(void *desc, uint8_t enable);

//hdl_gpio_state hdl_gpio_read(const hdl_gpio_pin_t *gpio);
hdl_gpio_state hdl_shift_reg_spi_read_output(const hdl_gpio_pin_t *gpio);
void hdl_shift_reg_spi_write(const hdl_gpio_pin_t *gpio, const hdl_gpio_state state);
void hdl_shift_reg_spi_toggle(const hdl_gpio_pin_t *gpio);

#endif // HDL_SHIFT_REG_H_
