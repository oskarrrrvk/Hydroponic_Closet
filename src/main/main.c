#include <stdio.h>


#include "wifi/wifi.h"

#include "Protocols/I2C_protocol.h"

#include "sensors/Humidity/humidity.h"
#include "sensors/Temperature/temperature.h"

#include "actuators/WaterFlow/water_flow.h"
#include "actuators/Leds/leds.h"
#include "actuators/Fan/Fan.h"

int counters_time = 0;
httpd_handle_t server;

void init (void);
void send_sensor_info (float, float);

void app_main(void)
{
    server =initialize_ap();
    if(server)
    {
        //wifi_sta_init(ssid,psswd);
        init();
        while(1)
        {
            vTaskDelay(2000/portTICK_PERIOD_MS);
        }
    }
    //stop_website(server);
}

void init(void)
{
    config_i2c_channel();
    config_water_flow_channel();
    config_lights();
    config_fan();
}

void send_sensor_info(float temperature, float humidity)
{


}