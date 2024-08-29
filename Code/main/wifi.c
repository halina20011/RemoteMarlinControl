#include "wifi.h"

#include "driver/gpio.h"

static void event_handler(void* arg, esp_event_base_t event_base, int32_t eventId, void *eventData);

static void eventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData){
    if(eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START){
        esp_wifi_connect();
    }
    else if(eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED){
        if(retryNum < MAX_RETRY_NUM){
            esp_wifi_connect();
            retryNum++;
        }
        else{
            xEventGroupSetBits(wifiEventGroup, WIFI_FAIL_BIT);
        }
    }
    else if(eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP){
        // gpio_set_level(GPIO_NUM_0, 1);
        retryNum = 0;
        xEventGroupSetBits(wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}

#define RED     GPIO_NUM_2
#define GREEN   GPIO_NUM_0
#define BLUE    GPIO_NUM_3
#define STATUS_LED  GPIO_NUM_1

void wifiInit(){
    gpio_set_level(BLUE, 1);
    vTaskDelay(1000 / portTICK_RATE_MS);
    gpio_set_level(BLUE, 0);

    wifiEventGroup = xEventGroupCreate();
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    wifi_init_config_t wic = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wic));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, NULL));

    wifi_config_t wifiConfig = {
        .sta = {
            .ssid = SSID,
            .password = SSID_PASS
        }
    };

    if(strlen((char *)wifiConfig.sta.password)){
        wifiConfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(wifiEventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    if(bits & WIFI_CONNECTED_BIT){
        gpio_set_level(STATUS_LED, 1);
    }
    else if(bits & WIFI_FAIL_BIT){
        gpio_set_level(RED, 1);
    }
    else{
        gpio_set_level(BLUE, 1);
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler));
    vEventGroupDelete(wifiEventGroup);
}
