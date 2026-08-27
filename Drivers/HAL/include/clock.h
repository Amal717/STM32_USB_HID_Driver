#ifndef CLOCK_H_
#define CLOCK_H_
#include <stm32f407xx.h>
#include "config.h"

#if CONFIG_CLOCK

#define RCC_PLLCFGR_PLLP_DIV2   (0U << RCC_PLLCFGR_PLLP_Pos)
#define RCC_PLLCFGR_PLLP_DIV4   (1U << RCC_PLLCFGR_PLLP_Pos)
#define RCC_PLLCFGR_PLLP_DIV6   (2U << RCC_PLLCFGR_PLLP_Pos)
#define RCC_PLLCFGR_PLLP_DIV8   (3U << RCC_PLLCFGR_PLLP_Pos)

#define PLLSRC                      RCC_PLLCFGR_PLLSRC_HSE
#define HSE_PLL_M                   8U                      /* Main PLL and audio PLL input clock division factor */
#define HSE_PLL_N                   192U                    /* Main PLL multiplication factor */
#define HSE_PLL_P                   RCC_PLLCFGR_PLLP_DIV2   /* Main PLL division factor */
#define HSE_PLL_Q                   2                       /* Main PLL division factor for USB OTG FS, SDIO,random number generator */
#define AHB_PRE                     RCC_CFGR_HPRE_DIV1      /* AHB prescaler */
#define APB1_PRE                    RCC_CFGR_PPRE1_DIV4     /* APB1 prescaler */
#define APB2_PRE                    RCC_CFGR_PPRE2_DIV2     /* APB2 prescaler */

#define PCLK1_HZ                    24000000U
#define PCLK2_HZ                    48000000U

#if (HSE_PLL_M < 2U) || (HSE_PLL_M > 63U)
#error "Invalid PLL_M"
#endif

#if (HSE_PLL_N < 50U) || (HSE_PLL_N > 432U)
#error "Invalid PLL_N"
#endif

#endif

void clock_init(void);

#endif
