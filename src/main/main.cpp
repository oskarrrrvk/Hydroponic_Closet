#include <esp_netif.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_random.h>
#include <stdio.h>
//Protocolos
#include "Protocols/I2C_protocol.h"
//Actuadores
//#include "actuators/Fan/Fan.h"
//#include "actuators/leds/leds.h"
//#include "actuators/WaterFlow/water_flow.h"
//Sensores
#include "sensors/Humidity/humidity.h"
#include "sensors/Temperature/temperature.h"
//Thingsboard
#include "thingsboard/Espressif_MQTT_Client.h"
#include "thingsboard/ThingsBoard.h"

#include "wifi/wifi_def.h"

//probando I2C
#include "protocols/I2C_protocol.h"
#include <math.h>
#include "driver/i2c.h"

#define TEMPERATURE_ADDRS 0xE3 //0b11100011
#define READING_NUMBER_TEMPERATURE 2
#define HUMIDITY_ADDRS 0xE5 //0b11100101
#define READING_NUMBER_HUMIDITY 2
#define I2C_MASTER_FREQ_HZ 400000


//Thingsboard
//Tokens
constexpr char TOKEN1[] = "63fLqpJ1zow5jS4lfMoA";

// Datos servidor Thingsboard
constexpr char THINGSBOARD_SERVER[] = "thingsboard.cloud";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint16_t MAX_MESSAGE_SIZE = 128U;
constexpr char TEMPERATURE_KEY[] = "temperature";
constexpr char HUMIDITY_KEY[] = "humidity";

// Inicializa MQTT
struct Espressif_MQTT_Client mqttClient;
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

//funciones server
esp_err_t get_handler(httpd_req_t*);
esp_err_t post_handler(httpd_req_t*);
httpd_handle_t start_website(void);
void stop_website(httpd_handle_t server);





static const char *TAG = "SBC_GRUPO02_main";

static char wifi_ok = 0;
static float temperatura = 0;
static float humedad = 0;
static int credentials_recived=0;
const char *html = "<!DOCTYPE html><html><head><title>Formulario de Configuracion Wi-Fi</title></head><body><h2>Configuracion Wi-Fi</h2><form action='/post_handler' method='post'><label for='ssid'>SSID:</label><input type='text' id='ssid' name='ssid' required><br><label for='password'>Password:</label><input type='password' id='password' name='password' required><br><input type='submit' value='Conectar'></form></body></html";

char new_ssid[32] = "";
char new_password[64] = "";

//server
httpd_uri_t uri_get = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = get_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_post = {
    .uri      = "/post_handler",
    .method   = HTTP_POST,
    .handler  = post_handler,
    .user_ctx = NULL
};
//-----------------------------------------------------------------------------------------------------------------------------
extern "C"
void app_main() {
    esp_err_t ret;
    httpd_handle_t server;
    
    /*char w_ssid[35] = {'\0'};//Variables para almacenar respuesta html
    char w_pass[65] = {'\0'};*/
    //char *w_ssid1 = "";
    //char *w_pass1 = "";

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    set_wifi_ok_addr(&wifi_ok);//no hace falta
    
    void config_i2c_channel (void);


    while(1){

        if(credentials_recived)
        wifi_init_sta(new_ssid,new_password);

        if(wifi_ok) {

            for(;;usleep(1000000)) {
                temperatura = 90;
                temperatura = read_temperature();
                humedad = 20;
                humedad = read_humidity();
                ESP_LOGI(TAG, "la temperatura marca %f",temperatura);
                ESP_LOGI(TAG, "la luz marca %f",humedad);

                if (!tb.connected()){
                    tb.connect(THINGSBOARD_SERVER, TOKEN1, THINGSBOARD_PORT);
                }

                tb.sendTelemetryData(TEMPERATURE_KEY, temperatura);
                tb.sendTelemetryData("humidity",humedad);


                usleep(10000);
                tb.loop();
            }
            
        } else {
            wifi_init_softap();
            server = start_website();
            while(!credentials_recived){
                usleep(10000000);
            }
            stop_website(server);
        }
    }
}
//----------------------------------------------------------------------------------------------------


esp_err_t get_handler(httpd_req_t *req) 
{
    esp_err_t err = httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return err;
}

esp_err_t post_handler(httpd_req_t * req)
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
        httpd_resp_send(req, "ERROR 413", 10);  // Enviar respuesta de carga demasiado grande
        return ESP_FAIL;
    }

    int ret = httpd_req_recv(req, buff, size);
    ESP_LOGI("trama", "%s", buff);
    buff[ret] = '\0';
    if (ret <= 0) {
        // Manejar errores durante la recepción de datos
        free(buff);
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);  // Tiempo de espera agotado
        }
        return ESP_FAIL;
    }

    char *resp = "Formulario procesado adecuadamente";
    char *ssid_start = strstr(buff, "ssid=");
    char *password_start = strstr(buff, "password=");

    if (!ssid_start || !password_start) 
    {
        free(buff);
        return ESP_FAIL;
    }

    ssid_start += 5;
    password_start += 9; // El valor de "password=" tiene 8 caracteres.
    char *ssid_end = strchr(ssid_start, '&');
    char *password_end = strchr(password_start, '\0');

    if (!ssid_end || !password_end)
    {
        free(buff);
        ESP_LOGI("ERROR", "problem with formulary");
        return ESP_FAIL;
    }   
    memset(new_ssid, 0, sizeof(new_ssid));
    strncpy(new_ssid, ssid_start, ssid_end - ssid_start);
    memset(new_password, 0, sizeof(new_password));
    strncpy(new_password, password_start, password_end - password_start);
    ESP_LOGI("HTTP server", "SSID: %s", new_ssid);
    ESP_LOGI("HTTP server", "Contraseña: %s", new_password);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    free(buff);
    credentials_recived = 1;  // Se han recibido las credenciales
    return ESP_OK;
}

httpd_handle_t start_website()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
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

