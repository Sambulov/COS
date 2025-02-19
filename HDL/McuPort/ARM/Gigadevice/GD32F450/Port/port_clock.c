#include "hdl_iface.h"

typedef struct {
  hdl_clock_freq_t freq;
} hdl_clock_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_clock_var_t, *((hdl_clock_mcu_t *)0)->obj_var, HDL_CLOCK_VAR_SIZE, port_clock.h);

#define IRC48M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC32K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)
#define CK_SYS_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define PLL_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)
#define PMU_STARTUP_TIMEOUT       ((uint32_t)0xFFFF)

#define MAX_SYS_CLOCK             240000000UL
#define PLL_MAX_FREQ              MAX_SYS_CLOCK
#define APB2_MAX_FREQ             120000000UL
#define APB1_MAX_FREQ             60000000UL

/*!
    \brief      get flag state
    \param[in]  flag:
      \arg        PMU_FLAG_WAKEUP: wakeup flag
      \arg        PMU_FLAG_STANDBY: standby flag
      \arg        PMU_FLAG_LVD: lvd flag
      \arg        PMU_FLAG_BLDORF: backup SRAM LDO ready flag
      \arg        PMU_FLAG_LDOVSRF: LDO voltage select ready flag
      \arg        PMU_FLAG_HDRF: high-driver ready flag
      \arg        PMU_FLAG_HDSRF: high-driver switch ready flag
      \arg        PMU_FLAG_LDRF: low-driver mode ready flag
    \retval     HDL_MODULE_FAULT
    \retval     HDL_MODULE_ACTIVE
*/
static hdl_module_state_t _hdl_power_manager_wait_stable(uint32_t pmu_flag){
  uint32_t stb_timer = PMU_STARTUP_TIMEOUT;
  FlagStatus osci_statable = RESET;
  while ((RESET == osci_statable) && (stb_timer--))
    osci_statable = pmu_flag_get(pmu_flag);
  if (osci_statable == RESET)
    return HDL_MODULE_FAULT;
  return HDL_MODULE_ACTIVE;
}
/* TODO: This function have to be in other module */
static hdl_module_state_t _hdl_power_manager_high_frequency(uint8_t enable){
  if(enable){
      /* Enable the high-drive to extend the clock frequency to 240 Mhz */
      PMU_CTL |= PMU_CTL_LDOVS;
      /* Enable the high-drive to extend the clock frequency to 240 Mhz */
      PMU_CTL |= PMU_CTL_HDEN;
      if(_hdl_power_manager_wait_stable(PMU_FLAG_HDRF) != HDL_MODULE_ACTIVE)
        return HDL_MODULE_FAULT;
      /* select the high-drive mode */
      PMU_CTL |= PMU_CTL_HDS;
      if(_hdl_power_manager_wait_stable(PMU_CS_HDSRF) != HDL_MODULE_ACTIVE)
        return HDL_MODULE_FAULT;
      return HDL_MODULE_ACTIVE;
  }
  PMU_CTL &= ~(PMU_CTL_LDOVS | PMU_CTL_HDEN | PMU_CTL_HDS);
  return HDL_MODULE_UNLOADED;
}

// hdl_module_state_t hdl_clock_pll_i2s(void *desc, uint8_t enable) {
//   return HDL_MODULE_UNLOADED;
// }

// hdl_module_state_t hdl_clock_pll_sai(void *desc, uint8_t enable) {
//   return HDL_MODULE_UNLOADED;
// }


static hdl_module_state_t _hdl_clock_system_switch(uint32_t src) {
  rcu_system_clock_source_config(src);
  uint32_t stb_cnt = CK_SYS_STARTUP_TIMEOUT;
  src <<= 2;
  while((src != (RCU_CFG0 & RCU_CFG0_SCSS)) && stb_cnt--) ;
  return (src != (RCU_CFG0 & RCU_CFG0_SCSS))? HDL_MODULE_FAULT: HDL_MODULE_ACTIVE;
}

