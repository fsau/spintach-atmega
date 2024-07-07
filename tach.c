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
#include "tach.h"

uint16_t ovf_cnt = 0;
uint16_t last_t = 0;
uint16_t per_i = 0;
uint8_t per_n = 1;
uint32_t per_accum = 0;

uint32_t tach_per = 0;
uint8_t tach_mul = 0;

void tach_setup()
{
    TCCR1B |= _BV(ICNC1) | _BV(CS11);  // clk = clk_io/8
    TIMSK |= _BV(ICIE1) | _BV(TOIE1); // enable input capture interrupt
    TACH_PORT |= _BV(TACH_N); // pullup
}

void tach_input(uint16_t now_t)
{
    uint32_t per = now_t - last_t;

    if(ovf_cnt>=OVF_MAX)
    {
        ovf_cnt=0;
        per_accum = 0;
        per_i = 0;
        last_t = now_t;
        return;
    }

    if(ovf_cnt != 0)
    {
        ovf_cnt--;
        if(now_t >= last_t)
        {
            per += TACH_CNT_MAX;
        }
        per += TACH_CNT_MAX*ovf_cnt;
        ovf_cnt = 0;
    }

    per_accum += per;
    last_t = now_t;

    if(++per_i==per_n)
    {
        tach_per = per_accum;
        tach_mul = per_n;

        if((per_accum > PER_TARGET)&&(per_n > PER_MUL_MIN))
        {
            per_n/=2;
        }
        else if((per_accum < PER_TARGET/2)&&(per_n < PER_MUL_MAX))
        {
            per_n*=2;
        }

        per_i = 0;
        per_accum = 0;
    }
}

void tach_ovf(void)
{
    if(ovf_cnt>=OVF_MAX)
    {
        tach_mul = 0;
        per_i = 0;
    }
    else
    {
        ovf_cnt++;
    }
}

/*
clock = clk_io/8 = 4.096 MHz / 8 = 512 kHz
unit = 1/clock = 1.953125 µs
max_period = 2^16 * unit = 0.128 s
RPM = 60*f_in/2 = 30/(unit * cnt) = 15360000 / cnt
*/
uint16_t tach_rpm(void)
{
    uint16_t r = 0;
    if(tach_per > 50)
    {
        r = (uint32_t) 15360000 * tach_mul / tach_per;
    }
    return r;
}