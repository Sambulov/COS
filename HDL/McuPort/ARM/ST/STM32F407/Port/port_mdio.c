#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
} hdl_mdio_mcu_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_mdio_mcu_var_t, *((hdl_mdio_mcu_t *)0)->obj_var, HDL_MDIO_MCU_VAR_SIZE, port_mdio.h);

static uint8_t _mdio_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this; (void)cancel;
  hdl_mdio_mcu_t *mdio = (hdl_mdio_mcu_t *)arg;
  hdl_mdio_mcu_var_t *mdio_var = (hdl_mdio_mcu_var_t *)mdio->obj_var;
  (void)mdio_var;

  //return cancel;
  return HDL_TRUE;
}

static inline uint8_t _eth_soft_reset(ETH_TypeDef *phy) {
  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  CL_REG_SET(phy->DMABMR, ETH_DMABMR_SR);
  uint32_t time = 10000;
  while (CL_REG_GET(phy->DMABMR, ETH_DMABMR_SR) && time--);
  return (time != 0);
}

static hdl_module_state_t _hdl_mdio(const void *desc, uint8_t enable) {
  hdl_mdio_mcu_t *mdio = (hdl_mdio_mcu_t *)desc;
  hdl_mdio_mcu_var_t *mdio_var = (hdl_mdio_mcu_var_t *)mdio->obj_var;
  ETH_TypeDef *phy = ETH;
  if(enable) {
    __HAL_RCC_ETHMAC_CLK_ENABLE();

    if(!_eth_soft_reset(phy)) return HDL_MODULE_FAULT;

    hdl_clock_freq_t eth_clk;
    hdl_clock_get(mdio->dependencies[0], &eth_clk);
    uint32_t clk = (eth_clk.num + (eth_clk.denom - 1)) / eth_clk.denom;
    uint32_t div;
    if (clk < 35000000U) div = ETH_MACMIIAR_CR_Div16;
    else if (clk < 60000000U) div = ETH_MACMIIAR_CR_Div26;
    else if (clk < 100000000U) div = ETH_MACMIIAR_CR_Div42;
    else if (clk < 150000000U) div = ETH_MACMIIAR_CR_Div62;
    else div = ETH_MACMIIAR_CR_Div102;
    CL_REG_MODIFY(phy->MACMIIAR, ETH_MACMIIAR_CR, div);

    coroutine_add(&mdio_var->worker, &_mdio_worker, mdio);
    return  HDL_MODULE_ACTIVE;
  }
  __HAL_RCC_ETH_CLK_DISABLE();
  coroutine_cancel(&mdio_var->worker);
  return HDL_MODULE_UNLOADED;
}

#define MDIO_MODE_WRITE    0
#define MDIO_MODE_READ     1

static uint8_t _hdl_mdio_mdio_rw(const void *desc, hdl_mdio_message_t *message, uint8_t mode) {
  (void)desc;
  //hdl_mdio_mcu_t *mdio = (hdl_mdio_mcu_t *)desc;
  ETH_TypeDef *phy = ETH;
  uint32_t tmpreg = CL_REG_GET(phy->MACMIIAR, ETH_MACMIIAR_CR);
  const uint8_t mii_div[] = {42, 62, 16, 26, 102};
  uint32_t time = (uint32_t)mii_div[tmpreg >> ETH_MACMIIAR_CR_Pos] * 32 * 10;
  tmpreg |= (((uint32_t)message->dev_addr << ETH_MACMIIAR_PA_Pos) & ETH_MACMIIAR_PA);
  tmpreg |= (((uint32_t)message->reg_addr << ETH_MACMIIAR_MR_Pos) & ETH_MACMIIAR_MR);
  tmpreg |= (mode == MDIO_MODE_WRITE)? ETH_MACMIIAR_MW : 0;
  tmpreg |= ETH_MACMIIAR_MB;
  phy->MACMIIDR = message->reg_val;
  phy->MACMIIAR = tmpreg;
  /* Check for the Busy flag */
  while (CL_REG_GET(phy->MACMIIAR, ETH_MACMIIAR_MB) && time--);
  if(!time) 
    return HDL_FALSE;
  if(mode == MDIO_MODE_READ) message->reg_val = phy->MACMIIDR;
  return HDL_TRUE;
}

static uint8_t _hdl_mdio_mdio_read(const void *desc, hdl_mdio_message_t *message) {
  return _hdl_mdio_mdio_rw(desc, message, MDIO_MODE_READ);
}

static uint8_t _hdl_mdio_mdio_write(const void *desc, hdl_mdio_message_t *message) {
  return _hdl_mdio_mdio_rw(desc, message, MDIO_MODE_WRITE);
}

const hdl_mdio_iface_t hdl_mdio_mcu_iface = {
  .init = &_hdl_mdio,
  .read_reg = &_hdl_mdio_mdio_read,
  .write_reg = &_hdl_mdio_mdio_write,
};
