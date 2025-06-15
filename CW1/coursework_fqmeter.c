#include <stm32f1xx.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Підключення:
 * 7-сегментний дисплей з спільним катодом:
 * - сегменти A-G, DP: PB0-PB7
 * - розряд 1-4: PA4-PA7 (active low)
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


// основна функція
int main(void) {

    for(;;)
    {
    }
}
