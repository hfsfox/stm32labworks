/*Copyright (C) 2025 Kravchuk Roman <darkkitsunezx128k2309@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 * STM32F103C8T6 Frequency Meter - Compact Version
 * 8MHz HSI, TIM2 Input Capture, TIM3 Display Timer
 */

#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif
#include <gpio_pins.h>
#include <shared_functions.h>
#include <compiler_macro.h>
#include <stdint.h>
#include <stdbool.h>

// 7-segment patterns (0-9)
// 7-Segment Display Patterns (Common Cathode)
const uint8_t digit_patterns[10] = {
    0x3F, // 0: ABCDEF
    0x06, // 1: BC
    0x5B, // 2: ABDEG
    0x4F, // 3: ABCDG
    0x66, // 4: BCFG
    0x6D, // 5: ACDFG
    0x7D, // 6: ACDEFG
    0x07, // 7: ABC
    0x7F, // 8: ABCDEFG
    0x6F  // 9: ABCDFG
};

inline inline_code void
sys_gpio_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN /*| RCC_APB2ENR_AFIOEN*/; // Enable clocks
    //GPIOA->CRL =
    GPIOA->CRL = 0x33333333;                           // PA0-PA7 outputs (segments)
    GPIOB->CRL = 0x88883333;                           // PB0-3 outputs, PB4-5 inputs pullup
    GPIOB->CRH = 0x00033300;                           // PB6-8 outputs
    GPIOB->ODR |= 0x30;                                // Enable pullups PB4-5
}

void display_digit(uint8_t pos, uint8_t val) {
    GPIOB->ODR |= 0x0F;                                // Disable all digits
    GPIOA->ODR = digit_patterns[val];                    // Set segments
    GPIOB->ODR &= ~(1 << pos);                         // Enable digit
}

int main(void) {
    _sys_init_external_clk();
    sys_gpio_init();

    for(;;)
    {
        //display_digit(digit_idx, digits[digit_idx]);       // Show current digit
        //digit_idx = (digit_idx + 1) & 3;                   // Next digit
        for(int l = 0; l < 3; l++){
            for(int i = 0; i < 10; i++){
            display_digit(l, i);
            _delay_ticks(10);
            }
        }
    }
}
