/*Copyright (C) 2025 Kravchuk Roman <darkkitsunezx128k2309@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 * STM32F103C8T6 Frequency Meter Implementation
 * Using CMSIS Library Only
 *
 * Features:
 * - Frequency measurement using Timer Input Capture
 * - 4-digit 7-segment LED display
 * - LED indicators for frequency divider status
 * - Two-button control panel (Reset/Set Divider)
 * - Multiple frequency ranges with automatic scaling
 */


#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif
#include <shared_functions.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpio_pins.h>

volatile uint32_t period = 0;
volatile uint32_t overflow_counter = 0;
volatile uint32_t last_capture = 0;

// Configuration Constants
//#define SYSTEM_CLOCK        72000000    // 72MHz system clock
#define MEASUREMENT_PERIOD  1000        // 1 second measurement window (ms)
#define DEBOUNCE_TIME       50          // Button debounce time (ms)

// Pin Definitions
// 7-Segment Display (Common Cathode) - Port B
#define DISPLAY_PORT        GPIOB
#define SEG_A_PIN           GPIO_PIN_0
#define SEG_B_PIN           GPIO_PIN_1
#define SEG_C_PIN           GPIO_PIN_2
#define SEG_D_PIN           GPIO_PIN_3
#define SEG_E_PIN           GPIO_PIN_4
#define SEG_F_PIN           GPIO_PIN_5
#define SEG_G_PIN           GPIO_PIN_6
#define SEG_DP_PIN          GPIO_PIN_7

// Digit Select Pins - Port A
#define DIGIT_PORT          GPIOA
#define DIGIT1_PIN          GPIO_PIN_4
#define DIGIT2_PIN          GPIO_PIN_5
#define DIGIT3_PIN          GPIO_PIN_6
#define DIGIT4_PIN          GPIO_PIN_7

// Control Buttons - Port A
#define BUTTON_PORT         GPIOA
#define RESET_BUTTON_PIN    GPIO_PIN_0
#define DIVIDER_BUTTON_PIN  GPIO_PIN_1

// LED Indicators - Port C
#define LED_PORT            GPIOC
#define LED_DIV1_PIN        GPIO_PIN_13  // Divider x1 indicator
#define LED_DIV10_PIN       GPIO_PIN_14  // Divider x10 indicator
#define LED_DIV100_PIN      GPIO_PIN_15  // Divider x100 indicator

// Input Signal Pin - Timer 2 Channel 1 (PA0 - conflicts with button, using PA8)
#define INPUT_PORT          GPIOA
#define INPUT_PIN           GPIO_PIN_8

// Global Variables
volatile uint32_t frequency = 0;
volatile uint32_t pulse_count = 0;
//volatile uint32_t last_capture = 0;
volatile bool measurement_ready = false;

typedef enum {
    DIV_1 = 1,
    DIV_10 = 10,
    DIV_100 = 100
} frequency_divider_t;

volatile frequency_divider_t current_divider = DIV_1;
volatile uint32_t display_value = 0;
volatile uint8_t current_digit = 0;

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

// Function Prototypes
void system_init(void);
void gpio_init(void);
void timer_init(void);
void systick_init(void);
void display_init(void);
void update_display(void);
void display_digit(uint8_t digit, uint8_t value);
void clear_display(void);
void update_leds(void);
void process_buttons(void);
uint32_t read_button(GPIO_TypeDef* port, uint16_t pin);
//void delay_ms(uint32_t ms);

// System Initialization
void system_init(void) {
    // Enable HSE and configure PLL for 72MHz
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // Configure PLL: HSE * 9 = 72MHz
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // Configure flash latency
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    // Select PLL as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // Configure APB1 = HCLK/2, APB2 = HCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
}

// GPIO Initialization
void gpio_init(void) {
    // Enable GPIO clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    // Configure display segment pins (PB0-PB7) as output push-pull
    DISPLAY_PORT->CRL = 0x33333333; // All pins as output 50MHz push-pull

    // Configure digit select pins (PA4-PA7) as output push-pull
    DIGIT_PORT->CRL &= ~(0xFFFF0000);
    DIGIT_PORT->CRL |= 0x33330000;

    // Configure button pins (PA0, PA1) as input with pull-up
    BUTTON_PORT->CRL &= ~(0x000000FF);
    BUTTON_PORT->CRL |= 0x00000088; // Input with pull-up/pull-down
    BUTTON_PORT->ODR |= RESET_BUTTON_PIN | DIVIDER_BUTTON_PIN; // Enable pull-up

    // Configure LED pins (PC13-PC15) as output push-pull
    LED_PORT->CRH &= ~(0xFFF00000);
    LED_PORT->CRH |= 0x33300000;

    // Configure input signal pin (PA8) for Timer 1 Channel 1
    INPUT_PORT->CRH &= ~(0x0000000F);
    INPUT_PORT->CRH |= 0x00000004; // Floating input
}

