#ifndef WIFI
#define WIFI

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include <esp_http_server.h>
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

#define SSID        "TestWifi"
#define SSID_PASS   "12345678"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define MAX_RETRY_NUM 0

static EventGroupHandle_t wifiEventGroup;
static int retryNum = 0;

void wifiInit();

#endif
