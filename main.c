#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "tach.h"
#include "pwmctrl.h"
#include "lcd-driver.h"

#define SW_PORT PORTD
#define SW_PIN PIND
#define SW_PORT PORTD
#define SW1_N PD2
#define SW2_N PD3

FUSES = {
    // attiny2313a, ext. crystal 4.096MHz
    .low = (FUSE_CKSEL1),
    .high = (FUSE_SPIEN),
};

// todos: zero

int main(void)
{
    display_setup();
    pwm_setup();
    tach_setup();

    display_clear();
    sei();
    uint8_t sel = 0;
    for (uint8_t i = 0;; i++)
    {
        pwm_set(127, 255 - i / 8);
        if (bit_is_clear(SW_PIN, SW1_N))
        {
            if (~sel & 2)
            {
                sel ^= 1;
                display_clear();
            }
            sel |= 2;
        }
        else
        {
            sel &= ~2;
        }

        if (sel & 1)
            display_test(bit_is_clear(SW_PIN, SW1_N), bit_is_clear(SW_PIN, SW2_N));
        else
            display_rpm();
        _delay_ms(10);
    }

    return 0;
}

ISR(TIMER1_CAPT_vect)
{
    tach_input(ICR1);
}

ISR(TIMER1_OVF_vect)
{
    tach_ovf();
}
