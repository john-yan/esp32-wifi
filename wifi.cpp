#include "esp_log.h"
#include "wifi.h"
#include "constants.h"
#include "utils.h"
#include <string.h>

#define TAG "WifiSta"

esp_err_t WifiSta::defaultWifiHandler(void *ctx, system_event_t *event) {

  WifiSta* wifi = reinterpret_cast<WifiSta*>(ctx);
  CHECK_NOT_NULL(wifi);

  wifi_call_back_info_t info = wifi->registry[event->event_id];

  if (info.handler) {
    return info.handler(info.ctx, event);
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

void WifiSta::Init() {

  registry.reserve(SYSTEM_EVENT_MAX);

  tcpip_adapter_init();

  // TODO: Wifi event handler
  ESP_ERROR_CHECK(esp_event_loop_init(defaultWifiHandler, this) );

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

void WifiSta::onEvent(system_event_id_t event_id, wifiHandler _handler, void *ctx) {
  wifi_call_back_info_t info = {_handler, ctx};
  registry[event_id] = info;
}

