#include "hdl_portable.h"
#include "Macros.h"

typedef struct {
  hdl_module_t module;
  const hdl_clock_config_t *config;
  hdl_clock_freq_t freq;
} hdl_clock_private_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_clock_private_t, hdl_clock_t, HDL_CLOCK_PRV_SIZE, port_clock.h);

#define PLL_STARTUP_TIMEOUT    ((uint32_t)0xFFFFFFFF)
#define IRC48M_STARTUP_TIMEOUT    ((uint32_t)0xFFFFFFFF)
#define IRC12M_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define IRC10K_STARTUP_TIMEOUT    ((uint32_t)0xFFFF)
#define LXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFFFFFF)
#define HXTAL_STARTUP_TIMEOUT     ((uint32_t)0xFFFF)

#define APB2_MAX_FREQ             120000000UL
#define APB1_MAX_FREQ             60000000UL

static hdl_module_state_t _hdl_clock_osc_en(uint32_t osc, uint32_t stat, uint32_t timeout) {
  SYS_UnlockReg();
  HDL_REG_SET(CLK->PWRCTL, osc);
  SYS_LockReg();
  while ((!HDL_REG_CHECK(CLK->STATUS, stat)) && (timeout--)); /* Wait until HXTAL will be stable */
  if (!HDL_REG_CHECK(CLK->STATUS, stat))
    return HDL_MODULE_FAULT;
  return HDL_MODULE_ACTIVE;
}

#define CHECK_PLL_REF_PSC_VALID(val)   ((val > 0) && (val <= 32))

static hdl_module_state_t _hdl_clock_pll_ref(hdl_clock_private_t *clk, uint8_t enable) {
  while (enable) {
    if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) break;
    hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
    if(!CHECK_PLL_REF_PSC_VALID(clk->config->property.div)) break;
    hdl_clock_calc_div(&clock_src->freq, clk->config->property.div, &clk->freq);
    uint32_t val = clk->freq.num / clk->freq.denom;
    if((val < 4000000) || (val > 8000000)) break;
    SYS_UnlockReg();
    HDL_REG_SET(CLK->PLLCTL, CLK_PLLCTL_OE_Msk | CLK_PLLCTL_STBSEL_Msk);
    val = ((clk->config->property.div - 1) & 0x1f) << CLK_PLLCTL_INDIV_Pos;
    HDL_REG_MODIFY(CLK->PLLCTL, CLK_PLLCTL_INDIV_Msk, val);
    if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) HDL_REG_CLEAR(CLK->PLLCTL, CLK_PLLCTL_PLLSRC_Msk);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_IRC12M) HDL_REG_SET(CLK->PLLCTL, CLK_PLLCTL_PLLSRC_Msk);
    else break;
    SYS_LockReg();
    return HDL_MODULE_ACTIVE;
  }
  SYS_LockReg();
  return HDL_MODULE_UNLOADED;
}

#define CHECK_PLL_VCO_PSC_VALID(val)   (!(val & 1) && (val >= 4) && (val <= 1028))

