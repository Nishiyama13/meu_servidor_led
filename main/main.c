#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#define SSID "ESP32-AP-NISHI"
#define PASSWORD "12345678"
#define TAG "WiFi-AP"

const char html_page[] =
    "<!DOCTYPE html><html>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<title>Controle LED</title>"
    "<style>"
    "  body {"
    "    font-family: Arial, sans-serif;"
    "    text-align: center;"
    "    background-color: #f0f0f0;"
    "    margin-top: 50px;"
    "  }"
    "  h1 {"
    "    color: #333;"
    "  }"
    "  button {"
    "    padding: 15px 30px;"
    "    font-size: 18px;"
    "    background-color: #4CAF50;"
    "    color: white;"
    "    border: none;"
    "    border-radius: 8px;"
    "    cursor: pointer;"
    "    transition: background-color 0.3s;"
    "  }"
    "  button:hover {"
    "    background-color: #45a049;"
    "  }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>Controle do LED</h1>"
    "<button onclick=\"toggleLED()\">Alternar LED</button>"
    "<script>"
    "let estado = false;"
    "function toggleLED() {"
    "  estado = !estado;"
    "  fetch('/led?state=' + (estado ? 'on' : 'off'))"
    "    .then(response => response.text())"
    "    .then(data => console.log(data));"
    "}"
    "</script>"
    "</body></html>";

void nvs_init();
void event_loop_init();
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
void wifi_init();
httpd_handle_t http_server_init(void);
esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t led_get_handler(httpd_req_t *req);
void gpio_init();

void app_main()
{
    nvs_init();
    event_loop_init();
    wifi_init();
    http_server_init();
};

void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
};

void event_loop_init()
{
    gpio_init();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &wifi_event_handler, NULL, NULL);
};

void wifi_init()
{
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_cfg);
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_config = {
        .ap = {
            .ssid = SSID,
            .ssid_len = strlen(SSID),
            .channel = 1,
            .password = PASSWORD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    if (strlen(PASSWORD) == 0)
    {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
};

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_AP_START:
            ESP_LOGW(TAG, "Access Point iniciado. SSID:%s senha:%s", SSID, PASSWORD);
            break;

        case WIFI_EVENT_AP_STACONNECTED:
        {
            wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
            ESP_LOGW(TAG, "Estacao conectada! MAC: " MACSTR ", AID: %d",
                     MAC2STR(event->mac), (int)event->aid);
            break;
        }

        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
            ESP_LOGW(TAG, "Estacao desconectada! MAC: " MACSTR ", AID: %d",
                     MAC2STR(event->mac), (int)event->aid);
            break;
        }

        default:
        {
            ESP_LOGW(TAG, "Evento WiFi nao tratado: %d", (int)event_id);
            break;
        }
        }
    }
};

httpd_handle_t http_server_init(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    esp_err_t err = httpd_start(&server, &config);

    if (err == ESP_OK)
    {
        httpd_uri_t uri_root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL};
        httpd_uri_t uri_led = {
            .uri = "/led",
            .method = HTTP_GET,
            .handler = led_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_led);
    }
    return server;
};

esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
};

esp_err_t led_get_handler(httpd_req_t *req)
{
    char query[32];
    char param[8];
    esp_err_t err = httpd_req_get_url_query_str(req, query, sizeof(query));

    if (err == ESP_OK)
    {
        err = httpd_query_key_value(query, "state", param, sizeof(param));
        if (err == ESP_OK)
        {
            if (strcmp(param, "on") == 0)
            {
                gpio_set_level(GPIO_NUM_8, 1);
                ESP_LOGI("LED", "Ligado");
            }
            else if (strcmp(param, "off") == 0)
            {
                gpio_set_level(GPIO_NUM_8, 0);
                ESP_LOGI("LED", "Desligado");
            }
        }
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
};

void gpio_init()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << GPIO_NUM_8,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
};