#include "leds.h"

void config_lights()
{
    esp_rom_gpio_pad_select_gpio (RED_LIGHT_CHANNEL);
    esp_rom_gpio_pad_select_gpio (BLUE_LIGHT_CHANNEL);

    gpio_set_direction (RED_LIGHT_CHANNEL,GPIO_MODE_OUTPUT);
    gpio_set_direction (BLUE_LIGHT_CHANNEL,GPIO_MODE_OUTPUT);
}

void change_light_state(int red_state, int blue_state)
{
    gpio_set_level(RED_LIGHT_CHANNEL,red_state);
    gpio_set_level(BLUE_LIGHT_CHANNEL,blue_state);
}