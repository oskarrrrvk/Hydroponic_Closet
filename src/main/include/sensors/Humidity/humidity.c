#include "humidity.h"

float read_humidity()
{
    int humidity = receive_i2c_channel(HUMIDITY_ADDRS,READING_NUMBER_HUMIDITY);
    humidity &= 0x0FFF;
    return -6 + 125 * ((float)humidity / 65536);
}
void show_humidity_info(float humidity_value)
{
    show_i2c_info("HUMIDITY",humidity_value);
}