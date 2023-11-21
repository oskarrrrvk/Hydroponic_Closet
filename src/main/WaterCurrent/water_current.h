#ifndef WATER_CURRENT_H
#define WATER_CURRENT_H

#include "driver/gpio.h"

#define GPIO_CHANNEL GPIO_NUM_11

int enable_current = 0;

void config_water_current_channel(void);
void switch_water_current_enable(void);
void enable_water_current(void);

#endif