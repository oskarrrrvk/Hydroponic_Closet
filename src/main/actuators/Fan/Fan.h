#ifndef FAN_H
#define FAN_H

#include "driver/gpio.h"

#define FAN_CHANNEL GPIO_NUM_14
int fan_state = 0;

void config_fan (void);
void change_fan_state (void);
void set_fan_state(void);

#endif