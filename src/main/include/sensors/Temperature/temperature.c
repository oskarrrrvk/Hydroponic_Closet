#include "temperature.h"

float read_temperature()
{
    int temperature = receive_i2c_channel(TEMPERATURE_ADDRS,READING_NUMBER_TEMPERATURE);
    temperature &= 0x3FFF;
    return -46.85 + 175.72 * ((float)temperature/65536);
}

void show_temperature_info(float temperature_value)
{
    show_i2c_info("TEMPERATURE",temperature_value);
}