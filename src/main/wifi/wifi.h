#ifndef WIFI_H
#define WIFI_H

#include "esp_wifi.h"
#include "nvs_flash.h"

void config_wifi(void);
static void wifi_event_handler();

#endif