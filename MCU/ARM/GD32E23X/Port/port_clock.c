#include "hdl_portable.h"

/* TODO: Does flash depend on clock or not? Should you set flash clock before clock? */

/* Value of HXTAL, we can`t get this value from register, this value depends on dts*/
static uint32_t hxtal_mhz = 0;
static const uint32_t irc8m_mhz = 8000000;
static const uint32_t lxtal_mhz = 32768;
static const uint32_t irc40k_mhz = 40000;
static const uint32_t irc28m_mhz = 28000000;

#define INVAID_VALUE  ((uint32_t)0xffffffff)

#define IS_POWER_OF_TWO(x) ((x) && !((x) & ((x)-1)))
/*
* Array provides relate between integer prescaler value and register bit field,
* where index it,s prescaler value. If value in the array equal INVAID_VALUE, is`s ERROR
*/
static const uint32_t gd_rcu_pll_prescaler_array[] = {INVAID_VALUE, RCU_PLL_PREDV1, RCU_PLL_PREDV2, RCU_PLL_PREDV3,
  RCU_PLL_PREDV4, RCU_PLL_PREDV5, RCU_PLL_PREDV6, RCU_PLL_PREDV7, RCU_PLL_PREDV8, RCU_PLL_PREDV9, RCU_PLL_PREDV10,
  RCU_PLL_PREDV11, RCU_PLL_PREDV12, RCU_PLL_PREDV13, RCU_PLL_PREDV14, RCU_PLL_PREDV15};

/*
* Array provides relate between integer multiply coefficient value and register bit field,
* where index it,s prescaler value. If value in the array equal INVAID_VALUE, is`s ERROR
*/
  static const uint32_t gd_rcu_pll_multiply_coefficient_array[] = {INVAID_VALUE, INVAID_VALUE, 
    RCU_PLL_MUL2, RCU_PLL_MUL3, RCU_PLL_MUL4, RCU_PLL_MUL5, RCU_PLL_MUL6, RCU_PLL_MUL7, RCU_PLL_MUL8,
    RCU_PLL_MUL9, RCU_PLL_MUL10, RCU_PLL_MUL11, RCU_PLL_MUL12, RCU_PLL_MUL13, RCU_PLL_MUL14, RCU_PLL_MUL15,
    RCU_PLL_MUL16, RCU_PLL_MUL17, RCU_PLL_MUL18, RCU_PLL_MUL19, RCU_PLL_MUL20, RCU_PLL_MUL21, RCU_PLL_MUL22,
    RCU_PLL_MUL23, RCU_PLL_MUL24, RCU_PLL_MUL25, RCU_PLL_MUL26, RCU_PLL_MUL27, RCU_PLL_MUL28, RCU_PLL_MUL29,
    RCU_PLL_MUL30, RCU_PLL_MUL31, RCU_PLL_MUL32};

/*!
    \brief          This macros checks correct APB1 and APB2 prescaler value
    \param[in]      x - APB1 prescaler (1, 2, 4, 8, 16)
    \param[in]      result - output value
    \return
      \retval         HDL_HW_INIT_OK - OK
      \retval         HDL_HW_INIT_FAILED - ERROR
 */
#define APB1_APB2_PRESCALER_ASSERT(x, result) \
  do                                          \
  {                                           \
    if ((x) < 1 || (x) > 16)                  \
    {                                         \
      (result) = HDL_HW_INIT_FAILED;          \
    }                                         \
    else                                      \
    {                                         \
      if (IS_POWER_OF_TWO(x))                 \
      {                                       \
        (result) = HDL_HW_INIT_OK;            \
      }                                       \
      else                                    \
      {                                       \
        (result) = HDL_HW_INIT_FAILED;        \
      }                                       \
    }                                         \
  } while (0)

#define APB1_PRESCALER_CONVERT_INTEGER_TO_BIT_FIELD(x, result) \
  do                                                           \
  {                                                            \
    if ((x) == 1)                                              \
      (result) = RCU_APB1_CKAHB_DIV1;                          \
    else if ((x) == 2)                                         \
      (result) = RCU_APB1_CKAHB_DIV2;                          \
    else if ((x) == 4)                                         \
      (result) = RCU_APB1_CKAHB_DIV4;                          \
    else if ((x) == 8)                                         \
      (result) = RCU_APB1_CKAHB_DIV8;                          \
    else if ((x) == 16)                                        \
      (result) = RCU_APB1_CKAHB_DIV16;                         \
    else                                                       \
      (result) = INVAID_VALUE;                                   \
  } while (0);

