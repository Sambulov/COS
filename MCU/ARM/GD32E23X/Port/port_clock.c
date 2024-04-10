#include "hdl_portable.h"

/* TODO: Does flash depend on clock or not? Should you set flash clock before clock? */

/* Value of HXTAL, we can`t get this value from register, this value depends on dts*/
static uint32_t hxtal_mhz = 0;
static const uint32_t irc8m_mhz = 8000000;
static const uint32_t lxtal_mhz = 32768;
static const uint32_t irc40k_mhz = 40000;
static const uint32_t irc28m_mhz = 28000000;

#define IS_POWER_OF_TWO(x) ((x) && !((x) & ((x)-1)))
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
      (result) = 0xffffffff;                                   \
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
      (result) = 0xffffffff;                                   \
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
  \brief          This macros checks correct predv value
  \param[in]      x - mf pll value must be 1, 2 .. 16
  \param[in]      result - output value
  \return
    \retval         HDL_HW_INIT_OK - OK
    \retval         HDL_HW_INIT_FAILED - ERROR
*/
#define PLL_PRESCALER_ASSERT(x, result)  \
  do                                 \
  {                                  \
    if ((x) < 1 || (x) > 16)         \
    {                                \
      (result) = HDL_HW_INIT_FAILED; \
    }                                \
    else                             \
    {                                \
      (result) = HDL_HW_INIT_OK;     \
    }                                \
  } while (0)

/*!
    \brief          This macros convert pll multiply coefficient to bit field in the register
    \param[in]      x - mf pll value must be 2, 3 .. 32
    \param[in]      result - output value
    \return
      \retval         0xffffffff - ERROR
      \retval         other, OK
 */
#define PLL_MC_CONVERT_INTEGER_TO_BIT_FIELD(x, result) \
  do                                                  \
  {                                                   \
    if ((x) == 2)                                     \
      (result) = RCU_PLL_MUL2;                        \
    else if ((x) == 3)                                \
      (result) = RCU_PLL_MUL3;                        \
    else if ((x) == 4)                                \
      (result) = RCU_PLL_MUL4;                        \
    else if ((x) == 5)                                \
      (result) = RCU_PLL_MUL5;                        \
    else if ((x) == 6)                                \
      (result) = RCU_PLL_MUL6;                        \
    else if ((x) == 7)                                \
      (result) = RCU_PLL_MUL7;                        \
    else if ((x) == 8)                                \
      (result) = RCU_PLL_MUL8;                        \
    else if ((x) == 9)                                \
      (result) = RCU_PLL_MUL9;                        \
    else if ((x) == 10)                               \
      (result) = RCU_PLL_MUL10;                       \
    else if ((x) == 11)                               \
      (result) = RCU_PLL_MUL11;                       \
    else if ((x) == 12)                               \
      (result) = RCU_PLL_MUL12;                       \
    else if ((x) == 13)                               \
      (result) = RCU_PLL_MUL13;                       \
    else if ((x) == 14)                               \
      (result) = RCU_PLL_MUL14;                       \
    else if ((x) == 15)                               \
      (result) = RCU_PLL_MUL15;                       \
    else if ((x) == 16)                               \
      (result) = RCU_PLL_MUL16;                       \
    else if ((x) == 17)                               \
      (result) = RCU_PLL_MUL17;                       \
    else if ((x) == 18)                               \
      (result) = RCU_PLL_MUL18;                       \
    else if ((x) == 19)                               \
      (result) = RCU_PLL_MUL19;                       \
    else if ((x) == 20)                               \
      (result) = RCU_PLL_MUL20;                       \
    else if ((x) == 21)                               \
      (result) = RCU_PLL_MUL21;                       \
    else if ((x) == 22)                               \
      (result) = RCU_PLL_MUL22;                       \
    else if ((x) == 23)                               \
      (result) = RCU_PLL_MUL23;                       \
    else if ((x) == 24)                               \
      (result) = RCU_PLL_MUL24;                       \
    else if ((x) == 25)                               \
      (result) = RCU_PLL_MUL25;                       \
    else if ((x) == 26)                               \
      (result) = RCU_PLL_MUL26;                       \
    else if ((x) == 27)                               \
      (result) = RCU_PLL_MUL27;                       \
    else if ((x) == 28)                               \
      (result) = RCU_PLL_MUL28;                       \
    else if ((x) == 29)                               \
      (result) = RCU_PLL_MUL29;                       \
    else if ((x) == 30)                               \
      (result) = RCU_PLL_MUL30;                       \
    else if ((x) == 31)                               \
      (result) = RCU_PLL_MUL31;                       \
    else if ((x) == 32)                               \
      (result) = RCU_PLL_MUL32;                       \
    else                                              \
      (result) = 0xffffffff;                          \
  } while (0);

/*!
    \brief          This macros convert pll predv coefficient to bit field in the register
    \param[in]      x - pll predv value must be 1, 2 .. 16
    \param[in]      result - output value
    \return
      \retval         0xffffffff - ERROR
      \retval         other, OK
 */
