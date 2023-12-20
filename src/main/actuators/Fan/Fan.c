#include "Fan.h"
int fan_state = 0;
void config_fan()
{
    esp_rom_gpio_pad_select_gpio (FAN_CHANNEL);
    gpio_set_direction (FAN_CHANNEL,GPIO_MODE_OUTPUT);

}

void change_fan_state()
{
    fan_state = !fan_state;
}

void set_fan_state()
{
    gpio_set_level(FAN_CHANNEL,fan_state);
}