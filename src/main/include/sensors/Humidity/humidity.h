#ifndef HUMIDITY_H
#define HUMIDITY_H

#include "../../Protocols/I2C_protocol.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define HUMIDITY_ADDRS 0xE5 //0b11100101
#define READING_NUMBER_HUMIDITY 2

float read_humidity(void);
void show_humidity_info(float);

#ifdef __cplusplus
}
#endif

#endif