#define PLL_PREDV_COVERT_INTEGER_TO_BIT_FIELD(x, result) \
  do                                                     \
  {                                                      \
    if ((x) == 1)                                        \
      (result) = RCU_PLL_PREDV1;                         \
    else if ((x) == 2)                                   \
      (result) = RCU_PLL_PREDV2;                         \
    else if ((x) == 3)                                   \
      (result) = RCU_PLL_PREDV3;                         \
    else if ((x) == 4)                                   \
      (result) = RCU_PLL_PREDV4;                         \
    else if ((x) == 5)                                   \
      (result) = RCU_PLL_PREDV5;                         \
    else if ((x) == 6)                                   \
      (result) = RCU_PLL_PREDV6;                         \
    else if ((x) == 7)                                   \
      (result) = RCU_PLL_PREDV7;                         \
    else if ((x) == 8)                                   \
      (result) = RCU_PLL_PREDV8;                         \
    else if ((x) == 10)                                  \
      (result) = RCU_PLL_PREDV9;                         \
    else if ((x) == 11)                                  \
      (result) = RCU_PLL_PREDV10;                        \
    else if ((x) == 12)                                  \
      (result) = RCU_PLL_PREDV11;                        \
    else if ((x) == 13)                                  \
      (result) = RCU_PLL_PREDV12;                        \
    else if ((x) == 14)                                  \
      (result) = RCU_PLL_PREDV13;                        \
    else if ((x) == 15)                                  \
      (result) = RCU_PLL_PREDV14;                        \
    else if ((x) == 16)                                  \
      (result) = RCU_PLL_PREDV15;                        \
    else                                                 \
      (result) = 0xffffffff;                             \
  } while (0);

/*!
    \brief          Set pll source as CK_PLL
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
*/
hdl_init_state_t hdl_gd_clock_system_source_pll(void *desc, uint8_t enable){
  /* Manufacturer don`t define this value */
  #define CK_SYS_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
  #define PLL_STARTUP_TIMEOUT ((uint32_t)0xFFFF)

  FlagStatus osci_statable = RESET;
  FlagStatus ck_sys_changed_status = RESET;
  uint32_t stb_cnt = 0;

  if (enable){
    rcu_osci_on(RCU_PLL_CK);
    /* Wait PLL stabilization */
    while ((RESET == osci_statable) && (PLL_STARTUP_TIMEOUT != stb_cnt))
    {
        osci_statable = rcu_flag_get(RCU_FLAG_PLLSTB);
        stb_cnt++;
    }
    if (osci_statable == RESET)
      return HDL_HW_INIT_FAILED;

    rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);
    /* Wait CK_SYS changed status */
    uint32_t stb_cnt = 0;
    while ((RESET == ck_sys_changed_status) && (CK_SYS_STARTUP_TIMEOUT != stb_cnt))
    {
      ck_sys_changed_status = (RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) ? RESET : SET;
      stb_cnt++;
    }
    if (ck_sys_changed_status == RESET)
      return HDL_HW_INIT_FAILED;

    return HDL_HW_INIT_OK;
  }
  else{
    rcu_osci_off(RCU_PLL_CK);
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
    return HDL_HW_INIT_OK;
  }
}

  /*!
      \brief          Set pll source as HXTAL
      \note           HXTAL - high speed crystal oscillator
      \param[in]      desc - descriptor
      \param[in]      enable
        \arg            0 - diable
        \arg            other - enable
      \return
        \retval         HDL_HW_INIT_OK
        \retval         HDL_HW_INIT_ONGOING
        \retval         HDL_HW_INIT_FAILED
  */
  hdl_init_state_t hdl_gd_clock_system_source_hxtal(void *desc, uint8_t enable){
    /* Manufacturer don`t define this value */
    #define CK_SYS_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
    FlagStatus ck_sys_changed_status = RESET;
    uint32_t stb_cnt = 0;
    if (enable){
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

      return HDL_HW_INIT_OK;
    }
    else{
      // rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
      return HDL_HW_INIT_OK;
    }
  }

  /*!
      \brief          Set pll source as IRC8M
      \param[in]      desc - descriptor
      \param[in]      enable
        \arg            0 - diable
        \arg            other - enable
      \return
        \retval         HDL_HW_INIT_OK
        \retval         HDL_HW_INIT_ONGOING
        \retval         HDL_HW_INIT_FAILED
  */
  hdl_init_state_t hdl_gd_clock_system_source_irc8m(void *desc, uint8_t enable)
  {
    /* Manufacturer don`t define this value */
    #define CK_SYS_STARTUP_TIMEOUT ((uint32_t)0xFFFF)
    FlagStatus ck_sys_changed_status = RESET;
    uint32_t stb_cnt = 0;
    if (enable){
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
    else{
      // rcu_osci_off(RCU_IRC8M);
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
    \brief          Set rtc source as HXTAL
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_rtc_hxtal(void *desc, uint8_t enable){
  if(enable)
    rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV32);
  else
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);

  return HDL_HW_INIT_OK;
}

/*!
    \brief          Set rtc source as LXTAL
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_rtc_lxtal(void *desc, uint8_t enable)
{
  if (enable)
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
  else
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);

  return HDL_HW_INIT_OK;
}

/*!
    \brief          Set rtc source as IRC40k
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_rtc_irc40k(void *desc, uint8_t enable)
{
  if (enable)
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
  else
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);

  return HDL_HW_INIT_OK;
}
/*!
    \brief          Set rtc source as none(default value)
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_rtc_none(void *desc, uint8_t enable)
{
  if (enable)
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);
  else
    rcu_rtc_clock_config(RCU_RTCSRC_NONE);

  return HDL_HW_INIT_OK;
}



/*!
    \brief          Set pll selector as a hxtal
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_pll_hxtal(void *desc, uint8_t enable)
{
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL);
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL);
    return HDL_HW_INIT_OK;
}

/*!
    \brief          Set pll selector as a irc8m
    \param[in]      desc - can be casting to hdl_clock_prescaler_t *
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_selector_pll_irc8m(void *desc, uint8_t enable)
{
  hdl_clock_prescaler_t* hdl_prescaler = NULL;
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;

  hdl_prescaler = (hdl_clock_prescaler_t*)desc;


  RCU_CFG0 &= ~(RCU_CFG0_PLLSEL);
  RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2);
  return HDL_HW_INIT_OK;
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
  hdl_clock_prescaler_t* hdl_prescaler = NULL;
  /* Try casting void pointer to hdl_clock_prescaler_t */
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;
  else{
    /* Checking to void pointer */
    hdl_prescaler = (hdl_clock_prescaler_t *)desc;
    if(hdl_prescaler->hw.periph == NULL || hdl_prescaler->hw.dependencies == NULL)
      return HDL_HW_INIT_FAILED;
    /* Geting prescaler value from dts */
    prescaler_value = hdl_prescaler->div;
    uint32_t rez = 0;
    /* Check valid value */
    PLL_PRESCALER_ASSERT(prescaler_value, rez);
    if (rez != HDL_HW_INIT_OK)
      return HDL_HW_INIT_FAILED;
  }

  if (enable){
    uint32_t register_bit_field_value = 0;
    /* Convert prescaler value to bit field for register */
    PLL_PREDV_COVERT_INTEGER_TO_BIT_FIELD(prescaler_value, register_bit_field_value);
    if (register_bit_field_value == 0xffffffff)
      return HDL_HW_INIT_FAILED;

    rcu_hxtal_prediv_config(register_bit_field_value);
    /* We can`t get value hxtal_mhz without static variable*/
    hdl_prescaler->freq = hxtal_mhz / prescaler_value;
    hdl_prescaler->div = hxtal_mhz % prescaler_value;

    return HDL_HW_INIT_OK;
  }
  else{
    return HDL_HW_INIT_OK;
  }
}
/*!
    \brief          Set pll multiply coefficient
    \param[in]      desc - descriptor
    \param[in]      enable
      \arg            0 - diable
      \arg            other - enable
    \return         description
      \retval         HDL_HW_INIT_OK
      \retval         HDL_HW_INIT_ONGOING
      \retval         HDL_HW_INIT_FAILED
 */
