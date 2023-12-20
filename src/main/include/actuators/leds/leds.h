#ifndef LEDS_H
#define LEDS_H

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RED_LIGHT_CHANNEL  GPIO_NUM_32
#define BLUE_LIGHT_CHANNEL GPIO_NUM_33

void config_lights (void);

void change_light_state (int,int);

#ifdef __cplusplus
}
#endif

#endif