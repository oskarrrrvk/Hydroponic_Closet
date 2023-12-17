#ifndef LEDS_H
#define LEDS_H

#include "driver/gpio.h"

#define RED_LIGHT_CHANNEL  GPIO_NUM_32
#define BLUE_LIGHT_CHANNEL GPIO_NUM_33

void config_lights (void);
void change_lights_state (int,int);

#endif