hdl_init_state_t hdl_gd_clock_pll_multiply_coefficient(void *desc, uint8_t enable){
  /* Multiply coefficient */
  uint32_t multiply_coefficient = 0;
  /* Try casting void pointer to PLL multiply coefficient */
  if (desc == NULL)
    return HDL_HW_INIT_FAILED;
  else{
    multiply_coefficient = *((uint32_t *)desc);
    uint32_t rez = 0;
    /* Check valid value */
    PLL_MC_ASSERT(multiply_coefficient, rez);
    if (rez != HDL_HW_INIT_OK)
      return HDL_HW_INIT_FAILED;
  }

  if (enable){
    /* Register is divided in the memory map */
    uint32_t register_bit_field_value = 0;
    /* convert pll multiply coefficient to bit field in the register */
    PLL_MC_CONVERT_INTEGER_TO_BIT_FIELD(multiply_coefficient, register_bit_field_value);
    if (register_bit_field_value == 0xFFFFFFFF)
      return HDL_HW_INIT_FAILED;

    /* Configure PLL multiply factor */
    RCU_CFG0 &= ~RCU_CFG0_PLLMF;
    RCU_CFG0 |= register_bit_field_value;
    return HDL_HW_INIT_OK;
  }
  else{
    //RCU_CFG0 &= ~RCU_CFG0_PLLMF;
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
    if(register_bit_field_value == 0xffffffff)
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
    if(register_bit_field_value == 0xffffffff)
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
    if(register_bit_field_value == 0xffffffff)
      return HDL_HW_INIT_FAILED;

    rcu_apb2_clock_config(register_bit_field_value);
    return HDL_HW_INIT_OK;
  }
  else{
    //rcu_apb2_clock_config(RCU_AHB_CKSYS_DIV1);
    return HDL_HW_INIT_OK;
  }
}
