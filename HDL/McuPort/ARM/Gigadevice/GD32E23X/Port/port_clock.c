#include "hdl_iface.h"

typedef struct {
  hdl_clock_freq_t freq;
} hdl_clock_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_clock_var_t, *((hdl_clock_mcu_t *)0)->obj_var, HDL_CLOCK_VAR_SIZE, port_clock.h);

#define IRC28M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC40K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define PLL_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)

static hdl_module_state_t _hdl_clock_osc_en(rcu_osci_type_enum osc, rcu_flag_enum stb_flag, uint32_t timeout) {
  rcu_osci_on(osc); /* Turn on oscillator */
  uint32_t stb_timer = timeout;
  FlagStatus osci_statable = RESET;
  while ((RESET == osci_statable) && (stb_timer--)) /* Wait until HXTAL will be stable */
    osci_statable = rcu_flag_get(stb_flag);
  if (osci_statable == RESET)
    return HDL_MODULE_FAULT;
  return HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_selector_pll(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if ((clk->dependencies == NULL) || (clk->dependencies[0] == NULL)) 
      return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clock_src_var = (hdl_clock_var_t *)clock_src->obj_var;
    hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;
    if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) {
      if (clk->config->property.div == 0 || clk->config->property.div > 16) return HDL_MODULE_FAULT;
      CL_REG_MODIFY(RCU_CFG1, RCU_CFG1_PREDV, (clk->config->property.div - 1) << 0);
      CL_REG_SET(RCU_CFG0, RCU_PLLSRC_HXTAL);
      hdl_clock_calc_div(&clock_src_var->freq, clk->config->property.div, &clock_var->freq);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC8M) {
      CL_REG_SET(RCU_CFG0, RCU_PLLSRC_IRC8M_DIV2);
      hdl_clock_calc_div(&clock_src_var->freq, 2, &clock_var->freq);
    }
    else {
      return HDL_MODULE_FAULT;
    }
    return HDL_MODULE_ACTIVE;
  }
  else {
    CL_REG_CLEAR(RCU_CFG0, RCU_CFG0_PLLSEL);
    CL_REG_MODIFY(RCU_CFG1, RCU_CFG1_PREDV, RCU_PLL_PREDV16);
    return HDL_MODULE_UNLOADED;
  }
}

static hdl_module_state_t _hdl_clock_pll(hdl_clock_mcu_t *clk, uint8_t enable) {
  while (enable) {
    if (clk->dependencies == NULL || clk->dependencies[0] == NULL) break;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clock_src_var = (hdl_clock_var_t *)clock_src->obj_var;
    hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;
    uint32_t pll_cnf = clk->config->property.mul;
    if((pll_cnf < 2) || (pll_cnf > 32)) break;
    pll_cnf -= (pll_cnf > 15)? 1: 2;
    pll_cnf = ((pll_cnf & 0x0F) << 18) | ((pll_cnf & 0x10) << (27 - 4));
    CL_REG_MODIFY(RCU_CFG0, (RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4), pll_cnf);
    if(_hdl_clock_osc_en(RCU_PLL_CK, RCU_FLAG_PLLSTB, PLL_STARTUP_TIMEOUT) != HDL_MODULE_ACTIVE) {
      rcu_osci_off(RCU_PLL_CK);
      break;
    }
    hdl_clock_calc_mul(&clock_src_var->freq, clk->config->property.mul, &clock_var->freq);
    return HDL_MODULE_ACTIVE;
  }
  CL_REG_CLEAR(RCU_CFG0, (RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4));
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_clock_system_switch(uint32_t src) {
  /* Register start pos 2 bit */
  static const uint32_t rcu_cfg0_scss_shift = 2;
  rcu_system_clock_source_config(src);
  uint32_t stb_cnt = CK_SYS_STARTUP_TIMEOUT;
  while((src != ((RCU_CFG0 & RCU_CFG0_SCSS) >> rcu_cfg0_scss_shift)) && stb_cnt--);
  return (src != ((RCU_CFG0 & RCU_CFG0_SCSS) >> rcu_cfg0_scss_shift)) ? HDL_MODULE_FAULT: HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_system(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if (clk->dependencies == NULL || 
        clk->dependencies[0] == NULL || 
        clk->dependencies[1] == NULL)
      return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[2];
    if(clock_src == NULL) clock_src = (hdl_clock_mcu_t *)clk->dependencies[1];
    hdl_clock_var_t *clock_src_var = (hdl_clock_var_t *)clock_src->obj_var;
    hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;
    hdl_clock_calc_div(&clock_src_var->freq, 1, &clock_var->freq);
    if(clock_src->config->type == HDL_CLOCK_TYPE_PLL) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_PLL);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_HXTAL);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC8M) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_IRC8M);
    }
    /* TODO: config clock mon */
  }
  _hdl_clock_system_switch(RCU_CKSYSSRC_IRC8M);
  return HDL_MODULE_UNLOADED;
}