// Timer Initialization for Frequency Measurement
void timer_init(void) {
    // Enable Timer 1 clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Configure Timer 1 for input capture
    TIM1->PSC = 71; // Prescaler for 1MHz timer clock (72MHz / 72 = 1MHz)
    TIM1->ARR = 0xFFFF; // Maximum auto-reload value

    // Configure Channel 1 for input capture
    TIM1->CCMR1 |= TIM_CCMR1_CC1S_0; // CC1 channel configured as input, IC1 mapped on TI1
    TIM1->CCMR1 |= TIM_CCMR1_IC1F_3; // Input capture filter
    TIM1->CCER |= TIM_CCER_CC1E; // Enable capture

    // Enable capture/compare interrupt
    TIM1->DIER |= TIM_DIER_CC1IE;
    NVIC_EnableIRQ(TIM1_CC_IRQn);

    // Start timer
    TIM1->CR1 |= TIM_CR1_CEN;

    // Timer 3 for measurement window (1 second)
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 7199; // Prescaler for 10kHz (72MHz / 7200 = 10kHz)
    TIM3->ARR = 9999; // 1 second period (10000 / 10kHz = 1s)
    TIM3->DIER |= TIM_DIER_UIE; // Enable update interrupt
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= TIM_CR1_CEN; // Start timer
}

