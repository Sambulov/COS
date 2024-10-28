/*
  Module Init Graph (MIG)
*/

#include "app.h"

#if defined ( ATB_RK3568J_SMC_R1 )




// #define TEST_NO 7

// /* 
//   1  - pll by hxtal/2, sys clock 108MHz SysTick          | w
//   2  - pll by hxtal/2, sys clock 72MHz  SysTick          | w
//   3  - pll by hxtal/1, sys clock 108MHz SysTick          | w
//   4  - pll by irc8m/2, sys clock 108MHz SysTick          | w
//   5  - pll by irc8m/2, sys clock 72MHz  SysTick          | w
//   6  - pll by irc8m/2, sys clock 36MHz TIMER0            | w
//   7  - pll by irc8m/2, sys clock 36MHz APB2/2 TIMER0*2   | w
//   8  - pll by irc8m/2, sys clock 108MHz APB2/16 TIMER0*2 | w
//   9  - pll by irc8m/2, sys clock 108MHz APB1/16 TIMER1*2 | w
//   10 - pll by irc8m/2, sys clock 52MHz  APB1/1 TIMER1*1  | w
//   11 - pll by irc8m/2, sys clock 52MHz  APB1/2 TIMER1*2  | w
// */


#define TEST_CLOCK
#define TEST_CLOCK_NUM 1

#ifdef TEST_CLOCK
/*
* 1 - pll by hxtal/1, sys clock 32MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
*                       APB 2 TEST
* 2 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/1, APB2/1  | SUCCESS
* 3 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/1, APB2/2  | SUCCESS
* 4 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/1, APB2/4  | SUCCESS
* 5 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/1, APB2/8  | SUCCESS
* 6 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/1, APB2/16 | SUCCESS
*                       APB 1 TEST
* 7 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/2, APB2/1  | SUCCESS
* 8 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/4, APB2/1  | SUCCESS
* 9 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/8, APB2/1  | SUCCESS         
* 10 - pll by hxtal/1, sys clock 32MHz, AHB/2, APB1/16, APB2/1  | SUCCESS
*                       AHB TEST
* 11 - pll by hxtal/1, sys clock 32MHz, AHB/4, APB1/1, APB2/1  | SUCCESS
* 12 - pll by hxtal/1, sys clock 32MHz, AHB/8, APB1/1, APB2/1  | SUCCESS
* 13 - pll by hxtal/1, sys clock 32MHz, AHB/16, APB1/1, APB2/1  | SUCCESS   
* 14 - pll by hxtal/1, sys clock 32MHz, AHB/64, APB1/1, APB2/1  | SUCCESS   
* 15 - pll by hxtal/1, sys clock 32MHz, AHB/128, APB1/1, APB2/1  | FAIL (unexpected behavior)   
* 16 - pll by hxtal/1, sys clock 32MHz, AHB/256, APB1/1, APB2/1  | FAIL (unexpected behavior) 
* 17 - pll by hxtal/1, sys clock 32MHz, AHB/512, APB1/1, APB2/1  | FAIL (unexpected behavior)
*                       PLL MF 
* 18 - pll by hxtal/1, pll mf 3 , sys clock 48MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 19 - pll by hxtal/2, pll mf 7 , sys clock 56MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 20 - pll by hxtal/16, pll mf 15 , sys clock 16MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 21 - pll by hxtal/16, pll mf 16 , sys clock 16MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 22 - pll by hxtal/16, pll mf 17 , sys clock 16MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 23 - pll by hxtal/16, pll mf 26 , sys clock 26MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 24 - pll by hxtal/16, pll mf 32 , sys clock 32MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
*                       PREDV
* 25 - pll by hxtal/1, pll mf 2 , sys clock 32 MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 26 - pll by hxtal/4, pll mf 2 , sys clock 8 MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 27 - pll by hxtal/16, pll mf 16 , sys clock 16 MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
*                      PLL SEL
* 28 - pll by IRC8M/2, pll mf 4 , sys clock 16 MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
*                      SYS_SEL
* 29 - pll by IRC8M, sys clock 8 MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
* 30 - pll by HXTAL, sys clock 16 MHz, AHB/1, APB1/1, APB2/1  | SUCCESS
*/

