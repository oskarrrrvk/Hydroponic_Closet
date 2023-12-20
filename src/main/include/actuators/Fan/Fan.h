#ifndef FAN_H
#define FAN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "driver/gpio.h"

#define FAN_CHANNEL GPIO_NUM_14
extern int fan_state;

void config_fan (void);
int change_fan_state (float);
void set_fan_state(void);

#ifdef __cplusplus
}
#endif
#endif