static hdl_module_state_t _hdl_clock_system(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if (clk->dependencies == NULL || 
        clk->dependencies[0] == NULL || 
        clk->dependencies[1] == NULL)
      return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[2];
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
    hdl_clock_var_t *src_var = (hdl_clock_var_t *)clock_src->obj_var;
    if(clock_src == NULL) clock_src = (hdl_clock_mcu_t *)clk->dependencies[1];
    hdl_clock_calc_div(&src_var->freq, 1, &clk_var->freq);
    if(clock_src->config->type == HDL_CLOCK_TYPE_PLL_P) {
      _hdl_power_manager_high_frequency(HDL_TRUE);
      return _hdl_clock_system_switch(RCU_CKSYSSRC_PLLP);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_HXTAL);
    }
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC16M) {
      return _hdl_clock_system_switch(RCU_CKSYSSRC_IRC16M);
    }
    /* TODO: config clock mon */
  }
  _hdl_clock_system_switch(RCU_CKSYSSRC_IRC16M);
  _hdl_power_manager_high_frequency(HDL_FALSE);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_bus_clock_cnf(hdl_clock_mcu_t *clk, uint32_t bit_from, uint32_t bit_to, 
                                             uint32_t check_frec, hdl_clock_type_t base_clock) {
  uint32_t factor = clk->config->property.div;
  if(factor >= 64) factor >>= 1;

  if ((factor == 0) || (clk->dependencies == NULL) || (clk->dependencies[0] == NULL))
    return HDL_MODULE_FAULT;
  hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
  hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
  hdl_clock_var_t *src_var = (hdl_clock_var_t *)clock_src->obj_var;
  if(clock_src->config->type != base_clock)
    return HDL_MODULE_FAULT;
  hdl_clock_calc_div(&src_var->freq, factor, &clk_var->freq);
  if((clk_var->freq.num / clk_var->freq.denom) > check_frec) return HDL_MODULE_FAULT;
  uint32_t div_cnf = 31 - __CLZ(factor);
  if (div_cnf)
    div_cnf = ((div_cnf - 1) << bit_from) | (1UL << bit_to);
  HDL_REG_MODIFY(RCU_CFG0, BITS(bit_from, bit_to), div_cnf);
  return HDL_MODULE_ACTIVE;
}

// hdl_module_state_t hdl_clock_selector_rtc(void *desc, uint8_t enable) {
//   if (enable) {
//     hdl_clock_mcu_t *clock = (hdl_clock_mcu_t *)desc;
//     if (clock->dependencies == NULL || clock->dependencies[0] == NULL)
//       return HDL_MODULE_FAULT;
//     hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clock->dependencies[0];
//     if(clock_src->module.init == &hdl_clock_hxtal) {
//       /* RTC is cloking by HXTAL / 32 */
//       rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
//       hdl_clock_calc_div(clock, clock_src, 32);
//     }
//     else if(clock_src->module.init == &hdl_clock_lxtal) {
//       rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
//       hdl_clock_calc_div(clock, clock_src, 1);
//     }
//     else if (clock_src->module.init == &hdl_clock_irc40k) {
//       rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
//       hdl_clock_calc_div(clock, clock_src, 1);
//     }
//     else {
//       return HDL_MODULE_FAULT;
//     }
//     return HDL_MODULE_ACTIVE;
//   }
//   else {
//     rcu_rtc_clock_config(RCU_RTCSRC_NONE);
//     return HDL_MODULE_UNLOADED;
//   }
// }


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