#if TEST_CLOCK_NUM == 1

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   4
  #define HDL_AHB_PREDIV               1
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              1
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel         /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll             /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  64000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   32000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   32000 - 1                  /* Clocked by APB 1 */

#endif

/* RETRY */
#if TEST_CLOCK_NUM == 2

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              1
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 3

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              2                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 4

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              4                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   8000 - 1                   /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 5

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              8                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   4000 - 1                   /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 6

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              16                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   2000 - 1                   /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 7

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              2                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 8

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              4                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   8000 - 1                   /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 9

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              8                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   4000 - 1                   /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 10

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               2
  #define HDL_APB1_PREDIV              16                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   2000 - 1                   /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 11

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               4                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  8000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   8000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   8000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 12

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               8                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  4000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   4000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   4000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 13

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               16                         /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  2000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   2000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   2000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 14

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               64                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  500 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   500 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   500 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 15

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               128                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  250 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 16

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               256                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  250 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 17

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               512                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  250 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 18

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   3
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  24000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   24000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   24000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 19

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   7
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  28000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   28000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   28000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 20

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    16
  #define HDL_PLLMUL                   15
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  15000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   15000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   15000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 21

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    16
  #define HDL_PLLMUL                   16
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 22

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    16
  #define HDL_PLLMUL                   17
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  17000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   17000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   17000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 23

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    16
  #define HDL_PLLMUL                   26
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  26000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   26000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   26000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 24

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    16
  #define HDL_PLLMUL                   32
  #define HDL_AHB_PREDIV               1                        /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  32000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   32000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   32000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 25

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               1                          /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  32000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 26

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    4
  #define HDL_PLLMUL                   2
  #define HDL_AHB_PREDIV               1                          /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  8000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   8000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   8000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 27

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    16
  #define HDL_PLLMUL                   16
  #define HDL_AHB_PREDIV               1                          /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 28

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   4
  #define HDL_AHB_PREDIV               1                          /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m           /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 29

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   4
  #define HDL_AHB_PREDIV               1                          /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m           /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_irc8m         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  8000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   8000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   8000 - 1                  /* Clocked by APB 1 */

#endif

#if TEST_CLOCK_NUM == 30

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    1
  #define HDL_PLLMUL                   4
  #define HDL_AHB_PREDIV               1                          /* Can be 1, 2, 4, 8, 16, 64, 128, 256, 512 */
  #define HDL_APB1_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */
  #define HDL_APB2_PREDIV              1                         /* Can be 1, 2, 4, 8, 16 */                    
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_irc8m           /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_hxtal         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  16000 - 1                  /* Clocked by AHB   */
  #define HDL_TIMER0_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 2 */
  #define HDL_TIMER2_COUNTER_RELOAD   16000 - 1                  /* Clocked by APB 1 */

#endif

#else

  #define HDL_HXTAL_CLOCK              16000000
  #define HDL_LXTAL_CLOCK              32768
  #define HDL_HXTAL_2_PLLSEL_PREDIV    2
  #define HDL_PLLMUL                   9
  #define HDL_AHB_PREDIV               1
  #define HDL_APB1_PREDIV              1
  #define HDL_APB2_PREDIV              1
  #define HDL_RTC_CLOCK                mod_clock_hxtal           /* Can be clocked by: mod_clock_hxtal, mod_clock_lxtal, mod_clock_irc40k. For mod_clock_hxtal applied prediv 2 */
  #define HDL_PLL_MUL_CLOCK            mod_clock_pll_sel   /* Can be clocked by: mod_clock_pll_sel, mod_clock_irc8m. For mod_clock_irc8m applied prediv 2 */
  #define HDL_SYS_CLOCK                mod_clock_pll         /* Can be clocked by: mod_clock_pll, mod_clock_irc8m, mod_clock_hxtal */

  #define HDL_SYSTICK_COUNTER_RELOAD  32000 - 1 

