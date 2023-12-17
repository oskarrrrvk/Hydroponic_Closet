#ifndef WATER_CURRENT_H
#define WATER_CURRENT_H

#include "driver/gpio.h"
#include "freertos/task.h"

#define GPIO_CHANNEL_WP    GPIO_NUM_25
#define GPIO_CHANNEL_EV_EN GPIO_NUM_26
#define GPIO_CHANNEL_EV_SW GPIO_NUM_27

void config_water_current_channel(void);
void enable_water_current(void);
void disable_water_current(void);
void manage_water_current_rutine(void);

#endif