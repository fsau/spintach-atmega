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
#include <util/delay.h>
#include "uart.h"
#include <util/setbaud.h>

uint16_t buff[BUFF_SIZE];
uint8_t buff_i=0, buff_n=0;

void uart_setup(void)
{
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X); // Use 2x prescaler if needed
#else
	UCSRA &= ~(1 << U2X);
#endif
	UCSRB |= _BV(TXEN) | _BV(TXCIE);
    UART_DDR |= _BV(UART_TX_N);
}

static void send_byte(uint8_t val)
{
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = val;
}

static void buff_push_byte(uint8_t val)
{
    buff[buff_n] = val;
    buff_n = (buff_n+1)%BUFF_SIZE;
}

static uint8_t buff_pop_byte()
{
    uint8_t val = buff[buff_i];
    buff_i = (buff_i+1)%BUFF_SIZE;
    return val;
}

static uint8_t buff_is_empty()
{
    return (buff_i==buff_n);
}

void uart_send_byte(uint8_t val)
{
    if(bit_is_set(UCSRA, UDRE)&&buff_is_empty())
    {
        send_byte(val);
    }
    else
    {
        buff_push_byte(val);
    }
}

void uart_send_word(uint16_t val)
{
    uint8_t high,low;
    high = (val>>8)&0xFF;
    low = val&0xFF;
    uart_send_byte(high);
    buff_push_byte(low);
}

void uart_tx_int()
{
    if(!buff_is_empty())
    {
        send_byte(buff_pop_byte());
    }
}