#define APB2_PRESCALER_CONVERT_INTEGER_TO_BIT_FIELD(x, result) \
  do                                                           \
  {                                                            \
    if ((x) == 1)                                              \
      (result) = RCU_APB2_CKAHB_DIV1;                          \
    else if ((x) == 2)                                         \
      (result) = RCU_APB2_CKAHB_DIV2;                          \
    else if ((x) == 4)                                         \
      (result) = RCU_APB2_CKAHB_DIV4;                          \
    else if ((x) == 8)                                         \
      (result) = RCU_APB2_CKAHB_DIV8;                          \
    else if ((x) == 16)                                        \
      (result) = RCU_APB2_CKAHB_DIV16;                         \
    else                                                       \
      (result) = INVAID_VALUE;                                   \
  } while (0);

/*!
    \brief          This macros checks correct AHB prescaler value
    \param[in]      x - AHB1 prescaler (1, 2, 4 .. 512)
    \param[in]      result - output value
    \return
      \retval         HDL_HW_INIT_OK - OK
      \retval         HDL_HW_INIT_FAILED - ERROR
 */
#define AHB_PRESCALER_ASSERT(x, result) \
  do                                    \
  {                                     \
    if ((x) < 1 || (x) > 512)           \
    {                                   \
      (result) = HDL_HW_INIT_FAILED;    \
    }                                   \
    else                                \
    {                                   \
      if (IS_POWER_OF_TWO(x))           \
      {                                 \
        (result) = HDL_HW_INIT_OK;      \
      }                                 \
      else                              \
      {                                 \
        (result) = HDL_HW_INIT_FAILED;  \
      }                                 \
    }                                   \
  } while (0)

#define AHB_PRESCALER_CONVERT_INTEGER_TO_BIT_FIELD(x, result) \
  do                                                         \
  {                                                          \
    if ((x) == 1)                                            \
      (result) = RCU_AHB_CKSYS_DIV1;                         \
    else if ((x) == 2)                                       \
      (result) = RCU_AHB_CKSYS_DIV2;                         \
    else if ((x) == 4)                                       \
      (result) = RCU_AHB_CKSYS_DIV4;                         \
    else if ((x) == 8)                                       \
      (result) = RCU_AHB_CKSYS_DIV8;                         \
    else if ((x) == 16)                                      \
      (result) = RCU_AHB_CKSYS_DIV16;                        \
    else if ((x) == 64)                                      \
      (result) = RCU_AHB_CKSYS_DIV64;                        \
    else if ((x) == 128)                                     \
      (result) = RCU_AHB_CKSYS_DIV128;                       \
    else if ((x) == 256)                                     \
      (result) = RCU_AHB_CKSYS_DIV256;                       \
    else if ((x) == 512)                                     \
      (result) = RCU_AHB_CKSYS_DIV512;                       \
    else                                                     \
      (result) = 0xffffffff;                                 \
  } while (0);

/*!
    \brief          This macros checks correct multiply coefficient value
    \param[in]      x - multiply coefficient pll value must be 2, 3 .. 32
    \param[in]      result - output value
    \return
      \retval         HDL_HW_INIT_OK - OK
      \retval         HDL_HW_INIT_FAILED - ERROR
 */
#define PLL_MC_ASSERT(x, result)     \
  do                                 \
  {                                  \
    if ((x) < 2 || (x) > 32)         \
    {                                \
      (result) = HDL_HW_INIT_FAILED; \
    }                                \
    else                             \
    {                                \
      (result) = HDL_HW_INIT_OK;     \
    }                                \
  } while (0)


