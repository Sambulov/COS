#include "hdl_iface.h"

// typedef struct {
//   hdl_clock_freq_t freq;
// } hdl_clock_var_t;

// HDL_ASSERRT_STRUCTURE_CAST(hdl_clock_var_t, *((hdl_clock_mcu_t *)0)->obj_var, HDL_CLOCK_VAR_SIZE, port_clock.h);

#define IRC48M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC32K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define PLL_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)
#define PMU_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)

#define MAX_SYS_CLOCK             168000000UL
//#define PLL_MAX_FREQ              MAX_SYS_CLOCK
//#define APB2_MAX_FREQ             120000000UL
//#define APB1_MAX_FREQ             60000000UL

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
      case HDL_CLOCK_TYPE_PLL_SRC:
      case HDL_CLOCK_TYPE_APB2:
      case HDL_CLOCK_TYPE_PLL_P:
      case HDL_CLOCK_TYPE_PLL_Q:
      case HDL_CLOCK_TYPE_PLL_I2S_R:
      case HDL_CLOCK_TYPE_MCO1:
      case HDL_CLOCK_TYPE_MCO2:
        hdl_clock_calc_div(freq, clk->config->property.div, freq);
        break;
      case HDL_CLOCK_TYPE_PLL_N:
      case HDL_CLOCK_TYPE_PLL_I2S_N:
        hdl_clock_calc_mul(freq, clk->config->property.mul, freq);
        break;
      case HDL_CLOCK_TYPE_APB1_TIMERS:
      case HDL_CLOCK_TYPE_APB2_TIMERS:
        if(clk->config->clock_src->config->property.div != 1)
          hdl_clock_calc_mul(freq, 2, freq);
      default:
        break;
    }
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static hdl_module_state_t _hdl_clock_osc_en(volatile uint32_t *reg, uint32_t on, uint32_t rdy) {
  uint32_t timer = 1000000;
  CL_REG_SET(*reg, on);
  while (timer-- && !(*reg & rdy));
  if(*reg & rdy) return HDL_MODULE_ACTIVE;
  return HDL_MODULE_FAULT;
}

static hdl_module_state_t _hdl_clock_system_switch(uint32_t rdy, uint32_t sw, uint32_t swr) {
  uint32_t timer = 10000;
  if(RCC->CR & rdy) {
    CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_SW, sw);
    while (timer-- && !CL_REG_CHECK(RCC->CFGR, RCC_CFGR_SWS, swr));
    if(CL_REG_CHECK(RCC->CFGR, RCC_CFGR_SWS, swr)) return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_FAULT;
}

