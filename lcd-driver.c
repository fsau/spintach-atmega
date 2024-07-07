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
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd-driver.h"

static void strobe_e_line()
{
    LCD_PORT &= ~_BV(LCD_E_N);
    _delay_us(STROBE_LEN_US);
    LCD_PORT |= _BV(LCD_E_N);
    _delay_us(STROBE_LEN_US);
    LCD_PORT &= ~_BV(LCD_E_N);
}

static void send_data(uint8_t rs, uint8_t data)
{
    uint8_t temp;
    if (rs)
    {
        LCD_PORT |= _BV(LCD_RS_N);
    }
    else
    {
        LCD_PORT &= ~_BV(LCD_RS_N);
    }

    LCD_PORT &= ~_BV(LCD_E_N);

    temp = LCD_PORT;
    temp &= ~(_BV(LCD_D7_N) | _BV(LCD_D6_N) | _BV(LCD_D5_N) | _BV(LCD_D4_N));
    temp |= ((data & _BV(7)) ? _BV(LCD_D7_N) : 0) |
            ((data & _BV(6)) ? _BV(LCD_D6_N) : 0) |
            ((data & _BV(5)) ? _BV(LCD_D5_N) : 0) |
            ((data & _BV(4)) ? _BV(LCD_D4_N) : 0);
    LCD_PORT = temp;

    strobe_e_line();

    temp = LCD_PORT;
    temp &= ~(_BV(LCD_D7_N) | _BV(LCD_D6_N) | _BV(LCD_D5_N) | _BV(LCD_D4_N));
    temp |= ((data & _BV(3)) ? _BV(LCD_D7_N) : 0) |
            ((data & _BV(2)) ? _BV(LCD_D6_N) : 0) |
            ((data & _BV(1)) ? _BV(LCD_D5_N) : 0) |
            ((data & _BV(0)) ? _BV(LCD_D4_N) : 0);
    LCD_PORT = temp;

    strobe_e_line();

    LCD_PORT &= ~(_BV(LCD_D7_N) | _BV(LCD_D6_N) | _BV(LCD_D5_N) | _BV(LCD_D4_N));
}

static void set_pos(uint8_t line, uint8_t col)
{
    int8_t offset = 0;
    if (line)
    {
        offset = 64;
    }
    send_data(0, 0x80 + offset + col);
}

static void send_lcd_num(uint32_t num, uint8_t slen, uint8_t div, uint8_t line, uint8_t col)
{
    uint8_t str[slen];

    for (uint8_t i = slen; i; --i)
    {
        uint8_t dig = num % 10;
        if ((num != 0) || ((slen - i) <= div))
            str[i - 1] = '0' + dig;
        else
            str[i - 1] = ' ';
        num /= 10;
    }

    set_pos(line, col);
    for (uint8_t i = 0; i < slen; i++)
    {
        if ((slen - i) == div)
            send_data(1, '.');
        send_data(1, str[i]);
    }
}

static void send_lcd_cstr(const char str[], uint8_t line, uint8_t col)
{
    set_pos(line, col);
    uint8_t i = 0;
    char cts = pgm_read_byte(&str[0]);

    do
    {
        if (cts <= 0x10)
            cts--;
        send_data(1, cts);
        i++;
        cts = pgm_read_byte(&str[i]);
    } while (cts);
}

static void send_lcd_blank(uint8_t len, uint8_t line, uint8_t col)
{
    set_pos(line, col);
    for (uint8_t i = len; i; --i)
    {
        send_data(1, ' ');
    }
}

// chars "ãáçéê"
const uint8_t special_c[] PROGMEM = "SXTGUWUFHJTGUWUFFFTVWTJRHJTWeVTFJPTWeVTFHJFRJJTF";
#define INIT_STR_SIZE (sizeof(special_c) / sizeof(const uint8_t))

void display_setup(void)
{
    LCD_DDR |= (_BV(LCD_D7_N) | _BV(LCD_D6_N) | _BV(LCD_D5_N) |
                _BV(LCD_D4_N) | _BV(LCD_E_N) | _BV(LCD_RS_N) | _BV(LCD_RW_N));

    LCD_PORT |= _BV(LCD_D5_N) | _BV(LCD_D4_N);

    strobe_e_line();
    strobe_e_line();
    strobe_e_line();

    LCD_PORT &= ~_BV(LCD_D4_N); // 4-bit mode
    strobe_e_line();

    send_data(0, 0x28); // 4 bit/2 line
    send_data(0, 0x08); // display off
    send_data(0, 0x06); // auto increment, no shift

    for (uint8_t k = 0; k < INIT_STR_SIZE; k++)
    {
        send_data(0, 0x40 + k);                           // set cgaddr = 0;
        send_data(1, pgm_read_byte(&special_c[k]) - 'F'); // load special chars
    }
}

void display_clear()
{
    send_data(0, 0x28);
    send_data(0, 0x06);
    send_data(0, 0x0C);
    send_data(0, 0x01);
    _delay_ms(2);
    send_data(0, 0x02);
    _delay_ms(2);
}

const char rpmstr[] PROGMEM = "RPM";
const char potstr[] PROGMEM = "Pot\005ncia";
const char ligstr[] PROGMEM = "Ligado";
const char offstr[] PROGMEM = "Deslig";
const char perstr[] PROGMEM = "\%";
void display_rpm(uint16_t rpm, uint8_t pwm, uint8_t state)
{
    send_lcd_num(rpm, 5, 0, 0, 7);
    send_lcd_cstr(rpmstr, 0, 13);

    send_lcd_cstr(potstr, 1, 0);
    send_lcd_num(5*(pwm-55), 4, 1, 1, 9);
    send_lcd_cstr(perstr, 1, 15);

    if(state&1)
    {
        send_lcd_cstr(ligstr, 0, 0);
    }
    else
    {
        send_lcd_cstr(offstr, 0, 0);
    }
}