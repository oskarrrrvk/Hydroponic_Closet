#include <esp_netif.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_random.h>
#include <stdio.h>
#include "blue/bt_main.h"
//Protocolos
#include "Protocols/I2C_protocol.h"
//Actuadores
#include "actuators/Fan/Fan.h"
#include "actuators/leds/leds.h"
#include "actuators/WaterFlow/water_flow.h"
//Sensores
#include "sensors/Humidity/humidity.h"
#include "sensors/Temperature/temperature.h"
//Thingsboard
#include "thingsboard/Espressif_MQTT_Client.h"
#include "thingsboard/ThingsBoard.h"

#include "contador/contador.h"

#include "wifi/wifi_def.h"
#include "esp_sleep.h"

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

static const char *TAG = "SBC_GRUPO02_main";

int flag_leds = 1, flag_bomba = 0, cont_leds = 0;
double time_ant, time_bomba;
int flag_modem, time_off_modem, time_on_modem;

static char wifi_ok = 0;
static float temperatura = 0;
static float humedad = 0;
static int credentials_recived=0;

//bluetooth
FILE *t;
    struct stat st;
    const char *wifi_conf= "/data/wifi.config";

    unsigned char content[1024] = {'\0'};
    unsigned char fi[128] = {'\0'};
    
    char ssid[70] = {'\0'};
    char pass[70] = {'\0'};
    int ok = 0;
//

void comprobar_rutinas(void);

extern "C"
void app_main() {
    esp_err_t ret;
    time_off_modem = get_time();

    config_water_flow_channel();
    //config_lights();
    config_i2c_channel();
    config_fan();

    set_dir(fi, content);
    
    char *w_ssid1 = "ssid";
    char *w_pass1 = "pass";

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    set_wifi_ok_addr(&wifi_ok);//no hace falta
    
    while(1){
        if(credentials_recived)
            wifi_init_sta(ssid,pass);

    if(wifi_ok) {

        for(;;usleep(1000000)) {

            

            temperatura = read_temperature();
            humedad = read_humidity();
            ESP_LOGI(TAG, "la temperatura marca %f",temperatura);
            ESP_LOGI(TAG, "la humedad marca %f",humedad);

            if (!tb.connected()){
                tb.connect(THINGSBOARD_SERVER, TOKEN1, THINGSBOARD_PORT);
            }

            tb.sendTelemetryData(TEMPERATURE_KEY, temperatura);
            tb.sendTelemetryData("humidity",humedad);


            usleep(10000);
            
            //tb.loop();
            comprobar_rutinas();
        }
        
    } else {

        if(!credentials_recived){
            init_bt();
            if (stat(wifi_conf, &st) == -1) 
            {
            ESP_LOGI(TAG, "ERROR AL CARGAR EL FICHERO CONFIG, wifi.config no existe en la BDD");
            ESP_LOGI(TAG, "INTENTANDO BUSCAR FICHERO wifi.config");
            int salir = 1;
            while(salir) 
            {
                if(!strcmp((char*)fi, "wifi.config") && sscanf((char*)content, "SSID: %s PASS: %s", ssid, pass) == 2) 
                {
                    ESP_LOGI(TAG, "contenido del wifi.config: %s\n", content);
                    ESP_LOGI(TAG, "configuracion actualizada EXITOSAMENTE!!!");
                    credentials_recived=1;
                    esp_bt_controller_disable();
                    vTaskDelay(100);
                    salir = 0;
                }
                *fi = *content = '\0';
                vTaskDelay(200);
            }
            } 
            else if (!S_ISDIR(st.st_mode)) 
            {
                t = fopen(wifi_conf, "r");
                ok = fscanf(t, "SSID: %s PASS: %s", ssid, pass) == 2;
                fclose(t);

                if(ok != 2)
                    ESP_LOGI(TAG, "ERROR CON EL FORMATO DEL FICHERO DE CONFIGURACION");
                else
                {
                    ESP_LOGI(TAG, "CONFIGURACION ACTUALIZADA EXITOSAMENTE");
                    printf("wifi.config:  ->%s<- (SSID) ->%s<- (PASS)\n", ssid, pass);
                    w_ssid1 = strdup(ssid);
                    w_pass1 = strdup(pass);
            
                    credentials_recived=1;
                }
            }
        }
    }
    }
}

void comprobar_rutinas()
{
     if(flag_bomba)
    {
        if(duration(time_bomba) > 30) // Han pasado 30 segundos
        {
            ESP_LOGI(TAG, "\n   Bomba de agua: OFF");
            disable_water_flow();
            flag_bomba = 0;
            time_ant = get_time();     
        }
    }

    if(!flag_bomba) // Ha pasado 1 minuto
    {
        if(duration(time_ant) > 60)
        {
            ESP_LOGI(TAG, "\n   Bomba de agua: ON");
            enable_water_flow();
            flag_bomba = 1;
            time_bomba = get_time();
            flag_leds = !flag_leds;

            if(flag_leds)
            {
                ESP_LOGI(TAG, "\n   Es de noche LEDs: ON");
                //change_lights_state(1,1);
            }
            else
            {
                ESP_LOGI(TAG, "\n   Es de dia LEDs: OFF");
                //change_lights_state(0,0);
            }
        }

    if(flag_modem)
    {
        if(duration(time_on_modem) > 15)
        {
            ESP_LOGI(TAG, "\n   Bluetooth y Wifi: OFF");
            esp_sleep_disable_wifi_wakeup();
            flag_modem = 0;
            time_off_modem = get_time();
        }
    }
    if(!flag_modem)
    {
        if(duration(time_off_modem)> 60)
        {
            ESP_LOGI(TAG, "\n   Bluetooth y Wifi: ON");
            esp_sleep_enable_wifi_wakeup();
            flag_modem = 1;
            time_on_modem = get_time();
        }
    }

    }
}