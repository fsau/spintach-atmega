#ifndef PWMCTRL_H_FILE
#define PWMCTRL_H_FILE

#define PWM_DDR DDRD
#define PWM_PORT PORTD
#define PWM_PIN PIND
#define PWM_N PD5 // OC0B

void pwm_setup();
void pwm_set(uint8_t, uint8_t);

#endif