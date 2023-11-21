#include <stdio.h>
#include "WaterCurrent/water_current.h"

void init(void);

void app_main(void)
{
    init();
    manage_water_current_rutine();
}

void init(void)
{
    config_water_current_channel();
}