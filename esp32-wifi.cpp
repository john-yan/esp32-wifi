#include "esp_log.h"
#include "esp32-wifi.h"
#include "esp_event_loop.h"
#include <string.h>

#define TAG "ESP32-WIFI"

#define CHECK_NOT_NULL(p) assert(p != NULL)
esp_err_t Wifi::default_wifi_handler(void *ctx, system_event_t *event) {

  Wifi* wifi = reinterpret_cast<Wifi*>(ctx);
  CHECK_NOT_NULL(wifi);

  wifi_call_back_info_t info = wifi->registry[event->event_id];

  if (info.handler) {
    return info.handler(event, info.ctx);
  }

  // no handler is provided
  switch(event->event_id) {
  case SYSTEM_EVENT_STA_START:
    esp_wifi_connect();
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    ESP_LOGI(TAG, "got ip:%s",
             ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED: {
    esp_wifi_connect();
    ESP_LOGI(TAG,"retry to connect to the AP");
    break;
  }
  default:
    break;
  }
  return ESP_OK;
}

void Wifi::init() {

  registry.resize(SYSTEM_EVENT_MAX);

  tcpip_adapter_init();

  // TODO: Wifi event handler
  ESP_ERROR_CHECK(esp_event_loop_init(default_wifi_handler, this));

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_config_t wifi_config = { 0 };
  strcpy(reinterpret_cast<char*>(wifi_config.sta.ssid), WIFI_SSID);
  strcpy(reinterpret_cast<char*>(wifi_config.sta.password), WIFI_PASSWORD);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_sta finished.");
  ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", WIFI_SSID, WIFI_PASSWORD);
}

Wifi::wifi_call_back_info_t Wifi::onEvent(system_event_id_t event_id, wifi_handler_t _handler, void *ctx) {
  assert(event_id >= SYSTEM_EVENT_WIFI_READY && event_id < SYSTEM_EVENT_MAX);
  wifi_call_back_info_t old_info = registry[event_id];
  registry[event_id] = {_handler, ctx};
  return old_info;
}