static hdl_module_state_t _hdl_clock_pll_vco(hdl_clock_private_t *clk, uint8_t enable) {
  while (enable) {
    if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) break;
    hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
    if(clock_src->config->type != HDL_CLOCK_TYPE_PLL_REF) break;
    if(!CHECK_PLL_VCO_PSC_VALID(clk->config->property.mul)) break;
    hdl_clock_calc_mul(&clock_src->freq, clk->config->property.mul, &clk->freq);
    uint32_t val = clk->freq.num / clk->freq.denom;
    if((val < 200000000) || (val > 500000000)) break;
    SYS_UnlockReg();
    val = (((clk->config->property.mul >> 1) - 2) & 0x1ff) << CLK_PLLCTL_FBDIV_Pos;
    HDL_REG_MODIFY(CLK->PLLCTL, CLK_PLLCTL_FBDIV_Msk, val);
    SYS_LockReg();
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

#define CHECK_PLL_PSC_VALID(val)   ((val == 1) || (val == 2) || (val == 4))

static hdl_module_state_t _hdl_clock_pll(hdl_clock_private_t *clk, uint8_t enable, uint32_t timeout) {
  while (enable) {
    if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) break;
    hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
    if(clock_src->config->type != HDL_CLOCK_TYPE_PLL_VCO) break;
    if(!CHECK_PLL_PSC_VALID(clk->config->property.div)) break;
    hdl_clock_calc_div(&clock_src->freq, clk->config->property.div, &clk->freq);
    uint32_t val = clk->freq.num / clk->freq.denom;
    if((val < 50000000) || (val > 500000000)) break;
    SYS_UnlockReg();
    val = ((clk->config->property.div - 1) & 0x3) << CLK_PLLCTL_OUTDIV_Pos;
    HDL_REG_MODIFY(CLK->PLLCTL, CLK_PLLCTL_OUTDIV_Msk, val);
    HDL_REG_CLEAR(CLK->PLLCTL, CLK_PLLCTL_PD_Msk | CLK_PLLCTL_OE_Msk);
    while ((!HDL_REG_CHECK(CLK->STATUS, CLK_STATUS_PLLSTB_Msk)) && (timeout--)); /* Wait until PLL will be stable */
    SYS_LockReg();
    return HDL_MODULE_ACTIVE;
  }
  SYS_UnlockReg();
  HDL_REG_SET(CLK->PLLCTL, CLK_PLLCTL_OE_Msk);
  SYS_LockReg();
  return HDL_MODULE_UNLOADED;
}

#define CHECK_SYS_PSC_VALID(val)   ((val == 1) || (val == 2) || (val == 4))

static hdl_module_state_t _hdl_clock_sys(hdl_clock_private_t *clk, uint8_t enable) {
  while (enable) {
    if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) break;
    hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
    if(!CHECK_SYS_PSC_VALID(clk->config->property.div)) break;
    hdl_clock_calc_div(&clock_src->freq, clk->config->property.div, &clk->freq);
    uint32_t val = clk->freq.num / clk->freq.denom;
    if(val > 200000000) break;
    SYS_UnlockReg();
    val = ((clk->config->property.div - 1) & 0xf) << CLK_CLKDIV0_HCLKDIV_Pos;
    HDL_REG_MODIFY(CLK->PLLCTL, CLK_CLKDIV0_HCLKDIV_Msk, val);
    if(clock_src->config->type == HDL_CLOCK_TYPE_PLL) HDL_REG_MODIFY(CLK->CLKSEL0, CLK_CLKSEL0_HCLKSEL_Msk, CLK_CLKSEL0_HCLKSEL_PLL);
    else if(clock_src->config->type == HDL_CLOCK_TYPE_HXTAL) HDL_REG_MODIFY(CLK->CLKSEL0, CLK_CLKSEL0_HCLKSEL_Msk, CLK_CLKSEL0_HCLKSEL_HXT);
    else if (clock_src->config->type == HDL_CLOCK_TYPE_LXTAL) HDL_REG_MODIFY(CLK->CLKSEL0, CLK_CLKSEL0_HCLKSEL_Msk, CLK_CLKSEL0_HCLKSEL_LXT);
    else if (clock_src->config->type == HDL_CLOCK_TYPE_IRC10K) HDL_REG_MODIFY(CLK->CLKSEL0, CLK_CLKSEL0_HCLKSEL_Msk, CLK_CLKSEL0_HCLKSEL_LIRC);
    else if (clock_src->config->type == HDL_CLOCK_TYPE_IRC12M) HDL_REG_MODIFY(CLK->CLKSEL0, CLK_CLKSEL0_HCLKSEL_Msk, CLK_CLKSEL0_HCLKSEL_HIRC);
    else break;
    SYS_LockReg();
    if(HDL_REG_CHECK(CLK->STATUS, CLK_STATUS_CLKSFAIL_Msk)) break;
    return HDL_MODULE_ACTIVE;
  }
  SYS_UnlockReg();
  HDL_REG_MODIFY(CLK->CLKSEL0, CLK_CLKSEL0_HCLKSEL_Msk, CLK_CLKSEL0_HCLKSEL_HIRC);
  SYS_LockReg();
  return HDL_MODULE_UNLOADED;
}

