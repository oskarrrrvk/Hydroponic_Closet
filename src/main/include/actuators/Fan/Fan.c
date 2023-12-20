#include "Fan.h"
int fan_state = 0;
void config_fan()
{
    esp_rom_gpio_pad_select_gpio (FAN_CHANNEL);
    gpio_set_direction (FAN_CHANNEL,GPIO_MODE_OUTPUT);
}

int change_fan_state(float temperature)
{
    if((temperature >= 35.0f && !fan_state) || (temperature < 35.0f && fan_state))
    {
        fan_state = !fan_state;
        return 1;
    }
    return 0;
}

void set_fan_state()
{
    gpio_set_level(FAN_CHANNEL,fan_state);
}