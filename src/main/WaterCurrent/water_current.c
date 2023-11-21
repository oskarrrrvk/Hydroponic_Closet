#include "water_current.h"


/**
 * Config al the ports to control the water flow
*/
void config_water_current_channel (void)
{
    esp_rom_gpio_pad_select_gpio (GPIO_CHANNEL_WP);
    esp_rom_gpio_pad_select_gpio (GPIO_CHANNEL_EV_EN);
    esp_rom_gpio_pad_select_gpio (GPIO_CHANNEL_EV_SW);

    gpio_set_direction (GPIO_CHANNEL_WP,GPIO_MODE_OUTPUT);
    gpio_set_direction (GPIO_CHANNEL_EV_EN,GPIO_MODE_OUTPUT);
    gpio_set_direction (GPIO_CHANNEL_EV_SW,GPIO_MODE_OUTPUT);
}

/**
 * Enable the water flow 
*/
void enable_water_current (void)
{
    gpio_set_level (GPIO_CHANNEL_EV_EN,1);
    gpio_set_level (GPIO_CHANNEL_EV_SW,0);
    vTaskDelay(5000); // pendiente de calculo
    gpio_set_level(GPIO_CHANNEL_WP,1);
}

/**
 * Disable the water flow 
*/
void disable_water_current (void)
{
    gpio_set_level(GPIO_CHANNEL_WP,0);
    vTaskDelay(5000); // pendiente de calculo apertura de válvula
    gpio_set_level (GPIO_CHANNEL_EV_EN,1);
    gpio_set_level (GPIO_CHANNEL_EV_SW,1);
}

/**
 * control the water flow.
 * Firstly the valve and the water pump  are opened.
 * wait n second for the water to flow.
 * And lastly the valve and the water pump are closed.
*/
void manage_water_current_rutine(void)
{
    enable_water_current();
    gpio_set_level (GPIO_CHANNEL_EV_EN,0);
    vTaskDelay(5000); // pendiente de calculo ciclo de agua
    disable_water_current();
    gpio_set_level (GPIO_CHANNEL_EV_EN,0);
}