static hdl_module_state_t _hdl_clock_system(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->config->clock_src;
    if((clock_src == NULL) && (clk->dependencies != NULL)) clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_freq_t freq;
    if((clock_src == NULL) || !_hdl_get_clock(clock_src, &freq)) 
      return HDL_MODULE_FAULT;
    uint32_t clock = (freq.num + freq.denom - 1) / freq.denom; /* HAL dependency */
    if(clock > MAX_SYS_CLOCK)
      return HDL_MODULE_FAULT;
    SystemCoreClock = clock;
    if(clock_src->config->type == HDL_CLOCK_TYPE_PLL_P)
      return _hdl_clock_system_switch(RCC_CR_PLLRDY, RCC_CFGR_SW_PLL, RCC_CFGR_SWS_PLL);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL)
      return _hdl_clock_system_switch(RCC_CR_HSERDY, RCC_CFGR_SW_HSE, RCC_CFGR_SWS_HSE);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC16M)
      return _hdl_clock_system_switch(RCC_CR_HSIRDY, RCC_CFGR_SW_HSI, RCC_CFGR_SWS_HSI);
    /* TODO: config clock mon */
  }
  SystemCoreClock = 16000000;
  _hdl_clock_system_switch(RCC_CR_HSIRDY, RCC_CFGR_SW_HSI, RCC_CFGR_SWS_HSI);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_bus_clock_cnf(hdl_clock_mcu_t *clk, uint32_t mask, uint32_t offset) {
  uint32_t factor = clk->config->property.div;
  uint8_t is_ahb = clk->config->type == HDL_CLOCK_TYPE_AHB;
  uint32_t reg = 0;
  while (factor) {
    factor >>= 1;
    reg++;
  }
  reg--;
  if((!is_ahb && (reg > 5)) || (is_ahb && ((reg == 5) || (reg > 9))))
    return HDL_MODULE_FAULT; 
  if(reg > 0) {
    reg--;
    if(is_ahb) {
      if(reg > 7) reg--;
      reg |= 0x08;
    }
    else reg |= 0x04;
  }
  CL_REG_MODIFY(RCC->CFGR, mask, reg << offset);
  return HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_pll_init(hdl_clock_mcu_t *clk, uint32_t mask, uint32_t value) {
  volatile uint32_t *rcc_reg = &RCC->PLLCFGR;
  if((clk->config->type == HDL_CLOCK_TYPE_PLL_I2S_N) || (clk->config->type == HDL_CLOCK_TYPE_PLL_I2S_R)) rcc_reg = &RCC->PLLI2SCFGR;
  CL_REG_MODIFY(*rcc_reg, mask, value);
  return HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_mco_init(hdl_clock_mcu_t *clk, uint8_t enable) {
  if(enable) {
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->config->clock_src;
    uint32_t div = clk->config->property.div;
    if((clock_src == NULL) || (div == 0) || (div > 5)) 
      return HDL_MODULE_FAULT;
    if(div > 1) div = (div - 2) | 0x4;
    if(clk->config->type == HDL_CLOCK_TYPE_MCO1) {
      CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO1PRE, div << 24);
      switch (clock_src->config->type) {
        case HDL_CLOCK_TYPE_IRC16M: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO1, 0 << 21); break;
        case HDL_CLOCK_TYPE_LXTAL: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO1, 1 << 21); break;
        case HDL_CLOCK_TYPE_HXTAL: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO1, 2 << 21); break;
        case HDL_CLOCK_TYPE_PLL_P: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO1, 3 << 21); break;
        default: return HDL_MODULE_FAULT;
      }
    }
    else {
      CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO2PRE, div << 27);
      switch (clock_src->config->type) {
        case HDL_CLOCK_TYPE_SYS_SEL: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO2, 0 << 30); break;
        case HDL_CLOCK_TYPE_PLL_I2S_R: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO2, 1 << 30); break;
        case HDL_CLOCK_TYPE_HXTAL: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO2, 2 << 30); break;
        case HDL_CLOCK_TYPE_PLL_P: CL_REG_MODIFY(RCC->CFGR, RCC_CFGR_MCO2, 3 << 30); break;
        default: return HDL_MODULE_FAULT;
      }
    }
    return HDL_MODULE_ACTIVE;
  } 
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_clock(const void *desc, uint8_t enable) {
  hdl_clock_mcu_t *clk = (hdl_clock_mcu_t *)desc;
  switch (clk->config->type) {
    case HDL_CLOCK_TYPE_HXTAL:
      if (enable) return _hdl_clock_osc_en(&RCC->CR, RCC_CR_HSEON, RCC_CR_HSERDY);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_LXTAL:
      if(enable) { /* todo: power dimain module dependesy */
        CL_REG_SET(RCC->APB1ENR, RCC_APB1ENR_PWREN);
        __DSB();
        if(!(PWR->CR & PWR_CR_DBP)) {
          uint32_t timer = 1000000;
          CL_REG_SET(PWR->CR, PWR_CR_DBP);
          while (timer-- && !(PWR->CR & PWR_CR_DBP));
          if(!(PWR->CR & PWR_CR_DBP)) return HDL_MODULE_FAULT;
          CL_REG_SET(RCC->BDCR, RCC_BDCR_LSEON);
        }
        /* todo timeout */
        if(!(RCC->BDCR & RCC_BDCR_LSERDY)) return HDL_MODULE_LOADING;
        return HDL_MODULE_ACTIVE;
      }
      CL_REG_CLEAR(RCC->BDCR, RCC_BDCR_LSEON);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC16M:
      if (enable) return _hdl_clock_osc_en(&RCC->CR, RCC_CR_HSION, RCC_CR_HSIRDY);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC32K:
      if (enable) return _hdl_clock_osc_en(&RCC->CSR, RCC_CSR_LSION, RCC_CSR_LSIRDY);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_PLL_SRC:
      if (enable) {
        hdl_clock_mcu_t *src = (hdl_clock_mcu_t *)clk->dependencies[0];
        if(src->config->type == HDL_CLOCK_TYPE_IRC16M) CL_REG_CLEAR(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC);
        else if (src->config->type == HDL_CLOCK_TYPE_HXTAL) CL_REG_SET(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC);
        else return HDL_MODULE_FAULT;
        uint32_t factor = clk->config->property.div;
        if((factor < 2) || (factor > 63)) return HDL_MODULE_FAULT;
        return _hdl_clock_pll_init(clk, RCC_PLLCFGR_PLLM, factor << 0);
      }
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_PLL_N:
      if (enable) {
        uint32_t factor = clk->config->property.mul;
        if((factor < 50) || (factor > 432)) return HDL_MODULE_FAULT;
        return _hdl_clock_pll_init(clk, RCC_PLLCFGR_PLLN, factor << 6);
      }
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_PLL_P:
      if (enable) {
        uint32_t factor = (clk->config->property.div >> 1) - 1;
        if(factor > 3) return HDL_MODULE_FAULT;
        return _hdl_clock_pll_init(clk, RCC_PLLCFGR_PLLP, factor << 16);
      }
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_PLL_Q:
      if (enable) {
        uint32_t factor = clk->config->property.div;
        if((factor < 2) || (factor > 15)) return HDL_MODULE_FAULT;      
        return _hdl_clock_pll_init(clk, RCC_PLLCFGR_PLLQ, factor << 24);
      }
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_SYS_SEL:
      return _hdl_clock_system(clk, enable);

    case HDL_CLOCK_TYPE_AHB:
      if (enable) return _hdl_bus_clock_cnf(clk, RCC_CFGR_HPRE, 8);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB1:
      if (enable) return _hdl_bus_clock_cnf(clk, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_Pos);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB2:
      if (enable) return _hdl_bus_clock_cnf(clk, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_Pos);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_PLL_I2S_N:
      if (enable) {
        uint32_t factor = clk->config->property.mul;
        if((factor < 50) || (factor > 432)) return HDL_MODULE_FAULT;      
        return _hdl_clock_pll_init(clk, RCC_PLLI2SCFGR_PLLI2SN, factor << 6);
      }
      return HDL_MODULE_UNLOADED;
    case HDL_CLOCK_TYPE_PLL_I2S_R:
      if (enable) {
        uint32_t factor = (clk->config->property.div >> 1) - 1;
        if((factor < 2) || (factor > 7)) return HDL_MODULE_FAULT;      
        return _hdl_clock_pll_init(clk, RCC_PLLI2SCFGR_PLLI2SR, factor << 28);
      }
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_APB1_TIMERS:
    case HDL_CLOCK_TYPE_APB2_TIMERS: {
      if(enable) return HDL_MODULE_ACTIVE;
      return HDL_MODULE_UNLOADED;
    }

    case HDL_CLOCK_TYPE_MCO1:
    case HDL_CLOCK_TYPE_MCO2:
      return _hdl_clock_mco_init(clk, enable);
    default:
      break;
  }
  return HDL_MODULE_FAULT;
}