// SysTick Initialization for Display Multiplexing
void systick_init(void) {
    SysTick->LOAD = (SYSCLOCK / 1000) - 1; // 1ms tick
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

// Display Initialization
void display_init(void) {
    clear_display();
    current_digit = 0;
}

// Clear Display
void clear_display(void) {
    DISPLAY_PORT->ODR = 0x00;
    DIGIT_PORT->ODR |= (DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN | DIGIT4_PIN);
}

// Display Single Digit
void display_digit(uint8_t digit, uint8_t value) {
    // Clear all digits first
    DIGIT_PORT->ODR |= (DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN | DIGIT4_PIN);

    // Set segment pattern
    DISPLAY_PORT->ODR = digit_patterns[value];

    // Enable specific digit (active low)
    switch(digit) {
        case 0: DIGIT_PORT->ODR &= ~DIGIT1_PIN; break;
        case 1: DIGIT_PORT->ODR &= ~DIGIT2_PIN; break;
        case 2: DIGIT_PORT->ODR &= ~DIGIT3_PIN; break;
        case 3: DIGIT_PORT->ODR &= ~DIGIT4_PIN; break;
    }
}

// Update Display (Called from SysTick)
void update_display(void) {
    static uint32_t digit_timer = 0;

    if (++digit_timer >= 2) { // Change digit every 2ms
        digit_timer = 0;

        uint32_t temp = display_value;
        uint8_t digits[4];

        // Extract digits
        digits[3] = temp % 10; temp /= 10;
        digits[2] = temp % 10; temp /= 10;
        digits[1] = temp % 10; temp /= 10;
        digits[0] = temp % 10;

        // Display current digit
        display_digit(current_digit, digits[current_digit]);

        // Move to next digit
        current_digit = (current_digit + 1) % 4;
    }
}

// Update LED Indicators
void update_leds(void) {
    // Clear all LEDs
    LED_PORT->ODR |= (LED_DIV1_PIN | LED_DIV10_PIN | LED_DIV100_PIN);

    // Set appropriate LED (active low)
    switch(current_divider) {
        case DIV_1:   LED_PORT->ODR &= ~LED_DIV1_PIN; break;
        case DIV_10:  LED_PORT->ODR &= ~LED_DIV10_PIN; break;
        case DIV_100: LED_PORT->ODR &= ~LED_DIV100_PIN; break;
    }
}

// Button Reading with Debounce
uint32_t read_button(GPIO_TypeDef* port, uint16_t pin) {
    static uint32_t last_state = 0;
    static uint32_t debounce_timer = 0;

    uint32_t current_state = (port->IDR & pin) ? 0 : 1; // Active low

    if (current_state != last_state) {
        debounce_timer = 0;
        last_state = current_state;
        return 0;
    }

    if (++debounce_timer > DEBOUNCE_TIME && current_state) {
        debounce_timer = 0;
        return 1;
    }

    return 0;
}

// Process Button Presses
void process_buttons(void) {
    static uint32_t reset_pressed = 0;
    static uint32_t divider_pressed = 0;

    // Reset button
    if (read_button(BUTTON_PORT, RESET_BUTTON_PIN) && !reset_pressed) {
        reset_pressed = 1;
        frequency = 0;
        pulse_count = 0;
        display_value = 0;
    } else if (!(BUTTON_PORT->IDR & RESET_BUTTON_PIN)) {
        reset_pressed = 0;
    }

    // Divider button
    if (read_button(BUTTON_PORT, DIVIDER_BUTTON_PIN) && !divider_pressed) {
        divider_pressed = 1;
        switch(current_divider) {
            case DIV_1:   current_divider = DIV_10; break;
            case DIV_10:  current_divider = DIV_100; break;
            case DIV_100: current_divider = DIV_1; break;
        }
        update_leds();
    } else if (!(BUTTON_PORT->IDR & DIVIDER_BUTTON_PIN)) {
        divider_pressed = 0;
    }
}

// Delay Function
/*void delay_ms(uint32_t ms) {
    uint32_t start = SysTick->VAL;
    uint32_t ticks = ms * (SYSCLOCK / 1000);

    while ((start - SysTick->VAL) < ticks);
}*/

// Timer 1 Capture/Compare Interrupt Handler
void TIM1_CC_IRQHandler(void) {
    if (TIM1->SR & TIM_SR_CC1IF) {
        TIM1->SR &= ~TIM_SR_CC1IF; // Clear interrupt flag

        uint32_t current_capture = TIM1->CCR1;

        if (pulse_count > 0) {
            // Calculate frequency based on period between pulses
            uint32_t period = current_capture - last_capture;
            if (period > 0) {
                frequency = 1000000 / period; // Timer runs at 1MHz
            }
        }

        last_capture = current_capture;
        pulse_count++;
    }
}

// Timer 3 Update Interrupt Handler (Measurement Window)
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF; // Clear interrupt flag

        // Measurement window completed
        measurement_ready = true;

        // Calculate display value based on divider
        display_value = frequency / current_divider;

        // Limit display to 4 digits
        if (display_value > 9999) {
            display_value = 9999;
        }

        // Reset for next measurement
        pulse_count = 0;
    }
}

// SysTick Handler for Display Multiplexing
void SysTick_Handler(void) {
    update_display();
    process_buttons();
}

// Main Function
int main(void) {
    // Initialize system
    system_init();
    gpio_init();
    timer_init();
    systick_init();
    display_init();

    // Initialize LEDs
    update_leds();

    // Display startup pattern
    display_value = 8888;
    _delay_msecs(1000);
    display_value = 0;

    while (1) {
        // Main loop - most work done in interrupts
        if (measurement_ready) {
            measurement_ready = false;

            // Additional processing can be done here
            // For example, averaging multiple measurements
            // or implementing auto-ranging
        }

        // Power saving - could implement WFI here
        __WFI(); // Wait for interrupt
    }
}

/*
 * Hardware Connections:
 *
 * 7-Segment Display (Common Cathode):
 * - Segments A-G, DP: PB0-PB7
 * - Digit 1-4 select: PA4-PA7 (active low)
 *
 * Control Panel:
 * - Reset button: PA0 (active low, internal pull-up)
 * - Divider button: PA1 (active low, internal pull-up)
 *
 * LED Indicators:
 * - Divider x1: PC13 (active low)
 * - Divider x10: PC14 (active low)
 * - Divider x100: PC15 (active low)
 *
 * Input Signal:
 * - Frequency input: PA8 (Timer 1 Channel 1)
 *
 * Features:
 * - Frequency range: 1 Hz to 9999 Hz (with divider)
 * - Three selectable dividers: x1, x10, x100
 * - Real-time 4-digit LED display
 * - Button debouncing
 * - Interrupt-driven measurement
 * - Low power design with WFI
 */
