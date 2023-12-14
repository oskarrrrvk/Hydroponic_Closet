#include "ap.h"

const char *html_ap_conf = "<!DOCTYPE html><html><head><title>Formulario de Configuracion Wi-Fi</title></head><body><h2>Configuracion Wi-Fi</h2><form action='/post_handler' method='post'><label for='ssid'>SSID:</label><input type='text' id='ssid' name='ssid' required><br><label for='password'>Password:</label><input type='password' id='password' name='password' required><br><input type='submit' value='Conectar'></form></body></html>";
const char *TAG = "wifi softAP";


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

    char ssid_value[32];
    char password_value[32];

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