static hdl_module_state_t _hdl_bus_clock_cnf(hdl_clock_mcu_t *clk, uint32_t bit_from, uint32_t bit_to, 
                                             uint32_t check_frec, hdl_clock_type_t base_clock) {
  uint32_t factor = clk->config->property.div;
  if(factor >= 64) factor >>= 1;
  if ((factor == 0) || (clk->dependencies == NULL) || (clk->dependencies[0] == NULL))
    return HDL_MODULE_FAULT;
  hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
  if(clock_src->config->type != base_clock) return HDL_MODULE_FAULT;
  hdl_clock_var_t *clock_src_var = (hdl_clock_var_t *)clock_src->obj_var;
  hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;

  hdl_clock_calc_div(&clock_src_var->freq, factor, &clock_var->freq);
  if((clock_var->freq.num / clock_var->freq.denom) > check_frec) return HDL_MODULE_FAULT;
  uint32_t div_cnf = 31 - __CLZ(factor);
  if (div_cnf) div_cnf = ((div_cnf - 1) << bit_from) | (1UL << bit_to);
  CL_REG_MODIFY(RCU_CFG0, BITS(bit_from, bit_to), div_cnf);
  return HDL_MODULE_ACTIVE;
}

static hdl_module_state_t hdl_clock_selector_rtc(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if (clk->dependencies == NULL || clk->dependencies[0] == NULL)
      return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clock_src_var = (hdl_clock_var_t *)clock_src->obj_var;
    hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;
    if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) {
      /* RTC is cloking by HXTAL / 32 */
      rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
      hdl_clock_calc_div(&clock_src_var->freq, 32, &clock_var->freq);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_LXTAL) {
      rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
      hdl_clock_calc_div(&clock_src_var->freq, 1, &clock_var->freq);
    }
    else if (clock_src->config->type == HDL_CLOCK_TYPE_IRC40K) {
      rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
      hdl_clock_calc_div(&clock_src_var->freq, 1, &clock_var->freq);
    }
    else {
      return HDL_MODULE_FAULT;
    }
    return HDL_MODULE_ACTIVE;
  }
  else {
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);
    return HDL_MODULE_UNLOADED;
  }
}

