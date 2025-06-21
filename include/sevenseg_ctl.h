#ifndef   __SEVENSEG_CTL_H__
#define   __SEVENSEG_CTL_H__
#endif // __SEVENSEG_CTL_H__

#include <stdint.h>

// 7-segment patterns for digits 0-9 (without decimal point)
volatile static const uint8_t digit_patterns[10] = {
    0x3F,  // 0: ABCDEF
    0x06,  // 1: BC
    0x5B,  // 2: ABDEG
    0x4F,  // 3: ABCDG
    0x66,  // 4: BCFG
    0x6D,  // 5: ACDFG
    0x7D,  // 6: ACDEFG
    0x07,  // 7: ABC
    0x7F,  // 8: ABCDEFG
    0x6F   // 9: ABCDFG
};
// 0-3 segments (digit position)
volatile uint16_t digit_pins[4];

void
_display_init(void);
void
_display_off(void);
void
_set_digit(uint8_t position, uint8_t value);
void
_display_value(uint16_t value);