static hdl_module_state_t _hdl_clock_pll_init(hdl_clock_mcu_t *clk, uint32_t max_freq, 
                                              uint32_t factor, uint32_t bits_from, uint32_t bits_to,
                                              hdl_clock_type_t base_clock) {
  (void)factor;
  while (HDL_TRUE) {
    if (clk->dependencies == NULL || clk->dependencies[0] == NULL) break;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
    hdl_clock_var_t *src_var = (hdl_clock_var_t *)clock_src->obj_var;
      if(clock_src->config->type != base_clock) break;
    if(max_freq) {
      hdl_clock_calc_mul(&src_var->freq, clk->config->property.mul, &clk_var->freq); //TODO: test ????
      if((clk_var->freq.num / clk_var->freq.denom) > max_freq) break;
    }
    else {
      hdl_clock_calc_div(&src_var->freq, clk->config->property.div, &clk_var->freq);
    }
    HDL_REG_MODIFY(RCU_PLL, BITS(bits_from, bits_to), (clk->config->property.div) << bits_from);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_clock_selector_pll(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if (clk->dependencies == NULL || clk->dependencies[0] == NULL) return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *clock_src = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
    hdl_clock_var_t *src_var = (hdl_clock_var_t *)clock_src->obj_var;
    if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) HDL_REG_SET(RCU_PLL, RCU_PLLSRC_HXTAL);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC16M) HDL_REG_CLEAR(RCU_PLL, RCU_PLLSRC_HXTAL);
    else return HDL_MODULE_FAULT;
    hdl_clock_calc_div(&src_var->freq, 1, &clk_var->freq);
    if(CHECK_PLL_PSC_VALID(clk->config->property.div))
      return _hdl_clock_pll_init(clk, 0, clk->config->property.div, 0, 5, clock_src->config->type);
  }
  else {
    // todo: pll disable
    HDL_REG_MODIFY(RCU_PLL, RCU_PLL_PLLPSC, RCU_PLLPSC_DIV_MAX);
  }
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_clock_pll_vco(hdl_clock_mcu_t *clk, uint8_t enable) {
  if(enable) {
    uint32_t ss_modulation_inc = 0U;
    uint32_t ss_modulation_reg = RCU_PLLSSCTL;
    /* calculate the minimum factor of PLLN */
    if((ss_modulation_reg & RCU_PLLSSCTL_SSCGON) == RCU_PLLSSCTL_SSCGON) {
        if((ss_modulation_reg & RCU_SS_TYPE_DOWN) == RCU_SS_TYPE_DOWN) {
            ss_modulation_inc += RCU_SS_MODULATION_DOWN_INC;
        } else {
            ss_modulation_inc += RCU_SS_MODULATION_CENTER_INC;
        }
    }
    if(CHECK_PLL_N_VALID(clk->config->property.mul, ss_modulation_inc))
      if(_hdl_clock_pll_init(clk, -1, (clk->config->property.mul << 6), 6, 14, HDL_CLOCK_TYPE_PLL_SEL) == HDL_MODULE_ACTIVE)
        return _hdl_clock_osc_en(RCU_PLL_CK, RCU_FLAG_PLLSTB, PLL_STARTUP_TIMEOUT);
  }
  HDL_REG_MODIFY(RCU_PLL, RCU_PLL_PLLN, RCU_PLLN_MUL_MIN << 6);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_apb_timers_clock_cnf(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if ((clk->dependencies == NULL) || 
       (clk->dependencies[0] == NULL) || 
       ((clk->config->property.mul != 2) && (clk->config->property.mul != 4))) return HDL_MODULE_FAULT;
    rcu_timer_clock_prescaler_config((clk->config->property.mul == 4)? RCU_TIMER_PSC_MUL4: RCU_TIMER_PSC_MUL2);
    hdl_clock_mcu_t *apb = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_mcu_t *ahb = (hdl_clock_mcu_t *)apb->dependencies[0];
    hdl_clock_var_t *apb_var = (hdl_clock_var_t *)apb->obj_var;
    hdl_clock_var_t *ahb_var = (hdl_clock_var_t *)ahb->obj_var;
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
    if(apb->config->property.div == 1) {
      hdl_clock_calc_mul(&ahb_var->freq, 1, &clk_var->freq);
    }
    else 
      hdl_clock_calc_mul(&apb_var->freq, clk->config->property.mul, &clk_var->freq);
    return HDL_MODULE_ACTIVE;
  }
  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL2);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_adc_clock_cnf(hdl_clock_mcu_t *clk, uint8_t enable) {
  if (enable) {
    if ((clk->dependencies == NULL) || (clk->dependencies[0] == NULL)) return HDL_MODULE_FAULT;
    hdl_clock_mcu_t *src_clk = (hdl_clock_mcu_t *)clk->dependencies[0];
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
    hdl_clock_var_t *src_var = (hdl_clock_var_t *)src_clk->obj_var;
      volatile uint32_t reg = 0;
    if(src_clk->config->type == HDL_CLOCK_TYPE_AHB) {
      if((clk->config->property.div != 5) && (clk->config->property.mul != 6) && 
         (clk->config->property.div != 10) && (clk->config->property.mul != 20)) return HDL_MODULE_FAULT;
      reg = 0b100 + ((clk->config->property.div == 10)? 2: (clk->config->property.div / 6));
    } else if (src_clk->config->type == HDL_CLOCK_TYPE_APB2) {
      if((clk->config->property.div != 2) && (clk->config->property.mul != 4) && 
         (clk->config->property.div != 6) && (clk->config->property.mul != 8)) return HDL_MODULE_FAULT;
      reg = 0b000 + ((clk->config->property.div == 8)? 3: (clk->config->property.div / 3));
    }
    else return HDL_MODULE_FAULT;
    hdl_clock_calc_div(&src_var->freq, clk->config->property.div, &clk_var->freq);
    if((clk_var->freq.num / clk_var->freq.denom)> 40000000) return HDL_MODULE_FAULT;
    rcu_periph_clock_enable(RCU_ADC0);
    HDL_REG_MODIFY(ADC_SYNCCTL, ADC_SYNCCTL_ADCCK, reg << 16);
    rcu_periph_clock_disable(RCU_ADC0);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_clock(const void *desc, uint8_t enable) {
  hdl_clock_mcu_t *clk = (hdl_clock_mcu_t *)desc;
  hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
  clk_var->freq.denom = 1;
  switch (clk->config->type) {
    case HDL_CLOCK_TYPE_HXTAL:
      clk_var->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(RCU_HXTAL, RCU_FLAG_HXTALSTB, HXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_LXTAL:
      clk_var->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(RCU_LXTAL, RCU_FLAG_LXTALSTB, LXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC16M:
      clk_var->freq.num = 16000000;
      if (enable) return _hdl_clock_osc_en(RCU_IRC16M, RCU_FLAG_IRC16MSTB, IRC16M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_PLL_SEL:
      return _hdl_clock_selector_pll(clk, enable);

    case HDL_CLOCK_TYPE_PLL_VCO:
      return _hdl_clock_pll_vco(clk, enable);

    case HDL_CLOCK_TYPE_PLL_P:
      if(enable && CHECK_PLL_P_VALID(clk->config->property.div))
        return _hdl_clock_pll_init(clk, 0, (((clk->config->property.div >> 1) - 1U) << 16), 16, 17, HDL_CLOCK_TYPE_PLL_VCO);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_PLL_Q:
      if(enable && CHECK_PLL_Q_VALID(clk->config->property.div))
        return _hdl_clock_pll_init(clk, 0, (clk->config->property.div << 24), 24, 27, HDL_CLOCK_TYPE_PLL_VCO);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_SYS_SEL:
      return _hdl_clock_system(clk, enable);

    case HDL_CLOCK_TYPE_AHB:
      if (enable) {
        return _hdl_bus_clock_cnf(clk, 4, 7, MAX_SYS_CLOCK, HDL_CLOCK_TYPE_SYS_SEL);
      }
      rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV512);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB1:
      if (enable) return _hdl_bus_clock_cnf(clk, 10, 12, APB1_MAX_FREQ, HDL_CLOCK_TYPE_AHB);
      rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV16);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB2:
      if (enable) return _hdl_bus_clock_cnf(clk, 13, 15, APB2_MAX_FREQ, HDL_CLOCK_TYPE_AHB);
      rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV16);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_APB1_TIMERS:
    case HDL_CLOCK_TYPE_APB2_TIMERS:
      return _hdl_apb_timers_clock_cnf(clk, enable);

    case HDL_CLOCK_TYPE_ADC:
      return _hdl_adc_clock_cnf(clk, enable);

    case HDL_CLOCK_TYPE_IRC48M:
      clk_var->freq.num = 48000000;
      if (enable) return _hdl_clock_osc_en(RCU_IRC48M, RCU_FLAG_IRC48MSTB, IRC48M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC40K:
      clk_var->freq.num = 40000;
      if (enable) return _hdl_clock_osc_en(RCU_IRC32K, RCU_FLAG_IRC32KSTB, IRC32K_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    default:
      break;
  }
  return HDL_MODULE_FAULT;
}

static void _hdl_get_clock(const void *desc, hdl_clock_freq_t *freq) {
  hdl_clock_mcu_t *clock = (hdl_clock_mcu_t *)desc;
  if(freq != NULL) {
    freq->num = 0;
    freq->denom = 1;
    hdl_clock_mcu_t *clk = (hdl_clock_mcu_t *)clock;
    hdl_clock_var_t *clk_var = (hdl_clock_var_t *)clk->obj_var;
    if((clk != NULL) && (hdl_state(clk) != HDL_MODULE_FAULT)) {
      freq->num = clk_var->freq.num;
      freq->denom = clk_var->freq.denom;
    }
  }
}

const hdl_clock_iface_t hdl_clock_iface = {
  .init = &_hdl_clock,
  .get = &_hdl_get_clock
};
