#include "hdl_iface.h"

typedef struct {
  //hdl_delegate_t can_err_isr;
  //hdl_event_t event;
  coroutine_t worker;
} hdl_sdio_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_sdio_mcu_var_t, *((hdl_sdio_mcu_t *)0)->obj_var, HDL_SDIO_MCU_VAR_SIZE, port_sd.h);

static hdl_module_state_t _hdl_sdio(const void *desc, uint8_t enable) {
  hdl_sdio_mcu_t *sdio = (hdl_sdio_mcu_t *)desc;
  SDIO_TypeDef *phy = (SDIO_TypeDef *)sdio->config->phy;
  if(enable) {
    /* Check the parameters */
    if(!(IS_SDIO_ALL_INSTANCE(phy) &&
        IS_SDIO_CLOCK_EDGE(sdio->config->clock_edge) &&
        IS_SDIO_CLOCK_BYPASS(sdio->config->clock_bypass) &&
        IS_SDIO_CLOCK_POWER_SAVE(sdio->config->clock_power_save) &&
        IS_SDIO_BUS_WIDE(sdio->config->bus_wide) &&
        IS_SDIO_HARDWARE_FLOW_CONTROL(sdio->config->hardware_flow_control) &&
        IS_SDIO_CLKDIV(sdio->config->clock_div)))
      return HDL_MODULE_FAULT;
    /* Set SDMMC configuration parameters */
    uint32_t tmpreg = (sdio->config->clock_edge   |\
              sdio->config->clock_bypass          |\
              sdio->config->clock_power_save      |\
              sdio->config->bus_wide              |\
              sdio->config->hardware_flow_control |\
              sdio->config->clock_div); 
    
    /* Write to SDMMC CLKCR */
    CL_REG_MODIFY(phy->CLKCR, CLKCR_CLEAR_MASK, tmpreg);
    CL_REG_SET(phy->POWER, SDIO_POWER_PWRCTRL);
    return  HDL_MODULE_ACTIVE;
  }
  CL_REG_SET(phy->POWER, (uint32_t)0x00000000);
  return HDL_MODULE_UNLOADED;
}

extern const hdl_sdio_mcu_iface_t hdl_sdio_mcu_iface = {
  .init = &_hdl_sdio
};
