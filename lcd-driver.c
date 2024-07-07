#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd-driver.h"

void strobe_e_line()
{
    LCD_PORT &= ~_BV(LCD_E_N);
    _delay_us(STROBE_LEN_US);
    LCD_PORT |= _BV(LCD_E_N);
    _delay_us(STROBE_LEN_US);
    LCD_PORT &= ~_BV(LCD_E_N);
}

void send_data(uint8_t rs, uint8_t data)
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

void set_pos(uint8_t line, uint8_t col)
{
    int8_t offset = 0;
    if (line)
    {
        offset = 64;
    }
    send_data(0, 0x80 + offset + col);
}

void send_lcd_num(uint32_t num, uint8_t slen, uint8_t div, uint8_t line, uint8_t col)
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

void send_lcd_nn(uint8_t num, uint8_t line, uint8_t col)
{
    set_pos(line, col);
    send_data(1, num / 10 + '0');
    send_data(1, num % 10 + '0');
}

void send_lcd_cstr(const char str[], uint8_t line, uint8_t col)
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

void display_erase(uint8_t len, uint8_t line, uint8_t col)
{
    set_pos(line, col);
    for (uint8_t i = len; i; --i)
    {
        send_data(1, ' ');
    }
}

const char helloworld[] PROGMEM = "Ol\002 mundo!";
const char but[] PROGMEM = "Bot\001o";
void display_test(uint8_t sw1, uint8_t sw2)
{
    send_lcd_cstr(helloworld, 0, 0);
    send_lcd_num(tach_mul, 3, 0, 0, 13);
    send_lcd_num(tach_per, 10, 0, 1, 6);

    if (sw1 || sw2)
    {
        send_lcd_cstr(but, 1, 0);
        send_lcd_nn((sw1 ? 1 : 0) | (sw2 ? 10 : 0), 1, 7);
    }
    else
    {
        display_erase(9, 1, 0);
    }
}

/*
clock = clk_io/8 = 4.096 MHz / 8 = 512 kHz
unit = 1/clock = 1.953125 µs
max_period = 2^16 * unit = 0.128 s
RPM = 60*f_in/2 = 30/(unit * cnt) = 15360000 / cnt
*/

const char rpm[] PROGMEM = "RPM";
void display_rpm()
{
    uint16_t r;
    if(tach_per > 50)
    {
        r = (uint32_t) 15360000 * tach_mul / tach_per;
    }
    else
    {
        r = 0;
    }
    send_lcd_num(r, 5, 0, 1, 7);
    send_lcd_cstr(rpm, 1, 13);
}