#include "things_board.h"

const char *TAG_MQTT = "MQTT";
//Thingsboard
//Tokens
const char TOKEN1[] = "63fLqpJ1zow5jS4lfMoA";

// Datos servidor Thingsboard
const char THINGSBOARD_SERVER[] = "thingsboard.cloud";
const uint16_t THINGSBOARD_PORT = 1883U;
const uint16_t MAX_MESSAGE_SIZE = 128U;
const char TEMPERATURE_KEY[] = "temperature";
const char HUMIDITY_KEY[] = "humidity";


esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED");
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED");
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG_MQTT, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;

}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_app_start(char *key, float value)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = THINGSBOARD_SERVER,
        .broker.address.port = THINGSBOARD_PORT,
        .credentials.username = TOKEN1,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
    cJSON *root = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(root, key, value);
    char *post_data = cJSON_PrintUnformatted(root);
    esp_mqtt_client_publish(client, "v1/devices/me/telemetry", post_data, 0, 1, 0);
    
    cJSON_Delete(root);
    // Free is intentional, it's client responsibility to free the result of cJSON_Print
    free(post_data);
}