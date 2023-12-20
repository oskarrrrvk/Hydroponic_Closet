#ifndef THINGS_BOARD_H
#define THINGS_BOARD_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

extern const char *TAG_MQTT; 

extern const char THINGSBOARD_SERVER[];
extern const uint16_t THINGSBOARD_PORT;
extern const uint16_t MAX_MESSAGE_SIZE;
extern const char TEMPERATURE_KEY[];
extern const char HUMIDITY_KEY[];

esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t);
void mqtt_event_handler(void *, esp_event_base_t,int,void *);
void mqtt_app_start(char *, float);


#endif