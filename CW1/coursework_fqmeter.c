#include <stm32f1xx.h>
#include <stdint.h>
#include <stdbool.h>

/*
 *  Частотомір з динамічною індикацією
 *
 * Підключення:
 * 7-сегментний дисплей з спільним катодом:
 * - сегменти A-G, DP: PB0-PB7
 * - розряд 1-4: PA4-PA7 (active low)
 * Вхід сигналу:
 * - PA8 (Timer 1 Channel 1)
 * Control Panel:
 * - Кнопка перезапуску: PA0 (active low, internal pull-up)
 * - Кнопка вибору дільника: PA1 (active low, internal pull-up)
 *
 * LED індикатори дільника:
 * - Divider x1: PC13 (active low)
 * - Divider x10: PC14 (active low)
 * - Divider x100: PC15 (active low)
 *
 */

// для зручності номери портів GPIO взято з HAL
#define GPIO_PIN_0                 ((uint16_t)0x0001)  // Pin 0 selected
#define GPIO_PIN_1                 ((uint16_t)0x0002)  // Pin 1 selected
#define GPIO_PIN_2                 ((uint16_t)0x0004)  // Pin 2 selected
#define GPIO_PIN_3                 ((uint16_t)0x0008)  // Pin 3 selected
#define GPIO_PIN_4                 ((uint16_t)0x0010)  // Pin 4 selected
#define GPIO_PIN_5                 ((uint16_t)0x0020)  // Pin 5 selected
#define GPIO_PIN_6                 ((uint16_t)0x0040)  // Pin 6 selected
#define GPIO_PIN_7                 ((uint16_t)0x0080)  // Pin 7 selected
#define GPIO_PIN_8                 ((uint16_t)0x0100)  // Pin 8 selected
#define GPIO_PIN_9                 ((uint16_t)0x0200)  // Pin 9 selected
#define GPIO_PIN_10                ((uint16_t)0x0400)  // Pin 10 selected
#define GPIO_PIN_11                ((uint16_t)0x0800)  // Pin 11 selected
#define GPIO_PIN_12                ((uint16_t)0x1000)  // Pin 12 selected
#define GPIO_PIN_13                ((uint16_t)0x2000)  // Pin 13 selected
#define GPIO_PIN_14                ((uint16_t)0x4000)  // Pin 14 selected
#define GPIO_PIN_15                ((uint16_t)0x8000)  // Pin 15 selected
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  // All pins selected

// масив кодів знаків 7-сегментного дисплею (0-9)(Спільний катод)
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
// робоча частота зовнішнього джерела тактового сигналу
#define SYSCLOCK 72000000U

//прототипи функцій
//void _delay_ticks(volatile uint32_t ticks);
//void _delay_msecs(volatile uint32_t ms);
//void _delay_usecs(volatile uint32_t ms);
//void _work_fq_setup(void);
//void _sys_init_external_clk(void);
//void _sys_init_internal_clk(void);

inline void
SysInitExternalClock(void)
{
    RCC->CR |= RCC_CR_HSEON;                           // увімкнути HSE ()
    while(!(RCC->CR & RCC_CR_HSERDY));                 // Чекати поки HSE буде готовий
    RCC->CFGR |= RCC_CFGR_SW_HSE;                      // Select HSE як системний тактовий генератор
    SystemCoreClockUpdate();                           // оновити змінну clock ядра
}

// Функція затримки в мілісекундах
inline void
delay_msecs(volatile uint32_t ms) {
    //MODIFY_REG(SysTick->VAL,SysTick_VAL_CURRENT_Msk,SYSCLOCK / 1000 - 1);
    SysTick->VAL = (SysTick->VAL & ~SysTick_VAL_CURRENT_Msk) | (SYSCLOCK / 1000 - 1);
    volatile uint32_t SysTick_CNT = 0;
    SysTick_CNT = ms;
    while(SysTick_CNT)
    {
        __asm volatile
            (
                "nop" // вимкнення оптимізації для циклів комплілятора
            );
    }
}

// Функція затримки в мікросекундах
inline void
delay_usecs(volatile uint32_t ms) {
    SysTick->VAL = (SysTick->VAL & ~SysTick_VAL_CURRENT_Msk) | (SYSCLOCK / 1000000 - 1);
    volatile uint32_t SysTick_CNT = 0;
    SysTick_CNT = ms;
    while(SysTick_CNT)
    {
        __asm volatile
            (
                "nop" // вимкнення оптимізації для циклів комплілятора
            );
    }
}
// Затримка в процесорних тактах
inline void
delay_ticks(volatile uint32_t ticks)
{
    SysTick->VAL = (SysTick->VAL & ~SysTick_VAL_CURRENT_Msk) | (SYSCLOCK);
    while (ticks)
    {
        ticks--;
    }
}


// основна функція
int main(void) {

    for(;;)
    {
    }
}
