#ifndef STA_H
#define STA_H

#include "wifi.h"

void config_sta(void);
static void sta_wifi_handler();
void connect_wifi_station(char *, char *);


#endif