#endif

  #define HDL_INTERRUPT_PRIO_GROUP_BITS   __NVIC_PRIO_BITS

hdl_core_config_t mod_sys_core_cnf = {
  .flash_latency = WS_WSCNT_2 /* WS_WSCNT_0: sys_clock <= 24MHz, WS_WSCNT_1: sys_clock <= 48MHz, WS_WSCNT_2: sys_clock <= 72MHz */
};

hdl_core_t mod_sys_core = {
  .module.init = &hdl_core,
  .module.dependencies = NULL,
  .module.reg = (void *)SCB_BASE,
  .config = &mod_sys_core_cnf
};

  /**************************************************************
   *  NVIC, IRQ, EXTI
   *************************************************************/





hdl_interrupt_t mod_irq_systick = {
  .irq_type = HDL_NVIC_EXCEPTION_SysTick,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_timer0 = {
  .irq_type = HDL_NVIC_IRQ13_TIMER0_BRK_UP_TRG_COM,
  .priority = 0,
  .priority_group = 1,
};

hdl_interrupt_t mod_irq_timer2 = {
  .irq_type = HDL_NVIC_IRQ16_TIMER2,
  .priority = 2,
  .priority_group = 2,
};

hdl_interrupt_t mod_irq_exti_0_1 = {
  .irq_type = HDL_NVIC_IRQ5_EXTI0_1,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_exti_2_3 = {
  .irq_type = HDL_NVIC_IRQ6_EXTI2_3,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_exti_4_15 = {
  .irq_type = HDL_NVIC_IRQ7_EXTI4_15,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_usart_0 = {
  .irq_type = HDL_NVIC_IRQ27_USART0,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_spi_0 = {
  .irq_type = HDL_NVIC_IRQ25_SPI0,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0_ev = {
  .irq_type = HDL_NVIC_IRQ23_I2C0_EV,
  .priority = 0,
  .priority_group = 0,
};

hdl_interrupt_t mod_irq_i2c0_er = {
  .irq_type = HDL_NVIC_IRQ32_I2C0_ER,
  .priority = 0,
  .priority_group = 0,
};

hdl_exti_t mod_nvic_exti_line_0 = {
  .line = HDL_EXTI_LINE_0,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PA,
  .trigger = HDL_EXTI_TRIGGER_FALLING
};

hdl_exti_t mod_nvic_exti_line_8 = {
  .line = HDL_EXTI_LINE_8,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PB,
  .trigger = HDL_EXTI_TRIGGER_FALLING
};

hdl_exti_t mod_nvic_exti_line_15 = {
  .line = HDL_EXTI_LINE_15,
  .mode = HDL_EXTI_MODE_INTERRUPT,
  .source = HDL_EXTI_SOURCE_PA,
  .trigger = HDL_EXTI_TRIGGER_RISING
};

extern const hdl_interrupt_controller_config_t mod_nvic_cnf;

const void* const irq_vector[] __attribute__((aligned(HDL_VTOR_TAB_ALIGN))) = {
  &mod_nvic_cnf,
  &reset_handler,
  &nmi_handler,
  &hard_fault_handler,
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  ((void *)0),
  &svc_handler,
  ((void *)0),
  ((void *)0),
  &pend_sv_handler,
  &systick_handler,
  &wwdgt_handler,                    /* IRQ0 */ 
  &lvd_handler,                      /* IRQ1 */
  &rtc_handler,                      /* IRQ2 */
  &fmc_handler,                      /* IRQ3 */
  &rcu_handler,                      /* IRQ4 */
  &exti0_1_handler,                  /* IRQ5 */
  &exti2_3_IRQHandler,               /* IRQ6 */
  &exti4_15_handler,                 /* IRQ7 */
  &irq_n_handler,                    /* IRQ8 */
  &dma_channel0_handler,             /* IRQ9 */
  &dma_channel1_2_handler,           /* IRQ10 */
  &dma_channel3_4_handler,           /* IRQ11 */
  &adc_cmp_handler,                  /* IRQ12 */
  &timer0_brk_up_trg_com_handler,    /* IRQ13 */
  &timer0_channel_handler,           /* IRQ14 */
  &irq_n_handler,                    /* IRQ15 */
  &timer2_handler,                   /* IRQ16 */
  &timer5_handler,                   /* IRQ17 */
  &irq_n_handler,                    /* IRQ18 */
  &timer13_handler,                  /* IRQ19 */
  &timer14_handler,                  /* IRQ21 */
  &timer15_handler,                  /* IRQ22 */
  &timer16_handler,                  /* IRQ23 */
  &i2c0_ev_handler,                  /* IRQ24 */
  &i2c1_ev_handler,                  /* IRQ25 */
  &spi0_handler,                     /* IRQ26 */
  &spi1_handler,                     /* IRQ27 */
  &usart0_handler,                   /* IRQ28 */
  &usart1_handler,                   /* IRQ29 */
  &irq_n_handler,                    /* IRQ30 */
  &irq_n_handler,                    /* IRQ31 */
  &irq_n_handler,                    /* IRQ32 */
  &i2c0_er_handler,                  /* IRQ33 */
  &irq_n_handler,                    /* IRQ34 */
  &i2c1_er_handler,                  /* IRQ35 */
};

const hdl_interrupt_controller_config_t mod_nvic_cnf = {
  .vector = &irq_vector,
  .prio_bits = HDL_INTERRUPT_PRIO_GROUP_BITS,
  .irq_latency = 0, /* TODO: define static assert */
  .interrupts = hdl_interrupts(&mod_irq_systick, &mod_irq_exti_0_1, &mod_irq_exti_2_3, &mod_irq_exti_4_15,
    &mod_irq_timer0, &mod_irq_timer2, &mod_irq_usart_0, &mod_irq_spi_0, &mod_irq_i2c0_ev, &mod_irq_i2c0_er),
};

hdl_interrupt_controller_t mod_nvic = {
  .module.init = &hdl_interrupt_controller,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module),
  .module.reg = NVIC,
  .config = &mod_nvic_cnf
};

  /**************************************************************
   *  Oscillator
   *************************************************************/
hdl_clock_t mod_clock_irc8m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC8M, 
    .property.freq = 8000000
  }})
};

hdl_clock_t mod_clock_hxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_HXTAL, 
    .property.freq = HDL_HXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_lxtal = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_LXTAL, 
    .property.freq = HDL_LXTAL_CLOCK
  }})
};

