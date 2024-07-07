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

#ifndef LCD_DRIVER_H_FILE
#define LCD_DRIVER_H_FILE

#define LCD_DDR DDRB
#define LCD_PIN PINB
#define LCD_PORT PORTB
#define LCD_D7_N PB0
#define LCD_D6_N PB1
#define LCD_D5_N PB2
#define LCD_D4_N PB3
#define LCD_E_N PB4
#define LCD_RW_N PB5
#define LCD_RS_N PB6

#define SIZE_COL 2
#define SIZE_LINE 16
#define STROBE_LEN_US 40

void display_setup();
void display_clear();
void display_rpm(uint16_t,uint8_t,uint8_t);
void display_test(uint8_t, uint8_t);

#endif