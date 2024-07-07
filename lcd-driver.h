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

extern uint32_t tach_per;
extern uint8_t tach_mul;
extern uint8_t tach_div;

void display_setup();
void display_clear();
void display_rpm();
void display_test(uint8_t, uint8_t);

#endif