hdl_clock_t mod_clock_irc28m = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC28M, 
    .property.freq = 28000000
  }})
};

hdl_clock_t mod_clock_irc40k = {
  .module.init = &hdl_clock,
  .module.dependencies = NULL,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_IRC40K, 
    .property.freq = 40000
  }})
};

hdl_clock_t mod_clock_pll_sel = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_PLL_MUL_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL_SEL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.div = HDL_HXTAL_2_PLLSEL_PREDIV
  }})
};

hdl_clock_t mod_clock_pll = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_pll_sel.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_PLL, 
    /* If source IRC8M prescaler fixed on 2 */
    .property.mul = HDL_PLLMUL
  }})
};

hdl_clock_t mod_clock_system = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_sys_core.module, &mod_clock_irc8m.module, &HDL_SYS_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_SYS_SEL
  }})
};

hdl_clock_t mod_clock_rtc = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&HDL_RTC_CLOCK.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_RTC_SEL
  }})
};

hdl_clock_t mod_clock_ahb = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_system.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_AHB,
    .property.div = HDL_AHB_PREDIV
  }})
};

hdl_clock_t mod_clock_apb1 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB1,
    .property.div = HDL_APB1_PREDIV
  }})
};

hdl_clock_t mod_clock_apb2 = {
  .module.init = &hdl_clock,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void *)RCU,
  .config = ((const hdl_clock_config_t const []) {{
    .type = HDL_CLOCK_TYPE_APB2,
    .property.div = HDL_APB2_PREDIV
  }})
};

  /**************************************************************
   *  DMA
   *************************************************************/