static hdl_module_state_t _hdl_clock_pll(const void *desc, uint8_t enable) {
  if(enable) {
    hdl_clock_mcu_pll_t *pll = (hdl_clock_mcu_pll_t *)desc;
    hdl_clock_mcu_t *pll_n = (hdl_clock_mcu_t *)pll->dependencies[0];
    if(pll_n->config->type == HDL_CLOCK_TYPE_PLL_N) {
      hdl_clock_mcu_t *pll_p = (hdl_clock_mcu_t *)pll->dependencies[1];
      hdl_clock_mcu_t *pll_q = (hdl_clock_mcu_t *)pll->dependencies[2];
      if((pll_p->config->type == HDL_CLOCK_TYPE_PLL_P) && (pll_q->config->type == HDL_CLOCK_TYPE_PLL_Q))
        return _hdl_clock_osc_en(&RCC->CR, RCC_CR_PLLON, RCC_CR_PLLRDY);
    }
    else if (pll_n->config->type == HDL_CLOCK_TYPE_PLL_I2S_N) {
      hdl_clock_mcu_t *pll_r = (hdl_clock_mcu_t *)pll->dependencies[1];
      if(pll_r->config->type == HDL_CLOCK_TYPE_PLL_I2S_R)
        return _hdl_clock_osc_en(&RCC->CR, RCC_CR_PLLI2SON, RCC_CR_PLLI2SRDY);
    }
    return HDL_MODULE_FAULT;
  }
  return HDL_MODULE_UNLOADED;
}

const hdl_clock_iface_t hdl_clock_iface = {
  .init = &_hdl_clock,
  .get = &_hdl_get_clock
};

const hdl_module_base_iface_t hdl_clock_mcu_pll_iface = {
  .init = &_hdl_clock_pll
};
