#include "app_framework.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

static const char *TAG = "app_health";

static void health_task(void *arg)
{
    (void)arg;
    while (1) {
        app_health_print_once();
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_health_start(void)
{
    xTaskCreate(health_task, "health", 3072, NULL, 1, NULL);
}

void app_health_print_once(void)
{
    size_t free8 = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t min8  = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "heap free=%u, min_free=%u", (unsigned)free8, (unsigned)min8);
}