static hdl_module_state_t _hdl_clock_adc(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if (clk->dependencies == NULL || clk->dependencies[0] == NULL)
      return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clock_src_var = (hdl_clock_var_t *)clock_src->obj_var;
    hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;
    uint32_t rcu_cnf0 = RCU_CFG0;
    uint32_t rcu_cnf2 = RCU_CFG2;

    if(clock_src->config->type == HDL_CLOCK_TYPE_IRC28M) {
      if((clk->config->property.div != 1) && (clk->config->property.div != 2)) return HDL_MODULE_FAULT;
      rcu_cnf2 &= ~RCU_CFG2_ADCSEL;
      if(clk->config->property.div == 2) rcu_cnf2 &= ~RCU_CFG2_IRC28MDIV;
      else rcu_cnf2 |= RCU_CFG2_IRC28MDIV;
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_AHB) {
      if((clk->config->property.div != 3) && (clk->config->property.mul != 5) && 
         (clk->config->property.div != 7) && (clk->config->property.mul != 9)) return HDL_MODULE_FAULT;
      CL_REG_MODIFY(rcu_cnf0, RCU_CFG0_ADCPSC,((clk->config->property.div == 3)? 0: ((clk->config->property.div * 2) / 6)) << 14);
      rcu_cnf2 |= RCU_CFG2_ADCPSC2 | RCU_CFG2_ADCSEL;
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_APB2) {
      if((clk->config->property.div != 2) && (clk->config->property.mul != 4) && 
         (clk->config->property.div != 6) && (clk->config->property.mul != 8)) return HDL_MODULE_FAULT;
      CL_REG_MODIFY(rcu_cnf0, RCU_CFG0_ADCPSC,((clk->config->property.div == 2)? 0: ((clk->config->property.div + 1) / 3)) << 14);
      rcu_cnf2 |= RCU_CFG2_ADCSEL;
      rcu_cnf2 &= ~RCU_CFG2_ADCPSC2;
    }
    else return HDL_MODULE_FAULT;
    hdl_clock_calc_div(&clock_src_var->freq, clk->config->property.div, &clock_var->freq);
    if((clock_var->freq.num / clock_var->freq.denom)> 28000000) return HDL_MODULE_FAULT;
    RCU_CFG0 = rcu_cnf0;
    RCU_CFG2 = rcu_cnf2;
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

hdl_module_state_t __hdl_clock(const void *desc, const uint8_t enable) {
  hdl_clock_mcu_t *clk = (hdl_clock_mcu_t *)desc;
  hdl_clock_var_t *clock_var = (hdl_clock_var_t *)clk->obj_var;
  clock_var->freq.denom = 1;
  switch (clk->config->type) {
    case HDL_CLOCK_TYPE_RTC_SEL:
      return hdl_clock_selector_rtc(clk, enable);

    case HDL_CLOCK_TYPE_HXTAL:
      clock_var->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(RCU_HXTAL, RCU_FLAG_HXTALSTB, HXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_LXTAL:
      clock_var->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(RCU_LXTAL, RCU_FLAG_LXTALSTB, LXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC8M:
      clock_var->freq.num = 8000000;
      if (enable) return _hdl_clock_osc_en(RCU_IRC8M, RCU_FLAG_IRC8MSTB, IRC8M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_PLL_SEL:
      return _hdl_clock_selector_pll(clk, enable);

    case HDL_CLOCK_TYPE_PLL:
      return _hdl_clock_pll(clk, enable);

    case HDL_CLOCK_TYPE_SYS_SEL:
      return _hdl_clock_system(clk, enable);

    case HDL_CLOCK_TYPE_AHB:
      if (enable) {
        return _hdl_bus_clock_cnf(clk, 4, 7, MAX_SYS_CLOCK, HDL_CLOCK_TYPE_SYS_SEL);
      }
      rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV512);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB1:
      if (enable) return _hdl_bus_clock_cnf(clk, 10, 12, MAX_APB1_CLOCK, HDL_CLOCK_TYPE_AHB);
      rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV16);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB2:
      if (enable) return _hdl_bus_clock_cnf(clk, 13, 15, MAX_APB2_CLOCK, HDL_CLOCK_TYPE_AHB);
      rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV16);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC28M:
      clock_var->freq.num = 28000000;
      if (enable) return _hdl_clock_osc_en(RCU_IRC28M, RCU_FLAG_IRC28MSTB, IRC28M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC40K:
      clock_var->freq.num = 40000;
      if (enable) return _hdl_clock_osc_en(RCU_IRC40K, RCU_FLAG_IRC40KSTB, IRC40K_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_ADC:
      return _hdl_clock_adc(clk, enable);

    default:
      break;
  }
  return HDL_MODULE_FAULT;
}
