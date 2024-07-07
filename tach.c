#include <avr/io.h>
#include "tach.h"

uint16_t ovf_cnt = 0;
uint16_t last = 0;
uint16_t per_n = 0;
uint8_t per_cnt = 1;
uint32_t per_accum = 0;

uint32_t tach_per = 0;
uint8_t tach_mul = 0;
uint8_t tach_div = 8;

void tach_setup()
{
    TCCR1B |= _BV(ICNC1) | _BV(CS11);  // clk = clk_io/8
    TIMSK |= _BV(ICIE1) | _BV(TOIE1); // enable input capture interrupt
    TACH_PORT |= _BV(TACH_N); // pullup
}

void tach_input(uint16_t cnt_val)
{
    uint32_t per = cnt_val - last;

    if(ovf_cnt>=OVF_MAX)
    {
        ovf_cnt=0;
        per_accum = 0;
        per_n = 0;
        last = cnt_val;
        return;
    }

    if(ovf_cnt != 0)
    {
        ovf_cnt--;
        if(cnt_val >= last)
        {
            per += TACH_CNT_MAX;
        }
        per += TACH_CNT_MAX*ovf_cnt;
        ovf_cnt = 0;
    }
    per_accum += per;
    last = cnt_val;

    if(++per_n==per_cnt)
    {
        tach_per = per_accum;
        tach_mul = per_cnt;

        if((per_accum > PER_TARGET)&&(per_cnt > PER_MUL_MIN))
        {
            per_cnt/=2;
        }
        else if((per_accum < PER_TARGET/2)&&(per_cnt < PER_MUL_MAX))
        {
            per_cnt*=2;
        }

        per_n = 0;
        per_accum = 0;
    }
}

void tach_ovf(void)
{
    if(ovf_cnt>=OVF_MAX)
    {
        tach_mul = 0;
        per_n = 0;
    }
    else
    {
        ovf_cnt++;
    }
}
