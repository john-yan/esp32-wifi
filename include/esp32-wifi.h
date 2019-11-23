
#ifndef WIFI_H
#define WIFI_H

#include "esp_system.h"
#include "esp_wifi.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <vector>

static constexpr char const* WIFI_SSID = CONFIG_WIFI_SSID;
static constexpr char const* WIFI_PASSWORD = CONFIG_WIFI_PASSWORD;

class Wifi {
  typedef esp_err_t (*wifi_handler_t)(system_event_t *event, void *ctx);
  typedef struct {
    wifi_handler_t handler;
    void* ctx;
  } wifi_call_back_info_t;

  typedef std::vector<wifi_call_back_info_t> wifi_event_registry_t;

  wifi_event_registry_t registry;
  static esp_err_t default_wifi_handler(void *ctx, system_event_t *event);

  public:
    void init();
    wifi_call_back_info_t onEvent(system_event_id_t event_id, wifi_handler_t _handler, void *ctx);
};

#endif // WIFI_H
