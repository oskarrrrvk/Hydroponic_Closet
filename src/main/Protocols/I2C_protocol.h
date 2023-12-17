#ifndef I2C_PROTOCOL_H
#define I2C_PROTOCOL_H

#include <math.h>
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_FREQ_HZ 400000

void config_i2c_channel (void);
int receive_i2c_channel (uint8_t, int);
void show_i2c_info (char *, float);

#endif