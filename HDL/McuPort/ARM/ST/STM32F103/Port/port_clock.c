#include "hdl_iface.h"

#define IRC8M_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC40K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define PLL_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)
#define HXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)

static hdl_module_state_t _hdl_clock_osc_en(uint32_t on_flag, uint32_t stb_flag, uint32_t timeout) {
  CL_REG_SET(RCC->CR, on_flag);
  while (!(CL_REG_GET(RCC->CR, stb_flag)) && (timeout--));
  if (!(CL_REG_GET(RCC->CR, stb_flag)))
    return HDL_MODULE_FAULT;
  return HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_selector_pll(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->config->clock_src;
    if(clock_src == NULL) return HDL_MODULE_FAULT;
    if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) {
      if (clk->config->property.div == 0 || clk->config->property.div > 2) return HDL_MODULE_FAULT;
      CL_REG_CLEAR(RCC->CFGR, RCC_CFGR_PLLXTPRE);
      if(clk->config->property.div == 2) CL_REG_SET(RCC->CFGR, RCC_CFGR_PLLXTPRE);
      CL_REG_SET(RCC->CFGR, RCC_CFGR_PLLSRC);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC8M)
      CL_REG_CLEAR(RCC->CFGR, RCC_CFGR_PLLSRC);
    else
      return HDL_MODULE_FAULT;
    return HDL_MODULE_ACTIVE;
  }
  else {
    CL_REG_CLEAR(RCC->CFGR, RCC_CFGR_PLLSRC);
    return HDL_MODULE_UNLOADED;
  }
}

static hdl_module_state_t _hdl_clock_pll(hdl_clock_mcu_t *clk, uint8_t enable) {
  while (enable) {
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->config->clock_src;
    if(clock_src == NULL) return HDL_MODULE_FAULT;
    uint32_t pll_cnf = clk->config->property.mul;
    if((pll_cnf < 2) || (pll_cnf > 16)) break;
    CL_REG_MODIFY(RCC->CFGR ,RCC_CFGR_PLLMULL_Msk , (pll_cnf - 2) << RCC_CFGR_PLLMULL_Pos);
    if(_hdl_clock_osc_en(RCC_CR_PLLON, RCC_CR_PLLRDY, PLL_STARTUP_TIMEOUT) != HDL_MODULE_ACTIVE) {
      CL_REG_CLEAR(RCC->CR, RCC_CR_PLLON);
      break;
    }
    return HDL_MODULE_ACTIVE;
  }
  CL_REG_CLEAR(RCC->CFGR ,RCC_CFGR_PLLMULL_Msk);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_clock_system_switch(uint32_t src) {
  CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_SW, src);
  uint32_t timeout = CK_SYS_STARTUP_TIMEOUT;
  while((CL_REG_GET(RCC->CFGR, RCC_CFGR_SWS) != (src << RCC_CFGR_SWS_Pos)) && timeout--) ;
  return ((CL_REG_GET(RCC->CFGR, RCC_CFGR_SWS) != (src << RCC_CFGR_SWS_Pos)))? HDL_MODULE_FAULT: HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_system(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->config->clock_src;
    if((clock_src == NULL) && (clk->dependencies != NULL)) 
      clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    if(clock_src == NULL) return HDL_MODULE_FAULT;
    if(clock_src->config->type == HDL_CLOCK_TYPE_PLL) 
      return _hdl_clock_system_switch(RCC_CFGR_SW_PLL);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL)
      return _hdl_clock_system_switch(RCC_CFGR_SW_HSE);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC8M)
      return _hdl_clock_system_switch(RCC_CFGR_SW_HSI);
    /* TODO: config clock mon */
  }
  _hdl_clock_system_switch(RCC_CFGR_SW_HSI);
  return HDL_MODULE_UNLOADED;
}


