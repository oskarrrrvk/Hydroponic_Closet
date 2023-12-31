#ifndef WATER_FLOW_H
#define WATER_FLOW_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define GPIO_CHANNEL_WP    GPIO_NUM_25
#define GPIO_CHANNEL_EV_EN GPIO_NUM_26
#define GPIO_CHANNEL_EV_SW GPIO_NUM_27

//const gpio_num_t ports [] = {GPIO_CHANNE};

void config_water_flow_channel(void);
void enable_water_flow(void);
void disable_water_flow(void);
void manage_water_flow_rutine(void);

#ifdef __cplusplus
}
#endif

#endif