const hdl_dma_config_t mod_dma_config = {
  .rcu = RCU_DMA
};

hdl_dma_t mod_dma = {
  .module.init = &hdl_dma,
  .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
  .module.reg = (void*)DMA_BASE,
  .config = &mod_dma_config
};

const hdl_dma_channel_config_t mod_adc_dma_ch_config = {
  .priority = DMA_PRIORITY_LOW,
  .direction = DMA_PERIPHERAL_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_32BIT,
  .periph_width = DMA_PERIPHERAL_WIDTH_16BIT,
  .memory_inc = 1,
  .periph_inc = 0,
  .circular = 1
};

const hdl_dma_channel_config_t mod_m2m_dma_ch_config = {
  .priority = DMA_PRIORITY_LOW,
  .direction = DMA_PERIPHERAL_TO_MEMORY,
  .memory_width = DMA_MEMORY_WIDTH_32BIT,
  .periph_width = DMA_PERIPHERAL_WIDTH_32BIT,
  .memory_inc = 1,
  .periph_inc = 1,
  .m2m_direction = 1,
  .circular = 0
};

hdl_dma_channel_t mod_adc_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH0,
  .config = &mod_adc_dma_ch_config
};

hdl_dma_channel_t mod_m2m_dma_ch = {
  .module.init = &hdl_dma_ch,
  .module.dependencies = hdl_module_dependencies(&mod_dma.module),
  .module.reg = (void*)DMA_CH1,
  .config = &mod_m2m_dma_ch_config
};

  const hdl_tick_counter_timer_config_t mod_tick_counter0_cnf = {
    .alignedmode = TIMER_COUNTER_EDGE,
    .clockdivision = TIMER_CKDIV_DIV1,
    .counterdirection = TIMER_COUNTER_UP,
    .period = HDL_TIMER2_COUNTER_RELOAD,
    .prescaler = 0,
    .repetitioncounter = 0,
    .rcu = RCU_TIMER0
  };

  const hdl_tick_counter_timer_config_t mod_tick_counter2_cnf = {
    .alignedmode = TIMER_COUNTER_EDGE,
    .clockdivision = TIMER_CKDIV_DIV1,
    .counterdirection = TIMER_COUNTER_UP,
    .period = HDL_TIMER2_COUNTER_RELOAD,
    .prescaler = 0,
    .repetitioncounter = 0,
    .rcu = RCU_TIMER2
  };

  const hdl_tick_counter_systick_config_t mod_systick_counter_cnf = {
    .period = HDL_SYSTICK_COUNTER_RELOAD
  };

  hdl_tick_counter_t mod_timer0_counter = {
    .module.init = &hdl_tick_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_apb2.module),
    .module.reg = (void *)TIMER0,
    .config = &mod_tick_counter0_cnf
  };

    hdl_tick_counter_t mod_timer2_counter = {
    .module.init = &hdl_tick_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_apb1.module),
    .module.reg = (void *)TIMER2,
    .config = &mod_tick_counter2_cnf
  };

  hdl_tick_counter_t mod_systick_counter = {
    .module.init = &hdl_tick_counter,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)SysTick,
    .config.systick = &mod_systick_counter_cnf
  };

  const hdl_time_counter_config_t mod_timer_ms_cnf = {
    .reload_interrupt = &mod_irq_systick,
  };

  hdl_time_counter_t mod_timer_ms = {
    .module.init = hdl_time_counter,
    .module.dependencies = hdl_module_dependencies(&mod_systick_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .config = &mod_timer_ms_cnf
  };

  const hdl_time_counter_config_t mod_timer0_ms_cnf = {
    .reload_interrupt = &mod_irq_timer0,
  };

  hdl_time_counter_t mod_timer0_ms = {
    .module.init = hdl_time_counter,
    .module.dependencies = hdl_module_dependencies(&mod_timer0_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .config = &mod_timer0_ms_cnf
  };

  const hdl_time_counter_config_t mod_timer2_ms_cnf = {
    .reload_interrupt = &mod_irq_timer2,
  };

  hdl_time_counter_t mod_timer2_ms = {
    .module.init = hdl_time_counter,
    .module.dependencies = hdl_module_dependencies(&mod_timer2_counter.module, &mod_nvic.module),
    .module.reg = NULL,
    .config = &mod_timer2_ms_cnf
  };

  /**************************************************************
   *  ADC
   *************************************************************/
  hdl_adc_source_t mod_adc_source_0 = {
    .channel = HDL_ADC_CHANNEL_3,
    .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  };
  hdl_adc_source_t mod_adc_source_1 = {
    .channel = HDL_ADC_CHANNEL_7,
    .sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  };
  // hdl_adc_channel_source_t mod_adc_channel_7 = {
  //   .channel_number = HDL_ADC_CHANNEL_7,
  //   .channel_sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  // };
  // hdl_adc_channel_source_t mod_adc_channel_8 = {
  //   .channel_number = HDL_ADC_CHANNEL_8,
  //   .channel_sample_time = HDL_ADC_CHANNEL_SAMPLE_TIME_7P5
  // };

  const hdl_adc_config_t mod_adc_cnf = {
    .resolution = HDL_ADC_RESOLUTION_12BIT,
    .data_alignment = HDL_ADC_DATA_ALIGN_RIGHT,
    .init_timeout = 3000,
    .sources = hdl_adc_src(&mod_adc_source_1, &mod_adc_source_0),
  };

  hdl_adc_t mod_adc = {
    .module.init = &hdl_adc,
    .module.dependencies = hdl_module_dependencies(&mod_clock_irc28m.module, &mod_timer_ms.module, &mod_adc_dma_ch.module),
    .module.reg = (void*)ADC,
    .config = &mod_adc_cnf
  };

  const hdl_gpio_port_config_t hdl_gpio_port_config_a = {
    .rcu = RCU_GPIOA
  };

  const hdl_gpio_port_config_t hdl_gpio_port_config_b = {
    .rcu = RCU_GPIOB
  };

  const hdl_gpio_port_config_t hdl_gpio_port_config_c = {
    .rcu = RCU_GPIOC
  };

  const hdl_gpio_port_config_t hdl_gpio_port_config_f = {
    .rcu = RCU_GPIOF
  };

  hdl_gpio_port_t hdl_gpio_port_a = {
    .module.init = &hdl_gpio_port,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)GPIOA,
    .config = &hdl_gpio_port_config_a
  };

  hdl_gpio_port_t hdl_gpio_port_b = {
    .module.init = &hdl_gpio_port,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)GPIOB,
    .config = &hdl_gpio_port_config_b
  };

  hdl_gpio_port_t hdl_gpio_port_c = {
    .module.init = &hdl_gpio_port,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)GPIOC,
    .config = &hdl_gpio_port_config_c
  };

  hdl_gpio_port_t hdl_gpio_port_f = {
    .module.init = &hdl_gpio_port,
    .module.dependencies = hdl_module_dependencies(&mod_clock_ahb.module),
    .module.reg = (void *)GPIOF,
    .config = &hdl_gpio_port_config_f
  };

  const hdl_gpio_pin_hw_config_t mod_gpio_output_pp_mode = {
    .type = GPIO_MODE_OUTPUT,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_2MHZ,
  };

  const hdl_gpio_pin_hw_config_t mod_gpio_input_pullup_mode = {
    .type = GPIO_MODE_INPUT,
    .pull = GPIO_PUPD_PULLUP,
  };

  const hdl_gpio_pin_hw_config_t mod_gpio_input_analog = {
    .type = GPIO_MODE_ANALOG,
    .pull = GPIO_PUPD_NONE,
  };

  const hdl_gpio_pin_hw_config_t mod_gpio_alternate_swd_mode = {
    .af = GPIO_AF_0,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_PP,
    .ospeed = GPIO_OSPEED_50MHZ,
  };

  const hdl_gpio_pin_hw_config_t mod_gpio_i2c_mode = {
    .af = GPIO_AF_4,
    .type = GPIO_MODE_AF,
    .otype = GPIO_OTYPE_OD,
    .ospeed = GPIO_OSPEED_2MHZ
  };

  // const hdl_gpio_t mod_gpi_pmic_sleep_1v8 = {
  //   .port = &hdl_gpio_port_a,
  //   .pin = GPIO_PIN_15,
  //   .hwc = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_lid = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_2,
  //   .hwc = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_batlow = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_6,
  //   .hwc = &mod_gpio_input_pullup_mode
  // };

  // const hdl_gpio_t mod_gpi_carrier_sleep = {
  //   .port = &hdl_gpio_port_b,
  //   .pin = GPIO_PIN_7,
  //   .hwc = &mod_gpio_input_pullup_mode
  // };

  hdl_gpio_pin_t mod_gpi_carrier_wdt_time_out = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_8,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpio_adc_channel_3v3 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_7,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_analog)
  };

    hdl_gpio_pin_t mod_gpio_adc_channel_1v5 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_3,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_analog)
  };

  // const hdl_gpio_t mod_gpi_carrier_charging = {
  //   .port = &hdl_gpio_port_f,
  //   .pin = GPIO_PIN_0,
  //   .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  // };

  // const hdl_gpio_t mod_gpi_carrier_charger_prstn = {
  //   .port = &hdl_gpio_port_f,
  //   .pin = GPIO_PIN_1,
  //   .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  // };

  hdl_gpio_pin_t mod_gpo_emmc_lock = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_0,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpo_qspi_lock = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_1,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpo_sd_lock = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_2,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpi_carrier_force_recovery = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_13,
    #if defined(DEBUG)
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_alternate_swd_mode)
    #else
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
    #endif
  };

  hdl_gpio_pin_t mod_gpi_carrier_boot_sel0 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_4,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  };

  hdl_gpio_pin_t mod_gpi_carrier_boot_sel1 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_5,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  };

  hdl_gpio_pin_t mod_gpi_carrier_boot_sel2 = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_6,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  };

  hdl_gpio_pin_t mod_gpi_carrier_power_good = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_0,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  };

  hdl_gpio_pin_t mod_gpi_carrier_reset_in = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_7,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  };

  hdl_gpio_pin_t mod_gpo_carrier_reset_out = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_8,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpo_carrier_pwr_on = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_1,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpo_carrier_stby = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_3,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpi_carrier_power_btn = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_4,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_input_pullup_mode)
  };

  hdl_gpio_pin_t mod_gpo_pmic_soc_rst = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_3,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpo_pmic_power_on = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_b.module),
    .module.reg = (void *)GPIO_PIN_5,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_output_pp_mode)
  };

  hdl_gpio_pin_t mod_gpio_soc_scl = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_9,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_i2c_mode)
  };

  hdl_gpio_pin_t mod_gpio_soc_sda = {
    .module.init = &hdl_gpio_pin,
    .module.dependencies = hdl_module_dependencies(&hdl_gpio_port_a.module),
    .module.reg = (void *)GPIO_PIN_10,
    .config = hdl_gpio_pin_config(.inactive_default = HDL_GPIO_LOW, .hwc = &mod_gpio_i2c_mode)
  };

  hdl_i2c_config_t mod_i2c_config = {
    .err_interrupt = &mod_irq_i2c0_er,
    .ev_interrupt = &mod_irq_i2c0_ev,
    .dtcy = I2C_DTCY_2,
    .speed = 400000,
    .rcu = RCU_I2C0
  };

  hdl_i2c_t mod_i2c0_client = {
    .module.init = &hdl_i2c,
    .module.reg = (void *)I2C0,
    .module.dependencies = hdl_module_dependencies(&mod_gpio_soc_scl.module, &mod_gpio_soc_sda.module, &mod_clock_apb1.module, &mod_nvic.module),
    .config = &mod_i2c_config,
  };

#endif
