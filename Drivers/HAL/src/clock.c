#include "clock.h"

void clock_init(void)
{
    /* Step 1: Enable PWR interface clock */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* Step 2: configure voltage scaling */
    PWR->CR |= PWR_CR_VOS;
    
    /* step 3: Enable the HSE oscillator */
    RCC->CR |= RCC_CR_HSEON;

    /* Wait until HSE is ready */
    while (!(RCC->CR & RCC_CR_HSERDY));

    /* Step 4: Disable PLL before configuration */
    RCC->CR &= ~RCC_CR_PLLON;

    /* wait until PLL is completely stopped */
    while (RCC->CR & RCC_CR_PLLRDY);

    /* Step 5: Configure PLL */

    /* Configure PLLM */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR |= HSE_PLL_M << RCC_PLLCFGR_PLLM_Pos;

    /* Configure PLLN */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
    RCC->PLLCFGR |= HSE_PLL_N << RCC_PLLCFGR_PLLN_Pos;

    /* Configure PLLP */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
    RCC->PLLCFGR |= HSE_PLL_P;

    /* Configure PLLQ */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
    RCC->PLLCFGR |= HSE_PLL_Q << RCC_PLLCFGR_PLLQ_Pos;

    /* Configure PLL Source */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
    RCC->PLLCFGR |= PLLSRC;

    /* Step 6: Configure Flash */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

    /* Enable instruction cache, data cache and prefetch */
    FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;

    /* Step 7: Configure AHB / APB prescalers */

    /* AHB = SYSCLK / 1 = 96 MHz */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR |= AHB_PRE;

    /* APB1 = AHB / 4 = 24 MHz */
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= APB1_PRE;

    /* APB2 = AHB / 2 = 48 MHz */
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    RCC->CFGR |= APB2_PRE;

    /* Step 8: Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* wait until PLL is ready */
    while (!(RCC->CR & RCC_CR_PLLRDY));

    /* Step 9: Select PLL as SYSCLK */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* wait until PLL is actually selected as SYSCLK */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