/*!
    \brief          Set system source
    \brief          System source can be CK_PLL, IRC8M, HXTAL
    \param[in]      desc - can be casting to hdl_clock_prescaler_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
*/
hdl_init_state_t hdl_gd_clock_system_source(void *desc, uint8_t enable){
  /* Manufacturer don`t define this value */
  #define CK_SYS_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  #define PLL_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  hdl_clock_prescaler_t *hdl_prescaler = NULL;
  FlagStatus osci_statable = RESET;
  FlagStatus ck_sys_changed_status = RESET;
  uint32_t stb_cnt = 0;

  /* Try casting void pointer to hdl_clock_prescaler_t */
  hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if (hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL || hdl_prescaler->hw.dependencies[0] == NULL)
    return HDL_HW_INIT_FAILED;
  /* Try casting hdl_prescaler.hw.dependencies to hdl_clock_t */
  hdl_clock_t *hdl_clock = (hdl_clock_t *)hdl_prescaler->hw.dependencies[0];
  if (enable){
    switch ((uint32_t)hdl_clock->hw.periph){
      /* Source clock selector is clocking by CK_PLL */
      case (uint32_t)HDL_GD_PLL_MULTIPLY_CLOCK_PERIPHERY:
        /* Turn on pll_ck */
        rcu_osci_on(RCU_PLL_CK);
        /* Wait PLL stabilization */
        stb_cnt = 0;
        while ((RESET == osci_statable) && (PLL_STARTUP_TIMEOUT != stb_cnt))
        {
            osci_statable = rcu_flag_get(RCU_FLAG_PLLSTB);
            stb_cnt++;
        }
        if (osci_statable == RESET)
          return HDL_HW_INIT_FAILED;
        /* Change switch state */
        rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
        /* Wait CK_SYS changed status */
        stb_cnt = 0;
        while ((RESET == ck_sys_changed_status) && (CK_SYS_STARTUP_TIMEOUT != stb_cnt))
        {
          ck_sys_changed_status = (RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) ? RESET : SET;
          stb_cnt++;
        }
        if (ck_sys_changed_status == RESET)
          return HDL_HW_INIT_FAILED;
      break;
      /* Source clock selector is clocking by HXTAL */
      case (uint32_t)HDL_GD_HXTAL_OSCILLATOR_CLOCK_PERIPHERY:
        /* Change switch state */
        rcu_system_clock_source_config(RCU_CKSYSSRC_HXTAL);
        /* Wait CK_SYS changed status */
        stb_cnt = 0;
        while ((RESET == ck_sys_changed_status) && (CK_SYS_STARTUP_TIMEOUT != stb_cnt))
        {
          ck_sys_changed_status = (RCU_SCSS_HXTAL != (RCU_CFG0 & RCU_CFG0_SCSS)) ? RESET : SET;
          stb_cnt++;
        }
        if (ck_sys_changed_status == RESET)
          return HDL_HW_INIT_FAILED;
      break;
      /* Source clock selector is clocking by IRC8M */
      case (uint32_t)HDL_GD_IRC8M_OSCILLATOR_CLOCK_PERIPHERY:
        /* Change switch state */
        rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
        /* Wait CK_SYS changed status */
        stb_cnt = 0;
        while ((RESET == ck_sys_changed_status) && (CK_SYS_STARTUP_TIMEOUT != stb_cnt))
        {
          ck_sys_changed_status = (RCU_SCSS_IRC8M != (RCU_CFG0 & RCU_CFG0_SCSS)) ? RESET : SET;
          stb_cnt++;
        }
        if (ck_sys_changed_status == RESET)
          return HDL_HW_INIT_FAILED;
      break;
      default:
        return HDL_HW_INIT_FAILED;
    }
    hdl_prescaler->freq = hdl_clock->freq;
    hdl_prescaler->div = hdl_clock->div;

    return HDL_HW_INIT_OK;
  }
  else{
    /* Turn off pll ck */
    rcu_osci_off(RCU_PLL_CK);
    /* Reset to default value */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
    /* Wait CK_SYS changed status */
    stb_cnt = 0;
    while ((RESET == ck_sys_changed_status) && (CK_SYS_STARTUP_TIMEOUT != stb_cnt))
    {
      ck_sys_changed_status = (RCU_SCSS_IRC8M != (RCU_CFG0 & RCU_CFG0_SCSS)) ? RESET : SET;
      stb_cnt++;
    }
    if (ck_sys_changed_status == RESET)
      return HDL_HW_INIT_FAILED;
      
    return HDL_HW_INIT_OK;
  }
}
/*!
    \brief          Set pll source as HXTAL
    \note           HXTAL - high speed crystal oscillator
    \param[in]      desc - descriptor can be casting to hdl_clock_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_hxtal(void *desc, uint8_t enable){
  hdl_clock_t* hdl_clock = NULL;
  FlagStatus osci_statable = RESET;
  uint32_t stb_cnt = 0;

  if(desc == NULL)
    return HDL_HW_INIT_FAILED;

  hdl_clock = (hdl_clock_t *)desc;
  if (enable){
    /* Turn on oscillator */
    rcu_osci_on(RCU_HXTAL);
    /* Wait until HXTAL will be stable */
    while ((RESET == osci_statable) && (HXTAL_STARTUP_TIMEOUT != stb_cnt))
    {
      osci_statable = rcu_flag_get(RCU_FLAG_HXTALSTB);
      stb_cnt++;
    }
    if (osci_statable == RESET)
      return HDL_HW_INIT_FAILED;

    hxtal_mhz = hdl_clock->freq;
    return HDL_HW_INIT_OK;
  }
  else{
    rcu_osci_off(RCU_HXTAL);
    hxtal_mhz = 0;
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          LXTAL set state
    \note           LXTAL - low speed crystal or ceramic resonator oscillator
    \param[in]      desc - descriptor can be casting to hdl_clock_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_lxtal(void *desc, uint8_t enable){
  /* Manufacturer don`t define this value */
  #define LXTAL_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  FlagStatus osci_statable = RESET;
  uint32_t stb_cnt = 0;
  if (enable){
    /* Turn on oscillator */
    rcu_osci_on(RCU_LXTAL);
    /* Wait until LXTAL will be stable */
    while ((RESET == osci_statable) && (LXTAL_STARTUP_TIMEOUT != stb_cnt))
    {
      osci_statable = rcu_flag_get(RCU_FLAG_LXTALSTB);
      stb_cnt++;
    }
    if (osci_statable == RESET)
      return HDL_HW_INIT_FAILED;

    return HDL_HW_INIT_OK;
  }
  else{
    rcu_osci_off(RCU_LXTAL);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          IRC40K set state
    \note           IRC40K - internal 40 KHz RC Oscillator
    \param[in]      desc - descriptor can be casting to hdl_clock_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_irc40k(void *desc, uint8_t enable){
  /* Manufacturer don`t define this value */
  #define IRC40K_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  FlagStatus osci_statable = RESET;
  uint32_t stb_cnt = 0;
  if (enable){
    /* Turn on oscillator */
    rcu_osci_on(RCU_IRC40K);
    /* Wait until IRC40K will be stable */
    while ((RESET == osci_statable) && (IRC40K_STARTUP_TIMEOUT != stb_cnt))
    {
      osci_statable = rcu_flag_get(RCU_FLAG_IRC40KSTB);
      stb_cnt++;
    }
    if (osci_statable == RESET)
      return HDL_HW_INIT_FAILED;

    return HDL_HW_INIT_OK;
  }
  else{
    rcu_osci_off(RCU_IRC40K);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          IRC8M set state
    \note           IRC8M - The internal 8 MHz RC oscillator
    \param[in]      desc - descriptor can be casting to hdl_clock_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_irc8m(void *desc, uint8_t enable)
{
  /* Manufacturer don`t define this value */
  #define CK_SYS_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  FlagStatus osci_statable = RESET;
  FlagStatus ck_sys_changed_status = RESET;
  uint32_t stb_cnt = 0;
  if (enable)
  {
    /* Turn on oscillator */
    rcu_osci_on(RCU_IRC8M);
    /* Wait until IRC8M will be stable */
    while ((RESET == osci_statable) && (IRC8M_STARTUP_TIMEOUT != stb_cnt))
    {
      osci_statable = rcu_flag_get(RCU_FLAG_IRC8MSTB);
      stb_cnt++;
    }
    if (osci_statable == RESET)
      return HDL_HW_INIT_FAILED;

    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
    /* Wait CK_SYS changed status */
    stb_cnt = 0;
    while ((RESET == ck_sys_changed_status) && (CK_SYS_STARTUP_TIMEOUT != stb_cnt))
    {
      ck_sys_changed_status = (RCU_SCSS_IRC8M != (RCU_CFG0 & RCU_CFG0_SCSS)) ? RESET : SET;
      stb_cnt++;
    }
    if (ck_sys_changed_status == RESET)
      return HDL_HW_INIT_FAILED;

    return HDL_HW_INIT_OK;
  }
  else
  {
    //rcu_osci_off(RCU_IRC8M);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          IRC28M set state
    \note           IRC28M - The internal 28 MHz RC Oscillator
    \param[in]      desc - descriptor can be casting to hdl_clock_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_irc28m(void *desc, uint8_t enable){
  /* Manufacturer don`t define this value */
  #define IRC28M_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  FlagStatus osci_statable = RESET;
  uint32_t stb_cnt = 0;
  if (enable){
    /* Turn on oscillator */
    rcu_osci_on(RCU_IRC28M);
    /* Wait until IRC28M will be stable */
    while ((RESET == osci_statable) && (IRC28M_STARTUP_TIMEOUT != stb_cnt))
    {
      osci_statable = rcu_flag_get(RCU_FLAG_IRC28MSTB);
      stb_cnt++;
    }
    if (osci_statable == RESET)
      return HDL_HW_INIT_FAILED;

    return HDL_HW_INIT_OK;
  }
  else{
    rcu_osci_off(RCU_IRC28M);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          Set rtc source
    \note           Can be clocking by HXTAL, LXTAL, IRC40K and defualt value is empty
    \param[in]      desc - can be casting to hdl_clock_prescaler_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_rtc(void *desc, uint8_t enable){
  hdl_clock_prescaler_t *hdl_prescaler = NULL;
  uint32_t frequency_prescaler = 1;
  /* Try casting void pointer to hdl_clock_prescaler_t */
  hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if (hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL || hdl_prescaler->hw.dependencies[0] == NULL)
    return HDL_HW_INIT_FAILED;
  /* Try casting hdl_prescaler.hw.dependencies to hdl_clock_t */
  hdl_clock_t *hdl_clock = (hdl_clock_t *)hdl_prescaler->hw.dependencies[0];
  if (enable){
    switch ((uint32_t)hdl_clock->hw.periph){
      /* RTC is cloking by HXTAL / 32 */
      case (uint32_t)HDL_GD_HXTAL_OSCILLATOR_CLOCK_PERIPHERY:
        rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
        frequency_prescaler = 32;
      break;
      /* RTC is cloking by LXTAL */
      case (uint32_t)HDL_GD_LXTAL_OSCILLATOR_CLOCK_PERIPHERY:
       rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
      break;
      /* RTC is cloking by IRC40K */
      case (uint32_t)HDL_GD_IRC40K_OSCILLATOR_CLOCK_PERIPHERY:
       rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
      break;

      default:
        return HDL_HW_INIT_FAILED;
    }
    /* TODO: This value can be calculated incorrect */
    hdl_prescaler->freq = hdl_clock->freq / frequency_prescaler;
    hdl_prescaler->div = hdl_clock->div / frequency_prescaler;

    return HDL_HW_INIT_OK;
  }
  else{
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          Set pll selector
    \param[in]      desc - descriptor can be casting to hdl_clock_prescaler_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_pll(void *desc, uint8_t enable){
  hdl_clock_prescaler_t *hdl_prescaler = NULL;
  /* Try casting void pointer to hdl_clock_prescaler_t */
  hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if (hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL || hdl_prescaler->hw.dependencies[0] == NULL)
    return HDL_HW_INIT_FAILED;
  /* Try casting hdl_prescaler.hw.dependencies to hdl_clock_t */
  hdl_clock_t *hdl_clock = (hdl_clock_t *)hdl_prescaler->hw.dependencies[0];
  if (enable){
    switch ((uint32_t)hdl_clock->hw.periph){
      case (uint32_t)HDL_GD_PLL_PRESCALER_CLOCK_PERIPHERY:
        /* CMSIS function */
        RCU_CFG0 &= ~(RCU_CFG0_PLLSEL);
        RCU_CFG0 |= (RCU_PLLSRC_HXTAL);
        /* Update frequency value */
        hdl_prescaler->freq = hdl_clock->freq;
        hdl_prescaler->div = hdl_clock->div;
        break;
      case (uint32_t)HDL_GD_IRC8M_OSCILLATOR_CLOCK_PERIPHERY:
        /* CMSIS function */
        RCU_CFG0 &= ~(RCU_CFG0_PLLSEL);
        RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2);
        /* Update frequency value */
        hdl_prescaler->freq = hdl_clock->freq / 2;  /* See curuit, 2 - it`s value of prescaler */
        hdl_prescaler->div = hdl_clock->freq % 2;
        break;
      default:
        return HDL_HW_INIT_FAILED;
      }
    return HDL_HW_INIT_OK;
  }
  else{
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          Set pll prescaler (in the circuit it`s PREDV)
    \param[in]      desc - can be cating to hdl_clock_prescaler_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_pll_prescaler(void *desc, uint8_t enable){
  uint32_t prescaler_value = 0;
  hdl_clock_prescaler_t *hdl_prescaler = NULL;
  /* Try casting void pointer to hdl_clock_prescaler_t */
  hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if (hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL || hdl_prescaler->hw.dependencies[0] == NULL)
    return HDL_HW_INIT_FAILED;
  /* Try casting hdl_prescaler.hw.dependencies to hdl_clock_t */
  hdl_clock_t *hdl_clock = (hdl_clock_t *)hdl_prescaler->hw.dependencies[0];

  if (enable){
    /* Geting prescaler value from dts */
    prescaler_value = hdl_prescaler->muldiv_factor;
    uint32_t register_bit_field_value = gd_rcu_pll_prescaler_array[prescaler_value];
    if (register_bit_field_value == INVAID_VALUE)
      return HDL_HW_INIT_FAILED;
    /* Calling HAL function */
    rcu_hxtal_prediv_config(register_bit_field_value);
    /* Calculate output frequency*/
    hdl_prescaler->freq = hdl_clock->freq / prescaler_value;
    hdl_prescaler->div = hdl_clock->freq % prescaler_value;

    return HDL_HW_INIT_OK;
  }
  else{
    return HDL_HW_INIT_OK;
  }
}
/*!
    \brief          Set pll multiply coefficient
    \param[in]      desc - descriptor cab me casting to hdl_clock_prescaler_t*
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_pll_multiply_coefficient(void *desc, uint8_t enable){
  uint32_t multiply_coefficient = 0;
  hdl_clock_prescaler_t *hdl_prescaler = NULL;
  /* Try casting void pointer to hdl_clock_prescaler_t */
  hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  if (hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL || hdl_prescaler->hw.dependencies[0] == NULL)
    return HDL_HW_INIT_FAILED;
  /* Try casting hdl_prescaler.hw.dependencies to hdl_clock_t */
  hdl_clock_t *hdl_clock = (hdl_clock_t *)hdl_prescaler->hw.dependencies[0];

  if (enable){
    /* Geting prescaler value from dts */
    multiply_coefficient = hdl_prescaler->muldiv_factor;
    uint32_t register_bit_field_value = gd_rcu_pll_multiply_coefficient_array[multiply_coefficient];
    if (register_bit_field_value == INVAID_VALUE)
      return HDL_HW_INIT_FAILED;
    /* Calling CMSIS function */
    RCU_CFG0 &= ~RCU_CFG0_PLLMF;
    RCU_CFG0 |= register_bit_field_value;
    /* Calculate output frequency*/
    /* TODO: fractal part calculating don't work correct */
    hdl_prescaler->freq = hdl_clock->freq * multiply_coefficient;
    hdl_prescaler->div = hdl_clock->div * multiply_coefficient;

    return HDL_HW_INIT_OK;
  }
  else{
    return HDL_HW_INIT_OK;
  }
}



hdl_init_state_t hdl_gd_clock_sys(void *desc, uint8_t enable)
{
  return HDL_HW_INIT_OK;
}

/*!
    \brief          Set AHB prescaler value
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_ahb(void *desc, uint8_t enable){
  //   uint32_t multiply_coefficient = 0;
  // hdl_clock_prescaler_t *hdl_prescaler = NULL;
  // /* Try casting void pointer to hdl_clock_prescaler_t */
  // hdl_prescaler = (hdl_clock_prescaler_t *)desc;
  // if (hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL || hdl_prescaler->hw.dependencies[0] == NULL)
  //   return HDL_HW_INIT_FAILED;
  // /* Try casting hdl_prescaler.hw.dependencies to hdl_clock_t */
  // hdl_clock_t *hdl_clock = (hdl_clock_t *)hdl_prescaler->hw.dependencies[0];

  // if (enable){
  //   /* Geting prescaler value from dts */
  //   multiply_coefficient = hdl_prescaler->muldiv_factor;
  //   uint32_t register_bit_field_value = gd_rcu_pll_multiply_coefficient_array[multiply_coefficient];
  //   if (register_bit_field_value == INVAID_VALUE)
  //     return HDL_HW_INIT_FAILED;
  //   /* Calling CMSIS function */
  //   RCU_CFG0 &= ~RCU_CFG0_PLLMF;
  //   RCU_CFG0 |= register_bit_field_value;
  //   /* Calculate output frequency*/
  //   /* TODO: fractal part calculating don't work correct */
  //   hdl_prescaler->freq = hdl_clock->freq * multiply_coefficient;
  //   hdl_prescaler->div = hdl_clock->div * multiply_coefficient;

  //   return HDL_HW_INIT_OK;
  // }
  // else{
  //   return HDL_HW_INIT_OK;
  // }
  uint32_t prescaler_value = 0;
  /* Try casting void pointer to PLL multiply coefficient */
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;
  else{
    prescaler_value = *((uint32_t *)desc);
    uint32_t rez = 0;
    /* Check valid value */
    AHB_PRESCALER_ASSERT(prescaler_value, rez);
    if (rez != HDL_HW_INIT_OK)
      return HDL_HW_INIT_FAILED;
  }

  if(enable){
    uint32_t register_bit_field_value = 0;
    AHB_PRESCALER_CONVERT_INTEGER_TO_BIT_FIELD(prescaler_value, register_bit_field_value);
    if(register_bit_field_value == INVAID_VALUE)
      return HDL_HW_INIT_FAILED;

    rcu_ahb_clock_config(register_bit_field_value);
    return HDL_HW_INIT_OK;
  }
  else{
    //rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          Set APB1 prescaler value
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_apb1(void *desc, uint8_t enable)
{
  uint32_t prescaler_value = 0;
  /* Try casting void pointer to PLL multiply coefficient */
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;
  else{
    prescaler_value = *((uint32_t *)desc);
    uint32_t rez = 0;
    /* Check valid value */
    APB1_APB2_PRESCALER_ASSERT(prescaler_value, rez);
    if (rez != HDL_HW_INIT_OK)
      return HDL_HW_INIT_FAILED;
  }

  if(enable){
    uint32_t register_bit_field_value = 0;
    APB1_PRESCALER_CONVERT_INTEGER_TO_BIT_FIELD(prescaler_value, register_bit_field_value);
    if(register_bit_field_value == INVAID_VALUE)
      return HDL_HW_INIT_FAILED;

    rcu_apb1_clock_config(register_bit_field_value);
    return HDL_HW_INIT_OK;
  }
  else{
    //rcu_apb1_clock_config(RCU_AHB_CKSYS_DIV1);
    return HDL_HW_INIT_OK;
  }
}

/*!
    \brief          Set APB2 prescaler value
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_apb2(void *desc, uint8_t enable)
{
  uint32_t prescaler_value = 0;
  /* Try casting void pointer to PLL multiply coefficient */
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;
  else{
    prescaler_value = *((uint32_t *)desc);
    uint32_t rez = 0;
    /* Check valid value */
    APB1_APB2_PRESCALER_ASSERT(prescaler_value, rez);
    if (rez != HDL_HW_INIT_OK)
      return HDL_HW_INIT_FAILED;
  }

  if(enable){
    uint32_t register_bit_field_value = 0;
    APB2_PRESCALER_CONVERT_INTEGER_TO_BIT_FIELD(prescaler_value, register_bit_field_value);
    if(register_bit_field_value == INVAID_VALUE)
      return HDL_HW_INIT_FAILED;

    rcu_apb2_clock_config(register_bit_field_value);
    return HDL_HW_INIT_OK;
  }
  else{
    //rcu_apb2_clock_config(RCU_AHB_CKSYS_DIV1);
    return HDL_HW_INIT_OK;
  }
}
