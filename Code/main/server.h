#ifndef SERVER
#define SERVER

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include <esp_http_server.h>
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

static const httpd_uri_t indexUri = {
    .uri = "/",
    .method = HTTP_GET,
};

esp_err_t helloGetHandler(httpd_req_t *req);

static const httpd_uri_t hello = {
    .uri = "/hello",
    .method = HTTP_GET,
    .handler = helloGetHandler,
    .user_ctx = "<p>Haiii</p>"
};

static httpd_handle_t server = NULL;

void serverInit();

#endif
