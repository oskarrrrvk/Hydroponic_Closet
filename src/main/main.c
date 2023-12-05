#include <stdio.h>

#include "actuators/WaterFlow/water_flow.h"
#include "Protocols/I2C_protocol.h"
#include "sensors/Humidity/humidity.h"
#include "sensors/Temperature/temperature.h"

void init(void);

void app_main(void)
{
    float temperature,humidity;
    init();
    while(1)
    {
        //manage_water_current_rutine();
        temperature = read_temperature();
        show_temperature_info(temperature);
        humidity = read_humidity();
        show_humidity_info(humidity);
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
}

void init(void)
{
    //config_water_current_channel();
    config_i2c_channel();
}