static hdl_module_state_t _hdl_bus_clock_cnf(hdl_clock_mcu_t *clk, uint32_t bit_from, uint32_t bit_to, 
                                             hdl_clock_type_t base_clock) {
  uint32_t factor = clk->config->property.div;
  hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->config->clock_src;
  if(factor >= 64) factor >>= 1; /* AHB have no 32 div factor */
  if((factor == 0) || (clock_src == NULL) || (clock_src->config->type != base_clock)) 
    return HDL_MODULE_FAULT;
  uint32_t div_cnf = 31 - __CLZ(factor);
  if (div_cnf) div_cnf = ((div_cnf - 1) << bit_from) | (1UL << bit_to);  
  CL_REG_MODIFY(RCC->CFGR, CL_BIT_MASK(bit_from, bit_to), div_cnf);
  return HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock(const void *desc, uint8_t enable) {
  hdl_clock_mcu_t *clk = (hdl_clock_mcu_t *)desc;
  switch (clk->config->type) {
    case HDL_CLOCK_TYPE_HXTAL:
      if (enable) return _hdl_clock_osc_en(RCC_CR_HSEON, RCC_CR_HSERDY, HXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_LXTAL:
      if (enable) {
        CL_REG_SET(RCC->BDCR, RCC_BDCR_LSEON);
        uint32_t timeout = IRC40K_STARTUP_TIMEOUT;
        while (!(CL_REG_GET(RCC->CSR, RCC_BDCR_LSERDY)) && (timeout--));
        if (CL_REG_GET(RCC->CSR, RCC_BDCR_LSERDY))
          return HDL_MODULE_ACTIVE;
      }
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC8M:
      if (enable) return _hdl_clock_osc_en(RCC_CR_HSION, RCC_CR_HSIRDY, IRC8M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_PLL_SEL:
      return _hdl_clock_selector_pll(clk, enable);

    case HDL_CLOCK_TYPE_PLL:
      return _hdl_clock_pll(clk, enable);

    case HDL_CLOCK_TYPE_SYS_SEL:
      return _hdl_clock_system(clk, enable);

    case HDL_CLOCK_TYPE_AHB:
      if (enable)
        return _hdl_bus_clock_cnf(clk, 4, 7, HDL_CLOCK_TYPE_SYS_SEL);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB1:
      if (enable) return _hdl_bus_clock_cnf(clk, 8, 10, HDL_CLOCK_TYPE_AHB);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB2:
      if (enable) return _hdl_bus_clock_cnf(clk, 11, 13, HDL_CLOCK_TYPE_AHB);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC40K:
      if (enable) {
        CL_REG_SET(RCC->CSR, RCC_CSR_LSION);
        uint32_t timeout = IRC40K_STARTUP_TIMEOUT;
        while (!(CL_REG_GET(RCC->CSR, RCC_CSR_LSIRDY)) && (timeout--));
        if (CL_REG_GET(RCC->CSR, RCC_CSR_LSIRDY))
          return HDL_MODULE_ACTIVE;
      }
      return HDL_MODULE_UNLOADED;

    default:
      break;
  }
  return HDL_MODULE_FAULT;
}

static uint8_t _hdl_get_clock(const void *desc, hdl_clock_freq_t *freq) {
  if(freq != NULL) {
    hdl_clock_mcu_t *clk = (hdl_clock_mcu_t *)desc;
    if(clk->config->clock_src != NULL) {
      if(!_hdl_get_clock(clk->config->clock_src, freq))
        return HDL_FALSE;
    }
    else {
      freq->denom = 1;
      freq->num = clk->config->property.freq;
    }
    switch (clk->config->type) {
      case HDL_CLOCK_TYPE_AHB:
      case HDL_CLOCK_TYPE_APB1:
      case HDL_CLOCK_TYPE_PLL_SEL:
      case HDL_CLOCK_TYPE_APB2:
        hdl_clock_calc_div(freq, clk->config->property.div, freq);
        break;
      case HDL_CLOCK_TYPE_PLL:
        hdl_clock_calc_mul(freq, clk->config->property.mul, freq);
      default:
        break;
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_clock_iface_t hdl_clock_iface = {
  .init = &_hdl_clock,
  .get = &_hdl_get_clock
};
