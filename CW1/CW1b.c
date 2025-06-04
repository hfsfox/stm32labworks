#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif
#include <shared_functions.h>
#include <stdint.h>
#include <stdbool.h>

volatile uint32_t period = 0;
volatile uint32_t overflow_counter = 0;
volatile uint32_t last_capture = 0;

// from HAL
#if defined (__TARGET_STM32F103C8T6__)
#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */
#endif

void
tim2_init(void)
{
    // Enable Timer 2 clock
    RCC->APB2ENR |= RCC_APB1ENR_TIM2EN;
    if(TIM2->SR & TIM_SR_CC1IF)
    // Configure Timer 1 for input capture
    TIM2->PSC = 72-1; // Prescaler for 1MHz timer clock (72MHz / 72 = 1MHz)
    TIM2->ARR = 0xFFFF; // Maximum auto-reload value
    //
    // Configure Channel 1 for input capture
    /*TIM1->CCMR1 |= TIM_CCMR1_CC1S_0; // CC1 channel configured as input, IC1 mapped on TI1
    TIM1->CCMR1 |= TIM_CCMR1_IC1F_3; // Input capture filter
    TIM1->CCER |= TIM_CCER_CC1E; // Enable capture*/
}

void
gpio_init()
{
    // Enable GPIO clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
}

int
main(void)
{
    return 0;
}
