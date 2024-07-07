/*
 * This file is part of spintach, a tachometer/frequency meter using ATtiny2313A
 * Copyright (c) 2024 - Franco Sauvisky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "tach.h"
#include "pwmctrl.h"
#include "lcd-driver.h"

#define SW_PORT PORTD
#define SW_PIN PIND
#define SW_PORT PORTD
#define SW1_N PD2
#define SW2_N PD3
#define SW_DELAY_FAST 16

FUSES = {
    // attiny2313a, ext. crystal 4.096MHz
    .low = (FUSE_CKSEL1),
    .high = (FUSE_SPIEN),
};

uint8_t pwm_save EEMEM = 127;
uint8_t state_save EEMEM = 0;

int main(void)
{
    display_setup();
    pwm_setup();
    tach_setup();

    display_clear();

    uint8_t pwm_duty = eeprom_read_byte(&pwm_save);
    uint8_t state = eeprom_read_byte(&state_save);

    sei();
    uint8_t button1 = 0, button2 = 0;
    for (uint8_t i = 0;; i++)
    {
        if (bit_is_clear(SW_PIN, SW1_N))
        {
            if(button1==0)
            {
                button1 = SW_DELAY_FAST;
            }
            else
            {
                button1--;
                if(button1==0)
                {
                    if(pwm_duty<255) pwm_duty++;
                    button1=1;
                }
            }
        }
        else
        {
            if((button1 > 1)&&(button1 < SW_DELAY_FAST))
            {
                if(pwm_duty<255) pwm_duty++;
            }
            button1=0;
        }

        if (bit_is_clear(SW_PIN, SW2_N))
        {
            if(button2==0)
            {
                button2 = SW_DELAY_FAST;
            }
            else
            {
                button2--;
                if(button2==0)
                {
                    if(pwm_duty>56) pwm_duty--;
                    button2=1;
                }
            }
        }
        else
        {
            if((button2 > 1)&&(button2 < SW_DELAY_FAST))
            {
                if(pwm_duty>56) pwm_duty--;
            }
            button2=0;
        }

        if((button1>1)&&(button2>1))
        {
            if(button1<=SW_DELAY_FAST)
            {
                state^=1;
                eeprom_write_byte(&state_save,state&1);
                if(~state&1) eeprom_write_byte(&pwm_save,pwm_duty);
            } 
            button1=button2=0xFF;
        }

        pwm_set_duty((state&1)?pwm_duty:0);
        display_rpm(tach_rpm(),pwm_duty,state);
        _delay_ms(20);
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
