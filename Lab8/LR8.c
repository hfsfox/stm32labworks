#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif
#include <shared_functions.h>

int main(void)
{
    /*
     // Enable clock at GPIO port C
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN_Msk;
    //enable TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;
    // Execute dsb instruction after enabling the peripheral clock, as per the errata
    //https://www.st.com/resource/en/errata_sheet/es0222-stm32f401xb-and-stm32f401xc-device-errata-stmicroelectronics.pdf
    //__DSB();
    _set_dsb();

    // Set pin 13 connected to blue led of port C as output
    //GPIOC->MODER |= (1 << GPIO_MODER_MODER13_Pos);

    GPIOC->MODER |= GPIO_MODER_MODER13_Pos;
    */

    for(;;)
    {
        
    }

    return 0;
}
