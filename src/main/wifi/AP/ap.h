#ifndef AP_H
#define AP_H

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

#define AP_SSID "SBC23G02"
#define AP_PSSWD "alpacino"
#define WIFI_CHANNEL 1
#define MAX_STA_CONN 4
#define MAXIMUN_RETRY 5

extern const char *html_ap_conf;
extern const char *TAG;

void config_ap(void);
void ap_wifi_handler(void*, esp_event_base_t, int32_t, void*);
esp_err_t get_handler(httpd_req_t *);
esp_err_t post_handler(httpd_req_t *);
httpd_handle_t start_website (void);
void stop_website(httpd_handle_t server);

#endif