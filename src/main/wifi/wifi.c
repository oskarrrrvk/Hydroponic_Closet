#include "wifi.h"

char ssid_value[32];
char password_value[32];

const char *html_ap_conf = "<!DOCTYPE html><html><head><title>Formulario de Configuracion Wi-Fi</title></head><body><h2>Configuracion Wi-Fi</h2><form action='/post_handler' method='post'><label for='ssid'>SSID:</label><input type='text' id='ssid' name='ssid' required><br><label for='password'>Password:</label><input type='password' id='password' name='password' required><br><input type='submit' value='Conectar'></form></body></html>";
const char *TAG = "wifi";


httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

/* URI handler structure for POST /uri */
httpd_uri_t uri_post = {
    .uri      = "/post_handler",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};

httpd_handle_t initialize_ap()
{
    esp_err_t ret = nvs_flash_init();
    httpd_handle_t server;
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    config_ap();
    
    server = start_website();
    return server;
}

void ap_wifi_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void config_ap (void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ap_wifi_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = WIFI_CHANNEL,
            .password = AP_PSSWD,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg ={
                .required = 1,
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             AP_SSID, AP_PSSWD, WIFI_CHANNEL);
}



esp_err_t get_handler (httpd_req_t *req) 
{
    esp_err_t err = httpd_resp_send(req,html_ap_conf,HTTPD_RESP_USE_STRLEN);
    return err;
}

esp_err_t post_handler (httpd_req_t * req)
{
    // Tamaño máximo permitido para datos del formulario
    size_t max_len = 100000;
    char *buff = (char *)malloc(max_len);
    if (buff == NULL) {
        // Manejar error de asignación de memoria
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    size_t size = req->content_len;
    if (size > max_len) {
        // Datos del formulario demasiado grandes
        free(buff);
        httpd_resp_send(req,"ERROR 413",10);  // Enviar respuesta de carga demasiado grande
        return ESP_FAIL;
    }

    int ret = httpd_req_recv(req, buff, size);
    buff[ret] ='\0';
    if (ret <= 0) {
        // Manejar errores durante la recepción de datos
        free(buff);
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);  // Tiempo de espera agotado
        }
        return ESP_FAIL;
    }

    char *ssid_start = strstr(buff,"ssid=");
    char *password_start = strstr(buff, "password=");

    if(!ssid_start || !password_start) 
    {
        free(buff);
        return ESP_FAIL;
    }

    ssid_start += 5;
    password_start += 9; // El valor de "password=" tiene 8 caracteres.
    char *ssid_end = strchr(ssid_start,'&');
    char *password_end = strchr(password_start, '\0');// no me encuentra el caracter

    if(!ssid_end || !password_end)
    {
        free(buff);
        return ESP_FAIL;
    }

    memset(ssid_value,0,sizeof(ssid_value));
    strncpy(ssid_value,ssid_start,ssid_end-ssid_start);
    memset(password_value, 0, sizeof(password_value));
    strncpy(password_value, password_start, password_end - password_start);

    ESP_LOGI("SSID","%s",ssid_value);
    ESP_LOGI("Password", "%s", password_value);
    free(buff);
    char *next_page = "Formulario registrado correctamente<br>";
    httpd_resp_send(req,next_page,strlen(next_page));
    return ESP_OK;
}

httpd_handle_t start_website (void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.recv_wait_timeout = 20;
    config.send_wait_timeout = 20;

    httpd_handle_t server = NULL;
    if (httpd_start(&server,&config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    return server;
}

void stop_website(httpd_handle_t server)
{
    if(server)
        httpd_stop(server);
}

static EventGroupHandle_t s_wifi_event_group;


static int s_retry_num = 0;
static char *wifi_ok;


static void wifi_sta_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            if(*wifi_ok)
                esp_restart();
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta()
{
    s_wifi_event_group = xEventGroupCreate();

    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_sta_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_sta_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    wifi_config_t wifi_config = {
        .sta = {
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = WIFI_AUTH_OPEN,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };
    
    strcpy((char*)wifi_config.sta.ssid, ssid_value);
    strcpy((char*)wifi_config.sta.password, password_value);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        *wifi_ok = 1;
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ssid_value, password_value);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ssid_value, password_value);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}
