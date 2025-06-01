/**
 * Copyright (C) 2025 Kravchuk Roman <darkkitsunezx128k2309@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 * Lab work 4 -- PWM
 */

#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif
#include <shared_functions.h>

int main(void)
{
    // Enable clock at GPIO port C
    /*RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN_Msk;
    //enable TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;
    // Execute dsb instruction after enabling the peripheral clock, as per the errata
    //https://www.st.com/resource/en/errata_sheet/es0222-stm32f401xb-and-stm32f401xc-device-errata-stmicroelectronics.pdf
    //__DSB();
    _set_dsb();

    // Set pin 13 connected to blue led of port C as output
    //GPIOC->MODER |= (1 << GPIO_MODER_MODER13_Pos);

    GPIOC->MODER |= GPIO_MODER_MODER13_Pos;

    for(;;)
    {
        
    }*/

    #if defined (__TARGET_STM32F401CEU6__)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    #elif defined (__TARGET_STM32F103C8T6__)
    //RCC->APB1ENR |= RCC_AHB1ENR_GPIOAEN;
    //RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= 1<<2;//включить тактирование порта A
	RCC->APB1ENR |= 1;//включить тактирование таймера 2
	//PA0 - выход ШИМ канал 1 таймер 2
	GPIOA->CRL |= 0b1011;//выход, частота 50МГц
	GPIOA->CRL &= 0b1011;//пушпул, альтернативная функция
	TIM2->PSC = 7200-1;//насколько делится максимальная частота
	TIM2->ARR = 5000-1;//до скольки таймер считает перед сбросом
	TIM2->CCR1 = 2500-1;//на каком числе переключение
	TIM2->CCER |= 1;//разблокируем выход
	TIM2->CCMR1 |= 3<<5;//режим ШИМ1
	TIM2->CR1 |= 1;//запускаем таймер
    #endif

    for(;;)
    {

    }
    return 0;
}
