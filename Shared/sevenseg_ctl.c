#include <sevenseg_ctl.h>

void
_display_init(void)
{
}

void
_display_off(void)
{
}

void
_set_digit(uint8_t position, uint8_t value)
{
    // Validate inputs
    if (position > 3 || value > 9) return;
}

void
_display_value(uint16_t counter)
{
    uint8_t digits[4];
    // заповнення буфера розряду з лічильника
        digits[0] = (counter / 1000) % 10; // тисячі
        digits[1] = (counter / 100) % 10;  // сотні
        digits[2] = (counter / 10) % 10;   // десятки
        digits[3] = counter % 10;         // одиниці

        // Dynamic indication: quickly cycle through digits
        for (int refresh = 0; refresh < 50; ++refresh) {
            for (uint8_t i = 0; i < 4; ++i) {
                _set_digit(i, digits[i]);
            }
        }
        counter = (counter + 1) % 10000;
}
