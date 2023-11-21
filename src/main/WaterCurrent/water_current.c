#include "water_current.h"

void config_water_current_channel(void)
{
    esp_rom_gpio_pad_select_gpio(GPIO_CHANNEL);
    gpio_set_direction(GPIO_CHANNEL,GPIO_MODE_OUTPUT);
}

void switch_water_current_enable(void)
{

}

void enable_water_current(void)
{
    gpio_set_level(GPIO_CHANNEL,enable_current);
}