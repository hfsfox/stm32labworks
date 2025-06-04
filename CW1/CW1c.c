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
#include <shared_functions.h>
#include <stdint.h>
#include <stdbool.h>

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

// System configuration - 8MHz HSI for simulation
#define SYS_FREQ 8000000
#define TIM_FREQ 1000000  // 1MHz timer frequency

// Pin definitions - optimized port usage
#define DISP_SEG_PORT GPIOA        // PA0-PA7 segments
#define DISP_DIG_PORT GPIOB        // PB0-PB3 digit select
#define BTN_PORT      GPIOB        // PB4-PB5 buttons
#define LED_PORT      GPIOB        // PB6-PB8 LEDs
#define INPUT_PORT    GPIOA        // PA15 TIM2_CH1

// Global variables
volatile uint32_t period = 0;
volatile uint32_t last_capture = 0;
volatile uint32_t overflow_counter = 0;
volatile uint32_t frequency = 0;
volatile uint8_t divider = 1;       // 1, 10, 100
volatile uint16_t display_val = 0;
volatile uint8_t digit_idx = 0;

// 7-segment patterns (0-9)
const uint8_t seg_patterns[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void sys_init(void) {
    RCC->CR |= RCC_CR_HSION;                           // Enable HSI
    while(!(RCC->CR & RCC_CR_HSIRDY));                 // Wait HSI ready
    RCC->CFGR |= RCC_CFGR_SW_HSI;                      // Select HSI as sysclk
    SystemCoreClockUpdate();                           // Update core clock variable
}

void gpio_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // Enable clocks

    GPIOA->CRL = 0x33333333;                           // PA0-PA7 outputs (segments)
    GPIOA->CRH &= ~0xF0000000; GPIOA->CRH |= 0x40000000; // PA15 floating input (TIM2_CH1)

    GPIOB->CRL = 0x88883333;                           // PB0-3 outputs, PB4-5 inputs pullup
    GPIOB->CRH = 0x00033300;                           // PB6-8 outputs
    GPIOB->ODR |= 0x30;                                // Enable pullups PB4-5
}

void tim2_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                // Enable TIM2 clock

    TIM2->PSC = (SYS_FREQ / TIM_FREQ) - 1;             // Set prescaler for 1MHz
    TIM2->ARR = 0xFFFFFFFF;                            // Max auto-reload
    TIM2->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_IC1F_3; // IC1->TI1, filter
    TIM2->CCER = TIM_CCER_CC1E;                        // Enable capture
    TIM2->DIER = TIM_DIER_CC1IE | TIM_DIER_UIE;        // Enable interrupts

    NVIC_EnableIRQ(TIM2_IRQn);                         // Enable TIM2 interrupt
    TIM2->CR1 = TIM_CR1_CEN;                           // Start timer
}

void tim3_init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;                // Enable TIM3 clock

    TIM3->PSC = (SYS_FREQ / 1000) - 1;                 // 1ms tick
    TIM3->ARR = 1;                                     // 2ms period for display mux
    TIM3->DIER = TIM_DIER_UIE;                         // Enable update interrupt

    NVIC_EnableIRQ(TIM3_IRQn);                         // Enable TIM3 interrupt
    TIM3->CR1 = TIM_CR1_CEN;                           // Start timer
}

void display_digit(uint8_t pos, uint8_t val) {
    GPIOB->ODR |= 0x0F;                                // Disable all digits
    GPIOA->ODR = seg_patterns[val];                    // Set segments
    GPIOB->ODR &= ~(1 << pos);                         // Enable digit
}

void update_leds(void) {
    GPIOB->ODR &= ~0x1C0;                              // Clear LED bits
    if(divider == 1)   GPIOB->ODR |= (1 << 6);        // LED1 on
    if(divider == 10)  GPIOB->ODR |= (1 << 7);        // LED2 on
    if(divider == 100) GPIOB->ODR |= (1 << 8);        // LED3 on
}

void process_buttons(void) {
    static uint8_t btn_state = 0;
    uint8_t btn_curr = (~GPIOB->IDR >> 4) & 0x03;     // Read buttons (active low)

    if((btn_curr & 1) && !(btn_state & 1)) {          // Reset button pressed
        frequency = 0; display_val = 0;
    }
    if((btn_curr & 2) && !(btn_state & 2)) {          // Divider button pressed
        divider = (divider == 100) ? 1 : divider * 10;
        update_leds();
    }
    btn_state = btn_curr;
}

void TIM2_IRQHandler(void) {
    if(TIM2->SR & TIM_SR_CC1IF) {                      // Capture interrupt
        TIM2->SR &= ~TIM_SR_CC1IF;                     // Clear flag

        uint32_t capture = TIM2->CCR1;                 // Read capture value
        period = (capture - last_capture) + (overflow_counter << 32); // Calculate period
        last_capture = capture;                        // Store for next calculation
        overflow_counter = 0;                          // Reset overflow counter

        if(period > 0) frequency = TIM_FREQ / period;  // Calculate frequency
    }

    if(TIM2->SR & TIM_SR_UIF) {                        // Overflow interrupt
        TIM2->SR &= ~TIM_SR_UIF;                       // Clear flag
        overflow_counter++;                            // Increment overflow counter
    }
}

void TIM3_IRQHandler(void) {
    TIM3->SR &= ~TIM_SR_UIF;                           // Clear interrupt flag

    // Display multiplexing
    uint16_t temp = frequency / divider;               // Apply divider
    display_val = (temp > 9999) ? 9999 : temp;        // Limit to 4 digits

    uint8_t digits[4] = {                              // Extract digits
        display_val / 1000,
        (display_val / 100) % 10,
        (display_val / 10) % 10,
        display_val % 10
    };

    display_digit(digit_idx, digits[digit_idx]);       // Show current digit
    digit_idx = (digit_idx + 1) & 3;                   // Next digit

    process_buttons();                                 // Handle buttons
}

int main(void) {
    sys_init();                                        // Initialize system clock
    gpio_init();                                       // Configure GPIO
    tim2_init();                                       // Setup input capture
    tim3_init();                                       // Setup display timer
    update_leds();                                     // Initialize LED state

    while(1) {
        __WFI();                                       // Wait for interrupt
    }
}

/*
 * Pin Assignments:
 * PA0-PA7: 7-segment display segments A-G,DP
 * PA15: Frequency input (TIM2_CH1)
 * PB0-PB3: Digit select (active low)
 * PB4: Reset button (active low)
 * PB5: Divider button (active low)
 * PB6 PB7 PB8: Divider LEDs (÷1, ÷10, ÷100)
 *
 * Features:
 * - 8MHz HSI clock for simulation
 * - Period measurement with overflow handling
 * - 4-digit multiplexed display
 * - Three frequency ranges
 * - Compact interrupt-driven design
 */
