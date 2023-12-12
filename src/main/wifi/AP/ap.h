#ifndef AP_H
#define AP_H

#include "wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"

const char * form = "";

void config_ap(void);
static void ap_wifi_handler();
esp_err_t get_handler(httpd_req_t *);
esp_err_t post_handler(httpd_req_t *);
httpd_handle_t start_website (void);
void stop_website(httpd_handle_t server);

#endif