#ifndef TACH_H_FILE
#define TACH_H_FILE

#define TACH_CNT_MAX 65536
#define PER_TARGET 512000
#define PER_MUL_MAX 128
#define PER_MUL_MIN 4
#define OVF_MAX 2

#define TACH_DDR DDRD
#define TACH_PORT PORTD
#define TACH_PIN PIND
#define TACH_N PD6

void tach_setup();
void tach_ovf();
void tach_input(uint16_t);
uint16_t tach_rpm();

#endif