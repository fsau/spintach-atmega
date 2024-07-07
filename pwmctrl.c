#include <avr/io.h>
#include "pwmctrl.h"

void pwm_setup()
{
    PWM_DDR |= _BV(PWM_N);            // set as output
    TCCR0A = _BV(WGM01) | _BV(WGM00); // fast pwm, TOP=OCR0A
    TCCR0B = _BV(WGM02) | _BV(CS00);  // clk = clk_io
    OCR0A = 255;                      // min freq
}

void pwm_set(uint8_t duty, uint8_t per)
{
    if (duty == 0)
        TCCR0A &= ~_BV(COM0B1); // disable
    else
        TCCR0A |= _BV(COM0B1); // enable OCR0B

    OCR0B = ((uint16_t)per * duty) / 255;
    OCR0A = per;
}

void pwm_set_duty(uint8_t duty)
{
    if (duty == 0)
        TCCR0A &= ~_BV(COM0B1); // disable
    else
        TCCR0A |= _BV(COM0B1); // enable OCR0B

    OCR0B = ((uint16_t)OCR0A * duty) / 255;
}
