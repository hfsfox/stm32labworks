#include <shared_functions.h>
#include <compiler_macro.h>

#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif

inline void _work_fq_setup(void)
{
    /*
    // 1. Enable HSE (High speed external clock signal) @ 25 MHz and wait for it to become ready
    RCC->CR |= RCC_CR_HSEON_Msk;
    while (! (RCC->CR & RCC_CR_HSERDY_Msk));
    // 2a. Enable power interface clock
    RCC->APB1ENR |= RCC_APB1ENR_PWREN_Msk;
    // Execute dsb instruction after enabling the peripheral clock, as per the errata
    _set_dsb();
    // 2b. Set regulator voltage scale mode 3 (HCLK <= 64MHz)
    PWR->CR &= ~(PWR_CR_VOS_Msk);
    PWR->CR |= (0b01 << PWR_CR_VOS_Pos);
    // 3. Configure flash controller for 2.7-3.3V supply and 30 MHz < HCLK <= 64 < MHz -> 1 WS,
    // enable Instruction prefetch (PRFTEN), Instruction cache (ICEN), Data cache (DCEN)
    FLASH->ACR |= (FLASH_ACR_DCEN_Msk | FLASH_ACR_ICEN_Msk | FLASH_ACR_PRFTEN_Msk | FLASH_ACR_LATENCY_1WS);
    */
}

inline inline_code void
_set_dsb(void)
{
    #if defined (__TARGET_STM32F401CEU6__) || !defined (__TARGET_STM32F103C8T6__)
        #if defined (COMPILER_CLANG) || defined (COMPILER_GCC)
        __asm volatile (
            "dsb 0xF":::"memory"
            );
        #endif
    #endif
}

inline inline_code void
_sys_init_external_clk(void)
{
    RCC->CR |= RCC_CR_HSEON;                           // Enable HSE
    while(!(RCC->CR & RCC_CR_HSERDY));                 // Wait HSE ready
    RCC->CFGR |= RCC_CFGR_SW_HSE;                      // Select HSE as sysclk
    SystemCoreClockUpdate();                           // Update core clock variable
}

inline inline_code void
_sys_init_internal_clk(void)
{
    RCC->CR |= RCC_CR_HSION;                           // Enable HSI
    while(!(RCC->CR & RCC_CR_HSIRDY));                 // Wait HSI ready
    RCC->CFGR |= RCC_CFGR_SW_HSI;                      // Select HSI as sysclk
    SystemCoreClockUpdate();                           // Update core clock variable
}

inline inline_code void
_delay_msecs(volatile uint32_t ms) {
    //MODIFY_REG(SysTick->VAL,SysTick_VAL_CURRENT_Msk,SYSCLOCK / 1000 - 1);
    SysTick->VAL = (SysTick->VAL & ~SysTick_VAL_CURRENT_Msk) | (SYSCLOCK / 1000 - 1);
    volatile uint32_t SysTick_CNT = 0;
    SysTick_CNT = ms;
    while(SysTick_CNT)
    {
        __asm volatile
            (
                "nop"
            );
    }
}

inline inline_code void
_delay_usecs(volatile uint32_t ms) {
    //MODIFY_REG(SysTick->VAL,SysTick_VAL_CURRENT_Msk,SYSCLOCK / 1000000 - 1);
    SysTick->VAL = (SysTick->VAL & ~SysTick_VAL_CURRENT_Msk) | (SYSCLOCK / 1000000 - 1);
    volatile uint32_t SysTick_CNT = 0;
    SysTick_CNT = ms;
    while(SysTick_CNT)
    {
        __asm volatile
            (
                "nop"
            );
    }
}

inline inline_code void
_delay_ticks(volatile uint32_t ticks)
{
    SysTick->VAL = (SysTick->VAL & ~SysTick_VAL_CURRENT_Msk) | (SYSCLOCK);
    while (ticks)
    {
        ticks--;
    }
}