#define CHECK_APB_PSC_VALID(val)   ((val == 1) || (val == 2) || (val == 4) || (val == 8) || (val == 16))

static hdl_module_state_t _hdl_clock_apb(hdl_clock_private_t *clk, uint8_t enable, uint8_t bus_no) {
  while (enable) {
    if (clk->module.dependencies == NULL || clk->module.dependencies[0] == NULL) break;
    hdl_clock_private_t *clock_src = (hdl_clock_private_t *)clk->module.dependencies[0];
    if(clock_src->config->type != HDL_CLOCK_TYPE_SYS) break;
    if(!CHECK_APB_PSC_VALID(clk->config->property.div)) break;
    hdl_clock_calc_div(&clock_src->freq, clk->config->property.div, &clk->freq);
    // uint32_t val = clk->freq.num / clk->freq.denom;
    // if((val < 4000000) || (val > 8000000)) break; ????
    SYS_UnlockReg();
    uint32_t val = (31 - __CLZ(clk->config->property.div));
    if(bus_no == 0)
      HDL_REG_MODIFY(CLK->PCLKDIV, CLK_PCLKDIV_APB0DIV_Msk, val << CLK_PCLKDIV_APB0DIV_Pos);
    else
      HDL_REG_MODIFY(CLK->PCLKDIV, CLK_PCLKDIV_APB1DIV_Msk, val << CLK_PCLKDIV_APB1DIV_Pos);
    SYS_LockReg();
    return HDL_MODULE_ACTIVE;
  }
  SYS_LockReg();
  return HDL_MODULE_UNLOADED;
}

hdl_module_state_t hdl_clock(void *desc, uint8_t enable) {
  hdl_clock_private_t *clk = (hdl_clock_private_t *)desc;
  clk->freq.denom = 1;
  switch (clk->config->type) {
    case HDL_CLOCK_TYPE_HXTAL:
      
      clk->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_HXTEN_Msk, CLK_STATUS_HXTSTB_Msk, HXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_LXTAL:
      clk->freq.num = clk->config->property.freq;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_LXTEN_Msk, CLK_STATUS_LXTSTB_Msk, LXTAL_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC12M:
      clk->freq.num = 12000000;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_HIRCEN_Msk, CLK_STATUS_HIRCSTB_Msk, IRC12M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;
    
    case HDL_CLOCK_TYPE_IRC48M:
      clk->freq.num = 48000000;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_HIRC48MEN_Msk, CLK_STATUS_HIRC48MSTB_Msk, IRC48M_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;

    case HDL_CLOCK_TYPE_IRC10K:
      clk->freq.num = 10000;
      if (enable) return _hdl_clock_osc_en(CLK_PWRCTL_LIRCEN_Msk, CLK_STATUS_LIRCSTB_Msk, IRC10K_STARTUP_TIMEOUT);
      return HDL_MODULE_UNLOADED;


    case HDL_CLOCK_TYPE_PLL_REF:
      return _hdl_clock_pll_ref(clk, enable);

    case HDL_CLOCK_TYPE_PLL_VCO:
      return _hdl_clock_pll_vco(clk, enable);

    case HDL_CLOCK_TYPE_PLL:
      return _hdl_clock_pll(clk, enable, PLL_STARTUP_TIMEOUT);

    case HDL_CLOCK_TYPE_SYS:
      return _hdl_clock_sys(clk, enable);

    case HDL_CLOCK_TYPE_APB0:
      return _hdl_clock_apb(clk, enable, 0);

    case HDL_CLOCK_TYPE_APB1:
      return _hdl_clock_apb(clk, enable, 1);

    default:
      break;
  }
  return HDL_MODULE_FAULT;
}

void hdl_get_clock(hdl_clock_t *clock, hdl_clock_freq_t *freq) {
  if(freq != NULL) {
    freq->num = 0;
    freq->denom = 1;
    hdl_clock_private_t *clk = (hdl_clock_private_t *)clock;
    if((clk != NULL) && (hdl_state(&clk->module) != HDL_MODULE_FAULT)) {
      freq->num = clk->freq.num;
      freq->denom = clk->freq.denom;
    }
  }
}
