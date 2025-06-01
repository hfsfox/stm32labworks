/**
 * Copyright (C) 2025 Kravchuk Roman <darkkitsunezx128k2309@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 * Course work 1 -- FqMeter
 */

#include "stm32f411xe.h"
#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
//#elif defined (__TARGET_STM32F103C8T6__)
//#include <stm32f1xx.h>
#endif
#include <shared_functions.h>


void
timer_setup(void)
{
    #if defined (__TARGET_STM32F401CEU6__)
    #endif
}

void
gpio_setup(void)
{
}


int main(void)
{
    #if defined (__TARGET_STM32F401CEU6__)
    // enable GPIOA and TIM2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    GPIOA->MODER |= 0x2; // PA0 in alternate function mode

    // clear pin 0 (first bit) alternate function and
    // initialize to AF01 (TIM2 CH1)
    GPIOA->AFR[0] |= (uint8_t)1;

    // configure timer
    TIM2->PSC = 1000; // clear prescaler
    TIM2->ARR = 200; // set period (Auto Reload Register)

    // configure capture/compare 1
    TIM2->CCMR1 |= 0x60; // PWM1 mode
    TIM2->CCR1 = 100; // pulse width in cycles
    TIM2->CCER |= 1;  // enable cc1

    // enable TIM1
    TIM2->CR1 |= TIM_CR1_CEN;
    #endif

    for(;;)
    {
        for(uint16_t i=0; i == sizeof(uint16_t); i++)
        {
            //WRITE_REG(TIM2->CCR3, i);
            TIM2->CCR3 = i;
            _delay_msecs(10);
        }
    };
}

/*int main(void)
{
    for(;;)
    {
        //_delay_msecs(100);
        //_delay_msecs(100);
    }
}*/
