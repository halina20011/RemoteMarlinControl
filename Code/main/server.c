#include "server.h"

httpd_handle_t startWebserver(){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if(httpd_start(&server, &config) == ESP_OK){
        httpd_register_uri_handler(server, &hello);
        return server;
    }

    return NULL;
}

static void connectHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData){
    httpd_handle_t  *server = arg;
    if(*server == NULL){
        *server = startWebserver();
    }
}

static void disconnectHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData){
    httpd_handle_t *server = arg;
    if(*server){
        httpd_stop(server);
        *server = NULL;
    }
}

esp_err_t helloGetHandler(httpd_req_t *req){
    // char *buf;
    // size_t bufSize;

    // bufSize = httpd_req_get_hdr_value_len(req, "Host") + 1;
    // if(1 < bufSize){
    //     buf = malloc(bufSize);
    //     if(httpd_req_get_hdr_value_str(req, "Host", buf, bufSize) == ESP_OK){
    //     }
    // }
    
    httpd_resp_set_hdr(req, "Content-Type", "text/html; charset=utf-8");

    const char *respStr = (const char*)req->user_ctx;
    httpd_resp_send(req, respStr, strlen(respStr));

    return ESP_OK;
}

void serverInit(){
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connectHandler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnectHandler, &server));

    server = startWebserver();
}
