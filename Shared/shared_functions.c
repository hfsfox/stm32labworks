#include <shared_functions.h>

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

inline void _set_dsb(void)
{
    #if defined (__GNUC__) || defined(__clang__)
    __asm volatile (
        "dsb 0xF":::"memory"
        );
    #endif
}

inline void _delay(volatile uint32_t ms) {
    while (ms--)
    {
        #if defined (__GNUC__) || defined(__clang__)
            __asm volatile
            (
                "nop"
            );
        #endif
    }
}
