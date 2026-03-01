#include "app_framework.h"
#include "esp_log.h"

static const char *TAG = "app_framework";

void app_framework_init(void)
{
    ESP_LOGI(TAG, "framework init");
    app_health_start();
}
