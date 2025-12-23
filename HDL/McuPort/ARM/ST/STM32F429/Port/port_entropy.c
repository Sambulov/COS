#include "hdl_iface.h"

static uint32_t _hdl_entropy_poll(const void *desc, void *output, uint32_t amount) {
  (void)desc;
  volatile uint8_t randomValue[4];
  uint8_t av = 0;
  uint32_t index = 0;
  for (; index < amount; index++) {
    if(!av) {
      uint32_t to = 1000;
      while (!(RNG->SR & RNG_SR_DRDY) && to--);
      if(!to) break;
      *(uint32_t *)randomValue = RNG->DR;
      av = 4;
    }
    ((uint8_t *)output)[index] = randomValue[--av];
  }
  return index;
}

static hdl_module_state_t _hdl_entropy(const void *desc, uint8_t enable) {
  (void)desc;
  if(enable) {
    CL_REG_SET(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);
    /* Delay after an RCC peripheral clock enabling (Errata 2.1.13)*/
    (void volatile) CL_REG_GET(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);
    /* Enable the RNG Peripheral */
    CL_REG_SET(RNG->CR, RNG_CR_RNGEN);
    return  HDL_MODULE_ACTIVE;
  }
  CL_REG_CLEAR(RNG->CR, RNG_CR_RNGEN);
  CL_REG_CLEAR(RCC->AHB2ENR, RCC_AHB2ENR_RNGEN);
  return HDL_MODULE_UNLOADED;
}

const hdl_entropy_iface_t hdl_entropy_iface = {
  .init = &_hdl_entropy,
  .poll = &_hdl_entropy_poll
};
