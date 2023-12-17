#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../../Protocols/I2C_protocol.h"

#define TEMPERATURE_ADDRS 0xE3 //0b11100011
#define READING_NUMBER_TEMPERATURE 2

float read_temperature(void);
void show_temperature_info(float);

#ifdef __cplusplus
}
#endif

#endif