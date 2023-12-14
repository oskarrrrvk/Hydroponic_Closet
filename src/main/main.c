#include <stdio.h>

#include "Protocols/I2C_protocol.h"

#include "wifi/AP/ap.h"

#include "sensors/Humidity/humidity.h"
#include "sensors/Temperature/temperature.h"

#include "actuators/WaterFlow/water_flow.h"
#include "actuators/Leds/leds.h"
#include "actuators/Fan/Fan.h"

void init (void);
void show_sensor_info (void);

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    httpd_handle_t server;
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    config_ap();
    
    server = start_website();
    
    while(1)
    {
       vTaskDelay(2000/portTICK_PERIOD_MS);
    }
    stop_website(server);
}

void init(void)
{
    config_water_current_channel();
    config_i2c_channel();
}